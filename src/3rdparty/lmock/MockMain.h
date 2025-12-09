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
#include <QTextEdit>
#include <QDockWidget>
#include <dlfcn.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QFileInfo>

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
#include <pcl/FileFormat.h>
#include <pcl/Exception.h>
#include <pcl/View.h>
#include <pcl/ImageWindow.h>

#include "PCLInterfaceScanner.h"
#include "PCLMockAPI.h"
#include "RootWidgetSelector.h"
#include "ExportHelper.h"
#include "ImageReader.h"

using RawCtor0 = void (*)(void*);

struct ProcessInfo
{
    std::string className;
    std::string mangledCtorName;
    void* ctorAddress;
    std::vector<std::string> methods;  // All discovered methods
};

static inline void* dynamicNew(void* ctorAddr, size_t size)
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

static inline void* mustResolve(const char* name)
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

// ============================================================================
// Console Widget - Captures PCL console output
// ============================================================================

class ConsoleWidget : public QWidget
{
public:
    ConsoleWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        
        m_textEdit = new QTextEdit(this);
        m_textEdit->setReadOnly(true);
        m_textEdit->setFont(QFont("Courier", 10));
        m_textEdit->setLineWrapMode(QTextEdit::NoWrap);
        
        layout->addWidget(m_textEdit);
        
        // Store global instance for PCL callbacks
        s_instance = this;
    }
    
    void appendText(const QString& text)
    {
        m_textEdit->moveCursor(QTextCursor::End);
        m_textEdit->insertPlainText(text);
        m_textEdit->moveCursor(QTextCursor::End);
    }
    
    void clear()
    {
        m_textEdit->clear();
    }
    
    static ConsoleWidget* instance() { return s_instance; }
    
private:
    QTextEdit* m_textEdit;
    static ConsoleWidget* s_instance;
};

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
        
        // Create console dock widget
        m_consoleDock = new QDockWidget("PCL Console", this);
        m_consoleWidget = new ConsoleWidget(m_consoleDock);
        m_consoleDock->setWidget(m_consoleWidget);
        m_consoleDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);
        addDockWidget(Qt::BottomDockWidgetArea, m_consoleDock);
        
        // Console starts visible
        m_consoleDock->setVisible(true);
        
        buildMenus();
    }

