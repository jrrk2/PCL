// MockMain.cpp
// PCL interface export tool with interactive menu selection
// Discovers all available processes and interfaces via symbol scanning
// and lets you select them manually from Qt menubars

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QThread>
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
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
// Symbol Resolution Utilities (from original MockMain.cpp)
// ============================================================================

#include <cxxabi.h>
#include <string>
#include <regex>
#include <map>

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
        size_t parenPos = result.find('(');
        if (parenPos != std::string::npos)
            result = result.substr(0, parenPos);
    }
    
    return result;
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

static void* mustResolve(const char* name)
{
    void* p = dlsym(RTLD_MAIN_ONLY, name);
    if (p) return p;
    
    p = dlsym(RTLD_DEFAULT, name);
    if (p) return p;
    
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
    
    fprintf(stderr, "ERROR: Symbol not found: %s\n", name);
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return nullptr;
}

static void* dynamicNew(void* ctorAddr, size_t size)
{
    if (!ctorAddr) {
        fprintf(stderr, "ERROR: Null constructor address\n");
        return nullptr;
    }
    
    void* mem = ::operator new(size);
    memset(mem, 0, size);
    RawCtor0 ctor = reinterpret_cast<RawCtor0>(ctorAddr);
    ctor(mem);
    return mem;
}

// ============================================================================
// Module & Process Discovery (from original MockMain.cpp)
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
        std::string demangled = demangle(mangled.c_str());
        
        if (demangled.find("Module::") != std::string::npos &&
            demangled.find("()") != std::string::npos)
        {
            size_t parenPos = demangled.find('(');
            size_t colonPos = demangled.rfind("::", parenPos);
            std::string className = demangled.substr(0, colonPos);
            std::string membName = demangled.substr(colonPos+2);
            std::string classBase = className.length() > membName.length() ?
              className.substr(className.length() - (membName.length() - 2)) : "";
            
            if (membName.length() >= 8 &&
                membName.substr(0, membName.length() - 2) == classBase &&
                membName.substr(membName.length() - 8) == "Module()" &&
                membName.find("MetaModule()") == std::string::npos)
            {
                void* addr = mustResolve(mangled.c_str());
                if (addr)
                {
                    qDebug() << "  ✓ Found Module:" << className.c_str();
                    modlst.append({className, mangled, demangled, addr});
                }
            }
        }
    }

    if (modlst.isEmpty())
        qCritical() << "  ❌ Module class not found!";
    return modlst;
}

struct ProcessInfo
{
    std::string className;
    std::string mangledCtorName;
    void* ctorAddress;
    std::vector<std::string> methods;  // All discovered methods
};

QList<ProcessInfo> findProcessClasses()
{
    QList<ProcessInfo> proclst;
    qDebug() << "Scanning for Process classes and their methods...";
    
    auto allSymbols = collectAllExternalFunctionSymbols();
    
    // First pass: find all Process classes by their constructors
    std::map<std::string, ProcessInfo> processMap;
    
    for (const auto& mangled : allSymbols)
    {
        std::string demangled = demangle(mangled.c_str());
        
        // Look for constructors
        if (demangled.find("Process::") != std::string::npos &&
            demangled.find("Interface") == std::string::npos &&
            demangled.find("Implementation") == std::string::npos &&
            demangled.find("()") != std::string::npos)
        {
            size_t parenPos = demangled.find('(');
            size_t colonPos = demangled.rfind("::", parenPos);
            std::string className = demangled.substr(0, colonPos);
            
            // Check if this is a constructor (class::class())
            std::string methodName = demangled.substr(colonPos + 2, parenPos - colonPos - 2);
            size_t lastClassColon = className.rfind("::");
            std::string bareClassName = (lastClassColon != std::string::npos) 
                ? className.substr(lastClassColon + 2) 
                : className;
            
            bool isConstructor = (methodName == bareClassName);
            
            if (isConstructor &&
                className.length() >= 7 &&
                className.substr(className.length() - 7) == "Process" &&
                className.find("MetaProcess") == std::string::npos &&
                className.find("ParallelProcess") == std::string::npos)
            {
                void* addr = mustResolve(mangled.c_str());
                if (addr && processMap.find(className) == processMap.end())
                {
                    ProcessInfo pi;
                    pi.className = className;
                    pi.mangledCtorName = mangled;
                    pi.ctorAddress = addr;
                    processMap[className] = pi;
                    
                    qDebug() << "  ✓ Found Process:" << className.c_str();
                }
            }
        }
    }
    
    // Second pass: find all methods for each discovered process
    for (const auto& mangled : allSymbols)
    {
        std::string demangled = demangle(mangled.c_str());
        
        // Check if this symbol belongs to any of our discovered processes
        for (auto& pair : processMap)
        {
            const std::string& className = pair.first;
            ProcessInfo& pi = pair.second;
            
            // Does this symbol belong to this class?
            if (demangled.find(className + "::") != std::string::npos)
            {
                // Extract just the method signature
                size_t classPos = demangled.find(className + "::");
                if (classPos != std::string::npos)
                {
                    std::string methodSig = demangled.substr(classPos);
                    
                    // Skip constructors and destructors - we already have those
                    if (methodSig.find("::~") == std::string::npos)
                    {
                        // Only add if not already in list
                        if (std::find(pi.methods.begin(), pi.methods.end(), methodSig) == pi.methods.end())
                        {
                            pi.methods.push_back(methodSig);
                        }
                    }
                }
            }
        }
    }
    
    // Convert map to list and log
    for (auto& pair : processMap)
    {
        ProcessInfo& pi = pair.second;
        qDebug() << "    Found" << pi.methods.size() << "methods";
        proclst.append(pi);
    }
    
    if (proclst.isEmpty())
        qWarning() << "  ⚠️  No Process classes found (continuing without)";
    
    return proclst;
}

