// MockMain.cpp
// Fully automatic PCL interface export tool
// NO factory functions needed - discovers everything via symbol scanning!

#include <QApplication>
#include <QThread>
#include <QDebug>
#include <dlfcn.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#ifdef __linux__
#include <unistd.h>
#include <limits.h>
#endif

#include <pcl/MetaModule.h>
#include <pcl/api/APIInterface.h>
#include <pcl/MetaProcess.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ProcessImplementation.h>

#include "PCLInterfaceScanner.h"
#include "PCLMockAPI.h"
#include "ExportHelper.h"
#include "RootWidgetSelector.h"

// ============================================================================
// Symbol Resolution Utilities
// ============================================================================

#include <cxxabi.h>
#include <string>
#include <regex>

static std::string demangle(const char* mangled, bool includeParams = true)
{
    if (!mangled)
        return std::string();
    
    int status = 0;
    char* dem = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
    
    if (status != 0 || !dem)
        return std::string(mangled);
    
    std::string result(dem);
    std::free(dem);
    
    if (!includeParams)
    {
        // Strip parameters - keep only up to first '('
        size_t parenPos = result.find('(');
        if (parenPos != std::string::npos)
            result = result.substr(0, parenPos);
    }
    
    return result;
}

// Enhanced version with parameter demangling and cleanup
static std::string demangleVerbose(const char* mangled)
{
    if (!mangled)
        return std::string();
    
    int status = 0;
    char* dem = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
    
    if (status != 0 || !dem)
        return std::string(mangled);
    
    std::string result(dem);
    std::free(dem);
    
    // Clean up common cruft for readability
    
    // Replace "std::__1::" with "std::" (libc++ implementation detail)
    size_t pos = 0;
    while ((pos = result.find("std::__1::", pos)) != std::string::npos)
    {
        result.replace(pos, 10, "std::");
        pos += 5;
    }
    
    // Replace "std::basic_string<char, ...>" with "std::string"
    std::regex string_regex(R"(std::basic_string<char, std::char_traits<char>, std::allocator<char>\s*>)");
    result = std::regex_replace(result, string_regex, "std::string");
    
    // Replace "unsigned long" with "size_t" where appropriate
    std::regex ulong_regex(R"(\bunsigned long\b)");
    result = std::regex_replace(result, ulong_regex, "size_t");
    
    return result;
}

// Extract just the class name from a fully qualified function signature
static std::string extractClassName(const std::string& demangled)
{
    // For "pcl::SandboxInterface::SandboxInterface()" -> "pcl::SandboxInterface"
    
    size_t parenPos = demangled.find('(');
    if (parenPos == std::string::npos)
        return demangled; // No parameters, return as-is
    
    // Find the last "::" before the '('
    std::string beforeParen = demangled.substr(0, parenPos);
    size_t lastColon = beforeParen.rfind("::");
    
    if (lastColon == std::string::npos)
        return beforeParen; // No namespace
    
    return beforeParen.substr(0, lastColon);
}

// Extract just the method/function name (no namespace, no parameters)
static std::string extractMethodName(const std::string& demangled)
{
    // For "pcl::SandboxInterface::SandboxInterface()" -> "SandboxInterface"
    
    size_t parenPos = demangled.find('(');
    std::string beforeParen = (parenPos != std::string::npos) 
        ? demangled.substr(0, parenPos) 
        : demangled;
    
    size_t lastColon = beforeParen.rfind("::");
    if (lastColon != std::string::npos)
        return beforeParen.substr(lastColon + 2);
    
    return beforeParen;
}

// Parse parameter types from demangled signature
static std::vector<std::string> extractParameters(const std::string& demangled)
{
    std::vector<std::string> params;
    
    size_t parenStart = demangled.find('(');
    size_t parenEnd = demangled.rfind(')');
    
    if (parenStart == std::string::npos || parenEnd == std::string::npos || parenEnd <= parenStart)
        return params;
    
    std::string paramStr = demangled.substr(parenStart + 1, parenEnd - parenStart - 1);
    
    // Handle empty parameters
    if (paramStr.empty() || paramStr == "void")
        return params;
    
    // Parse parameters (simple comma split - doesn't handle nested templates perfectly)
    size_t start = 0;
    int angleDepth = 0;
    int parenDepth = 0;
    
    for (size_t i = 0; i < paramStr.length(); ++i)
    {
        if (paramStr[i] == '<') angleDepth++;
        else if (paramStr[i] == '>') angleDepth--;
        else if (paramStr[i] == '(') parenDepth++;
        else if (paramStr[i] == ')') parenDepth--;
        else if (paramStr[i] == ',' && angleDepth == 0 && parenDepth == 0)
        {
            std::string param = paramStr.substr(start, i - start);
            // Trim whitespace
            size_t first = param.find_first_not_of(" \t");
            size_t last = param.find_last_not_of(" \t");
            if (first != std::string::npos)
                params.push_back(param.substr(first, last - first + 1));
            start = i + 1;
        }
    }
    
    // Last parameter
    std::string param = paramStr.substr(start);
    size_t first = param.find_first_not_of(" \t");
    size_t last = param.find_last_not_of(" \t");
    if (first != std::string::npos)
        params.push_back(param.substr(first, last - first + 1));
    
    return params;
}

