// ****************************************************************************
// PixInsight Background Extraction Module
// ****************************************************************************
// BackgroundExtractionInterface.h - Process interface definition
// ****************************************************************************

#ifndef __BackgroundExtractionInterface_h
#define __BackgroundExtractionInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/ToolButton.h>
#include <pcl/PushButton.h>
#include <pcl/NumericControl.h>
#include <pcl/Label.h>
#include <pcl/ComboBox.h>
#include <pcl/CheckBox.h>
#include <pcl/SpinBox.h>
#include <pcl/GroupBox.h>
#include <pcl/Edit.h>
#include <pcl/ViewList.h>

#include "BackgroundExtractionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class BackgroundExtractionInterface : public ProcessInterface
{
public:

   BackgroundExtractionInterface();
   virtual ~BackgroundExtractionInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   BackgroundExtractionInstance m_instance;

   struct GUIData
   {
      GUIData( BackgroundExtractionInterface& );

      VerticalSizer     Global_Sizer;

      // Model Section
      SectionBar        Model_SectionBar;
      Control           Model_Control;
      VerticalSizer     Model_Sizer;
         HorizontalSizer   ModelType_Sizer;
            Label             ModelType_Label;
            ComboBox          ModelType_ComboBox;

      // Sample Generation Section
      SectionBar        Sampling_SectionBar;
      Control           Sampling_Control;
      VerticalSizer     Sampling_Sizer;
         HorizontalSizer   SampleGen_Sizer;
            Label             SampleGen_Label;
            ComboBox          SampleGen_ComboBox;
         NumericControl    Tolerance_NumericControl;
         NumericControl    Deviation_NumericControl;
         HorizontalSizer   SampleCount_Sizer;
            Label             MinSamples_Label;
            SpinBox           MinSamples_SpinBox;
            Label             MaxSamples_Label;
            SpinBox           MaxSamples_SpinBox;
         HorizontalSizer   GridSize_Sizer;
            Label             GridRows_Label;
            SpinBox           GridRows_SpinBox;
            Label             GridColumns_Label;
            SpinBox           GridColumns_SpinBox;

      // Outlier Rejection Section
      SectionBar        Rejection_SectionBar;
      Control           Rejection_Control;
      VerticalSizer     Rejection_Sizer;
         HorizontalSizer   RejectionEnabled_Sizer;
            CheckBox          RejectionEnabled_CheckBox;
         NumericControl    RejectionLow_NumericControl;
         NumericControl    RejectionHigh_NumericControl;
         HorizontalSizer   RejectionIter_Sizer;
            Label             RejectionIter_Label;
            SpinBox           RejectionIter_SpinBox;

      // Output Section
      SectionBar        Output_SectionBar;
      Control           Output_Control;
      VerticalSizer     Output_Sizer;
         HorizontalSizer   ReplaceTarget_Sizer;
            CheckBox          ReplaceTarget_CheckBox;
         HorizontalSizer   OutputBackground_Sizer;
            CheckBox          OutputBackground_CheckBox;
            CheckBox          ApplySTFToBackground_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateModelControls();
   void UpdateSamplingControls();
   void UpdateRejectionControls();
   void UpdateOutputControls();

   // Event Handlers
   void __Model_ItemSelected( ComboBox& sender, int itemIndex );
   void __SampleGen_ItemSelected( ComboBox& sender, int itemIndex );
   void __Tolerance_ValueUpdated( NumericEdit& sender, double value );
   void __Deviation_ValueUpdated( NumericEdit& sender, double value );
   void __MinSamples_ValueUpdated( SpinBox& sender, int value );
   void __MaxSamples_ValueUpdated( SpinBox& sender, int value );
   void __GridRows_ValueUpdated( SpinBox& sender, int value );
   void __GridColumns_ValueUpdated( SpinBox& sender, int value );
   void __RejectionEnabled_Click( Button& sender, bool checked );
   void __RejectionLow_ValueUpdated( NumericEdit& sender, double value );
   void __RejectionHigh_ValueUpdated( NumericEdit& sender, double value );
   void __RejectionIter_ValueUpdated( SpinBox& sender, int value );
   void __ReplaceTarget_Click( Button& sender, bool checked );
   void __OutputBackground_Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern BackgroundExtractionInterface* TheBackgroundExtractionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif   // __BackgroundExtractionInterface_h

// ----------------------------------------------------------------------------
