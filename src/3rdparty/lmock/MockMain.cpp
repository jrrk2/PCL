// MockMain.cpp
// PCL interface export tool with interactive menu selection
// Discovers all available processes and interfaces via symbol scanning
// and lets you select them manually from Qt menubars

#include "MockMain.h"

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

ConsoleWidget* ConsoleWidget::s_instance = nullptr;

// Helper function for PCLMockAPI to write to console
extern "C" void MockMainWriteConsole(const pcl::char16_type* text, bool appendNewline)
{
    if (ConsoleWidget::instance())
    {
        QString qtext = QString::fromUtf16(reinterpret_cast<const char16_t*>(text));
        if (appendNewline)
            qtext += "\n";
        ConsoleWidget::instance()->appendText(qtext);
    }
}

void SelectionWindow::onOpenImageFile()
{
    if (m_consoleWidget)
    {
        m_consoleWidget->appendText("\n===========================================\n");
        m_consoleWidget->appendText("Opening image file\n");
        m_consoleWidget->appendText("===========================================\n");
    }
    
    // Show file dialog
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open Image File",
        QString(),
        "Image Files (*.xisf *.fits *.fit *.fts *.tiff *.tif);;XISF Files (*.xisf);;FITS Files (*.fits *.fit *.fts);;TIFF Files (*.tiff *.tif);;All Files (*)"
    );
    
    if (fileName.isEmpty())
    {
        if (m_consoleWidget)
            m_consoleWidget->appendText("✗ File selection cancelled\n");
        return;
    }
    
    if (m_consoleWidget)
    {
        m_consoleWidget->appendText(QString("Selected file: %1\n").arg(fileName));
    }
    
    // Show progress dialog
    QProgressDialog progress("Loading image...", "Cancel", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(500);
    progress.setValue(10);
    
    // Read the image
    ImageReader reader;
    progress.setValue(30);
    QApplication::processEvents();
    
    if (!reader.readFile(fileName))
    {
        progress.close();
        QMessageBox::critical(this, "Image Load Error", 
            QString("Failed to load image:\n\n%1").arg(reader.lastError()));
        
        if (m_consoleWidget)
        {
            m_consoleWidget->appendText(QString("✗ Failed: %1\n").arg(reader.lastError()));
        }
        return;
    }
    
    progress.setValue(60);
    const ImageData& imgData = reader.imageData();
    
    if (m_consoleWidget)
    {
        m_consoleWidget->appendText("✓ Image loaded successfully\n");
        m_consoleWidget->appendText(QString("  %1 × %2 × %3 channels\n")
            .arg(imgData.width).arg(imgData.height).arg(imgData.channels));
    }
    
    // Create image window
    try
    {
        QFileInfo fileInfo(fileName);
        QString windowId = fileInfo.baseName();
        
        if (m_consoleWidget)
        {
            m_consoleWidget->appendText(QString("Creating window '%1'...\n").arg(windowId));
        }
        
        pcl::ImageWindow window(
            imgData.width,
            imgData.height,
            imgData.channels,
            32, true,  // 32-bit float
            imgData.channels >= 3,  // color
            true,
            windowId.toUtf8().constData()
        );
        
        progress.setValue(80);
        
        // Get image handle
        pcl::View view = window.MainView();
        image_handle imgHandle = API->View->GetViewImage(view.handle);
        
        if (imgHandle && API && API->SharedImage)
        {
            void** channelData = nullptr;
            if (API->SharedImage->GetImagePixelData(imgHandle, &channelData) && channelData)
            {
		// Copy pixels - both ImageReader and PCL use planar format
		const float* src = imgData.pixels.constData();

		if (m_consoleWidget)
		{
		    m_consoleWidget->appendText("Copying pixel data (planar → planar)...\n");
		}

		size_t pixelsPerChannel = imgData.width * imgData.height;

		for (int c = 0; c < imgData.channels; ++c)
		{
		    float* dst = static_cast<float*>(channelData[c]);

		    // Source is also planar, so just copy each channel
		    const float* srcChannel = src + (c * pixelsPerChannel);

		    memcpy(dst, srcChannel, pixelsPerChannel * sizeof(float));

		    if (m_consoleWidget && c == 0)
		    {
			m_consoleWidget->appendText(QString("  Sample pixel [0,0] channel %1: %2\n")
			    .arg(c).arg(srcChannel[0]));
		    }
		}                
                if (m_consoleWidget)
                {
                    m_consoleWidget->appendText("✓ Pixel data copied\n");
                }
            }
        }
        
        window.Show();
        window.ZoomToFit();
        
        progress.setValue(100);
        
        if (m_consoleWidget)
        {
            m_consoleWidget->appendText("✓ Window displayed\n");
            m_consoleWidget->appendText("===========================================\n\n");
        }
    }
    catch (const pcl::Exception& e)
    {
        progress.close();
        QString errorMsg = QString::fromUtf16(
            reinterpret_cast<const char16_t*>(e.Message().c_str()));
        QMessageBox::critical(this, "Error", errorMsg);
        
        if (m_consoleWidget)
        {
            m_consoleWidget->appendText(QString("✗ PCL Error: %1\n").arg(errorMsg));
        }
    }
}