// Pretty-print a demangled function signature
static void printSignature(const std::string& mangled)
{
    std::string demangled = demangleVerbose(mangled.c_str());
    std::string className = extractClassName(demangled);
    std::string methodName = extractMethodName(demangled);
    auto params = extractParameters(demangled);
    
    qDebug() << "Full:   " << demangled.c_str() << "\n";
    qDebug() << "Class:  " << className.c_str() << "\n";
    qDebug() << "Method: " << methodName.c_str() << "\n";
    
    if (!params.empty())
    {
        qDebug() << "Params: (" << params.size() << ")\n";
        for (size_t i = 0; i < params.size(); ++i)
            qDebug() << "  [" << i << "] " << params[i].c_str() << "\n";
    }
    else
    {
        qDebug() << "Params: (none)\n";
    }
}

static const char* getExecutablePath()
{
#ifdef __APPLE__
    return _dyld_get_image_name(0);
#elif defined(__linux__)
    static char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        return path;
    }
    return "unknown";
#else
    return "unknown";
#endif
}

using RawCtor0 = void (*)(void*);
using RawCtor1 = void (*)(void*, void*);

static void* mustResolve(const char* name)
{
    // Try the name as-is first
    void* p = dlsym(RTLD_MAIN_ONLY, name);
    if (p) return p;
    
    p = dlsym(RTLD_DEFAULT, name);
    if (p) return p;
    
    // If name starts with __, try with single _ (remove one)
    if (name[0] == '_' && name[1] == '_')
    {
        const char* singleUnderscore = name + 1;
        p = dlsym(RTLD_MAIN_ONLY, singleUnderscore);
        if (p) {
            fprintf(stderr, "INFO: Found with single underscore: %s\n", singleUnderscore);
            return p;
        }
        
        p = dlsym(RTLD_DEFAULT, singleUnderscore);
        if (p) {
            fprintf(stderr, "INFO: Found with single underscore: %s\n", singleUnderscore);
            return p;
        }
    }
    
    // If name starts with single _, try with double __
    if (name[0] == '_' && name[1] != '_')
    {
        std::string doubleUnderscore = std::string("_") + name;
        p = dlsym(RTLD_MAIN_ONLY, doubleUnderscore.c_str());
        if (p) {
            fprintf(stderr, "INFO: Found with double underscore: %s\n", doubleUnderscore.c_str());
            return p;
        }
        
        p = dlsym(RTLD_DEFAULT, doubleUnderscore.c_str());
        if (p) {
            fprintf(stderr, "INFO: Found with double underscore: %s\n", doubleUnderscore.c_str());
            return p;
        }
    }
    
    return nullptr;
}

static void* dynamicNew(void* ctorAddr, size_t size)
{
    if (!ctorAddr) {
        fprintf(stderr, "ERROR: Null constructor address\n");
        return nullptr;
    }
    
    void* mem = ::operator new(size);
    RawCtor0 ctor = reinterpret_cast<RawCtor0>(ctorAddr);
    ctor(mem);
    return mem;
}

// ============================================================================
// Automatic Class Discovery
// ============================================================================

struct DiscoveredClass
{
    std::string className;
    std::string mangledCtorName;
    std::string demangledCtorName;
    void* ctorAddress;
};

