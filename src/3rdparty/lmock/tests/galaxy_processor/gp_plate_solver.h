// gp_plate_solver.h -- Plate solving via Gaia catalog and StellarSolver
#pragma once

#include "gp_wcs.h"

// Gaia catalog plate solver (PI ImageSolver technique)
bool PlateSolveGaia( const Image& image, SimpleTANWCS& wcs,
                     const std::string& gaiaDbPath,
                     const FITSKeywordArray& keywords = FITSKeywordArray() );

// Plate solve: tries Gaia first, falls back to StellarSolver
bool PlateSolve( const Image& image, SimpleTANWCS& wcs,
                 const FITSKeywordArray& keywords = FITSKeywordArray() );
