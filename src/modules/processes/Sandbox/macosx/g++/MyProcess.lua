-- Auto-generated automation script
-- Module: MyModule
-- Process: MyProcess
-- Generated: 2025-12-09T17:13:45

local process = {
}

-- Execute the process
function execute()
    print("Executing MyProcess with parameters:")
    for k, v in pairs(process) do
        print(string.format("  %s = %s", k, tostring(v)))
    end
    
    -- TODO: Call actual module execution
    -- MyModule.MyProcess.execute(process)
end

-- Run if this is the main script
if arg and arg[0]:match("MyProcess.lua") then
    execute()
end

return process