QList<DiscoveredClass> findModuleClass()
{
    QList<DiscoveredClass> modlst;
    qDebug() << "Scanning for Module class...";
    
    auto allSymbols = collectAllExternalFunctionSymbols();
    
    for (const auto& mangled : allSymbols)
    {
        std::string demangled = demangleVerbose(mangled.c_str());
        
        // Look for Module constructor: SomeModule::SomeModule()
        // Must end with "Module" and be a constructor
        if (demangled.find("Module::") != std::string::npos &&
            demangled.find("()") != std::string::npos)
        {
            // Extract class name
	    qDebug() << "Full prototype: " << demangled.c_str();
            size_t parenPos = demangled.find('(');
            size_t colonPos = demangled.rfind("::", parenPos);
            std::string className = demangled.substr(0, colonPos);
            std::string membName = demangled.substr(colonPos+2);
	    std::string classBase = className.length() > membName.length() ?
	      className.substr(className.length() - (membName.length() - 2)) : "";
            // Must end with "Module" and not be "MetaModule"
            if (membName.length() >= 8 &&
		membName.substr(0, membName.length() - 2) == classBase &&
                membName.substr(membName.length() - 8) == "Module()" &&
                membName.find("MetaModule()") == std::string::npos)
            {
                void* addr = mustResolve(mangled.c_str());
		qDebug() << "className: " << className.c_str();
		qDebug() << "classBase: " << classBase.c_str();
		qDebug() << "membName: " << membName.c_str();
		qDebug() << QString("addr: 0x%1").arg((size_t)addr, 0, 16);
                if (addr)
                {
                    qDebug() << "  ✓ Found Module:" << className.c_str();
                    qDebug() << "    Symbol:" << mangled.c_str();
                    modlst.append({className, mangled, demangled, addr});
                }
            }
        }
    }

    if (modlst.isEmpty())
        qCritical() << "  ❌ Module class not found!";
    return modlst;
}

DiscoveredClass findProcessClass()
{
    qDebug() << "Scanning for Process class...";
    
    auto allSymbols = collectAllExternalFunctionSymbols();
    
    for (const auto& mangled : allSymbols)
    {
        std::string demangled = demangle(mangled.c_str());
        
        // Look for Process constructor: SomeProcess::SomeProcess()
        // Must end with "Process" but NOT "MetaProcess"
        if (demangled.find("Process::") != std::string::npos &&
            demangled.find("Interface") == std::string::npos &&
            demangled.find("Implementation") == std::string::npos &&
            demangled.find("()") != std::string::npos)
        {
            // Extract class name
            size_t parenPos = demangled.find('(');
            size_t colonPos = demangled.rfind("::", parenPos);
            std::string className = demangled.substr(0, colonPos);
            
            // Must end with "Process" and not be "MetaProcess"
            if (className.length() >= 7 &&
                className.substr(className.length() - 7) == "Process" &&
                className.find("MetaProcess") == std::string::npos)
            {
                void* addr = mustResolve(mangled.c_str());
                if (addr)
                {
                    qDebug() << "  ✓ Found Process:" << className.c_str();
                    qDebug() << "    Symbol:" << mangled.c_str();
                    return {className, mangled, demangled, addr};
                }
            }
        }
    }
    
    qWarning() << "  ⚠️  Process class not found (continuing without)";
    return {"", "", "", nullptr};
}

