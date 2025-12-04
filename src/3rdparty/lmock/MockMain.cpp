// MockMain_Library.cpp
// Main program for use with pre-compiled mock library
// This version understands that PCLMockAPI is a library and 
// the actual ProcessInterface classes come from the linked module

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
// Module Factory Function
// ============================================================================
// Each module must provide this factory function to create its MetaModule.
// The module code should implement:
//   extern "C" pcl::MetaModule* CreateModuleInstance() {
//       return new YourModule();
//   }

extern "C"
  {
    pcl::MetaModule* CreateModuleInstance();
    pcl::MetaProcess* CreateProcessInstance();
    pcl::ProcessInterface* CreateProcessInterface();
  };

// ============================================================================
// Library Architecture Support
// ============================================================================

// Cross-platform way to get executable path
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

// When compiling as a library + module architecture:
// 1. PCLMockAPI, ExportHelper, etc. are in a library (e.g., libPCLMock.a)
// 2. SandboxInterface, SandboxProcess, etc. are in the final executable
// 3. Symbol scanning needs to look at the FINAL executable, not the library

// Raw constructor types
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
        const char* singleUnderscore = name + 1;  // Skip first _
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
    
    // If name starts with single _, try with double __ (add one)
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
    
    fprintf(stderr, "WARNING: dlsym(%s) failed: %s\n", name, dlerror());
    return nullptr;
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

    qDebug() << "";
    qDebug() << "Creating module instance...";
    
    // Call the factory function to create the module
    // The module code must implement:
    //   extern "C" pcl::MetaModule* CreateModuleInstance() { return new YourModule(); }
    Module = CreateModuleInstance();
    
    if (!Module)
    {
        qCritical() << "❌ ERROR: CreateModuleInstance() returned null!";
        qCritical() << "Make sure your module code implements:";
        qCritical() << "  extern \"C\" pcl::MetaModule* CreateModuleInstance()";
        qCritical() << "";
        qCritical() << "Example:";
        qCritical() << "  extern \"C\" pcl::MetaModule* CreateModuleInstance() {";
        qCritical() << "      return new SandboxModule();";
        qCritical() << "  }";
        return 1;
    }
    
    qDebug() << "✓ Module instance created:" << (void*)Module;
    qDebug() << "";
    
    // Initialize the module with PCL API
    qDebug() << "Initializing module with PCL API...";
    InitializePixInsightModule(Module, GetMockFunctionResolver(), 
                              PCL_API_Version, nullptr);

    if (!Module)
    {
        qCritical() << "ERROR: Module initialization failed!";
        qCritical() << "Make sure InitializePixInsightModule() is properly implemented";
        qCritical() << "in your module and creates a MetaModule instance.";
        return 1;
    }

    qDebug() << "Module initialized successfully";
    qDebug() << "Module name:" << Module->Name().c_str();
    qDebug() << "Process count:" << Module->Length();
    qDebug() << "";

    // ============================================
    // Process Registration in PCL
    // ============================================
    // In PCL, MetaProcess objects register themselves automatically
    // via their constructor: MetaProcess::MetaProcess() -> MetaObject(Module)
    //
    // There is NO Install() method in the PCL API!
    //
    // Processes get registered when:
    // - new SandboxProcess is called (heap allocation)
    // - SandboxProcess proc; is declared (stack allocation)  
    // - Static/global MetaProcess instances are created
    //
    // The Module constructor or static initialization typically creates
    // MetaProcess instances, which auto-register.
    // ============================================
    
    // In mock environment, processes are optional
    // The interface can be launched without a real MetaProcess
    bool hasProcesses = (Module->Length() > 0);
    
    if (!hasProcesses)
    {
        qWarning() << "⚠️  No processes registered (Process count: 0)";
        qWarning() << "This is OK for mock environment - interface will show without MetaProcess";
        qWarning() << "Note: MetaProcess instances auto-register via constructor";
        qWarning() << "";
    }
    else
    {
        // List all processes
        qDebug() << "Available processes:";
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
    }

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
    // Find the MetaProcess for this interface (optional in mock mode)
    // ========================================================================
    
    qDebug() << "Finding MetaProcess for interface...";
    pcl::MetaProcess* MP = findMetaProcessForInterface(chosen->className);
    
    if (!MP)
    {
        qWarning() << "";
        qWarning() << "⚠️  Could not find MetaProcess (no processes registered)";
        qWarning() << "Continuing in MOCK MODE - interface will launch without real process";
        qWarning() << "";
	MP = CreateProcessInstance();
    }
    else
    {
        qDebug() << "✓ Found MetaProcess:" << MP->Id().c_str();
    }

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
    
    pcl::ProcessInterface* IF = reinterpret_cast<pcl::ProcessInterface*>(dynamicNew(ctorAddr, sz)
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
    
    pcl::ProcessImplementation* inst = nullptr;
    
    if (MP)
    {
        qDebug() << "\nCreating process instance...";
        inst = MP->Create();
        
        if (!inst)
        {
            qCritical() << "\n❌ ERROR: MetaProcess::Create() returned null!";
            return 1;
        }
        qDebug() << "✓ Process instance created";

	MockBase* interfaceRoot = new MockBase();
	interfaceRoot->isSizer = false;
	interfaceRoot->widget = new QWidget(nullptr);  // True top-level
	interfaceRoot->widget->setWindowTitle("MockMain");

	// Add to top-level list
	g_topLevelWidgets.append(interfaceRoot);

	// Set the interface's handle (simulate what PixInsight core does)
	// This is what InterfaceDispatcher::Initialize() does:
	IF->handle = (control_handle)interfaceRoot;
	
        qDebug() << "\nLaunching interface...";
        bool dynamic = false;
        unsigned flags = 0;
        IF->Launch(*MP, inst, dynamic, flags);
        IF->Show();
        qDebug() << "✓ Interface launched and shown";
    }
    else
    {
        // Mock mode: No MetaProcess available
        // Just show the interface without launching
        qDebug() << "\nMock mode: Showing interface without Launch()";
        qDebug() << "⚠️  Interface shown but not launched (no process)";
        qDebug() << "⚠️  For full testing, implement Module->Install() with your process";
        IF->Show();
        qDebug() << "✓ Interface shown";
    }

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
// USAGE
// ============================================================================
/*

ARCHITECTURE:
-------------
This tool is designed for the following build architecture:

1. Core mock library (built once):
   - PCLMockAPI.cpp
   - PCLThreadMock.cpp
   - ExportHelper.cpp
   - QtUiExporter.cpp
   - PCLInterfaceScanner.cpp
   → Compiled into: libPCLMock.a (or .dylib)

2. Module-specific code (built per module):
   - SandboxModule.cpp (or YourModule.cpp)
   - SandboxProcess.cpp (or YourProcess.cpp)
   - SandboxInterface.cpp (or YourInterface.cpp)
   - MockMain_Library.cpp (this file)
   → Linked with libPCLMock.a
   → Produces: Sandbox-pxm.mock (or YourModule-pxm.mock)

BUILD EXAMPLE:
-------------

Step 1: Build the mock library (once)
$ clang++ -std=c++17 -c -g \
    -I/opt/homebrew/include \
    -I/path/to/PCL/include \
    PCLMockAPI.cpp \
    PCLThreadMock.cpp \
    ExportHelper.cpp \
    QtUiExporter.cpp \
    PCLInterfaceScanner.cpp

$ ar rcs libPCLMock.a *.o

Step 2: Build the module executable (per module)
$ clang++ -std=c++17 -g \
    -I/opt/homebrew/include \
    -I/path/to/PCL/include \
    -L. -L/opt/homebrew/lib \
    MockMain_Library.cpp \
    SandboxModule.cpp \
    SandboxProcess.cpp \
    SandboxInterface.cpp \
    -lPCLMock \
    -framework QtCore -framework QtWidgets \
    -o Sandbox-pxm.mock

Step 3: Run
$ ./Sandbox-pxm.mock

Step 4: Use exported code
$ cd exported
$ mkdir build && cd build
$ cmake ..
$ make
$ ./SandboxInterface

COMMAND LINE OPTIONS:
--------------------

Export a specific interface (if multiple are present):
$ ./Sandbox-pxm.mock SandboxInterface

Or just run without arguments to auto-select:
$ ./Sandbox-pxm.mock

TROUBLESHOOTING:
---------------

If you get "No PCL ProcessInterface derivatives found":

1. Check symbols are present:
   $ nm Sandbox-pxm.mock | grep Interface

2. Check symbols aren't stripped:
   $ nm -a Sandbox-pxm.mock | wc -l
   (Should be > 1000 for a typical module)

3. Build with debug symbols:
   Add -g flag, remove -s flag

4. Ensure interface .cpp is actually linked:
   $ nm Sandbox-pxm.mock | grep SandboxInterface
   Should show C1, C2, D0, D1, D2 symbols

*/

int _main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SetDebugLogging(true);

    // Initialize API before Console
    
    Module = CreateModuleInstance();
    InitializePixInsightModule(Module, GetMockFunctionResolver(), PCL_API_Version, nullptr );

    pcl::MetaProcess* MP = CreateProcessInstance();

    pcl::ProcessInterface* IF = CreateProcessInterface();
    MockBase* interfaceRoot = new MockBase();
    interfaceRoot->isSizer = false;
    interfaceRoot->widget = new QWidget(nullptr);  // True top-level
    interfaceRoot->widget->setWindowTitle("MockMain");
    
    // Add to top-level list
    g_topLevelWidgets.append(interfaceRoot);
    
    // Set the interface's handle (simulate what PixInsight core does)
    // This is what InterfaceDispatcher::Initialize() does:
    IF->handle = (control_handle)interfaceRoot;

    bool dynamic = false;
    unsigned flags = 0;
    
    IF->Launch(*MP, nullptr, dynamic, flags);
    IF->Show();

    return app.exec();
}
