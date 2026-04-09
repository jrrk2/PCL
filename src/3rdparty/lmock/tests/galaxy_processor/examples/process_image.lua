-- process_image.lua -- Example galaxy_processor Lua pipeline
--
-- Usage: galaxy_processor --script process_image.lua
-- (set INPUT and OUTPUT variables below, or pass via environment)

local INPUT  = os.getenv("GP_INPUT")  or arg and arg[1] or error("Set GP_INPUT env var or pass input path")
local OUTPUT = os.getenv("GP_OUTPUT") or arg and arg[2] or INPUT:gsub("%.%w+$", "_processed.xisf")

print("=== Galaxy Processor Pipeline (Lua) ===")
print("Input:  " .. INPUT)
print("Output: " .. OUTPUT)

-- Step 1: Load
local img = galaxy.load(INPUT)
local info = galaxy.info(img)
print(string.format("Loaded: %dx%dx%d", info.width, info.height, info.channels))

-- Step 2: Background Neutralization
print("\n--- Background Neutralization ---")
galaxy.bg_neutralize(img)

-- Step 3: VeraLux Stretch
print("\n--- VeraLux HyperMetric Stretch ---")
galaxy.stretch(img, {
   target_bg = 0.10,
   protect_b = 5.0,
   convergence = 3.5,
})

-- Step 4: Wavelet Enhancement
print("\n--- Starlet Wavelet Enhancement ---")
galaxy.wavelet(img, {
   wavelet_scales = 5,
   wavelet_gain = {0.0, 0.7, 1.0, 1.3, 0.5},
   wavelet_residual = 1.3,
})

-- Step 5: Save
print("\n--- Saving ---")
galaxy.save(img, OUTPUT)

-- Step 6: Histogram summary
local h = galaxy.histogram(img)
print(string.format("\nHistogram percentiles:"))
for p = 0, 10 do
   print(string.format("  P%d0 = %.4f", p, h.percentiles[p]))
end

print("\nDone!")