// ============================================================================
// Main Program
// ============================================================================

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SetDebugLogging(true);

    qDebug() << "===========================================";
    qDebug() << "PCL Interface Export Tool";
    qDebug() << "Fully Automatic Symbol Discovery";
    qDebug() << "===========================================\n";

    qDebug() << "Executable:" << getExecutablePath();
    qDebug() << "";

    // ========================================================================
    // Step 1: Discover and create Module
    // ========================================================================

    // Early Init if needed
    API = new APIInterface( GetMockFunctionResolver() );

    auto modlst = findModuleClass();
    for (auto moduleInfo: modlst)
      {
	qDebug() << "Mangled: " << moduleInfo.mangledCtorName.c_str();
	if (!moduleInfo.ctorAddress)
	  {
	    qCritical() << "\n❌ ERROR: Could not find Module class!";
	    qCritical() << "Make sure your module defines a class ending with 'Module'";
	    qCritical() << "Example: class SandboxModule : public pcl::MetaModule";
	    return 1;
	  }

	qDebug() << "\nCreating module instance...";
	if (!Module)
	  {
	    Module = reinterpret_cast<pcl::MetaModule*>(
                dynamicNew(moduleInfo.ctorAddress, 16384)  // Conservative size
            );
	    /*
	    qDebug() << "Module->m_parent = " << Module->m_parent;
	    qDebug() << "Module->m_children.Length() == " << Module->m_children.Length();
	    for (auto child : Module->m_children)
	      {
		qDebug() << "Module->m_child = " << child;
	      }
	    */
	  }
      }
    
    API = nullptr;
    
    if (!Module)
    {
        qCritical() << "❌ ERROR: Module construction failed!";
        return 1;
    }
    
    qDebug() << "✓ Module created:" << (void*)Module;
    
    // ========================================================================
    // Step 2: Initialize PCL API
    // ========================================================================
    
    qDebug() << "\nInitializing PCL API...";
    InitializePixInsightModule(Module, GetMockFunctionResolver(), 
                              PCL_API_Version, nullptr);
    
    qDebug() << "✓ Module initialized";
    qDebug() << "  Name:" << Module->Name().c_str();
    qDebug() << "  Registered processes:" << Module->Length();
    
    // ========================================================================
    // Step 3: Discover and create Process
    // ========================================================================
    
    auto processInfo = findProcessClass();
    pcl::MetaProcess* MP = nullptr;
    
    if (processInfo.ctorAddress)
    {
        qDebug() << "\nCreating process instance...";
        MP = reinterpret_cast<pcl::MetaProcess*>(
            dynamicNew(processInfo.ctorAddress, 512)
        );
        
        if (MP)
        {
            qDebug() << "✓ Process created:" << (void*)MP;
            qDebug() << "  ID:" << MP->Id().c_str();
        }
        else
        {
            qWarning() << "⚠️  Process construction failed (continuing without)";
        }
    }
    
    // ========================================================================
    // Step 4: Discover Interface classes
    // ========================================================================
    
    qDebug() << "\nScanning for ProcessInterface derivatives...";
    auto discovered = scanDerivedPCLInterfaces();

    if (discovered.empty())
    {
        qCritical() << "\n❌ ERROR: No ProcessInterface derivatives found!\n";
        qCritical() << "To diagnose:";
        qCritical() << "  nm" << getExecutablePath() << "| grep Interface";
        return 1;
    }

    qDebug() << "\n✓ Discovered" << discovered.size() << "interface(s):";
    for (const auto& di : discovered) {
        qDebug() << "  -" << di.className.c_str();
    }

    // ========================================================================
    // Step 5: Choose interface to export
    // ========================================================================
    
    const DiscoveredInterface* chosen = nullptr;
    
    // Command line argument?
    if (argc > 1)
    {
        std::string requested = argv[1];
        for (const auto& di : discovered)
        {
            if (di.className.find(requested) != std::string::npos)
            {
                chosen = &di;
                qDebug() << "\n✓ Selected (requested):" << di.className.c_str();
                break;
            }
        }
    }
    
    // Auto-select first one
    if (!chosen)
    {
        chosen = &discovered.front();
        qDebug() << "\n✓ Selected (first):" << chosen->className.c_str();
    }

    // ========================================================================
    // Step 6: Construct Interface
    // ========================================================================
    
    qDebug() << "\nResolving interface constructor...";
    void* ifaceCtorAddr = mustResolve(chosen->mangledCtorName.c_str());
    
    if (!ifaceCtorAddr)
    {
        qCritical() << "\n❌ ERROR: Could not resolve constructor!";
        qCritical() << "Symbol:" << chosen->mangledCtorName.c_str();
        return 1;
    }

    qDebug() << "✓ Constructor resolved:" << ifaceCtorAddr;

    qDebug() << "\nConstructing interface...";
    pcl::ProcessInterface* IF = reinterpret_cast<pcl::ProcessInterface*>(
        dynamicNew(ifaceCtorAddr, 1024)  // Conservative size for interface
    );
    
    if (!IF)
    {
        qCritical() << "\n❌ ERROR: Interface construction failed!";
        return 1;
    }

    qDebug() << "✓ Interface created:" << (void*)IF;

    // ========================================================================
    // Step 7: Set up mock Qt container
    // ========================================================================
    
    qDebug() << "\nSetting up Qt container...";
    
    MockBase* interfaceRoot = new MockBase();
    interfaceRoot->isSizer = false;
    interfaceRoot->widget = new QWidget(nullptr);
    
    // Extract short name for window title
    std::string shortName = chosen->className;
    size_t colonPos = shortName.rfind("::");
    if (colonPos != std::string::npos)
        shortName = shortName.substr(colonPos + 2);
    
    interfaceRoot->widget->setWindowTitle(QString::fromStdString(shortName));
    g_topLevelWidgets.append(interfaceRoot);
    
    // Set interface handle
    IF->handle = (control_handle)interfaceRoot;
    
    qDebug() << "✓ Container ready";

    // ========================================================================
    // Step 8: Launch interface
    // ========================================================================
    
    if (MP)
    {
        qDebug() << "\nLaunching interface with process...";
        
        pcl::ProcessImplementation* inst = MP->Create();
        if (!inst)
        {
            qCritical() << "❌ ERROR: Could not create process instance!";
            return 1;
        }
        
        bool dynamic = false;
        unsigned flags = 0;
        
        IF->Launch(*MP, inst, dynamic, flags);
        IF->Show();
        
        qDebug() << "✓ Interface launched and shown";
    }
    else
    {
        qWarning() << "\nMock mode: Showing interface without process...";
        IF->Show();
        qDebug() << "✓ Interface shown (limited functionality)";
    }

    // ========================================================================
    // Step 9: Wait for UI to initialize
    // ========================================================================
    
    qDebug() << "\nInitializing UI...";
    QApplication::processEvents();
    QThread::msleep(200);
    QApplication::processEvents();
    QThread::msleep(100);
    QApplication::processEvents();

    // ========================================================================
    // Step 10: Find root widget
    // ========================================================================
    
    qDebug() << "\n===========================================";
    qDebug() << "Finding interface widgets...";
    qDebug() << "===========================================\n";
    
    qDebug() << "Top-level widgets:" << g_topLevelWidgets.size();

    QList<QWidget*> candidates;
    for (MockBase* base : g_topLevelWidgets)
    {
        if (base && base->widget)
        {
            qDebug() << "  Widget:" << base->widget->metaObject()->className()
                     << "Size:" << base->widget->size();
            candidates.append(base->widget);
        }
    }

    if (candidates.isEmpty())
    {
        qCritical() << "\n❌ ERROR: No widgets found!";
        return 1;
    }

    QWidget* root = RootWidgetSelector::selectBestRoot(candidates, true);
    
    if (!root)
    {
        qCritical() << "\n❌ ERROR: Widget selection failed!";
        return 1;
    }

    qDebug() << "\n✓ Root widget selected:" << root->metaObject()->className();

    // ========================================================================
    // Step 11: Export interface
    // ========================================================================
    
    std::string baseName = chosen->className;
    colonPos = baseName.rfind("::");
    if (colonPos != std::string::npos)
        baseName = baseName.substr(colonPos + 2);
    
    QString qBaseName = QString::fromStdString(baseName);
    QString outputDir = "./exported";

    qDebug() << "\n===========================================";
    qDebug() << "Exporting interface...";
    qDebug() << "===========================================\n";
    qDebug() << "Interface:" << qBaseName;
    qDebug() << "Output:   " << outputDir;

    bool success = ExportHelper::exportInterface(root, qBaseName, outputDir);

    if (success)
    {
        qDebug() << "\n===========================================";
        qDebug() << "✅ EXPORT COMPLETED SUCCESSFULLY!";
        qDebug() << "===========================================\n";
        qDebug() << "Generated files in:" << outputDir;
        qDebug() << "  -" << qBaseName + ".h";
        qDebug() << "  -" << qBaseName + ".cpp";
        qDebug() << "  -" << qBaseName + "_metadata.json";
        qDebug() << "  - CMakeLists.txt";
        qDebug() << "  - main.cpp";
        qDebug() << "  - README.md\n";
        qDebug() << "To build:";
        qDebug() << "  cd" << outputDir;
        qDebug() << "  mkdir build && cd build";
        qDebug() << "  cmake .. && make";
        qDebug() << "  ./" + qBaseName;
        qDebug() << "===========================================\n";
    }
    else
    {
        qCritical() << "\n❌ EXPORT FAILED!";
        return 1;
    }

    // ========================================================================
    // Step 12: Keep window open
    // ========================================================================
    
    qDebug() << "Interface ready. Close window to exit.\n";
    
    // Show the window
    if (root)
    {
        root->show();
        root->raise();
        root->activateWindow();
    }
    
    return app.exec();
}

// ============================================================================
// USAGE NOTES
// ============================================================================
/*

FULLY AUTOMATIC - NO FACTORY FUNCTIONS NEEDED!
-----------------------------------------------

This version discovers everything automatically via symbol scanning:
- Finds Module class (anything ending with "Module")
- Finds Process class (anything ending with "Process")  
- Finds Interface classes (anything ending with "Interface")

Your module code needs NO special factory functions!

Just define your classes normally:

    class SandboxModule : public pcl::MetaModule { ... };
    class SandboxProcess : public pcl::MetaProcess { ... };
    class SandboxInterface : public pcl::ProcessInterface { ... };

Build and run:

    $ make
    $ ./Sandbox-pxm.mock

The tool will:
1. Scan the binary for your classes
2. Construct them dynamically
3. Launch the interface
4. Export Qt code

Command line options:

    $ ./Sandbox-pxm.mock              # Auto-select first interface
    $ ./Sandbox-pxm.mock Sandbox      # Select specific interface

Build requirements:

    - Link with: -Wl,-export_dynamic (for dlsym to work)
    - Keep symbols: Don't use -s flag
    - Debug info helpful: Use -g flag

*/