// ============================================================================
// Selection Window - Main menu interface
// ============================================================================

class SelectionWindow : public QMainWindow
{
public:
    SelectionWindow(const std::vector<DiscoveredInterface>& discovered,
                   const QList<ProcessInfo>& processes,
                   QWidget* parent = nullptr)
        : QMainWindow(parent)
        , m_discovered(discovered)
        , m_processes(processes)
        , m_selectedProcess(nullptr)
        , m_selectedInterface(nullptr)
        , m_processInstance(nullptr)
        , m_interfaceRoot(nullptr)
    {
        setWindowTitle("PCL Interface Export Tool");
        resize(800, 600);
        
        // Create central widget with instructions
        QWidget* central = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(central);
        
        QString processInfo = processes.isEmpty() ? 
            "" : QString("<li><b>Processes:</b> Select which process to use (%1 available)</li>").arg(processes.size());
        
        QLabel* instructions = new QLabel(
            "<h2>PCL Interface Export Tool</h2>"
            "<p>Select components from the menus to configure and export.</p>"
            "<h3>Available options:</h3>"
            "<ul>" +
            processInfo +
            QString("<li><b>Interfaces:</b> Choose which interface to work with (%1 available)</li>").arg(discovered.size()) +
            "<li><b>Instances:</b> Create new process instances (after selecting interface)</li>"
            "<li><b>Export:</b> Export the current interface to Qt code</li>"
            "</ul>", central);
        instructions->setWordWrap(true);
        instructions->setTextFormat(Qt::RichText);
        layout->addWidget(instructions);
        layout->addStretch();
        
        setCentralWidget(central);
        
        buildMenus();
    }

private:
    void buildMenus()
    {
        QMenuBar* menuBar = this->menuBar();
        
        // Processes Menu - hierarchical: Process > Methods
        if (!m_processes.isEmpty())
        {
            QMenu* processesMenu = menuBar->addMenu("&Processes");
            
            for (int i = 0; i < m_processes.size(); ++i)
            {
                const auto& proc = m_processes[i];
                
                // Extract short name for menu
                std::string shortName = proc.className;
                size_t colonPos = shortName.rfind("::");
                if (colonPos != std::string::npos)
                    shortName = shortName.substr(colonPos + 2);
                
                // Create submenu for this process
                QMenu* processSubmenu = processesMenu->addMenu(QString::fromStdString(shortName));
                
                // Add action to select the process (constructor)
                QAction* selectAction = new QAction("Select this process", processSubmenu);
                selectAction->setData(i);
                selectAction->setCheckable(true);
                QObject::connect(selectAction, &QAction::triggered, 
                               this, &SelectionWindow::onProcessSelected);
                processSubmenu->addAction(selectAction);
                m_processActions.append(selectAction);
                
                processSubmenu->addSeparator();
                
                // Add all methods as informational items
                for (const auto& method : proc.methods)
                {
                    QAction* methodAction = new QAction(QString::fromStdString(method), processSubmenu);
                    methodAction->setEnabled(false);  // Just for display
                    processSubmenu->addAction(methodAction);
                }
                
                if (proc.methods.empty())
                {
                    QAction* noMethods = new QAction("(no methods found)", processSubmenu);
                    noMethods->setEnabled(false);
                    processSubmenu->addAction(noMethods);
                }
            }
        }
        
        // Interfaces Menu
        QMenu* interfacesMenu = menuBar->addMenu("&Interfaces");
        for (size_t i = 0; i < m_discovered.size(); ++i)
        {
            const auto& di = m_discovered[i];
            
            // Extract short name for menu
            std::string shortName = di.className;
            size_t colonPos = shortName.rfind("::");
            if (colonPos != std::string::npos)
                shortName = shortName.substr(colonPos + 2);
            
            QAction* action = new QAction(QString::fromStdString(shortName), this);
            action->setData(static_cast<int>(i));
            QObject::connect(action, &QAction::triggered, 
                           this, &SelectionWindow::onInterfaceSelected);
            interfacesMenu->addAction(action);
        }
        
        // Instances Menu
        m_instancesMenu = menuBar->addMenu("&Instances");
        m_instancesMenu->setEnabled(false);
        
        QAction* createInstance = new QAction("Create New Instance", this);
        QObject::connect(createInstance, &QAction::triggered, 
                       this, &SelectionWindow::onCreateInstance);
        m_instancesMenu->addAction(createInstance);
        
        m_instancesMenu->addSeparator();
        
        // Export Menu
        m_exportMenu = menuBar->addMenu("&Export");
        m_exportMenu->setEnabled(false);
        
        QAction* exportAction = new QAction("Export to Qt Code...", this);
        QObject::connect(exportAction, &QAction::triggered, 
                       this, &SelectionWindow::onExportInterface);
        m_exportMenu->addAction(exportAction);
        
        // Help Menu
        QMenu* helpMenu = menuBar->addMenu("&Help");
        
        QAction* aboutAction = new QAction("About", this);
        QObject::connect(aboutAction, &QAction::triggered, 
                       this, &SelectionWindow::onAbout);
        helpMenu->addAction(aboutAction);
    }

