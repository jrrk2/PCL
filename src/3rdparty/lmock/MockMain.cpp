// mockmain_main_find_all_widgets.cpp
// Alternative approach: Find all widgets regardless of layout hierarchy

#include <QApplication>
#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QDebug>

#include <pcl/Console.h>
#include <pcl/api/APIInterface.h>
#include <pcl/MetaModule.h>
#include <pcl/MetaProcess.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ProcessImplementation.h>

#include "PCLMockAPI.h"
#include "ExportHelper.h"
#include "RootWidgetSelector.h"

using namespace pcl;

// Find all widgets of a type, regardless of layout
QList<QWidget*> findAllWidgetsOfType(QWidget* root, const QString& typeName)
{
    QList<QWidget*> found;
    
    std::function<void(QWidget*)> search = [&](QWidget* w) {
        if (!w) return;
        
        if (w->metaObject()->className() == typeName) {
            found.append(w);
        }
        
        // Search all children
        for (QObject* child : w->children()) {
            if (QWidget* childWidget = qobject_cast<QWidget*>(child)) {
                search(childWidget);
            }
        }
    };
    
    search(root);
    return found;
}

class MockMainInstance : public ProcessImplementation
{
public:

   MockMainInstance( const MetaProcess* );
   MockMainInstance( const MockMainInstance& );
  ~MockMainInstance();
   void Assign( const ProcessImplementation& ) override;
};

MockMainInstance::MockMainInstance( const MetaProcess* m )
   : ProcessImplementation( m )
{
}

MockMainInstance::MockMainInstance( const MockMainInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

MockMainInstance::~MockMainInstance()
{
}

void MockMainInstance::Assign( const ProcessImplementation& p )
{
   const MockMainInstance* x = dynamic_cast<const MockMainInstance*>( &p );
}

class MockMainModule : public MetaModule
{
public:
   MockMainModule();
   ~MockMainModule();
   const char* Version() const override;
   IsoString Name() const override;
};

MockMainModule::MockMainModule()
{
}

MockMainModule::~MockMainModule()
{
}

IsoString MockMainModule::Name() const
{
   return "MockMain";
}

const char* MockMainModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

class MockMainProcess : public MetaProcess
{
public:
   MockMainProcess();
   ~MockMainProcess();
   IsoString Id() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
};

MockMainProcess* TheMockMainProcess = nullptr;

MockMainProcess::MockMainProcess()
{
}

MockMainProcess::~MockMainProcess()
{
}

IsoString MockMainProcess::Id() const
{
   return "MockMain";
}

ProcessImplementation* MockMainProcess::Create() const
{
   return new MockMainInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* MockMainProcess::Clone( const ProcessImplementation& p ) const
{
  /*
   const MockMainInstance* instance = dynamic_cast<const MockMainInstance*>( &p );
   return (instance != nullptr) ? new MockMainInstance( *instance ) : nullptr;
  */
  abort();
}
 
class MockMainInterface : public ProcessInterface
{
public:

   MockMainInterface();
   virtual ~MockMainInterface();
   IsoString Id() const override;
   MetaProcess* Process() const override;
   MockMainProcess* m_instance;
};

MockMainInterface* TheMockMainInterface = nullptr;

MockMainInterface::MockMainInterface()
   : m_instance( TheMockMainProcess )
{
   TheMockMainInterface = this;
}

MockMainInterface::~MockMainInterface()
{

}

IsoString MockMainInterface::Id() const
{
   return "MockMain";
}

// ----------------------------------------------------------------------------

MetaProcess* MockMainInterface::Process() const
{
   return TheMockMainProcess;
}
 
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SetDebugLogging(true);

    // Initialize API before Console
    
    Module = new MockMainModule;
    InitializePixInsightModule(Module, GetMockFunctionResolver(), PCL_API_Version, nullptr );

    MockMainProcess proc;
    MockMainInterface iface;
    MockBase* interfaceRoot = new MockBase();
    interfaceRoot->isSizer = false;
    interfaceRoot->widget = new QWidget(nullptr);  // True top-level
    interfaceRoot->widget->setWindowTitle("MockMainInterface Mock");
    
    // Add to top-level list
    g_topLevelWidgets.append(interfaceRoot);
    
    // Set the interface's handle (simulate what PixInsight core does)
    // This is what InterfaceDispatcher::Initialize() does:
    iface.handle = (control_handle)interfaceRoot;

    bool dynamic = false;
    unsigned flags = 0;
    
    iface.Launch(proc, nullptr, dynamic, flags);

    iface.Show();

    QList<QWidget*> candidates;
    for (MockBase* base : g_topLevelWidgets) {
        if (base && base->widget) {
            candidates.append(base->widget);
        }
    }
    
    // Smart selection!
    QWidget* bestRoot = RootWidgetSelector::selectBestRoot(candidates, true);
    
    if (!bestRoot) {
        fputs("Error: No root widget!\n", stderr);
        return 1;
    }
    
    // Wait for everything to be created
    QApplication::processEvents();
    QThread::msleep(200);
    QApplication::processEvents();
    ExportHelper::exportInterface(bestRoot, "MockMainDialog", "./exported");
    return app.exec();
}
