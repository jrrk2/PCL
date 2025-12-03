// SymbolDiagnostic.cpp
// Diagnostic tool to see what ProcessInterface-related symbols exist

#include <mach-o/loader.h>
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
#include <algorithm>

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

struct SymbolInfo
{
    std::string mangled;
    std::string demangled;
    uint8_t type;
    uint64_t value;
};

std::vector<SymbolInfo> collectAllSymbols()
{
    std::vector<SymbolInfo> result;

    const char* exePath = _dyld_get_image_name(0);
    if (!exePath)
    {
        std::cerr << "ERROR: Cannot get executable path\n";
        return result;
    }

    std::cout << "Analyzing: " << exePath << "\n\n";

    int fd = ::open(exePath, O_RDONLY);
    if (fd < 0)
    {
        std::cerr << "ERROR: Cannot open executable\n";
        return result;
    }

    struct stat st;
    if (fstat(fd, &st) != 0)
    {
        std::cerr << "ERROR: Cannot stat executable\n";
        ::close(fd);
        return result;
    }

    void* mapped = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    ::close(fd);
    if (mapped == MAP_FAILED)
    {
        std::cerr << "ERROR: Cannot mmap executable\n";
        return result;
    }

    const uint32_t* magic = reinterpret_cast<const uint32_t*>(mapped);
    const bool is64 = (*magic == MH_MAGIC_64 || *magic == MH_CIGAM_64);

    if (!is64)
    {
        std::cerr << "ERROR: Not a 64-bit binary\n";
        munmap(mapped, st.st_size);
        return result;
    }

    const mach_header_64* mh = reinterpret_cast<const mach_header_64*>(mapped);
    const uint8_t* p = reinterpret_cast<const uint8_t*>(mh) + sizeof(mach_header_64);
    
    const symtab_command* symtabCmd = nullptr;

    for (uint32_t i = 0; i < mh->ncmds; ++i)
    {
        const load_command* cmd = reinterpret_cast<const load_command*>(p);
        if (cmd->cmd == LC_SYMTAB)
        {
            symtabCmd = reinterpret_cast<const symtab_command*>(cmd);
            break;
        }
        p += cmd->cmdsize;
    }

    if (!symtabCmd)
    {
        std::cerr << "ERROR: No symbol table found\n";
        munmap(mapped, st.st_size);
        return result;
    }

    const uint8_t* base = reinterpret_cast<const uint8_t*>(mapped);
    const nlist_64* symTable = reinterpret_cast<const nlist_64*>(base + symtabCmd->symoff);
    const char* strTable = reinterpret_cast<const char*>(base + symtabCmd->stroff);

    std::cout << "Total symbols in table: " << symtabCmd->nsyms << "\n\n";

    for (uint32_t i = 0; i < symtabCmd->nsyms; ++i)
    {
        const nlist_64& n = symTable[i];
        if (n.n_un.n_strx == 0)
            continue;

        const char* rawName = strTable + n.n_un.n_strx;
        if (rawName[0] == 0)
            continue;

        std::string mangled = rawName;
        std::string demangled;
        
        // Try to demangle if it looks like a C++ symbol
        if (mangled.size() > 1 && mangled[0] == '_' && mangled[1] == 'Z')
        {
            std::string noUnderscore = mangled.substr(1);
            demangled = demangle(noUnderscore.c_str());
        }
        else
        {
            demangled = mangled;
        }

        SymbolInfo info;
        info.mangled = std::move(mangled);
        info.demangled = std::move(demangled);
        info.type = n.n_type;
        info.value = n.n_value;
        
        result.push_back(std::move(info));
    }

    munmap(mapped, st.st_size);
    return result;
}

