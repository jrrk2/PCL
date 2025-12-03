// MockMain.cpp
//
// Generic: scan current executable's symbol table for ProcessInterface
// subclasses, construct them via their constructors, and launch their GUIs.

#include <QApplication>
#include <QThread>
#include <QWidget>
#include <QDebug>

#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/dyld.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cxxabi.h>
#include <string>
#include <vector>
#include <iostream>

#include <pcl/MetaModule.h>
#include <pcl/api/APIInterface.h>
#include <pcl/MetaProcess.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ProcessImplementation.h>

#include "PCLMockAPI.h"
#include "RootWidgetSelector.h"
#include "ExportHelper.h"

// This comes from your mock framework
extern QList<MockBase*> g_topLevelWidgets;

using namespace pcl;

// ---------- small helpers ----------

struct DiscoveredInterfaceCtor
{
    std::string className;   // e.g. "pcl::SandboxInterface"
    std::string demangled;   // full demangled symbol
    uintptr_t   address;     // runtime address of ctor function
};

static std::string demangle(const char* mangled)
{
    int status = 0;
    char* dem = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
    if (status == 0 && dem)
    {
        std::string s(dem);
        std::free(dem);
        return s;
    }
    return mangled ? std::string(mangled) : std::string();
}

// Scan the main executable (image index 0) and collect all
// external function symbols whose demangled names look like
// "somethingInterface::Interface(...)"
static std::vector<DiscoveredInterfaceCtor> scanInterfaceCtors()
{
    std::vector<DiscoveredInterfaceCtor> out;

    const char* exePath = _dyld_get_image_name(0);
    if (!exePath)
    {
        std::fprintf(stderr, "Cannot get main image name.\n");
        return out;
    }

    int fd = ::open(exePath, O_RDONLY);
    if (fd < 0)
    {
        std::perror("open");
        return out;
    }

    struct stat st{};
    if (fstat(fd, &st) != 0)
    {
        std::perror("fstat");
        ::close(fd);
        return out;
    }

    void* mapped = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    ::close(fd);
    if (mapped == MAP_FAILED)
    {
        std::perror("mmap");
        return out;
    }

    const auto* mh = reinterpret_cast<const mach_header_64*>(mapped);
    if (mh->magic != MH_MAGIC_64 && mh->magic != MH_CIGAM_64)
    {
        std::fprintf(stderr, "Not a 64-bit Mach-O main image.\n");
        munmap(mapped, st.st_size);
        return out;
    }

    const uint8_t* p = reinterpret_cast<const uint8_t*>(mh) + sizeof(mach_header_64);
    const load_command* cmd = nullptr;
    const symtab_command* symtabCmd = nullptr;

    for (uint32_t i = 0; i < mh->ncmds; ++i)
    {
        cmd = reinterpret_cast<const load_command*>(p);
        if (cmd->cmd == LC_SYMTAB)
        {
            symtabCmd = reinterpret_cast<const symtab_command*>(cmd);
            break;
        }
        p += cmd->cmdsize;
    }

    if (!symtabCmd)
    {
        std::fprintf(stderr, "LC_SYMTAB not found.\n");
        munmap(mapped, st.st_size);
        return out;
    }

    const uint8_t* base = reinterpret_cast<const uint8_t*>(mapped);
    const auto* symTable =
        reinterpret_cast<const nlist_64*>(base + symtabCmd->symoff);
    const char* strTable =
        reinterpret_cast<const char*>(base + symtabCmd->stroff);

    intptr_t slide = _dyld_get_image_vmaddr_slide(0);

    for (uint32_t i = 0; i < symtabCmd->nsyms; ++i)
    {
        const nlist_64& n = symTable[i];
        if (n.n_un.n_strx == 0)
            continue;

        uint8_t type = n.n_type;
        if (type & N_STAB)
            continue;

        // We want external text symbols
        if (!(type & N_EXT))
            continue;
        if ((type & N_TYPE) != N_SECT)
            continue;
        if (n.n_value == 0)
            continue;

        const char* rawName = strTable + n.n_un.n_strx;
        if (!rawName || rawName[0] == 0)
            continue;

        // C++ symbols: start with _Z (Itanium ABI)
        if (rawName[0] != '_' || rawName[1] != 'Z')
            continue;

        // Strip leading '_'
        std::string noUnder = rawName + 1;
        std::string dem = demangle(noUnder.c_str());

        // Heuristic: look for "Interface::Interface(" in the demangled name.
        // e.g. "pcl::SandboxInterface::SandboxInterface()"
        if (dem.find("Interface::Interface(") == std::string::npos)
            continue;

        // Extract class name prefix: everything before "::Interface("
        auto pos = dem.find("::Interface(");
        if (pos == std::string::npos)
            continue;

        std::string cls = dem.substr(0, pos); // e.g. "pcl::SandboxInterface"

        DiscoveredInterfaceCtor di;
        di.className  = std::move(cls);
        di.demangled  = std::move(dem);
        di.address    = static_cast<uintptr_t>(n.n_value) + static_cast<uintptr_t>(slide);

        out.push_back(std::move(di));
    }

    munmap(mapped, st.st_size);
    return out;
}