void SelectionWindow::onZoomIn()
{
    if (m_consoleWidget)
    {
        m_consoleWidget->appendText("Zoom In\n");
    }
    
    try
    {
        // Get the active image window
        pcl::ImageWindow window = pcl::ImageWindow::ActiveWindow();
        
        if (window.IsNull())
        {
            if (m_consoleWidget)
            {
                m_consoleWidget->appendText("  No active window\n");
            }
            return;
        }
        
        // Get current zoom factor
        int currentZoom = window.ZoomFactor();
        
        // Increase zoom (positive = zoom in)
        int newZoom = currentZoom + 1;
        if (newZoom > 16)  // Max zoom
            newZoom = 16;
        
        window.SetZoomFactor(newZoom);
        
        if (m_consoleWidget)
        {
            m_consoleWidget->appendText(QString("  Zoom: %1 → %2\n")
                .arg(currentZoom).arg(newZoom));
        }
    }
    catch (const pcl::Exception& e)
    {
        if (m_consoleWidget)
        {
            QString msg = QString::fromUtf16(
                reinterpret_cast<const char16_t*>(e.Message().c_str()));
            m_consoleWidget->appendText(QString("  Error: %1\n").arg(msg));
        }
    }
}

void SelectionWindow::onZoomOut()
{
    if (m_consoleWidget)
    {
        m_consoleWidget->appendText("Zoom Out\n");
    }
    
    try
    {
        pcl::ImageWindow window = pcl::ImageWindow::ActiveWindow();
        
        if (window.IsNull())
        {
            if (m_consoleWidget)
            {
                m_consoleWidget->appendText("  No active window\n");
            }
            return;
        }
        
        int currentZoom = window.ZoomFactor();
        
        // Decrease zoom (negative = zoom out)
        int newZoom = currentZoom - 1;
        if (newZoom < -16)  // Max zoom out
            newZoom = -16;
        
        window.SetZoomFactor(newZoom);
        
        if (m_consoleWidget)
        {
            m_consoleWidget->appendText(QString("  Zoom: %1 → %2\n")
                .arg(currentZoom).arg(newZoom));
        }
    }
    catch (const pcl::Exception& e)
    {
        if (m_consoleWidget)
        {
            QString msg = QString::fromUtf16(
                reinterpret_cast<const char16_t*>(e.Message().c_str()));
            m_consoleWidget->appendText(QString("  Error: %1\n").arg(msg));
        }
    }
}

void SelectionWindow::onZoom11()
{
    if (m_consoleWidget)
    {
        m_consoleWidget->appendText("Zoom to 1:1\n");
    }
    
    try
    {
        pcl::ImageWindow window = pcl::ImageWindow::ActiveWindow();
        
        if (window.IsNull())
        {
            if (m_consoleWidget)
            {
                m_consoleWidget->appendText("  No active window\n");
            }
            return;
        }
        
        // Zoom factor 1 = 1:1 (actual pixels)
        window.SetZoomFactor(1);
        
        if (m_consoleWidget)
        {
            m_consoleWidget->appendText("  Set zoom to 1:1\n");
        }
    }
    catch (const pcl::Exception& e)
    {
        if (m_consoleWidget)
        {
            QString msg = QString::fromUtf16(
                reinterpret_cast<const char16_t*>(e.Message().c_str()));
            m_consoleWidget->appendText(QString("  Error: %1\n").arg(msg));
        }
    }
}

void SelectionWindow::onZoomToFit()
{
    if (m_consoleWidget)
    {
        m_consoleWidget->appendText("Zoom to Fit\n");
    }
    
    try
    {
        pcl::ImageWindow window = pcl::ImageWindow::ActiveWindow();
        
        if (window.IsNull())
        {
            if (m_consoleWidget)
            {
                m_consoleWidget->appendText("  No active window\n");
            }
            return;
        }
        
        window.ZoomToFit();
        
        if (m_consoleWidget)
        {
            m_consoleWidget->appendText("  Zoomed to fit window\n");
        }
    }
    catch (const pcl::Exception& e)
    {
        if (m_consoleWidget)
        {
            QString msg = QString::fromUtf16(
                reinterpret_cast<const char16_t*>(e.Message().c_str()));
            m_consoleWidget->appendText(QString("  Error: %1\n").arg(msg));
        }
    }
}

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
- Tools: Test ExecuteGlobal and file format loaders
- View: Toggle console visibility
- Export: Export the selected interface to Qt code

Console Output:
- A dockable console window captures PCL console output
- To integrate with PCLMockAPI.cpp, modify the WriteConsole function:

    api_bool GlobalContext::WriteConsole(console_handle, const char16_type* text, api_bool newline)
    {
        extern void MockMainWriteConsole(const char16_t*, bool);
        MockMainWriteConsole(text, newline != 0);
        return api_true;
    }

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
