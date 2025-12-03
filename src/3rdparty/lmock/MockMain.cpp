// MockMain.cpp

#include <QApplication>
#include <QThread>
#include <QDebug>

#include <dlfcn.h>

#include <pcl/MetaModule.h>
#include <pcl/api/APIInterface.h>
#include <pcl/MetaProcess.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ProcessImplementation.h>

#include "PCLInterfaceScanner.h"
#include "PCLMockAPI.h"
#include "ExportHelper.h"
#include "RootWidgetSelector.h"

// Raw constructor types
using RawCtor0 = void (*)(void*);
using RawCtor1 = void (*)(void*, void*);

static void* mustResolve(const char* name)
{
    void* p = dlsym(RTLD_DEFAULT, name);
    if (!p)
    {
        fprintf(stderr, "FATAL: dlsym(%s) failed: %s\n", name, dlerror());
        std::abort();
    }
    return p;
}

static void* dynamicNew(void* ctorAddr, size_t size, void* arg1 = nullptr)
{
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

// For now: a simple size map for the interfaces you care about.
// You can extend this without touching module source.
size_t sizeForInterfaceClass(const std::string& className)
{
    return sizeof(pcl::ProcessInterface);
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SetDebugLogging(true);

    // Initialize PCL mock API (your existing call)
    Module = nullptr;
    InitializePixInsightModule(Module, GetMockFunctionResolver(), PCL_API_Version, nullptr);

    // Discover all interface classes present in *this* executable
    auto discovered = scanDerivedPCLInterfaces();

    qDebug() << "Discovered interfaces:";
    for (const auto& di : discovered)
        qDebug() << "  class =" << di.className.c_str()
                 << "ctor =" << di.mangledCtorName.c_str();

    if (discovered.empty())
    {
        fprintf(stderr, "No PCL ProcessInterface derivatives found.\n");
        return 1;
    }

    // For now, pick the first one, or prefer Sandbox
    const DiscoveredInterface* chosen = nullptr;
    for (const auto& di : discovered)
    {
        if (di.className.find("SandboxInterface") != std::string::npos)
        {
            chosen = &di;
            break;
        }
    }
    if (!chosen)
        chosen = &discovered.front();

    qDebug() << "Chosen interface:" << chosen->className.c_str();

    // Construct the REAL MetaModule and MetaProcess the usual way
    // (Here I'm assuming you still know how to make them; if needed we
    // can do a similar auto-detect for processes and modules.)
    // For now, assume you have:
    //   extern pcl::MetaModule* TheSandboxModule;
    //   extern pcl::MetaProcess* TheSandboxProcess;
    // or you can construct them exactly the way you did before.

    // --------- Simplified example: one process / one module ----------
    // In practice, you'd also auto-detect MetaProcess/MetaModule
    // using the same pattern as interfaces.
    pcl::MetaModule* MM = nullptr;   // TODO: initialize properly
    pcl::MetaProcess* MP = nullptr;  // TODO: initialize properly

    // Resolve and call the chosen interface constructor
    void* ctorAddr = mustResolve(chosen->mangledCtorName.c_str());
    size_t sz = sizeForInterfaceClass(chosen->className);

    // We don't know the concrete C++ type at compile time,
    // so treat it as a ProcessInterface*
    pcl::ProcessInterface* IF = reinterpret_cast<pcl::ProcessInterface*>(
        dynamicNew(ctorAddr, sz)
    );

    if (!MP)
    {
        fprintf(stderr, "MetaProcess not set up yet.\n");
        return 1;
    }

    // Create a REAL process instance and launch the interface
    pcl::ProcessImplementation* inst = MP->Create();
    if (!inst)
    {
        fprintf(stderr, "MetaProcess::Create() returned null.\n");
        return 1;
    }

    bool dynamic = false;
    unsigned flags = 0;

    IF->Launch(*MP, inst, dynamic, flags);
    IF->Show();

    // Your existing widget export logic
    QList<QWidget*> candidates;
    for (MockBase* base : g_topLevelWidgets)
        if (base && base->widget)
            candidates.append(base->widget);

    QWidget* root = RootWidgetSelector::selectBestRoot(candidates, true);
    if (!root)
    {
        fprintf(stderr, "No root widget found.\n");
        return 1;
    }

    QApplication::processEvents();
    QThread::msleep(200);
    QApplication::processEvents();

    ExportHelper::exportInterface(root,
                                  chosen->className.c_str(),
                                  "./exported");

    return app.exec();
}
