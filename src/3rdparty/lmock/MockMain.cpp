// MockMain_Library.cpp
// Main program for use with pre-compiled mock library
// This version requires explicit module instantiation via extern functions

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
// MODULE INSTANTIATION - CONFIGURE THIS SECTION
// ============================================================================
// 
// Each module must provide a factory function that creates its MetaModule.
// Declare it here as extern and the linker will find it from your module code.
//
// Example for Sandbox module:
//   extern pcl::MetaModule* CreateSandboxModule();
//
// Then in your SandboxModule.cpp, implement:
//   pcl::MetaModule* CreateSandboxModule() {
//       return new SandboxModule();
//   }
//
// Or, if you prefer a more generic approach, use a preprocessor define:
//   Compile with: -DMODULE_FACTORY=CreateSandboxModule

#ifndef MODULE_FACTORY
// Default: Try to find a factory function by naming convention
// If your module is "Sandbox", it looks for CreateSandboxModule()
// You can override this by defining MODULE_FACTORY at compile time
#define MODULE_FACTORY CreateModuleInstance
#endif

// Declare the module factory function
// Your module code must implement this
extern "C" pcl::MetaModule* MODULE_FACTORY();

// Helper for error messages
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

// ============================================================================
// Cross-platform executable path helper
// ============================================================================

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

// ============================================================================
// Library Architecture Support
// ============================================================================

// When compiling as a library + module architecture:
// 1. PCLMockAPI, ExportHelper, etc. are in a library (e.g., libPCLMock.a)
// 2. SandboxInterface, SandboxProcess, etc. are in the final executable
// 3. Symbol scanning needs to look at the FINAL executable, not the library

// Raw constructor types
using RawCtor0 = void (*)(void*);
using RawCtor1 = void (*)(void*, void*);

static void* mustResolve(const char* name)
{
    // Try resolving from the main program first
    void* p = dlsym(RTLD_MAIN_ONLY, name);
    if (!p) {
        // Fall back to default search
        p = dlsym(RTLD_DEFAULT, name);
    }
    
    if (!p)
    {
        fprintf(stderr, "WARNING: dlsym(%s) failed: %s\n", name, dlerror());
        return nullptr;
    }
    return p;
}

static void* dynamicNew(void* ctorAddr, size_t size, void* arg1 = nullptr)
{
    if (!ctorAddr) {
        fprintf(stderr, "ERROR: Null constructor address\n");
        return nullptr;
    }
    
    void* mem = ::operator new(size);
    if (!arg1)
    {
        RawCtor0 ctor = reinterpret_cast<RawCtor0>(ctorAddr);
        ctor(mem);
    }
    else
    {
        RawCtor1 ctor = reinterpret_cast<RawCtor1>(ctorAddr);
        ctor(mem, arg1);
    }
    return mem;
}

// Size estimation for ProcessInterface derivatives
size_t sizeForInterfaceClass(const std::string& className)
{
    // Base size - most interfaces are close to this
    size_t baseSize = sizeof(pcl::ProcessInterface);
    
    // Add padding for derived class members
    // Most ProcessInterface derivatives add 50-200 bytes of members
    return baseSize + 256;  // Conservative estimate
}

