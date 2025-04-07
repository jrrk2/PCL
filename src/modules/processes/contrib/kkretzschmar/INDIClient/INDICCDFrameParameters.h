//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDICCDFrameParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __INDICCDFrameParameters_h
#define __INDICCDFrameParameters_h

#include <pcl/MetaParameter.h>

#include "ModuleParameterRegistration.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ICFDeviceName : public MetaString
{
public:

   ICFDeviceName( MetaProcess* );

   IsoString Id() const override;
};

extern ICFDeviceName* TheICFDeviceNameParameter;

// ----------------------------------------------------------------------------

class ICFUploadMode : public MetaEnumeration
{
public:

   enum
   {
      UploadClient,
      UploadServer,
      UploadServerAndClient,
      NumberOfUploadModes,
      Default = UploadClient
   };

   ICFUploadMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICFUploadMode* TheICFUploadModeParameter;

// ----------------------------------------------------------------------------

class ICFServerUploadDirectory : public MetaString
{
public:

   ICFServerUploadDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern ICFServerUploadDirectory* TheICFServerUploadDirectoryParameter;

// ----------------------------------------------------------------------------

class ICFServerFileNameTemplate : public MetaString
{
public:

   ICFServerFileNameTemplate( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFServerFileNameTemplate* TheICFServerFileNameTemplateParameter;

// ----------------------------------------------------------------------------

class ICFFrameType : public MetaEnumeration
{
public:

   enum
   {
      LightFrame,
      BiasFrame,
      DarkFrame,
      FlatFrame,
      NumberOfFrameTypes,
      Default = LightFrame
   };

   ICFFrameType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICFFrameType* TheICFFrameTypeParameter;

// ----------------------------------------------------------------------------

class ICFBinningX : public MetaInt32
{
public:

   ICFBinningX( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFBinningX* TheICFBinningXParameter;

// ----------------------------------------------------------------------------

class ICFBinningY : public MetaInt32
{
public:

   ICFBinningY( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFBinningY* TheICFBinningYParameter;

// ----------------------------------------------------------------------------

class ICFFilterSlot : public MetaInt32
{
public:

   ICFFilterSlot( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFFilterSlot* TheICFFilterSlotParameter;

// ----------------------------------------------------------------------------

class ICFExposureTime : public MetaDouble
{
public:

   ICFExposureTime( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFExposureTime* TheICFExposureTimeParameter;

// ----------------------------------------------------------------------------

class ICFExposureDelay : public MetaDouble
{
public:

   ICFExposureDelay( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFExposureDelay* TheICFExposureDelayParameter;

// ----------------------------------------------------------------------------

class ICFExposureCount : public MetaInt32
{
public:

   ICFExposureCount( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ICFExposureCount* TheICFExposureCountParameter;

// ----------------------------------------------------------------------------

class ICFOpenClientImages : public MetaBoolean
{
public:

   ICFOpenClientImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFOpenClientImages* TheICFOpenClientImagesParameter;

// ----------------------------------------------------------------------------

class ICFNewImageIdTemplate : public MetaString
{
public:

   ICFNewImageIdTemplate( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFNewImageIdTemplate* TheICFNewImageIdTemplateParameter;

// ----------------------------------------------------------------------------

class ICFReuseImageWindow : public MetaBoolean
{
public:

   ICFReuseImageWindow( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFReuseImageWindow* TheICFReuseImageWindowParameter;

// ----------------------------------------------------------------------------

class ICFAutoStretch : public MetaBoolean
{
public:

   ICFAutoStretch( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFAutoStretch* TheICFAutoStretchParameter;

// ----------------------------------------------------------------------------

class ICFLinkedAutoStretch : public MetaBoolean
{
public:

   ICFLinkedAutoStretch( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFLinkedAutoStretch* TheICFLinkedAutoStretchParameter;

// ----------------------------------------------------------------------------

class ICFSaveClientImages : public MetaBoolean
{
public:

   ICFSaveClientImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFSaveClientImages* TheICFSaveClientImagesParameter;

// ----------------------------------------------------------------------------

class ICFOverwriteClientImages : public MetaBoolean
{
public:

   ICFOverwriteClientImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFOverwriteClientImages* TheICFOverwriteClientImagesParameter;

// ----------------------------------------------------------------------------

class ICFClientDownloadDirectory : public MetaString
{
public:

   ICFClientDownloadDirectory( MetaProcess* );

   IsoString Id() const override;
};

extern ICFClientDownloadDirectory* TheICFClientDownloadDirectoryParameter;

// ----------------------------------------------------------------------------

class ICFClientFileNameTemplate : public MetaString
{
public:

   ICFClientFileNameTemplate( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFClientFileNameTemplate* TheICFClientFileNameTemplateParameter;

// ----------------------------------------------------------------------------

class ICFClientOutputFormatHints : public MetaString
{
public:

   ICFClientOutputFormatHints( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFClientOutputFormatHints* TheICFClientOutputFormatHintsParameter;

// ----------------------------------------------------------------------------

class ICFObjectName : public MetaString
{
public:

   ICFObjectName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFObjectName* TheICFObjectNameParameter;

// ----------------------------------------------------------------------------

class ICFTelescopeSelection : public MetaEnumeration
{
public:

   enum
   {
      NoTelescope,
      ActiveTelescope,
      MountControllerTelescope,
      MountControllerOrActiveTelescope,
      TelescopeDeviceName,
      NumberOfTelescopeSelections,
      Default = MountControllerOrActiveTelescope
   };

   ICFTelescopeSelection( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICFTelescopeSelection* TheICFTelescopeSelectionParameter;

// ----------------------------------------------------------------------------

class ICFRequireSelectedTelescope : public MetaBoolean
{
public:

   ICFRequireSelectedTelescope( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICFRequireSelectedTelescope* TheICFRequireSelectedTelescopeParameter;

// ----------------------------------------------------------------------------

class ICFTelescopeDeviceName : public MetaString
{
public:

   ICFTelescopeDeviceName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ICFTelescopeDeviceName* TheICFTelescopeDeviceNameParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class ICFClientFrames : public MetaTable
{
public:

   ICFClientFrames( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFClientFrames* TheICFClientFramesParameter;

// ----------------------------------------------------------------------------

class ICFClientViewId : public MetaString
{
public:

   ICFClientViewId( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFClientViewId* TheICFClientViewIdParameter;

// ----------------------------------------------------------------------------

class ICFClientFilePath : public MetaString
{
public:

   ICFClientFilePath( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFClientFilePath* TheICFClientFilePathParameter;

// ----------------------------------------------------------------------------

class ICFServerFrames : public MetaTable
{
public:

   ICFServerFrames( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFServerFrames* TheICFServerFramesParameter;

// ----------------------------------------------------------------------------

class ICFServerFrame : public MetaString
{
public:

   ICFServerFrame( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ICFServerFrame* TheICFServerFrameParameter;

// ----------------------------------------------------------------------------

DECLARE_STRING_PARAMETER_WITH_DEFAULT_CLASS( ICFExternalFilterWheelDeviceName );

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_PARAMETER_CLASS( ICFEnableAlignmentCorrection );
DECLARE_STRING_PARAMETER_CLASS( ICFAlignmentFile );

// ----------------------------------------------------------------------------

DECLARE_INT32_PARAMETER_CLASS( ICFTelescopeFocalLength );

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_PARAMETER_CLASS( ICFApplyPlateSolver );
DECLARE_BOOLEAN_PARAMETER_CLASS( ICFCenterTarget );
DECLARE_STRING_PARAMETER_WITH_DEFAULT_CLASS( ICFServerURL );
DECLARE_STRING_PARAMETER_WITH_DEFAULT_CLASS( ICFSolverCatalogName );
DECLARE_BOOLEAN_PARAMETER_CLASS( ICFSolverAutoCatalog );
DECLARE_INT32_PARAMETER_CLASS( ICFSolverLimitMagnitude );
DECLARE_BOOLEAN_PARAMETER_CLASS( ICFSolverAutoLimitMagnitude );
DECLARE_FLOAT_PARAMETER_CLASS(ICFSolverStarSensitivity);
DECLARE_INT32_PARAMETER_CLASS(ICFSolverNoiseLayers);
DECLARE_ENUM_PARAMETER(ICFSolverAlignmentDevice, ENUM_ITEM(TriangleSimilarity) ENUM_ITEM(PolygonMatching), TriangleSimilarity);
DECLARE_BOOLEAN_PARAMETER_CLASS( ICFSolverDistortionCorrection );
DECLARE_FLOAT_PARAMETER_CLASS(ICFSolverSplineSmoothing);
DECLARE_ENUM_PARAMETER(ICFSolverProjection, ENUM_ITEM(Gnomonic) ENUM_ITEM(Stereographic) ENUM_ITEM(PlateCarree) ENUM_ITEM(Mercator) ENUM_ITEM(HammerAitoff) ENUM_ITEM(ZenithalEqualArea) ENUM_ITEM(Orthographic), Gnomonic);
DECLARE_STRING_PARAMETER_CLASS( ICFCCDMode );


PCL_END_LOCAL

} // namespace pcl

#endif // __INDICCDFrameParameters_h

// ----------------------------------------------------------------------------
// EOF INDICCDFrameParameters.h - Released 2025-04-07T08:53:56Z
