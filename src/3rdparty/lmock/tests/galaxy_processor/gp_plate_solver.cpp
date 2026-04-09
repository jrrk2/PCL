#include "gp_plate_solver.h"

bool PlateSolveGaia( const Image& image, SimpleTANWCS& wcs,
                     const std::string& gaiaDbPath,
                     const FITSKeywordArray& keywords )
{
   int w = image.Width();
   int h = image.Height();
   int ch = image.NumberOfChannels();

   std::cout << "  Plate solving via Gaia catalog matching...\n";

   // --- Step 1: Get initial position hint from FITS keywords ---
   double hintRA = -1, hintDec = -91;
   double hintPixscale = -1; // arcsec/px
   for ( const auto& kw : keywords )
   {
      IsoString name = kw.name.Trimmed();
      if ( name == "RA" && hintRA < 0 )
         hintRA = kw.value.Trimmed().ToDouble();
      else if ( name == "DEC" && hintDec < -90 )
         hintDec = kw.value.Trimmed().ToDouble();
      else if ( name == "OBJCTRA" && hintRA < 0 )
      {
         IsoString v = kw.value.Trimmed();
         v.DeleteChar( '\'' );
         StringList parts;
         v.Break( parts, ' ', true );
         if ( parts.Length() >= 3 )
            hintRA = ( parts[0].ToDouble() + parts[1].ToDouble() / 60 + parts[2].ToDouble() / 3600 ) * 15.0;
      }
      else if ( name == "OBJCTDEC" && hintDec < -90 )
      {
         IsoString v = kw.value.Trimmed();
         v.DeleteChar( '\'' );
         StringList parts;
         v.Break( parts, ' ', true );
         if ( parts.Length() >= 3 )
         {
            double d = std::abs( parts[0].ToDouble() );
            hintDec = d + parts[1].ToDouble() / 60 + parts[2].ToDouble() / 3600;
            if ( v.StartsWith( '-' ) || parts[0].ToDouble() < 0 )
               hintDec = -hintDec;
         }
      }
      else if ( name == "CDELT1" || name == "CDELT2" )
      {
         double cdelt = std::abs( kw.value.Trimmed().ToDouble() );
         if ( cdelt > 0 && hintPixscale < 0 )
            hintPixscale = cdelt * 3600.0;
      }
      else if ( name == "CD1_1" && hintPixscale < 0 )
      {
         double cd = std::abs( kw.value.Trimmed().ToDouble() );
         if ( cd > 0 )
            hintPixscale = cd * 3600.0;
      }
      else if ( ( name == "SCALE" || name == "SECPIX1" || name == "SECPIX2" ) && hintPixscale < 0 )
      {
         double s = kw.value.Trimmed().ToDouble();
         if ( s > 0 )
            hintPixscale = s; // already in arcsec/px
      }
   }

   // Compute pixscale from focal length + pixel size if not yet available
   if ( hintPixscale < 0 )
   {
      double focalLen = -1, pixSize = -1;
      for ( const auto& kw : keywords )
      {
         IsoString name = kw.name.Trimmed();
         if ( name == "FOCALLEN" )
            focalLen = kw.value.Trimmed().ToDouble();
         else if ( name == "PIXSIZE1" || name == "XPIXSZ" )
            pixSize = kw.value.Trimmed().ToDouble();
      }
      if ( focalLen > 0 && pixSize > 0 )
         hintPixscale = pixSize / focalLen * 206.265; // arcsec/px
   }

   // Also try to get pixscale from existing WCS
   if ( hintPixscale < 0 && wcs.valid )
      hintPixscale = wcs.Resolution() * 3600.0;
   if ( ( hintRA < 0 || hintDec < -90 ) && wcs.valid )
   {
      hintRA = wcs.crval1;
      hintDec = wcs.crval2;
   }

   if ( hintRA < 0 || hintRA > 360 || hintDec < -90 || hintDec > 90 )
   {
      std::cerr << "    No valid RA/Dec hint in header for Gaia solver\n";
      return false;
   }
   if ( hintPixscale <= 0 )
   {
      std::cerr << "    No pixel scale hint in header for Gaia solver\n";
      return false;
   }

   double resolution = hintPixscale / 3600.0; // degrees/pixel
   std::cout << "    Hint: RA=" << std::setprecision( 6 ) << hintRA
             << " Dec=" << hintDec
             << " scale=" << std::setprecision( 4 ) << hintPixscale << "\"/px\n";

   // --- Step 2: Detect stars via DoG (Difference of Gaussians) ---
   // Build luminance cv::Mat from image pixel data using PixelData() for efficiency
   cv::Mat gray( h, w, CV_32F );

   // Check if pixel data is accessible via PixelData() (fast path)
   bool pixelsValid = false;
   if ( ch >= 1 && image.PixelData( 0 ) != nullptr )
   {
      // Use direct pointer access
      int nc = std::min( ch, 3 );
      const float* planes[3] = {};
      for ( int c = 0; c < nc; ++c )
         planes[c] = image.PixelData( c );

      float invN = 1.0f / nc;
      for ( int y = 0; y < h; ++y )
         for ( int x = 0; x < w; ++x )
         {
            float lum = 0;
            int idx = y * w + x;
            for ( int c = 0; c < nc; ++c )
               lum += planes[c][idx];
            gray.at<float>( y, x ) = lum * invN;
         }
      pixelsValid = true;
   }

   if ( !pixelsValid )
   {
      // Fallback: use Pixel() accessor
      for ( int y = 0; y < h; ++y )
         for ( int x = 0; x < w; ++x )
         {
            float lum = 0;
            for ( int c = 0; c < std::min( ch, 3 ); ++c )
               lum += image.Pixel( x, y, c );
            gray.at<float>( y, x ) = lum / std::min( ch, 3 );
         }
   }

   // Diagnostic: show pixel range
   {
      double minV, maxV;
      cv::minMaxLoc( gray, &minV, &maxV );
      std::cout << "    Luminance range: " << minV << " .. " << maxV << "\n";
      if ( maxV == 0 )
      {
         std::cerr << "    WARNING: All pixels are zero — image data not loaded correctly\n";
         std::cerr << "    (likely UInt16 XISF format not converted to Float)\n";
         return false;
      }
   }
   // No auto-stretch needed: DoG is self-normalizing (background cancels in difference)

   // DoG star detection (same algorithm as stackengine)
   cv::Mat blur1, blur2, dog;
   cv::GaussianBlur( gray, blur1, cv::Size( 0, 0 ), 1.5 );
   cv::GaussianBlur( gray, blur2, cv::Size( 0, 0 ), 4.0 );
   dog = blur1 - blur2;

   // Robust threshold: median + 5*MAD (subsampled)
   std::vector<float> vals;
   vals.reserve( dog.rows * dog.cols / 4 );
   for ( int y = 0; y < dog.rows; y += 2 )
      for ( int x = 0; x < dog.cols; x += 2 )
         vals.push_back( dog.at<float>( y, x ) );
   std::nth_element( vals.begin(), vals.begin() + vals.size() / 2, vals.end() );
   float dogMedian = vals[vals.size() / 2];

   std::vector<float> absdev( vals.size() );
   for ( size_t i = 0; i < vals.size(); i++ )
      absdev[i] = std::abs( vals[i] - dogMedian );
   std::nth_element( absdev.begin(), absdev.begin() + absdev.size() / 2, absdev.end() );
   float dogMAD = absdev[absdev.size() / 2] * 1.4826f;
   float threshold = dogMedian + 5.0f * dogMAD;

   // Local maxima via dilation
   cv::Mat dilated;
   cv::dilate( dog, dilated, cv::getStructuringElement( cv::MORPH_RECT, cv::Size( 5, 5 ) ) );

   struct ImgStar { DPoint pos; float flux; };
   std::vector<ImgStar> imgStars;
   int border = 20;
   for ( int y = border; y < dog.rows - border; y++ )
   {
      const float* pd = dog.ptr<float>( y );
      const float* pdi = dilated.ptr<float>( y );
      for ( int x = border; x < dog.cols - border; x++ )
      {
         if ( pd[x] > threshold && pd[x] == pdi[x] )
         {
            // Sub-pixel centroid via weighted moments
            double sx = 0, sy = 0, sw = 0;
            for ( int dy = -2; dy <= 2; dy++ )
               for ( int dx = -2; dx <= 2; dx++ )
               {
                  float wt = dog.at<float>( y + dy, x + dx ) - dogMedian;
                  if ( wt > 0 ) { sx += wt * ( x + dx ); sy += wt * ( y + dy ); sw += wt; }
               }
            if ( sw > 0 )
               imgStars.push_back( { DPoint( sx / sw, sy / sw ), pd[x] } );
         }
      }
   }

   // Sort by flux descending
   std::sort( imgStars.begin(), imgStars.end(),
              []( const ImgStar& a, const ImgStar& b ) { return a.flux > b.flux; } );
   if ( int( imgStars.size() ) > 2000 )
      imgStars.resize( 2000 );

   std::cout << "    Detected " << imgStars.size() << " stars via DoG\n";

   if ( int( imgStars.size() ) < 10 )
   {
      std::cerr << "    Too few stars detected\n";
      return false;
   }

   int maxStars = std::min( int( imgStars.size() ), 2000 );

   // --- Step 3: Query Gaia catalog ---
   double fieldRadius = resolution * std::max( w, h ) * 0.75; // degrees

   GaiaDatabaseFile gaiaDB( String::UTF8ToUTF16( gaiaDbPath.c_str() ) );

   // Estimate limit magnitude from detected star count and field size
   double fov = resolution * std::max( w, h );
   float limitMag = float( std::min( 20.0, std::max( 7.0, 14.5 * std::pow( fov, -0.179 ) ) ) );

   GaiaSearchData search;
   search.centerRA = hintRA;
   search.centerDec = hintDec;
   search.radius = fieldRadius;
   search.magnitudeLow = -1.5;
   search.magnitudeHigh = limitMag;
   search.exclusionFlags = GaiaStarFlag::NoPM; // require proper motion data
   search.sourceLimit = uint32( maxStars * 2 );
   gaiaDB.Search( search );

   std::cout << "    Gaia catalog: " << search.stars.Length() << " stars"
             << " (mag limit " << std::setprecision( 1 ) << limitMag
             << ", radius " << std::setprecision( 2 ) << fieldRadius << " deg)\n";

   if ( search.stars.Length() < 10 )
   {
      std::cerr << "    Too few Gaia stars found\n";
      return false;
   }

   // --- Step 4: Project catalog stars onto image plane ---
   // Use gnomonic (TAN) projection centered on hint coordinates
   GnomonicProjection projection( hintRA, hintDec );

   // Build linear ref_S_G matrix: maps image pixels to gnomonic coordinates
   // Initially: pixel (w/2, h/2) -> (0,0) in gnomonic, scale = resolution
   // Gnomonic coords are in degrees; projection origin is at (hintRA, hintDec)
   // ref_S_G maps from image (S) to gnomonic (G):
   //   G = ref_S_G * S, where S = (px, py, 1)
   // Initially assume no rotation, standard CCD parity (RA increases left)
   double scale = resolution; // deg/px
   // [xi]   = [-scale    0   scale*w/2 ] * [px]
   // [eta]     [  0    -scale scale*h/2]   [py]
   //                                       [ 1]

   // Project Gaia stars to initial image coordinates
   struct CatStar { DPoint imgPos; DPoint rdPos; DPoint gnomonic; float mag; };
   std::vector<CatStar> catStars;
   catStars.reserve( search.stars.Length() );

   for ( const auto& gs : search.stars )
   {
      DPoint pW; // gnomonic coords (degrees)
      DPoint pRD( gs.ra, gs.dec );
      if ( !projection.Direct( pW, pRD ) )
         continue;

      // Convert gnomonic coords to approximate pixel coords
      // pW is in degrees from projection center
      double px = w / 2.0 - pW.x / scale;
      double py = h / 2.0 - pW.y / scale;

      // Keep stars generously (scale might be wrong by 2x)
      if ( px >= -w * 0.5 && px <= w * 1.5 && py >= -h * 0.5 && py <= h * 1.5 )
         catStars.push_back( { DPoint( px, py ), pRD, pW, gs.magG } );
   }

   std::cout << "    Catalog stars projected to image: " << catStars.size() << "\n";

   if ( catStars.size() < 6 )
   {
      std::cerr << "    Too few catalog stars project into image\n";
      return false;
   }

   // --- Step 5: Triangle-based star matching ---
   // Build triangles from brightest N stars in both sets and match by
   // side-ratio invariants (scale-invariant, rotation-invariant)

   // Sort image stars by brightness (already sorted by StarDetector)
   int nImg = std::min( maxStars, int( imgStars.size() ) );
   // Sort catalog stars by magnitude (brightest first)
   std::sort( catStars.begin(), catStars.end(),
              []( const CatStar& a, const CatStar& b ) { return a.mag < b.mag; } );
   int nCat = std::min( maxStars, int( catStars.size() ) );

   // Use top N stars for triangle matching
   int nMatch = std::min( 100, std::min( nImg, nCat ) );

   // Build triangle descriptors: for each triple (i,j,k), compute sorted
   // side ratios r1 = d_mid/d_max, r2 = d_min/d_max
   struct Triangle {
      int idx[3];
      double r1, r2; // sorted side ratios
   };

   auto buildTriangles = [&]( const auto& stars, int n, auto getPos ) -> std::vector<Triangle>
   {
      std::vector<Triangle> tris;
      int limit = std::min( n, 40 ); // use top 40 stars for triangle formation
      tris.reserve( limit * ( limit - 1 ) * ( limit - 2 ) / 6 );
      for ( int i = 0; i < limit; ++i )
         for ( int j = i + 1; j < limit; ++j )
            for ( int k = j + 1; k < limit; ++k )
            {
               DPoint pi = getPos( i ), pj = getPos( j ), pk = getPos( k );
               // Sides: d_ij opposite k, d_jk opposite i, d_ki opposite j
               double d_ij = std::sqrt( ( pi.x - pj.x ) * ( pi.x - pj.x ) + ( pi.y - pj.y ) * ( pi.y - pj.y ) );
               double d_jk = std::sqrt( ( pj.x - pk.x ) * ( pj.x - pk.x ) + ( pj.y - pk.y ) * ( pj.y - pk.y ) );
               double d_ki = std::sqrt( ( pk.x - pi.x ) * ( pk.x - pi.x ) + ( pk.y - pi.y ) * ( pk.y - pi.y ) );

               // Sort (side, opposite_vertex) by side length
               struct SV { double side; int vtx; };
               SV sv[3] = { { d_ij, k }, { d_jk, i }, { d_ki, j } };
               std::sort( sv, sv + 3, []( const SV& a, const SV& b ) { return a.side < b.side; } );

               if ( sv[2].side < 1e-6 ) continue; // degenerate
               Triangle t;
               // idx[n] = vertex opposite the n-th sorted side
               t.idx[0] = sv[0].vtx; // opposite shortest side
               t.idx[1] = sv[1].vtx; // opposite middle side
               t.idx[2] = sv[2].vtx; // opposite longest side
               t.r1 = sv[1].side / sv[2].side; // mid/max
               t.r2 = sv[0].side / sv[2].side; // min/max
               tris.push_back( t );
            }
      return tris;
   };

   auto imgTriangles = buildTriangles( imgStars, nMatch,
      [&]( int i ) -> DPoint { return imgStars[i].pos; } );
   auto catTriangles = buildTriangles( catStars, nMatch,
      [&]( int i ) -> DPoint { return catStars[i].imgPos; } );

   std::cout << "    Triangles: " << imgTriangles.size() << " image, "
             << catTriangles.size() << " catalog\n";

   // --- Step 6: Triangle RANSAC plate solution ---
   // For each candidate triangle match (similar ratios), compute the affine
   // transform from the 3 image stars to catalog gnomonic coords, then count
   // how many other detected stars land near a catalog star under that transform.
   double ratioTol = 0.03; // tolerance on side ratios

   struct MatchedPair { DPoint imgPos; DPoint gnomonic; DPoint rdPos; };
   int bestInlierCount = 0;
   cv::Mat bestSolX, bestSolY;
   std::vector<MatchedPair> bestInlierPairs;

   // Sort catalog triangles by r1 for binary search
   std::sort( catTriangles.begin(), catTriangles.end(),
              []( const Triangle& a, const Triangle& b ) { return a.r1 < b.r1; } );

   int triMatchCount = 0;
   for ( const auto& it : imgTriangles )
   {
      // Binary search for catalog triangles with r1 in [it.r1-tol, it.r1+tol]
      auto lo = std::lower_bound( catTriangles.begin(), catTriangles.end(), it.r1 - ratioTol,
         []( const Triangle& t, double v ) { return t.r1 < v; } );
      auto hi = std::upper_bound( catTriangles.begin(), catTriangles.end(), it.r1 + ratioTol,
         []( double v, const Triangle& t ) { return v < t.r1; } );

      for ( auto ctIt = lo; ctIt != hi; ++ctIt )
      {
         const auto& ct = *ctIt;
         if ( std::abs( it.r2 - ct.r2 ) > ratioTol )
            continue;
         triMatchCount++;

         // For each triangle match, try the direct vertex correspondence
         // (vertices are ordered by opposite side length, so idx[n]->idx[n])
         DPoint imgPts[3], catGn[3];
         for ( int v = 0; v < 3; ++v )
         {
            imgPts[v] = imgStars[it.idx[v]].pos;
            catGn[v] = catStars[ct.idx[v]].gnomonic;
         }

         // Fit affine: gnomonic = A * pixel + b
         cv::Mat A( 3, 3, CV_64F ), bx( 3, 1, CV_64F ), by( 3, 1, CV_64F );
         for ( int v = 0; v < 3; ++v )
         {
            A.at<double>( v, 0 ) = imgPts[v].x;
            A.at<double>( v, 1 ) = imgPts[v].y;
            A.at<double>( v, 2 ) = 1.0;
            bx.at<double>( v, 0 ) = catGn[v].x;
            by.at<double>( v, 0 ) = catGn[v].y;
         }
         cv::Mat sx, sy;
         if ( !cv::solve( A, bx, sx, cv::DECOMP_SVD ) ||
              !cv::solve( A, by, sy, cv::DECOMP_SVD ) )
            continue;

         // Validate: pixel scale from this transform should be reasonable
         double cd11 = sx.at<double>( 0 ), cd12 = sx.at<double>( 1 );
         double cd21 = sy.at<double>( 0 ), cd22 = sy.at<double>( 1 );
         double det = cd11 * cd22 - cd12 * cd21;
         double solvedScale = std::sqrt( std::abs( det ) ) * 3600.0; // arcsec/px
         // Accept if scale is between 0.1 and 30 arcsec/px
         if ( solvedScale < 0.1 || solvedScale > 30.0 )
            continue;

         // Count inliers: for each detected image star, predict gnomonic coords,
         // then check if any catalog star is nearby
         double matchThresh = solvedScale * 5.0 / 3600.0; // 5 pixels in degrees
         double matchThresh2 = matchThresh * matchThresh;
         int inlierCount = 0;
         std::vector<MatchedPair> inlierPairs;

         for ( int si = 0; si < std::min( 200, nImg ); ++si )
         {
            DPoint ip = imgStars[si].pos;
            double gx = cd11 * ip.x + cd12 * ip.y + sx.at<double>( 2 );
            double gy = cd21 * ip.x + cd22 * ip.y + sy.at<double>( 2 );

            double bestD2 = matchThresh2;
            int bestCat = -1;
            for ( int ci = 0; ci < nCat; ++ci )
            {
               double dx = gx - catStars[ci].gnomonic.x;
               double dy = gy - catStars[ci].gnomonic.y;
               double d2 = dx * dx + dy * dy;
               if ( d2 < bestD2 ) { bestD2 = d2; bestCat = ci; }
            }
            if ( bestCat >= 0 )
            {
               inlierCount++;
               inlierPairs.push_back( { ip, catStars[bestCat].gnomonic, catStars[bestCat].rdPos } );
            }
         }

         if ( inlierCount > bestInlierCount )
         {
            bestInlierCount = inlierCount;
            bestSolX = sx.clone();
            bestSolY = sy.clone();
            bestInlierPairs = std::move( inlierPairs );
         }

         // Early exit if we found a good solution
         if ( bestInlierCount > nCat / 2 )
            goto triangleMatchDone;
      }
   }
   triangleMatchDone:;

   std::cout << "    Triangle matches tested: " << triMatchCount
             << ", best inliers: " << bestInlierCount << "\n";

   if ( bestInlierCount < 6 )
   {
      std::cerr << "    Too few inliers from triangle matching\n";
      return false;
   }

   // Refit using all inlier pairs
   {
      int n = int( bestInlierPairs.size() );
      cv::Mat A( n, 3, CV_64F ), bx( n, 1, CV_64F ), by( n, 1, CV_64F );
      for ( int i = 0; i < n; ++i )
      {
         A.at<double>( i, 0 ) = bestInlierPairs[i].imgPos.x;
         A.at<double>( i, 1 ) = bestInlierPairs[i].imgPos.y;
         A.at<double>( i, 2 ) = 1.0;
         bx.at<double>( i, 0 ) = bestInlierPairs[i].gnomonic.x;
         by.at<double>( i, 0 ) = bestInlierPairs[i].gnomonic.y;
      }
      cv::solve( A, bx, bestSolX, cv::DECOMP_SVD );
      cv::solve( A, by, bestSolY, cv::DECOMP_SVD );
   }

   // --- Step 7: Extract WCS from linear fit ---
   // gnomonic_x = cd11 * px + cd12 * py + offset_x
   // gnomonic_y = cd21 * px + cd22 * py + offset_y
   wcs.cd11 = bestSolX.at<double>( 0 );
   wcs.cd12 = bestSolX.at<double>( 1 );
   wcs.cd21 = bestSolY.at<double>( 0 );
   wcs.cd22 = bestSolY.at<double>( 1 );
   wcs.crpix1 = w / 2.0;
   wcs.crpix2 = h / 2.0;

   // Gnomonic coords at crpix
   double gCx = wcs.cd11 * wcs.crpix1 + wcs.cd12 * wcs.crpix2 + bestSolX.at<double>( 2 );
   double gCy = wcs.cd21 * wcs.crpix1 + wcs.cd22 * wcs.crpix2 + bestSolY.at<double>( 2 );

   DPoint centerRD;
   if ( !projection.Inverse( centerRD, DPoint( gCx, gCy ) ) )
   {
      std::cerr << "    Failed to inverse-project center\n";
      return false;
   }
   wcs.crval1 = centerRD.x;
   wcs.crval2 = centerRD.y;
   wcs.valid = true;

   // --- Step 8: Iterative refinement ---
   // Re-match using the new WCS: predict catalog star positions, find nearest
   // detected star, refit
   for ( int iter = 0; iter < 3; ++iter )
   {
      // Re-project catalog stars using current WCS
      std::vector<std::pair<DPoint, DPoint>> refinedPairs; // (imgPos, catGnomonic)
      double matchRadius = std::max( 3.0, 10.0 / ( iter + 1 ) ); // pixels, tighten each iteration

      // Build a new gnomonic projection centered on the current solution
      GnomonicProjection refProj( wcs.crval1, wcs.crval2 );

      for ( const auto& gs : search.stars )
      {
         // Project catalog star to pixel coords using current WCS
         double px, py;
         if ( !wcs.WorldToPixel( gs.ra, gs.dec, px, py ) )
            continue;
         if ( px < -50 || px > w + 50 || py < -50 || py > h + 50 )
            continue;

         // Find nearest detected star
         double bestDist2 = matchRadius * matchRadius;
         int bestIdx = -1;
         for ( int j = 0; j < nImg; ++j )
         {
            double dx = imgStars[j].pos.x - px;
            double dy = imgStars[j].pos.y - py;
            double d2 = dx * dx + dy * dy;
            if ( d2 < bestDist2 )
            {
               bestDist2 = d2;
               bestIdx = j;
            }
         }
         if ( bestIdx >= 0 )
         {
            DPoint pW;
            if ( refProj.Direct( pW, DPoint( gs.ra, gs.dec ) ) )
               refinedPairs.push_back( { imgStars[bestIdx].pos, pW } );
         }
      }

      if ( int( refinedPairs.size() ) < 6 )
         break;

      // Fit new linear transformation: image pixels -> gnomonic coords
      // Using least-squares on the inlier set
      // G = A * I + b, where G = (xi, eta), I = (px, py)
      int n = int( refinedPairs.size() );
      cv::Mat A( n, 3, CV_64F ), bx( n, 1, CV_64F ), by( n, 1, CV_64F );
      for ( int i = 0; i < n; ++i )
      {
         A.at<double>( i, 0 ) = refinedPairs[i].first.x;
         A.at<double>( i, 1 ) = refinedPairs[i].first.y;
         A.at<double>( i, 2 ) = 1.0;
         bx.at<double>( i, 0 ) = refinedPairs[i].second.x;
         by.at<double>( i, 0 ) = refinedPairs[i].second.y;
      }

      cv::Mat solX, solY;
      if ( !cv::solve( A, bx, solX, cv::DECOMP_SVD ) ||
           !cv::solve( A, by, solY, cv::DECOMP_SVD ) )
         break;

      // Update WCS: CD matrix from the linear fit coefficients
      // G_x = solX[0]*px + solX[1]*py + solX[2]
      // G_y = solY[0]*px + solY[1]*py + solY[2]
      // At center pixel (crpix), G should be (0,0) since projection is centered there
      wcs.cd11 = solX.at<double>( 0, 0 );
      wcs.cd12 = solX.at<double>( 1, 0 );
      wcs.cd21 = solY.at<double>( 0, 0 );
      wcs.cd22 = solY.at<double>( 1, 0 );

      // The gnomonic center offset tells us the updated center coords
      double gCx = solX.at<double>( 2, 0 ) + solX.at<double>( 0, 0 ) * ( w / 2.0 ) + solX.at<double>( 1, 0 ) * ( h / 2.0 );
      double gCy = solY.at<double>( 2, 0 ) + solY.at<double>( 0, 0 ) * ( w / 2.0 ) + solY.at<double>( 1, 0 ) * ( h / 2.0 );

      DPoint newCenter;
      if ( refProj.Inverse( newCenter, DPoint( gCx, gCy ) ) )
      {
         wcs.crval1 = newCenter.x;
         wcs.crval2 = newCenter.y;
      }
      wcs.crpix1 = w / 2.0;
      wcs.crpix2 = h / 2.0;

      // Recenter the CD matrix so that pixel (crpix) maps to gnomonic (0,0)
      // With the new projection center, we need: cd11*0 + cd12*0 + offset = 0
      // The offset is absorbed into crval, so just keep cd11..cd22

      // Compute RMS error
      double rmsErr = 0;
      for ( const auto& [ip, gp] : refinedPairs )
      {
         double predX = wcs.cd11 * ( ip.x - wcs.crpix1 ) + wcs.cd12 * ( ip.y - wcs.crpix2 );
         double predY = wcs.cd21 * ( ip.x - wcs.crpix1 ) + wcs.cd22 * ( ip.y - wcs.crpix2 );
         // Need gnomonic coords relative to new center
         DPoint pW;
         GnomonicProjection finalProj( wcs.crval1, wcs.crval2 );
         // The paired gnomonic coords are relative to refProj center, not the new center
         // Re-project the catalog star
         DPoint catRD;
         refProj.Inverse( catRD, gp );
         finalProj.Direct( pW, catRD );
         double ex = predX - pW.x, ey = predY - pW.y;
         rmsErr += ex * ex + ey * ey;
      }
      rmsErr = std::sqrt( rmsErr / n ) * 3600.0; // arcsec

      std::cout << "    Iteration " << ( iter + 1 ) << ": "
                << refinedPairs.size() << " matched stars, RMS "
                << std::setprecision( 3 ) << rmsErr << "\"\n";
   }

   // Compute final pixel scale from CD matrix
   double det = wcs.cd11 * wcs.cd22 - wcs.cd12 * wcs.cd21;
   double finalPixscale = std::sqrt( std::abs( det ) ) * 3600.0;
   double orientation = std::atan2( wcs.cd21, wcs.cd11 ) * 180.0 / M_PI;

   std::cout << "    Gaia plate solve SUCCESS\n";
   std::cout << "    RA=" << std::setprecision( 6 ) << wcs.crval1
             << " Dec=" << wcs.crval2
             << " pixscale=" << std::setprecision( 4 ) << finalPixscale << " arcsec/px"
             << " orient=" << std::setprecision( 1 ) << orientation << " deg\n";

   return true;
}