// Raw ctor type for a no-arg constructor: void (*)(void*)
using RawCtor0 = void (*)(void*);

// Allocate a big slab of memory and invoke the constructor at 'addr'.
// We over-allocate (e.g. 64 KB) to be safely larger than any interface object.
static ProcessInterface* constructInterface(const DiscoveredInterfaceCtor& di)
{
    constexpr size_t MAX_IFACE_SIZE = 64 * 1024; // generous upper bound

    void* storage = ::operator new(MAX_IFACE_SIZE);
    auto ctor = reinterpret_cast<RawCtor0>(di.address);
    ctor(storage); // placement new-style: this = storage

    return reinterpret_cast<ProcessInterface*>(storage);
}

// ---------- main ----------

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SetDebugLogging(true);

    // Initialize your mock PCL API layer.
    InitializePixInsightModule(Module, GetMockFunctionResolver(), PCL_API_Version, nullptr);

    auto ctors = scanInterfaceCtors();

    if (ctors.empty())
    {
        std::fprintf(stderr, "No ProcessInterface-derived constructors found via symbol scan.\n");
        return 1;
    }

    qDebug() << "Discovered interface ctors:";
    for (const auto& di : ctors)
        qDebug() << "  " << di.demangled.c_str();

    // For now, just pick the first one. You can refine this later
    // (e.g., filter by prefix, or let the user choose).
    const DiscoveredInterfaceCtor& chosen = ctors.front();
    qDebug() << "Chosen interface class:" << chosen.className.c_str();

    ProcessInterface* IF = constructInterface(chosen);
    if (!IF)
    {
        std::fprintf(stderr, "Failed to construct interface.\n");
        return 1;
    }

    // Use the real virtual API to connect things up.
    MetaProcess* MP = IF->Process();
    if (!MP)
    {
        std::fprintf(stderr, "Interface::Process() returned null.\n");
        return 1;
    }

    ProcessImplementation* inst = MP->Create();
    if (!inst)
    {
        std::fprintf(stderr, "MetaProcess::Create() returned null.\n");
        return 1;
    }

    bool dynamic = false;
    unsigned flags = 0;

    if (!IF->Launch(*MP, inst, dynamic, flags))
    {
        std::fprintf(stderr, "Interface::Launch() returned false.\n");
        return 1;
    }

    IF->Show();

    // Collect top-level widgets from your mock framework
    QList<QWidget*> candidates;
    for (MockBase* base : g_topLevelWidgets)
        if (base && base->widget)
            candidates.append(base->widget);

    if (candidates.isEmpty())
    {
        std::fprintf(stderr, "No top-level widgets registered.\n");
        return 1;
    }

    QWidget* root = RootWidgetSelector::selectBestRoot(candidates, true);
    if (!root)
    {
        std::fprintf(stderr, "RootWidgetSelector failed.\n");
        return 1;
    }

    QApplication::processEvents();
    QThread::msleep(200);
    QApplication::processEvents();

    // Use the interface ID (or className) as export tag
    ExportHelper::exportInterface(root, IF->Id().c_str(), "./exported");

    return app.exec();
}