    void onProcessSelected()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (!action) return;
        
        // Uncheck all other process actions
        for (QAction* act : m_processActions)
        {
            if (act != action)
                act->setChecked(false);
        }
        
        int index = action->data().toInt();
        if (index < 0 || index >= m_processes.size()) return;
        
        const ProcessInfo& proc = m_processes[index];
        
        qDebug() << "\n===========================================";
        qDebug() << "Constructing Process:" << proc.className.c_str();
        qDebug() << "===========================================\n";
        
        qDebug() << "Creating process instance...";
        pcl::MetaProcess* mp = reinterpret_cast<pcl::MetaProcess*>(
            dynamicNew(proc.ctorAddress, 512)
        );
        
        if (!mp)
        {
            QMessageBox::critical(this, "Error", 
                QString("Failed to construct process: %1")
                    .arg(QString::fromStdString(proc.className)));
            action->setChecked(false);
            return;
        }
        
        m_selectedProcess = mp;
        
        qDebug() << "✓ Process created:" << (void*)mp;
        
        // Read process metadata after construction
        // Note: Some methods return IsoString, some return uint32, some return String
        QString processId = QString::fromUtf8(mp->Id().c_str());
        
        // Version() returns uint32, not a string
        uint32_t versionNum = mp->Version();
        QString processVersion = QString("0x%1").arg(versionNum, 0, 16);
        
        QString processCategory = QString::fromUtf8(mp->Category().c_str());
        
        // Description() returns String (PCL's UTF-16 string class)
        pcl::String descStr = mp->Description();
        QString processDescription = QString::fromUtf16(
            reinterpret_cast<const char16_t*>(descStr.c_str())
        );
        
        qDebug() << "  ID:          " << processId;
        qDebug() << "  Version:     " << processVersion;
        qDebug() << "  Category:    " << processCategory;
        qDebug() << "  Description: " << processDescription;
        
        // Create a message box that can render HTML
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Process Selected");
        msgBox.setTextFormat(Qt::RichText);  // Enable HTML rendering
        