bool PlateSolve( const Image& image, SimpleTANWCS& wcs,
                 const FITSKeywordArray& keywords )
{
   // Try Gaia catalog solver first (PI technique), fall back to StellarSolver
   if ( !g_gaiaAstroDbPath.empty() )
   {
      try
      {
         if ( PlateSolveGaia( image, wcs, g_gaiaAstroDbPath, keywords ) )
            return true;
      }
      catch ( const Exception& e )
      {
         std::cerr << "  Gaia solver error: " << e.Message().ToUTF8() << "\n";
      }
      catch ( const std::exception& e )
      {
         std::cerr << "  Gaia solver error: " << e.what() << "\n";
      }
      std::cout << "  Falling back to StellarSolver...\n";
   }

   int w = image.Width();
   int h = image.Height();
   int ch = image.NumberOfChannels();

   std::cout << "  Plate solving via StellarSolver...\n";

   // Create luminance grayscale and convert to uint8
   std::vector<float> luminance( w * h );
   float minVal = 1e30f, maxVal = -1e30f;
   for ( int y = 0; y < h; ++y )
      for ( int x = 0; x < w; ++x )
      {
         float lum = 0;
         for ( int c = 0; c < std::min( ch, 3 ); ++c )
            lum += image.Pixel( x, y, c );
         lum /= std::min( ch, 3 );
         luminance[y * w + x] = lum;
         if ( lum < minVal ) minVal = lum;
         if ( lum > maxVal ) maxVal = lum;
      }

   // Downsample 2x and convert to uint8 for solver
   int dw = w / 2, dh = h / 2;
   std::vector<uint8_t> buffer( dw * dh );
   float range = maxVal - minVal;
   if ( range > 0 )
   {
      for ( int y = 0; y < dh; ++y )
         for ( int x = 0; x < dw; ++x )
         {
            int sx = x * 2, sy = y * 2;
            float avg = ( luminance[sy * w + sx] + luminance[sy * w + sx + 1] +
                          luminance[( sy + 1 ) * w + sx] + luminance[( sy + 1 ) * w + sx + 1] ) * 0.25f;
            float norm = ( avg - minVal ) / range;
            buffer[y * dw + x] = uint8_t( std::max( 0.0f, std::min( 255.0f, norm * 255.0f ) ) );
         }
   }

   // Configure statistics
   FITSImage::Statistic stats{};
   stats.width = dw;
   stats.height = dh;
   stats.channels = 1;
   stats.dataType = TBYTE;
   stats.bytesPerPixel = 1;
   stats.min[0] = minVal;
   stats.max[0] = maxVal;
   stats.mean[0] = ( minVal + maxVal ) / 2.0;
   stats.SNR = 1.0;

   // Create solver
   StellarSolver solver( stats, buffer.data() );

   // Configure parameters
   QList<Parameters> profiles = StellarSolver::getBuiltInProfiles();
   if ( profiles.isEmpty() )
   {
      std::cerr << "  ERROR: No StellarSolver parameter profiles available\n";
      return false;
   }
   Parameters params = profiles.at( 0 );
   params.search_radius = 10.0;
   params.minwidth = 0.1;
   params.maxwidth = 10.0;
   params.resort = true;
   params.autoDownsample = false;
   params.downsample = 1;
   params.inParallel = true;
   params.solverTimeLimit = 120;
   params.initialKeep = 2000;
   params.keepNum = 500;
   solver.setParameters( params );

   // Set position hint from FITS keywords if available
   double hintRA = -1, hintDec = -91;
   for ( const auto& kw : keywords )
   {
      IsoString name = kw.name.Trimmed();
      if ( name == "RA" && hintRA < 0 )
         hintRA = kw.value.Trimmed().ToDouble();
      else if ( name == "DEC" && hintDec < -90 )
         hintDec = kw.value.Trimmed().ToDouble();
      else if ( name == "OBJCTRA" && hintRA < 0 )
      {
         // Parse "hh mm ss" format
         IsoString v = kw.value.Trimmed();
         v.DeleteChar( '\'' );
         StringList parts;
         v.Break( parts, ' ', true );
         if ( parts.Length() >= 3 )
            hintRA = ( parts[0].ToDouble() + parts[1].ToDouble() / 60 + parts[2].ToDouble() / 3600 ) * 15.0;
      }
      else if ( name == "OBJCTDEC" && hintDec < -90 )
      {
         IsoString v = kw.value.Trimmed();
         v.DeleteChar( '\'' );
         StringList parts;
         v.Break( parts, ' ', true );
         if ( parts.Length() >= 3 )
         {
            double d = std::abs( parts[0].ToDouble() );
            hintDec = d + parts[1].ToDouble() / 60 + parts[2].ToDouble() / 3600;
            if ( v.StartsWith( '-' ) || parts[0].ToDouble() < 0 )
               hintDec = -hintDec;
         }
      }
   }
   if ( hintRA >= 0 && hintRA <= 360 && hintDec >= -90 && hintDec <= 90 )
   {
      solver.setSearchPositionInDegrees( hintRA, hintDec );
      std::cout << "    Position hint: RA=" << std::setprecision( 4 ) << hintRA
                << " Dec=" << hintDec << "\n";
   }

   // Find index files — search standard locations on macOS and Linux
   QStringList indexPaths;
   QStringList searchDirs = {
      "/Users/jonathan/kstars/astrometry",
      "/opt/homebrew/share/astrometry",
      "/usr/local/share/astrometry",
      "/usr/share/astrometry",
      QDir::homePath() + "/.local/share/kstars/astrometry",
      QDir::homePath() + "/kstars/astrometry",
      QDir::homePath() + "/.local/share/astrometry"
   };
   for ( const auto& dir : searchDirs )
   {
      QDir d( dir );
      if ( d.exists() )
         indexPaths << dir;
   }
   if ( indexPaths.isEmpty() )
   {
      std::cerr << "  ERROR: No astrometry index file directories found\n"
                << "  Searched:\n";
      for ( const auto& dir : searchDirs )
         std::cerr << "    " << dir.toStdString() << "\n";
      return false;
   }
   solver.setIndexFolderPaths( indexPaths );

   // Set solver mode
   solver.setProperty( "ProcessType", SSolver::SOLVE );
   solver.setProperty( "ExtractorType", SSolver::EXTRACTOR_INTERNAL );
   solver.setProperty( "SolverType", SSolver::SOLVER_STELLARSOLVER );

   // Synchronous solve
   solver.solve();

   if ( !solver.solvingDone() || !solver.hasWCSData() )
   {
      std::cerr << "  ERROR: Plate solve failed\n";
      return false;
   }

   FITSImage::Solution solution = solver.getSolution();
   double pixscale = solution.pixscale / 2.0; // correct for our 2x manual downsample

   // Store WCSData for accurate pixel-to-sky transforms
   g_solvedWCS = std::make_unique<WCSData>( solver.getWCSData() );

   // Populate SimpleTANWCS with proper rotation from plate solve.
   // StellarSolver orientation = position angle of "up" measured East of North.
   // StellarSolver pixel coords are 0-indexed top-down (same as PCL).
   // We store CRPIX in PCL convention (0-indexed, y=0 at top) since PixelToWorld
   // and WorldToPixel will be called with those coordinates.
   wcs.crval1 = solution.ra;
   wcs.crval2 = solution.dec;
   wcs.crpix1 = w / 2.0;
   wcs.crpix2 = h / 2.0;
   double degPerPix = pixscale / 3600.0;
   double theta = solution.orientation * M_PI / 180.0;
   // Negative parity (standard CCD): RA increases to the left.
   // For top-down image (y increases downward), the CD matrix is:
   wcs.cd11 = -degPerPix * cos( theta );
   wcs.cd12 = degPerPix * sin( theta );
   wcs.cd21 = degPerPix * sin( theta );
   wcs.cd22 = degPerPix * cos( theta );
   wcs.valid = true;

   std::cout << "    Plate solve SUCCESS\n";
   std::cout << "    RA=" << std::setprecision( 6 ) << solution.ra
             << " Dec=" << solution.dec
             << " pixscale=" << std::setprecision( 4 ) << pixscale << " arcsec/px"
             << " orient=" << solution.orientation << " deg\n";

   return true;
}