// Try to find MetaProcess by scanning the module
pcl::MetaProcess* findMetaProcessForInterface(const std::string& className)
{
    // Access the global Module
    if (!Module) {
        fprintf(stderr, "ERROR: Global Module is null\n");
        return nullptr;
    }
    
    // Scan through all processes in the module
    for (size_t i = 0; i < Module->Length(); ++i)
    {
        const pcl::MetaObject* obj = (*Module)[i];
        if (!obj) continue;
        
        // Cast to MetaProcess
        pcl::MetaProcess* process = const_cast<pcl::MetaProcess*>(
            dynamic_cast<const pcl::MetaProcess*>(obj)
        );
        if (!process) continue;
        
        // Try to match by name heuristics
        std::string processId = process->Id().c_str();
        
        // e.g., "SandboxInterface" -> look for "Sandbox" process
        // Remove "Interface" suffix
        std::string baseName = className;
        size_t ifacePos = baseName.rfind("Interface");
        if (ifacePos != std::string::npos) {
            baseName = baseName.substr(0, ifacePos);
        }
        
        // Remove namespace
        size_t colonPos = baseName.rfind("::");
        if (colonPos != std::string::npos) {
            baseName = baseName.substr(colonPos + 2);
        }
        
        qDebug() << "  Checking process:" << processId.c_str() 
                 << "against" << baseName.c_str();
        
        if (processId.find(baseName) != std::string::npos ||
            baseName.find(processId) != std::string::npos)
        {
            qDebug() << "  ✓ Matched!";
            return process;
        }
    }
    
    // If no match found, just return the first process
    if (Module->Length() > 0) {
        qWarning() << "  No exact match, using first process";
        const pcl::MetaObject* obj = (*Module)[0];
        return const_cast<pcl::MetaProcess*>(
            dynamic_cast<const pcl::MetaProcess*>(obj)
        );
    }
    
    return nullptr;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SetDebugLogging(true);

    qDebug() << "===========================================";
    qDebug() << "PCL Interface Export Tool (Library Mode)";
    qDebug() << "===========================================\n";

    qDebug() << "Architecture: Mock library + Module executable";
    qDebug() << "Executable:" << getExecutablePath();
    qDebug() << "";

    // ========================================================================
    // Initialize Module - Create instance then initialize
    // ========================================================================
    
    qDebug() << "Creating module instance...";
    
    // Call the factory function to create the module
    Module = MODULE_FACTORY();
    
    if (!Module)
    {
        qCritical() << "ERROR: Module factory returned null!";
        qCritical() << "Make sure your module code implements:";
        qCritical() << "  extern \"C\" pcl::MetaModule*" << TO_STRING(MODULE_FACTORY) << "()";
        qCritical() << "";
        qCritical() << "Example implementation:";
        qCritical() << "  extern \"C\" pcl::MetaModule*" << TO_STRING(MODULE_FACTORY) << "() {";
        qCritical() << "      return new YourModule();";
        qCritical() << "  }";
        return 1;
    }
    
    qDebug() << "✓ Module instance created:" << Module;
    
    qDebug() << "\nInitializing module with PCL API...";
    InitializePixInsightModule(Module, GetMockFunctionResolver(), 
                              PCL_API_Version, nullptr);

    qDebug() << "✓ Module initialized successfully";
    qDebug() << "Module name:" << Module->Name().c_str();
    qDebug() << "Process count:" << Module->Length();

    // List all processes
    qDebug() << "\nAvailable processes:";
    for (size_t i = 0; i < Module->Length(); ++i)
    {
        const pcl::MetaObject* obj = (*Module)[i];
        if (obj) {
            pcl::MetaProcess* process = const_cast<pcl::MetaProcess*>(
                dynamic_cast<const pcl::MetaProcess*>(obj)
            );
            if (process) {
                qDebug() << "  [" << i << "]" << process->Id().c_str();
            }
        }
    }
    qDebug() << "";

    // ========================================================================
    // Discover interface classes in the final executable
    // ========================================================================
    
    qDebug() << "Scanning for ProcessInterface derivatives...";
    auto discovered = scanDerivedPCLInterfaces();

    if (discovered.empty())
    {
        qCritical() << "\n❌ ERROR: No PCL ProcessInterface derivatives found!\n";
        qCritical() << "This could mean:";
        qCritical() << "1. The module's interface classes aren't linked into the executable";
        qCritical() << "2. Symbols were stripped from the final binary";
        qCritical() << "3. The interface classes don't follow expected naming conventions";
        qCritical() << "";
        qCritical() << "To diagnose:";
        qCritical() << "  nm" << getExecutablePath() << "| grep Interface";
        qCritical() << "";
        qCritical() << "To fix:";
        qCritical() << "  - Ensure module interface .cpp files are linked";
        qCritical() << "  - Build with -g flag (keep debug symbols)";
        qCritical() << "  - Don't use -s flag (don't strip)";
        return 1;
    }

    qDebug() << "\n✓ Discovered" << discovered.size() << "interface(s):\n";
    for (const auto& di : discovered) {
        qDebug() << "  Class:  " << di.className.c_str();
        qDebug() << "  Symbol: " << di.mangledCtorName.c_str();
        qDebug() << "";
    }

    // ========================================================================
    // Choose which interface to export
    // ========================================================================
    
    const DiscoveredInterface* chosen = nullptr;
    
    // Strategy 1: User specified via command line
    if (argc > 1) {
        std::string requested = argv[1];
        qDebug() << "Looking for requested interface:" << requested.c_str();
        
        for (const auto& di : discovered) {
            if (di.className.find(requested) != std::string::npos) {
                chosen = &di;
                qDebug() << "✓ Found:" << di.className.c_str();
                break;
            }
        }
        
        if (!chosen) {
            qWarning() << "⚠ Requested interface not found, using heuristic selection";
        }
    }
    
    // Strategy 2: Prefer "Sandbox" if present (common test module)
    if (!chosen) {
        for (const auto& di : discovered) {
            if (di.className.find("Sandbox") != std::string::npos) {
                chosen = &di;
                qDebug() << "Auto-selected (Sandbox):" << di.className.c_str();
                break;
            }
        }
    }
    
    // Strategy 3: Just use the first one
    if (!chosen) {
        chosen = &discovered.front();
        qDebug() << "Auto-selected (first):" << chosen->className.c_str();
    }

    qDebug() << "\n===========================================";
    qDebug() << "Selected interface:" << chosen->className.c_str();
    qDebug() << "===========================================\n";

    // ========================================================================
    // Find the MetaProcess for this interface
    // ========================================================================
    
    qDebug() << "Finding MetaProcess for interface...";
    pcl::MetaProcess* MP = findMetaProcessForInterface(chosen->className);
    
    if (!MP)
    {
        qCritical() << "\n❌ ERROR: Could not find MetaProcess!";
        qCritical() << "The module must define at least one MetaProcess.";
        return 1;
    }

    qDebug() << "✓ Found MetaProcess:" << MP->Id().c_str();

    // ========================================================================
    // Dynamically construct the interface
    // ========================================================================
    
    qDebug() << "\nResolving constructor symbol...";
    void* ctorAddr = mustResolve(chosen->mangledCtorName.c_str());
    
    if (!ctorAddr)
    {
        qCritical() << "\n❌ ERROR: Could not resolve constructor!";
        qCritical() << "Symbol:" << chosen->mangledCtorName.c_str();
        qCritical() << "";
        qCritical() << "This usually means:";
        qCritical() << "1. The symbol is present but not exported (not extern)";
        qCritical() << "2. The symbol was stripped after linking";
        qCritical() << "3. The interface is in a separate shared library";
        return 1;
    }

    qDebug() << "✓ Constructor resolved at:" << ctorAddr;

    qDebug() << "\nConstructing interface instance...";
    size_t sz = sizeForInterfaceClass(chosen->className);
    qDebug() << "  Allocating" << sz << "bytes";
    
    pcl::ProcessInterface* IF = reinterpret_cast<pcl::ProcessInterface*>(
        dynamicNew(ctorAddr, sz)
    );
    
    if (!IF)
    {
        qCritical() << "\n❌ ERROR: Interface construction failed!";
        return 1;
    }

    qDebug() << "✓ Interface constructed at:" << IF;

    // ========================================================================
    // Create process instance and launch interface
    // ========================================================================
    
    qDebug() << "\nCreating process instance...";
    pcl::ProcessImplementation* inst = MP->Create();
    
    if (!inst)
    {
        qCritical() << "\n❌ ERROR: MetaProcess::Create() returned null!";
        return 1;
    }

    qDebug() << "✓ Process instance created";

    qDebug() << "\nLaunching interface...";
    bool dynamic = false;
    unsigned flags = 0;

    IF->Launch(*MP, inst, dynamic, flags);
    IF->Show();

    qDebug() << "✓ Interface launched and shown";

    // ========================================================================
    // Allow Qt to render the interface
    // ========================================================================
    
    qDebug() << "\nProcessing Qt events...";
    QApplication::processEvents();
    QThread::msleep(200);  // Give widgets time to initialize
    QApplication::processEvents();
    QThread::msleep(100);  // Extra time for complex layouts
    QApplication::processEvents();

    // ========================================================================
    // Find and select the best root widget
    // ========================================================================
    
    qDebug() << "\n===========================================";
    qDebug() << "Searching for interface widgets...";
    qDebug() << "===========================================\n";
    
    qDebug() << "Top-level widgets found:" << g_topLevelWidgets.size();

    QList<QWidget*> candidates;
    for (MockBase* base : g_topLevelWidgets)
    {
        if (base && base->widget)
        {
            qDebug() << "  Candidate:";
            qDebug() << "    Widget:" << base->widget;
            qDebug() << "    Type:  " << base->widget->metaObject()->className();
            qDebug() << "    Size:  " << base->widget->size();
            qDebug() << "    Name:  " << base->widget->objectName();
            candidates.append(base->widget);
        }
    }

    if (candidates.isEmpty())
    {
        qCritical() << "\n❌ ERROR: No top-level widgets found!";
        qCritical() << "The interface may not have created any widgets.";
        qCritical() << "Check that your ProcessInterface::Initialize() creates controls.";
        return 1;
    }

    qDebug() << "\nSelecting best root widget...";
    QWidget* root = RootWidgetSelector::selectBestRoot(candidates, true);
    
    if (!root)
    {
        qCritical() << "\n❌ ERROR: Widget selection failed!";
        return 1;
    }

    qDebug() << "\n===========================================";
    qDebug() << "✓ Root widget selected";
    qDebug() << "===========================================";
    qDebug() << "Widget:     " << root;
    qDebug() << "Type:       " << root->metaObject()->className();
    qDebug() << "Size:       " << root->size();
    qDebug() << "Object Name:" << root->objectName();
    qDebug() << "===========================================\n";

    // ========================================================================
    // Export the interface
    // ========================================================================
    
    // Extract base name from class name
    // e.g., "pcl::SandboxInterface" -> "SandboxInterface"
    std::string baseName = chosen->className;
    size_t colonPos = baseName.rfind("::");
    if (colonPos != std::string::npos) {
        baseName = baseName.substr(colonPos + 2);
    }
    
    QString qBaseName = QString::fromStdString(baseName);
    QString outputDir = "./exported";

    qDebug() << "Exporting interface...";
    qDebug() << "  Base name: " << qBaseName;
    qDebug() << "  Output:    " << outputDir;
    qDebug() << "";

    bool success = ExportHelper::exportInterface(root, qBaseName, outputDir);

    if (success)
    {
        qDebug() << "\n===========================================";
        qDebug() << "✅ EXPORT COMPLETED SUCCESSFULLY!";
        qDebug() << "===========================================";
        qDebug() << "";
        qDebug() << "Generated files in:" << outputDir;
        qDebug() << "  -" << qBaseName + ".h";
        qDebug() << "  -" << qBaseName + ".cpp";
        qDebug() << "  -" << qBaseName + "_metadata.json";
        qDebug() << "  - CMakeLists.txt";
        qDebug() << "  - main.cpp";
        qDebug() << "  - README.md";
        qDebug() << "";
        qDebug() << "To build the exported interface:";
        qDebug() << "  cd" << outputDir;
        qDebug() << "  mkdir build && cd build";
        qDebug() << "  cmake ..";
        qDebug() << "  make";
        qDebug() << "  ./" + qBaseName;
        qDebug() << "===========================================\n";
    }
    else
    {
        qCritical() << "\n❌ EXPORT FAILED!";
        qCritical() << "Check error messages above for details.";
        return 1;
    }

    // ========================================================================
    // Optional: Keep interface open for inspection
    // ========================================================================
    
    qDebug() << "Interface is ready. Close the window to exit.";
    qDebug() << "(Or press Ctrl+C to exit immediately)\n";
    
    return app.exec();
}