        QString message = QString(
            "<h3>Successfully selected process:</h3>"
            "<p><b>ID:</b> %1</p>"
            "<p><b>Version:</b> %2</p>"
            "<p><b>Category:</b> %3</p>"
            "<p><b>Description:</b></p>"
            "%4"
        ).arg(processId)
         .arg(processVersion)
         .arg(processCategory.isEmpty() ? "<i>(none)</i>" : processCategory)
         .arg(processDescription);
        
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }

    void onInterfaceSelected()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (!action) return;
        
        int index = action->data().toInt();
        if (index < 0 || index >= static_cast<int>(m_discovered.size())) return;
        
        const DiscoveredInterface& di = m_discovered[index];
        
        qDebug() << "\n===========================================";
        qDebug() << "Selected Interface:" << di.className.c_str();
        qDebug() << "===========================================\n";
        
        if (!constructInterface(di))
        {
            QMessageBox::critical(this, "Error", 
                QString("Failed to construct interface: %1")
                    .arg(QString::fromStdString(di.className)));
            return;
        }
        
        // Enable menus
        m_instancesMenu->setEnabled(m_selectedProcess != nullptr);
        m_exportMenu->setEnabled(true);
        
        QString msg = QString("Successfully loaded: %1\n\n"
                    "You can now:\n");
        if (m_selectedProcess)
            msg += "- Create instances\n";
        else
            msg += "- Select a process from the Processes menu\n";
        msg += "- Export to Qt code";
        
        QMessageBox::information(this, "Interface Loaded", msg);
    }
    
    void onCreateInstance()
    {
        if (!m_selectedProcess)
        {
            QMessageBox::warning(this, "No Process",
                "No process selected. Please select one from the Processes menu first.");
            return;
        }
        
        qDebug() << "\nCreating new process instance...";
        
        pcl::ProcessImplementation* inst = m_selectedProcess->Create();
        if (!inst)
        {
            QMessageBox::critical(this, "Error", 
                "Failed to create process instance!");
            return;
        }
        
        m_processInstance = inst;
        
        qDebug() << "✓ Instance created:" << (void*)inst;
        
        // Add to instances menu
        QAction* instanceAction = new QAction(
            QString("Instance %1").arg(m_instanceActions.size() + 1), 
            this);
        instanceAction->setData(QVariant::fromValue(reinterpret_cast<quintptr>(inst)));
        QObject::connect(instanceAction, &QAction::triggered, 
                       this, &SelectionWindow::onInstanceSelected);
        
        m_instancesMenu->addAction(instanceAction);
        m_instanceActions.append(instanceAction);
        
        // Launch with the interface if available
        if (m_selectedInterface && m_selectedProcess)
        {
            bool dynamic = false;
            unsigned flags = 0;
            m_selectedInterface->Launch(*m_selectedProcess, inst, dynamic, flags);
            m_selectedInterface->Show();
            
            QMessageBox::information(this, "Instance Created",
                QString("Process instance created and launched!\n\n"
                        "Instance: %1")
                    .arg(m_instanceActions.size()));
        }
    }
    
    void onInstanceSelected()
    {
        QAction* action = qobject_cast<QAction*>(sender());
        if (!action) return;
        
        pcl::ProcessImplementation* inst = 
            reinterpret_cast<pcl::ProcessImplementation*>(action->data().toULongLong());
        
        m_processInstance = inst;
        
        qDebug() << "Selected instance:" << (void*)inst;
        
        QMessageBox::information(this, "Instance Selected",
            QString("Selected process instance: %1")
                .arg(action->text()));
    }
    
    void onExportInterface()
    {
        if (!m_selectedInterface)
        {
            QMessageBox::warning(this, "No Interface",
                "Please select an interface first.");
            return;
        }
        
        if (!m_interfaceRoot || !m_interfaceRoot->widget)
        {
            QMessageBox::critical(this, "Error",
                "Interface widget not available!");
            return;
        }
        
        qDebug() << "\n===========================================";
        qDebug() << "Exporting interface...";
        qDebug() << "===========================================\n";
        
        // Wait for UI to initialize
        QApplication::processEvents();
        QThread::msleep(200);
        QApplication::processEvents();
        
        // Find root widget
        QList<QWidget*> candidates;
        candidates.append(m_interfaceRoot->widget);
        
        QWidget* root = RootWidgetSelector::selectBestRoot(candidates, true);
        if (!root)
        {
            QMessageBox::critical(this, "Error",
                "Failed to find root widget for export!");
            return;
        }
        
        // Get base name
        std::string baseName = m_currentClassName;
        size_t colonPos = baseName.rfind("::");
        if (colonPos != std::string::npos)
            baseName = baseName.substr(colonPos + 2);
        
        QString qBaseName = QString::fromStdString(baseName);
        QString outputDir = "./exported";
        
        qDebug() << "Interface:" << qBaseName;
        qDebug() << "Output:   " << outputDir;
        
        bool success = ExportHelper::exportInterface(root, qBaseName, outputDir);
        
        if (success)
        {
            QMessageBox::information(this, "Export Complete",
                QString("Successfully exported interface!\n\n"
                        "Output directory: %1\n\n"
                        "Generated files:\n"
                        "- %2.h\n"
                        "- %2.cpp\n"
                        "- %2_metadata.json\n"
                        "- CMakeLists.txt\n"
                        "- main.cpp\n"
                        "- README.md")
                    .arg(outputDir, qBaseName));
            
            qDebug() << "\n✅ EXPORT COMPLETED SUCCESSFULLY!";
        }
        else
        {
            QMessageBox::critical(this, "Export Failed",
                "Failed to export interface. Check console for details.");
            
            qDebug() << "\n❌ EXPORT FAILED!";
        }
    }
    
    void onAbout()
    {
        QMessageBox::about(this, "About PCL Export Tool",
            "PCL Interface Export Tool\n\n"
            "Automatically discovers PCL processes and interfaces\n"
            "via symbol scanning and exports them to standalone Qt code.\n\n"
            "Features:\n"
            "- Automatic symbol discovery\n"
            "- Interactive interface selection\n"
            "- Process instance management\n"
            "- Qt code export\n\n"
            "No factory functions needed!");
    }

    bool constructInterface(const DiscoveredInterface& di)
    {
        qDebug() << "Resolving interface constructor...";
        void* ifaceCtorAddr = mustResolve(di.mangledCtorName.c_str());
        
        if (!ifaceCtorAddr)
        {
            qCritical() << "❌ Could not resolve constructor!";
            qCritical() << "Symbol:" << di.mangledCtorName.c_str();
            return false;
        }
        
        qDebug() << "✓ Constructor resolved:" << ifaceCtorAddr;
        
        qDebug() << "Constructing interface...";
        m_selectedInterface = reinterpret_cast<pcl::ProcessInterface*>(
            dynamicNew(ifaceCtorAddr, 65536)
        );
        
        if (!m_selectedInterface)
        {
            qCritical() << "❌ Interface construction failed!";
            return false;
        }
        
        qDebug() << "✓ Interface created:" << (void*)m_selectedInterface;
        
        // Set up Qt container
        qDebug() << "Setting up Qt container...";
        
        m_interfaceRoot = new MockBase();
        m_interfaceRoot->isSizer = false;
        m_interfaceRoot->widget = new QWidget(nullptr);
        
        // Extract short name for window title
        std::string shortName = di.className;
        size_t colonPos = shortName.rfind("::");
        if (colonPos != std::string::npos)
            shortName = shortName.substr(colonPos + 2);
        
        m_interfaceRoot->widget->setWindowTitle(QString::fromStdString(shortName));
        g_topLevelWidgets.append(m_interfaceRoot);
        
        // Set interface handle
        m_selectedInterface->handle = (control_handle)m_interfaceRoot;
        
        qDebug() << "✓ Container ready";
        
        // Launch interface
        if (m_selectedProcess)
        {
            qDebug() << "Launching interface with selected process...";
            
            pcl::ProcessImplementation* inst = m_selectedProcess->Create();
            if (!inst)
            {
                qCritical() << "❌ Could not create process instance!";
                return false;
            }
            
            m_processInstance = inst;
            
            bool dynamic = false;
            unsigned flags = 0;
            
            m_selectedInterface->Launch(*m_selectedProcess, inst, dynamic, flags);
            m_selectedInterface->Show();
            
            qDebug() << "✓ Interface launched and shown";
        }
        else
        {
            qWarning() << "Mock mode: Showing interface without process...";
            m_selectedInterface->Show();
            qDebug() << "✓ Interface shown (limited functionality)";
        }
        
        // Store current class name for export
        m_currentClassName = di.className;
        
        // Show the interface widget
        if (m_interfaceRoot && m_interfaceRoot->widget)
        {
            m_interfaceRoot->widget->show();
            m_interfaceRoot->widget->raise();
            m_interfaceRoot->widget->activateWindow();
        }
        
        return true;
    }

