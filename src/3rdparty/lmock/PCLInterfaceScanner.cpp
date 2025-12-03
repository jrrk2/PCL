// PCLInterfaceScanner.cpp

#include "PCLInterfaceScanner.h"

#include <mach-o/loader.h>
#include <mach-o/fat.h>
#include <mach-o/nlist.h>
#include <mach-o/dyld.h>

#include <cxxabi.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <iostream>

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

// Very small helper to read the main executable's Mach-O symbol table
// and collect all external function symbols.
std::vector<std::string> collectAllExternalFunctionSymbols()
{
    std::vector<std::string> result;

    const char* exePath = _dyld_get_image_name(0);
    if (!exePath)
        return result;

    int fd = ::open(exePath, O_RDONLY);
    if (fd < 0)
        return result;

    struct stat st;
    if (fstat(fd, &st) != 0)
    {
        ::close(fd);
        return result;
    }

    void* mapped = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    ::close(fd);
    if (mapped == MAP_FAILED)
        return result;

    const uint32_t* magic = reinterpret_cast<const uint32_t*>(mapped);
    const bool is64 =
        (*magic == MH_MAGIC_64 || *magic == MH_CIGAM_64);

    if (!is64)
    {
        munmap(mapped, st.st_size);
        return result;
    }

    const mach_header_64* mh =
        reinterpret_cast<const mach_header_64*>(mapped);

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
        munmap(mapped, st.st_size);
        return result;
    }

    const uint8_t* base = reinterpret_cast<const uint8_t*>(mapped);

    const nlist_64* symTable =
        reinterpret_cast<const nlist_64*>(base + symtabCmd->symoff);
    const char* strTable =
        reinterpret_cast<const char*>(base + symtabCmd->stroff);

    for (uint32_t i = 0; i < symtabCmd->nsyms; ++i)
    {
        const nlist_64& n = symTable[i];
        if (n.n_un.n_strx == 0)
            continue;

        const char* rawName = strTable + n.n_un.n_strx;

        // We want external text symbols
        if (!(n.n_type & N_EXT))
            continue;
        if ((n.n_type & N_TYPE) != N_SECT)
            continue;
        if (rawName[0] == 0)
            continue;

        // Mach-O C symbols start with '_'
        std::string mangled = rawName;
        result.push_back(std::move(mangled));
    }

    munmap(mapped, st.st_size);
    return result;
}

std::vector<DiscoveredInterface> scanDerivedPCLInterfaces()
{
    std::vector<DiscoveredInterface> out;
    auto allSyms = collectAllExternalFunctionSymbols();

    for (const auto& mangled : allSyms)
    {
        // We only care about C++ symbols (start with '_Z')
        if (mangled.size() < 3 || mangled[0] != '_' || mangled[1] != 'Z')
            continue;

        // Heuristic: constructor of something whose name ends in "Interface"
        // Symbol patterns: C1/C2 for ctors
        if (mangled.find("InterfaceC1E") == std::string::npos &&
            mangled.find("InterfaceC2E") == std::string::npos)
            continue;

        // Strip leading '_' for demangling
        std::string noUnderscore = mangled.substr(1);
        std::string dem = demangle(noUnderscore.c_str());

        // Example demangled:
        // "pcl::SandboxInterface::SandboxInterface()"
        if (dem.find("::") == std::string::npos)
            continue;
        if (dem.find("Interface::Interface") == std::string::npos)
            continue;

        // Extract class name: everything before "::Interface("
        auto pos = dem.find("::Interface");
        if (pos == std::string::npos)
            continue;
        std::string cls = dem.substr(0, pos); // e.g. "pcl::SandboxInterface"

        DiscoveredInterface di;
        di.className = std::move(cls);
        di.mangledCtorName = mangled; // still with leading '_'

        out.push_back(std::move(di));
    }

    return out;
}
