// ============================================================================
// pi_fft_reference.js - PixInsight PJSR script for FFT reference generation
//
// This script creates the same synthetic test images used by test_fft,
// applies PixInsight's real FourierTransform, and saves the results as XISF
// for comparison with mock API output.
//
// Usage from command line:
//   /Applications/PixInsight/PixInsight.app/Contents/MacOS/PixInsight \
//     --run=pi_fft_reference.js --arguments=<output_dir>
//
// Or run from PixInsight's Script Editor.
// ============================================================================

#feature-id  TestFFTReference
#feature-info  Generate FFT reference images for mock comparison.

// Output directory - override via --arguments or edit here
var outputDir = File.systemTempDirectory + "/fft_reference";
if (jsArguments.length > 0)
   outputDir = jsArguments[0];

console.writeln("FFT Reference Generator");
console.writeln("Output directory: " + outputDir);

if (!File.directoryExists(outputDir))
   File.createDirectory(outputDir, true);

// ============================================================================
// Helper: Create a synthetic image window
// ============================================================================

function createTestImage(id, width, height, fillFunc)
{
   var w = new ImageWindow(width, height, 1, 32, true, false, id);
   var v = w.mainView;

   v.beginProcess();
   var img = v.image;
   for (var y = 0; y < height; y++)
      for (var x = 0; x < width; x++)
         img.setSample(fillFunc(x, y, width, height), x, y, 0);
   v.endProcess();

   return w;
}

// ============================================================================
// Helper: Save image window to XISF
// ============================================================================

function saveXISF(window, path)
{
   var f = new FileFormat("XISF", false/*read*/, true/*write*/);
   if (f.isNull)
      throw new Error("XISF format not available");

   var fi = new FileFormatInstance(f);
   if (!fi.create(path))
      throw new Error("Cannot create: " + path);

   var d = new ImageDescription;
   d.bitsPerSample = 32;
   d.ieeefpSampleFormat = true;
   fi.setOptions(d);

   if (!fi.writeImage(window.mainView.image))
      throw new Error("Write failed: " + path);

   fi.close();
   console.writeln("  Saved: " + path);
}

// ============================================================================
// Helper: Apply FourierTransform process and return component windows
// ============================================================================

function applyFFT(sourceWindow, centered)
{
   // Apply FourierTransform in real/imaginary mode (not radial)
   var P = new FourierTransform;
   P.radialCoordinates = false;  // Output real + imaginary
   P.centered = (centered === true);

   // ExecuteOn creates two new windows: DFT_real and DFT_imaginary
   P.executeOn(sourceWindow.mainView);

   // Find the output windows
   var realWin = ImageWindow.windowById("DFT_real");
   var imagWin = ImageWindow.windowById("DFT_imaginary");

   return { real: realWin, imag: imagWin };
}

// ============================================================================
// Test cases (must match test_fft.cpp)
// ============================================================================

var testCases = [
   {
      name: "constant_042",
      fill: function(x, y, w, h) { return 0.42; }
   },
   {
      name: "cosine_3_5",
      fill: function(x, y, w, h) {
         return 0.5 + 0.5 * Math.cos(2 * Math.PI * 3 * x / w + 2 * Math.PI * 5 * y / h);
      }
   },
   {
      name: "cosine_4_0",
      fill: function(x, y, w, h) {
         return 0.5 * Math.cos(2 * Math.PI * 4 * x / w);
      }
   },
   {
      name: "impulse",
      fill: function(x, y, w, h) {
         return (x == Math.floor(w/2) && y == Math.floor(h/2)) ? 1.0 : 0.0;
      }
   },
   {
      name: "cosine_7_3",
      fill: function(x, y, w, h) {
         return 0.5 + 0.5 * Math.cos(2 * Math.PI * 7 * x / w + 2 * Math.PI * 3 * y / h);
      }
   }
];

// ============================================================================
// Main: generate reference images
// ============================================================================

console.writeln("\nGenerating " + testCases.length + " test cases...\n");

for (var i = 0; i < testCases.length; i++)
{
   var tc = testCases[i];
   console.writeln("--- " + tc.name + " ---");

   // Create test image
   var srcWin = createTestImage("test_" + tc.name, 64, 64, tc.fill);

   // Save input
   saveXISF(srcWin, outputDir + "/" + tc.name + "_input.xisf");

   // Apply FFT (not centered, to match test_fft.cpp which doesn't center)
   var dft = applyFFT(srcWin, false);

   if (!dft.real.isNull && !dft.imag.isNull)
   {
      // PixInsight's FourierTransformInstance normalizes by 1/N and rescales
      // to [0,1]. We need the raw values for comparison.
      // The raw values are: pixel * (max - min) + min
      // where min/max are stored in FITS keywords PIDFTMIN, PIDFTMAX.
      //
      // Save the rescaled images as-is (we'll undo the rescaling in comparison
      // code using the FITS keywords, or we just compare the rescaled versions).

      saveXISF(dft.real, outputDir + "/" + tc.name + "_pi_real.xisf");
      saveXISF(dft.imag, outputDir + "/" + tc.name + "_pi_imag.xisf");

      // Also log the FITS keywords for debugging
      var keywords = dft.real.keywords;
      for (var k = 0; k < keywords.length; k++)
      {
         var kw = keywords[k];
         if (kw.name.indexOf("PIDFT") >= 0)
            console.writeln("  " + kw.name + " = " + kw.value + " // " + kw.comment);
      }

      // Clean up DFT windows
      dft.real.forceClose();
      dft.imag.forceClose();
   }
   else
   {
      console.warningln("  WARNING: FourierTransform did not create output windows");
   }

   // Clean up source
   srcWin.forceClose();
}

console.writeln("\n===========================================");
console.writeln("Reference generation complete.");
console.writeln("Output directory: " + outputDir);
console.writeln("===========================================");