private:
    std::vector<DiscoveredInterface> m_discovered;
    QList<ProcessInfo> m_processes;
    
    pcl::MetaProcess* m_selectedProcess;
    pcl::ProcessInterface* m_selectedInterface;
    pcl::ProcessImplementation* m_processInstance;
    MockBase* m_interfaceRoot;
    std::string m_currentClassName;
    
    QMenu* m_instancesMenu;
    QMenu* m_exportMenu;
    QList<QAction*> m_processActions;
    QList<QAction*> m_instanceActions;
};

// ============================================================================
// Main Entry Point
// ============================================================================

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SetDebugLogging(true);

    qDebug() << "===========================================";
    qDebug() << "PCL Interface Export Tool";
    qDebug() << "Interactive Menu Selection";
    qDebug() << "===========================================\n";
    
    const char* exePath = getExecutablePath();
    qDebug() << "Executable:" << exePath << "\n";

    // ========================================================================
    // Step 1: Discover and create Module
    // ========================================================================

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
                dynamicNew(moduleInfo.ctorAddress, 16384)
            );
        }
    }
    
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
    // Step 3: Discover Process classes
    // ========================================================================
    
    qDebug() << "\nScanning for Process classes...";
    auto processes = findProcessClasses();
    
    if (!processes.isEmpty())
    {
        qDebug() << "\n✓ Discovered" << processes.size() << "process(es):";
        for (const auto& proc : processes) {
            qDebug() << "  -" << proc.className.c_str();
        }
    }
    else
    {
        qWarning() << "⚠️  No Process classes found (limited functionality)";
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
        qCritical() << "  nm" << exePath << "| grep Interface";
        QMessageBox::critical(nullptr, "Error", 
            "No PCL interfaces found!\n\n"
            "Make sure the executable contains PCL classes\n"
            "and was built with -Wl,-export_dynamic");
        return 1;
    }

    qDebug() << "\n✓ Discovered" << discovered.size() << "interface(s):";
    for (const auto& di : discovered) {
        qDebug() << "  -" << di.className.c_str();
    }

    // ========================================================================
    // Step 5: Create and show selection window
    // ========================================================================
    
    SelectionWindow* window = new SelectionWindow(discovered, processes);
    window->show();

    qDebug() << "\n===========================================";
    qDebug() << "Selection window ready";
    qDebug() << "===========================================\n";
    qDebug() << "Select components from the menus to begin.\n";

    return app.exec();
}

