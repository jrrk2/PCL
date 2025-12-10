// ****************************************************************************
// PixInsight Background Extraction Module
// ****************************************************************************
// BackgroundExtractionInterface.cpp - Process interface implementation
// ****************************************************************************

#include "BackgroundExtractionInterface.h"
#include "BackgroundExtractionProcess.h"

#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BackgroundExtractionInterface* TheBackgroundExtractionInterface = nullptr;

// ----------------------------------------------------------------------------

BackgroundExtractionInterface::BackgroundExtractionInterface()
   : m_instance( TheBackgroundExtractionProcess )
{
   TheBackgroundExtractionInterface = this;
}

// ----------------------------------------------------------------------------

BackgroundExtractionInterface::~BackgroundExtractionInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString BackgroundExtractionInterface::Id() const
{
   return "BackgroundExtraction";
}

// ----------------------------------------------------------------------------

MetaProcess* BackgroundExtractionInterface::Process() const
{
   return TheBackgroundExtractionProcess;
}

// ----------------------------------------------------------------------------

String BackgroundExtractionInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/BackgroundExtraction.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures BackgroundExtractionInterface::Features() const
{
   return InterfaceFeature::Default;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::ResetInstance()
{
   BackgroundExtractionInstance defaultInstance( TheBackgroundExtractionProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool BackgroundExtractionInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Background Extraction" );
      UpdateControls();
   }

   dynamic = false;
   return true;
}

// ----------------------------------------------------------------------------

ProcessImplementation* BackgroundExtractionInterface::NewProcess() const
{
   return new BackgroundExtractionInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool BackgroundExtractionInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const BackgroundExtractionInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a BackgroundExtraction instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool BackgroundExtractionInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool BackgroundExtractionInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// GUI
// ----------------------------------------------------------------------------

BackgroundExtractionInterface::GUIData::GUIData( BackgroundExtractionInterface& w )
{
   // Model Section
   ModelType_Label.SetText( "Background Model:" );
   ModelType_Label.SetMinWidth( w.Font().Width( "Background Model:" + String( 'M', 2 ) ) );
   ModelType_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ModelType_ComboBox.AddItem( "Linear" );
   ModelType_ComboBox.AddItem( "2nd Degree Polynomial" );
   ModelType_ComboBox.AddItem( "3rd Degree Polynomial" );
   ModelType_ComboBox.AddItem( "Radial Basis Function" );
   ModelType_ComboBox.AddItem( "Gradient Domain" );
   ModelType_ComboBox.SetToolTip( "<p>Background model type.</p>"
      "<p><b>Linear</b>: Simple planar fit.</p>"
      "<p><b>Polynomial</b>: 2nd or 3rd degree polynomial surface.</p>"
      "<p><b>RBF</b>: Radial basis function interpolation.</p>"
      "<p><b>Gradient Domain</b>: Advanced gradient domain reconstruction.</p>" );
   ModelType_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&BackgroundExtractionInterface::__Model_ItemSelected, w );

   ModelType_Sizer.SetSpacing( 4 );
   ModelType_Sizer.Add( ModelType_Label );
   ModelType_Sizer.Add( ModelType_ComboBox, 100 );

   Model_Sizer.SetSpacing( 4 );
   Model_Sizer.Add( ModelType_Sizer );

   Model_Control.SetSizer( Model_Sizer );

   Model_SectionBar.SetTitle( "Background Model" );
   Model_SectionBar.SetSection( Model_Control );

   // Sampling Section
   SampleGen_Label.SetText( "Sample Generation:" );
   SampleGen_Label.SetMinWidth( w.Font().Width( "Sample Generation:" + String( 'M', 2 ) ) );
   SampleGen_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   SampleGen_ComboBox.AddItem( "Automatic" );
   SampleGen_ComboBox.AddItem( "Manual" );
   SampleGen_ComboBox.AddItem( "Grid" );
   SampleGen_ComboBox.AddItem( "Gradient Analysis" );
   SampleGen_ComboBox.SetToolTip( "<p>Method for generating background samples.</p>" );
   SampleGen_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&BackgroundExtractionInterface::__SampleGen_ItemSelected, w );

   SampleGen_Sizer.SetSpacing( 4 );
   SampleGen_Sizer.Add( SampleGen_Label );
   SampleGen_Sizer.Add( SampleGen_ComboBox, 100 );

   Tolerance_NumericControl.label.SetText( "Tolerance:" );
   Tolerance_NumericControl.label.SetFixedWidth( w.Font().Width( "Tolerance:" + String( 'M', 2 ) ) );
   Tolerance_NumericControl.slider.SetRange( 0, 100 );
   Tolerance_NumericControl.SetReal();
   Tolerance_NumericControl.SetRange( TheBEToleranceParameter->MinimumValue(), TheBEToleranceParameter->MaximumValue() );
   Tolerance_NumericControl.SetPrecision( TheBEToleranceParameter->Precision() );
   Tolerance_NumericControl.edit.SetFixedWidth( w.Font().Width( "88888.88" ) );
   Tolerance_NumericControl.SetToolTip( "<p>Outlier detection tolerance in sigma units.</p>" );
   Tolerance_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BackgroundExtractionInterface::__Tolerance_ValueUpdated, w );

   Deviation_NumericControl.label.SetText( "Deviation:" );
   Deviation_NumericControl.label.SetFixedWidth( w.Font().Width( "Deviation:" + String( 'M', 2 ) ) );
   Deviation_NumericControl.slider.SetRange( 0, 100 );
   Deviation_NumericControl.SetReal();
   Deviation_NumericControl.SetRange( TheBEDeviationParameter->MinimumValue(), TheBEDeviationParameter->MaximumValue() );
   Deviation_NumericControl.SetPrecision( TheBEDeviationParameter->Precision() );
   Deviation_NumericControl.edit.SetFixedWidth( w.Font().Width( "88888.88" ) );
   Deviation_NumericControl.SetToolTip( "<p>Sample deviation threshold in sigma units.</p>" );
   Deviation_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BackgroundExtractionInterface::__Deviation_ValueUpdated, w );

   MinSamples_Label.SetText( "Min Samples:" );
   MinSamples_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   
   MinSamples_SpinBox.SetRange( int( TheBEMinSamplesParameter->MinimumValue() ), int( TheBEMinSamplesParameter->MaximumValue() ) );
   MinSamples_SpinBox.SetToolTip( "<p>Minimum number of samples to generate.</p>" );
   MinSamples_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&BackgroundExtractionInterface::__MinSamples_ValueUpdated, w );

   MaxSamples_Label.SetText( "Max Samples:" );
   MaxSamples_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   
   MaxSamples_SpinBox.SetRange( int( TheBEMaxSamplesParameter->MinimumValue() ), int( TheBEMaxSamplesParameter->MaximumValue() ) );
   MaxSamples_SpinBox.SetToolTip( "<p>Maximum number of samples to generate.</p>" );
   MaxSamples_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&BackgroundExtractionInterface::__MaxSamples_ValueUpdated, w );

   SampleCount_Sizer.SetSpacing( 4 );
   SampleCount_Sizer.Add( MinSamples_Label );
   SampleCount_Sizer.Add( MinSamples_SpinBox );
   SampleCount_Sizer.AddSpacing( 12 );
   SampleCount_Sizer.Add( MaxSamples_Label );
   SampleCount_Sizer.Add( MaxSamples_SpinBox );
   SampleCount_Sizer.AddStretch();

   GridRows_Label.SetText( "Grid Rows:" );
   GridRows_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   
   GridRows_SpinBox.SetRange( int( TheBEGridRowsParameter->MinimumValue() ), int( TheBEGridRowsParameter->MaximumValue() ) );
   GridRows_SpinBox.SetToolTip( "<p>Number of rows in the sampling grid.</p>" );
   GridRows_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&BackgroundExtractionInterface::__GridRows_ValueUpdated, w );

   GridColumns_Label.SetText( "Columns:" );
   GridColumns_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   
   GridColumns_SpinBox.SetRange( int( TheBEGridColumnsParameter->MinimumValue() ), int( TheBEGridColumnsParameter->MaximumValue() ) );
   GridColumns_SpinBox.SetToolTip( "<p>Number of columns in the sampling grid.</p>" );
   GridColumns_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&BackgroundExtractionInterface::__GridColumns_ValueUpdated, w );

   GridSize_Sizer.SetSpacing( 4 );
   GridSize_Sizer.Add( GridRows_Label );
   GridSize_Sizer.Add( GridRows_SpinBox );
   GridSize_Sizer.AddSpacing( 12 );
   GridSize_Sizer.Add( GridColumns_Label );
   GridSize_Sizer.Add( GridColumns_SpinBox );
   GridSize_Sizer.AddStretch();

   Sampling_Sizer.SetSpacing( 4 );
   Sampling_Sizer.Add( SampleGen_Sizer );
   Sampling_Sizer.Add( Tolerance_NumericControl );
   Sampling_Sizer.Add( Deviation_NumericControl );
   Sampling_Sizer.Add( SampleCount_Sizer );
   Sampling_Sizer.Add( GridSize_Sizer );

   Sampling_Control.SetSizer( Sampling_Sizer );

   Sampling_SectionBar.SetTitle( "Sample Generation" );
   Sampling_SectionBar.SetSection( Sampling_Control );

   // Rejection Section
   RejectionEnabled_CheckBox.SetText( "Enable outlier rejection" );
   RejectionEnabled_CheckBox.SetToolTip( "<p>Enable iterative outlier rejection.</p>" );
   RejectionEnabled_CheckBox.OnClick( (Button::click_event_handler)&BackgroundExtractionInterface::__RejectionEnabled_Click, w );

   RejectionEnabled_Sizer.AddUnscaledSpacing( w.Font().Width( String( 'M' ) ) );
   RejectionEnabled_Sizer.Add( RejectionEnabled_CheckBox );
   RejectionEnabled_Sizer.AddStretch();

   RejectionLow_NumericControl.label.SetText( "Low Threshold:" );
   RejectionLow_NumericControl.label.SetFixedWidth( w.Font().Width( "Low Threshold:" + String( 'M', 2 ) ) );
   RejectionLow_NumericControl.slider.SetRange( 0, 100 );
   RejectionLow_NumericControl.SetReal();
   RejectionLow_NumericControl.SetRange( TheBERejectionLowParameter->MinimumValue(), TheBERejectionLowParameter->MaximumValue() );
   RejectionLow_NumericControl.SetPrecision( TheBERejectionLowParameter->Precision() );
   RejectionLow_NumericControl.edit.SetFixedWidth( w.Font().Width( "88888.88" ) );
   RejectionLow_NumericControl.SetToolTip( "<p>Low rejection threshold in sigma units.</p>" );
   RejectionLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BackgroundExtractionInterface::__RejectionLow_ValueUpdated, w );

   RejectionHigh_NumericControl.label.SetText( "High Threshold:" );
   RejectionHigh_NumericControl.label.SetFixedWidth( w.Font().Width( "High Threshold:" + String( 'M', 2 ) ) );
   RejectionHigh_NumericControl.slider.SetRange( 0, 100 );
   RejectionHigh_NumericControl.SetReal();
   RejectionHigh_NumericControl.SetRange( TheBERejectionHighParameter->MinimumValue(), TheBERejectionHighParameter->MaximumValue() );
   RejectionHigh_NumericControl.SetPrecision( TheBERejectionHighParameter->Precision() );
   RejectionHigh_NumericControl.edit.SetFixedWidth( w.Font().Width( "88888.88" ) );
   RejectionHigh_NumericControl.SetToolTip( "<p>High rejection threshold in sigma units.</p>" );
   RejectionHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&BackgroundExtractionInterface::__RejectionHigh_ValueUpdated, w );

   RejectionIter_Label.SetText( "Iterations:" );
   RejectionIter_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   
   RejectionIter_SpinBox.SetRange( int( TheBERejectionIterationsParameter->MinimumValue() ), int( TheBERejectionIterationsParameter->MaximumValue() ) );
   RejectionIter_SpinBox.SetToolTip( "<p>Number of outlier rejection iterations.</p>" );
   RejectionIter_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&BackgroundExtractionInterface::__RejectionIter_ValueUpdated, w );

   RejectionIter_Sizer.SetSpacing( 4 );
   RejectionIter_Sizer.Add( RejectionIter_Label );
   RejectionIter_Sizer.Add( RejectionIter_SpinBox );
   RejectionIter_Sizer.AddStretch();

   Rejection_Sizer.SetSpacing( 4 );
   Rejection_Sizer.Add( RejectionEnabled_Sizer );
   Rejection_Sizer.Add( RejectionLow_NumericControl );
   Rejection_Sizer.Add( RejectionHigh_NumericControl );
   Rejection_Sizer.Add( RejectionIter_Sizer );

   Rejection_Control.SetSizer( Rejection_Sizer );

   Rejection_SectionBar.SetTitle( "Outlier Rejection" );
   Rejection_SectionBar.SetSection( Rejection_Control );

   // Output Section
   ReplaceTarget_CheckBox.SetText( "Replace target image" );
   ReplaceTarget_CheckBox.SetToolTip( "<p>Subtract background from target image.</p>" );
   ReplaceTarget_CheckBox.OnClick( (Button::click_event_handler)&BackgroundExtractionInterface::__ReplaceTarget_Click, w );

   ReplaceTarget_Sizer.AddUnscaledSpacing( w.Font().Width( String( 'M' ) ) );
   ReplaceTarget_Sizer.Add( ReplaceTarget_CheckBox );
   ReplaceTarget_Sizer.AddStretch();

   OutputBackground_CheckBox.SetText( "Output background model" );
   OutputBackground_CheckBox.SetToolTip( "<p>Create new image with background model.</p>" );
   OutputBackground_CheckBox.OnClick( (Button::click_event_handler)&BackgroundExtractionInterface::__OutputBackground_Click, w );

   OutputBackground_Sizer.AddUnscaledSpacing( w.Font().Width( String( 'M' ) ) );
   OutputBackground_Sizer.Add( OutputBackground_CheckBox );
   OutputBackground_Sizer.AddStretch();

   Output_Sizer.SetSpacing( 4 );
   Output_Sizer.Add( ReplaceTarget_Sizer );
   Output_Sizer.Add( OutputBackground_Sizer );

   Output_Control.SetSizer( Output_Sizer );

   Output_SectionBar.SetTitle( "Output" );
   Output_SectionBar.SetSection( Output_Control );

   // Global Sizer
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Model_SectionBar );
   Global_Sizer.Add( Model_Control );
   Global_Sizer.Add( Sampling_SectionBar );
   Global_Sizer.Add( Sampling_Control );
   Global_Sizer.Add( Rejection_SectionBar );
   Global_Sizer.Add( Rejection_Control );
   Global_Sizer.Add( Output_SectionBar );
   Global_Sizer.Add( Output_Control );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::UpdateControls()
{
   UpdateModelControls();
   UpdateSamplingControls();
   UpdateRejectionControls();
   UpdateOutputControls();
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::UpdateModelControls()
{
   GUI->ModelType_ComboBox.SetCurrentItem( m_instance.p_model );
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::UpdateSamplingControls()
{
   GUI->SampleGen_ComboBox.SetCurrentItem( m_instance.p_sampleGeneration );
   GUI->Tolerance_NumericControl.SetValue( m_instance.p_tolerance );
   GUI->Deviation_NumericControl.SetValue( m_instance.p_deviation );
   GUI->MinSamples_SpinBox.SetValue( m_instance.p_minSamples );
   GUI->MaxSamples_SpinBox.SetValue( m_instance.p_maxSamples );
   GUI->GridRows_SpinBox.SetValue( m_instance.p_gridRows );
   GUI->GridColumns_SpinBox.SetValue( m_instance.p_gridColumns );

   bool isGrid = m_instance.p_sampleGeneration == BESampleGeneration::Grid;
   GUI->GridRows_Label.Enable( isGrid );
   GUI->GridRows_SpinBox.Enable( isGrid );
   GUI->GridColumns_Label.Enable( isGrid );
   GUI->GridColumns_SpinBox.Enable( isGrid );
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::UpdateRejectionControls()
{
   GUI->RejectionEnabled_CheckBox.SetChecked( m_instance.p_rejectionEnabled );
   GUI->RejectionLow_NumericControl.SetValue( m_instance.p_rejectionLow );
   GUI->RejectionHigh_NumericControl.SetValue( m_instance.p_rejectionHigh );
   GUI->RejectionIter_SpinBox.SetValue( m_instance.p_rejectionIterations );

   bool enabled = m_instance.p_rejectionEnabled;
   GUI->RejectionLow_NumericControl.Enable( enabled );
   GUI->RejectionHigh_NumericControl.Enable( enabled );
   GUI->RejectionIter_Label.Enable( enabled );
   GUI->RejectionIter_SpinBox.Enable( enabled );
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::UpdateOutputControls()
{
   GUI->ReplaceTarget_CheckBox.SetChecked( m_instance.p_replaceTarget );
   GUI->OutputBackground_CheckBox.SetChecked( m_instance.p_outputBackgroundModel );
}

// ----------------------------------------------------------------------------
// Event Handlers
// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__Model_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->ModelType_ComboBox )
      m_instance.p_model = itemIndex;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__SampleGen_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->SampleGen_ComboBox )
   {
      m_instance.p_sampleGeneration = itemIndex;
      UpdateSamplingControls();
   }
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__Tolerance_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Tolerance_NumericControl )
      m_instance.p_tolerance = value;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__Deviation_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Deviation_NumericControl )
      m_instance.p_deviation = value;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__MinSamples_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->MinSamples_SpinBox )
      m_instance.p_minSamples = value;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__MaxSamples_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->MaxSamples_SpinBox )
      m_instance.p_maxSamples = value;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__GridRows_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->GridRows_SpinBox )
      m_instance.p_gridRows = value;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__GridColumns_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->GridColumns_SpinBox )
      m_instance.p_gridColumns = value;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__RejectionEnabled_Click( Button& sender, bool checked )
{
   if ( sender == GUI->RejectionEnabled_CheckBox )
   {
      m_instance.p_rejectionEnabled = checked;
      UpdateRejectionControls();
   }
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__RejectionLow_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RejectionLow_NumericControl )
      m_instance.p_rejectionLow = value;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__RejectionHigh_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RejectionHigh_NumericControl )
      m_instance.p_rejectionHigh = value;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__RejectionIter_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->RejectionIter_SpinBox )
      m_instance.p_rejectionIterations = value;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__ReplaceTarget_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ReplaceTarget_CheckBox )
      m_instance.p_replaceTarget = checked;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInterface::__OutputBackground_Click( Button& sender, bool checked )
{
   if ( sender == GUI->OutputBackground_CheckBox )
      m_instance.p_outputBackgroundModel = checked;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
