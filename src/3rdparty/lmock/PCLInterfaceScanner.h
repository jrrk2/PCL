// PCLInterfaceScanner.h

#pragma once

#include <string>
#include <vector>

struct DiscoveredInterface
{
    std::string className;       // e.g. "pcl::SandboxInterface"
    std::string mangledCtorName; // e.g. "_ZN3pcl16SandboxInterfaceC1Ev"
};

// Scan the current executable for derived ProcessInterface classes.
// Returns ctor mangled name and demangled class name.
std::vector<DiscoveredInterface> scanDerivedPCLInterfaces();
std::vector<std::string> collectAllExternalFunctionSymbols();
