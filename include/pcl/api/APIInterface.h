//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/APIInterface.h - Released 2025-04-07T08:52:44Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __PCL_API_APIInterface_h
#define __PCL_API_APIInterface_h

#include <pcl/api/APIDefs.h>

// Global namespace

#define PCL_API_Version 0x0186

// ----------------------------------------------------------------------------

struct api_context GlobalContext
{
   /*
    * Version information
    */
   void        (GetPixInsightVersion)( uint32* major, uint32* minor, uint32* release, uint32* revision,
                                                 uint32* betaRelease, uint32* confidentialRelease,
                                                 uint32* leVersion, char* langCode );
   // ### Returns a string allocated by the caller module.
   char16_type*(GetPixInsightCodename)( api_handle );

   /*
    * Error information
    */
   uint32      (LastError)();
   void        (ClearError)();
   api_bool    (ErrorMessage)( uint32, char16_type*, size_type* );

   /*
    * Memory allocation
    */
   void*       (Allocate)( size_type );
   api_bool    (Deallocate)( void* );

   /*
    * Thread status functions
    */
   uint32      (GetProcessStatus)();
   api_bool    (ResetProcessStatus)();
   api_bool    (EnableAbort)();
   api_bool    (DisableAbort)();
   api_bool    (Abort)();
   void        (ProcessEvents)( api_bool excludeUserInputEvents );

   /*
    * Console functions
    */
   api_handle  (GetThreadWindowId)(); // *** ### disabled function

   console_handle (GetConsole)();
   api_bool    (ValidateConsole)( const_console_handle );
   api_bool    (WriteConsole)( console_handle, const char16_type*, api_bool appendNewline );
   int32       (ReadConsoleChar)( console_handle ); // TODO - still not implemented

   // ### The following two functions return strings allocated by the caller module.
   char16_type*(ReadConsoleString)( api_handle, console_handle ); // TODO - still not implemented
   char16_type*(GetConsoleText)( api_handle, const_console_handle ); // ### security issues

   api_bool    (FlushConsole)( console_handle );
   api_bool    (ShowConsole)( console_handle, api_bool );
   api_bool    (ExecuteCommand)( api_handle, console_handle, const char16_type* cmd );

   /*
    * Global cursor position
    */
   void        (GetCursorPosition)( int32* x, int32* y );
   void        (SetCursorPosition)( int32 x, int32 y );

   /*
    * Returns an OR combination of pcl::ModifierKey values corresponding to the
    * current state of supported modifier keys.
    */
   uint32      (GetKeyboardModifiers)();

   /*
    * Tool tip window
    */
   void        (ShowToolTipWindow)( int32 x, int32 y, const char16_type*,
                                              const_control_handle, int32, int32, int32, int32 );
   void        (HideToolTipWindow)();
   api_bool    (GetToolTipWindowText)( char16_type*, size_type* );

   /*
    * MessageBox functions
    */
   uint32      (MessageBox)( const char16_type* text, const char16_type* caption, uint32 button0, uint32 button1, uint32 button2, uint32 defButton, uint32 escButton, uint32 icon );

   /*
    * Instance and interface launch functions
    */
   void        (LaunchProcessInstance)( meta_process_handle, const_process_handle, int32 mode, uint32 flags );
   void        (LaunchProcessInstanceOnView)( meta_process_handle, const_process_handle, view_handle, uint32 flags );
   api_bool    (LaunchProcessInterface)( meta_interface_handle, uint32 flags );

   /*
    * Readout options
    */
   void        (GetReadoutOptions)( api_readout_options* options );
   void        (SetReadoutOptions)( const api_readout_options* options );

   /*
    * Real-time preview
    * ### Obsolete -- preserved for compatibility -- see RealTimePreviewContext
    */
   api_bool    (SetRealTimePreviewOwner)( interface_handle, uint32 flags );
   api_bool    (IsRealTimePreviewUpdating)();
   void        (UpdateRealTimePreview)();

   /*
    * Integrated documentation system
    */
   api_bool    (BrowseProcessDocumentation)( meta_process_handle, uint32 flags );

   /*
    * Global settings
    */
   api_bool    (GetGlobalFlag)( const char*, api_bool* );
   api_bool    (GetGlobalInteger)( const char*, void*, api_bool isSigned );
   api_bool    (GetGlobalReal)( const char*, double* );
   api_bool    (GetGlobalColor)( const char*, uint32* );
   api_bool    (GetGlobalFont)( const char*, char16_type*, size_type*, int32* sizePt );
   api_bool    (GetGlobalString)( const char*, char16_type*, size_type* );

   api_bool    (EnterGlobalSettingsUpdateContext)();
   api_bool    (IsGlobalSettingsUpdateContextActive)();

   api_bool    (SetGlobalFlag)( const char*, api_bool );
   api_bool    (SetGlobalInteger)( const char*, uint32, api_bool isSigned );
   api_bool    (SetGlobalReal)( const char*, double );
   api_bool    (SetGlobalColor)( const char*, uint32 );
   api_bool    (SetGlobalFont)( const char*, const char16_type*, int32 sizePt );
   api_bool    (SetGlobalString)( const char*, const char16_type* );

   api_bool    (CancelGlobalSettingsUpdate)( api_handle, uint32 reserved );
   api_bool    (ExitGlobalSettingsUpdateContext)();

   /*
    * Module-defined settings
    */
   // ### The following two functions return data allocated by the caller module.
   api_bool    (ReadSettingsBlock)( api_handle, void**, size_type*, const char* key, api_bool global );
   api_bool    (ReadSettingsString)( api_handle, char16_type**, const char* key, api_bool global );

   api_bool    (ReadSettingsFlag)( api_handle, api_bool*, const char* key, api_bool global );
   api_bool    (ReadSettingsInteger)( api_handle, int32*, const char* key, api_bool global );
   api_bool    (ReadSettingsUnsignedInteger)( api_handle, uint32*, const char* key, api_bool global );
   api_bool    (ReadSettingsReal)( api_handle, double*, const char* key, api_bool global );

   api_bool    (WriteSettingsBlock)( api_handle, const void*, size_type, const char* key, api_bool global );
   api_bool    (WriteSettingsString)( api_handle, const char16_type*, const char* key, api_bool global );
   api_bool    (WriteSettingsFlag)( api_handle, api_bool, const char* key, api_bool global );
   api_bool    (WriteSettingsInteger)( api_handle, int32, const char* key, api_bool global );
   api_bool    (WriteSettingsUnsignedInteger)( api_handle, uint32, const char* key, api_bool global );
   api_bool    (WriteSettingsReal)( api_handle, double, const char* key, api_bool global );

   api_bool    (DeleteSettingsItem)( api_handle, const char* key, api_bool global );

   uint32      (GetSettingsItemGlobalAccess)( api_handle, const char* key );                // bit#0=W bit#1=R
   api_bool    (SetSettingsItemGlobalAccess)( api_handle, const char* key, uint32 flags );  // bit#0=W bit#1=R

   /*
    * Miscellaneous message broadcasting
    */
   void        (BroadcastImageUpdated)( const_view_handle, const void* /*reserved*/ );
   void        (BroadcastGlobalFiltersUpdated)( const void* /*reserved*/ );

   /*
    * Miscellaneous color management
    */
   api_bool    (GetProfilesDirectory)( int32, char16_type*, size_type* );

   /*
    * Access to the global PixelTraits LUT
    */
   const ::api_pixtraits_lut* (GetPixelTraitsLUT)( uint32 version ); // version must be zero

   /*
    * Fast module thread control (since core version 1.8.8-7)
    */
   int32       (MaxProcessorsAllowedForModule)( api_handle, uint32 flags/*unused*/ );

   /*
    * Instance slot of the running application in [1,256] (since core version 1.8.9-2 build 1581)
    */
   int32       (ApplicationInstanceSlot)( api_handle );

   /*
    * Application configuration directory (since core version 1.8.9-2 build 1581)
    */
   api_bool    (GetApplicationConfigurationDirectory)( char16_type*, size_type* );
};

// ----------------------------------------------------------------------------

struct api_context ModuleDefinitionContext
{
   void        (EnterModuleDefinitionContext)();
   api_bool    (IsModuleDefinitionContextActive)();

   void        (SetModuleOnLoadRoutine)( pcl::module_on_load_routine );
   void        (SetModuleOnUnloadRoutine)( pcl::module_on_unload_routine );
   void        (SetModuleAllocationRoutine)( pcl::module_allocation_routine );
   void        (SetModuleDeallocationRoutine)( pcl::module_deallocation_routine );

   void        (ExitModuleDefinitionContext)();
};

// ----------------------------------------------------------------------------

struct api_context ProcessDefinitionContext
{
   void        (EnterProcessDefinitionContext)();
   api_bool    (IsProcessDefinitionContextActive)();

   void        (BeginProcessDefinition)( meta_process_handle, const char* procId );
   api_bool    (GetProcessBeingDefined)( char*, size_type* );
   void        (SetProcessCategory)( const char* );
   void        (SetProcessVersion)( uint32 );
   void        (SetProcessAliasIdentifiers)( const char* );
   void        (SetProcessDescription)( const char16_type* );
   void        (SetProcessScriptComment)( const char16_type* );
   void        (SetProcessIconSVG)( const char* );
   void        (SetProcessIconSVGFile)( const char16_type* );
   void        (SetProcessIconImage)( const char** ); // ### deprecated
   void        (SetProcessIconImageFile)( const char16_type* ); // ### deprecated
   void        (SetProcessIconSmallImage)( const char** ); // ### deprecated
   void        (SetProcessIconSmallImageFile)( const char16_type* ); // ### deprecated

   void        (SetProcessClassInitializationRoutine)( pcl::process_class_initialization_routine );
   void        (SetProcessCreationRoutine)( pcl::process_creation_routine );
   void        (SetProcessDestructionRoutine)( pcl::process_destruction_routine );
   void        (SetProcessClonationRoutine)( pcl::process_clonation_routine);
   void        (SetProcessTestClonationRoutine)( pcl::process_test_clonation_routine );
   void        (SetProcessSetServerHandleRoutine)( pcl::process_set_handle_routine );
   void        (SetProcessAssignmentRoutine)( pcl::process_assignment_routine );
   void        (SetProcessInitializationRoutine)( pcl::process_initialization_routine );
   void        (SetProcessValidationRoutine)( pcl::process_validation_routine );

   void        (SetProcessCommandLineProcessingRoutine)( pcl::process_command_line_processing_routine, uint32 flags );
   void        (SetProcessEditPreferencesRoutine)( pcl::process_edit_preferences_routine );
   void        (SetProcessBrowseDocumentationRoutine)( pcl::process_browse_documentation_routine );
   void        (SetProcessExecutionPreferencesRoutine)( pcl::process_execution_preferences_routine );

   void        (SetProcessExecutionValidationRoutine)( pcl::process_execution_validation_routine );
   void        (SetProcessMaskValidationRoutine)( pcl::process_mask_validation_routine );
   void        (SetProcessHistoryUpdateValidationRoutine)( pcl::process_history_update_validation_routine );
   void        (SetProcessUndoModeRoutine)( pcl::process_undo_mode_routine );
   void        (SetProcessPreExecutionRoutine)( pcl::process_pre_execution_routine );
   void        (SetProcessExecutionRoutine)( pcl::process_execution_routine );
   void        (SetProcessPostExecutionRoutine)( pcl::process_post_execution_routine );

   void        (SetProcessGlobalExecutionValidationRoutine)( pcl::process_global_execution_validation_routine );
   void        (SetProcessPreGlobalExecutionRoutine)( pcl::process_pre_global_execution_routine );
   void        (SetProcessGlobalExecutionRoutine)( pcl::process_global_execution_routine );
   void        (SetProcessPostGlobalExecutionRoutine)( pcl::process_post_global_execution_routine );

   void        (SetProcessImageExecutionValidationRoutine)( pcl::process_image_execution_validation_routine );
   void        (SetProcessImageExecutionRoutine)( pcl::process_image_execution_routine );

   void        (SetProcessDefaultInterfaceSelectionRoutine)( pcl::process_default_interface_selection_routine );
   void        (SetProcessInterfaceSelectionRoutine)( pcl::process_interface_selection_routine );
   void        (SetProcessInterfaceValidationRoutine)( pcl::process_interface_validation_routine );

   void        (SetProcessPreReadingRoutine)( pcl::process_pre_reading_routine );
   void        (SetProcessPostReadingRoutine)( pcl::process_post_reading_routine );
   void        (SetProcessPreWritingRoutine)( pcl::process_pre_writing_routine );
   void        (SetProcessPostWritingRoutine)( pcl::process_post_writing_routine );

   void        (SetProcessIPCStartRoutine)( pcl::process_ipc_notification_routine );
   void        (SetProcessIPCStopRoutine)( pcl::process_ipc_notification_routine );
   void        (SetProcessIPCSetParametersRoutine)( pcl::process_ipc_notification_routine );
   void        (SetProcessIPCGetStatusRoutine)( pcl::process_ipc_status_routine );

   void        (BeginParameterDefinition)( meta_parameter_handle, const char* parId, uint32 parType );
   api_bool    (GetParameterBeingDefined)( char*, size_type* );
   void        (SetParameterProcessVersionRange)( uint32, uint32 );
   void        (SetParameterRequired)( api_bool );
   void        (SetParameterReadOnly)( api_bool );
   void        (SetParameterAliasIdentifiers)( const char* );
   void        (SetParameterDescription)( const char16_type* );
   void        (SetParameterScriptComment)( const char16_type* );
   void        (SetParameterLockRoutine)( pcl::parameter_lock_routine );
   void        (SetParameterUnlockRoutine)( pcl::parameter_unlock_routine );
   void        (SetParameterValidationRoutine)( pcl::parameter_validation_routine );
   void        (SetParameterAllocationRoutine)( pcl::parameter_allocation_routine );
   void        (SetParameterLengthQueryRoutine)( pcl::parameter_length_query_routine );
   void        (SetDefaultNumericValue)( double );
   void        (SetValidNumericRange)( double, double );
   void        (SetPrecision)( int32 );
   void        (SetScientificNotation)( api_bool );
   void        (SetDefaultBooleanValue)( api_bool );
   void        (DefineEnumerationElement)( const char*, api_enum );
   void        (DefineEnumerationAlias)( const char*, const char* );
   void        (SetDefaultEnumerationValueIndex)( uint32 );
   void        (SetDefaultStringValue)( const char16_type* );
   void        (SetStringAllowedCharacters)( const char16_type* );
   void        (SetStringLengthLimits)( size_type, size_type );
   void        (BeginTableColumnDefinition)( meta_parameter_handle, const char* colId, uint32 colType );
   void        (EndTableColumnDefinition)();
   void        (SetTableRowLimits)( size_type, size_type );
   void        (SetBlockSizeLimits)( size_type, size_type );

   void        (EndParameterDefinition)();
   void        (EndProcessDefinition)();
   void        (ExitProcessDefinitionContext)();
};

// ----------------------------------------------------------------------------

struct api_context InterfaceDefinitionContext
{
   void           (EnterInterfaceDefinitionContext)();
   api_bool       (IsInterfaceDefinitionContextActive)();

   void           (BeginInterfaceDefinition)( meta_interface_handle, const char* ifaceId, uint32 flags );
   api_bool       (GetInterfaceBeingDefined)( char*, size_type* );
   void           (SetInterfaceVersion)( uint32 );
   void           (SetInterfaceAliasIdentifiers)( const char* );
   void           (SetInterfaceDescription)( const char16_type* );
   void           (SetInterfaceIconSVG)( const char* );
   void           (SetInterfaceIconSVGFile)( const char16_type* );
   void           (SetInterfaceIconImage)( const char** ); // ### deprecated
   void           (SetInterfaceIconImageFile)( const char16_type* ); // ### deprecated
   void           (SetInterfaceIconSmallImage)( const char** ); // ### deprecated
   void           (SetInterfaceIconSmallImageFile)( const char16_type* ); // ### deprecated

   void           (SetInterfaceFeatures)( uint32, uint32 );

   void           (SetInterfaceInitializationRoutine)( pcl::interface_initialization_routine );
   void           (SetInterfaceLaunchRoutine)( pcl::interface_launch_routine );
   void           (SetInterfaceProcessInstantiationRoutine)( pcl::interface_process_instantiation_routine );
   void           (SetInterfaceProcessTestInstantiationRoutine)( pcl::interface_process_instantiation_routine );
   void           (SetInterfaceProcessValidationRoutine)( pcl::interface_process_validation_routine );
   void           (SetInterfaceProcessImportRoutine)( pcl::interface_process_import_routine );

   void           (SetInterfaceApplyRoutine)( pcl::interface_control_routine );
   void           (SetInterfaceApplyGlobalRoutine)( pcl::interface_control_routine );
   void           (SetInterfaceRealTimePreviewUpdatedRoutine)( pcl::interface_control_state_routine );
   void           (SetInterfaceExecuteRoutine)( pcl::interface_control_routine );
   void           (SetInterfaceCancelRoutine)( pcl::interface_control_routine );
   void           (SetInterfaceBrowseDocumentationRoutine)( pcl::interface_control_routine );
   void           (SetInterfaceTrackViewUpdatedRoutine)( pcl::interface_control_state_routine );
   void           (SetInterfaceEditPreferencesRoutine)( pcl::interface_control_routine );
   void           (SetInterfaceResetRoutine)( pcl::interface_control_routine );

   void           (SetInterfaceRealTimeUpdateQueryRoutine)( pcl::interface_real_time_update_query_routine );
   void           (SetInterfaceRealTimeGenerationFlagsRoutine)( pcl::interface_real_time_generation_flags_routine );
   void           (SetInterfaceRealTimeGenerationRoutine)( pcl::interface_real_time_generation_routine );
   void           (SetInterfaceRealTimeCancelRoutine)( pcl::interface_real_time_cancel_routine );

   void           (SetInterfaceDynamicModeEnterRoutine)( pcl::interface_dynamic_mode_enter_routine );
   void           (SetInterfaceDynamicModeExitRoutine)( pcl::interface_dynamic_mode_exit_routine );
   void           (SetInterfaceDynamicMouseEnterRoutine)( pcl::interface_dynamic_view_event_routine );
   void           (SetInterfaceDynamicMouseLeaveRoutine)( pcl::interface_dynamic_view_event_routine );
   void           (SetInterfaceDynamicMouseMoveRoutine)( pcl::interface_dynamic_mouse_routine );
   void           (SetInterfaceDynamicMousePressRoutine)( pcl::interface_dynamic_mouse_button_routine );
   void           (SetInterfaceDynamicMouseReleaseRoutine)( pcl::interface_dynamic_mouse_button_routine );
   void           (SetInterfaceDynamicMouseDoubleClickRoutine)( pcl::interface_dynamic_mouse_routine );
   void           (SetInterfaceDynamicKeyPressRoutine)( pcl::interface_dynamic_keyboard_routine );
   void           (SetInterfaceDynamicKeyReleaseRoutine)( pcl::interface_dynamic_keyboard_routine );
   void           (SetInterfaceDynamicMouseWheelRoutine)( pcl::interface_dynamic_wheel_routine );
   void           (SetInterfaceDynamicUpdateQueryRoutine)( pcl::interface_dynamic_update_query_routine );
   void           (SetInterfaceDynamicPaintRoutine)( pcl::interface_dynamic_paint_routine );

   void           (SetImageCreatedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageRenamedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageDeletedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageFocusedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageLockedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageUnlockedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageSTFEnabledNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageSTFDisabledNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageSTFUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageRGBWSUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageCMEnabledNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageCMDisabledNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageCMUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetImageSavedNotificationRoutine)( pcl::image_notification_routine );

