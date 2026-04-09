// gp_lua.h -- Lua scripting interface for galaxy_processor
#pragma once

#include <string>

// Initialize the Lua scripting environment with all galaxy.* bindings
// and run a script file. Returns 0 on success, non-zero on error.
int RunLuaScript( const std::string& scriptPath );