// ============================================================================
// USAGE NOTES
// ============================================================================
/*

INTERACTIVE MENU-BASED INTERFACE SELECTION
-------------------------------------------

This version discovers all PCL classes via symbol scanning and presents
them in Qt menubars for manual selection.

Menus:
- Processes: Select which process to use (when multiple are available)
- Interfaces: Select which interface to work with
- Instances: Create and manage process instances
- Export: Export the selected interface to Qt code

Your module code needs NO special factory functions!

Just define your classes normally:

    class SandboxModule : public pcl::MetaModule { ... };
    class SandboxProcess : public pcl::MetaProcess { ... };
    class AnotherProcess : public pcl::MetaProcess { ... };  // Multiple processes supported!
    class SandboxInterface : public pcl::ProcessInterface { ... };

Build and run:

    $ make
    $ ./Sandbox-pxm.mock

The tool will:
1. Scan the binary for all available classes
2. Present them in interactive menus
3. Let you select which process and interface to work with
4. Create instances as needed
5. Export selected interfaces to Qt code

Workflow:
1. (Optional) Select a process from the Processes menu
2. Select an interface from the Interfaces menu
3. The interface will launch (with selected process if available)
4. Create additional instances via Instances → Create New Instance
5. Export via Export → Export to Qt Code...

Build requirements:

    - Link with: -Wl,-export_dynamic (for dlsym to work)
    - Keep symbols: Don't use -s flag
    - Debug info helpful: Use -g flag

*/
