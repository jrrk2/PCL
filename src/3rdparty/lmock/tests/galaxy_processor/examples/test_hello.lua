-- test_hello.lua -- Simple test of galaxy_processor Lua scripting
print("Galaxy Processor Lua scripting interface loaded!")
print("Available functions in galaxy namespace:")
for k, v in pairs(galaxy) do
   print("  galaxy." .. k .. " = " .. type(v))
end