private:
    void buildMenus()
    {
        QMenuBar* menuBar = this->menuBar();
	m_fileMenu = menuBar->addMenu("&File");

	m_openAction = new QAction("&Open Image...", this);
	m_openAction->setShortcut(QKeySequence::Open);
	connect(m_openAction, &QAction::triggered, this, &SelectionWindow::onOpenImageFile);
	m_fileMenu->addAction(m_openAction);

	m_fileMenu->addSeparator();

	QAction* exitAction = new QAction("E&xit", this);
	exitAction->setShortcut(QKeySequence::Quit);
	connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
	m_fileMenu->addAction(exitAction);

	// ADD VIEW MENU
	m_viewMenu = menuBar->addMenu("&View");

	m_zoomInAction = new QAction("Zoom &In", this);
	m_zoomInAction->setShortcut(QKeySequence::ZoomIn);  // Ctrl/Cmd + Plus
	// m_zoomInAction->setEnabled(false);  // Enable when window is active
	connect(m_zoomInAction, &QAction::triggered, this, &SelectionWindow::onZoomIn);
	m_viewMenu->addAction(m_zoomInAction);

	m_zoomOutAction = new QAction("Zoom &Out", this);
	m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);  // Ctrl/Cmd + Minus
	// m_zoomOutAction->setEnabled(false);
	connect(m_zoomOutAction, &QAction::triggered, this, &SelectionWindow::onZoomOut);
	m_viewMenu->addAction(m_zoomOutAction);

	m_zoom11Action = new QAction("&Actual Size (1:1)", this);
	m_zoom11Action->setShortcut(Qt::CTRL | Qt::Key_0);  // Ctrl/Cmd + 0
	// m_zoom11Action->setEnabled(false);
	connect(m_zoom11Action, &QAction::triggered, this, &SelectionWindow::onZoom11);
	m_viewMenu->addAction(m_zoom11Action);

	m_zoomToFitAction = new QAction("Zoom to &Fit", this);
	m_zoomToFitAction->setShortcut(Qt::CTRL | Qt::Key_F);  // Ctrl/Cmd + F
	// m_zoomToFitAction->setEnabled(false);
	connect(m_zoomToFitAction, &QAction::triggered, this, &SelectionWindow::onZoomToFit);
	m_viewMenu->addAction(m_zoomToFitAction);
	
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
        
        // Tools Menu
        QMenu* toolsMenu = menuBar->addMenu("&Tools");
        
        // Global Execution submenu
        QMenu* globalExecMenu = toolsMenu->addMenu("Global Execution");
        
        QAction* execGlobalAction = new QAction("ExecuteGlobal()", this);
        QObject::connect(execGlobalAction, &QAction::triggered,
                       this, &SelectionWindow::onExecuteGlobal);
        globalExecMenu->addAction(execGlobalAction);
        
        QAction* execGlobalOnCurrentViewAction = new QAction("ExecuteOn(View&)", this);
        QObject::connect(execGlobalOnCurrentViewAction, &QAction::triggered,
                       this, &SelectionWindow::onExecuteGlobalOnCurrentView);
        globalExecMenu->addAction(execGlobalOnCurrentViewAction);
        
        QAction* execOnImageAction = new QAction("ExecuteOn(ImageVariant&, hints)", this);
        QObject::connect(execOnImageAction, &QAction::triggered,
                       this, &SelectionWindow::onExecuteOnImage);
        globalExecMenu->addAction(execOnImageAction);
        
        globalExecMenu->addSeparator();
        
        QAction* createTestImageAction = new QAction("Create Test Image Window", this);
        QObject::connect(createTestImageAction, &QAction::triggered,
                       this, &SelectionWindow::onCreateTestImage);
        globalExecMenu->addAction(createTestImageAction);
        
        // File Format Loaders submenu
        QMenu* fileFormatMenu = toolsMenu->addMenu("File Format Loaders");
        
        QAction* testFITSAction = new QAction("Test FITS Loader", this);
        QObject::connect(testFITSAction, &QAction::triggered,
                       this, &SelectionWindow::onTestFITSLoader);
        fileFormatMenu->addAction(testFITSAction);
        
        QAction* testTIFFAction = new QAction("Test TIFF Loader", this);
        QObject::connect(testTIFFAction, &QAction::triggered,
                       this, &SelectionWindow::onTestTIFFLoader);
        fileFormatMenu->addAction(testTIFFAction);
        
        QAction* testXISFAction = new QAction("Test XISF Loader", this);
        QObject::connect(testXISFAction, &QAction::triggered,
                       this, &SelectionWindow::onTestXISFLoader);
        fileFormatMenu->addAction(testXISFAction);
        
        QAction* testAllFormatsAction = new QAction("Test All Loaders", this);
        QObject::connect(testAllFormatsAction, &QAction::triggered,
                       this, &SelectionWindow::onTestAllLoaders);
        fileFormatMenu->addAction(testAllFormatsAction);
        
        toolsMenu->addSeparator();
        
        QAction* showProcessInfoAction = new QAction("Show Current Process Info", this);
        QObject::connect(showProcessInfoAction, &QAction::triggered,
                       this, &SelectionWindow::onShowProcessInfo);
        toolsMenu->addAction(showProcessInfoAction);
        
        toolsMenu->addSeparator();
        
        QAction* testConsoleAction = new QAction("Test Console Output", this);
        QObject::connect(testConsoleAction, &QAction::triggered,
                       this, &SelectionWindow::onTestConsole);
        toolsMenu->addAction(testConsoleAction);
        
        // View Menu
        QMenu* viewMenu = menuBar->addMenu("&View");
        
        QAction* toggleConsoleAction = new QAction("Show/Hide Console", this);
        toggleConsoleAction->setCheckable(true);
        toggleConsoleAction->setChecked(true);
        QObject::connect(toggleConsoleAction, &QAction::triggered,
                       this, &SelectionWindow::onToggleConsole);
        viewMenu->addAction(toggleConsoleAction);
        
        QAction* clearConsoleAction = new QAction("Clear Console", this);
        QObject::connect(clearConsoleAction, &QAction::triggered,
                       this, &SelectionWindow::onClearConsole);
        viewMenu->addAction(clearConsoleAction);
        
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
    
    void onExecuteGlobal()
    {
        if (!m_selectedProcess)
        {
            QMessageBox::warning(this, "No Process",
                "Please select a process first from the Processes menu.");
            return;
        }
        
        if (!m_processInstance)
        {
            QMessageBox::warning(this, "No Instance",
                "Please create a process instance first from the Instances menu.");
            return;
        }
        
        qDebug() << "\n===========================================";
        qDebug() << "Testing ExecuteGlobal()";
        qDebug() << "===========================================\n";
        
        // ExecuteGlobal is a virtual method that must be implemented by the concrete
        // process implementation class. We need to get the concrete type.
        // The m_processInstance was created by MetaProcess::Create() which returns
        // the correct concrete type, but we need to call ExecuteGlobal on it.
        
        try {
            // Try to find the ExecuteGlobal method dynamically
            // Get the class name from the process
            QString processId = QString::fromUtf8(m_selectedProcess->Id().c_str());
            
            qDebug() << "Process ID:" << processId;
            qDebug() << "Instance type:" << typeid(*m_processInstance).name();
            
            // The instance is the concrete implementation created by MetaProcess::Create()
            // We can safely call ExecuteGlobal on it - it should be overridden
            bool result = m_processInstance->ExecuteGlobal();
            
            QMessageBox::information(this, "ExecuteGlobal Result",
                QString("ExecuteGlobal() completed.\n\n"
                        "Result: %1")
                    .arg(result ? "Success (true)" : "Failed (false)"));
            
            qDebug() << "✓ ExecuteGlobal() returned:" << result;
        }
        catch (const pcl::Exception& e)
        {
            QString errorMsg = QString::fromUtf16(
                reinterpret_cast<const char16_t*>(e.Message().c_str())
            );
            
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("PCL Error");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(QString(
                "<h3>ExecuteGlobal() Error</h3>"
                "<p><b>PCL Error:</b></p>"
                "<p>%1</p>"
                "<hr>"
                "<p><i>Note: ExecuteGlobal() must be reimplemented in your "
                "process implementation class (descendant of ProcessImplementation).</i></p>"
            ).arg(errorMsg));
            msgBox.exec();
            
            qDebug() << "❌ PCL Error:" << errorMsg;
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Error",
                QString("ExecuteGlobal() threw exception:\n%1")
                    .arg(e.what()));
            qDebug() << "❌ Exception:" << e.what();
        }
    }
    
    void onExecuteGlobalOnCurrentView()
    {
        if (!m_selectedProcess)
        {
            QMessageBox::warning(this, "No Process",
                "Please select a process first from the Processes menu.");
            return;
        }
        
        if (!m_processInstance)
        {
            QMessageBox::warning(this, "No Instance",
                "Please create a process instance first from the Instances menu.");
            return;
        }
        
        qDebug() << "\n===========================================";
        qDebug() << "Testing ExecuteOn(View&)";
        qDebug() << "===========================================\n";
        
        try {
            // Get all open image windows
            pcl::Array<pcl::ImageWindow> windows = pcl::ImageWindow::AllWindows();
            
            if (windows.IsEmpty())
            {
                QMessageBox::information(this, "No Images",
                    "No image windows are currently open.\n\n"
                    "To test ExecuteOn(View&), you need to:\n"
                    "1. Load an image file (FITS, TIFF, or XISF)\n"
                    "2. Or create a test image window\n\n"
                    "For now, this is just a placeholder.");
                
                qDebug() << "ℹ No image windows available";
                return;
            }
            
            // Get the main view of the first window
            pcl::ImageWindow& window = windows[0];
            pcl::View view = window.MainView();
            
            QString windowId = QString::fromUtf8(window.MainView().Id().c_str());
            qDebug() << "Executing on view:" << windowId;
            
            bool result = m_processInstance->ExecuteOn(view);
            
            QMessageBox::information(this, "ExecuteOn Result",
                QString("ExecuteOn(View&) completed.\n\n"
                        "View: %1\n"
                        "Result: %2")
                    .arg(windowId)
                    .arg(result ? "Success (true)" : "Failed (false)"));
            
            qDebug() << "✓ ExecuteOn() returned:" << result;
        }
        catch (const pcl::Exception& e)
        {
            QString errorMsg = QString::fromUtf16(
                reinterpret_cast<const char16_t*>(e.Message().c_str())
            );
            
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("PCL Error");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(QString(
                "<h3>ExecuteOn(View&) Error</h3>"
                "<p><b>PCL Error:</b></p>"
                "<p>%1</p>"
                "<hr>"
                "<p><i>Note: ExecuteOn() must be reimplemented in your "
                "process implementation class if it processes images.</i></p>"
            ).arg(errorMsg));
            msgBox.exec();
            
            qDebug() << "❌ PCL Error:" << errorMsg;
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Error",
                QString("ExecuteOn() threw exception:\n%1")
                    .arg(e.what()));
            qDebug() << "❌ Exception:" << e.what();
        }
    }
    
    void onExecuteOnImage()
    {
        if (!m_selectedProcess)
        {
            QMessageBox::warning(this, "No Process",
                "Please select a process first from the Processes menu.");
            return;
        }
        
        if (!m_processInstance)
        {
            QMessageBox::warning(this, "No Instance",
                "Please create a process instance first from the Instances menu.");
            return;
        }
        
        qDebug() << "\n===========================================";
        qDebug() << "Testing ExecuteOn(ImageVariant&, hints)";
        qDebug() << "===========================================\n";
        
        try {
            // Get all open image windows
            pcl::Array<pcl::ImageWindow> windows = pcl::ImageWindow::AllWindows();
            
            if (windows.IsEmpty())
            {
                QMessageBox::information(this, "No Images",
                    "No image windows are currently open.\n\n"
                    "To test ExecuteOn(ImageVariant&), you need to:\n"
                    "1. Create a test image via 'Create Test Image Window'\n"
                    "2. Or load an image file (FITS, TIFF, or XISF)");
                
                qDebug() << "ℹ No image windows available";
                return;
            }
            
            // Get the main view of the first window and its image
            pcl::ImageWindow& window = windows[0];
            pcl::View view = window.MainView();
            pcl::ImageVariant image = view.Image();
            
            QString windowId = QString::fromUtf8(view.Id().c_str());
            qDebug() << "Executing on image from view:" << windowId;
            
            // Execute with empty hints string
            pcl::IsoString hints;
            bool result = m_processInstance->ExecuteOn(image, hints);
            
            QMessageBox::information(this, "ExecuteOn Result",
                QString("ExecuteOn(ImageVariant&, hints) completed.\n\n"
                        "View: %1\n"
                        "Result: %2")
                    .arg(windowId)
                    .arg(result ? "Success (true)" : "Failed (false)"));
            
            qDebug() << "✓ ExecuteOn(ImageVariant&) returned:" << result;
        }
        catch (const pcl::Exception& e)
        {
            QString errorMsg = QString::fromUtf16(
                reinterpret_cast<const char16_t*>(e.Message().c_str())
            );
            
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("PCL Error");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(QString(
                "<h3>ExecuteOn(ImageVariant&) Error</h3>"
                "<p><b>PCL Error:</b></p>"
                "<p>%1</p>"
                "<hr>"
                "<p><i>Note: ExecuteOn() must be reimplemented in your "
                "process implementation class if it processes images.</i></p>"
            ).arg(errorMsg));
            msgBox.exec();
            
            qDebug() << "❌ PCL Error:" << errorMsg;
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Error",
                QString("ExecuteOn() threw exception:\n%1")
                    .arg(e.what()));
            qDebug() << "❌ Exception:" << e.what();
        }
    }
    
    void onShowProcessInfo()
    {
        if (!m_selectedProcess)
        {
            QMessageBox::warning(this, "No Process",
                "No process currently selected.");
            return;
        }
        
        QString processId = QString::fromUtf8(m_selectedProcess->Id().c_str());
        uint32_t versionNum = m_selectedProcess->Version();
        QString processVersion = QString("0x%1").arg(versionNum, 0, 16);
        QString processCategory = QString::fromUtf8(m_selectedProcess->Category().c_str());
        pcl::String descStr = m_selectedProcess->Description();
        QString processDescription = QString::fromUtf16(
            reinterpret_cast<const char16_t*>(descStr.c_str())
        );
        
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Current Process Information");
        msgBox.setTextFormat(Qt::RichText);
        
        QString message = QString(
            "<h3>Current Process:</h3>"
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
    
    void onToggleConsole()
    {
        if (m_consoleDock)
        {
            m_consoleDock->setVisible(!m_consoleDock->isVisible());
        }
    }
    
    void onClearConsole()
    {
        if (m_consoleWidget)
        {
            m_consoleWidget->clear();
        }
    }
    
    void onTestConsole()
    {
        if (!m_consoleWidget)
        {
            QMessageBox::warning(this, "No Console",
                "Console widget not available.");
            return;
        }
        
        // Test the console with sample output
        m_consoleWidget->appendText("===========================================\n");
        m_consoleWidget->appendText("Console Output Test\n");
        m_consoleWidget->appendText("===========================================\n");
        m_consoleWidget->appendText("This is a test of the console output system.\n");
        m_consoleWidget->appendText("\n");
        m_consoleWidget->appendText("Features:\n");
        m_consoleWidget->appendText("  - Scrollable text area\n");
        m_consoleWidget->appendText("  - Monospace font for alignment\n");
        m_consoleWidget->appendText("  - Captures PCL Console::Write() calls\n");
        m_consoleWidget->appendText("  - Dockable window (bottom or right)\n");
        m_consoleWidget->appendText("\n");
        m_consoleWidget->appendText("To integrate with PCL, see:\n");
        m_consoleWidget->appendText("  PCLMockAPI_ConsoleIntegration.cpp\n");
        m_consoleWidget->appendText("\n");
        m_consoleWidget->appendText("Test completed successfully!\n");
        m_consoleWidget->appendText("===========================================\n");
        
        QMessageBox::information(this, "Console Test",
            "Test output has been written to the console.\n\n"
            "Check the console window at the bottom of the main window.");
    }
    
    void onCreateTestImage()
    {
        qDebug() << "\n===========================================";
        qDebug() << "Creating test image window";
        qDebug() << "===========================================\n";
        
        if (m_consoleWidget)
        {
            m_consoleWidget->appendText("Creating 512x512 test image...\n");
        }
        
        try {
            // This calls: ImageWindowContext::CreateImageWindow() via the API
            // The constructor signature is:
            // ImageWindow(width, height, channels, bitsPerSample, floatSample, color, initialProcessing, id)
            
            pcl::ImageWindow window(
                512,                      // width
                512,                      // height
                1,                        // numberOfChannels (grayscale)
                32,                       // bitsPerSample
                true,                     // floatSample
                false,                    // color
                true,                     // initialProcessing
                pcl::IsoString("TestImage") // id
            );
            
            if (m_consoleWidget)
            {
                m_consoleWidget->appendText("✓ ImageWindow created successfully\n");
                m_consoleWidget->appendText("  ID: TestImage\n");
                m_consoleWidget->appendText("  Size: 512x512\n");
                m_consoleWidget->appendText("  Type: 32-bit float, grayscale\n");
            }
            
            // Get the main view and its image
	    pcl::View view = window.MainView();

	    // Get the view's image handle
	    void* viewHandle = view.handle;  // or view.Handle()

	    // Call the API directly to get the image handle
	    image_handle imgHandle = API->View->GetViewImage(viewHandle);

	    if (m_consoleWidget)
	    {
		m_consoleWidget->appendText(QString("View handle: %1\n").arg((quintptr)viewHandle));
		m_consoleWidget->appendText(QString("Image handle: %1\n").arg((quintptr)imgHandle));
	    }

	    if (imgHandle != nullptr)
	    {
		// Get image format directly
		uint32_t bits = 0;
		api_bool isFloat = api_false;

		if (API->SharedImage->GetImageFormat(imgHandle, &bits, &isFloat))
		{
		    if (m_consoleWidget)
		    {
			m_consoleWidget->appendText(QString("Image format: %1-bit %2\n")
			    .arg(bits).arg(isFloat ? "float" : "integer"));
		    }

		    if (isFloat && bits == 32)
		    {
			// Get pixel data
			void** channelData = nullptr;
			if (API->SharedImage->GetImagePixelData(imgHandle, &channelData) && channelData)
			{
			    // Get geometry
			    uint32_t width = 0, height = 0, channels = 0;
			    API->SharedImage->GetImageGeometry(imgHandle, &width, &height, &channels);

			    if (m_consoleWidget)
			    {
				m_consoleWidget->appendText(QString("Image size: %1x%2, %3 channels\n")
				    .arg(width).arg(height).arg(channels));
				m_consoleWidget->appendText("Filling with gradient pattern...\n");
			    }

			    // Fill with gradient
			    float* pixels = static_cast<float*>(channelData[0]);

			    for (uint32_t y = 0; y < height; ++y)
			    {
				for (uint32_t x = 0; x < width; ++x)
				{
				    float value = (float)x / (width - 1.0f) * (float)y / (height - 1.0f);
				    pixels[y * width + x] = value;
				}
			    }

			    if (m_consoleWidget)
			    {
				m_consoleWidget->appendText("✓ Gradient pattern created!\n");
				m_consoleWidget->appendText(QString("  Samples: [0,0]=%1, [255,255]=%2, [511,511]=%3\n")
				    .arg(pixels[0])
				    .arg(pixels[255 * width + 255])
				    .arg(pixels[(width * height) - 1]));
			    }
			}
			else
			{
			    if (m_consoleWidget)
			    {
				m_consoleWidget->appendText("✗ Failed to get pixel data!\n");
			    }
			}
		    }
		    else
		    {
			if (m_consoleWidget)
			{
			    m_consoleWidget->appendText("✗ Image is not 32-bit float!\n");
			}
		    }
		}
		else
		{
		    if (m_consoleWidget)
		    {
			m_consoleWidget->appendText("✗ GetImageFormat failed!\n");
		    }
		}
	    }
	    else
	    {
		if (m_consoleWidget)
		{
		    m_consoleWidget->appendText("✗ Failed to get image handle from view!\n");
		}
	    }            
            if (view.IsNull())
            {
                throw pcl::Error("Failed to get main view from window");
            }
            
            // Get the image variant
            pcl::ImageVariant image = view.Image();

	    qDebug() << "\n=== DIAGNOSTIC: Testing pixel access ===";
	    qDebug() << "ImageVariant obtained";
	    qDebug() << "  IsComplexSample:" << image.IsComplexSample();
	    qDebug() << "  IsFloatSample:" << image.IsFloatSample();
	    qDebug() << "  BitsPerSample:" << image.BitsPerSample();

	    try {
		if (image.IsFloatSample())
		{
		    qDebug() << "Image IS float sample, proceeding...";

		    pcl::Image& img = static_cast<pcl::Image&>(*image);

		    qDebug() << "Cast successful";
		    qDebug() << "  Width:" << img.Width();
		    qDebug() << "  Height:" << img.Height();
		    qDebug() << "  Channels:" << img.NumberOfChannels();

		    // Method 1: Try Pixel() accessor
		    qDebug() << "\nMethod 1: Pixel() accessor";
		    try {
			qDebug() << "  Reading pixel [0,0]:" << img.Pixel(0, 0, 0);

			qDebug() << "  Writing 0.5 to pixel [10,10]...";
			img.Pixel(10, 10, 0) = 0.5f;

			qDebug() << "  Reading back pixel [10,10]:" << img.Pixel(10, 10, 0);
		    } catch (const std::exception& e) {
			qDebug() << "  Exception in Pixel() test:" << e.what();
		    } catch (...) {
			qDebug() << "  Unknown exception in Pixel() test";
		    }

		    // Method 2: Try PixelData()
		    qDebug() << "\nMethod 2: PixelData()";
		    try {
			float* rawPtr = img.PixelData(0);
			qDebug() << "  PixelData(0) returned:" << rawPtr;

			if (rawPtr) {
			    qDebug() << "  rawPtr[0]:" << rawPtr[0];
			    qDebug() << "  rawPtr[100]:" << rawPtr[100];

			    qDebug() << "  Writing 0.75 to rawPtr[100]...";
			    rawPtr[100] = 0.75f;

			    qDebug() << "  Reading back rawPtr[100]:" << rawPtr[100];
			    qDebug() << "  Reading via Pixel(100, 0):" << img.Pixel(100, 0, 0);
			} else {
			    qDebug() << "  ERROR: PixelData() returned NULL!";
			}
		    } catch (const std::exception& e) {
			qDebug() << "  Exception in PixelData() test:" << e.what();
		    } catch (...) {
			qDebug() << "  Unknown exception in PixelData() test";
		    }
		}
		else
		{
		    qDebug() << "Image is NOT float sample!";
		    qDebug() << "  This is why the if block doesn't run";
		}
	    } catch (const pcl::Exception& e) {
		qDebug() << "PCL Exception:" << QString::fromUtf16(
		    reinterpret_cast<const char16_t*>(e.Message().c_str()));
	    } catch (const std::exception& e) {
		qDebug() << "std::exception:" << e.what();
	    } catch (...) {
		qDebug() << "Unknown exception caught";
	    }

	    qDebug() << "=== END DIAGNOSTIC ===\n";
            if (image.IsFloatSample())
            {
                // Cast to float image and fill with gradient pattern
                pcl::Image& img = static_cast<pcl::Image&>(*image);
                
                if (m_consoleWidget)
                {
                    m_consoleWidget->appendText("Filling with gradient pattern...\n");
                }
                
                // Create a gradient pattern: value = (x/width) * (y/height)
                for (int y = 0; y < 512; ++y)
                {
                    for (int x = 0; x < 512; ++x)
                    {
                        float value = (float)x / 511.0f * (float)y / 511.0f;
                        img.Pixel(x, y, 0) = value;
                    }
                }
                
                if (m_consoleWidget)
                {
                    m_consoleWidget->appendText("✓ Pattern created\n");
                }
            }
            
            // Show and zoom the window
            window.Show();
            window.ZoomToFit();
            
            if (m_consoleWidget)
            {
                m_consoleWidget->appendText("✓ Window displayed\n");
            }
            
            QMessageBox::information(this, "Test Image Created",
                "A 512x512 test image has been created.\n\n"
                "ID: TestImage\n"
                "Type: 32-bit float, grayscale\n"
                "Pattern: Gradient (x * y)\n\n"
                "You can now test ExecuteOn(View&) and\n"
                "ExecuteOn(ImageVariant&) with this image.\n\n"
                "Note: The window is managed by ImageWindowContext\n"
                "mock implementations in PCLMockAPI.cpp");
            
            qDebug() << "✓ Test image window created successfully";
        }
        catch (const pcl::Exception& e)
        {
            QString errorMsg = QString::fromUtf16(
                reinterpret_cast<const char16_t*>(e.Message().c_str())
            );
            
            if (m_consoleWidget)
            {
                m_consoleWidget->appendText("❌ Failed to create test image\n");
                m_consoleWidget->appendText(QString("   Error: %1\n").arg(errorMsg));
            }
            
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Image Creation Failed");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(QString(
                "<h3>Test Image Creation Failed</h3>"
                "<p><b>Error:</b> %1</p>"
                "<hr>"
                "<p><b>This means ImageWindowContext::CreateImageWindow() needs implementation.</b></p>"
                "<p>To fix this:</p>"
                "<ol>"
                "<li>Implement <b>CreateImageWindow()</b> in PCLMockAPI.cpp</li>"
                "<li>Return a valid window_handle (can be a MockBase* or similar)</li>"
                "<li>See <b>ImageWindowMock_Implementation.cpp</b> for a complete example</li>"
                "</ol>"
                "<p>Key functions to implement:</p>"
                "<ul>"
                "<li>CreateImageWindow() - Must return non-null handle</li>"
                "<li>GetImageWindowMainView() - Return a view_handle</li>"
                "<li>ViewContext::GetViewImage() - Return image data</li>"
                "</ul>"
            ).arg(errorMsg));
            msgBox.exec();
            
            qDebug() << "❌ PCL Error:" << errorMsg;
        }
        catch (const std::exception& e)
        {
            if (m_consoleWidget)
            {
                m_consoleWidget->appendText("❌ Exception during image creation\n");
                m_consoleWidget->appendText(QString("   %1\n").arg(e.what()));
            }
            
            QMessageBox::critical(this, "Error",
                QString("Failed to create test image:\n%1").arg(e.what()));
            qDebug() << "❌ Exception:" << e.what();
        }
    }
    
    void onTestFITSLoader()
    {
        qDebug() << "\n===========================================";
        qDebug() << "Testing FITS Loader";
        qDebug() << "===========================================\n";
        
        QString result = testFileFormatLoader("FITS", "fits");
        
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("FITS Loader Test");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(result);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
    
    void onTestTIFFLoader()
    {
        qDebug() << "\n===========================================";
        qDebug() << "Testing TIFF Loader";
        qDebug() << "===========================================\n";
        
        QString result = testFileFormatLoader("TIFF", "tif");
        
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("TIFF Loader Test");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(result);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
    
    void onTestXISFLoader()
    {
        qDebug() << "\n===========================================";
        qDebug() << "Testing XISF Loader";
        qDebug() << "===========================================\n";
        
        QString result = testFileFormatLoader("XISF", "xisf");
        
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("XISF Loader Test");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(result);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
    
    void onTestAllLoaders()
    {
        qDebug() << "\n===========================================";
        qDebug() << "Testing All File Format Loaders";
        qDebug() << "===========================================\n";
        
        QString fitsResult = testFileFormatLoader("FITS", "fits");
        QString tiffResult = testFileFormatLoader("TIFF", "tif");
        QString xisfResult = testFileFormatLoader("XISF", "xisf");
        
        QString combined = QString(
            "<h3>File Format Loader Tests</h3>"
            "<hr>"
            "%1"
            "<hr>"
            "%2"
            "<hr>"
            "%3"
        ).arg(fitsResult).arg(tiffResult).arg(xisfResult);
        
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("All Loader Tests");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(combined);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
    
    QString testFileFormatLoader(const QString& formatName, const QString& extension)
    {
        qDebug() << "Testing" << formatName << "loader...";
        
        QString result = QString("<h4>%1 Loader Test</h4>").arg(formatName);
        
        try {
            // Try to find the file format in PCL's registry
            const pcl::FileFormat* foundFormat = nullptr;
            
            // Search through all file formats
            pcl::Array<pcl::FileFormat> formats = pcl::FileFormat::AllFormats();
            
            qDebug() << "  Searching through" << formats.Length() << "registered formats...";
            
            for (const pcl::FileFormat& fmt : formats)
            {
                pcl::StringList extensions = fmt.FileExtensions();
                for (const pcl::String& ext : extensions)
                {
                    pcl::IsoString isoExt = ext.ToIsoString();
                    if (isoExt.CompareIC(extension.toUtf8().constData()) == 0)
                    {
                        foundFormat = &fmt;
                        break;
                    }
                }
                if (foundFormat)
                    break;
            }
            
            if (foundFormat)
            {
                result += QString("<p><b>✓ Format found:</b> %1</p>")
                    .arg(QString::fromUtf8(foundFormat->Name().c_str()));
                
                qDebug() << "  ✓ Found format:" << foundFormat->Name().c_str();
                
                // Get format information
                pcl::StringList extensions = foundFormat->FileExtensions();
                result += "<p><b>Extensions:</b> ";
                QStringList extList;
                for (const pcl::String& ext : extensions)
                {
                    extList << QString::fromUtf16(reinterpret_cast<const char16_t*>(ext.c_str()));
                }
                result += extList.join(", ") + "</p>";
                
                result += QString("<p><b>Can read:</b> %1</p>")
                    .arg(foundFormat->CanRead() ? "Yes" : "No");
                result += QString("<p><b>Can write:</b> %1</p>")
                    .arg(foundFormat->CanWrite() ? "Yes" : "No");
                result += QString("<p><b>Can read incrementally:</b> %1</p>")
                    .arg(foundFormat->CanReadIncrementally() ? "Yes" : "No");
                result += QString("<p><b>Can write incrementally:</b> %1</p>")
                    .arg(foundFormat->CanWriteIncrementally() ? "Yes" : "No");
                
                qDebug() << "  Can read:" << foundFormat->CanRead();
                qDebug() << "  Can write:" << foundFormat->CanWrite();
                qDebug() << "  Extensions:" << extList.join(", ");
            }
            else
            {
                result += QString("<p><b>❌ Format not found</b></p>");
                result += QString("<p>Extension '.%1' is not registered in PCL.</p>").arg(extension);
                result += QString("<p>Available formats: %1</p>").arg(formats.Length());
                
                qDebug() << "  ❌ Format not found for extension:" << extension;
                qDebug() << "  Available formats:" << formats.Length();
            }
        }
        catch (const std::exception& e)
        {
            result += QString("<p><b>❌ Exception:</b> %1</p>").arg(e.what());
            qDebug() << "  ❌ Exception:" << e.what();
        }
        catch (...)
        {
            result += "<p><b>❌ Unknown exception occurred</b></p>";
            qDebug() << "  ❌ Unknown exception";
        }
        
        return result;
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

private slots:
    void onOpenImageFile();      // NEW
    void onZoomIn();
    void onZoomOut();
    void onZoom11();
    void onZoomToFit();
    
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
    
    QDockWidget* m_consoleDock;
    ConsoleWidget* m_consoleWidget;
    QMenu* m_fileMenu;           // NEW (or find if it exists)
    QMenu* m_viewMenu;
    
    QAction* m_openAction;       // NEW
    QAction* m_zoomInAction;
    QAction* m_zoomOutAction;
    QAction* m_zoom11Action;
    QAction* m_zoomToFitAction;

};