// ============================================================================
// USAGE DOCUMENTATION
// ============================================================================
/*

MODULE FACTORY FUNCTION
=======================

Your module code must provide a factory function that creates the MetaModule.

METHOD 1: Default naming (recommended)
---------------------------------------
Implement in your module code:

    extern "C" pcl::MetaModule* CreateModuleInstance() {
        return new SandboxModule();  // or YourModule()
    }

Then compile normally.

METHOD 2: Custom factory name
------------------------------
If you want a different name, define it at compile time:

    -DMODULE_FACTORY=CreateSandboxModule

Then implement in your module code:

    extern "C" pcl::MetaModule* CreateSandboxModule() {
        return new SandboxModule();
    }

METHOD 3: Module-specific header
---------------------------------
Create a header that your module includes:

    // ModuleFactory.h
    #define CREATE_MODULE_FACTORY(ModuleClass) \
        extern "C" pcl::MetaModule* CreateModuleInstance() { \
            return new ModuleClass(); \
        }

Then in your module:

    #include "ModuleFactory.h"
    CREATE_MODULE_FACTORY(SandboxModule)

EXAMPLE: SandboxModule.cpp
==========================

    #include <pcl/MetaModule.h>
    #include "SandboxModule.h"
    
    // Module factory function
    extern "C" pcl::MetaModule* CreateModuleInstance() {
        return new SandboxModule();
    }
    
    // Rest of your module code...
    SandboxModule::SandboxModule() { ... }

BUILD EXAMPLE
=============

    clang++ -std=c++17 -g \
        -I/opt/homebrew/include \
        -I/path/to/PCL/include \
        MockMain_Library.cpp \
        SandboxModule.cpp \
        SandboxProcess.cpp \
        SandboxInterface.cpp \
        -lPCLMock \
        -framework QtCore -framework QtWidgets \
        -o Sandbox-pxm.mock

Or with custom factory name:

    clang++ -std=c++17 -g \
        -DMODULE_FACTORY=CreateSandboxModule \
        ...

TROUBLESHOOTING
===============

Error: "Module factory returned null"
→ Make sure you implemented the factory function in your module code

Error: "undefined reference to CreateModuleInstance"  
→ Your module code doesn't export the factory function

Error: "MODULE_FACTORY is not defined"
→ Normal - it uses the default "CreateModuleInstance"

*/
