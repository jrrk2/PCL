// ****************************************************************************
// PixInsight Background Extraction Module
// ****************************************************************************
// BackgroundExtractionModule.h - Module definition
// ****************************************************************************

#ifndef __BackgroundExtractionModule_h
#define __BackgroundExtractionModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class BackgroundExtractionModule : public MetaModule
{
public:

   BackgroundExtractionModule();
   virtual ~BackgroundExtractionModule();

   const char* Version() const override;
   IsoString Name() const override;
   String Description() const override;
   String Company() const override;
   String Author() const override;
   String Copyright() const override;
   String TradeMarks() const override;
   String OriginalFileName() const override;
   void GetReleaseDate( int& year, int& month, int& day ) const override;
};

// ----------------------------------------------------------------------------

extern BackgroundExtractionModule* TheBackgroundExtractionModule;

// ----------------------------------------------------------------------------

} // namespace pcl

#endif   // __BackgroundExtractionModule_h

// ----------------------------------------------------------------------------
