// galaxy_processor.h -- Galaxy preprocessing pipeline
//
// Umbrella header for all galaxy_processor modules
#pragma once

#include "gp_common.h"
#include "gp_wcs.h"
#include "gp_plate_solver.h"
#include "gp_color_calibration.h"
#include "gp_stretch.h"
#include "gp_histogram.h"
#include "gp_survey.h"
#include "gp_stacking.h"

// MetaModule stub
class GalaxyProcessorModule : public MetaModule
{
public:
   GalaxyProcessorModule() : MetaModule() {}
   const char* Version() const override { return "1.0.0"; }
   IsoString Name() const override { return "galaxy_processor"; }
   String Description() const override { return "Galaxy Preprocessing Pipeline"; }
   String Company() const override { return String(); }
   String Author() const override { return "test"; }
   String Copyright() const override { return String(); }
   String TradeMarks() const override { return String(); }
   String OriginalFileName() const override { return "galaxy_processor"; }
   void GetReleaseDate( int& y, int& m, int& d ) const override { y=2026; m=4; d=3; }
};