void analyzeSymbols(const std::vector<SymbolInfo>& symbols)
{
    std::cout << "=================================================================\n";
    std::cout << "SYMBOL ANALYSIS FOR PROCESSINTERFACE\n";
    std::cout << "=================================================================\n\n";

    // Find all symbols containing "Interface"
    std::vector<const SymbolInfo*> interfaceSymbols;
    for (const auto& sym : symbols)
    {
        if (sym.demangled.find("Interface") != std::string::npos)
        {
            interfaceSymbols.push_back(&sym);
        }
    }

    std::cout << "Found " << interfaceSymbols.size() << " symbols containing 'Interface'\n\n";

    // Group by category
    std::vector<const SymbolInfo*> constructors;
    std::vector<const SymbolInfo*> destructors;
    std::vector<const SymbolInfo*> vtables;
    std::vector<const SymbolInfo*> typeinfo;
    std::vector<const SymbolInfo*> methods;
    std::vector<const SymbolInfo*> other;

    for (const auto* sym : interfaceSymbols)
    {
        const std::string& dem = sym->demangled;
        
        if (dem.find("::~") != std::string::npos)
            destructors.push_back(sym);
        else if (dem.find("::Interface(") != std::string::npos)
            constructors.push_back(sym);
        else if (dem.find("vtable for") != std::string::npos)
            vtables.push_back(sym);
        else if (dem.find("typeinfo") != std::string::npos)
            typeinfo.push_back(sym);
        else if (dem.find("::") != std::string::npos && dem.find("(") != std::string::npos)
            methods.push_back(sym);
        else
            other.push_back(sym);
    }

    // Print constructors
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "CONSTRUCTORS (" << constructors.size() << " found):\n";
    std::cout << "-----------------------------------------------------------------\n";
    for (const auto* sym : constructors)
    {
        std::cout << "Mangled:   " << sym->mangled << "\n";
        std::cout << "Demangled: " << sym->demangled << "\n";
        std::cout << "Type:      0x" << std::hex << (int)sym->type << std::dec << "\n";
        std::cout << "\n";
    }

    // Print destructors
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "DESTRUCTORS (" << destructors.size() << " found):\n";
    std::cout << "-----------------------------------------------------------------\n";
    for (const auto* sym : destructors)
    {
        std::cout << "Mangled:   " << sym->mangled << "\n";
        std::cout << "Demangled: " << sym->demangled << "\n";
        std::cout << "\n";
    }

    // Print vtables
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "VTABLES (" << vtables.size() << " found):\n";
    std::cout << "-----------------------------------------------------------------\n";
    for (const auto* sym : vtables)
    {
        std::cout << "Mangled:   " << sym->mangled << "\n";
        std::cout << "Demangled: " << sym->demangled << "\n";
        std::cout << "\n";
    }

    // Print typeinfo
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "TYPEINFO (" << typeinfo.size() << " found):\n";
    std::cout << "-----------------------------------------------------------------\n";
    for (const auto* sym : typeinfo)
    {
        std::cout << "Mangled:   " << sym->mangled << "\n";
        std::cout << "Demangled: " << sym->demangled << "\n";
        std::cout << "\n";
    }

    // Print methods (first 10 only if more)
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "METHODS (" << methods.size() << " found, showing first 10):\n";
    std::cout << "-----------------------------------------------------------------\n";
    size_t count = 0;
    for (const auto* sym : methods)
    {
        if (count++ >= 10) break;
        std::cout << "Demangled: " << sym->demangled << "\n";
    }
    std::cout << "\n";

    // Print other (first 10 only if more)
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "OTHER (" << other.size() << " found, showing first 10):\n";
    std::cout << "-----------------------------------------------------------------\n";
    count = 0;
    for (const auto* sym : other)
    {
        if (count++ >= 10) break;
        std::cout << "Demangled: " << sym->demangled << "\n";
    }
    std::cout << "\n";

    // Now look specifically for ProcessInterface base class
    std::cout << "=================================================================\n";
    std::cout << "LOOKING FOR pcl::ProcessInterface BASE CLASS\n";
    std::cout << "=================================================================\n\n";

    bool foundPclProcessInterface = false;
    for (const auto& sym : symbols)
    {
        if (sym.demangled.find("pcl::ProcessInterface") != std::string::npos)
        {
            foundPclProcessInterface = true;
            std::cout << "Found: " << sym.demangled << "\n";
            std::cout << "Mangled: " << sym.mangled << "\n\n";
        }
    }

    if (!foundPclProcessInterface)
    {
        std::cout << "WARNING: No symbols found for pcl::ProcessInterface base class!\n";
        std::cout << "This likely means ProcessInterface is not linked into this binary.\n\n";
    }

    // Final summary
    std::cout << "=================================================================\n";
    std::cout << "SUMMARY\n";
    std::cout << "=================================================================\n";
    std::cout << "Total symbols analyzed:  " << symbols.size() << "\n";
    std::cout << "Interface-related:       " << interfaceSymbols.size() << "\n";
    std::cout << "  - Constructors:        " << constructors.size() << "\n";
    std::cout << "  - Destructors:         " << destructors.size() << "\n";
    std::cout << "  - VTables:             " << vtables.size() << "\n";
    std::cout << "  - TypeInfo:            " << typeinfo.size() << "\n";
    std::cout << "  - Methods:             " << methods.size() << "\n";
    std::cout << "  - Other:               " << other.size() << "\n";
    std::cout << "=================================================================\n\n";

    if (constructors.empty())
    {
        std::cout << "❌ NO INTERFACE CONSTRUCTORS FOUND!\n\n";
        std::cout << "Possible reasons:\n";
        std::cout << "1. ProcessInterface classes are not defined in this executable\n";
        std::cout << "2. They are in a separate shared library (.dylib)\n";
        std::cout << "3. Symbols were stripped from the binary\n";
        std::cout << "4. The classes use a different naming convention\n\n";
    }
    else
    {
        std::cout << "✓ Found interface constructors that can be used!\n\n";
    }
}

int main(int argc, char** argv)
{
    std::cout << "PCL Interface Symbol Diagnostic Tool\n";
    std::cout << "=====================================\n\n";

    auto symbols = collectAllSymbols();
    
    if (symbols.empty())
    {
        std::cerr << "ERROR: No symbols collected\n";
        return 1;
    }

    analyzeSymbols(symbols);
    
    return 0;
}