   void           (SetMaskUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (SetMaskEnabledNotificationRoutine)( pcl::image_notification_routine );
   void           (SetMaskDisabledNotificationRoutine)( pcl::image_notification_routine );
   void           (SetMaskShownNotificationRoutine)( pcl::image_notification_routine );
   void           (SetMaskHiddenNotificationRoutine)( pcl::image_notification_routine );

   void           (SetTransparencyHiddenNotificationRoutine)( pcl::image_notification_routine );
   void           (SetTransparencyModeUpdatedNotificationRoutine)( pcl::image_notification_routine );

   void           (SetViewPropertyUpdatedNotificationRoutine)( pcl::view_property_notification_routine );
   void           (SetViewPropertyDeletedNotificationRoutine)( pcl::view_property_notification_routine );

   void           (SetBeginReadoutNotificationRoutine)( pcl::image_notification_routine );
   void           (SetUpdateReadoutNotificationRoutine)( pcl::readout_notification_routine );
   void           (SetEndReadoutNotificationRoutine)( pcl::image_notification_routine );

   void           (SetProcessCreatedNotificationRoutine)( pcl::process_notification_routine );
   void           (SetProcessUpdatedNotificationRoutine)( pcl::process_notification_routine );
   void           (SetProcessDeletedNotificationRoutine)( pcl::process_notification_routine );
   void           (SetProcessSavedNotificationRoutine)( pcl::process_notification_routine );

   void           (SetRealTimePreviewOwnerChangeNotificationRoutine)( pcl::interface_notification_routine );
   void           (SetRealTimePreviewLUTUpdatedNotificationRoutine)( pcl::lut_notification_routine );
   void           (SetRealTimePreviewGenerationStartNotificationRoutine)( pcl::global_notification_routine );
   void           (SetRealTimePreviewGenerationFinishNotificationRoutine)( pcl::global_notification_routine );

   void           (SetGlobalRGBWSUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (SetGlobalCMEnabledNotificationRoutine)( pcl::global_notification_routine );
   void           (SetGlobalCMDisabledNotificationRoutine)( pcl::global_notification_routine );
   void           (SetGlobalCMUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (SetReadoutOptionsUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (SetGlobalPreferencesUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (SetGlobalFiltersUpdatedNotificationRoutine)( pcl::global_notification_routine );

   void           (EndInterfaceDefinition)();
   void           (ExitInterfaceDefinitionContext)();
};

// ----------------------------------------------------------------------------

struct api_context FileFormatDefinitionContext
{
   void           (EnterFileFormatDefinitionContext)();
   api_bool       (IsFileFormatDefinitionContextActive)();

   void           (BeginFileFormatDefinition)( meta_format_handle, const char* fmtName, const char16_type** fmtExtensions, const char** fmtMimeTypes );
   api_bool       (GetFileFormatBeingDefined)( char*, size_type* );
   void           (SetFileFormatVersion)( uint32 );
   void           (SetFileFormatDescription)( const char16_type* );
   void           (SetFileFormatImplementation)( const char16_type* );
   void           (SetFileFormatIconSVG)( const char* );
   void           (SetFileFormatIconSVGFile)( const char16_type* );
   void           (SetFileFormatIconImage)( const char** ); // ### deprecated
   void           (SetFileFormatIconImageFile)( const char16_type* ); // ### deprecated
   void           (SetFileFormatIconSmallImage)( const char** ); // ### deprecated
   void           (SetFileFormatIconSmallImageFile)( const char16_type* ); // ### deprecated
   void           (SetFileFormatCaps)( const api_format_capabilities* );

   void           (SetFileFormatCreationRoutine)( pcl::format_creation_routine );
   void           (SetFileFormatDestructionRoutine)( pcl::format_destruction_routine );
   void           (SetFileFormatValidateFormatSpecificDataRoutine)( pcl::format_validate_format_specific_data_routine );
   void           (SetFileFormatDisposeFormatSpecificDataRoutine)( pcl::format_dispose_format_specific_data_routine );
   void           (SetFileFormatEditPreferencesRoutine)( pcl::format_edit_preferences_routine );

   void           (SetFileFormatOpenRoutine)( pcl::format_open_routine );
   void           (SetFileFormatGetImageCountRoutine)( pcl::format_get_image_count_routine );
   void           (SetFileFormatGetImageIdRoutine)( pcl::format_get_image_id_routine );
   void           (SetFileFormatGetImageDescriptionRoutine)( pcl::format_get_image_description_routine );
   void           (SetFileFormatCloseRoutine)( pcl::format_close_routine );
   void           (SetFileFormatIsOpenRoutine)( pcl::format_is_open_routine );
   void           (SetFileFormatGetFilePathRoutine)( pcl::format_get_file_path_routine );
   void           (SetFileFormatSetSelectedImageIndexRoutine)( pcl::format_set_selected_image_index_routine );
   void           (SetFileFormatGetSelectedImageIndexRoutine)( pcl::format_get_selected_image_index_routine );
   void           (SetFileFormatSetFormatSpecificDataRoutine)( pcl::format_set_format_specific_data_routine );
   void           (SetFileFormatGetFormatSpecificDataRoutine)( pcl::format_get_format_specific_data_routine );
   void           (SetFileFormatGetImageFormatInfoRoutine)( pcl::format_get_image_format_info_routine );
   void           (SetFileFormatBeginKeywordExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (SetFileFormatGetKeywordCountRoutine)( pcl::format_get_keyword_count_routine );
   void           (SetFileFormatGetNextKeywordRoutine)( pcl::format_get_next_keyword_routine );
   void           (SetFileFormatEndKeywordExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (SetFileFormatBeginICCProfileExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (SetFileFormatGetICCProfileRoutine)( pcl::format_get_icc_profile_routine );
   void           (SetFileFormatEndICCProfileExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (SetFileFormatBeginThumbnailExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (SetFileFormatGetThumbnailRoutine)( pcl::format_get_thumbnail_routine );
   void           (SetFileFormatEndThumbnailExtractionRoutine)( pcl::format_end_extraction_routine );

   void           (SetFileFormatEnumerateImagePropertiesRoutine)( pcl::format_enumerate_image_properties_routine );
   void           (SetFileFormatBeginImagePropertyExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (SetFileFormatGetImagePropertyRoutine)( pcl::format_get_image_property_routine );
   void           (SetFileFormatEndImagePropertyExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (SetFileFormatBeginImagePropertyEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (SetFileFormatSetImagePropertyRoutine)( pcl::format_set_image_property_routine );
   void           (SetFileFormatEndImagePropertyEmbeddingRoutine)( pcl::format_end_embedding_routine );

   void           (SetFileFormatEnumeratePropertiesRoutine)( pcl::format_enumerate_image_properties_routine );
   void           (SetFileFormatBeginPropertyExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (SetFileFormatGetPropertyRoutine)( pcl::format_get_image_property_routine );
   void           (SetFileFormatEndPropertyExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (SetFileFormatBeginPropertyEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (SetFileFormatSetPropertyRoutine)( pcl::format_set_image_property_routine );
   void           (SetFileFormatEndPropertyEmbeddingRoutine)( pcl::format_end_embedding_routine );

   void           (SetFileFormatBeginRGBWSExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (SetFileFormatGetImageRGBWSRoutine)( pcl::format_get_image_rgbws_routine );
   void           (SetFileFormatEndRGBWSExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (SetFileFormatBeginRGBWSEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (SetFileFormatSetImageRGBWSRoutine)( pcl::format_set_image_rgbws_routine );
   void           (SetFileFormatEndRGBWSEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (SetFileFormatBeginDisplayFunctionExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (SetFileFormatGetImageDisplayFunctionRoutine)( pcl::format_get_image_display_function_routine );
   void           (SetFileFormatEndDisplayFunctionExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (SetFileFormatBeginDisplayFunctionEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (SetFileFormatSetImageDisplayFunctionRoutine)( pcl::format_set_image_display_function_routine );
   void           (SetFileFormatEndDisplayFunctionEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (SetFileFormatBeginColorFilterArrayExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (SetFileFormatGetImageColorFilterArrayRoutine)( pcl::format_get_image_color_filter_array_routine );
   void           (SetFileFormatEndColorFilterArrayExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (SetFileFormatBeginColorFilterArrayEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (SetFileFormatSetImageColorFilterArrayRoutine)( pcl::format_set_image_color_filter_array_routine );
   void           (SetFileFormatEndColorFilterArrayEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (SetFileFormatReadImageRoutine)( pcl::format_read_image_routine );
   void           (SetFileFormatAllowIncrementalReadRoutine)( pcl::format_allow_incremental_op_routine );
   void           (SetFileFormatReadSamplesRoutine)( pcl::format_read_pixels_routine );
   void           (SetFileFormatQueryOptionsRoutine)( pcl::format_query_options_routine );
   void           (SetFileFormatCreateRoutine)( pcl::format_create_routine );
   void           (SetFileFormatSetImageIdRoutine)( pcl::format_set_image_id_routine );
   void           (SetFileFormatSetImageOptionsRoutine)( pcl::format_set_image_options_routine );
   void           (SetFileFormatCreateImageRoutine)( pcl::format_create_image_routine );
   void           (SetFileFormatCloseImageRoutine)( pcl::format_close_routine );
   void           (SetFileFormatBeginKeywordEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (SetFileFormatAddKeywordRoutine)( pcl::format_add_keyword_routine );
   void           (SetFileFormatEndKeywordEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (SetFileFormatBeginICCProfileEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (SetFileFormatSetICCProfileRoutine)( pcl::format_set_icc_profile_routine );
   void           (SetFileFormatEndICCProfileEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (SetFileFormatBeginThumbnailEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (SetFileFormatSetThumbnailRoutine)( pcl::format_set_thumbnail_routine );
   void           (SetFileFormatEndThumbnailEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (SetFileFormatWriteImageRoutine)( pcl::format_write_image_routine );
   void           (SetFileFormatAllowIncrementalWriteRoutine)( pcl::format_allow_incremental_op_routine );
   void           (SetFileFormatWriteSamplesRoutine)( pcl::format_write_pixels_routine );
   void           (SetFileFormatQueryInexactReadRoutine)( pcl::format_query_inexact_read_routine );
   void           (SetFileFormatQueryLossyWriteRoutine)( pcl::format_query_lossy_write_routine );
   void           (SetFileFormatQueryFormatStatusRoutine)( pcl::format_query_format_status_routine );
   void           (EndFileFormatDefinition)();

   void           (ExitFileFormatDefinitionContext)();
};

// ----------------------------------------------------------------------------

struct api_context ModuleContext
{
   api_bool    (LoadResource)( api_handle, const char16_type*, const char16_type* );
   api_bool    (UnloadResource)( api_handle, const char16_type*, const char16_type* );
   api_bool    (EvaluateScript)( api_handle, api_property_value* result, const char16_type* sourceCode, const char* language );
   api_bool    (HasEntitlement)( api_handle, const char* );
};

// ----------------------------------------------------------------------------

struct api_context ProcessContext
{
   api_bool             (EnumerateProcessCategories)( pcl::category_enumeration_callback, char*, size_type*, void* );
   api_bool             (EnumerateProcesses)( pcl::process_enumeration_callback, void* );

   meta_process_handle  (GetProcessByName)( api_handle, const char* id );

   api_bool             (GetProcessIdentifier)( meta_process_handle, char*, size_type* );
   api_bool             (GetProcessCategory)( meta_process_handle, char*, size_type* );
   uint32               (GetProcessVersion)( meta_process_handle );
   api_bool             (GetProcessAliasIdentifiers)( meta_process_handle, char*, size_type* );
   api_bool             (GetProcessDescription)( meta_process_handle, char16_type*, size_type* );
   api_bool             (GetProcessScriptComment)( meta_process_handle, char16_type*, size_type* );
   bitmap_handle        (GetProcessIcon)( meta_process_handle );
   bitmap_handle        (GetProcessSmallIcon)( meta_process_handle );
   api_bool             (GetProcessProperties)( meta_process_handle, api_process_properties* );
   interface_handle     (GetProcessDefaultInterface)( meta_process_handle );

   api_bool             (EditProcessPreferences)( meta_process_handle );
   api_bool             (BrowseProcessDocumentation)( meta_process_handle, uint32 flags );
   int32                (RunProcessCommandLine)( meta_process_handle, const char16_type* );
   api_bool             (LaunchProcess)( meta_process_handle );

   api_bool             (EnumerateProcessParameters)( meta_process_handle, pcl::parameter_enumeration_callback, void* );
   api_bool             (EnumerateTableColumns)( meta_parameter_handle, pcl::parameter_enumeration_callback, void* );

   meta_parameter_handle (GetParameterByName)( meta_process_handle, const char* id );
   meta_parameter_handle (GetTableColumnByName)( meta_parameter_handle, const char* id );

   meta_process_handle  (GetParameterProcess)( meta_parameter_handle );
   meta_parameter_handle (GetParameterTable)( meta_parameter_handle );

   uint32               (GetParameterType)( meta_parameter_handle );
   api_bool             (GetParameterIdentifier)( meta_parameter_handle, char*, size_type* );
   api_bool             (GetParameterAliasIdentifiers)( meta_parameter_handle, char*, size_type* );
   api_bool             (GetParameterDescription)( meta_parameter_handle, char16_type*, size_type* );
   api_bool             (GetParameterScriptComment)( meta_parameter_handle, char16_type*, size_type* );
   api_bool             (GetParameterRequired)( meta_parameter_handle );
   api_bool             (GetParameterReadOnly)( meta_parameter_handle );

   // Boolean, numeric and string parameter types
   api_bool             (GetParameterDefaultValue)( meta_parameter_handle, void* value, size_type* length );

   // Enumerated parameters
   size_type            (GetParameterElementCount)( meta_parameter_handle );
   api_bool             (GetParameterElementIdentifier)( meta_parameter_handle, size_type index, char*, size_type* );
   api_bool             (GetParameterElementAliasIdentifiers)( meta_parameter_handle, size_type index, char*, size_type* );
   api_enum             (GetParameterElementValue)( meta_parameter_handle, size_type index );
   int32                (GetParameterDefaultElementIndex)( meta_parameter_handle );

   // Numeric parameters
   api_bool             (GetParameterRange)( meta_parameter_handle, double* minValue, double* maxValue );
   int32                (GetParameterPrecision)( meta_parameter_handle );
   api_bool             (GetParameterScientificNotation)( meta_parameter_handle );

   // Variable length parameters
   api_bool             (GetParameterLengthLimits)( meta_parameter_handle, size_type* minLength, size_type* maxLength );

   // String parameters
   api_bool             (GetParameterAllowedCharacters)( meta_parameter_handle, char16_type*, size_type* );

   process_handle       (CreateProcessInstance)( api_handle, meta_process_handle );

   meta_process_handle  (GetProcessInstanceProcess)( const_process_handle );

   uint32               (GetProcessInstanceVersion)( const_process_handle );

   process_handle       (CloneProcessInstance)( api_handle, const_process_handle, uint32 flags );
   api_bool             (AssignProcessInstance)( process_handle, const_process_handle, uint32 flags );
   api_bool             (ValidateProcessInstance)( process_handle, char16_type*, size_type );

   api_bool             (GetUpdatesViewHistory)( const_process_handle, const_view_handle );
   api_bool             (ValidateViewExecutionMask)( const_process_handle, const_view_handle, const_window_handle );
   api_bool             (ValidateViewExecution)( const_process_handle, const_view_handle, char16_type*, size_type );
   api_bool             (ExecuteOnView)( process_handle, view_handle, uint32 flags );

   api_bool             (ValidateGlobalExecution)( const_process_handle, char16_type*, size_type );
   api_bool             (ExecuteGlobal)( process_handle, uint32 flags );

   api_bool             (ValidateImageExecution)( const_process_handle, const_image_handle, char16_type*, size_type );
   api_bool             (ExecuteOnImage)( process_handle, image_handle, const char*, uint32 flags );

   api_bool             (LaunchProcessInstance)( process_handle );

   api_bool             (ValidateInterfaceLaunch)( const_process_handle );
   api_bool             (LaunchInterface)( process_handle );

   // ### TODO: The following two functions are not yet implemented.
   api_bool             (ValidateInterface)( const_process_handle, const_interface_handle, char16_type*, size_type );
   interface_handle     (GetInterface)( const_process_handle );

   api_bool             (GetProcessInstanceDescription)( const_process_handle, char16_type*, size_type* );
   api_bool             (SetProcessInstanceDescription)( process_handle, const char16_type* );

   api_bool             (GetExecutionTimes)( const_process_handle, double* startJD, double* elapsedSecs );

   // ### Returns a string allocated by the caller module.
   char16_type*         (GetProcessInstanceSourceCode)( api_handle, const_process_handle, const char* language, const char* varId, uint32 indent );
   process_handle       (CreateProcessInstanceFromSourceCode)( const char16_type* source, const char* language );

   process_handle       (CreateProcessInstanceFromIcon)( api_handle, const char* iconId );

   api_bool             (EnumerateProcessIcons)( pcl::icon_enumeration_callback, char*, size_type*, void* );

   api_bool             (GetParameterValue)( const_process_handle, meta_parameter_handle, size_type tableRow, uint32* parType, void* value, size_type* length );
   api_bool             (SetParameterValue)( process_handle, meta_parameter_handle, size_type tableRow, const void* value, size_type length );

   size_type            (GetTableRowCount)( const_process_handle, meta_parameter_handle );

   api_bool             (AllocateTableRows)( process_handle, meta_parameter_handle, size_type rowCount );
};

// ----------------------------------------------------------------------------

struct api_context InterfaceContext
{
   // ### TODO
};

// ----------------------------------------------------------------------------

struct api_context FileFormatContext
{
   api_bool             (EnumerateFileFormats)( pcl::format_enumeration_callback, void* );

   meta_format_handle   (GetFileFormatByName)( api_handle, const char* id );
   meta_format_handle   (GetFileFormatByFileExtension)( api_handle, const char16_type* ext, api_bool toRead, api_bool toWrite );
   meta_format_handle   (GetFileFormatByMimeType)( api_handle, const char* mimeType, api_bool toRead, api_bool toWrite );

   api_bool             (GetFileFormatName)( meta_format_handle, char*, size_type* );
   api_bool             (GetFileFormatFileExtensions)( meta_format_handle, char16_type**, size_type* extCount, size_type* maxExtLen );
   api_bool             (GetFileFormatMimeTypes)( meta_format_handle, char**, size_type* mimeCount, size_type* maxMimeLen );
   uint32               (GetFileFormatVersion)( meta_format_handle );
   api_bool             (GetFileFormatDescription)( meta_format_handle, char16_type*, size_type* );
   api_bool             (GetFileFormatImplementation)( meta_format_handle, char16_type*, size_type* );
   bitmap_handle        (GetFileFormatIcon)( meta_format_handle );
   bitmap_handle        (GetFileFormatSmallIcon)( meta_format_handle );
   api_bool             (GetFileFormatCapabilities)( meta_format_handle, api_format_capabilities* );
   api_bool             (GetFileFormatStatus)( meta_format_handle, char16_type*, size_type*, void* );

   api_bool             (EditFileFormatPreferences)( meta_format_handle );

   file_format_handle   (CreateFileFormatInstance)( api_handle, meta_format_handle );

   meta_format_handle   (GetFileFormatInstanceFormat)( const_file_format_handle );

   api_bool             (CloseImageFile)( file_format_handle );

   api_bool             (IsImageFileOpen)( const_file_format_handle );

   api_bool             (GetImageFilePath)( file_format_handle, char16_type*, size_type* );

   api_bool             (OpenImageFile)( file_format_handle, const char16_type* );
   api_bool             (OpenImageFileEx)( file_format_handle, const char16_type*, const char*, uint32 );
   uint32               (GetImageCount)( const_file_format_handle );
   api_bool             (GetImageId)( const_file_format_handle, char*, size_type*, uint32 );
   api_bool             (GetImageDescription)( const_file_format_handle, api_image_info*, api_image_options*, uint32 );

   api_bool             (SelectImage)( file_format_handle, uint32 );
   uint32               (GetSelectedImageIndex)( const_file_format_handle );

   const void*          (GetFormatSpecificData)( const_file_format_handle );
   api_bool             (SetFormatSpecificData)( file_format_handle, const void* );
   api_bool             (ValidateFormatSpecificData)( meta_format_handle, const void* );
   void                 (DisposeFormatSpecificData)( meta_format_handle, const void* );

   api_bool             (GetImageFormatInfo)( const_file_format_handle, char16_type*, size_type* );

   api_bool             (BeginKeywordExtraction)( file_format_handle );
   size_type            (GetKeywordCount)( file_format_handle );
   api_bool             (GetNextKeyword)( file_format_handle, char*, char*, char*, uint32 );
   void                 (EndKeywordExtraction)( file_format_handle );

   api_bool             (BeginICCProfileExtraction)( file_format_handle );
   const void*          (GetICCProfile)( file_format_handle );
   void                 (EndICCProfileExtraction)( file_format_handle );

   api_bool             (BeginThumbnailExtraction)( file_format_handle );
   api_bool             (GetThumbnail)( file_format_handle, image_handle );
   void                 (EndThumbnailExtraction)( file_format_handle );

   api_bool             (EnumerateProperties)( file_format_handle, pcl::property_enumeration_callback, char*, size_type*, void* );

   api_bool             (BeginPropertyExtraction)( file_format_handle );
   api_bool             (GetProperty)( file_format_handle, const char* id, api_property_value* );
   void                 (EndPropertyExtraction)( file_format_handle );

   api_bool             (BeginPropertyEmbedding)( file_format_handle );
   api_bool             (SetProperty)( file_format_handle, const char* id, const api_property_value* );
   void                 (EndPropertyEmbedding)( file_format_handle );

   api_bool             (EnumerateImageProperties)( file_format_handle, pcl::property_enumeration_callback, char*, size_type*, void* );

   api_bool             (BeginImagePropertyExtraction)( file_format_handle );
   api_bool             (GetImageProperty)( file_format_handle, const char* id, api_property_value* );
   void                 (EndImagePropertyExtraction)( file_format_handle );

   api_bool             (BeginImagePropertyEmbedding)( file_format_handle );
   api_bool             (SetImageProperty)( file_format_handle, const char* id, const api_property_value* );
   void                 (EndImagePropertyEmbedding)( file_format_handle );

   api_bool             (BeginRGBWSExtraction)( file_format_handle );
   api_bool             (GetImageRGBWS)( file_format_handle, float*, api_bool*, float*, float*, float* );
   void                 (EndRGBWSExtraction)( file_format_handle );

   api_bool             (BeginRGBWSEmbedding)( file_format_handle );
   api_bool             (SetImageRGBWS)( file_format_handle, float, api_bool, const float*, const float*, const float* );
   void                 (EndRGBWSEmbedding)( file_format_handle );

   api_bool             (BeginDisplayFunctionExtraction)( file_format_handle );
   api_bool             (GetImageDisplayFunction)( file_format_handle, double*, double*, double*, double*, double* );
   void                 (EndDisplayFunctionExtraction)( file_format_handle );

   api_bool             (BeginDisplayFunctionEmbedding)( file_format_handle );
   api_bool             (SetImageDisplayFunction)( file_format_handle, const double*, const double*, const double*, const double*, const double* );
   void                 (EndDisplayFunctionEmbedding)( file_format_handle );

   api_bool             (BeginColorFilterArrayExtraction)( file_format_handle );
   api_bool             (GetImageColorFilterArray)( file_format_handle, char*, size_type*, int32*, int32*, char16_type*, size_type* );
   void                 (EndColorFilterArrayExtraction)( file_format_handle );

   api_bool             (BeginColorFilterArrayEmbedding)( file_format_handle );
   api_bool             (SetImageColorFilterArray)( file_format_handle, const char*, int32, int32, const char16_type* );
   void                 (EndColorFilterArrayEmbedding)( file_format_handle );

   api_bool             (ReadImage)( file_format_handle, image_handle );

   api_bool             (CanReadIncrementally)( const_file_format_handle );
   api_bool             (ReadSamples)( file_format_handle, void*, uint32, uint32, uint32, uint32, api_bool, api_bool );

   api_bool             (QueryImageFileOptions)( file_format_handle, api_image_options*, const void**, uint32 );

   api_bool             (CreateImageFile)( file_format_handle, const char16_type*, uint32 );
   api_bool             (CreateImageFileEx)( file_format_handle, const char16_type*, uint32, const char*, uint32 );

   api_bool             (SetImageId)( file_format_handle, const char* );
   api_bool             (SetImageOptions)( file_format_handle, const api_image_options* );

   api_bool             (BeginKeywordEmbedding)( file_format_handle );
   api_bool             (AddKeyword)( file_format_handle, const char*, const char*, const char* );
   void                 (EndKeywordEmbedding)( file_format_handle );

   api_bool             (BeginICCProfileEmbedding)( file_format_handle );
   api_bool             (SetICCProfile)( file_format_handle, const void* );
   void                 (EndICCProfileEmbedding)( file_format_handle );

   api_bool             (BeginThumbnailEmbedding)( file_format_handle );
   api_bool             (SetThumbnail)( file_format_handle, const_image_handle );
   void                 (EndThumbnailEmbedding)( file_format_handle );

   api_bool             (WriteImage)( file_format_handle, const_image_handle );

   api_bool             (CanWriteIncrementally)( const_file_format_handle );
   api_bool             (CreateImage)( file_format_handle, const api_image_info* );
   api_bool             (WriteSamples)( file_format_handle, const void*, uint32, uint32, uint32, uint32, api_bool, api_bool );
   api_bool             (CloseImage)( file_format_handle );

   api_bool             (WasInexactRead)( const_file_format_handle );
   api_bool             (WasLossyWrite)( const_file_format_handle );
};

// ----------------------------------------------------------------------------

struct api_context UIContext
{
   api_bool       (AttachToUIObject)( api_handle, api_handle );
   api_bool       (DetachFromUIObject)( api_handle, api_handle );

   api_handle     (GetUIObjectModule)( const_api_handle );

   size_type      (GetUIObjectRefCount)( const_api_handle );

   api_bool       (GetUIObjectType)( const_api_handle, char*, size_type* );

   api_bool       (GetUIObjectId)( const_api_handle, char16_type*, size_type* );
   api_bool       (SetUIObjectId)( api_handle, const char16_type* );

   api_bool       (SetHandleDestroyedEventRoutine)( api_handle, pcl::destroy_event_routine );

   api_bool       (AttachToUIControlObject)( api_handle, api_handle );
   api_bool       (DetachFromUIControlObject)( api_handle, api_handle );

   api_handle     (GetUIControlObjectModule)( const_api_handle );

   size_type      (GetUIControlObjectRefCount)( const_api_handle );

   api_bool       (GetUIControlObjectType)( const_api_handle, char*, size_type* );

   api_bool       (GetUIControlObjectId)( const_api_handle, char16_type*, size_type* );
   api_bool       (SetUIControlObjectId)( api_handle, const char16_type* );
};

// ----------------------------------------------------------------------------

struct api_context ActionContext
{
   action_handle  (CreateActionSVG)( api_handle, api_handle client,
                                                   const char16_type* menuItem, const char16_type* toolBar,
                                                   const char* svgIcon,
                                                   uint32 flags );
   action_handle  (CreateActionSVGFile)( api_handle, api_handle client,
                                                   const char16_type* menuItem, const char16_type* toolBar,
                                                   const char16_type* svgIconPath,
                                                   uint32 flags );
   // ### deprecated
   action_handle  (CreateAction)( api_handle, api_handle client,
                                            const char16_type* menuItem, const char16_type* toolBar,
                                            const_bitmap_handle icon,
                                            uint32 flags );

   api_bool       (GetActionMenuItem)( const_action_handle, char16_type*, size_type* );

   api_bool       (GetActionMenuText)( const_action_handle, char16_type*, size_type* );
   void           (SetActionMenuText)( action_handle, const char16_type* );

   api_bool       (GetActionToolBar)( const_action_handle, char16_type*, size_type* );

   api_bool       (GetActionToolTip)( const_action_handle, char16_type*, size_type* );
   void           (SetActionToolTip)( action_handle, const char16_type* );

   bitmap_handle  (GetActionIcon)( const_action_handle );
   void           (SetActionIconSVG)( action_handle, const char* );
   void           (SetActionIconSVGFile)( action_handle, const char16_type* );
   // ### deprecated
   void           (SetActionIcon)( action_handle, const_bitmap_handle );

   void           (GetActionAccelerator)( const_action_handle, int32* keyModifiers, int32* keyCode );
   void           (SetActionAccelerator)( action_handle, int32 keyModifiers, int32 keyCode );

   api_bool       (SetActionExecutionRoutine)( action_handle, pcl::action_execution_routine );
   api_bool       (SetActionStateQueryRoutine)( action_handle, pcl::action_state_query_routine );
};

// ----------------------------------------------------------------------------

struct api_context ControlContext
{
   control_handle (CreateControl)( api_handle, api_handle client, control_handle parent, uint32 flags );

   void           (GetFrameRect)( const_control_handle, int32*, int32*, int32*, int32* );

   void           (GetClientRect)( const_control_handle, int32*, int32*, int32*, int32* );
   void           (SetClientRect)( control_handle, int32, int32, int32, int32 );

   void           (SetControlSize)( control_handle, int32, int32 );

   void           (AdjustControlToContents)( control_handle );

   void           (GetControlMinSize)( const_control_handle, int32*, int32* );
   void           (SetControlMinSize)( control_handle, int32, int32 );

   void           (GetControlMaxSize)( const_control_handle, int32*, int32* );
   void           (SetControlMaxSize)( control_handle, int32, int32 );

   void           (SetControlFixedSize)( control_handle, int32, int32 );

   void           (GetControlExpansionEnabled)( const_control_handle, api_bool*, api_bool* );
   void           (SetControlExpansionEnabled)( control_handle, api_bool, api_bool );

   void           (GetControlPosition)( const_control_handle, int32*, int32* );
   void           (SetControlPosition)( control_handle, int32, int32 );

   api_bool       (GetControlUnderMouseStatus)( const_control_handle );

   void           (BringControlToFront)( control_handle );
   void           (SendControlToBack)( control_handle );

   void           (StackControls)( control_handle stackThis, control_handle underThis );

   sizer_handle   (GetControlSizer)( const_control_handle );
   void           (SetControlSizer)( control_handle, sizer_handle );

   void           (GlobalToLocal)( const_control_handle, int32*, int32* );
   void           (LocalToGlobal)( const_control_handle, int32*, int32* );
   void           (ParentToLocal)( const_control_handle, int32*, int32* );
   void           (LocalToParent)( const_control_handle, int32*, int32* );
   void           (ControlToLocal)( const_control_handle, const_control_handle, int32*, int32* );
   void           (LocalToControl)( const_control_handle, const_control_handle, int32*, int32* );

   control_handle (GetChildByPos)( const_control_handle, int32, int32 ); // returns client handle

   void           (GetChildrenRect)( const_control_handle, int32*, int32*, int32*, int32* );

   api_bool       (GetControlAncestry)( const_control_handle, const_control_handle );

   control_handle (GetControlParent)( const_control_handle ); // returns client handle
   void           (SetControlParent)( control_handle, control_handle );

   control_handle (GetControlWindow)( const_control_handle ); // returns client handle

   api_bool       (GetControlEnabled)( const_control_handle );
   void           (SetControlEnabled)( control_handle, api_bool );

   api_bool       (GetControlMouseTrackingEnabled)( const_control_handle );
   void           (SetControlMouseTrackingEnabled)( control_handle, api_bool );

   api_bool       (GetControlVisible)( const_control_handle );
   void           (SetControlVisible)( control_handle, api_bool );

   void           (GetControlVisibleRect)( const_control_handle, int32*, int32*, int32*, int32* );

   api_bool       (GetWindowState)( const_control_handle, api_bool* active, api_bool* modal, api_bool* maximized, api_bool* minimized ); // returns true if control is a window

   void           (ActivateWindow)( control_handle );

   api_bool       (GetControlFocus)( const_control_handle );
   void           (SetControlFocus)( control_handle, api_bool );

   int32          (GetControlFocusStyle)( const_control_handle );
   void           (SetControlFocusStyle)( control_handle, int32 );

   control_handle (GetFocusChildControl)( const_control_handle ); // returns client handle

   control_handle (GetChildControlToFocus)( const_control_handle ); // returns client handle
   void           (SetChildControlToFocus)( control_handle, control_handle );

   control_handle (GetNextSiblingControlToFocus)( const_control_handle ); // returns client handle
   void           (SetNextSiblingControlToFocus)( control_handle, control_handle );

   api_bool       (GetControlUpdatesEnabled)( const_control_handle );
   void           (SetControlUpdatesEnabled)( control_handle, api_bool );

   void           (UpdateControl)( control_handle );
   void           (UpdateControlRect)( control_handle, int32, int32, int32, int32 );

   void           (RepaintControl)( control_handle );
   void           (RepaintControlRect)( control_handle, int32, int32, int32, int32 );

   void           (RestyleControl)( control_handle );

   void           (EnsureControlLayoutUpdated)( control_handle );

   void           (ScrollControl)( control_handle, int32, int32 );
   void           (ScrollControlRect)( control_handle, int32, int32, int32, int32, int32, int32 );

   cursor_handle  (GetControlCursor)( const_control_handle );
   void           (SetControlCursor)( control_handle, const_cursor_handle );
   void           (SetControlCursorToParent)( control_handle );

   api_bool       (GetControlStyleSheet)( const_control_handle, char16_type*, size_type* );
   void           (SetControlStyleSheet)( control_handle, const char16_type* );

   uint32         (GetControlBackgroundColor)( const_control_handle );
   void           (SetControlBackgroundColor)( control_handle, uint32 );

   uint32         (GetControlForegroundColor)( const_control_handle );
   void           (SetControlForegroundColor)( control_handle, uint32 );

   uint32         (GetControlCanvasColor)( const_control_handle );
   void           (SetControlCanvasColor)( control_handle, uint32 );

   uint32         (GetControlAlternateCanvasColor)( const_control_handle );
   void           (SetControlAlternateCanvasColor)( control_handle, uint32 );

   uint32         (GetControlTextColor)( const_control_handle );
   void           (SetControlTextColor)( control_handle, uint32 );

   uint32         (GetControlButtonColor)( const_control_handle );
   void           (SetControlButtonColor)( control_handle, uint32 );

   uint32         (GetControlButtonTextColor)( const_control_handle );
   void           (SetControlButtonTextColor)( control_handle, uint32 );

   uint32         (GetControlHighlightColor)( const_control_handle );
   void           (SetControlHighlightColor)( control_handle, uint32 );

   uint32         (GetControlHighlightedTextColor)( const_control_handle );
   void           (SetControlHighlightedTextColor)( control_handle, uint32 );

   font_handle    (GetControlFont)( const_control_handle );
   void           (SetControlFont)( control_handle, const_font_handle );

   void           (GetWindowOpacity)( const_control_handle, double* );
   void           (SetWindowOpacity)( control_handle, double );

   api_bool       (GetWindowTitle)( const_control_handle, char16_type*, size_type* );
   void           (SetWindowTitle)( control_handle, const char16_type* );

   api_bool       (GetInfoText)( const_control_handle, char16_type*, size_type* );
   void           (SetInfoText)( control_handle, const char16_type* );

   api_bool       (GetRealTimePreviewActive)( const_control_handle );
   void           (SetRealTimePreviewActive)( control_handle, api_bool );

   api_bool       (GetTrackViewActive)( const_control_handle );
   void           (SetTrackViewActive)( control_handle, api_bool );

   api_bool       (GetWindowToolTip)( const_control_handle, char16_type*, size_type* );
   void           (SetWindowToolTip)( control_handle, const char16_type* );

   api_bool       (GetControlDisplayPixelRatio)( const_control_handle, double* );
   api_bool       (GetControlResourcePixelRatio)( const_control_handle, double* );
   api_bool       (GetControlDevicePixelRatio)( const_control_handle, double* );

   api_bool       (SetDestroyEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (SetShowEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (SetHideEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (SetCloseEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (SetGetFocusEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (SetLoseFocusEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (SetEnterEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (SetLeaveEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (SetMoveEventRoutine)( control_handle, api_handle, pcl::move_event_routine );
   api_bool       (SetResizeEventRoutine)( control_handle, api_handle, pcl::resize_event_routine );
   api_bool       (SetPaintEventRoutine)( control_handle, api_handle, pcl::paint_event_routine );
   api_bool       (SetKeyPressEventRoutine)( control_handle, api_handle, pcl::keyboard_event_routine );
   api_bool       (SetKeyReleaseEventRoutine)( control_handle, api_handle, pcl::keyboard_event_routine );
   api_bool       (SetMouseMoveEventRoutine)( control_handle, api_handle, pcl::mouse_event_routine );
   api_bool       (SetMouseDoubleClickEventRoutine)( control_handle, api_handle, pcl::mouse_event_routine );
   api_bool       (SetMousePressEventRoutine)( control_handle, api_handle, pcl::mouse_button_event_routine );
   api_bool       (SetMouseReleaseEventRoutine)( control_handle, api_handle, pcl::mouse_button_event_routine );
   api_bool       (SetWheelEventRoutine)( control_handle, api_handle, pcl::wheel_event_routine );
   api_bool       (SetFileDragEventRoutine)( control_handle, api_handle, pcl::file_drag_event_handler );
   api_bool       (SetFileDropEventRoutine)( control_handle, api_handle, pcl::file_drag_event_handler );
   api_bool       (SetViewDragEventRoutine)( control_handle, api_handle, pcl::view_drag_event_handler );
   api_bool       (SetViewDropEventRoutine)( control_handle, api_handle, pcl::view_drag_event_handler );
   api_bool       (SetChildCreateEventRoutine)( control_handle, api_handle, pcl::child_event_routine );
   api_bool       (SetChildDestroyEventRoutine)( control_handle, api_handle, pcl::child_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context DialogContext
{
   control_handle (CreateDialog)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (ExecuteDialog)( control_handle );
   void           (OpenDialog)( control_handle );

   void           (ReturnDialog)( control_handle, int32 );

   api_bool       (GetDialogResizable)( const_control_handle );
   void           (SetDialogResizable)( control_handle, api_bool );

   api_bool       (SetExecuteDialogEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (SetReturnDialogEventRoutine)( control_handle, api_handle, pcl::value_event_routine );

   api_bool       (ExecuteOpenFileDialog)( char16_type* fileName,
                                 const char16_type* caption, const char16_type* initialPath,
                                 const char16_type* filters, const char16_type* selectedExtension );

   api_bool       (ExecuteOpenMultipleFilesDialog)( char16_type* fileName,
                                 ::file_enumeration_callback, void*,
                                 const char16_type* caption, const char16_type* initialPath,
                                 const char16_type* filters, const char16_type* selectedExtension );

   api_bool       (ExecuteSaveFileDialog)( char16_type* filePath,
                                 const char16_type* caption, const char16_type* initialPath,
                                 const char16_type* filters, const char16_type* selectedExtension,
                                 api_bool overwritePrompt );

   api_bool       (ExecuteGetDirectoryDialog)( char16_type* dirPath,
                                 const char16_type* caption, const char16_type* initialPath );
};

// ----------------------------------------------------------------------------

struct api_context FrameContext
{
   control_handle (CreateFrame)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (GetFrameStyle)( const_control_handle );
   void           (SetFrameStyle)( control_handle, int32 );

   int32          (GetFrameLineWidth)( const_control_handle );
   void           (SetFrameLineWidth)( control_handle, int32 );

   int32          (GetFrameBorderWidth)( const_control_handle );
};

// ----------------------------------------------------------------------------

struct api_context GroupBoxContext
{
   control_handle (CreateGroupBox)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );

   api_bool       (GetGroupBoxTitle)( const_control_handle, char16_type*, size_type* );
   void           (SetGroupBoxTitle)( control_handle, const char16_type* );

   api_bool       (GetGroupBoxCheckable)( const_control_handle );
   void           (SetGroupBoxCheckable)( control_handle, api_bool );

   api_bool       (GetGroupBoxChecked)( const_control_handle ); // returns true if group box checked
   void           (SetGroupBoxChecked)( control_handle, api_bool );

   api_bool       (SetGroupBoxCheckEventRoutine)( control_handle, api_handle, pcl::button_check_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context TabBoxContext
{
   control_handle (CreateTabBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (GetTabBoxLength)( const_control_handle );

   int32          (GetTabBoxCurrentPageIndex)( const_control_handle );
   void           (SetTabBoxCurrentPageIndex)( control_handle, int32 );

   control_handle (GetTabBoxPageByIndex)( const_control_handle, int32 ); // returns client handle

   void           (InsertTabBoxPage)( control_handle, int32, control_handle, const char16_type*, const_bitmap_handle );

   void           (RemoveTabBoxPage)( control_handle, int32 );

   int32          (GetTabBoxPosition)( const_control_handle );  // 0=top, 1=bottom
   void           (SetTabBoxPosition)( control_handle, int32 ); //   idem.

   api_bool       (GetTabBoxPageEnabled)( const_control_handle, int32 );
   void           (SetTabBoxPageEnabled)( control_handle, int32, api_bool );

   api_bool       (GetTabBoxPageLabel)( const_control_handle, int32, char16_type*, size_type* );
   void           (SetTabBoxPageLabel)( control_handle, int32, const char16_type* );

   bitmap_handle  (GetTabBoxPageIcon)( const_control_handle, int32 );
   void           (SetTabBoxPageIcon)( control_handle, int32, const_bitmap_handle );

   api_bool       (GetTabBoxPageToolTip)( const_control_handle, int32, char16_type*, size_type* );
   void           (SetTabBoxPageToolTip)( control_handle, int32, const char16_type* );

   control_handle (GetTabBoxLeftControl)( const_control_handle ); // returns client handle
   control_handle (GetTabBoxRightControl)( const_control_handle ); // returns client handle
   void           (SetTabBoxControls)( control_handle, control_handle, control_handle );

   api_bool       (SetTabBoxPageSelectedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context ButtonContext
{
   control_handle (CreatePushButton)( api_handle, api_handle client, const char16_type*, const_bitmap_handle, control_handle parent, uint32 flags );
   control_handle (CreateCheckBox)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );
   control_handle (CreateRadioButton)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );
   control_handle (CreateToolButton)( api_handle, api_handle client, const char16_type*, const_bitmap_handle, api_bool checkable, control_handle parent, uint32 flags );

   api_bool       (GetButtonText)( const_control_handle, char16_type*, size_type* );
   void           (SetButtonText)( control_handle, const char16_type* );

   bitmap_handle  (GetButtonIcon)( const_control_handle );
   void           (SetButtonIcon)( control_handle, const_bitmap_handle );

   void           (GetButtonIconSize)( const_control_handle, int32* w, int32* h );
   void           (SetButtonIconSize)( control_handle, int32 w, int32 h );

   api_bool       (GetButtonPushed)( const_control_handle ); // returns true if button pushed
   void           (SetButtonPushed)( control_handle, api_bool );

   uint32         (GetButtonChecked)( const_control_handle ); // returns 0=unchecked, 1=checked, 2=tristate
   void           (SetButtonChecked)( control_handle, uint32 );

   api_bool       (GetButtonDefaultEnabled)( const_control_handle ); // returns true if button is default
   void           (SetButtonDefaultEnabled)( control_handle, api_bool );

   api_bool       (GetButtonTristateEnabled)( const_control_handle ); // returns true if button is in tristate mode
   void           (SetButtonTristateEnabled)( control_handle, api_bool );

   api_bool       (GetToolButtonCheckable)( const_control_handle );
   void           (SetToolButtonCheckable)( control_handle, api_bool );

   api_bool       (SetButtonClickEventRoutine)( control_handle, api_handle, pcl::button_click_event_routine );
   api_bool       (SetButtonPressEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (SetButtonReleaseEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (SetButtonCheckEventRoutine)( control_handle, api_handle, pcl::button_check_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context EditContext
{
   control_handle (CreateEdit)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );

   api_bool       (GetEditText)( const_control_handle, char16_type*, size_type* );
   void           (SetEditText)( control_handle, const char16_type* );

   api_bool       (GetEditReadOnly)( const_control_handle );
   void           (SetEditReadOnly)( control_handle, api_bool );

   api_bool       (GetEditModified)( const_control_handle );
   void           (SetEditModified)( control_handle, api_bool );

   api_bool       (GetEditPasswordEnabled)( const_control_handle );
   void           (SetEditPasswordEnabled)( control_handle, api_bool );

   int32          (GetEditMaxLength)( const_control_handle );
   void           (SetEditMaxLength)( control_handle, int32 );

   api_bool       (GetEditMask)( const_control_handle, char16_type*, size_type* );
   void           (SetEditMask)( control_handle, const char16_type* );

   api_bool       (GetEditValidatingRegExp)( const_control_handle, char16_type*, size_type*, api_bool* caseSensitive );
   api_bool       (SetEditValidatingRegExp)( control_handle, const char16_type*, api_bool caseSensitive );
   api_bool       (GetEditValid)( const_control_handle );

   void           (SetEditSelected)( control_handle, api_bool );

   int32          (GetEditAlignment)( const_control_handle );  // only left and right alignments
   void           (SetEditAlignment)( control_handle, int32 ); //    idem.

   int32          (GetEditCaretPosition)( const_control_handle );
   void           (SetEditCaretPosition)( control_handle, int32 );

   void           (GetEditSelection)( const_control_handle, int32*, int32* );
   void           (SetEditSelection)( control_handle, int32, int32 );

   api_bool       (GetEditSelectedText)( const_control_handle, char16_type*, size_type* );

   api_bool       (SetEditCompletedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (SetReturnPressedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (SetTextUpdatedEventRoutine)( control_handle, api_handle, pcl::unicode_event_routine );
   api_bool       (SetCaretPositionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
   api_bool       (SetSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context TextBoxContext
{
   control_handle (CreateTextBox)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );

   api_bool       (GetTextBoxText)( const_control_handle, char16_type*, size_type* );
   void           (SetTextBoxText)( control_handle, const char16_type* );

   api_bool       (GetTextBoxReadOnly)( const_control_handle );
   void           (SetTextBoxReadOnly)( control_handle, api_bool );

   void           (SetTextBoxSelected)( control_handle, api_bool );

   int32          (GetTextBoxCaretPosition)( const_control_handle );
   void           (SetTextBoxCaretPosition)( control_handle, int32 );

   void           (GetTextBoxSelection)( const_control_handle, int32*, int32* );
   void           (SetTextBoxSelection)( control_handle, int32, int32 );

   api_bool       (GetTextBoxSelectedText)( const_control_handle, char16_type*, size_type* );

   void           (InsertTextBoxText)( control_handle, const char16_type* );
   void           (DeleteTextBoxText)( control_handle );

   api_bool       (SetTextBoxUpdatedEventRoutine)( control_handle, api_handle, pcl::unicode_event_routine );
   api_bool       (SetTextBoxCaretPositionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
   api_bool       (SetTextBoxSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context ComboBoxContext
{
   control_handle (CreateComboBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (GetComboBoxLength)( const_control_handle );

   int32          (GetComboBoxCurrentItem)( const_control_handle );
   void           (SetComboBoxCurrentItem)( control_handle, int32 );

   int32          (FindComboBoxItem)( const_control_handle, const char16_type*, int32, api_bool exactMatch, api_bool caseSensitive );

   void           (InsertComboBoxItem)( control_handle, int32, const char16_type*, const_bitmap_handle );

   void           (RemoveComboBoxItem)( control_handle, int32 );

   void           (ClearComboBox)( control_handle );

   api_bool       (GetComboBoxItemText)( const_control_handle, int32, char16_type*, size_type* );
   void           (SetComboBoxItemText)( control_handle, int32, const char16_type* );

   bitmap_handle  (GetComboBoxItemIcon)( const_control_handle, int32 );
   void           (SetComboBoxItemIcon)( control_handle, int32, const_bitmap_handle );

   api_bool       (GetComboBoxEditEnabled)( const_control_handle );
   void           (SetComboBoxEditEnabled)( control_handle, api_bool );

   api_bool       (GetComboBoxEditText)( const_control_handle, char16_type*, size_type* );
   void           (SetComboBoxEditText)( control_handle, const char16_type* );

   api_bool       (GetComboBoxAutoCompletionEnabled)( const_control_handle );
   void           (SetComboBoxAutoCompletionEnabled)( control_handle, api_bool );

   void           (GetComboBoxIconSize)( const_control_handle, int32*, int32* );
   void           (SetComboBoxIconSize)( control_handle, int32, int32 );

   int32          (GetComboBoxMaxVisibleItemCount)( const_control_handle );
   void           (SetComboBoxMaxVisibleItemCount)( control_handle, int32 );

   int32          (GetComboBoxMinItemCharWidth)( const_control_handle );
   void           (SetComboBoxMinItemCharWidth)( control_handle, int32 );

   void           (SetComboBoxListVisible)( control_handle, api_bool );

   api_bool       (SetComboBoxItemSelectedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (SetComboBoxItemHighlightedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (SetComboBoxEditTextUpdatedEventRoutine)( control_handle, api_handle, pcl::event_routine );
};

// ----------------------------------------------------------------------------

struct api_context SliderContext
{
   control_handle (CreateSlider)( api_handle, api_handle client, api_bool vertical, control_handle parent, uint32 flags );

   int32          (GetSliderValue)( const_control_handle );
   void           (SetSliderValue)( control_handle, int32 );

   void           (GetSliderRange)( const_control_handle, int32*, int32* );
   void           (SetSliderRange)( control_handle, int32, int32 );

   int32          (GetSliderStepSize)( const_control_handle );
   void           (SetSliderStepSize)( control_handle, int32 );

   int32          (GetSliderPageSize)( const_control_handle );
   void           (SetSliderPageSize)( control_handle, int32 );

   int32          (GetSliderTickInterval)( const_control_handle );
   void           (SetSliderTickInterval)( control_handle, int32 );

   int32          (GetSliderTickStyle)( const_control_handle );
   void           (SetSliderTickStyle)( control_handle, int32 );

   api_bool       (GetSliderTrackingEnabled)( const_control_handle );
   void           (SetSliderTrackingEnabled)( control_handle, api_bool );

   api_bool       (SetSliderValueUpdatedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (SetSliderRangeUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context SpinBoxContext
{
   control_handle (CreateSpinBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   int32          (GetSpinBoxValue)( const_control_handle );
   void           (SetSpinBoxValue)( control_handle, int32 );

   void           (GetSpinBoxRange)( const_control_handle, int32*, int32* );
   void           (SetSpinBoxRange)( control_handle, int32, int32 );

   int32          (GetSpinBoxStepSize)( const_control_handle );
   void           (SetSpinBoxStepSize)( control_handle, int32 );

   api_bool       (GetSpinBoxWrappingEnabled)( const_control_handle );
   void           (SetSpinBoxWrappingEnabled)( control_handle, api_bool );

   api_bool       (GetSpinBoxEditable)( const_control_handle );
   void           (SetSpinBoxEditable)( control_handle, api_bool );

   api_bool       (GetSpinBoxPrefix)( const_control_handle, char16_type*, size_type* );
   void           (SetSpinBoxPrefix)( control_handle, const char16_type* );

   api_bool       (GetSpinBoxSuffix)( const_control_handle, char16_type*, size_type* );
   void           (SetSpinBoxSuffix)( control_handle, const char16_type* );

   api_bool       (GetSpinBoxMinimumValueText)( const_control_handle, char16_type*, size_type* );
   void           (SetSpinBoxMinimumValueText)( control_handle, const char16_type* );

   int32          (GetSpinBoxAlignment)( const_control_handle );  // only left and right alignments
   void           (SetSpinBoxAlignment)( control_handle, int32 ); //    idem.

   api_bool       (SetSpinBoxValueUpdatedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (SetSpinBoxRangeUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context LabelContext
{
   control_handle (CreateLabel)( api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags );

   api_bool       (GetLabelText)( const_control_handle, char16_type*, size_type* );
   void           (SetLabelText)( control_handle, const char16_type* );

   int32          (GetLabelMargin)( const_control_handle );
   void           (SetLabelMargin)( control_handle, int32 );

   int32          (GetLabelAlignment)( const_control_handle );
   void           (SetLabelAlignment)( control_handle, int32 );

   api_bool       (GetLabelWordWrappingEnabled)( const_control_handle );
   void           (SetLabelWordWrappingEnabled)( control_handle, api_bool );

   api_bool       (GetLabelRichTextEnabled)( const_control_handle );
   void           (SetLabelRichTextEnabled)( control_handle, api_bool );
};

// ----------------------------------------------------------------------------

struct api_context BitmapBoxContext
{
   control_handle (CreateBitmapBox)( api_handle, api_handle client, const_bitmap_handle, control_handle parent, uint32 flags );

   bitmap_handle  (GetBitmapBoxBitmap)( const_control_handle );
   void           (SetBitmapBoxBitmap)( control_handle, const_bitmap_handle );

   int32          (GetBitmapBoxMargin)( const_control_handle );
   void           (SetBitmapBoxMargin)( control_handle, int32 );

   api_bool       (GetBitmapBoxAutoFitEnabled)( const_control_handle );
   void           (SetBitmapBoxAutoFitEnabled)( control_handle, api_bool );
};

// ----------------------------------------------------------------------------

struct api_context ScrollBoxContext
{
   control_handle (CreateScrollBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   control_handle (CreateScrollBoxViewport)( control_handle, api_handle client );

   void           (GetScrollBarsVisible)( const_control_handle, api_bool*, api_bool* );
   void           (SetScrollBarsVisible)( control_handle, api_bool, api_bool );

   void           (GetScrollBoxAutoScrollEnabled)( const_control_handle, api_bool*, api_bool* );
   void           (SetScrollBoxAutoScrollEnabled)( control_handle, api_bool, api_bool );

   void           (GetScrollBoxHorizontalRange)( const_control_handle, int32*, int32* );
   void           (SetScrollBoxHorizontalRange)( control_handle, int32, int32 );

   void           (GetScrollBoxVerticalRange)( const_control_handle, int32*, int32* );
   void           (SetScrollBoxVerticalRange)( control_handle, int32, int32 );

   void           (GetScrollBoxPageSize)( const_control_handle, int32*, int32* );
   void           (SetScrollBoxPageSize)( control_handle, int32, int32 );

   void           (GetScrollBoxLineSize)( const_control_handle, int32*, int32* );
   void           (SetScrollBoxLineSize)( control_handle, int32, int32 );

   void           (GetScrollBoxPosition)( const_control_handle, int32*, int32* );
   void           (SetScrollBoxPosition)( control_handle, int32, int32 );

   void           (GetScrollBoxTrackingEnabled)( const_control_handle, api_bool*, api_bool* );
   void           (SetScrollBoxTrackingEnabled)( control_handle, api_bool, api_bool );

   api_bool       (SetScrollBoxHorizontalPosUpdatedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (SetScrollBoxVerticalPosUpdatedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );

   api_bool       (SetScrollBoxHorizontalRangeUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
   api_bool       (SetScrollBoxVerticalRangeUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context TreeBoxContext
{
   control_handle (CreateTreeBox)( api_handle, api_handle client, control_handle parent, uint32 flags );

   control_handle (CreateTreeBoxViewport)( control_handle, api_handle client );

   api_handle     (CreateTreeBoxNode)( api_handle, api_handle nodeClient );

   int32          (GetTreeBoxChildCount)( const_control_handle );

   api_handle     (GetTreeBoxChild)( const_control_handle, int32 idx ); // returns client handle

   int32          (GetTreeBoxChildIndex)( const_control_handle, const_api_handle );

   void           (InsertTreeBoxNode)( control_handle, int32, api_handle );
   void           (RemoveTreeBoxNode)( control_handle, int32 );

   void           (ClearTreeBox)( control_handle );

   api_bool       (GetTreeBoxUniformRowHeightEnabled)( const_control_handle );
   void           (SetTreeBoxUniformRowHeightEnabled)( control_handle, api_bool );

   api_handle     (GetTreeBoxCurrentNode)( const_control_handle ); // returns client handle
   void           (SetTreeBoxCurrentNode)( control_handle, api_handle );

   api_bool       (GetTreeBoxMultipleNodeSelectionEnabled)( const_control_handle );
   void           (SetTreeBoxMultipleNodeSelectionEnabled)( control_handle, api_bool );

   api_bool       (GetTreeBoxSelectedNodes)( const_control_handle, api_handle*, size_type* );

   void           (SelectAllTreeBoxNodes)( control_handle );

   void           (BeginTreeBoxNodeEdition)( control_handle, api_handle, int32 col );
   void           (EndTreeBoxNodeEdition)( control_handle, api_handle, int32 col );

   void           (EditTreeBoxNode)( control_handle, api_handle, int32 col );

   api_handle     (GetTreeBoxNodeByPos)( const_control_handle, int32 x, int32 y ); // returns client handle

   void           (SetTreeBoxNodeIntoView)( control_handle, api_handle );

   void           (GetTreeBoxNodeRect)( const_control_handle, const_api_handle, int32*, int32*, int32*, int32* );

   int32          (GetTreeBoxColumnCount)( const_control_handle );
   void           (SetTreeBoxColumnCount)( control_handle, int32 );

   api_bool       (GetTreeBoxColumnVisible)( const_control_handle, int32 );
   void           (SetTreeBoxColumnVisible)( control_handle, int32, api_bool );

   int32          (GetTreeBoxColumnWidth)( const_control_handle, int32 );
   void           (SetTreeBoxColumnWidth)( control_handle, int32, int32 );

   void           (AdjustTreeBoxColumnWidthToContents)( control_handle, int32 );

   api_bool       (GetTreeBoxHeaderText)( const_control_handle, int32, char16_type*, size_type* );
   void           (SetTreeBoxHeaderText)( control_handle, int32, const char16_type* );

   bitmap_handle  (GetTreeBoxHeaderIcon)( const_control_handle, int32 );
   void           (SetTreeBoxHeaderIcon)( control_handle, int32, const_bitmap_handle );

   int32          (GetTreeBoxHeaderAlignment)( const_control_handle, int32 );
   void           (SetTreeBoxHeaderAlignment)( control_handle, int32, int32 );

   api_bool       (GetTreeBoxHeaderVisible)( const_control_handle );
   void           (SetTreeBoxHeaderVisible)( control_handle, api_bool );

   int32          (GetTreeBoxIndentSize)( const_control_handle );
   void           (SetTreeBoxIndentSize)( control_handle, int32 );

   api_bool       (GetTreeBoxNodeExpansionEnabled)( const_control_handle );
   void           (SetTreeBoxNodeExpansionEnabled)( control_handle, api_bool );

   api_bool       (GetTreeBoxRootDecorationEnabled)( const_control_handle );
   void           (SetTreeBoxRootDecorationEnabled)( control_handle, api_bool );

   api_bool       (GetTreeBoxAlternateRowColorEnabled)( const_control_handle );
   void           (SetTreeBoxAlternateRowColorEnabled)( control_handle, api_bool );

   void           (GetTreeBoxIconSize)( const_control_handle, int32*, int32* );
   void           (SetTreeBoxIconSize)( control_handle, int32, int32 );

   api_bool       (GetTreeBoxHeaderSortingEnabled)( const_control_handle );
   void           (SetTreeBoxHeaderSortingEnabled)( control_handle, api_bool );

   void           (SortTreeBox)( control_handle, int32 col, api_bool ascending );

   api_bool       (GetTreeBoxNodeDraggingEnabled)( const_control_handle );
   void           (SetTreeBoxNodeDraggingEnabled)( control_handle, api_bool );

   // TreeBox Nodes

   control_handle (GetTreeBoxNodeParentBox)( const_api_handle ); // returns client handle

   api_handle     (GetTreeBoxNodeParent)( const_api_handle ); // returns client handle

   int32          (GetTreeBoxNodeChildCount)( const_api_handle );

   api_handle     (GetTreeBoxNodeChild)( const_api_handle, int32 ); // returns client handle

   void           (InsertTreeBoxNodeChild)( api_handle, int32, api_handle );
   void           (RemoveTreeBoxNodeChild)( api_handle, int32 );

   api_bool       (GetTreeBoxNodeEnabled)( const_api_handle );
   void           (SetTreeBoxNodeEnabled)( api_handle, api_bool );

   api_bool       (GetTreeBoxNodeExpanded)( const_api_handle );
   void           (SetTreeBoxNodeExpanded)( api_handle, api_bool );

   api_bool       (GetTreeBoxNodeSelectable)( const_api_handle );
   void           (SetTreeBoxNodeSelectable)( api_handle, api_bool );

   api_bool       (GetTreeBoxNodeSelected)( const_api_handle );
   void           (SetTreeBoxNodeSelected)( api_handle, api_bool );

   api_bool       (GetTreeBoxNodeCheckable)( const_api_handle );
   void           (SetTreeBoxNodeCheckable)( api_handle, api_bool );

   api_bool       (GetTreeBoxNodeChecked)( const_api_handle );
   void           (SetTreeBoxNodeChecked)( api_handle, api_bool );

   api_bool       (GetTreeBoxNodeEditable)( const_api_handle );
   void           (SetTreeBoxNodeEditable)( api_handle, api_bool );

   api_bool       (GetTreeBoxNodeFirstColumnSpanned)( const_api_handle );
   void           (SetTreeBoxNodeFirstColumnSpanned)( api_handle, api_bool );

   api_bool       (GetTreeBoxNodeColText)( const_api_handle, int32, char16_type*, size_type* );
   void           (SetTreeBoxNodeColText)( api_handle, int32, const char16_type* );

   bitmap_handle  (GetTreeBoxNodeColIcon)( const_api_handle, int32 );
   void           (SetTreeBoxNodeColIcon)( api_handle, int32, const_bitmap_handle );

   int32          (GetTreeBoxNodeColAlignment)( const_api_handle, int32 );
   void           (SetTreeBoxNodeColAlignment)( api_handle, int32, int32 );

   api_bool       (GetTreeBoxNodeColToolTip)( const_api_handle, int32, char16_type*, size_type* );
   void           (SetTreeBoxNodeColToolTip)( api_handle, int32, const char16_type* );

   font_handle    (GetTreeBoxNodeColFont)( const_api_handle, int32 );
   void           (SetTreeBoxNodeColFont)( api_handle, int32, const_font_handle );

   uint32         (GetTreeBoxNodeColBackgroundColor)( const_api_handle, int32 );
   void           (SetTreeBoxNodeColBackgroundColor)( api_handle, int32, uint32 );

   uint32         (GetTreeBoxNodeColTextColor)( const_api_handle, int32 );
   void           (SetTreeBoxNodeColTextColor)( api_handle, int32, uint32 );

   // TreeBox Events

   api_bool       (SetTreeBoxCurrentNodeUpdatedEventRoutine)( control_handle, api_handle, pcl::item_range_event_routine );
   api_bool       (SetTreeBoxNodeActivatedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (SetTreeBoxNodeUpdatedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (SetTreeBoxNodeEnteredEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (SetTreeBoxNodeClickedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (SetTreeBoxNodeDoubleClickedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (SetTreeBoxNodeExpandedEventRoutine)( control_handle, api_handle, pcl::item_event_routine );
   api_bool       (SetTreeBoxNodeCollapsedEventRoutine)( control_handle, api_handle, pcl::item_event_routine );
   api_bool       (SetTreeBoxNodeSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::event_routine );
};

// ----------------------------------------------------------------------------

struct api_context TimerContext
{
   timer_handle   (CreateTimer)( api_handle, api_handle client, uint32 flags );

   void           (GetTimerInterval)( const_timer_handle, uint32* msec );
   void           (SetTimerInterval)( timer_handle, uint32 msec );

   api_bool       (GetTimerSingleShot)( const_timer_handle );
   void           (SetTimerSingleShot)( timer_handle, api_bool );

   api_bool       (IsTimerActive)( const_timer_handle );

   api_bool       (StartTimer)( timer_handle );
   void           (StopTimer)( timer_handle );

   api_bool       (SetTimerNotifyEventRoutine)( timer_handle, api_handle, pcl::timer_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context ThreadContext
{
   thread_handle  (CreateThread)( api_handle, api_handle client, uint32 flags );

   void           (StartThread)( thread_handle, uint32 priority );
   void           (KillThread)( thread_handle );

   api_bool       (IsThreadActive)( const_thread_handle );

   uint32         (GetThreadPriority)( const_thread_handle );
   void           (SetThreadPriority)( thread_handle, uint32 );

   uint32         (GetThreadStackSize)( const_thread_handle );
   void           (SetThreadStackSize)( thread_handle, uint32 );

   api_bool       (WaitThread)( thread_handle, uint32 msec );

   void           (SleepThread)( thread_handle, uint32 msec );

   uint32         (GetThreadStatus)( const_thread_handle );
   void           (SetThreadStatus)( thread_handle, uint32 );

   api_bool       (GetThreadStatusEx)( const_thread_handle, uint32* status, uint32 flags ); // 0x00=force_lock 0x01=try_lock

   api_bool       (GetThreadConsoleOutputText)( const_thread_handle, char16_type* text, size_type* len );
   void           (AppendThreadConsoleOutputText)( thread_handle, const char16_type* text, api_bool appendNewline );
   void           (ClearThreadConsoleOutputText)( thread_handle );

   thread_handle  (GetCurrentThread)();

   api_bool       (SetThreadExecRoutine)( thread_handle, pcl::thread_exec_routine );

   int32          (PerformanceAnalysisValue)( int32 algorithm, size_type length,
                                                        int32 itemSize, api_bool floatingPoint, int32 kernelSize, int32 width, int32 height );
};

// ----------------------------------------------------------------------------

struct api_context MutexContext
{
   mutex_handle   (CreateMutex)( api_handle, api_handle client, uint32 flags ); // ### deprecated
   mutex_handle   (CreateReadWriteMutex)( api_handle, api_handle client, uint32 flags );

   api_bool       (GetLockState)( const_mutex_handle ); // ### disabled ### returns api_true if the mutex is locked

   api_bool       (Lock)( mutex_handle, api_bool tryLock ); // ### deprecated
   api_bool       (LockForRead)( mutex_handle, api_bool tryLock );
   api_bool       (LockForWrite)( mutex_handle, api_bool tryLock );

   void           (Unlock)( mutex_handle );
};

// ----------------------------------------------------------------------------

struct api_context ViewListContext
{
   control_handle (CreateViewList)( api_handle, api_handle client, control_handle parent, uint32 flags );

   void           (RegenerateViewList)( control_handle, api_bool mainViews, api_bool previews, api_bool realTimePreview );
   void           (GetViewListContents)( const_control_handle, api_bool* mainViews, api_bool* previews, api_bool* realTimePreview );

   const_view_handle (GetViewListExcludedView)( const_control_handle );
   void           (SetViewListExcludedView)( control_handle, const_view_handle );

   view_handle    (GetViewListCurrentView)( const_control_handle );
   void           (SetViewListCurrentView)( control_handle, view_handle );

   api_bool       (FindViewListView)( const_control_handle, const_view_handle );

   void           (RemoveViewListView)( control_handle, const_view_handle );

   api_bool       (SetViewListViewSelectedEventRoutine)( control_handle, api_handle, pcl::view_event_routine );
   api_bool       (SetViewListCurrentViewUpdatedEventRoutine)( control_handle, api_handle, pcl::view_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context BitmapContext
{
   bitmap_handle  (CreateBitmap)( api_handle, int32, int32, void* );
   bitmap_handle  (CreateBitmapXPM)( api_handle, const char** );
   bitmap_handle  (CreateBitmapFromFile)( api_handle, const char16_type* );
   bitmap_handle  (CreateBitmapFromFile8)( api_handle, const char* );
   bitmap_handle  (CreateBitmapFromData)( api_handle, const void*, size_type, const char*, uint32 );
   bitmap_handle  (CreateEmptyBitmap)( api_handle );
   bitmap_handle  (CloneBitmap)( api_handle, const_bitmap_handle );
   bitmap_handle  (CloneBitmapRect)( api_handle, const_bitmap_handle, int32, int32, int32, int32 );
   bitmap_handle  (CreateBitmapFromSVG)( api_handle, const char*, int32, int32, uint32 flags );
   bitmap_handle  (CreateBitmapFromSVGFile)( api_handle, const char16_type*, int32, int32, uint32 flags );

   int32          (GetBitmapFormat)( bitmap_handle );
   void           (SetBitmapFormat)( bitmap_handle, int32 );

   api_bool       (GetBitmapDimensions)( const_bitmap_handle, int32*, int32* );
   api_bool       (IsEmptyBitmap)( const_bitmap_handle );

   uint32         (GetBitmapPixel)( const_bitmap_handle, int32, int32 );
   void           (SetBitmapPixel)( bitmap_handle, int32, int32, uint32 );

   uint32*        (GetBitmapScanLine)( bitmap_handle, int32 );

   bitmap_handle  (MirroredBitmap)( const_bitmap_handle, api_bool h, api_bool v );
   bitmap_handle  (ScaledBitmap)( const_bitmap_handle, int32, int32, api_bool );
   bitmap_handle  (RotatedBitmap)( const_bitmap_handle, double, api_bool );

   api_bool       (LoadBitmap)( bitmap_handle, const char16_type* );
   api_bool       (SaveBitmap)( const_bitmap_handle, const char16_type*, int32 q );

   api_bool       (LoadBitmapData)( bitmap_handle, const void*, size_type, const char*, uint32 );

   void           (CopyBitmap)( bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );

   void           (FillBitmap)( bitmap_handle, int32, int32, int32, int32, uint32 );

   void           (OrBitmap)( bitmap_handle, int32, int32, int32, int32, uint32 );
   void           (OrBitmaps)( bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );

   void           (AndBitmap)( bitmap_handle, int32, int32, int32, int32, uint32 );
   void           (AndBitmaps)( bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );

   void           (XorBitmap)( bitmap_handle, int32, int32, int32, int32, uint32 );
   void           (XorBitmaps)( bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );

   void           (XorBitmapRect)( bitmap_handle, int32, int32, int32, int32, uint32 );

   void           (ReplaceBitmapColor)( bitmap_handle, int32, int32, int32, int32, uint32, uint32 );

   void           (SetBitmapAlpha)( bitmap_handle, int32, int32, int32, int32, uint8 );

   void           (GetBitmapDevicePixelRatio)( const_bitmap_handle, double* );
   void           (SetBitmapDevicePixelRatio)( bitmap_handle, double );
};

// ----------------------------------------------------------------------------

struct api_context SVGContext
{
   svg_handle     (CreateSVGFile)( api_handle, const char16_type*, int32, int32, uint32 );
   svg_handle     (CreateSVGBuffer)( api_handle, int32, int32, uint32 );

   api_bool       (GetSVGDimensions)( const_svg_handle, int32*, int32* );
   api_bool       (SetSVGDimensions)( svg_handle, int32, int32 );

   api_bool       (GetSVGViewBox)( const_svg_handle, double*, double*, double*, double* );
   api_bool       (SetSVGViewBox)( svg_handle, double, double, double, double );

   int32          (GetSVGResolution)( const_svg_handle );
   void           (SetSVGResolution)( svg_handle, int32 );

   api_bool       (GetSVGFilePath)( const_svg_handle, char16_type*, size_type* );

   api_bool       (GetSVGDataBuffer)( const_svg_handle, void*, size_type* );

   api_bool       (GetSVGTitle)( const_svg_handle, char16_type*, size_type* );
   void           (SetSVGTitle)( svg_handle, const char16_type* );

   api_bool       (GetSVGDescription)( const_svg_handle, char16_type*, size_type* );
   void           (SetSVGDescription)( svg_handle, const char16_type* );

   api_bool       (IsSVGPainting)( const_svg_handle );
};

// ----------------------------------------------------------------------------

struct api_context BrushContext
{
   brush_handle   (CreateBrush)( api_handle, uint32, int32 );
   brush_handle   (CreateBitmapBrush)( api_handle, const_bitmap_handle );
   brush_handle   (CreateLinearGradientBrush)( api_handle, double x1, double y1, double x2, double y2,
                                                         int32 spread, const api_gradient_stop*, size_type count ); // spread: 0=pad 1=reflect 2=repeat
   brush_handle   (CreateRadialGradientBrush)( api_handle, double cx, double cy, double r, double fx, double fy,
                                                         int32 spread, const api_gradient_stop*, size_type count );
   brush_handle   (CreateConicalGradientBrush)( api_handle, double cx, double cy, double angle,
                                                         const api_gradient_stop*, size_type count );
   brush_handle   (CloneBrush)( api_handle, const_brush_handle );

   uint32         (GetBrushColor)( const_brush_handle );
   void           (SetBrushColor)( brush_handle, uint32 );

   int32          (GetBrushStyle)( const_brush_handle );
   void           (SetBrushStyle)( brush_handle, int32 );

   bitmap_handle  (GetBrushBitmap)( const_brush_handle );
   void           (SetBrushBitmap)( brush_handle, const_bitmap_handle );

   int32          (GetBrushGradientType)( const_brush_handle ); // 0=none 1=linear 2=radial 3=conical
   api_bool       (GetBrushLinearGradientParameters)( const_brush_handle, double* x1, double* y1, double* x2, double* y2 );
   api_bool       (GetBrushRadialGradientParameters)( const_brush_handle, double* cx, double* cy, double* r, double* fx, double* fy );
   api_bool       (GetBrushConicalGradientParameters)( const_brush_handle, double* cx, double* cy, double* angle );
   int32          (GetBrushGradientSpread)( const_brush_handle ); // -1=error 0=pad 1=reflect 2=repeat
   api_bool       (GetBrushGradientStops)( const_brush_handle, api_gradient_stop*, size_type *count );
};

// ----------------------------------------------------------------------------

struct api_context PenContext
{
   pen_handle     (CreatePen)( api_handle, uint32, float, int32, int32, int32 );
   pen_handle     (ClonePen)( api_handle, const_pen_handle );

   api_bool       (GetPenWidth)( const_pen_handle, float* );
   void           (SetPenWidth)( pen_handle, float );

   uint32         (GetPenColor)( const_pen_handle );
   void           (SetPenColor)( pen_handle, uint32 );

   int32          (GetPenStyle)( const_pen_handle );
   void           (SetPenStyle)( pen_handle, int32 );

   int32          (GetPenCap)( const_pen_handle );
   void           (SetPenCap)( pen_handle, int32 );

   int32          (GetPenJoin)( const_pen_handle );
   void           (SetPenJoin)( pen_handle, int32 );

   brush_handle   (GetPenBrush)( const_pen_handle );
   void           (SetPenBrush)( pen_handle, const_brush_handle );
};

// ----------------------------------------------------------------------------

struct api_context FontContext
{
   font_handle    (CreateFontByFamily)( api_handle, int32, double );
   font_handle    (CreateFontByFace)( api_handle, const char16_type*, double );
   font_handle    (CloneFont)( api_handle, const_font_handle );

   api_bool       (GetFontFace)( const_font_handle, char16_type*, size_type* );
   void           (SetFontFace)( font_handle, const char16_type* );

   api_bool       (GetFontExactMatch)( const_font_handle );

   int32          (GetFontPixelSize)( const_font_handle );
   void           (SetFontPixelSize)( font_handle, int32 );

   void           (GetFontPointSize)( const_font_handle, double* );
   void           (SetFontPointSize)( font_handle, double );

   api_bool       (GetFontFixedPitch)( const_font_handle );
   void           (SetFontFixedPitch)( font_handle, api_bool );

   api_bool       (GetFontKerning)( const_font_handle );
   void           (SetFontKerning)( font_handle, api_bool );

   int32          (GetFontStretchFactor)( const_font_handle );
   void           (SetFontStretchFactor)( font_handle, int32 );

   int32          (GetFontWeight)( const_font_handle );
   void           (SetFontWeight)( font_handle, int32 );

   api_bool       (GetFontItalic)( const_font_handle );
   void           (SetFontItalic)( font_handle, api_bool );

   api_bool       (GetFontUnderline)( const_font_handle );
   void           (SetFontUnderline)( font_handle, api_bool );

   api_bool       (GetFontOverline)( const_font_handle );
   void           (SetFontOverline)( font_handle, api_bool );

   api_bool       (GetFontStrikeOut)( const_font_handle );
   void           (SetFontStrikeOut)( font_handle, api_bool );

   int32          (GetFontAscent)( const_font_handle );
   int32          (GetFontDescent)( const_font_handle );

   int32          (GetFontHeight)( const_font_handle );

   int32          (GetFontLineSpacing)( const_font_handle );

   api_bool       (GetFontCharDefined)( const_font_handle, int32 );

   int32          (GetFontMaxWidth)( const_font_handle );

   int32          (GetStringPixelWidth)( const_font_handle, const char16_type* );
   int32          (GetCharPixelWidth)( const_font_handle, int32 );
   void           (GetStringPixelRect)( const_font_handle, const char16_type*, int32*, int32*, int32*, int32*, uint32 flags );

   api_bool       (EnumerateFonts)( font_enumeration_callback f,
                                                char16_type* fontFace, size_type* len, void* data,
                                                const char* writingSystem );

   api_bool       (EnumerateWritingSystems)( font_enumeration_callback f,
                                                char16_type* wrSystem, size_type* len, void* data,
                                                const char16_type* font );

   api_bool       (EnumerateFontStyles)( font_enumeration_callback f,
                                                char16_type* style, size_type* len, void* data,
                                                const char16_type* font );

   api_bool       (EnumerateOptimalFontPointSizes)(font_size_enumeration_callback f,
                                                double* ptSize, void* data,
                                                const char16_type* font, const char16_type* style );

   api_bool       (GetFontScalable)( const char16_type* font, const char16_type* style );
   api_bool       (GetNominalFontFixedPitch)( const char16_type* font, const char16_type* style );
   api_bool       (GetNominalFontItalic)( const char16_type* font, const char16_type* style );
   int32          (GetNominalFontWeight)( const char16_type* font, const char16_type* style );
};

// ----------------------------------------------------------------------------

struct api_context CursorContext
{
   cursor_handle  (CreateCursor)( api_handle, int32 );
   cursor_handle  (CreateBitmapCursor)( api_handle, const_bitmap_handle, int32, int32 );
   cursor_handle  (CloneCursor)( api_handle, const_cursor_handle );

   void           (GetCursorHotSpot)( const_cursor_handle, int32*, int32* );
};

// ----------------------------------------------------------------------------

struct api_context SizerContext
{
   sizer_handle   (CreateSizer)( api_handle, api_bool vertical );

   control_handle (GetSizerParentControl)( const_sizer_handle ); // returns client handle

   api_bool       (GetSizerOrientation)( const_sizer_handle ); // true=vertical

   int32          (GetSizerCount)( const_sizer_handle );

   int32          (GetSizerIndex)( const_sizer_handle, const_sizer_handle );
   int32          (GetSizerControlIndex)( const_sizer_handle, const_control_handle );

   void           (InsertSizer)( sizer_handle, int32, sizer_handle, int32 );
   void           (InsertSizerControl)( sizer_handle, int32, control_handle, int32, int32 );
   void           (InsertSizerSpacing)( sizer_handle, int32, int32 );
   void           (InsertSizerStretch)( sizer_handle, int32, int32 );

   void           (RemoveSizer)( sizer_handle, sizer_handle );
   void           (RemoveSizerControl)( sizer_handle, control_handle );

   void           (SetSizerStretchFactor)( sizer_handle, sizer_handle, int32 );
   void           (SetSizerControlStretchFactor)( sizer_handle, control_handle, int32 );

   void           (SetSizerAlignment)( sizer_handle, sizer_handle, int32 );
   void           (SetSizerControlAlignment)( sizer_handle, control_handle, int32 );

   int32          (GetSizerMargin)( const_sizer_handle );
   void           (SetSizerMargin)( sizer_handle, int32 );

   int32          (GetSizerSpacing)( const_sizer_handle );
   void           (SetSizerSpacing)( sizer_handle, int32 );

   api_bool       (GetSizerDisplayPixelRatio)( const_sizer_handle, double* );
   api_bool       (GetSizerResourcePixelRatio)( const_sizer_handle, double* );
   api_bool       (GetSizerDevicePixelRatio)( const_sizer_handle, double* );
};

// ----------------------------------------------------------------------------

struct api_context GraphicsContext
{
   graphics_handle (CreateGraphics)( api_handle );

   api_bool       (BeginControlPaint)( graphics_handle, control_handle );
   api_bool       (BeginBitmapPaint)( graphics_handle, bitmap_handle );
   api_bool       (BeginSVGPaint)( graphics_handle, svg_handle );
   void           (EndPaint)( graphics_handle );
   api_bool       (GetGraphicsStatus)( const_graphics_handle );

   api_bool       (GetGraphicsTransformationEnabled)( const_graphics_handle );
   void           (EnableGraphicsTransformation)( graphics_handle, api_bool );
   void           (GetGraphicsTransformationMatrix)( const_graphics_handle, double* m11, double* m12, double* m13, double* m21, double* m22, double* m23, double* m31, double* m32, double* m33 );
   void           (SetGraphicsTransformationMatrix)( graphics_handle, double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33 );
   void           (MultiplyGraphicsTransformationMatrix)( graphics_handle, double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33 );
   void           (RotateGraphicsTransformation)( graphics_handle, double );
   void           (ScaleGraphicsTransformation)( graphics_handle, double, double );
   void           (TranslateGraphicsTransformation)( graphics_handle, double, double );
   void           (ShearGraphicsTransformation)( graphics_handle, double, double );
   void           (ResetGraphicsTransformation)( graphics_handle );
   void           (TransformPoints)( const_graphics_handle, double* xy, size_type n );

   api_bool       (GetGraphicsClippingEnabled)( const_graphics_handle );
   void           (EnableGraphicsClipping)( graphics_handle, api_bool );
   void           (GetGraphicsClipRect)( const_graphics_handle, int32*, int32*, int32*, int32* );
   void           (SetGraphicsClipRect)( graphics_handle, int32, int32, int32, int32 );
   void           (GetGraphicsClipRectD)( const_graphics_handle, double*, double*, double*, double* );
   void           (SetGraphicsClipRectD)( graphics_handle, double, double, double, double );

   api_bool       (GetGraphicsAntialiasingEnabled)( const_graphics_handle );
   void           (EnableGraphicsAntialiasing)( graphics_handle, api_bool );

   api_bool       (GetGraphicsTextAntialiasingEnabled)( const_graphics_handle );
   void           (EnableGraphicsTextAntialiasing)( graphics_handle, api_bool );

   api_bool       (GetGraphicsSmoothInterpolationEnabled)( const_graphics_handle );
   void           (EnableGraphicsSmoothInterpolation)( graphics_handle, api_bool );

   int32          (GetGraphicsCompositionOperator)( const_graphics_handle );
   void           (SetGraphicsCompositionOperator)( graphics_handle, int32 );

   void           (GetGraphicsOpacity)( const_graphics_handle, double* );
   void           (SetGraphicsOpacity)( graphics_handle, double );

   brush_handle   (GetGraphicsBackgroundBrush)( const_graphics_handle );
   void           (SetGraphicsBackgroundBrush)( graphics_handle, const_brush_handle );

   api_bool       (GetGraphicsTransparentBackgroundEnabled)( const_graphics_handle );
   void           (SetGraphicsTransparentBackground)( graphics_handle, api_bool );

   pen_handle     (GetGraphicsPen)( const_graphics_handle );
   void           (SetGraphicsPen)( graphics_handle, const_pen_handle );

   brush_handle   (GetGraphicsBrush)( const_graphics_handle );
   void           (SetGraphicsBrush)( graphics_handle, const_brush_handle );

   void           (GetGraphicsBrushOrigin)( const_graphics_handle, int32*, int32* );
   void           (SetGraphicsBrushOrigin)( graphics_handle, int32, int32 );
   void           (GetGraphicsBrushOriginD)( const_graphics_handle, double*, double* );
   void           (SetGraphicsBrushOriginD)( graphics_handle, double, double );

   font_handle    (GetGraphicsFont)( const_graphics_handle );
   void           (SetGraphicsFont)( graphics_handle, const_font_handle );

   void           (PushGraphicsState)( graphics_handle );
   void           (PopGraphicsState)( graphics_handle );

   void           (DrawPoint)( graphics_handle, int32, int32 );
   void           (DrawPointD)( graphics_handle, double, double );

   void           (DrawLine)( graphics_handle, int32, int32, int32, int32 );
   void           (DrawLineD)( graphics_handle, double, double, double, double );

   void           (DrawRect)( graphics_handle, int32, int32, int32, int32 );
   void           (StrokeRect)( graphics_handle, int32, int32, int32, int32, const_pen_handle );
   void           (FillRect)( graphics_handle, int32, int32, int32, int32, const_brush_handle );
   void           (DrawRectD)( graphics_handle, double, double, double, double );
   void           (StrokeRectD)( graphics_handle, double, double, double, double, const_pen_handle );
   void           (FillRectD)( graphics_handle, double, double, double, double, const_brush_handle );

   void           (DrawRoundedRect)( graphics_handle, int32, int32, int32, int32, double, double );
   void           (StrokeRoundedRect)( graphics_handle, int32, int32, int32, int32, double, double, const_pen_handle );
   void           (FillRoundedRect)( graphics_handle, int32, int32, int32, int32, double, double, const_brush_handle );
   void           (DrawRoundedRectD)( graphics_handle, double, double, double, double, double, double );
   void           (StrokeRoundedRectD)( graphics_handle, double, double, double, double, double, double, const_pen_handle );
   void           (FillRoundedRectD)( graphics_handle, double, double, double, double, double, double, const_brush_handle );

   void           (DrawEllipse)( graphics_handle, int32, int32, int32, int32 );
   void           (StrokeEllipse)( graphics_handle, int32, int32, int32, int32, const_pen_handle );
   void           (FillEllipse)( graphics_handle, int32, int32, int32, int32, const_brush_handle );
   void           (DrawEllipseD)( graphics_handle, double, double, double, double );
   void           (StrokeEllipseD)( graphics_handle, double, double, double, double, const_pen_handle );
   void           (FillEllipseD)( graphics_handle, double, double, double, double, const_brush_handle );

   void           (DrawPolygon)( graphics_handle, const int32*, size_type, int32 );
   void           (StrokePolygon)( graphics_handle, const int32*, size_type, int32, const_pen_handle );
   void           (FillPolygon)( graphics_handle, const int32*, size_type, int32, const_brush_handle );
   void           (DrawPolygonD)( graphics_handle, const double*, size_type, int32 );
   void           (StrokePolygonD)( graphics_handle, const double*, size_type, int32, const_pen_handle );
   void           (FillPolygonD)( graphics_handle, const double*, size_type, int32, const_brush_handle );

   void           (DrawPolyline)( graphics_handle, const int32*, size_type );
   void           (DrawPolylineD)( graphics_handle, const double*, size_type );

   void           (DrawArc)( graphics_handle, int32, int32, int32, int32, double, double );
   void           (DrawArcD)( graphics_handle, double, double, double, double, double, double );

   void           (DrawChord)( graphics_handle, int32, int32, int32, int32, double, double );
   void           (StrokeChord)( graphics_handle, int32, int32, int32, int32, double, double, const_pen_handle );
   void           (FillChord)( graphics_handle, int32, int32, int32, int32, double, double, const_brush_handle );
   void           (DrawChordD)( graphics_handle, double, double, double, double, double, double );
   void           (StrokeChordD)( graphics_handle, double, double, double, double, double, double, const_pen_handle );
   void           (FillChordD)( graphics_handle, double, double, double, double, double, double, const_brush_handle );

   void           (DrawPie)( graphics_handle, int32, int32, int32, int32, double, double );
   void           (StrokePie)( graphics_handle, int32, int32, int32, int32, double, double, const_pen_handle );
   void           (FillPie)( graphics_handle, int32, int32, int32, int32, double, double, const_brush_handle );
   void           (DrawPieD)( graphics_handle, double, double, double, double, double, double );
   void           (StrokePieD)( graphics_handle, double, double, double, double, double, double, const_pen_handle );
   void           (FillPieD)( graphics_handle, double, double, double, double, double, double, const_brush_handle );

   void           (DrawBitmap)( graphics_handle, int32, int32, const_bitmap_handle );
   void           (DrawBitmapD)( graphics_handle, double, double, const_bitmap_handle );

   void           (DrawBitmapRect)( graphics_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );
   void           (DrawBitmapRectD)( graphics_handle, double, double, const_bitmap_handle, double, double, double, double );

   void           (DrawScaledBitmap)( graphics_handle, int32, int32, int32, int32, const_bitmap_handle );
   void           (DrawScaledBitmapD)( graphics_handle, double, double, double, double, const_bitmap_handle );

   void           (DrawScaledBitmapRect)( graphics_handle, int32, int32, int32, int32, const_bitmap_handle, int32, int32, int32, int32 );
   void           (DrawScaledBitmapRectD)( graphics_handle, double, double, double, double, const_bitmap_handle, double, double, double, double );

   void           (DrawTiledBitmap)( graphics_handle, int32, int32, int32, int32, const_bitmap_handle, int32, int32 );
   void           (DrawTiledBitmapD)( graphics_handle, double, double, double, double, const_bitmap_handle, double, double );

   void           (DrawText)( graphics_handle, int32, int32, const char16_type* );
   void           (DrawTextD)( graphics_handle, double, double, const char16_type* );

   void           (DrawTextRect)( graphics_handle, int32, int32, int32, int32, const char16_type*, int32 );
   void           (DrawTextRectD)( graphics_handle, double, double, double, double, const char16_type*, int32 );

   void           (GetTextRect)( graphics_handle, int32, int32, int32, int32, const char16_type*, int32, int32*, int32*, int32*, int32* );
   void           (GetTextRectD)( graphics_handle, double, double, double, double, const char16_type*, int32, double*, double*, double*, double* );
};

// ----------------------------------------------------------------------------

struct api_context RealTimePreviewContext
{
   api_bool       (SetRealTimePreviewOwner)( interface_handle, uint32 flags );
   api_bool       (IsRealTimePreviewUpdating)();
   void           (UpdateRealTimePreview)();
   void           (ShowRealTimePreviewProgressDialog)( const char16_type* title, const char16_type* text, size_type total, uint32 flags );
   void           (CloseRealTimePreviewProgressDialog)();
   api_bool       (IsRealTimePreviewProgressDialogVisible)();
   void           (SetRealTimePreviewProgressCount)( size_type newCount, uint32 flags );
   void           (SetRealTimePreviewProgressText)( const char16_type* text, uint32 flags );
};

// ----------------------------------------------------------------------------

struct api_context NumericalContext
{
   /*
    * Gauss-Jordan Solver: A*X = B
    * On output: A = Inv(A) and B = X
    */
   api_bool       (GaussJordanInPlaceF)( float** A, float** B, int32 rows, int32 cols );
   api_bool       (GaussJordanInPlaceD)( double** A, double** B, int32 rows, int32 cols );

   /*
    * Singular Value Decomposition: A = U*W*Vt
    * On output: A = U
    */
   api_bool       (SVDInPlaceF)( float** A, float* W, float** V, int32 rows, int32 cols );
   api_bool       (SVDInPlaceD)( double** A, double* W, double** V, int32 rows, int32 cols );

   /*
    * Linear Fit: y = a + b*x with minimized average absolute deviation.
    * Returns: 1=OK, -1=error, 0=aborted
    */
   api_enum       (LinearFitF)( double* a, double* b, double* adev, const float* fx, const float* fy, size_type n,
                                          api_bool (*callback)( void* ), void* );
   api_enum       (LinearFitD)( double* a, double* b, double* adev, const double* fx, const double* fy, size_type n,
                                          api_bool (*callback)( void* ), void* );

   /*
    * Cubic Splines
    */
   api_bool       (CubicSplineGenerateF)( float* dy2, const float* fx, const float* fy, float dy1, float dyn, int32 n );
   api_bool       (CubicSplineGenerateD)( double* dy2, const double* fx, const double* fy, double dy1, double dyn, int32 n );

   api_bool       (NaturalCubicSplineGenerateF)( float* dy2, const float* fx, const float* fy, int32 n );
   api_bool       (NaturalCubicSplineGenerateD)( double* dy2, const double* fx, const double* fy, int32 n );

   api_bool       (CubicSplineInterpolateF)( float* y, const float* fx, const float* fy, const float* dy2, int32 n, double x, int32* k );
   api_bool       (CubicSplineInterpolateD)( double* y, const double* fx, const double* fy, const double* dy2, int32 n, double x, int32* k );

   api_bool       (NaturalGridCubicSplineGenerateF)( float* dy2, const float* fy, int32 n );
   api_bool       (NaturalGridCubicSplineGenerateD)( double* dy2, const double* fy, int32 n );
   api_bool       (NaturalGridCubicSplineGenerateUI8)( float* dy2, const uint8* fy, int32 n );
   api_bool       (NaturalGridCubicSplineGenerateUI16)( float* dy2, const uint16* fy, int32 n );
   api_bool       (NaturalGridCubicSplineGenerateUI32)( double* dy2, const uint32* fy, int32 n );

   api_bool       (NaturalGridCubicSplineInterpolateF)( float* y, const float* fy, const float* dy2, int32 n, double x );
   api_bool       (NaturalGridCubicSplineInterpolateD)( double* y, const double* fy, const double* dy2, int32 n, double x );
   api_bool       (NaturalGridCubicSplineInterpolateUI8)( float* y, const uint8* fy, const float* dy2, int32 n, double x );
   api_bool       (NaturalGridCubicSplineInterpolateUI16)( float* y, const uint16* fy, const float* dy2, int32 n, double x );
   api_bool       (NaturalGridCubicSplineInterpolateUI32)( double* y, const uint32* fy, const double* dy2, int32 n, double x );

   /*
    * Two-Dimensional Surface Splines (with core implementation).
    */
   api_bool       (SurfaceSplineCreateF)( sspline_handle* hSS, int32 rbf, double e2, api_bool polynomial,
                                                    const float* x, const float* y, const float* z, int32 n, int32 m,
                                                    float rho, const float* w );
   api_bool       (SurfaceSplineCreateD)( sspline_handle* hSS, int32 rbf, double e2, api_bool polynomial,
                                                    const double* x, const double* y, const double* z, int32 n, int32 m,
                                                    float rho, const float* w );

   api_bool       (SurfaceSplineEvaluate)( const_sspline_handle hSS, double* z, double x, double y );
   api_bool       (SurfaceSplineEvaluateVectorF)( const_sspline_handle hSS, float* z, const float *x, const float *y,
                                                            double x0, double y0, double r, size_type n );
   api_bool       (SurfaceSplineEvaluateVectorD)( const_sspline_handle hSS, double* z, const double *x, const double *y,
                                                            double x0, double y0, double r, size_type n );

   api_bool       (SurfaceSplineDestroy)( sspline_handle hSS );

   // ### The following function returns a null-terminated string allocated by the caller module.
   char*          (SurfaceSplineSerialize)( api_handle hModule, const_sspline_handle hSS, uint32 flags );
   api_bool       (SurfaceSplineDeserialize)( sspline_handle* hSS, const char* data, size_type len, uint32 flags );

   api_bool       (SurfaceSplineDuplicate)( sspline_handle* hSS1, const_sspline_handle hSS );

   /*
    * Fast Fourier Transforms (one-dimensional)
    */
   size_type      (FFTComplexOptimizedLengthF)( size_type n );
   size_type      (FFTComplexOptimizedLengthD)( size_type n );

   size_type      (FFTRealOptimizedLengthF)( size_type n );
   size_type      (FFTRealOptimizedLengthD)( size_type n );

   fft_handle     (FFTCreateComplexTransformF)( size_type n );
   fft_handle     (FFTCreateComplexTransformD)( size_type n );

   fft_handle     (FFTCreateComplexInverseTransformF)( size_type n );
   fft_handle     (FFTCreateComplexInverseTransformD)( size_type n );

   fft_handle     (FFTCreateRealTransformF)( size_type n );
   fft_handle     (FFTCreateRealTransformD)( size_type n );

   fft_handle     (FFTCreateRealInverseTransformF)( size_type n );
   fft_handle     (FFTCreateRealInverseTransformD)( size_type n );

   api_bool       (FFTDestroyTransform)( fft_handle hFFT );

   api_bool       (FFTComplexTransformF)( fft_handle hFFT, void* y, const void* x ); // void* = fcomplex*
   api_bool       (FFTComplexTransformD)( fft_handle hFFT, void* y, const void* x ); // void* = dcomplex*

   api_bool       (FFTComplexInverseTransformF)( fft_handle hFFT, void* y, const void* x ); // void* = fcomplex*
   api_bool       (FFTComplexInverseTransformD)( fft_handle hFFT, void* y, const void* x ); // void* = dcomplex*

   api_bool       (FFTRealTransformF)( fft_handle hFFT, void* y, const float* x );   // void* = fcomplex*
   api_bool       (FFTRealTransformD)( fft_handle hFFT, void* y, const double* x );  // void* = dcomplex*

   api_bool       (FFTRealInverseTransformF)( fft_handle hFFT, float* x, const void* y );  // void* = fcomplex*
   api_bool       (FFTRealInverseTransformD)( fft_handle hFFT, double* x, const void* y ); // void* = dcomplex*
};

// ----------------------------------------------------------------------------

struct api_context GPUContext
{
   api_bool       (InitCUDARuntime)( api_handle, uint32 /*flags*/ );

   api_bool       (IsCUDADeviceAvailable)( api_handle );
   api_bool       (EnumerateCUDADevices)( api_handle, pcl::cuda_device_enumeration_callback,
                                                    void* deviceProps/*cudaDeviceProp*/, size_type structSize, void* data );

   cuda_device_handle (GetCUDASelectedDevice)( api_handle );

   api_bool       (GetCUDADeviceProperties)( api_handle, cuda_device_handle, void* deviceProps, size_type structSize );
   size_type      (GetCUDADeviceTotalGlobalMem)( api_handle, cuda_device_handle );
   int32          (GetCUDADeviceMaxThreadsPerBlock)( api_handle, cuda_device_handle );
   size_type      (GetCUDADeviceSharedMemoryPerBlock)( api_handle, cuda_device_handle );
};

// ----------------------------------------------------------------------------

struct api_context SharedImageContext
{
   image_handle   (CreateImage)( uint32 w, uint32 h, uint32 n, uint32 nbits, api_bool flt, uint32 cs, void* );

   void*          (GetImageOwner)( const_image_handle );

   api_bool       (GetImageRefCount)( const_image_handle, uint32* );

   api_bool       (IsValidImageHandle)( const_image_handle );

   api_bool       (AttachToImage)( image_handle, void* );
   api_bool       (DetachFromImage)( image_handle, void* );

   api_bool       (GetImageFormat)( const_image_handle, uint32* nbits, api_bool* flt );

   api_bool       (GetImageGeometry)( const_image_handle, uint32* w, uint32* h, uint32* n );
   api_bool       (SetImageGeometry)( image_handle, uint32 w, uint32 h, uint32 n );

   api_bool       (GetImageColorSpace)( const_image_handle, uint32* cs );
   api_bool       (SetImageColorSpace)( image_handle, uint32 cs );

   api_bool       (GetImageRGBWS)( const_image_handle, api_RGBWS* );
   api_bool       (SetImageRGBWS)( image_handle, const api_RGBWS* ); // ### must be set through ImageWindow

   api_bool       (GetImagePixelData)( image_handle, void*** );
   api_bool       (SetImagePixelData)( image_handle, void** );
};

// ----------------------------------------------------------------------------

/*
 * ### TODO - ASAP: Implement history management (ala PJSR).
 */
struct api_context ViewContext
{
   view_handle    (GetViewById)( const char* fullId );

   void           (EnumerateViews)( pcl::view_enumeration_callback, void*, api_bool includeMainViews, api_bool includePreviews );

   api_bool       (IsPreview)( const_view_handle );
   api_bool       (IsVolatilePreview)( const_view_handle );
   api_bool       (IsStoredPreview)( const_view_handle );

   window_handle  (GetViewParentWindow)( const_view_handle );

   api_bool       (GetViewId)( const_view_handle, char*, size_type* );
   api_bool       (GetViewFullId)( const_view_handle, char*, size_type* );
   api_bool       (SetViewId)( view_handle, const char* );

   void           (GetViewLocks)( const_view_handle, api_bool*, api_bool* );
   void           (LockView)( view_handle, api_bool, api_bool, api_bool );
   void           (UnlockView)( view_handle, api_bool, api_bool, api_bool );

   api_bool       (IsViewDynamicTarget)( const_view_handle );
   void           (AddViewToDynamicTargets)( view_handle );
   void           (RemoveViewFromDynamicTargets)( view_handle );

   image_handle   (GetViewImage)( view_handle );
   api_bool       (IsViewColorImage)( const_view_handle );
   api_bool       (GetViewDimensions)( const_view_handle, int32*, int32* );

   api_bool       (GetViewScreenTransferFunctions)( const_view_handle, double* m, double* c0, double* c1, double* r0, double* r1 );
   api_bool       (SetViewScreenTransferFunctions)( view_handle, const double* m, const double* c0, const double* c1, const double* r0, const double* r1, api_bool );
   api_bool       (DestroyViewScreenTransferFunctions)( view_handle, api_bool );
   api_bool       (GetViewScreenTransferFunctionsEnabled)( view_handle );
   void           (SetViewScreenTransferFunctionsEnabled)( view_handle, api_bool, api_bool );

   api_bool       (IsReservedViewPropertyId)( const char* id );
   api_bool       (EnumerateViewProperties)( const_view_handle, pcl::property_enumeration_callback, char*, size_type*, void* );
   api_bool       (GetViewPropertyValue)( api_handle hModule, const_view_handle, const char* id, api_property_value* );
   api_bool       (GetViewPropertyAttributes)( api_handle hModule, const_view_handle, const char* id, uint32* flags, uint64* type );
   api_bool       (SetViewPropertyValue)( api_handle hModule, view_handle, const char* id, const api_property_value*, uint32 flags, api_bool notify );
   api_bool       (SetViewPropertyAttributes)( api_handle hModule, view_handle, const char* id, uint32 flags, api_bool notify );
   api_bool       (GetViewPropertyExists)( api_handle hModule, const_view_handle, const char* id, uint64* type );
   api_bool       (DeleteViewProperty)( api_handle hModule, view_handle, const char* id, api_bool notify );
   api_bool       (ComputeViewProperty)( api_handle hModule, view_handle, const char* id, api_bool notify, api_property_value* );

};

// ----------------------------------------------------------------------------

struct api_context ImageWindowContext
{
   window_handle  (CreateImageWindow)( int32 width, int32 height, int32 numberOfChannels,
                                                 int32 bitsPerSample, api_bool floatSample, api_bool color,
                                                 api_bool initialProcessing, const char* id );

   api_bool       (LoadImageWindows)( const char16_type* url, const char* id, const char* hints,
                                                api_bool asACopy, api_bool allowMessages,
                                                pcl::window_enumeration_callback, void* );

   api_bool       (CloseImageWindow)( window_handle, api_bool force );

   window_handle  (GetImageWindowById)( const char* );
   window_handle  (GetImageWindowByFilePath)( const char16_type* );

   window_handle  (GetActiveImageWindow)();

   void           (EnumerateImageWindows)( pcl::window_enumeration_callback, void*, api_bool includeIconic );

   void           (EnumeratePreviews)( const_window_handle, pcl::view_enumeration_callback, void* );

   api_bool       (GetImageWindowNewFlag)( const_window_handle );
   api_bool       (GetImageWindowCopyFlag)( const_window_handle );

   api_bool       (GetImageWindowFileURL)( const_window_handle, char16_type*, size_type* );

   api_bool       (GetImageWindowFilePath)( const_window_handle, char16_type*, size_type* );

   api_bool       (GetImageWindowFileInfo)( const_window_handle, api_image_file_info* );

   size_type      (GetImageWindowModifyCount)( const_window_handle );

   view_handle    (GetImageWindowMainView)( const_window_handle );

   view_handle    (GetImageWindowCurrentView)( const_window_handle );
   void           (SetImageWindowCurrentView)( window_handle, view_handle );

   int32          (GetImageType)( const_window_handle );
   api_bool       (SetImageType)( window_handle, int32 imageType, api_bool notify );

   void           (PurgeImageWindowProperties)( window_handle );

   api_bool       (ValidateImageWindowView)( const_window_handle, const_view_handle );

   int32          (GetPreviewCount)( const_window_handle );

   view_handle    (GetPreviewById)( const_window_handle, const char* );

   view_handle    (GetSelectedPreview)( const_window_handle );
   void           (SelectPreview)( window_handle, view_handle );

   view_handle    (CreatePreview)( window_handle, int32, int32, int32, int32, const char* );

   void           (ModifyPreview)( window_handle, const char*, int32, int32, int32, int32, const char* );

   void           (GetPreviewRect)( const_window_handle, const char*, int32*, int32*, int32*, int32* );

   void           (DeletePreview)( window_handle, const char* );
   void           (DeletePreviews)( window_handle );

   window_handle  (GetImageWindowMask)( const_window_handle, api_bool* inverted );
   void           (SetImageWindowMask)( window_handle, window_handle, api_bool inverted );

   api_bool       (GetImageWindowMaskEnabled)( const_window_handle );
   void           (SetImageWindowMaskEnabled)( window_handle, api_bool );

   api_bool       (GetImageWindowMaskVisible)( const_window_handle );
   void           (SetImageWindowMaskVisible)( window_handle, api_bool );

   api_bool       (ValidateImageWindowMask)( const_window_handle, const_window_handle );

   int32          (GetMaskReferenceCount)( const_window_handle );

   void           (RemoveImageWindowMaskReferences)( window_handle );
   void           (UpdateImageWindowMaskReferences)( window_handle );

   void           (GetImageWindowSampleFormat)( const_window_handle, uint32* nbits, api_bool* flt );
   void           (SetImageWindowSampleFormat)( window_handle, uint32 nbits, api_bool flt );

   void           (GetImageWindowRGBWS)( const_window_handle, api_RGBWS* );
   void           (SetImageWindowRGBWS)( window_handle, const api_RGBWS* );

   api_bool       (GetImageWindowGlobalRGBWS)( const_window_handle );
   void           (SetImageWindowGlobalRGBWS)( window_handle );

   void           (GetGlobalRGBWS)( api_RGBWS* );
   void           (SetGlobalRGBWS)( const api_RGBWS* );

   void           (GetImageWindowCMEnabled)( const_window_handle, api_bool* enableCM, api_bool* proofing, api_bool* gamutCheck );
   void           (SetImageWindowCMEnabled)( window_handle, api_bool enableCM, api_bool proofing, api_bool gamutCheck );

   uint32         (GetImageWindowICCProfileLength)( const_window_handle );
   void           (GetImageWindowICCProfile)( const_window_handle, void* );
   void           (SetImageWindowICCProfile)( window_handle, const void* );
   void           (LoadImageWindowICCProfile)( window_handle, const char16_type* );
   void           (DeleteImageWindowICCProfile)( window_handle );

   int32          (GetImageWindowKeywordCount)( const_window_handle );
   void           (GetImageWindowKeyword)( const_window_handle, int32, char*, size_type, char*, size_type, char*, size_type );
   void           (AddImageWindowKeyword)( window_handle, const char*, const char*, const char* );
   void           (ResetImageWindowKeywords)( window_handle );

   api_bool       (GetImageWindowHasAstrometricSolution)( const_window_handle );
   api_bool       (RegenerateImageWindowAstrometricSolution)( window_handle, api_bool, api_bool );
   api_bool       (CopyImageWindowAstrometricSolution)( window_handle, const_window_handle, api_bool );
   void           (ClearImageWindowAstrometricSolution)( window_handle, api_bool );
   void           (UpdateImageWindowAstrometryMetadata)( window_handle, api_bool );

   api_bool       (ImageToCelestial)( const_window_handle, double* x, double* y, api_bool rawRA );
   api_bool       (CelestialToImage)( const_window_handle, double* ra, double* dec );

   void           (GetImageWindowResolution)( const_window_handle, double*, double*, api_bool* );
   void           (SetImageWindowResolution)( window_handle, double, double, api_bool );

   void           (GetDefaultResolution)( double*, double*, api_bool* );
   void           (GetDefaultICCProfileEmbedding)( api_bool* rgb, api_bool* grayscale );
   api_bool       (GetDefaultThumbnailEmbedding)();
   api_bool       (GetDefaultPropertiesEmbedding)();

   api_bool       (GetSwapDirectory)( int32, char16_type*, size_type* );
   api_bool       (SetSwapDirectories)( const char16_type**, int32 );

   int32          (GetCursorTolerance)();

   int32          (GetImageWindowTransparencyMode)( const_window_handle, uint32* );
   void           (SetImageWindowTransparencyMode)( window_handle, int32, uint32 );

   int32          (GetTransparencyBackgroundBrush)( uint32* fgColor, uint32* bgColor );
   void           (SetTransparencyBackgroundBrush)( int32 brush, uint32 fgColor, uint32 bgColor );

   int32          (GetImageWindowMode)();
   void           (SetImageWindowMode)( int32 );

   int32          (GetImageWindowDisplayChannel)( const_window_handle );
   void           (SetImageWindowDisplayChannel)( window_handle, int32 );

   int32          (GetImageWindowMaskMode)( const_window_handle );
   void           (SetImageWindowMaskMode)( window_handle, int32 );

   void           (FitImageWindow)( window_handle );
   void           (ZoomImageWindowToFit)( window_handle, api_bool, api_bool, api_bool, api_bool );

   int32          (GetImageWindowZoomFactor)( const_window_handle );
   void           (SetImageWindowZoomFactor)( window_handle, int32 );

   void           (UpdateImageWindowViewport)( window_handle );

   void           (RegenerateImageWindowViewport)( window_handle );

   void           (SetImageWindowViewport)( window_handle, double cx, double cy, int32 zoom );

   void           (GetImageWindowViewportSize)( const_window_handle, int32*, int32* );

   void           (GetImageWindowViewportOrigin)( const_window_handle, int32*, int32* );

   void           (GetImageWindowViewportPosition)( const_window_handle, int32*, int32* );
   void           (SetImageWindowViewportPosition)( window_handle, int32, int32 );

   void           (GetImageWindowVisibleViewportRect)( const_window_handle, int32*, int32*, int32*, int32* );

   api_bool       (GetImageWindowVisible)( const_window_handle );
   void           (SetImageWindowVisible)( window_handle, api_bool );

   api_bool       (GetImageWindowIconic)( const_window_handle );
   void           (SetImageWindowIconic)( window_handle, api_bool );

   void           (BringImageWindowToFront)( window_handle );
   void           (SendImageWindowToBack)( window_handle );

   interface_handle (GetActiveDynamicInterface)();
   api_bool       (TerminateDynamicSession)( api_bool closeInterface );

   void           (SetDynamicCursorXPM)( window_handle, const char**, int32 hx, int32 hy ); // ### deprecated
   void           (SetDynamicCursor)( window_handle, const_bitmap_handle, int32 hx, int32 hy );

   bitmap_handle  (GetDynamicCursorBitmap)( const_window_handle );
   void           (GetDynamicCursorHotSpot)( const_window_handle, int32* hx, int32* hy );

   void           (ViewportToImageArray)( const_window_handle, int32*, size_type n );
   void           (ViewportToImageArrayD)( const_window_handle, double*, size_type n );

   void           (ViewportToImage)( const_window_handle, int32* x, int32* y );
   void           (ViewportToImageD)( const_window_handle, double* x, double* y );

   void           (ImageToViewportArray)( const_window_handle, int32*, size_type n );
   void           (ImageToViewportArrayD)( const_window_handle, double*, size_type n );

   void           (ImageToViewport)( const_window_handle, int32* x, int32* y );
   void           (ImageToViewportD)( const_window_handle, double* x, double* y );

   void           (ViewportScalarToImageArray)( const_window_handle, int32*, size_type n );
   void           (ViewportScalarToImageArrayD)( const_window_handle, double*, size_type n );

   void           (ViewportScalarToImage)( const_window_handle, int32* );
   void           (ViewportScalarToImageD)( const_window_handle, double* );

   void           (ImageScalarToViewportArray)( const_window_handle, int32*, size_type n );
   void           (ImageScalarToViewportArrayD)( const_window_handle, double*, size_type n );

   void           (ImageScalarToViewport)( const_window_handle, int32* );
   void           (ImageScalarToViewportD)( const_window_handle, double* );

   void           (ViewportToGlobal)( const_window_handle, int32* x, int32* y );
   void           (GlobalToViewport)( const_window_handle, int32* x, int32* y );

   void           (UpdateViewportRect)( window_handle, int32, int32, int32, int32 );
   void           (UpdateImageRect)( window_handle, double, double, double, double );

   void           (RegenerateViewportRect)( window_handle, int32, int32, int32, int32 );
   void           (RegenerateImageRect)( window_handle, double, double, double, double );

   void           (CommitViewportUpdates)( window_handle );

   api_bool       (GetViewportUpdateRect)( const_window_handle, int32*, int32*, int32*, int32* );

   void           (BeginViewportSelection)( window_handle, int32 x, int32 y, uint32 flags );
   void           (ModifyViewportSelection)( window_handle, int32 x, int32 y, uint32 flags );
   void           (UpdateViewportSelection)( window_handle );
   void           (CancelViewportSelection)( window_handle );
   void           (EndViewportSelection)( window_handle );
   api_bool       (GetViewportSelection)( const_window_handle, int32* x0, int32* y0, int32* x1, int32* y1, uint32* flags );

   bitmap_handle  (GetViewportBitmap)( api_handle, const_window_handle, int32 x0, int32 y0, int32 x1, int32 y1, uint32 flags );

   api_bool       (GetImageWindowDisplayPixelRatio)( const_window_handle, double* );
   api_bool       (GetImageWindowResourcePixelRatio)( const_window_handle, double* );
   api_bool       (GetImageWindowDevicePixelRatio)( const_window_handle, double* );
};

// ----------------------------------------------------------------------------

struct api_context ImageViewContext
{
   control_handle (CreateImageView)( api_handle hModule, api_handle hClient, control_handle hParent, uint32 flags,
                                               int32 width, int32 height, int32 numberOfChannels,
                                               int32 bitsPerSample, api_bool floatSample, api_bool color );

   control_handle (CreateImageViewViewport)( control_handle, api_handle hClient );

   image_handle   (GetImageViewImage)( const_control_handle );
   api_bool       (IsImageViewColorImage)( const_control_handle );
   api_bool       (GetImageViewImageGeometry)( const_control_handle hView, int32*, int32*, int32* );

   api_bool       (GetImageViewSampleFormat)( const_control_handle, int32* nbits, api_bool* flt );
   void           (SetImageViewSampleFormat)( control_handle, int32 nbits, api_bool flt );

   void           (GetImageViewRGBWS)( const_control_handle, api_RGBWS* );
   void           (SetImageViewRGBWS)( control_handle, const api_RGBWS* );

   void           (GetImageViewCMEnabled)( const_control_handle, api_bool* enableCM, api_bool* proofing, api_bool* gamutCheck );
   void           (SetImageViewCMEnabled)( control_handle, api_bool enableCM, api_bool proofing, api_bool gamutCheck );

   uint32         (GetImageViewICCProfileLength)( const_control_handle );
   void           (GetImageViewICCProfile)( const_control_handle, void* );
   void           (SetImageViewICCProfile)( control_handle, const void* );
   void           (LoadImageViewICCProfile)( control_handle, const char16_type* );
   void           (DeleteImageViewICCProfile)( control_handle );

   api_bool       (SetImageViewScrollEventRoutine)( control_handle, api_handle, pcl::range_event_routine );

   /*
    * ### TODO
    * Unify viewport routines for ImageWindow and ImageView, i.e. create a new ImageViewport control and context.
    */

   int32          (GetImageViewMode)( const_control_handle );
   void           (SetImageViewMode)( control_handle, int32 );

   int32          (GetImageViewDisplayChannel)( const_control_handle );
   void           (SetImageViewDisplayChannel)( control_handle, int32 );

   int32          (GetImageViewZoomFactor)( const_control_handle );
   void           (SetImageViewZoomFactor)( control_handle, int32 );

   int32          (GetImageViewTransparencyMode)( const_control_handle, uint32* );
   void           (SetImageViewTransparencyMode)( control_handle, int32, uint32 );

   void           (UpdateImageViewViewport)( control_handle );

   void           (RegenerateImageViewViewport)( control_handle );

   void           (SetImageViewViewport)( control_handle, double cx, double cy, int32 zoom );

   void           (GetImageViewViewportSize)( const_control_handle, int32*, int32* );

   void           (GetImageViewViewportOrigin)( const_control_handle, int32*, int32* );

   void           (GetImageViewViewportPosition)( const_control_handle, int32*, int32* );
   void           (SetImageViewViewportPosition)( control_handle, int32, int32 );

   void           (GetImageViewVisibleViewportRect)( const_control_handle, int32*, int32*, int32*, int32* );

   void           (ViewportToImageArray)( const_control_handle, int32*, size_type n );
   void           (ViewportToImageArrayD)( const_control_handle, double*, size_type n );

   void           (ViewportToImage)( const_control_handle, int32* x, int32* y );
   void           (ViewportToImageD)( const_control_handle, double* x, double* y );

   void           (ImageToViewportArray)( const_control_handle, int32*, size_type n );
   void           (ImageToViewportArrayD)( const_control_handle, double*, size_type n );

   void           (ImageToViewport)( const_control_handle, int32* x, int32* y );
   void           (ImageToViewportD)( const_control_handle, double* x, double* y );

   void           (ViewportScalarToImageArray)( const_control_handle, int32*, size_type n );
   void           (ViewportScalarToImageArrayD)( const_control_handle, double*, size_type n );

   void           (ViewportScalarToImage)( const_control_handle, int32* );
   void           (ViewportScalarToImageD)( const_control_handle, double* );

   void           (ImageScalarToViewportArray)( const_control_handle, int32*, size_type n );
   void           (ImageScalarToViewportArrayD)( const_control_handle, double*, size_type n );

   void           (ImageScalarToViewport)( const_control_handle, int32* );
   void           (ImageScalarToViewportD)( const_control_handle, double* );

   void           (ViewportToGlobal)( const_control_handle, int32* x, int32* y );
   void           (GlobalToViewport)( const_control_handle, int32* x, int32* y );

   void           (UpdateViewportRect)( control_handle, int32, int32, int32, int32 );
   void           (UpdateImageRect)( control_handle, double, double, double, double );

   void           (RegenerateViewportRect)( control_handle, int32, int32, int32, int32 );
   void           (RegenerateImageRect)( control_handle, double, double, double, double );

   void           (CommitViewportUpdates)( control_handle );

   api_bool       (GetViewportUpdateRect)( const_control_handle, int32*, int32*, int32*, int32* );

   void           (BeginViewportSelection)( control_handle, int32 x, int32 y, uint32 flags );
   void           (ModifyViewportSelection)( control_handle, int32 x, int32 y, uint32 flags );
   void           (UpdateViewportSelection)( control_handle );
   void           (CancelViewportSelection)( control_handle );
   void           (EndViewportSelection)( control_handle );
   api_bool       (GetViewportSelection)( const_control_handle, int32* x0, int32* y0, int32* x1, int32* y1, uint32* flags );

   bitmap_handle  (GetViewportBitmap)( api_handle, const_control_handle, int32 x0, int32 y0, int32 x1, int32 y1, uint32 flags );
};

// ----------------------------------------------------------------------------

struct api_context CodeEditorContext
{
   control_handle (CreateCodeEditor)( api_handle hModule, api_handle hClient, control_handle hParent, uint32 flags );

   control_handle (CreateEditorLineNumbersControl)( control_handle, api_handle hClient, control_handle hParent, uint32 flags );

   api_bool       (GetEditorFilePath)( const_control_handle, char16_type*, size_type* );
   void           (SetEditorFilePath)( control_handle, const char16_type* );

   api_bool       (GetEditorText)( const_control_handle, char16_type*, size_type* );
   void           (SetEditorText)( control_handle, const char16_type* );

   api_bool       (GetEditorEncodedText)( const_control_handle, char*, size_type*, const char* encoding );
   api_bool       (SetEditorEncodedText)( control_handle, const char*, const char* encoding );

   void           (ClearEditorText)( control_handle );

   api_bool       (GetEditorReadOnly)( const_control_handle );
   void           (SetEditorReadOnly)( control_handle, api_bool );

   api_bool       (SaveEditorText)( control_handle, const char16_type* filePath, const char* encoding );
   api_bool       (LoadEditorText)( control_handle, const char16_type* filePath, const char* encoding );

   int32          (GetEditorLineCount)( const_control_handle );
   int32          (GetEditorCharacterCount)( const_control_handle );

   void           (GetEditorCursorCoordinates)( const_control_handle, int32* line, int32* col );
   void           (SetEditorCursorCoordinates)( control_handle, int32 line, int32 col );

   api_bool       (GetEditorInsertMode)( const_control_handle );
   void           (SetEditorInsertMode)( control_handle, api_bool );

   api_bool       (GetEditorBlockSelectionMode)( const_control_handle );
   void           (SetEditorBlockSelectionMode)( control_handle, api_bool );

   api_bool       (GetEditorDynamicWordWrapMode)( const_control_handle );
   void           (SetEditorDynamicWordWrapMode)( control_handle, api_bool );

   int32          (GetEditorUndoSteps)( const_control_handle );
   int32          (GetEditorRedoSteps)( const_control_handle );

   api_bool       (GetEditorHasSelection)( const_control_handle );
   void           (GetEditorSelectionCoordinates)( const_control_handle, int32* fromLine, int32* fromCol, int32* toLine, int32* toCol );
   void           (SetEditorSelectionCoordinates)( control_handle, int32 fromLine, int32 fromCol, int32 toLine, int32 toCol );
   api_bool       (GetEditorSelectedText)( const_control_handle, char16_type*, size_type* );

   void           (InsertEditorText)( control_handle, const char16_type* );

   void           (EditorUndo)( control_handle );
   void           (EditorRedo)( control_handle );
   void           (EditorCut)( control_handle );
   void           (EditorCopy)( control_handle );
   void           (EditorPaste)( control_handle );
   void           (EditorDelete)( control_handle );
   void           (EditorSelectAll)( control_handle );
   void           (EditorUnselect)( control_handle );

   api_bool       (EditorGotoMatchedParenthesis)( control_handle );

   int32          (EditorHighlightAllMatches)( control_handle, const char16_type*, uint32 flags );
   void           (EditorClearMatches)( control_handle );

   api_bool       (EditorFind)( control_handle, const char16_type*, uint32 flags );

   api_bool       (EditorReplace)( control_handle, const char16_type* );

   int32          (EditorReplaceAll)( control_handle, const char16_type*, const char16_type*, uint32 flags );

   api_bool       (SetEditorTextUpdatedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (SetEditorCursorPositionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
   api_bool       (SetEditorSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::rect_event_routine );
   api_bool       (SetEditorOverwriteModeUpdatedEventRoutine)( control_handle, api_handle, pcl::state_event_routine );
   api_bool       (SetEditorSelectionModeUpdatedEventRoutine)( control_handle, api_handle, pcl::state_event_routine );
   api_bool       (SetEditorDynamicWordWrapModeUpdatedEventRoutine)( control_handle, api_handle, pcl::state_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context WebViewContext
{
   control_handle (CreateWebView)( api_handle hModule, api_handle hClient, control_handle hParent, uint32 flags );

   api_bool       (SetWebViewContent)( control_handle, const void* data, size_type size, const char* mimeType );
   api_bool       (LoadWebViewContent)( control_handle, const char16_type* URI );

   api_bool       (RequestWebViewPlainText)( const_control_handle );
   api_bool       (RequestWebViewHTML)( const_control_handle );

   api_bool       (SaveWebViewAsPDF)( control_handle, const char16_type* filePath,
                                                const double* pageWidth, const double* pageHeight,
                                                const double* marginLeft, const double* marginTop, const double* marginRight, const double* marginBottom,
                                                int32 orientation );

   api_bool       (GetWebViewHasSelection)( const_control_handle );
   api_bool       (GetWebViewSelectedText)( const_control_handle, char16_type*, size_type* );

   api_bool       (GetWebViewZoomFactor)( const_control_handle, double* );
   api_bool       (SetWebViewZoomFactor)( control_handle, const double* );

   uint32         (GetWebViewBackgroundColor)( const_control_handle );
   api_bool       (SetWebViewBackgroundColor)( control_handle, uint32 );

   api_bool       (ReloadWebView)( control_handle );
   api_bool       (StopWebView)( control_handle );

   api_bool       (EvaluateWebViewScript)( control_handle, const char16_type* sourceCode, const char* language );

   api_bool       (SetWebViewLoadStartedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (SetWebViewLoadProgressEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (SetWebViewLoadFinishedEventRoutine)( control_handle, api_handle, pcl::state_event_routine );
   api_bool       (SetWebViewSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (SetWebViewPlainTextAvailableEventRoutine)( control_handle, api_handle, pcl::unicode_event_routine );
   api_bool       (SetWebViewHTMLAvailableEventRoutine)( control_handle, api_handle, pcl::unicode_event_routine );
   api_bool       (SetWebViewScriptResultAvailableEventRoutine)( control_handle, api_handle, pcl::property_event_routine );
};

// ----------------------------------------------------------------------------

struct api_context ExternalProcessContext
{
   enum IOStream
   {
      StandardInput,
      StandardOutput,
      StandardError,
      CombinedOutput
   };

   enum ExitStatus
   {
      NormalExit,
      CrashedExit,
      InvalidExit
   };

   enum ErrorCode
   {
      FailedToStart,
      Crashed,
      TimedOut,
      ReadError,
      WriteError,
      UnknownError,
      InvalidError
   };

   int32          (ExecuteProgram)( const char16_type* program, const char16_type** argv, size_type argc );

   api_bool       (StartProgram)( const char16_type* program, const char16_type** argv, size_type argc,
                                            const char16_type* workingDirectory, uint64* pid );

   external_process_handle (CreateExternalProcess)( api_handle hModule, api_handle hClient );

   api_bool       (StartExternalProcess)( external_process_handle,
                                                    const char16_type* program, const char16_type** argv, size_type argc );

   api_bool       (WaitForExternalProcessStarted)( external_process_handle, int32 ms );
   api_bool       (WaitForExternalProcessFinished)( external_process_handle, int32 ms );
   api_bool       (WaitForExternalProcessDataAvailable)( external_process_handle, int32 ms );
   api_bool       (WaitForExternalProcessDataWritten)( external_process_handle, int32 ms );

   api_bool       (TerminateExternalProcess)( external_process_handle );
   api_bool       (KillExternalProcess)( external_process_handle );

   api_bool       (CloseExternalProcessStream)( external_process_handle, int32 stream );

   api_bool       (RedirectExternalProcessToFile)( external_process_handle, int32 stream, const char16_type* fileName, api_bool append );
   api_bool       (PipeExternalProcess)( external_process_handle, int32 stream, external_process_handle toProcess );

   api_bool       (GetExternalProcessWorkingDirectory)( const_external_process_handle, char16_type*, size_type* );
   api_bool       (SetExternalProcessWorkingDirectory)( external_process_handle, const char16_type* );

   api_bool       (GetExternalProcessIsRunning)( const_external_process_handle );
   api_bool       (GetExternalProcessIsStarting)( const_external_process_handle );

   uint64         (GetExternalProcessPID)( const_external_process_handle );

   int32          (GetExternalProcessExitCode)( const_external_process_handle );
   int32          (GetExternalProcessExitStatus)( const_external_process_handle );
   int32          (GetExternalProcessErrorCode)( const_external_process_handle );

   size_type      (GetExternalProcessBytesAvailable)( const_external_process_handle );
   size_type      (GetExternalProcessBytesToWrite)( const_external_process_handle );

   // ### The following function returns data allocated by the caller module.
   api_bool       (ReadFromExternalProcess)( api_handle hModule, external_process_handle, int32 stream, void**, size_type* );
   api_bool       (WriteToExternalProcess)( external_process_handle, const void*, size_type count );

   api_bool       (EnumerateExternalProcessEnvironment)( const_external_process_handle, pcl::environment_enumeration_callback, void* );
   api_bool       (SetExternalProcessEnvironment)( external_process_handle, const char16_type** vars, size_type count );

   api_bool       (SetExternalProcessStartedEventRoutine)( external_process_handle, api_handle, pcl::external_process_event_routine );
   api_bool       (SetExternalProcessFinishedEventRoutine)( external_process_handle, api_handle, pcl::external_process_exit_status_event_routine );
   api_bool       (SetExternalProcessStandardOutputDataAvailableEventRoutine)( external_process_handle, api_handle, pcl::external_process_event_routine );
   api_bool       (SetExternalProcessStandardErrorDataAvailableEventRoutine)( external_process_handle, api_handle, pcl::external_process_event_routine );
   api_bool       (SetExternalProcessErrorEventRoutine)( external_process_handle, api_handle, pcl::external_process_status_event_routine );
};

// ----------------------------------------------------------------------------

/*
 * ### TODO: Somewhere during the 1.8 cycle:
 *           Improve NetworkTransfer with asynchronous network access support.
 */
struct api_context NetworkTransferContext
{
   network_transfer_handle (CreateNetworkTransfer)( api_handle hModule, api_handle hClient );

   api_bool       (SetNetworkTransferURL)( network_transfer_handle, const char16_type* url, const char16_type* userName, const char16_type* userPassword );
   api_bool       (SetNetworkTransferProxyURL)( network_transfer_handle, const char16_type* proxy, const char16_type* userName, const char16_type* userPassword );
   api_bool       (SetNetworkTransferSSL)( network_transfer_handle, api_bool useSSL, api_bool forceSSL, api_bool verifyPeer, api_bool verifyHost );
   api_bool       (SetNetworkTransferCustomHTTPHeaders)( network_transfer_handle, const char16_type* nlsHeaders );
   api_bool       (SetNetworkTransferConnectionTimeout)( network_transfer_handle, int32 seconds );

   api_bool       (PerformNetworkTransferDownload)( network_transfer_handle );
   api_bool       (PerformNetworkTransferUpload)( network_transfer_handle, fsize_type uploadSize );
   api_bool       (PerformNetworkTransferPOST)( network_transfer_handle, const char16_type* postFields );
   api_bool       (PerformNetworkTransferSMTP)( network_transfer_handle, const char16_type* mailFrom, const char16_type* mailRecipients );

   void           (CloseNetworkTransferConnection)( network_transfer_handle );

   api_bool       (GetNetworkTransferURL)( const_network_transfer_handle, char16_type*, size_type* );
   api_bool       (GetNetworkTransferProxyURL)( const_network_transfer_handle, char16_type*, size_type* );
   api_bool       (GetNetworkTransferCustomHTTPHeaders)( const_network_transfer_handle, char16_type*, size_type* );
   api_bool       (GetNetworkTransferStatus)( const_network_transfer_handle );
   api_bool       (GetNetworkTransferIsAborted)( const_network_transfer_handle );
   int32          (GetNetworkTransferResponseCode)( const_network_transfer_handle );
   api_bool       (GetNetworkTransferContentType)( const_network_transfer_handle, char16_type*, size_type* );
   fsize_type     (GetNetworkTransferBytesTransferred)( const_network_transfer_handle );
   void           (GetNetworkTransferTotalSpeed)( const_network_transfer_handle, double* ); // in KiB/s
   void           (GetNetworkTransferTotalTime)( const_network_transfer_handle, double* ); // in s
   api_bool       (GetNetworkTransferErrorInformation)( const_network_transfer_handle, char16_type*, size_type* );

   api_bool       (SetNetworkTransferDownloadEventRoutine)( network_transfer_handle, api_handle, pcl::network_download_event_routine );
   api_bool       (SetNetworkTransferUploadEventRoutine)( network_transfer_handle, api_handle, pcl::network_upload_event_routine );
   api_bool       (SetNetworkTransferProgressEventRoutine)( network_transfer_handle, api_handle, pcl::network_progress_event_routine );
};

// ----------------------------------------------------------------------------

// End global namespace

// ----------------------------------------------------------------------------

namespace pcl
{

/*
 * Low-level C API interface
 */
struct APIInterface
{
   GlobalContext*                Global = nullptr;
   ModuleDefinitionContext*      ModuleDefinition = nullptr;
   ProcessDefinitionContext*     ProcessDefinition = nullptr;
   InterfaceDefinitionContext*   InterfaceDefinition = nullptr;
   FileFormatDefinitionContext*  FileFormatDefinition = nullptr;
   ModuleContext*                Module = nullptr;
   ProcessContext*               Process = nullptr;
   InterfaceContext*             Interface = nullptr;
   FileFormatContext*            FileFormat = nullptr;
   UIContext*                    UI = nullptr;
   ActionContext*                Action = nullptr;
   ControlContext*               Control = nullptr;
   DialogContext*                Dialog = nullptr;
   FrameContext*                 Frame = nullptr;
   GroupBoxContext*              GroupBox = nullptr;
   TabBoxContext*                TabBox = nullptr;
   ButtonContext*                Button = nullptr;
   EditContext*                  Edit = nullptr;
   TextBoxContext*               TextBox = nullptr;
   ComboBoxContext*              ComboBox = nullptr;
   SliderContext*                Slider = nullptr;
   SpinBoxContext*               SpinBox = nullptr;
   LabelContext*                 Label = nullptr;
   BitmapBoxContext*             BitmapBox = nullptr;
   ScrollBoxContext*             ScrollBox = nullptr;
   TreeBoxContext*               TreeBox = nullptr;
   TimerContext*                 Timer = nullptr;
   ThreadContext*                Thread = nullptr;
   MutexContext*                 Mutex = nullptr;
   ViewListContext*              ViewList = nullptr;
   BitmapContext*                Bitmap = nullptr;
   SVGContext*                   SVG = nullptr;
   BrushContext*                 Brush = nullptr;
   PenContext*                   Pen = nullptr;
   FontContext*                  Font = nullptr;
   CursorContext*                Cursor = nullptr;
   SizerContext*                 Sizer = nullptr;
   GraphicsContext*              Graphics = nullptr;
   RealTimePreviewContext*       RealTimePreview = nullptr;
   NumericalContext*             Numerical = nullptr;
   GPUContext*                   GPU = nullptr;
   SharedImageContext*           SharedImage = nullptr;
   ViewContext*                  View = nullptr;
   ImageWindowContext*           ImageWindow = nullptr;
   ImageViewContext*             ImageView = nullptr;
   CodeEditorContext*            CodeEditor = nullptr;
   WebViewContext*               WebView = nullptr;
   ExternalProcessContext*       ExternalProcess = nullptr;
   NetworkTransferContext*       NetworkTransfer = nullptr;

   // Automatically generated APIInterface.cpp
   APIInterface( function_resolver );
   ~APIInterface();

private:

   bool Resolve( function_resolver );
};

/*
 * Global PCL API Data
 */
extern PCL_DATA const pcl::APIInterface* API;

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

api_handle PCL_FUNC ModuleHandle();

#endif

} // pcl

// ----------------------------------------------------------------------------

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION

/*
 * Core API Function Resolver
 */
namespace pi
{
// Automatically generated APIFunctionResolver.cpp
extern "C" void* api_func APIFunctionResolver( const char* );
}

#endif

// ----------------------------------------------------------------------------

#endif   // __PCL_API_APIInterface_h

// ----------------------------------------------------------------------------
// EOF pcl/APIInterface.h - Released 2025-04-07T08:52:44Z
