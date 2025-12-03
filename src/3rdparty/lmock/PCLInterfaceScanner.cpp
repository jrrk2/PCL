// PCLInterfaceScanner.cpp - FIXED VERSION

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
#include <cstring>

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

// Extract class name from a demangled constructor
// Input: "pcl::SandboxInterface::SandboxInterface()"
// Output: "pcl::SandboxInterface"
static std::string extractClassName(const std::string& demangled)
{
    // Find the last occurrence of "::" before the opening parenthesis
    size_t parenPos = demangled.find('(');
    if (parenPos == std::string::npos)
        return "";
    
    // Work backwards from the paren to find the last ::
    size_t colonPos = demangled.rfind("::", parenPos);
    if (colonPos == std::string::npos)
        return "";
    
    // The class name is everything before the last ::
    std::string className = demangled.substr(0, colonPos);
    
    return className;
}

std::vector<DiscoveredInterface> scanDerivedPCLInterfaces()
{
    std::vector<DiscoveredInterface> out;
    auto allSyms = collectAllExternalFunctionSymbols();

    std::cerr << "Scanning executable: " << _dyld_get_image_name(0) << std::endl;
    std::cerr << "Found " << allSyms.size() << " symbols in symbol table" << std::endl;

    // First pass: collect all C++ symbols
    std::vector<std::string> cppSymbols;
    for (const auto& mangled : allSyms)
    {
        // We only care about C++ symbols
        // Linux/ELF:  _Z...   (one underscore)
        // macOS/Mach-O: __Z... (two underscores)
        bool isCppSymbol = false;
        if (mangled.size() >= 3 && mangled[0] == '_' && mangled[1] == 'Z')
        {
            isCppSymbol = true;  // Linux style
        }
        else if (mangled.size() >= 4 && mangled[0] == '_' && mangled[1] == '_' && mangled[2] == 'Z')
        {
            isCppSymbol = true;  // macOS style
        }
        
        if (isCppSymbol)
        {
            cppSymbols.push_back(mangled);
        }
    }
    
    std::cerr << "Collected " << cppSymbols.size() << " C++ symbols" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Scanning for ProcessInterface-derived classes..." << std::endl;
    std::cerr << std::endl;

    int interfaceRelated = 0;
    int constructors = 0;

    for (const auto& mangled : cppSymbols)
    {
        // Look for constructor patterns: C1Ev or C2Ev
        // C1 = complete object constructor
        // C2 = base object constructor
        // Ev = void return, no parameters (default constructor)
        bool isConstructor = (mangled.find("C1Ev") != std::string::npos ||
                             mangled.find("C2Ev") != std::string::npos ||
                             mangled.find("C1E") != std::string::npos ||
                             mangled.find("C2E") != std::string::npos);
        
        if (!isConstructor)
            continue;
        
        constructors++;

        // Strip leading underscore(s) for demangling
        // Linux: _Z... → Z...
        // macOS: __Z... → _Z... (demangle expects single underscore)
        std::string toDemangle = mangled;
        if (mangled.size() >= 4 && mangled[0] == '_' && mangled[1] == '_' && mangled[2] == 'Z')
        {
            // macOS style: strip one underscore
            toDemangle = mangled.substr(1);
        }
        else if (mangled.size() >= 2 && mangled[0] == '_')
        {
            // Already correct format (Linux style with single _)
            // No change needed
        }
        
        std::string dem = demangle(toDemangle.c_str());
        
        // Must contain "Interface" to be an interface class
        if (dem.find("Interface") == std::string::npos)
            continue;
        
        interfaceRelated++;
        
        // Extract class name
        std::string className = extractClassName(dem);
        
        if (className.empty())
            continue;
        
        // Must end with "Interface" to match our pattern
        if (className.length() < 9 || 
            className.substr(className.length() - 9) != "Interface")
            continue;
        
        // Found one!
        std::cerr << "✓ Found ProcessInterface derivative:" << std::endl;
        std::cerr << "  Mangled:   " << mangled << std::endl;
        std::cerr << "  Demangled: " << dem << std::endl;
        std::cerr << "  Class:     " << className << std::endl;
        std::cerr << std::endl;

        DiscoveredInterface di;
        di.className = className;
        di.mangledCtorName = mangled; // still with leading '_'

        out.push_back(std::move(di));
    }

    std::cerr << "Scan summary:" << std::endl;
    std::cerr << "  C++ symbols: " << cppSymbols.size() << std::endl;
    std::cerr << "  Constructors: " << constructors << std::endl;
    std::cerr << "  Interface-related: " << interfaceRelated << std::endl;
    std::cerr << "  Interface classes found: " << out.size() << std::endl;
    std::cerr << std::endl;

    if (out.empty())
    {
        std::cerr << "⚠️  WARNING: No ProcessInterface derivatives found!" << std::endl;
        std::cerr << std::endl;
        std::cerr << "This could mean:" << std::endl;
        std::cerr << "1. Your interface classes are in a separate .dylib" << std::endl;
        std::cerr << "2. The symbols were stripped from the binary" << std::endl;
        std::cerr << "3. The interface classes aren't actually compiled into this executable" << std::endl;
        std::cerr << "4. They use a different naming pattern" << std::endl;
        std::cerr << std::endl;

        // Debug: show some Interface symbols if they exist
        std::cerr << "Debug: Looking for any symbol with 'Interface' in it..." << std::endl;
        int count = 0;
        for (const auto& mangled : cppSymbols)
        {
            std::string noUnderscore = mangled.substr(1);
            std::string dem = demangle(noUnderscore.c_str());
            if (dem.find("Interface") != std::string::npos)
            {
                std::cerr << "  Found: " << dem.substr(0, 120) << (dem.length() > 120 ? "..." : "") << std::endl;
                count++;
                if (count > 20) {
                    std::cerr << "  ... and more" << std::endl;
                    break;
                }
            }
        }
        
        if (count == 0) {
            std::cerr << "  (No symbols containing 'Interface' found)" << std::endl;
        }
        std::cerr << std::endl;
    }

    return out;
}
