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

extern "C"
{

// ----------------------------------------------------------------------------
// GlobalContext API
// ----------------------------------------------------------------------------

void API_Global_GetPixInsightVersion(uint32* major, uint32* minor, uint32* release, uint32* revision, uint32* betaRelease, uint32* confidentialRelease, uint32* leVersion, char* langCode); // ### Returns a string allocated by the caller module.
   char16_type*(API_Global_GetPixInsightCodename)( api_handle );

   /*
    * Error information
    */
   uint32      (API_Global_LastError)();
   void        (API_Global_ClearError)();
   api_bool    (API_Global_ErrorMessage)( uint32, char16_type*, size_type* );

   /*
    * Memory allocation
    */
   void*       (API_Global_Allocate)( size_type );
   api_bool    (API_Global_Deallocate)( void* );

   /*
    * Thread status functions
    */
   uint32      (API_Global_GetProcessStatus)();
   api_bool    (API_Global_ResetProcessStatus)();
   api_bool    (API_Global_EnableAbort)();
   api_bool    (API_Global_DisableAbort)();
   api_bool    (API_Global_Abort)();
   void        (API_Global_ProcessEvents)( api_bool excludeUserInputEvents );

   /*
    * Console functions
    */
   api_handle  (API_Global_GetThreadWindowId)(); // *** ### disabled function

   console_handle (API_Global_GetConsole)();
   api_bool    (API_Global_ValidateConsole)( const_console_handle );
   api_bool    (API_Global_WriteConsole)( console_handle, const char16_type*, api_bool appendNewline );
   int32       (API_Global_ReadConsoleChar)( console_handle ); // TODO - still not implemented

   // ### The following two functions return strings allocated by the caller module.
   char16_type*(API_Global_ReadConsoleString)( api_handle, console_handle ); // TODO - still not implemented
   char16_type*(API_Global_GetConsoleText)( api_handle, const_console_handle ); // ### security issues

   api_bool    (API_Global_FlushConsole)( console_handle );
   api_bool    (API_Global_ShowConsole)( console_handle, api_bool );
   api_bool    (API_Global_ExecuteCommand)( api_handle, console_handle, const char16_type* cmd );

   /*
    * Global cursor position
    */
   void        (API_Global_GetCursorPosition)( int32* x, int32* y );
   void        (API_Global_SetCursorPosition)( int32 x, int32 y );

   /*
    * Returns an OR combination of pcl::ModifierKey values corresponding to the
    * current state of supported modifier keys.
    */
   uint32      (API_Global_GetKeyboardModifiers)();

   /*
    * Tool tip window
    */
   void        (API_Global_ShowToolTipWindow)( int32 x, int32 y, const char16_type*,
                                              const_control_handle, int32, int32, int32, int32 );
   void        (API_Global_HideToolTipWindow)();
   api_bool    (API_Global_GetToolTipWindowText)( char16_type*, size_type* );

   /*
    * MessageBox functions
    */
   uint32      (API_Global_MessageBox)( const char16_type* text, const char16_type* caption, uint32 button0, uint32 button1, uint32 button2, uint32 defButton, uint32 escButton, uint32 icon );

   /*
    * Instance and interface launch functions
    */
   void        (API_Global_LaunchProcessInstance4)( meta_process_handle, const_process_handle, int32 mode, uint32 flags );
   void        (API_Global_LaunchProcessInstanceOnView)( meta_process_handle, const_process_handle, view_handle, uint32 flags );
   api_bool    (API_Global_LaunchProcessInterface)( meta_interface_handle, uint32 flags );

   /*
    * Readout options
    */
   void        (API_Global_GetReadoutOptions)( api_readout_options* options );
   void        (API_Global_SetReadoutOptions)( const api_readout_options* options );

   /*
    * Real-time preview
    * ### Obsolete -- preserved for compatibility -- see RealTimePreviewContext
    */
   api_bool    (API_Global_SetRealTimePreviewOwner)( interface_handle, uint32 flags );
   api_bool    (API_Global_IsRealTimePreviewUpdating)();
   void        (API_Global_UpdateRealTimePreview)();

   /*
    * Integrated documentation system
    */
   api_bool    (API_Global_BrowseProcessDocumentation)( meta_process_handle, uint32 flags );

   /*
    * Global settings
    */
   api_bool    (API_Global_GetGlobalFlag)( const char*, api_bool* );
   api_bool    (API_Global_GetGlobalInteger)( const char*, void*, api_bool isSigned );
   api_bool    (API_Global_GetGlobalReal)( const char*, double* );
   api_bool    (API_Global_GetGlobalColor)( const char*, uint32* );
   api_bool    (API_Global_GetGlobalFont)( const char*, char16_type*, size_type*, int32* sizePt );
   api_bool    (API_Global_GetGlobalString)( const char*, char16_type*, size_type* );

   api_bool    (API_Global_EnterGlobalSettingsUpdateContext)();
   api_bool    (API_Global_IsGlobalSettingsUpdateContextActive)();

   api_bool    (API_Global_SetGlobalFlag)( const char*, api_bool );
   api_bool    (API_Global_SetGlobalInteger)( const char*, uint32, api_bool isSigned );
   api_bool    (API_Global_SetGlobalReal)( const char*, double );
   api_bool    (API_Global_SetGlobalColor)( const char*, uint32 );
   api_bool    (API_Global_SetGlobalFont)( const char*, const char16_type*, int32 sizePt );
   api_bool    (API_Global_SetGlobalString)( const char*, const char16_type* );

   api_bool    (API_Global_CancelGlobalSettingsUpdate)( api_handle, uint32 reserved );
   api_bool    (API_Global_ExitGlobalSettingsUpdateContext)();

   /*
    * Module-defined settings
    */
   // ### The following two functions return data allocated by the caller module.
   api_bool    (API_Global_ReadSettingsBlock)( api_handle, void**, size_type*, const char* key, api_bool global );
   api_bool    (API_Global_ReadSettingsString)( api_handle, char16_type**, const char* key, api_bool global );

   api_bool    (API_Global_ReadSettingsFlag)( api_handle, api_bool*, const char* key, api_bool global );
   api_bool    (API_Global_ReadSettingsInteger)( api_handle, int32*, const char* key, api_bool global );
   api_bool    (API_Global_ReadSettingsUnsignedInteger)( api_handle, uint32*, const char* key, api_bool global );
   api_bool    (API_Global_ReadSettingsReal)( api_handle, double*, const char* key, api_bool global );

   api_bool    (API_Global_WriteSettingsBlock)( api_handle, const void*, size_type, const char* key, api_bool global );
   api_bool    (API_Global_WriteSettingsString)( api_handle, const char16_type*, const char* key, api_bool global );
   api_bool    (API_Global_WriteSettingsFlag)( api_handle, api_bool, const char* key, api_bool global );
   api_bool    (API_Global_WriteSettingsInteger)( api_handle, int32, const char* key, api_bool global );
   api_bool    (API_Global_WriteSettingsUnsignedInteger)( api_handle, uint32, const char* key, api_bool global );
   api_bool    (API_Global_WriteSettingsReal)( api_handle, double, const char* key, api_bool global );

   api_bool    (API_Global_DeleteSettingsItem)( api_handle, const char* key, api_bool global );

   uint32      (API_Global_GetSettingsItemGlobalAccess)( api_handle, const char* key );                // bit#0=W bit#1=R
   api_bool    (API_Global_SetSettingsItemGlobalAccess)( api_handle, const char* key, uint32 flags );  // bit#0=W bit#1=R

   /*
    * Miscellaneous message broadcasting
    */
   void        (API_Global_BroadcastImageUpdated)( const_view_handle, const void* /*reserved*/ );
   void        (API_Global_BroadcastGlobalFiltersUpdated)( const void* /*reserved*/ );

   /*
    * Miscellaneous color management
    */
   api_bool    (API_Global_GetProfilesDirectory)( int32, char16_type*, size_type* );

   /*
    * Access to the global PixelTraits LUT
    */
   const ::api_pixtraits_lut* (API_Global_GetPixelTraitsLUT)( uint32 version ); // version must be zero

   /*
    * Fast module thread control (API_Global_since core version 1.8.8-7)
    */
   int32       (API_Global_MaxProcessorsAllowedForModule)( api_handle, uint32 flags/*unused*/ );

   /*
    * Instance slot of the running application in [1,256] (API_Global_since core version 1.8.9-2 build 1581)
    */
   int32       (API_Global_ApplicationInstanceSlot)( api_handle );

   /*
    * Application configuration directory (API_Global_since core version 1.8.9-2 build 1581)
    */
   api_bool    (API_Global_GetApplicationConfigurationDirectory)( char16_type*, size_type* );

// ----------------------------------------------------------------------------
// ModuleDefinitionContext API
// ----------------------------------------------------------------------------

void API_ModuleDefinition_EnterModuleDefinitionContext();
api_bool API_ModuleDefinition_IsModuleDefinitionContextActive();
void API_ModuleDefinition_SetModuleOnLoadRoutine(pcl::module_on_load_routine);
void API_ModuleDefinition_SetModuleOnUnloadRoutine(pcl::module_on_unload_routine);
void API_ModuleDefinition_SetModuleAllocationRoutine(pcl::module_allocation_routine);
void API_ModuleDefinition_SetModuleDeallocationRoutine(pcl::module_deallocation_routine);
void API_ModuleDefinition_ExitModuleDefinitionContext();

// ----------------------------------------------------------------------------
// ProcessDefinitionContext API
// ----------------------------------------------------------------------------

void API_ProcessDefinition_EnterProcessDefinitionContext();
api_bool API_ProcessDefinition_IsProcessDefinitionContextActive();
void API_ProcessDefinition_BeginProcessDefinition(meta_process_handle, const char* procId);
api_bool API_ProcessDefinition_GetProcessBeingDefined(char*, size_type*);
void API_ProcessDefinition_SetProcessCategory(const char*);
void API_ProcessDefinition_SetProcessVersion(uint32);
void API_ProcessDefinition_SetProcessAliasIdentifiers(const char*);
void API_ProcessDefinition_SetProcessDescription(const char16_type*);
void API_ProcessDefinition_SetProcessScriptComment(const char16_type*);
void API_ProcessDefinition_SetProcessIconSVG(const char*);
void API_ProcessDefinition_SetProcessIconSVGFile(const char16_type*);
void API_ProcessDefinition_SetProcessIconImage(const char**); // ### deprecated
   void        (API_ProcessDefinition_SetProcessIconImageFile)( const char16_type* ); // ### deprecated
   void        (API_ProcessDefinition_SetProcessIconSmallImage)( const char** ); // ### deprecated
   void        (API_ProcessDefinition_SetProcessIconSmallImageFile)( const char16_type* ); // ### deprecated

   void        (API_ProcessDefinition_SetProcessClassInitializationRoutine)( pcl::process_class_initialization_routine );
   void        (API_ProcessDefinition_SetProcessCreationRoutine)( pcl::process_creation_routine );
   void        (API_ProcessDefinition_SetProcessDestructionRoutine)( pcl::process_destruction_routine );
   void        (API_ProcessDefinition_SetProcessClonationRoutine)( pcl::process_clonation_routine);
   void        (API_ProcessDefinition_SetProcessTestClonationRoutine)( pcl::process_test_clonation_routine );
   void        (API_ProcessDefinition_SetProcessSetServerHandleRoutine)( pcl::process_set_handle_routine );
   void        (API_ProcessDefinition_SetProcessAssignmentRoutine)( pcl::process_assignment_routine );
   void        (API_ProcessDefinition_SetProcessInitializationRoutine)( pcl::process_initialization_routine );
   void        (API_ProcessDefinition_SetProcessValidationRoutine)( pcl::process_validation_routine );

   void        (API_ProcessDefinition_SetProcessCommandLineProcessingRoutine)( pcl::process_command_line_processing_routine, uint32 flags );
   void        (API_ProcessDefinition_SetProcessEditPreferencesRoutine)( pcl::process_edit_preferences_routine );
   void        (API_ProcessDefinition_SetProcessBrowseDocumentationRoutine)( pcl::process_browse_documentation_routine );
   void        (API_ProcessDefinition_SetProcessExecutionPreferencesRoutine)( pcl::process_execution_preferences_routine );

   void        (API_ProcessDefinition_SetProcessExecutionValidationRoutine)( pcl::process_execution_validation_routine );
   void        (API_ProcessDefinition_SetProcessMaskValidationRoutine)( pcl::process_mask_validation_routine );
   void        (API_ProcessDefinition_SetProcessHistoryUpdateValidationRoutine)( pcl::process_history_update_validation_routine );
   void        (API_ProcessDefinition_SetProcessUndoModeRoutine)( pcl::process_undo_mode_routine );
   void        (API_ProcessDefinition_SetProcessPreExecutionRoutine)( pcl::process_pre_execution_routine );
   void        (API_ProcessDefinition_SetProcessExecutionRoutine)( pcl::process_execution_routine );
   void        (API_ProcessDefinition_SetProcessPostExecutionRoutine)( pcl::process_post_execution_routine );

   void        (API_ProcessDefinition_SetProcessGlobalExecutionValidationRoutine)( pcl::process_global_execution_validation_routine );
   void        (API_ProcessDefinition_SetProcessPreGlobalExecutionRoutine)( pcl::process_pre_global_execution_routine );
   void        (API_ProcessDefinition_SetProcessGlobalExecutionRoutine)( pcl::process_global_execution_routine );
   void        (API_ProcessDefinition_SetProcessPostGlobalExecutionRoutine)( pcl::process_post_global_execution_routine );

   void        (API_ProcessDefinition_SetProcessImageExecutionValidationRoutine)( pcl::process_image_execution_validation_routine );
   void        (API_ProcessDefinition_SetProcessImageExecutionRoutine)( pcl::process_image_execution_routine );

   void        (API_ProcessDefinition_SetProcessDefaultInterfaceSelectionRoutine)( pcl::process_default_interface_selection_routine );
   void        (API_ProcessDefinition_SetProcessInterfaceSelectionRoutine)( pcl::process_interface_selection_routine );
   void        (API_ProcessDefinition_SetProcessInterfaceValidationRoutine)( pcl::process_interface_validation_routine );

   void        (API_ProcessDefinition_SetProcessPreReadingRoutine)( pcl::process_pre_reading_routine );
   void        (API_ProcessDefinition_SetProcessPostReadingRoutine)( pcl::process_post_reading_routine );
   void        (API_ProcessDefinition_SetProcessPreWritingRoutine)( pcl::process_pre_writing_routine );
   void        (API_ProcessDefinition_SetProcessPostWritingRoutine)( pcl::process_post_writing_routine );

   void        (API_ProcessDefinition_SetProcessIPCStartRoutine)( pcl::process_ipc_notification_routine );
   void        (API_ProcessDefinition_SetProcessIPCStopRoutine)( pcl::process_ipc_notification_routine );
   void        (API_ProcessDefinition_SetProcessIPCSetParametersRoutine)( pcl::process_ipc_notification_routine );
   void        (API_ProcessDefinition_SetProcessIPCGetStatusRoutine)( pcl::process_ipc_status_routine );

   void        (API_ProcessDefinition_BeginParameterDefinition)( meta_parameter_handle, const char* parId, uint32 parType );
   api_bool    (API_ProcessDefinition_GetParameterBeingDefined)( char*, size_type* );
   void        (API_ProcessDefinition_SetParameterProcessVersionRange)( uint32, uint32 );
   void        (API_ProcessDefinition_SetParameterRequired)( api_bool );
   void        (API_ProcessDefinition_SetParameterReadOnly)( api_bool );
   void        (API_ProcessDefinition_SetParameterAliasIdentifiers)( const char* );
   void        (API_ProcessDefinition_SetParameterDescription)( const char16_type* );
   void        (API_ProcessDefinition_SetParameterScriptComment)( const char16_type* );
   void        (API_ProcessDefinition_SetParameterLockRoutine)( pcl::parameter_lock_routine );
   void        (API_ProcessDefinition_SetParameterUnlockRoutine)( pcl::parameter_unlock_routine );
   void        (API_ProcessDefinition_SetParameterValidationRoutine)( pcl::parameter_validation_routine );
   void        (API_ProcessDefinition_SetParameterAllocationRoutine)( pcl::parameter_allocation_routine );
   void        (API_ProcessDefinition_SetParameterLengthQueryRoutine)( pcl::parameter_length_query_routine );
   void        (API_ProcessDefinition_SetDefaultNumericValue)( double );
   void        (API_ProcessDefinition_SetValidNumericRange)( double, double );
   void        (API_ProcessDefinition_SetPrecision)( int32 );
   void        (API_ProcessDefinition_SetScientificNotation)( api_bool );
   void        (API_ProcessDefinition_SetDefaultBooleanValue)( api_bool );
   void        (API_ProcessDefinition_DefineEnumerationElement)( const char*, api_enum );
   void        (API_ProcessDefinition_DefineEnumerationAlias)( const char*, const char* );
   void        (API_ProcessDefinition_SetDefaultEnumerationValueIndex)( uint32 );
   void        (API_ProcessDefinition_SetDefaultStringValue)( const char16_type* );
   void        (API_ProcessDefinition_SetStringAllowedCharacters)( const char16_type* );
   void        (API_ProcessDefinition_SetStringLengthLimits)( size_type, size_type );
   void        (API_ProcessDefinition_BeginTableColumnDefinition)( meta_parameter_handle, const char* colId, uint32 colType );
   void        (API_ProcessDefinition_EndTableColumnDefinition)();
   void        (API_ProcessDefinition_SetTableRowLimits)( size_type, size_type );
   void        (API_ProcessDefinition_SetBlockSizeLimits)( size_type, size_type );

   void        (API_ProcessDefinition_EndParameterDefinition)();
   void        (API_ProcessDefinition_EndProcessDefinition)();
   void        (API_ProcessDefinition_ExitProcessDefinitionContext)();

// ----------------------------------------------------------------------------
// InterfaceDefinitionContext API
// ----------------------------------------------------------------------------

void API_InterfaceDefinition_EnterInterfaceDefinitionContext();
api_bool API_InterfaceDefinition_IsInterfaceDefinitionContextActive();
void API_InterfaceDefinition_BeginInterfaceDefinition(meta_interface_handle, const char* ifaceId, uint32 flags);
api_bool API_InterfaceDefinition_GetInterfaceBeingDefined(char*, size_type*);
void API_InterfaceDefinition_SetInterfaceVersion(uint32);
void API_InterfaceDefinition_SetInterfaceAliasIdentifiers(const char*);
void API_InterfaceDefinition_SetInterfaceDescription(const char16_type*);
void API_InterfaceDefinition_SetInterfaceIconSVG(const char*);
void API_InterfaceDefinition_SetInterfaceIconSVGFile(const char16_type*);
void API_InterfaceDefinition_SetInterfaceIconImage(const char**); // ### deprecated
   void           (API_InterfaceDefinition_SetInterfaceIconImageFile)( const char16_type* ); // ### deprecated
   void           (API_InterfaceDefinition_SetInterfaceIconSmallImage)( const char** ); // ### deprecated
   void           (API_InterfaceDefinition_SetInterfaceIconSmallImageFile)( const char16_type* ); // ### deprecated

   void           (API_InterfaceDefinition_SetInterfaceFeatures)( uint32, uint32 );

   void           (API_InterfaceDefinition_SetInterfaceInitializationRoutine)( pcl::interface_initialization_routine );
   void           (API_InterfaceDefinition_SetInterfaceLaunchRoutine)( pcl::interface_launch_routine );
   void           (API_InterfaceDefinition_SetInterfaceProcessInstantiationRoutine)( pcl::interface_process_instantiation_routine );
   void           (API_InterfaceDefinition_SetInterfaceProcessTestInstantiationRoutine)( pcl::interface_process_instantiation_routine );
   void           (API_InterfaceDefinition_SetInterfaceProcessValidationRoutine)( pcl::interface_process_validation_routine );
   void           (API_InterfaceDefinition_SetInterfaceProcessImportRoutine)( pcl::interface_process_import_routine );

   void           (API_InterfaceDefinition_SetInterfaceApplyRoutine)( pcl::interface_control_routine );
   void           (API_InterfaceDefinition_SetInterfaceApplyGlobalRoutine)( pcl::interface_control_routine );
   void           (API_InterfaceDefinition_SetInterfaceRealTimePreviewUpdatedRoutine)( pcl::interface_control_state_routine );
   void           (API_InterfaceDefinition_SetInterfaceExecuteRoutine)( pcl::interface_control_routine );
   void           (API_InterfaceDefinition_SetInterfaceCancelRoutine)( pcl::interface_control_routine );
   void           (API_InterfaceDefinition_SetInterfaceBrowseDocumentationRoutine)( pcl::interface_control_routine );
   void           (API_InterfaceDefinition_SetInterfaceTrackViewUpdatedRoutine)( pcl::interface_control_state_routine );
   void           (API_InterfaceDefinition_SetInterfaceEditPreferencesRoutine)( pcl::interface_control_routine );
   void           (API_InterfaceDefinition_SetInterfaceResetRoutine)( pcl::interface_control_routine );

   void           (API_InterfaceDefinition_SetInterfaceRealTimeUpdateQueryRoutine)( pcl::interface_real_time_update_query_routine );
   void           (API_InterfaceDefinition_SetInterfaceRealTimeGenerationFlagsRoutine)( pcl::interface_real_time_generation_flags_routine );
   void           (API_InterfaceDefinition_SetInterfaceRealTimeGenerationRoutine)( pcl::interface_real_time_generation_routine );
   void           (API_InterfaceDefinition_SetInterfaceRealTimeCancelRoutine)( pcl::interface_real_time_cancel_routine );

   void           (API_InterfaceDefinition_SetInterfaceDynamicModeEnterRoutine)( pcl::interface_dynamic_mode_enter_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicModeExitRoutine)( pcl::interface_dynamic_mode_exit_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicMouseEnterRoutine)( pcl::interface_dynamic_view_event_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicMouseLeaveRoutine)( pcl::interface_dynamic_view_event_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicMouseMoveRoutine)( pcl::interface_dynamic_mouse_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicMousePressRoutine)( pcl::interface_dynamic_mouse_button_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicMouseReleaseRoutine)( pcl::interface_dynamic_mouse_button_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicMouseDoubleClickRoutine)( pcl::interface_dynamic_mouse_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicKeyPressRoutine)( pcl::interface_dynamic_keyboard_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicKeyReleaseRoutine)( pcl::interface_dynamic_keyboard_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicMouseWheelRoutine)( pcl::interface_dynamic_wheel_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicUpdateQueryRoutine)( pcl::interface_dynamic_update_query_routine );
   void           (API_InterfaceDefinition_SetInterfaceDynamicPaintRoutine)( pcl::interface_dynamic_paint_routine );

   void           (API_InterfaceDefinition_SetImageCreatedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageRenamedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageDeletedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageFocusedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageLockedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageUnlockedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageSTFEnabledNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageSTFDisabledNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageSTFUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageRGBWSUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageCMEnabledNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageCMDisabledNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageCMUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetImageSavedNotificationRoutine)( pcl::image_notification_routine );

   void           (API_InterfaceDefinition_SetMaskUpdatedNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetMaskEnabledNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetMaskDisabledNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetMaskShownNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetMaskHiddenNotificationRoutine)( pcl::image_notification_routine );

   void           (API_InterfaceDefinition_SetTransparencyHiddenNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetTransparencyModeUpdatedNotificationRoutine)( pcl::image_notification_routine );

   void           (API_InterfaceDefinition_SetViewPropertyUpdatedNotificationRoutine)( pcl::view_property_notification_routine );
   void           (API_InterfaceDefinition_SetViewPropertyDeletedNotificationRoutine)( pcl::view_property_notification_routine );

   void           (API_InterfaceDefinition_SetBeginReadoutNotificationRoutine)( pcl::image_notification_routine );
   void           (API_InterfaceDefinition_SetUpdateReadoutNotificationRoutine)( pcl::readout_notification_routine );
   void           (API_InterfaceDefinition_SetEndReadoutNotificationRoutine)( pcl::image_notification_routine );

   void           (API_InterfaceDefinition_SetProcessCreatedNotificationRoutine)( pcl::process_notification_routine );
   void           (API_InterfaceDefinition_SetProcessUpdatedNotificationRoutine)( pcl::process_notification_routine );
   void           (API_InterfaceDefinition_SetProcessDeletedNotificationRoutine)( pcl::process_notification_routine );
   void           (API_InterfaceDefinition_SetProcessSavedNotificationRoutine)( pcl::process_notification_routine );

   void           (API_InterfaceDefinition_SetRealTimePreviewOwnerChangeNotificationRoutine)( pcl::interface_notification_routine );
   void           (API_InterfaceDefinition_SetRealTimePreviewLUTUpdatedNotificationRoutine)( pcl::lut_notification_routine );
   void           (API_InterfaceDefinition_SetRealTimePreviewGenerationStartNotificationRoutine)( pcl::global_notification_routine );
   void           (API_InterfaceDefinition_SetRealTimePreviewGenerationFinishNotificationRoutine)( pcl::global_notification_routine );

   void           (API_InterfaceDefinition_SetGlobalRGBWSUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (API_InterfaceDefinition_SetGlobalCMEnabledNotificationRoutine)( pcl::global_notification_routine );
   void           (API_InterfaceDefinition_SetGlobalCMDisabledNotificationRoutine)( pcl::global_notification_routine );
   void           (API_InterfaceDefinition_SetGlobalCMUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (API_InterfaceDefinition_SetReadoutOptionsUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (API_InterfaceDefinition_SetGlobalPreferencesUpdatedNotificationRoutine)( pcl::global_notification_routine );
   void           (API_InterfaceDefinition_SetGlobalFiltersUpdatedNotificationRoutine)( pcl::global_notification_routine );

   void           (API_InterfaceDefinition_EndInterfaceDefinition)();
   void           (API_InterfaceDefinition_ExitInterfaceDefinitionContext)();

// ----------------------------------------------------------------------------
// FileFormatDefinitionContext API
// ----------------------------------------------------------------------------

void API_FileFormatDefinition_EnterFileFormatDefinitionContext();
api_bool API_FileFormatDefinition_IsFileFormatDefinitionContextActive();
void API_FileFormatDefinition_BeginFileFormatDefinition(meta_format_handle, const char* fmtName, const char16_type** fmtExtensions, const char** fmtMimeTypes);
api_bool API_FileFormatDefinition_GetFileFormatBeingDefined(char*, size_type*);
void API_FileFormatDefinition_SetFileFormatVersion(uint32);
void API_FileFormatDefinition_SetFileFormatDescription(const char16_type*);
void API_FileFormatDefinition_SetFileFormatImplementation(const char16_type*);
void API_FileFormatDefinition_SetFileFormatIconSVG(const char*);
void API_FileFormatDefinition_SetFileFormatIconSVGFile(const char16_type*);
void API_FileFormatDefinition_SetFileFormatIconImage(const char**); // ### deprecated
   void           (API_FileFormatDefinition_SetFileFormatIconImageFile)( const char16_type* ); // ### deprecated
   void           (API_FileFormatDefinition_SetFileFormatIconSmallImage)( const char** ); // ### deprecated
   void           (API_FileFormatDefinition_SetFileFormatIconSmallImageFile)( const char16_type* ); // ### deprecated
   void           (API_FileFormatDefinition_SetFileFormatCaps)( const api_format_capabilities* );

   void           (API_FileFormatDefinition_SetFileFormatCreationRoutine)( pcl::format_creation_routine );
   void           (API_FileFormatDefinition_SetFileFormatDestructionRoutine)( pcl::format_destruction_routine );
   void           (API_FileFormatDefinition_SetFileFormatValidateFormatSpecificDataRoutine)( pcl::format_validate_format_specific_data_routine );
   void           (API_FileFormatDefinition_SetFileFormatDisposeFormatSpecificDataRoutine)( pcl::format_dispose_format_specific_data_routine );
   void           (API_FileFormatDefinition_SetFileFormatEditPreferencesRoutine)( pcl::format_edit_preferences_routine );

   void           (API_FileFormatDefinition_SetFileFormatOpenRoutine)( pcl::format_open_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetImageCountRoutine)( pcl::format_get_image_count_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetImageIdRoutine)( pcl::format_get_image_id_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetImageDescriptionRoutine)( pcl::format_get_image_description_routine );
   void           (API_FileFormatDefinition_SetFileFormatCloseRoutine)( pcl::format_close_routine );
   void           (API_FileFormatDefinition_SetFileFormatIsOpenRoutine)( pcl::format_is_open_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetFilePathRoutine)( pcl::format_get_file_path_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetSelectedImageIndexRoutine)( pcl::format_set_selected_image_index_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetSelectedImageIndexRoutine)( pcl::format_get_selected_image_index_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetFormatSpecificDataRoutine)( pcl::format_set_format_specific_data_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetFormatSpecificDataRoutine)( pcl::format_get_format_specific_data_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetImageFormatInfoRoutine)( pcl::format_get_image_format_info_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginKeywordExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetKeywordCountRoutine)( pcl::format_get_keyword_count_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetNextKeywordRoutine)( pcl::format_get_next_keyword_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndKeywordExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginICCProfileExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetICCProfileRoutine)( pcl::format_get_icc_profile_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndICCProfileExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginThumbnailExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetThumbnailRoutine)( pcl::format_get_thumbnail_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndThumbnailExtractionRoutine)( pcl::format_end_extraction_routine );

   void           (API_FileFormatDefinition_SetFileFormatEnumerateImagePropertiesRoutine)( pcl::format_enumerate_image_properties_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginImagePropertyExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetImagePropertyRoutine)( pcl::format_get_image_property_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndImagePropertyExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginImagePropertyEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetImagePropertyRoutine)( pcl::format_set_image_property_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndImagePropertyEmbeddingRoutine)( pcl::format_end_embedding_routine );

   void           (API_FileFormatDefinition_SetFileFormatEnumeratePropertiesRoutine)( pcl::format_enumerate_image_properties_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginPropertyExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetPropertyRoutine)( pcl::format_get_image_property_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndPropertyExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginPropertyEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetPropertyRoutine)( pcl::format_set_image_property_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndPropertyEmbeddingRoutine)( pcl::format_end_embedding_routine );

   void           (API_FileFormatDefinition_SetFileFormatBeginRGBWSExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetImageRGBWSRoutine)( pcl::format_get_image_rgbws_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndRGBWSExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginRGBWSEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetImageRGBWSRoutine)( pcl::format_set_image_rgbws_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndRGBWSEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginDisplayFunctionExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetImageDisplayFunctionRoutine)( pcl::format_get_image_display_function_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndDisplayFunctionExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginDisplayFunctionEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetImageDisplayFunctionRoutine)( pcl::format_set_image_display_function_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndDisplayFunctionEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginColorFilterArrayExtractionRoutine)( pcl::format_begin_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatGetImageColorFilterArrayRoutine)( pcl::format_get_image_color_filter_array_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndColorFilterArrayExtractionRoutine)( pcl::format_end_extraction_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginColorFilterArrayEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetImageColorFilterArrayRoutine)( pcl::format_set_image_color_filter_array_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndColorFilterArrayEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatReadImageRoutine)( pcl::format_read_image_routine );
   void           (API_FileFormatDefinition_SetFileFormatAllowIncrementalReadRoutine)( pcl::format_allow_incremental_op_routine );
   void           (API_FileFormatDefinition_SetFileFormatReadSamplesRoutine)( pcl::format_read_pixels_routine );
   void           (API_FileFormatDefinition_SetFileFormatQueryOptionsRoutine)( pcl::format_query_options_routine );
   void           (API_FileFormatDefinition_SetFileFormatCreateRoutine)( pcl::format_create_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetImageIdRoutine)( pcl::format_set_image_id_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetImageOptionsRoutine)( pcl::format_set_image_options_routine );
   void           (API_FileFormatDefinition_SetFileFormatCreateImageRoutine)( pcl::format_create_image_routine );
   void           (API_FileFormatDefinition_SetFileFormatCloseImageRoutine)( pcl::format_close_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginKeywordEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatAddKeywordRoutine)( pcl::format_add_keyword_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndKeywordEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginICCProfileEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetICCProfileRoutine)( pcl::format_set_icc_profile_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndICCProfileEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatBeginThumbnailEmbeddingRoutine)( pcl::format_begin_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatSetThumbnailRoutine)( pcl::format_set_thumbnail_routine );
   void           (API_FileFormatDefinition_SetFileFormatEndThumbnailEmbeddingRoutine)( pcl::format_end_embedding_routine );
   void           (API_FileFormatDefinition_SetFileFormatWriteImageRoutine)( pcl::format_write_image_routine );
   void           (API_FileFormatDefinition_SetFileFormatAllowIncrementalWriteRoutine)( pcl::format_allow_incremental_op_routine );
   void           (API_FileFormatDefinition_SetFileFormatWriteSamplesRoutine)( pcl::format_write_pixels_routine );
   void           (API_FileFormatDefinition_SetFileFormatQueryInexactReadRoutine)( pcl::format_query_inexact_read_routine );
   void           (API_FileFormatDefinition_SetFileFormatQueryLossyWriteRoutine)( pcl::format_query_lossy_write_routine );
   void           (API_FileFormatDefinition_SetFileFormatQueryFormatStatusRoutine)( pcl::format_query_format_status_routine );
   void           (API_FileFormatDefinition_EndFileFormatDefinition)();

   void           (API_FileFormatDefinition_ExitFileFormatDefinitionContext)();

// ----------------------------------------------------------------------------
// ModuleContext API
// ----------------------------------------------------------------------------

api_bool API_Module_LoadResource(api_handle, const char16_type*, const char16_type*);
api_bool API_Module_UnloadResource(api_handle, const char16_type*, const char16_type*);
api_bool API_Module_EvaluateScript(api_handle, api_property_value* result, const char16_type* sourceCode, const char* language);
api_bool API_Module_HasEntitlement(api_handle, const char*);

// ----------------------------------------------------------------------------
// ProcessContext API
// ----------------------------------------------------------------------------

api_bool API_Process_EnumerateProcessCategories(pcl::category_enumeration_callback, char*, size_type*, void*);
api_bool API_Process_EnumerateProcesses(pcl::process_enumeration_callback, void*);
meta_process_handle API_Process_GetProcessByName(api_handle, const char* id);
api_bool API_Process_GetProcessIdentifier(meta_process_handle, char*, size_type*);
api_bool API_Process_GetProcessCategory(meta_process_handle, char*, size_type*);
uint32 API_Process_GetProcessVersion(meta_process_handle);
api_bool API_Process_GetProcessAliasIdentifiers(meta_process_handle, char*, size_type*);
api_bool API_Process_GetProcessDescription(meta_process_handle, char16_type*, size_type*);
api_bool API_Process_GetProcessScriptComment(meta_process_handle, char16_type*, size_type*);
bitmap_handle API_Process_GetProcessIcon(meta_process_handle);
bitmap_handle API_Process_GetProcessSmallIcon(meta_process_handle);
api_bool API_Process_GetProcessProperties(meta_process_handle, api_process_properties*);
interface_handle API_Process_GetProcessDefaultInterface(meta_process_handle);
api_bool API_Process_EditProcessPreferences(meta_process_handle);
api_bool API_Process_BrowseProcessDocumentation(meta_process_handle, uint32 flags);
int32 API_Process_RunProcessCommandLine(meta_process_handle, const char16_type*);
api_bool API_Process_LaunchProcess(meta_process_handle);
api_bool API_Process_EnumerateProcessParameters(meta_process_handle, pcl::parameter_enumeration_callback, void*);
api_bool API_Process_EnumerateTableColumns(meta_parameter_handle, pcl::parameter_enumeration_callback, void*);
meta_parameter_handle API_Process_GetParameterByName(meta_process_handle, const char* id);
meta_parameter_handle API_Process_GetTableColumnByName(meta_parameter_handle, const char* id);
meta_process_handle API_Process_GetParameterProcess(meta_parameter_handle);
meta_parameter_handle API_Process_GetParameterTable(meta_parameter_handle);
uint32 API_Process_GetParameterType(meta_parameter_handle);
api_bool API_Process_GetParameterIdentifier(meta_parameter_handle, char*, size_type*);
api_bool API_Process_GetParameterAliasIdentifiers(meta_parameter_handle, char*, size_type*);
api_bool API_Process_GetParameterDescription(meta_parameter_handle, char16_type*, size_type*);
api_bool API_Process_GetParameterScriptComment(meta_parameter_handle, char16_type*, size_type*);
api_bool API_Process_GetParameterRequired(meta_parameter_handle);
api_bool API_Process_GetParameterReadOnly(meta_parameter_handle); // Boolean, numeric and string parameter types
   api_bool             (API_Process_GetParameterDefaultValue)( meta_parameter_handle, void* value, size_type* length );

   // Enumerated parameters
   size_type            (API_Process_GetParameterElementCount)( meta_parameter_handle );
   api_bool             (API_Process_GetParameterElementIdentifier)( meta_parameter_handle, size_type index, char*, size_type* );
   api_bool             (API_Process_GetParameterElementAliasIdentifiers)( meta_parameter_handle, size_type index, char*, size_type* );
   api_enum             (API_Process_GetParameterElementValue)( meta_parameter_handle, size_type index );
   int32                (API_Process_GetParameterDefaultElementIndex)( meta_parameter_handle );

   // Numeric parameters
   api_bool             (API_Process_GetParameterRange)( meta_parameter_handle, double* minValue, double* maxValue );
   int32                (API_Process_GetParameterPrecision)( meta_parameter_handle );
   api_bool             (API_Process_GetParameterScientificNotation)( meta_parameter_handle );

   // Variable length parameters
   api_bool             (API_Process_GetParameterLengthLimits)( meta_parameter_handle, size_type* minLength, size_type* maxLength );

   // String parameters
   api_bool             (API_Process_GetParameterAllowedCharacters)( meta_parameter_handle, char16_type*, size_type* );

   process_handle       (API_Process_CreateProcessInstance)( api_handle, meta_process_handle );

   meta_process_handle  (API_Process_GetProcessInstanceProcess)( const_process_handle );

   uint32               (API_Process_GetProcessInstanceVersion)( const_process_handle );

   process_handle       (API_Process_CloneProcessInstance)( api_handle, const_process_handle, uint32 flags );
   api_bool             (API_Process_AssignProcessInstance)( process_handle, const_process_handle, uint32 flags );
   api_bool             (API_Process_ValidateProcessInstance)( process_handle, char16_type*, size_type );

   api_bool             (API_Process_GetUpdatesViewHistory)( const_process_handle, const_view_handle );
   api_bool             (API_Process_ValidateViewExecutionMask)( const_process_handle, const_view_handle, const_window_handle );
   api_bool             (API_Process_ValidateViewExecution)( const_process_handle, const_view_handle, char16_type*, size_type );
   api_bool             (API_Process_ExecuteOnView)( process_handle, view_handle, uint32 flags );

   api_bool             (API_Process_ValidateGlobalExecution)( const_process_handle, char16_type*, size_type );
   api_bool             (API_Process_ExecuteGlobal)( process_handle, uint32 flags );

   api_bool             (API_Process_ValidateImageExecution)( const_process_handle, const_image_handle, char16_type*, size_type );
   api_bool             (API_Process_ExecuteOnImage)( process_handle, image_handle, const char*, uint32 flags );

   api_bool             (API_Process_LaunchProcessInstance1)( process_handle );

   api_bool             (API_Process_ValidateInterfaceLaunch)( const_process_handle );
   api_bool             (API_Process_LaunchInterface)( process_handle );

   // ### TODO: The following two functions are not yet implemented.
   api_bool             (API_Process_ValidateInterface)( const_process_handle, const_interface_handle, char16_type*, size_type );
   interface_handle     (API_Process_GetInterface)( const_process_handle );

   api_bool             (API_Process_GetProcessInstanceDescription)( const_process_handle, char16_type*, size_type* );
   api_bool             (API_Process_SetProcessInstanceDescription)( process_handle, const char16_type* );

   api_bool             (API_Process_GetExecutionTimes)( const_process_handle, double* startJD, double* elapsedSecs );

   // ### Returns a string allocated by the caller module.
   char16_type*         (API_Process_GetProcessInstanceSourceCode)( api_handle, const_process_handle, const char* language, const char* varId, uint32 indent );
   process_handle       (API_Process_CreateProcessInstanceFromSourceCode)( const char16_type* source, const char* language );

   process_handle       (API_Process_CreateProcessInstanceFromIcon)( api_handle, const char* iconId );

   api_bool             (API_Process_EnumerateProcessIcons)( pcl::icon_enumeration_callback, char*, size_type*, void* );

   api_bool             (API_Process_GetParameterValue)( const_process_handle, meta_parameter_handle, size_type tableRow, uint32* parType, void* value, size_type* length );
   api_bool             (API_Process_SetParameterValue)( process_handle, meta_parameter_handle, size_type tableRow, const void* value, size_type length );

   size_type            (API_Process_GetTableRowCount)( const_process_handle, meta_parameter_handle );

   api_bool             (API_Process_AllocateTableRows)( process_handle, meta_parameter_handle, size_type rowCount );

// ----------------------------------------------------------------------------
// InterfaceContext API
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// FileFormatContext API
// ----------------------------------------------------------------------------

api_bool API_FileFormat_EnumerateFileFormats(pcl::format_enumeration_callback, void*);
meta_format_handle API_FileFormat_GetFileFormatByName(api_handle, const char* id);
meta_format_handle API_FileFormat_GetFileFormatByFileExtension(api_handle, const char16_type* ext, api_bool toRead, api_bool toWrite);
meta_format_handle API_FileFormat_GetFileFormatByMimeType(api_handle, const char* mimeType, api_bool toRead, api_bool toWrite);
api_bool API_FileFormat_GetFileFormatName(meta_format_handle, char*, size_type*);
api_bool API_FileFormat_GetFileFormatFileExtensions(meta_format_handle, char16_type**, size_type* extCount, size_type* maxExtLen);
api_bool API_FileFormat_GetFileFormatMimeTypes(meta_format_handle, char**, size_type* mimeCount, size_type* maxMimeLen);
uint32 API_FileFormat_GetFileFormatVersion(meta_format_handle);
api_bool API_FileFormat_GetFileFormatDescription(meta_format_handle, char16_type*, size_type*);
api_bool API_FileFormat_GetFileFormatImplementation(meta_format_handle, char16_type*, size_type*);
bitmap_handle API_FileFormat_GetFileFormatIcon(meta_format_handle);
bitmap_handle API_FileFormat_GetFileFormatSmallIcon(meta_format_handle);
api_bool API_FileFormat_GetFileFormatCapabilities(meta_format_handle, api_format_capabilities*);
api_bool API_FileFormat_GetFileFormatStatus(meta_format_handle, char16_type*, size_type*, void*);
api_bool API_FileFormat_EditFileFormatPreferences(meta_format_handle);
file_format_handle API_FileFormat_CreateFileFormatInstance(api_handle, meta_format_handle);
meta_format_handle API_FileFormat_GetFileFormatInstanceFormat(const_file_format_handle);
api_bool API_FileFormat_CloseImageFile(file_format_handle);
api_bool API_FileFormat_IsImageFileOpen(const_file_format_handle);
api_bool API_FileFormat_GetImageFilePath(file_format_handle, char16_type*, size_type*);
api_bool API_FileFormat_OpenImageFile(file_format_handle, const char16_type*);
api_bool API_FileFormat_OpenImageFileEx(file_format_handle, const char16_type*, const char*, uint32);
uint32 API_FileFormat_GetImageCount(const_file_format_handle);
api_bool API_FileFormat_GetImageId(const_file_format_handle, char*, size_type*, uint32);
api_bool API_FileFormat_GetImageDescription(const_file_format_handle, api_image_info*, api_image_options*, uint32);
api_bool API_FileFormat_SelectImage(file_format_handle, uint32);
uint32 API_FileFormat_GetSelectedImageIndex(const_file_format_handle);
void *API_FileFormat_GetFormatSpecificData(file_format_handle);
api_bool API_FileFormat_SetFormatSpecificData(file_format_handle, const void*);
api_bool API_FileFormat_ValidateFormatSpecificData(meta_format_handle, const void*);
void API_FileFormat_DisposeFormatSpecificData(meta_format_handle, const void*);
api_bool API_FileFormat_GetImageFormatInfo(const_file_format_handle, char16_type*, size_type*);
api_bool API_FileFormat_BeginKeywordExtraction(file_format_handle);
size_type API_FileFormat_GetKeywordCount(file_format_handle);
api_bool API_FileFormat_GetNextKeyword(file_format_handle, char*, char*, char*, uint32);
void API_FileFormat_EndKeywordExtraction(file_format_handle);
api_bool API_FileFormat_BeginICCProfileExtraction(file_format_handle);
void API_FileFormat_EndICCProfileExtraction(file_format_handle);
api_bool API_FileFormat_BeginThumbnailExtraction(file_format_handle);
api_bool API_FileFormat_GetThumbnail(file_format_handle, image_handle);
void API_FileFormat_EndThumbnailExtraction(file_format_handle);
api_bool API_FileFormat_EnumerateProperties(file_format_handle, pcl::property_enumeration_callback, char*, size_type*, void*);
api_bool API_FileFormat_BeginPropertyExtraction(file_format_handle);
api_bool API_FileFormat_GetProperty(file_format_handle, const char* id, api_property_value*);
void API_FileFormat_EndPropertyExtraction(file_format_handle);
api_bool API_FileFormat_BeginPropertyEmbedding(file_format_handle);
api_bool API_FileFormat_SetProperty(file_format_handle, const char* id, const api_property_value*);
void API_FileFormat_EndPropertyEmbedding(file_format_handle);
api_bool API_FileFormat_EnumerateImageProperties(file_format_handle, pcl::property_enumeration_callback, char*, size_type*, void*);
api_bool API_FileFormat_BeginImagePropertyExtraction(file_format_handle);
api_bool API_FileFormat_GetImageProperty(file_format_handle, const char* id, api_property_value*);
void API_FileFormat_EndImagePropertyExtraction(file_format_handle);
api_bool API_FileFormat_BeginImagePropertyEmbedding(file_format_handle);
api_bool API_FileFormat_SetImageProperty(file_format_handle, const char* id, const api_property_value*);
void API_FileFormat_EndImagePropertyEmbedding(file_format_handle);
api_bool API_FileFormat_BeginRGBWSExtraction(file_format_handle);
api_bool API_FileFormat_GetImageRGBWS(file_format_handle, float*, api_bool*, float*, float*, float*);
void API_FileFormat_EndRGBWSExtraction(file_format_handle);
api_bool API_FileFormat_BeginRGBWSEmbedding(file_format_handle);
api_bool API_FileFormat_SetImageRGBWS(file_format_handle, float, api_bool, const float*, const float*, const float*);
void API_FileFormat_EndRGBWSEmbedding(file_format_handle);
api_bool API_FileFormat_BeginDisplayFunctionExtraction(file_format_handle);
api_bool API_FileFormat_GetImageDisplayFunction(file_format_handle, double*, double*, double*, double*, double*);
void API_FileFormat_EndDisplayFunctionExtraction(file_format_handle);
api_bool API_FileFormat_BeginDisplayFunctionEmbedding(file_format_handle);
api_bool API_FileFormat_SetImageDisplayFunction(file_format_handle, const double*, const double*, const double*, const double*, const double*);
void API_FileFormat_EndDisplayFunctionEmbedding(file_format_handle);
api_bool API_FileFormat_BeginColorFilterArrayExtraction(file_format_handle);
api_bool API_FileFormat_GetImageColorFilterArray(file_format_handle, char*, size_type*, int32*, int32*, char16_type*, size_type*);
void API_FileFormat_EndColorFilterArrayExtraction(file_format_handle);
api_bool API_FileFormat_BeginColorFilterArrayEmbedding(file_format_handle);
api_bool API_FileFormat_SetImageColorFilterArray(file_format_handle, const char*, int32, int32, const char16_type*);
void API_FileFormat_EndColorFilterArrayEmbedding(file_format_handle);
api_bool API_FileFormat_ReadImage(file_format_handle, image_handle);
api_bool API_FileFormat_CanReadIncrementally(const_file_format_handle);
api_bool API_FileFormat_ReadSamples(file_format_handle, void*, uint32, uint32, uint32, uint32, api_bool, api_bool);
api_bool API_FileFormat_QueryImageFileOptions(file_format_handle, api_image_options*, const void**, uint32);
api_bool API_FileFormat_CreateImageFile(file_format_handle, const char16_type*, uint32);
api_bool API_FileFormat_CreateImageFileEx(file_format_handle, const char16_type*, uint32, const char*, uint32);
api_bool API_FileFormat_SetImageId(file_format_handle, const char*);
api_bool API_FileFormat_SetImageOptions(file_format_handle, const api_image_options*);
api_bool API_FileFormat_BeginKeywordEmbedding(file_format_handle);
api_bool API_FileFormat_AddKeyword(file_format_handle, const char*, const char*, const char*);
void API_FileFormat_EndKeywordEmbedding(file_format_handle);
api_bool API_FileFormat_BeginICCProfileEmbedding(file_format_handle);
api_bool API_FileFormat_SetICCProfile(file_format_handle, const void*);
void *API_FileFormat_GetICCProfile(file_format_handle);
void API_FileFormat_EndICCProfileEmbedding(file_format_handle);
api_bool API_FileFormat_BeginThumbnailEmbedding(file_format_handle);
api_bool API_FileFormat_SetThumbnail(file_format_handle, const_image_handle);
void API_FileFormat_EndThumbnailEmbedding(file_format_handle);
api_bool API_FileFormat_WriteImage(file_format_handle, const_image_handle);
api_bool API_FileFormat_CanWriteIncrementally(const_file_format_handle);
api_bool API_FileFormat_CreateImage(file_format_handle, const api_image_info*);
api_bool API_FileFormat_WriteSamples(file_format_handle, const void*, uint32, uint32, uint32, uint32, api_bool, api_bool);
api_bool API_FileFormat_CloseImage(file_format_handle);
api_bool API_FileFormat_WasInexactRead(const_file_format_handle);
api_bool API_FileFormat_WasLossyWrite(const_file_format_handle);

// ----------------------------------------------------------------------------
// UIContext API
// ----------------------------------------------------------------------------

api_bool API_UI_AttachToUIObject(api_handle, api_handle);
api_bool API_UI_DetachFromUIObject(api_handle, api_handle);
api_handle API_UI_GetUIObjectModule(const_api_handle);
size_type API_UI_GetUIObjectRefCount(const_api_handle);
api_bool API_UI_GetUIObjectType(const_api_handle, char*, size_type*);
api_bool API_UI_GetUIObjectId(const_api_handle, char16_type*, size_type*);
api_bool API_UI_SetUIObjectId(api_handle, const char16_type*);
api_bool API_UI_SetHandleDestroyedEventRoutine(api_handle, pcl::destroy_event_routine);

// ----------------------------------------------------------------------------
// ActionContext API
// ----------------------------------------------------------------------------

action_handle API_Action_CreateActionSVG(api_handle, api_handle client, const char16_type* menuItem, const char16_type* toolBar, const char* svgIcon, uint32 flags);
action_handle API_Action_CreateActionSVGFile(api_handle, api_handle client, const char16_type* menuItem, const char16_type* toolBar, const char16_type* svgIconPath, uint32 flags); // ### deprecated
   action_handle  (API_Action_CreateAction)( api_handle, api_handle client,
                                            const char16_type* menuItem, const char16_type* toolBar,
                                            const_bitmap_handle icon,
                                            uint32 flags );

   api_bool       (API_Action_GetActionMenuItem)( const_action_handle, char16_type*, size_type* );

   api_bool       (API_Action_GetActionMenuText)( const_action_handle, char16_type*, size_type* );
   void           (API_Action_SetActionMenuText)( action_handle, const char16_type* );

   api_bool       (API_Action_GetActionToolBar)( const_action_handle, char16_type*, size_type* );

   api_bool       (API_Action_GetActionToolTip)( const_action_handle, char16_type*, size_type* );
   void           (API_Action_SetActionToolTip)( action_handle, const char16_type* );

   bitmap_handle  (API_Action_GetActionIcon)( const_action_handle );
   void           (API_Action_SetActionIconSVG)( action_handle, const char* );
   void           (API_Action_SetActionIconSVGFile)( action_handle, const char16_type* );
   // ### deprecated
   void           (API_Action_SetActionIcon)( action_handle, const_bitmap_handle );

   void           (API_Action_GetActionAccelerator)( const_action_handle, int32* keyModifiers, int32* keyCode );
   void           (API_Action_SetActionAccelerator)( action_handle, int32 keyModifiers, int32 keyCode );

   api_bool       (API_Action_SetActionExecutionRoutine)( action_handle, pcl::action_execution_routine );
   api_bool       (API_Action_SetActionStateQueryRoutine)( action_handle, pcl::action_state_query_routine );

// ----------------------------------------------------------------------------
// ControlContext API
// ----------------------------------------------------------------------------

control_handle API_Control_CreateControl(api_handle, api_handle client, control_handle parent, uint32 flags);
void API_Control_GetFrameRect(const_control_handle, int32*, int32*, int32*, int32*);
void API_Control_GetClientRect(const_control_handle, int32*, int32*, int32*, int32*);
void API_Control_SetClientRect(control_handle, int32, int32, int32, int32);
void API_Control_SetControlSize(control_handle, int32, int32);
void API_Control_AdjustControlToContents(control_handle);
void API_Control_GetControlMinSize(const_control_handle, int32*, int32*);
void API_Control_SetControlMinSize(control_handle, int32, int32);
void API_Control_GetControlMaxSize(const_control_handle, int32*, int32*);
void API_Control_SetControlMaxSize(control_handle, int32, int32);
void API_Control_SetControlFixedSize(control_handle, int32, int32);
void API_Control_GetControlExpansionEnabled(const_control_handle, api_bool*, api_bool*);
void API_Control_SetControlExpansionEnabled(control_handle, api_bool, api_bool);
void API_Control_GetControlPosition(const_control_handle, int32*, int32*);
void API_Control_SetControlPosition(control_handle, int32, int32);
api_bool API_Control_GetControlUnderMouseStatus(const_control_handle);
void API_Control_BringControlToFront(control_handle);
void API_Control_SendControlToBack(control_handle);
void API_Control_StackControls(control_handle stackThis, control_handle underThis);
sizer_handle API_Control_GetControlSizer(const_control_handle);
void API_Control_SetControlSizer(control_handle, sizer_handle);
void API_Control_GlobalToLocal(const_control_handle, int32*, int32*);
void API_Control_LocalToGlobal(const_control_handle, int32*, int32*);
void API_Control_ParentToLocal(const_control_handle, int32*, int32*);
void API_Control_LocalToParent(const_control_handle, int32*, int32*);
void API_Control_ControlToLocal(const_control_handle, const_control_handle, int32*, int32*);
void API_Control_LocalToControl(const_control_handle, const_control_handle, int32*, int32*);
control_handle API_Control_GetChildByPos(const_control_handle, int32, int32); // returns client handle

   void           (API_Control_GetChildrenRect)( const_control_handle, int32*, int32*, int32*, int32* );

   api_bool       (API_Control_GetControlAncestry)( const_control_handle, const_control_handle );

   control_handle (API_Control_GetControlParent)( const_control_handle ); // returns client handle
   void           (API_Control_SetControlParent)( control_handle, control_handle );

   control_handle (API_Control_GetControlWindow)( const_control_handle ); // returns client handle

   api_bool       (API_Control_GetControlEnabled)( const_control_handle );
   void           (API_Control_SetControlEnabled)( control_handle, api_bool );

   api_bool       (API_Control_GetControlMouseTrackingEnabled)( const_control_handle );
   void           (API_Control_SetControlMouseTrackingEnabled)( control_handle, api_bool );

   api_bool       (API_Control_GetControlVisible)( const_control_handle );
   void           (API_Control_SetControlVisible)( control_handle, api_bool );

   void           (API_Control_GetControlVisibleRect)( const_control_handle, int32*, int32*, int32*, int32* );

   api_bool       (API_Control_GetWindowState)( const_control_handle, api_bool* active, api_bool* modal, api_bool* maximized, api_bool* minimized ); // returns true if control is a window

   void           (API_Control_ActivateWindow)( control_handle );

   api_bool       (API_Control_GetControlFocus)( const_control_handle );
   void           (API_Control_SetControlFocus)( control_handle, api_bool );

   int32          (API_Control_GetControlFocusStyle)( const_control_handle );
   void           (API_Control_SetControlFocusStyle)( control_handle, int32 );

   control_handle (API_Control_GetFocusChildControl)( const_control_handle ); // returns client handle

   control_handle (API_Control_GetChildControlToFocus)( const_control_handle ); // returns client handle
   void           (API_Control_SetChildControlToFocus)( control_handle, control_handle );

   control_handle (API_Control_GetNextSiblingControlToFocus)( const_control_handle ); // returns client handle
   void           (API_Control_SetNextSiblingControlToFocus)( control_handle, control_handle );

   api_bool       (API_Control_GetControlUpdatesEnabled)( const_control_handle );
   void           (API_Control_SetControlUpdatesEnabled)( control_handle, api_bool );

   void           (API_Control_UpdateControl)( control_handle );
   void           (API_Control_UpdateControlRect)( control_handle, int32, int32, int32, int32 );

   void           (API_Control_RepaintControl)( control_handle );
   void           (API_Control_RepaintControlRect)( control_handle, int32, int32, int32, int32 );

   void           (API_Control_RestyleControl)( control_handle );

   void           (API_Control_EnsureControlLayoutUpdated)( control_handle );

   void           (API_Control_ScrollControl)( control_handle, int32, int32 );
   void           (API_Control_ScrollControlRect)( control_handle, int32, int32, int32, int32, int32, int32 );

   cursor_handle  (API_Control_GetControlCursor)( const_control_handle );
   void           (API_Control_SetControlCursor)( control_handle, const_cursor_handle );
   void           (API_Control_SetControlCursorToParent)( control_handle );

   api_bool       (API_Control_GetControlStyleSheet)( const_control_handle, char16_type*, size_type* );
   void           (API_Control_SetControlStyleSheet)( control_handle, const char16_type* );

   uint32         (API_Control_GetControlBackgroundColor)( const_control_handle );
   void           (API_Control_SetControlBackgroundColor)( control_handle, uint32 );

   uint32         (API_Control_GetControlForegroundColor)( const_control_handle );
   void           (API_Control_SetControlForegroundColor)( control_handle, uint32 );

   uint32         (API_Control_GetControlCanvasColor)( const_control_handle );
   void           (API_Control_SetControlCanvasColor)( control_handle, uint32 );

   uint32         (API_Control_GetControlAlternateCanvasColor)( const_control_handle );
   void           (API_Control_SetControlAlternateCanvasColor)( control_handle, uint32 );

   uint32         (API_Control_GetControlTextColor)( const_control_handle );
   void           (API_Control_SetControlTextColor)( control_handle, uint32 );

   uint32         (API_Control_GetControlButtonColor)( const_control_handle );
   void           (API_Control_SetControlButtonColor)( control_handle, uint32 );

   uint32         (API_Control_GetControlButtonTextColor)( const_control_handle );
   void           (API_Control_SetControlButtonTextColor)( control_handle, uint32 );

   uint32         (API_Control_GetControlHighlightColor)( const_control_handle );
   void           (API_Control_SetControlHighlightColor)( control_handle, uint32 );

   uint32         (API_Control_GetControlHighlightedTextColor)( const_control_handle );
   void           (API_Control_SetControlHighlightedTextColor)( control_handle, uint32 );

   font_handle    (API_Control_GetControlFont)( const_control_handle );
   void           (API_Control_SetControlFont)( control_handle, const_font_handle );

   void           (API_Control_GetWindowOpacity)( const_control_handle, double* );
   void           (API_Control_SetWindowOpacity)( control_handle, double );

   api_bool       (API_Control_GetWindowTitle)( const_control_handle, char16_type*, size_type* );
   void           (API_Control_SetWindowTitle)( control_handle, const char16_type* );

   api_bool       (API_Control_GetInfoText)( const_control_handle, char16_type*, size_type* );
   void           (API_Control_SetInfoText)( control_handle, const char16_type* );

   api_bool       (API_Control_GetRealTimePreviewActive)( const_control_handle );
   void           (API_Control_SetRealTimePreviewActive)( control_handle, api_bool );

   api_bool       (API_Control_GetTrackViewActive)( const_control_handle );
   void           (API_Control_SetTrackViewActive)( control_handle, api_bool );

   api_bool       (API_Control_GetWindowToolTip)( const_control_handle, char16_type*, size_type* );
   void           (API_Control_SetWindowToolTip)( control_handle, const char16_type* );

   api_bool       (API_Control_GetControlDisplayPixelRatio)( const_control_handle, double* );
   api_bool       (API_Control_GetControlResourcePixelRatio)( const_control_handle, double* );
   api_bool       (API_Control_GetControlDevicePixelRatio)( const_control_handle, double* );

   api_bool       (API_Control_SetDestroyEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (API_Control_SetShowEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (API_Control_SetHideEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (API_Control_SetCloseEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (API_Control_SetGetFocusEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (API_Control_SetLoseFocusEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (API_Control_SetEnterEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (API_Control_SetLeaveEventRoutine)( control_handle, api_handle, pcl::control_event_routine );
   api_bool       (API_Control_SetMoveEventRoutine)( control_handle, api_handle, pcl::move_event_routine );
   api_bool       (API_Control_SetResizeEventRoutine)( control_handle, api_handle, pcl::resize_event_routine );
   api_bool       (API_Control_SetPaintEventRoutine)( control_handle, api_handle, pcl::paint_event_routine );
   api_bool       (API_Control_SetKeyPressEventRoutine)( control_handle, api_handle, pcl::keyboard_event_routine );
   api_bool       (API_Control_SetKeyReleaseEventRoutine)( control_handle, api_handle, pcl::keyboard_event_routine );
   api_bool       (API_Control_SetMouseMoveEventRoutine)( control_handle, api_handle, pcl::mouse_event_routine );
   api_bool       (API_Control_SetMouseDoubleClickEventRoutine)( control_handle, api_handle, pcl::mouse_event_routine );
   api_bool       (API_Control_SetMousePressEventRoutine)( control_handle, api_handle, pcl::mouse_button_event_routine );
   api_bool       (API_Control_SetMouseReleaseEventRoutine)( control_handle, api_handle, pcl::mouse_button_event_routine );
   api_bool       (API_Control_SetWheelEventRoutine)( control_handle, api_handle, pcl::wheel_event_routine );
   api_bool       (API_Control_SetFileDragEventRoutine)( control_handle, api_handle, pcl::file_drag_event_handler );
   api_bool       (API_Control_SetFileDropEventRoutine)( control_handle, api_handle, pcl::file_drag_event_handler );
   api_bool       (API_Control_SetViewDragEventRoutine)( control_handle, api_handle, pcl::view_drag_event_handler );
   api_bool       (API_Control_SetViewDropEventRoutine)( control_handle, api_handle, pcl::view_drag_event_handler );
   api_bool       (API_Control_SetChildCreateEventRoutine)( control_handle, api_handle, pcl::child_event_routine );
   api_bool       (API_Control_SetChildDestroyEventRoutine)( control_handle, api_handle, pcl::child_event_routine );

// ----------------------------------------------------------------------------
// DialogContext API
// ----------------------------------------------------------------------------

control_handle API_Dialog_CreateDialog(api_handle, api_handle client, control_handle parent, uint32 flags);
int32 API_Dialog_ExecuteDialog(control_handle);
void API_Dialog_OpenDialog(control_handle);
void API_Dialog_ReturnDialog(control_handle, int32);
api_bool API_Dialog_GetDialogResizable(const_control_handle);
void API_Dialog_SetDialogResizable(control_handle, api_bool);
api_bool API_Dialog_SetExecuteDialogEventRoutine(control_handle, api_handle, pcl::event_routine);
api_bool API_Dialog_SetReturnDialogEventRoutine(control_handle, api_handle, pcl::value_event_routine);
api_bool API_Dialog_ExecuteOpenFileDialog(char16_type* fileName, const char16_type* caption, const char16_type* initialPath, const char16_type* filters, const char16_type* selectedExtension);
api_bool API_Dialog_ExecuteOpenMultipleFilesDialog(char16_type* fileName, ::file_enumeration_callback, void*, const char16_type* caption, const char16_type* initialPath, const char16_type* filters, const char16_type* selectedExtension);
api_bool API_Dialog_ExecuteSaveFileDialog(char16_type* filePath, const char16_type* caption, const char16_type* initialPath, const char16_type* filters, const char16_type* selectedExtension, api_bool overwritePrompt);
api_bool API_Dialog_ExecuteGetDirectoryDialog(char16_type* dirPath, const char16_type* caption, const char16_type* initialPath);

// ----------------------------------------------------------------------------
// FrameContext API
// ----------------------------------------------------------------------------

control_handle API_Frame_CreateFrame(api_handle, api_handle client, control_handle parent, uint32 flags);
int32 API_Frame_GetFrameStyle(const_control_handle);
void API_Frame_SetFrameStyle(control_handle, int32);
int32 API_Frame_GetFrameLineWidth(const_control_handle);
void API_Frame_SetFrameLineWidth(control_handle, int32);
int32 API_Frame_GetFrameBorderWidth(const_control_handle);

// ----------------------------------------------------------------------------
// GroupBoxContext API
// ----------------------------------------------------------------------------

control_handle API_GroupBox_CreateGroupBox(api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags);
api_bool API_GroupBox_GetGroupBoxTitle(const_control_handle, char16_type*, size_type*);
void API_GroupBox_SetGroupBoxTitle(control_handle, const char16_type*);
api_bool API_GroupBox_GetGroupBoxCheckable(const_control_handle);
void API_GroupBox_SetGroupBoxCheckable(control_handle, api_bool);
api_bool API_GroupBox_GetGroupBoxChecked(const_control_handle); // returns true if group box checked
   void           (API_GroupBox_SetGroupBoxChecked)( control_handle, api_bool );

   api_bool       (API_GroupBox_SetGroupBoxCheckEventRoutine)( control_handle, api_handle, pcl::button_check_event_routine );

// ----------------------------------------------------------------------------
// TabBoxContext API
// ----------------------------------------------------------------------------

control_handle API_TabBox_CreateTabBox(api_handle, api_handle client, control_handle parent, uint32 flags);
int32 API_TabBox_GetTabBoxLength(const_control_handle);
int32 API_TabBox_GetTabBoxCurrentPageIndex(const_control_handle);
void API_TabBox_SetTabBoxCurrentPageIndex(control_handle, int32);
control_handle API_TabBox_GetTabBoxPageByIndex(const_control_handle, int32); // returns client handle

   void           (API_TabBox_InsertTabBoxPage)( control_handle, int32, control_handle, const char16_type*, const_bitmap_handle );

   void           (API_TabBox_RemoveTabBoxPage)( control_handle, int32 );

   int32          (API_TabBox_GetTabBoxPosition)( const_control_handle );  // 0=top, 1=bottom
   void           (API_TabBox_SetTabBoxPosition)( control_handle, int32 ); //   idem.

   api_bool       (API_TabBox_GetTabBoxPageEnabled)( const_control_handle, int32 );
   void           (API_TabBox_SetTabBoxPageEnabled)( control_handle, int32, api_bool );

   api_bool       (API_TabBox_GetTabBoxPageLabel)( const_control_handle, int32, char16_type*, size_type* );
   void           (API_TabBox_SetTabBoxPageLabel)( control_handle, int32, const char16_type* );

   bitmap_handle  (API_TabBox_GetTabBoxPageIcon)( const_control_handle, int32 );
   void           (API_TabBox_SetTabBoxPageIcon)( control_handle, int32, const_bitmap_handle );

   api_bool       (API_TabBox_GetTabBoxPageToolTip)( const_control_handle, int32, char16_type*, size_type* );
   void           (API_TabBox_SetTabBoxPageToolTip)( control_handle, int32, const char16_type* );

   control_handle (API_TabBox_GetTabBoxLeftControl)( const_control_handle ); // returns client handle
   control_handle (API_TabBox_GetTabBoxRightControl)( const_control_handle ); // returns client handle
   void           (API_TabBox_SetTabBoxControls)( control_handle, control_handle, control_handle );

   api_bool       (API_TabBox_SetTabBoxPageSelectedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );

// ----------------------------------------------------------------------------
// ButtonContext API
// ----------------------------------------------------------------------------

control_handle API_Button_CreatePushButton(api_handle, api_handle client, const char16_type*, const_bitmap_handle, control_handle parent, uint32 flags);
control_handle API_Button_CreateCheckBox(api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags);
control_handle API_Button_CreateRadioButton(api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags);
control_handle API_Button_CreateToolButton(api_handle, api_handle client, const char16_type*, const_bitmap_handle, api_bool checkable, control_handle parent, uint32 flags);
api_bool API_Button_GetButtonText(const_control_handle, char16_type*, size_type*);
void API_Button_SetButtonText(control_handle, const char16_type*);
bitmap_handle API_Button_GetButtonIcon(const_control_handle);
void API_Button_SetButtonIcon(control_handle, const_bitmap_handle);
void API_Button_GetButtonIconSize(const_control_handle, int32* w, int32* h);
void API_Button_SetButtonIconSize(control_handle, int32 w, int32 h);
api_bool API_Button_GetButtonPushed(const_control_handle); // returns true if button pushed
   void           (API_Button_SetButtonPushed)( control_handle, api_bool );

   uint32         (API_Button_GetButtonChecked)( const_control_handle ); // returns 0=unchecked, 1=checked, 2=tristate
   void           (API_Button_SetButtonChecked)( control_handle, uint32 );

   api_bool       (API_Button_GetButtonDefaultEnabled)( const_control_handle ); // returns true if button is default
   void           (API_Button_SetButtonDefaultEnabled)( control_handle, api_bool );

   api_bool       (API_Button_GetButtonTristateEnabled)( const_control_handle ); // returns true if button is in tristate mode
   void           (API_Button_SetButtonTristateEnabled)( control_handle, api_bool );

   api_bool       (API_Button_GetToolButtonCheckable)( const_control_handle );
   void           (API_Button_SetToolButtonCheckable)( control_handle, api_bool );

   api_bool       (API_Button_SetButtonClickEventRoutine)( control_handle, api_handle, pcl::button_click_event_routine );
   api_bool       (API_Button_SetButtonPressEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (API_Button_SetButtonReleaseEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (API_Button_SetButtonCheckEventRoutine)( control_handle, api_handle, pcl::button_check_event_routine );

// ----------------------------------------------------------------------------
// EditContext API
// ----------------------------------------------------------------------------

control_handle API_Edit_CreateEdit(api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags);
api_bool API_Edit_GetEditText(const_control_handle, char16_type*, size_type*);
void API_Edit_SetEditText(control_handle, const char16_type*);
api_bool API_Edit_GetEditReadOnly(const_control_handle);
void API_Edit_SetEditReadOnly(control_handle, api_bool);
api_bool API_Edit_GetEditModified(const_control_handle);
void API_Edit_SetEditModified(control_handle, api_bool);
api_bool API_Edit_GetEditPasswordEnabled(const_control_handle);
void API_Edit_SetEditPasswordEnabled(control_handle, api_bool);
int32 API_Edit_GetEditMaxLength(const_control_handle);
void API_Edit_SetEditMaxLength(control_handle, int32);
api_bool API_Edit_GetEditMask(const_control_handle, char16_type*, size_type*);
void API_Edit_SetEditMask(control_handle, const char16_type*);
api_bool API_Edit_GetEditValidatingRegExp(const_control_handle, char16_type*, size_type*, api_bool* caseSensitive);
api_bool API_Edit_SetEditValidatingRegExp(control_handle, const char16_type*, api_bool caseSensitive);
api_bool API_Edit_GetEditValid(const_control_handle);
void API_Edit_SetEditSelected(control_handle, api_bool);
int32 API_Edit_GetEditAlignment(const_control_handle); // only left and right alignments
   void           (API_Edit_SetEditAlignment)( control_handle, int32 ); //    idem.

   int32          (API_Edit_GetEditCaretPosition)( const_control_handle );
   void           (API_Edit_SetEditCaretPosition)( control_handle, int32 );

   void           (API_Edit_GetEditSelection)( const_control_handle, int32*, int32* );
   void           (API_Edit_SetEditSelection)( control_handle, int32, int32 );

   api_bool       (API_Edit_GetEditSelectedText)( const_control_handle, char16_type*, size_type* );

   api_bool       (API_Edit_SetEditCompletedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (API_Edit_SetReturnPressedEventRoutine)( control_handle, api_handle, pcl::event_routine );
   api_bool       (API_Edit_SetTextUpdatedEventRoutine)( control_handle, api_handle, pcl::unicode_event_routine );
   api_bool       (API_Edit_SetCaretPositionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );
   api_bool       (API_Edit_SetSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );

// ----------------------------------------------------------------------------
// TextBoxContext API
// ----------------------------------------------------------------------------

control_handle API_TextBox_CreateTextBox(api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags);
api_bool API_TextBox_GetTextBoxText(const_control_handle, char16_type*, size_type*);
void API_TextBox_SetTextBoxText(control_handle, const char16_type*);
api_bool API_TextBox_GetTextBoxReadOnly(const_control_handle);
void API_TextBox_SetTextBoxReadOnly(control_handle, api_bool);
void API_TextBox_SetTextBoxSelected(control_handle, api_bool);
int32 API_TextBox_GetTextBoxCaretPosition(const_control_handle);
void API_TextBox_SetTextBoxCaretPosition(control_handle, int32);
void API_TextBox_GetTextBoxSelection(const_control_handle, int32*, int32*);
void API_TextBox_SetTextBoxSelection(control_handle, int32, int32);
api_bool API_TextBox_GetTextBoxSelectedText(const_control_handle, char16_type*, size_type*);
void API_TextBox_InsertTextBoxText(control_handle, const char16_type*);
void API_TextBox_DeleteTextBoxText(control_handle);
api_bool API_TextBox_SetTextBoxUpdatedEventRoutine(control_handle, api_handle, pcl::unicode_event_routine);
api_bool API_TextBox_SetTextBoxCaretPositionUpdatedEventRoutine(control_handle, api_handle, pcl::range_event_routine);
api_bool API_TextBox_SetTextBoxSelectionUpdatedEventRoutine(control_handle, api_handle, pcl::range_event_routine);

// ----------------------------------------------------------------------------
// ComboBoxContext API
// ----------------------------------------------------------------------------

control_handle API_ComboBox_CreateComboBox(api_handle, api_handle client, control_handle parent, uint32 flags);
int32 API_ComboBox_GetComboBoxLength(const_control_handle);
int32 API_ComboBox_GetComboBoxCurrentItem(const_control_handle);
void API_ComboBox_SetComboBoxCurrentItem(control_handle, int32);
int32 API_ComboBox_FindComboBoxItem(const_control_handle, const char16_type*, int32, api_bool exactMatch, api_bool caseSensitive);
void API_ComboBox_InsertComboBoxItem(control_handle, int32, const char16_type*, const_bitmap_handle);
void API_ComboBox_RemoveComboBoxItem(control_handle, int32);
void API_ComboBox_ClearComboBox(control_handle);
api_bool API_ComboBox_GetComboBoxItemText(const_control_handle, int32, char16_type*, size_type*);
void API_ComboBox_SetComboBoxItemText(control_handle, int32, const char16_type*);
bitmap_handle API_ComboBox_GetComboBoxItemIcon(const_control_handle, int32);
void API_ComboBox_SetComboBoxItemIcon(control_handle, int32, const_bitmap_handle);
api_bool API_ComboBox_GetComboBoxEditEnabled(const_control_handle);
void API_ComboBox_SetComboBoxEditEnabled(control_handle, api_bool);
api_bool API_ComboBox_GetComboBoxEditText(const_control_handle, char16_type*, size_type*);
void API_ComboBox_SetComboBoxEditText(control_handle, const char16_type*);
api_bool API_ComboBox_GetComboBoxAutoCompletionEnabled(const_control_handle);
void API_ComboBox_SetComboBoxAutoCompletionEnabled(control_handle, api_bool);
void API_ComboBox_GetComboBoxIconSize(const_control_handle, int32*, int32*);
void API_ComboBox_SetComboBoxIconSize(control_handle, int32, int32);
int32 API_ComboBox_GetComboBoxMaxVisibleItemCount(const_control_handle);
void API_ComboBox_SetComboBoxMaxVisibleItemCount(control_handle, int32);
int32 API_ComboBox_GetComboBoxMinItemCharWidth(const_control_handle);
void API_ComboBox_SetComboBoxMinItemCharWidth(control_handle, int32);
void API_ComboBox_SetComboBoxListVisible(control_handle, api_bool);
api_bool API_ComboBox_SetComboBoxItemSelectedEventRoutine(control_handle, api_handle, pcl::value_event_routine);
api_bool API_ComboBox_SetComboBoxItemHighlightedEventRoutine(control_handle, api_handle, pcl::value_event_routine);
api_bool API_ComboBox_SetComboBoxEditTextUpdatedEventRoutine(control_handle, api_handle, pcl::event_routine);

// ----------------------------------------------------------------------------
// SliderContext API
// ----------------------------------------------------------------------------

control_handle API_Slider_CreateSlider(api_handle, api_handle client, api_bool vertical, control_handle parent, uint32 flags);
int32 API_Slider_GetSliderValue(const_control_handle);
void API_Slider_SetSliderValue(control_handle, int32);
void API_Slider_GetSliderRange(const_control_handle, int32*, int32*);
void API_Slider_SetSliderRange(control_handle, int32, int32);
int32 API_Slider_GetSliderStepSize(const_control_handle);
void API_Slider_SetSliderStepSize(control_handle, int32);
int32 API_Slider_GetSliderPageSize(const_control_handle);
void API_Slider_SetSliderPageSize(control_handle, int32);
int32 API_Slider_GetSliderTickInterval(const_control_handle);
void API_Slider_SetSliderTickInterval(control_handle, int32);
int32 API_Slider_GetSliderTickStyle(const_control_handle);
void API_Slider_SetSliderTickStyle(control_handle, int32);
api_bool API_Slider_GetSliderTrackingEnabled(const_control_handle);
void API_Slider_SetSliderTrackingEnabled(control_handle, api_bool);
api_bool API_Slider_SetSliderValueUpdatedEventRoutine(control_handle, api_handle, pcl::value_event_routine);
api_bool API_Slider_SetSliderRangeUpdatedEventRoutine(control_handle, api_handle, pcl::range_event_routine);

// ----------------------------------------------------------------------------
// SpinBoxContext API
// ----------------------------------------------------------------------------

control_handle API_SpinBox_CreateSpinBox(api_handle, api_handle client, control_handle parent, uint32 flags);
int32 API_SpinBox_GetSpinBoxValue(const_control_handle);
void API_SpinBox_SetSpinBoxValue(control_handle, int32);
void API_SpinBox_GetSpinBoxRange(const_control_handle, int32*, int32*);
void API_SpinBox_SetSpinBoxRange(control_handle, int32, int32);
int32 API_SpinBox_GetSpinBoxStepSize(const_control_handle);
void API_SpinBox_SetSpinBoxStepSize(control_handle, int32);
api_bool API_SpinBox_GetSpinBoxWrappingEnabled(const_control_handle);
void API_SpinBox_SetSpinBoxWrappingEnabled(control_handle, api_bool);
api_bool API_SpinBox_GetSpinBoxEditable(const_control_handle);
void API_SpinBox_SetSpinBoxEditable(control_handle, api_bool);
api_bool API_SpinBox_GetSpinBoxPrefix(const_control_handle, char16_type*, size_type*);
void API_SpinBox_SetSpinBoxPrefix(control_handle, const char16_type*);
api_bool API_SpinBox_GetSpinBoxSuffix(const_control_handle, char16_type*, size_type*);
void API_SpinBox_SetSpinBoxSuffix(control_handle, const char16_type*);
api_bool API_SpinBox_GetSpinBoxMinimumValueText(const_control_handle, char16_type*, size_type*);
void API_SpinBox_SetSpinBoxMinimumValueText(control_handle, const char16_type*);
int32 API_SpinBox_GetSpinBoxAlignment(const_control_handle); // only left and right alignments
   void           (API_SpinBox_SetSpinBoxAlignment)( control_handle, int32 ); //    idem.

   api_bool       (API_SpinBox_SetSpinBoxValueUpdatedEventRoutine)( control_handle, api_handle, pcl::value_event_routine );
   api_bool       (API_SpinBox_SetSpinBoxRangeUpdatedEventRoutine)( control_handle, api_handle, pcl::range_event_routine );

// ----------------------------------------------------------------------------
// LabelContext API
// ----------------------------------------------------------------------------

control_handle API_Label_CreateLabel(api_handle, api_handle client, const char16_type*, control_handle parent, uint32 flags);
api_bool API_Label_GetLabelText(const_control_handle, char16_type*, size_type*);
void API_Label_SetLabelText(control_handle, const char16_type*);
int32 API_Label_GetLabelMargin(const_control_handle);
void API_Label_SetLabelMargin(control_handle, int32);
int32 API_Label_GetLabelAlignment(const_control_handle);
void API_Label_SetLabelAlignment(control_handle, int32);
api_bool API_Label_GetLabelWordWrappingEnabled(const_control_handle);
void API_Label_SetLabelWordWrappingEnabled(control_handle, api_bool);
api_bool API_Label_GetLabelRichTextEnabled(const_control_handle);
void API_Label_SetLabelRichTextEnabled(control_handle, api_bool);

// ----------------------------------------------------------------------------
// BitmapBoxContext API
// ----------------------------------------------------------------------------

control_handle API_BitmapBox_CreateBitmapBox(api_handle, api_handle client, const_bitmap_handle, control_handle parent, uint32 flags);
bitmap_handle API_BitmapBox_GetBitmapBoxBitmap(const_control_handle);
void API_BitmapBox_SetBitmapBoxBitmap(control_handle, const_bitmap_handle);
int32 API_BitmapBox_GetBitmapBoxMargin(const_control_handle);
void API_BitmapBox_SetBitmapBoxMargin(control_handle, int32);
api_bool API_BitmapBox_GetBitmapBoxAutoFitEnabled(const_control_handle);
void API_BitmapBox_SetBitmapBoxAutoFitEnabled(control_handle, api_bool);

// ----------------------------------------------------------------------------
// ScrollBoxContext API
// ----------------------------------------------------------------------------

control_handle API_ScrollBox_CreateScrollBox(api_handle, api_handle client, control_handle parent, uint32 flags);
control_handle API_ScrollBox_CreateScrollBoxViewport(control_handle, api_handle client);
void API_ScrollBox_GetScrollBarsVisible(const_control_handle, api_bool*, api_bool*);
void API_ScrollBox_SetScrollBarsVisible(control_handle, api_bool, api_bool);
void API_ScrollBox_GetScrollBoxAutoScrollEnabled(const_control_handle, api_bool*, api_bool*);
void API_ScrollBox_SetScrollBoxAutoScrollEnabled(control_handle, api_bool, api_bool);
void API_ScrollBox_GetScrollBoxHorizontalRange(const_control_handle, int32*, int32*);
void API_ScrollBox_SetScrollBoxHorizontalRange(control_handle, int32, int32);
void API_ScrollBox_GetScrollBoxVerticalRange(const_control_handle, int32*, int32*);
void API_ScrollBox_SetScrollBoxVerticalRange(control_handle, int32, int32);
void API_ScrollBox_GetScrollBoxPageSize(const_control_handle, int32*, int32*);
void API_ScrollBox_SetScrollBoxPageSize(control_handle, int32, int32);
void API_ScrollBox_GetScrollBoxLineSize(const_control_handle, int32*, int32*);
void API_ScrollBox_SetScrollBoxLineSize(control_handle, int32, int32);
void API_ScrollBox_GetScrollBoxPosition(const_control_handle, int32*, int32*);
void API_ScrollBox_SetScrollBoxPosition(control_handle, int32, int32);
void API_ScrollBox_GetScrollBoxTrackingEnabled(const_control_handle, api_bool*, api_bool*);
void API_ScrollBox_SetScrollBoxTrackingEnabled(control_handle, api_bool, api_bool);
api_bool API_ScrollBox_SetScrollBoxHorizontalPosUpdatedEventRoutine(control_handle, api_handle, pcl::value_event_routine);
api_bool API_ScrollBox_SetScrollBoxVerticalPosUpdatedEventRoutine(control_handle, api_handle, pcl::value_event_routine);
api_bool API_ScrollBox_SetScrollBoxHorizontalRangeUpdatedEventRoutine(control_handle, api_handle, pcl::range_event_routine);
api_bool API_ScrollBox_SetScrollBoxVerticalRangeUpdatedEventRoutine(control_handle, api_handle, pcl::range_event_routine);

// ----------------------------------------------------------------------------
// TreeBoxContext API
// ----------------------------------------------------------------------------

control_handle API_TreeBox_CreateTreeBox(api_handle, api_handle client, control_handle parent, uint32 flags);
control_handle API_TreeBox_CreateTreeBoxViewport(control_handle, api_handle client);
api_handle API_TreeBox_CreateTreeBoxNode(api_handle, api_handle nodeClient);
int32 API_TreeBox_GetTreeBoxChildCount(const_control_handle);
api_handle API_TreeBox_GetTreeBoxChild(const_control_handle, int32 idx); // returns client handle

   int32          (API_TreeBox_GetTreeBoxChildIndex)( const_control_handle, const_api_handle );

   void           (API_TreeBox_InsertTreeBoxNode)( control_handle, int32, api_handle );
   void           (API_TreeBox_RemoveTreeBoxNode)( control_handle, int32 );

   void           (API_TreeBox_ClearTreeBox)( control_handle );

   api_bool       (API_TreeBox_GetTreeBoxUniformRowHeightEnabled)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxUniformRowHeightEnabled)( control_handle, api_bool );

   api_handle     (API_TreeBox_GetTreeBoxCurrentNode)( const_control_handle ); // returns client handle
   void           (API_TreeBox_SetTreeBoxCurrentNode)( control_handle, api_handle );

   api_bool       (API_TreeBox_GetTreeBoxMultipleNodeSelectionEnabled)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxMultipleNodeSelectionEnabled)( control_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxSelectedNodes)( const_control_handle, api_handle*, size_type* );

   void           (API_TreeBox_SelectAllTreeBoxNodes)( control_handle );

   void           (API_TreeBox_BeginTreeBoxNodeEdition)( control_handle, api_handle, int32 col );
   void           (API_TreeBox_EndTreeBoxNodeEdition)( control_handle, api_handle, int32 col );

   void           (API_TreeBox_EditTreeBoxNode)( control_handle, api_handle, int32 col );

   api_handle     (API_TreeBox_GetTreeBoxNodeByPos)( const_control_handle, int32 x, int32 y ); // returns client handle

   void           (API_TreeBox_SetTreeBoxNodeIntoView)( control_handle, api_handle );

   void           (API_TreeBox_GetTreeBoxNodeRect)( const_control_handle, const_api_handle, int32*, int32*, int32*, int32* );

   int32          (API_TreeBox_GetTreeBoxColumnCount)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxColumnCount)( control_handle, int32 );

   api_bool       (API_TreeBox_GetTreeBoxColumnVisible)( const_control_handle, int32 );
   void           (API_TreeBox_SetTreeBoxColumnVisible)( control_handle, int32, api_bool );

   int32          (API_TreeBox_GetTreeBoxColumnWidth)( const_control_handle, int32 );
   void           (API_TreeBox_SetTreeBoxColumnWidth)( control_handle, int32, int32 );

   void           (API_TreeBox_AdjustTreeBoxColumnWidthToContents)( control_handle, int32 );

   api_bool       (API_TreeBox_GetTreeBoxHeaderText)( const_control_handle, int32, char16_type*, size_type* );
   void           (API_TreeBox_SetTreeBoxHeaderText)( control_handle, int32, const char16_type* );

   bitmap_handle  (API_TreeBox_GetTreeBoxHeaderIcon)( const_control_handle, int32 );
   void           (API_TreeBox_SetTreeBoxHeaderIcon)( control_handle, int32, const_bitmap_handle );

   int32          (API_TreeBox_GetTreeBoxHeaderAlignment)( const_control_handle, int32 );
   void           (API_TreeBox_SetTreeBoxHeaderAlignment)( control_handle, int32, int32 );

   api_bool       (API_TreeBox_GetTreeBoxHeaderVisible)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxHeaderVisible)( control_handle, api_bool );

   int32          (API_TreeBox_GetTreeBoxIndentSize)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxIndentSize)( control_handle, int32 );

   api_bool       (API_TreeBox_GetTreeBoxNodeExpansionEnabled)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxNodeExpansionEnabled)( control_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxRootDecorationEnabled)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxRootDecorationEnabled)( control_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxAlternateRowColorEnabled)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxAlternateRowColorEnabled)( control_handle, api_bool );

   void           (API_TreeBox_GetTreeBoxIconSize)( const_control_handle, int32*, int32* );
   void           (API_TreeBox_SetTreeBoxIconSize)( control_handle, int32, int32 );

   api_bool       (API_TreeBox_GetTreeBoxHeaderSortingEnabled)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxHeaderSortingEnabled)( control_handle, api_bool );

   void           (API_TreeBox_SortTreeBox)( control_handle, int32 col, api_bool ascending );

   api_bool       (API_TreeBox_GetTreeBoxNodeDraggingEnabled)( const_control_handle );
   void           (API_TreeBox_SetTreeBoxNodeDraggingEnabled)( control_handle, api_bool );

   // TreeBox Nodes

   control_handle (API_TreeBox_GetTreeBoxNodeParentBox)( const_api_handle ); // returns client handle

   api_handle     (API_TreeBox_GetTreeBoxNodeParent)( const_api_handle ); // returns client handle

   int32          (API_TreeBox_GetTreeBoxNodeChildCount)( const_api_handle );

   api_handle     (API_TreeBox_GetTreeBoxNodeChild)( const_api_handle, int32 ); // returns client handle

   void           (API_TreeBox_InsertTreeBoxNodeChild)( api_handle, int32, api_handle );
   void           (API_TreeBox_RemoveTreeBoxNodeChild)( api_handle, int32 );

   api_bool       (API_TreeBox_GetTreeBoxNodeEnabled)( const_api_handle );
   void           (API_TreeBox_SetTreeBoxNodeEnabled)( api_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxNodeExpanded)( const_api_handle );
   void           (API_TreeBox_SetTreeBoxNodeExpanded)( api_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxNodeSelectable)( const_api_handle );
   void           (API_TreeBox_SetTreeBoxNodeSelectable)( api_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxNodeSelected)( const_api_handle );
   void           (API_TreeBox_SetTreeBoxNodeSelected)( api_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxNodeCheckable)( const_api_handle );
   void           (API_TreeBox_SetTreeBoxNodeCheckable)( api_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxNodeChecked)( const_api_handle );
   void           (API_TreeBox_SetTreeBoxNodeChecked)( api_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxNodeEditable)( const_api_handle );
   void           (API_TreeBox_SetTreeBoxNodeEditable)( api_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxNodeFirstColumnSpanned)( const_api_handle );
   void           (API_TreeBox_SetTreeBoxNodeFirstColumnSpanned)( api_handle, api_bool );

   api_bool       (API_TreeBox_GetTreeBoxNodeColText)( const_api_handle, int32, char16_type*, size_type* );
   void           (API_TreeBox_SetTreeBoxNodeColText)( api_handle, int32, const char16_type* );

   bitmap_handle  (API_TreeBox_GetTreeBoxNodeColIcon)( const_api_handle, int32 );
   void           (API_TreeBox_SetTreeBoxNodeColIcon)( api_handle, int32, const_bitmap_handle );

   int32          (API_TreeBox_GetTreeBoxNodeColAlignment)( const_api_handle, int32 );
   void           (API_TreeBox_SetTreeBoxNodeColAlignment)( api_handle, int32, int32 );

   api_bool       (API_TreeBox_GetTreeBoxNodeColToolTip)( const_api_handle, int32, char16_type*, size_type* );
   void           (API_TreeBox_SetTreeBoxNodeColToolTip)( api_handle, int32, const char16_type* );

   font_handle    (API_TreeBox_GetTreeBoxNodeColFont)( const_api_handle, int32 );
   void           (API_TreeBox_SetTreeBoxNodeColFont)( api_handle, int32, const_font_handle );

   uint32         (API_TreeBox_GetTreeBoxNodeColBackgroundColor)( const_api_handle, int32 );
   void           (API_TreeBox_SetTreeBoxNodeColBackgroundColor)( api_handle, int32, uint32 );

   uint32         (API_TreeBox_GetTreeBoxNodeColTextColor)( const_api_handle, int32 );
   void           (API_TreeBox_SetTreeBoxNodeColTextColor)( api_handle, int32, uint32 );

   // TreeBox Events

   api_bool       (API_TreeBox_SetTreeBoxCurrentNodeUpdatedEventRoutine)( control_handle, api_handle, pcl::item_range_event_routine );
   api_bool       (API_TreeBox_SetTreeBoxNodeActivatedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (API_TreeBox_SetTreeBoxNodeUpdatedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (API_TreeBox_SetTreeBoxNodeEnteredEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (API_TreeBox_SetTreeBoxNodeClickedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (API_TreeBox_SetTreeBoxNodeDoubleClickedEventRoutine)( control_handle, api_handle, pcl::item_value_event_routine );
   api_bool       (API_TreeBox_SetTreeBoxNodeExpandedEventRoutine)( control_handle, api_handle, pcl::item_event_routine );
   api_bool       (API_TreeBox_SetTreeBoxNodeCollapsedEventRoutine)( control_handle, api_handle, pcl::item_event_routine );
   api_bool       (API_TreeBox_SetTreeBoxNodeSelectionUpdatedEventRoutine)( control_handle, api_handle, pcl::event_routine );

// ----------------------------------------------------------------------------
// TimerContext API
// ----------------------------------------------------------------------------

timer_handle API_Timer_CreateTimer(api_handle, api_handle client, uint32 flags);
void API_Timer_GetTimerInterval(const_timer_handle, uint32* msec);
void API_Timer_SetTimerInterval(timer_handle, uint32 msec);
api_bool API_Timer_GetTimerSingleShot(const_timer_handle);
void API_Timer_SetTimerSingleShot(timer_handle, api_bool);
api_bool API_Timer_IsTimerActive(const_timer_handle);
api_bool API_Timer_StartTimer(timer_handle);
void API_Timer_StopTimer(timer_handle);
api_bool API_Timer_SetTimerNotifyEventRoutine(timer_handle, api_handle, pcl::timer_event_routine);

// ----------------------------------------------------------------------------
// ThreadContext API
// ----------------------------------------------------------------------------

thread_handle API_Thread_CreateThread(api_handle, api_handle client, uint32 flags);
void API_Thread_StartThread(thread_handle, uint32 priority);
void API_Thread_KillThread(thread_handle);
api_bool API_Thread_IsThreadActive(const_thread_handle);
uint32 API_Thread_GetThreadPriority(const_thread_handle);
void API_Thread_SetThreadPriority(thread_handle, uint32);
uint32 API_Thread_GetThreadStackSize(const_thread_handle);
void API_Thread_SetThreadStackSize(thread_handle, uint32);
api_bool API_Thread_WaitThread(thread_handle, uint32 msec);
void API_Thread_SleepThread(thread_handle, uint32 msec);
uint32 API_Thread_GetThreadStatus(const_thread_handle);
void API_Thread_SetThreadStatus(thread_handle, uint32);
api_bool API_Thread_GetThreadStatusEx(const_thread_handle, uint32* status, uint32 flags); // 0x00=force_lock 0x01=try_lock

   api_bool       (API_Thread_GetThreadConsoleOutputText)( const_thread_handle, char16_type* text, size_type* len );
   void           (API_Thread_AppendThreadConsoleOutputText)( thread_handle, const char16_type* text, api_bool appendNewline );
   void           (API_Thread_ClearThreadConsoleOutputText)( thread_handle );

   thread_handle  (API_Thread_GetCurrentThread)();

   api_bool       (API_Thread_SetThreadExecRoutine)( thread_handle, pcl::thread_exec_routine );

   int32          (API_Thread_PerformanceAnalysisValue)( int32 algorithm, size_type length,
                                                        int32 itemSize, api_bool floatingPoint, int32 kernelSize, int32 width, int32 height );

// ----------------------------------------------------------------------------
// MutexContext API
// ----------------------------------------------------------------------------

mutex_handle API_Mutex_CreateMutex(api_handle, api_handle client, uint32 flags); // ### deprecated
   mutex_handle   (API_Mutex_CreateReadWriteMutex)( api_handle, api_handle client, uint32 flags );

   api_bool       (API_Mutex_GetLockState)( const_mutex_handle ); // ### disabled ### returns api_true if the mutex is locked

   api_bool       (API_Mutex_Lock)( mutex_handle, api_bool tryLock ); // ### deprecated
   api_bool       (API_Mutex_LockForRead)( mutex_handle, api_bool tryLock );
   api_bool       (API_Mutex_LockForWrite)( mutex_handle, api_bool tryLock );

   void           (API_Mutex_Unlock)( mutex_handle );

// ----------------------------------------------------------------------------
// ViewListContext API
// ----------------------------------------------------------------------------

control_handle API_ViewList_CreateViewList(api_handle, api_handle client, control_handle parent, uint32 flags);
void API_ViewList_RegenerateViewList(control_handle, api_bool mainViews, api_bool previews, api_bool realTimePreview);
void API_ViewList_GetViewListContents(const_control_handle, api_bool* mainViews, api_bool* previews, api_bool* realTimePreview);
const_view_handle API_ViewList_GetViewListExcludedView(const_control_handle);
void API_ViewList_SetViewListExcludedView(control_handle, const_view_handle);
view_handle API_ViewList_GetViewListCurrentView(const_control_handle);
void API_ViewList_SetViewListCurrentView(control_handle, view_handle);
api_bool API_ViewList_FindViewListView(const_control_handle, const_view_handle);
void API_ViewList_RemoveViewListView(control_handle, const_view_handle);
api_bool API_ViewList_SetViewListViewSelectedEventRoutine(control_handle, api_handle, pcl::view_event_routine);
api_bool API_ViewList_SetViewListCurrentViewUpdatedEventRoutine(control_handle, api_handle, pcl::view_event_routine);

// ----------------------------------------------------------------------------
// BitmapContext API
// ----------------------------------------------------------------------------

bitmap_handle API_Bitmap_CreateBitmap(api_handle, int32, int32, void*);
bitmap_handle API_Bitmap_CreateBitmapXPM(api_handle, const char**);
bitmap_handle API_Bitmap_CreateBitmapFromFile(api_handle, const char16_type*);
bitmap_handle API_Bitmap_CreateBitmapFromFile8(api_handle, const char*);
bitmap_handle API_Bitmap_CreateBitmapFromData(api_handle, const void*, size_type, const char*, uint32);
bitmap_handle API_Bitmap_CreateEmptyBitmap(api_handle);
bitmap_handle API_Bitmap_CloneBitmap(api_handle, const_bitmap_handle);
bitmap_handle API_Bitmap_CloneBitmapRect(api_handle, const_bitmap_handle, int32, int32, int32, int32);
bitmap_handle API_Bitmap_CreateBitmapFromSVG(api_handle, const char*, int32, int32, uint32 flags);
bitmap_handle API_Bitmap_CreateBitmapFromSVGFile(api_handle, const char16_type*, int32, int32, uint32 flags);
int32 API_Bitmap_GetBitmapFormat(bitmap_handle);
void API_Bitmap_SetBitmapFormat(bitmap_handle, int32);
unsigned int *API_Bitmap_GetBitmapScanLine(bitmap_handle, int32);
api_bool API_Bitmap_GetBitmapDimensions(const_bitmap_handle, int32*, int32*);
api_bool API_Bitmap_IsEmptyBitmap(const_bitmap_handle);
uint32 API_Bitmap_GetBitmapPixel(const_bitmap_handle, int32, int32);
void API_Bitmap_SetBitmapPixel(bitmap_handle, int32, int32, uint32);
bitmap_handle API_Bitmap_MirroredBitmap(const_bitmap_handle, api_bool h, api_bool v);
bitmap_handle API_Bitmap_ScaledBitmap(const_bitmap_handle, int32, int32, api_bool);
bitmap_handle API_Bitmap_RotatedBitmap(const_bitmap_handle, double, api_bool);
api_bool API_Bitmap_LoadBitmap(bitmap_handle, const char16_type*);
api_bool API_Bitmap_SaveBitmap(const_bitmap_handle, const char16_type*, int32 q);
api_bool API_Bitmap_LoadBitmapData(bitmap_handle, const void*, size_type, const char*, uint32);
void API_Bitmap_CopyBitmap(bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32);
void API_Bitmap_FillBitmap(bitmap_handle, int32, int32, int32, int32, uint32);
void API_Bitmap_OrBitmap(bitmap_handle, int32, int32, int32, int32, uint32);
void API_Bitmap_OrBitmaps(bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32);
void API_Bitmap_AndBitmap(bitmap_handle, int32, int32, int32, int32, uint32);
void API_Bitmap_AndBitmaps(bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32);
void API_Bitmap_XorBitmap(bitmap_handle, int32, int32, int32, int32, uint32);
void API_Bitmap_XorBitmaps(bitmap_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32);
void API_Bitmap_XorBitmapRect(bitmap_handle, int32, int32, int32, int32, uint32);
void API_Bitmap_ReplaceBitmapColor(bitmap_handle, int32, int32, int32, int32, uint32, uint32);
void API_Bitmap_SetBitmapAlpha(bitmap_handle, int32, int32, int32, int32, uint8);
void API_Bitmap_GetBitmapDevicePixelRatio(const_bitmap_handle, double*);
void API_Bitmap_SetBitmapDevicePixelRatio(bitmap_handle, double);

// ----------------------------------------------------------------------------
// SVGContext API
// ----------------------------------------------------------------------------

svg_handle API_SVG_CreateSVGFile(api_handle, const char16_type*, int32, int32, uint32);
svg_handle API_SVG_CreateSVGBuffer(api_handle, int32, int32, uint32);
api_bool API_SVG_GetSVGDimensions(const_svg_handle, int32*, int32*);
api_bool API_SVG_SetSVGDimensions(svg_handle, int32, int32);
api_bool API_SVG_GetSVGViewBox(const_svg_handle, double*, double*, double*, double*);
api_bool API_SVG_SetSVGViewBox(svg_handle, double, double, double, double);
int32 API_SVG_GetSVGResolution(const_svg_handle);
void API_SVG_SetSVGResolution(svg_handle, int32);
api_bool API_SVG_GetSVGFilePath(const_svg_handle, char16_type*, size_type*);
api_bool API_SVG_GetSVGDataBuffer(const_svg_handle, void*, size_type*);
api_bool API_SVG_GetSVGTitle(const_svg_handle, char16_type*, size_type*);
void API_SVG_SetSVGTitle(svg_handle, const char16_type*);
api_bool API_SVG_GetSVGDescription(const_svg_handle, char16_type*, size_type*);
void API_SVG_SetSVGDescription(svg_handle, const char16_type*);
api_bool API_SVG_IsSVGPainting(const_svg_handle);

// ----------------------------------------------------------------------------
// BrushContext API
// ----------------------------------------------------------------------------

brush_handle API_Brush_CreateBrush(api_handle, uint32, int32);
brush_handle API_Brush_CreateBitmapBrush(api_handle, const_bitmap_handle);
brush_handle API_Brush_CreateLinearGradientBrush(api_handle, double x1, double y1, double x2, double y2, int32 spread, const api_gradient_stop*, size_type count); // spread: 0=pad 1=reflect 2=repeat
   brush_handle   (API_Brush_CreateRadialGradientBrush)( api_handle, double cx, double cy, double r, double fx, double fy,
                                                         int32 spread, const api_gradient_stop*, size_type count );
   brush_handle   (API_Brush_CreateConicalGradientBrush)( api_handle, double cx, double cy, double angle,
                                                         const api_gradient_stop*, size_type count );
   brush_handle   (API_Brush_CloneBrush)( api_handle, const_brush_handle );

   uint32         (API_Brush_GetBrushColor)( const_brush_handle );
   void           (API_Brush_SetBrushColor)( brush_handle, uint32 );

   int32          (API_Brush_GetBrushStyle)( const_brush_handle );
   void           (API_Brush_SetBrushStyle)( brush_handle, int32 );

   bitmap_handle  (API_Brush_GetBrushBitmap)( const_brush_handle );
   void           (API_Brush_SetBrushBitmap)( brush_handle, const_bitmap_handle );

   int32          (API_Brush_GetBrushGradientType)( const_brush_handle ); // 0=none 1=linear 2=radial 3=conical
   api_bool       (API_Brush_GetBrushLinearGradientParameters)( const_brush_handle, double* x1, double* y1, double* x2, double* y2 );
   api_bool       (API_Brush_GetBrushRadialGradientParameters)( const_brush_handle, double* cx, double* cy, double* r, double* fx, double* fy );
   api_bool       (API_Brush_GetBrushConicalGradientParameters)( const_brush_handle, double* cx, double* cy, double* angle );
   int32          (API_Brush_GetBrushGradientSpread)( const_brush_handle ); // -1=error 0=pad 1=reflect 2=repeat
   api_bool       (API_Brush_GetBrushGradientStops)( const_brush_handle, api_gradient_stop*, size_type *count );

// ----------------------------------------------------------------------------
// PenContext API
// ----------------------------------------------------------------------------

pen_handle API_Pen_CreatePen(api_handle, uint32, float, int32, int32, int32);
pen_handle API_Pen_ClonePen(api_handle, const_pen_handle);
api_bool API_Pen_GetPenWidth(const_pen_handle, float*);
void API_Pen_SetPenWidth(pen_handle, float);
uint32 API_Pen_GetPenColor(const_pen_handle);
void API_Pen_SetPenColor(pen_handle, uint32);
int32 API_Pen_GetPenStyle(const_pen_handle);
void API_Pen_SetPenStyle(pen_handle, int32);
int32 API_Pen_GetPenCap(const_pen_handle);
void API_Pen_SetPenCap(pen_handle, int32);
int32 API_Pen_GetPenJoin(const_pen_handle);
void API_Pen_SetPenJoin(pen_handle, int32);
brush_handle API_Pen_GetPenBrush(const_pen_handle);
void API_Pen_SetPenBrush(pen_handle, const_brush_handle);

// ----------------------------------------------------------------------------
// FontContext API
// ----------------------------------------------------------------------------

font_handle API_Font_CreateFontByFamily(api_handle, int32, double);
font_handle API_Font_CreateFontByFace(api_handle, const char16_type*, double);
font_handle API_Font_CloneFont(api_handle, const_font_handle);
api_bool API_Font_GetFontFace(const_font_handle, char16_type*, size_type*);
void API_Font_SetFontFace(font_handle, const char16_type*);
api_bool API_Font_GetFontExactMatch(const_font_handle);
int32 API_Font_GetFontPixelSize(const_font_handle);
void API_Font_SetFontPixelSize(font_handle, int32);
void API_Font_GetFontPointSize(const_font_handle, double*);
void API_Font_SetFontPointSize(font_handle, double);
api_bool API_Font_GetFontFixedPitch(const_font_handle);
void API_Font_SetFontFixedPitch(font_handle, api_bool);
api_bool API_Font_GetFontKerning(const_font_handle);
void API_Font_SetFontKerning(font_handle, api_bool);
int32 API_Font_GetFontStretchFactor(const_font_handle);
void API_Font_SetFontStretchFactor(font_handle, int32);
int32 API_Font_GetFontWeight(const_font_handle);
void API_Font_SetFontWeight(font_handle, int32);
api_bool API_Font_GetFontItalic(const_font_handle);
void API_Font_SetFontItalic(font_handle, api_bool);
api_bool API_Font_GetFontUnderline(const_font_handle);
void API_Font_SetFontUnderline(font_handle, api_bool);
api_bool API_Font_GetFontOverline(const_font_handle);
void API_Font_SetFontOverline(font_handle, api_bool);
api_bool API_Font_GetFontStrikeOut(const_font_handle);
void API_Font_SetFontStrikeOut(font_handle, api_bool);
int32 API_Font_GetFontAscent(const_font_handle);
int32 API_Font_GetFontDescent(const_font_handle);
int32 API_Font_GetFontHeight(const_font_handle);
int32 API_Font_GetFontLineSpacing(const_font_handle);
api_bool API_Font_GetFontCharDefined(const_font_handle, int32);
int32 API_Font_GetFontMaxWidth(const_font_handle);
int32 API_Font_GetStringPixelWidth(const_font_handle, const char16_type*);
int32 API_Font_GetCharPixelWidth(const_font_handle, int32);
void API_Font_GetStringPixelRect(const_font_handle, const char16_type*, int32*, int32*, int32*, int32*, uint32 flags);
api_bool API_Font_EnumerateFonts(font_enumeration_callback f, char16_type* fontFace, size_type* len, void* data, const char* writingSystem);
api_bool API_Font_EnumerateWritingSystems(font_enumeration_callback f, char16_type* wrSystem, size_type* len, void* data, const char16_type* font);
api_bool API_Font_EnumerateFontStyles(font_enumeration_callback f, char16_type* style, size_type* len, void* data, const char16_type* font);
api_bool API_Font_EnumerateOptimalFontPointSizes(font_size_enumeration_callback f, double* ptSize, void* data, const char16_type* font, const char16_type* style);
api_bool API_Font_GetFontScalable(const char16_type* font, const char16_type* style);
api_bool API_Font_GetNominalFontFixedPitch(const char16_type* font, const char16_type* style);
api_bool API_Font_GetNominalFontItalic(const char16_type* font, const char16_type* style);
int32 API_Font_GetNominalFontWeight(const char16_type* font, const char16_type* style);

// ----------------------------------------------------------------------------
// CursorContext API
// ----------------------------------------------------------------------------

cursor_handle API_Cursor_CreateCursor(api_handle, int32);
cursor_handle API_Cursor_CreateBitmapCursor(api_handle, const_bitmap_handle, int32, int32);
cursor_handle API_Cursor_CloneCursor(api_handle, const_cursor_handle);
void API_Cursor_GetCursorHotSpot(const_cursor_handle, int32*, int32*);

// ----------------------------------------------------------------------------
// SizerContext API
// ----------------------------------------------------------------------------

sizer_handle API_Sizer_CreateSizer(api_handle, api_bool vertical);
control_handle API_Sizer_GetSizerParentControl(const_sizer_handle); // returns client handle

   api_bool       (API_Sizer_GetSizerOrientation)( const_sizer_handle ); // true=vertical

   int32          (API_Sizer_GetSizerCount)( const_sizer_handle );

   int32          (API_Sizer_GetSizerIndex)( const_sizer_handle, const_sizer_handle );
   int32          (API_Sizer_GetSizerControlIndex)( const_sizer_handle, const_control_handle );

   void           (API_Sizer_InsertSizer)( sizer_handle, int32, sizer_handle, int32 );
   void           (API_Sizer_InsertSizerControl)( sizer_handle, int32, control_handle, int32, int32 );
   void           (API_Sizer_InsertSizerSpacing)( sizer_handle, int32, int32 );
   void           (API_Sizer_InsertSizerStretch)( sizer_handle, int32, int32 );

   void           (API_Sizer_RemoveSizer)( sizer_handle, sizer_handle );
   void           (API_Sizer_RemoveSizerControl)( sizer_handle, control_handle );

   void           (API_Sizer_SetSizerStretchFactor)( sizer_handle, sizer_handle, int32 );
   void           (API_Sizer_SetSizerControlStretchFactor)( sizer_handle, control_handle, int32 );

   void           (API_Sizer_SetSizerAlignment)( sizer_handle, sizer_handle, int32 );
   void           (API_Sizer_SetSizerControlAlignment)( sizer_handle, control_handle, int32 );

   int32          (API_Sizer_GetSizerMargin)( const_sizer_handle );
   void           (API_Sizer_SetSizerMargin)( sizer_handle, int32 );

   int32          (API_Sizer_GetSizerSpacing)( const_sizer_handle );
   void           (API_Sizer_SetSizerSpacing)( sizer_handle, int32 );

   api_bool       (API_Sizer_GetSizerDisplayPixelRatio)( const_sizer_handle, double* );
   api_bool       (API_Sizer_GetSizerResourcePixelRatio)( const_sizer_handle, double* );
   api_bool       (API_Sizer_GetSizerDevicePixelRatio)( const_sizer_handle, double* );

// ----------------------------------------------------------------------------
// GraphicsContext API
// ----------------------------------------------------------------------------

graphics_handle API_Graphics_CreateGraphics(api_handle);
api_bool API_Graphics_BeginControlPaint(graphics_handle, control_handle);
api_bool API_Graphics_BeginBitmapPaint(graphics_handle, bitmap_handle);
api_bool API_Graphics_BeginSVGPaint(graphics_handle, svg_handle);
void API_Graphics_EndPaint(graphics_handle);
api_bool API_Graphics_GetGraphicsStatus(const_graphics_handle);
api_bool API_Graphics_GetGraphicsTransformationEnabled(const_graphics_handle);
void API_Graphics_EnableGraphicsTransformation(graphics_handle, api_bool);
void API_Graphics_GetGraphicsTransformationMatrix(const_graphics_handle, double* m11, double* m12, double* m13, double* m21, double* m22, double* m23, double* m31, double* m32, double* m33);
void API_Graphics_SetGraphicsTransformationMatrix(graphics_handle, double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33);
void API_Graphics_MultiplyGraphicsTransformationMatrix(graphics_handle, double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33);
void API_Graphics_RotateGraphicsTransformation(graphics_handle, double);
void API_Graphics_ScaleGraphicsTransformation(graphics_handle, double, double);
void API_Graphics_TranslateGraphicsTransformation(graphics_handle, double, double);
void API_Graphics_ShearGraphicsTransformation(graphics_handle, double, double);
void API_Graphics_ResetGraphicsTransformation(graphics_handle);
void API_Graphics_TransformPoints(const_graphics_handle, double* xy, size_type n);
api_bool API_Graphics_GetGraphicsClippingEnabled(const_graphics_handle);
void API_Graphics_EnableGraphicsClipping(graphics_handle, api_bool);
void API_Graphics_GetGraphicsClipRect(const_graphics_handle, int32*, int32*, int32*, int32*);
void API_Graphics_SetGraphicsClipRect(graphics_handle, int32, int32, int32, int32);
void API_Graphics_GetGraphicsClipRectD(const_graphics_handle, double*, double*, double*, double*);
void API_Graphics_SetGraphicsClipRectD(graphics_handle, double, double, double, double);
api_bool API_Graphics_GetGraphicsAntialiasingEnabled(const_graphics_handle);
void API_Graphics_EnableGraphicsAntialiasing(graphics_handle, api_bool);
api_bool API_Graphics_GetGraphicsTextAntialiasingEnabled(const_graphics_handle);
void API_Graphics_EnableGraphicsTextAntialiasing(graphics_handle, api_bool);
api_bool API_Graphics_GetGraphicsSmoothInterpolationEnabled(const_graphics_handle);
void API_Graphics_EnableGraphicsSmoothInterpolation(graphics_handle, api_bool);
int32 API_Graphics_GetGraphicsCompositionOperator(const_graphics_handle);
void API_Graphics_SetGraphicsCompositionOperator(graphics_handle, int32);
void API_Graphics_GetGraphicsOpacity(const_graphics_handle, double*);
void API_Graphics_SetGraphicsOpacity(graphics_handle, double);
brush_handle API_Graphics_GetGraphicsBackgroundBrush(const_graphics_handle);
void API_Graphics_SetGraphicsBackgroundBrush(graphics_handle, const_brush_handle);
api_bool API_Graphics_GetGraphicsTransparentBackgroundEnabled(const_graphics_handle);
void API_Graphics_SetGraphicsTransparentBackground(graphics_handle, api_bool);
pen_handle API_Graphics_GetGraphicsPen(const_graphics_handle);
void API_Graphics_SetGraphicsPen(graphics_handle, const_pen_handle);
brush_handle API_Graphics_GetGraphicsBrush(const_graphics_handle);
void API_Graphics_SetGraphicsBrush(graphics_handle, const_brush_handle);
void API_Graphics_GetGraphicsBrushOrigin(const_graphics_handle, int32*, int32*);
void API_Graphics_SetGraphicsBrushOrigin(graphics_handle, int32, int32);
void API_Graphics_GetGraphicsBrushOriginD(const_graphics_handle, double*, double*);
void API_Graphics_SetGraphicsBrushOriginD(graphics_handle, double, double);
font_handle API_Graphics_GetGraphicsFont(const_graphics_handle);
void API_Graphics_SetGraphicsFont(graphics_handle, const_font_handle);
void API_Graphics_PushGraphicsState(graphics_handle);
void API_Graphics_PopGraphicsState(graphics_handle);
void API_Graphics_DrawPoint(graphics_handle, int32, int32);
void API_Graphics_DrawPointD(graphics_handle, double, double);
void API_Graphics_DrawLine(graphics_handle, int32, int32, int32, int32);
void API_Graphics_DrawLineD(graphics_handle, double, double, double, double);
void API_Graphics_DrawRect(graphics_handle, int32, int32, int32, int32);
void API_Graphics_StrokeRect(graphics_handle, int32, int32, int32, int32, const_pen_handle);
void API_Graphics_FillRect(graphics_handle, int32, int32, int32, int32, const_brush_handle);
void API_Graphics_DrawRectD(graphics_handle, double, double, double, double);
void API_Graphics_StrokeRectD(graphics_handle, double, double, double, double, const_pen_handle);
void API_Graphics_FillRectD(graphics_handle, double, double, double, double, const_brush_handle);
void API_Graphics_DrawRoundedRect(graphics_handle, int32, int32, int32, int32, double, double);
void API_Graphics_StrokeRoundedRect(graphics_handle, int32, int32, int32, int32, double, double, const_pen_handle);
void API_Graphics_FillRoundedRect(graphics_handle, int32, int32, int32, int32, double, double, const_brush_handle);
void API_Graphics_DrawRoundedRectD(graphics_handle, double, double, double, double, double, double);
void API_Graphics_StrokeRoundedRectD(graphics_handle, double, double, double, double, double, double, const_pen_handle);
void API_Graphics_FillRoundedRectD(graphics_handle, double, double, double, double, double, double, const_brush_handle);
void API_Graphics_DrawEllipse(graphics_handle, int32, int32, int32, int32);
void API_Graphics_StrokeEllipse(graphics_handle, int32, int32, int32, int32, const_pen_handle);
void API_Graphics_FillEllipse(graphics_handle, int32, int32, int32, int32, const_brush_handle);
void API_Graphics_DrawEllipseD(graphics_handle, double, double, double, double);
void API_Graphics_StrokeEllipseD(graphics_handle, double, double, double, double, const_pen_handle);
void API_Graphics_FillEllipseD(graphics_handle, double, double, double, double, const_brush_handle);
void API_Graphics_DrawPolygon(graphics_handle, const int32*, size_type, int32);
void API_Graphics_StrokePolygon(graphics_handle, const int32*, size_type, int32, const_pen_handle);
void API_Graphics_FillPolygon(graphics_handle, const int32*, size_type, int32, const_brush_handle);
void API_Graphics_DrawPolygonD(graphics_handle, const double*, size_type, int32);
void API_Graphics_StrokePolygonD(graphics_handle, const double*, size_type, int32, const_pen_handle);
void API_Graphics_FillPolygonD(graphics_handle, const double*, size_type, int32, const_brush_handle);
void API_Graphics_DrawPolyline(graphics_handle, const int32*, size_type);
void API_Graphics_DrawPolylineD(graphics_handle, const double*, size_type);
void API_Graphics_DrawArc(graphics_handle, int32, int32, int32, int32, double, double);
void API_Graphics_DrawArcD(graphics_handle, double, double, double, double, double, double);
void API_Graphics_DrawChord(graphics_handle, int32, int32, int32, int32, double, double);
void API_Graphics_StrokeChord(graphics_handle, int32, int32, int32, int32, double, double, const_pen_handle);
void API_Graphics_FillChord(graphics_handle, int32, int32, int32, int32, double, double, const_brush_handle);
void API_Graphics_DrawChordD(graphics_handle, double, double, double, double, double, double);
void API_Graphics_StrokeChordD(graphics_handle, double, double, double, double, double, double, const_pen_handle);
void API_Graphics_FillChordD(graphics_handle, double, double, double, double, double, double, const_brush_handle);
void API_Graphics_DrawPie(graphics_handle, int32, int32, int32, int32, double, double);
void API_Graphics_StrokePie(graphics_handle, int32, int32, int32, int32, double, double, const_pen_handle);
void API_Graphics_FillPie(graphics_handle, int32, int32, int32, int32, double, double, const_brush_handle);
void API_Graphics_DrawPieD(graphics_handle, double, double, double, double, double, double);
void API_Graphics_StrokePieD(graphics_handle, double, double, double, double, double, double, const_pen_handle);
void API_Graphics_FillPieD(graphics_handle, double, double, double, double, double, double, const_brush_handle);
void API_Graphics_DrawBitmap(graphics_handle, int32, int32, const_bitmap_handle);
void API_Graphics_DrawBitmapD(graphics_handle, double, double, const_bitmap_handle);
void API_Graphics_DrawBitmapRect(graphics_handle, int32, int32, const_bitmap_handle, int32, int32, int32, int32);
void API_Graphics_DrawBitmapRectD(graphics_handle, double, double, const_bitmap_handle, double, double, double, double);
void API_Graphics_DrawScaledBitmap(graphics_handle, int32, int32, int32, int32, const_bitmap_handle);
void API_Graphics_DrawScaledBitmapD(graphics_handle, double, double, double, double, const_bitmap_handle);
void API_Graphics_DrawScaledBitmapRect(graphics_handle, int32, int32, int32, int32, const_bitmap_handle, int32, int32, int32, int32);
void API_Graphics_DrawScaledBitmapRectD(graphics_handle, double, double, double, double, const_bitmap_handle, double, double, double, double);
void API_Graphics_DrawTiledBitmap(graphics_handle, int32, int32, int32, int32, const_bitmap_handle, int32, int32);
void API_Graphics_DrawTiledBitmapD(graphics_handle, double, double, double, double, const_bitmap_handle, double, double);
void API_Graphics_DrawText(graphics_handle, int32, int32, const char16_type*);
void API_Graphics_DrawTextD(graphics_handle, double, double, const char16_type*);
void API_Graphics_DrawTextRect(graphics_handle, int32, int32, int32, int32, const char16_type*, int32);
void API_Graphics_DrawTextRectD(graphics_handle, double, double, double, double, const char16_type*, int32);
void API_Graphics_GetTextRect(graphics_handle, int32, int32, int32, int32, const char16_type*, int32, int32*, int32*, int32*, int32*);
void API_Graphics_GetTextRectD(graphics_handle, double, double, double, double, const char16_type*, int32, double*, double*, double*, double*);

// ----------------------------------------------------------------------------
// RealTimePreviewContext API
// ----------------------------------------------------------------------------

api_bool API_RealTimePreview_SetRealTimePreviewOwner(interface_handle, uint32 flags);
api_bool API_RealTimePreview_IsRealTimePreviewUpdating();
void API_RealTimePreview_UpdateRealTimePreview();
void API_RealTimePreview_ShowRealTimePreviewProgressDialog(const char16_type* title, const char16_type* text, size_type total, uint32 flags);
void API_RealTimePreview_CloseRealTimePreviewProgressDialog();
api_bool API_RealTimePreview_IsRealTimePreviewProgressDialogVisible();
void API_RealTimePreview_SetRealTimePreviewProgressCount(size_type newCount, uint32 flags);
void API_RealTimePreview_SetRealTimePreviewProgressText(const char16_type* text, uint32 flags);

// ----------------------------------------------------------------------------
// NumericalContext API
// ----------------------------------------------------------------------------

api_bool API_Numerical_GaussJordanInPlaceF(float** A, float** B, int32 rows, int32 cols);
api_bool API_Numerical_GaussJordanInPlaceD(double** A, double** B, int32 rows, int32 cols);
api_bool API_Numerical_SVDInPlaceF(float** A, float* W, float** V, int32 rows, int32 cols);
api_bool API_Numerical_SVDInPlaceD(double** A, double* W, double** V, int32 rows, int32 cols);
api_enum API_Numerical_LinearFitF(double* a, double* b, double* adev, const float* fx, const float* fy, size_type n, api_bool (*callback)( void* ), void*);
api_enum API_Numerical_LinearFitD(double* a, double* b, double* adev, const double* fx, const double* fy, size_type n, api_bool (*callback)( void* ), void*);
api_bool API_Numerical_CubicSplineGenerateF(float* dy2, const float* fx, const float* fy, float dy1, float dyn, int32 n);
api_bool API_Numerical_CubicSplineGenerateD(double* dy2, const double* fx, const double* fy, double dy1, double dyn, int32 n);
api_bool API_Numerical_NaturalCubicSplineGenerateF(float* dy2, const float* fx, const float* fy, int32 n);
api_bool API_Numerical_NaturalCubicSplineGenerateD(double* dy2, const double* fx, const double* fy, int32 n);
api_bool API_Numerical_CubicSplineInterpolateF(float* y, const float* fx, const float* fy, const float* dy2, int32 n, double x, int32* k);
api_bool API_Numerical_CubicSplineInterpolateD(double* y, const double* fx, const double* fy, const double* dy2, int32 n, double x, int32* k);
api_bool API_Numerical_NaturalGridCubicSplineGenerateF(float* dy2, const float* fy, int32 n);
api_bool API_Numerical_NaturalGridCubicSplineGenerateD(double* dy2, const double* fy, int32 n);
api_bool API_Numerical_NaturalGridCubicSplineGenerateUI8(float* dy2, const uint8* fy, int32 n);
api_bool API_Numerical_NaturalGridCubicSplineGenerateUI16(float* dy2, const uint16* fy, int32 n);
api_bool API_Numerical_NaturalGridCubicSplineGenerateUI32(double* dy2, const uint32* fy, int32 n);
api_bool API_Numerical_NaturalGridCubicSplineInterpolateF(float* y, const float* fy, const float* dy2, int32 n, double x);
api_bool API_Numerical_NaturalGridCubicSplineInterpolateD(double* y, const double* fy, const double* dy2, int32 n, double x);
api_bool API_Numerical_NaturalGridCubicSplineInterpolateUI8(float* y, const uint8* fy, const float* dy2, int32 n, double x);
api_bool API_Numerical_NaturalGridCubicSplineInterpolateUI16(float* y, const uint16* fy, const float* dy2, int32 n, double x);
api_bool API_Numerical_NaturalGridCubicSplineInterpolateUI32(double* y, const uint32* fy, const double* dy2, int32 n, double x);
api_bool API_Numerical_SurfaceSplineCreateF(sspline_handle* hSS, int32 rbf, double e2, api_bool polynomial, const float* x, const float* y, const float* z, int32 n, int32 m, float rho, const float* w);
api_bool API_Numerical_SurfaceSplineCreateD(sspline_handle* hSS, int32 rbf, double e2, api_bool polynomial, const double* x, const double* y, const double* z, int32 n, int32 m, float rho, const float* w);
api_bool API_Numerical_SurfaceSplineEvaluate(const_sspline_handle hSS, double* z, double x, double y);
api_bool API_Numerical_SurfaceSplineEvaluateVectorF(const_sspline_handle hSS, float* z, const float *x, const float *y, double x0, double y0, double r, size_type n);
api_bool API_Numerical_SurfaceSplineEvaluateVectorD(const_sspline_handle hSS, double* z, const double *x, const double *y, double x0, double y0, double r, size_type n);
api_bool API_Numerical_SurfaceSplineDestroy(sspline_handle hSS); // ### The following function returns a null-terminated string allocated by the caller module.
   char*          (API_Numerical_SurfaceSplineSerialize)( api_handle hModule, const_sspline_handle hSS, uint32 flags );
   api_bool       (API_Numerical_SurfaceSplineDeserialize)( sspline_handle* hSS, const char* data, size_type len, uint32 flags );

   api_bool       (API_Numerical_SurfaceSplineDuplicate)( sspline_handle* hSS1, const_sspline_handle hSS );

   /*
    * Fast Fourier Transforms (one-dimensional)
    */
   size_type      (API_Numerical_FFTComplexOptimizedLengthF)( size_type n );
   size_type      (API_Numerical_FFTComplexOptimizedLengthD)( size_type n );

   size_type      (API_Numerical_FFTRealOptimizedLengthF)( size_type n );
   size_type      (API_Numerical_FFTRealOptimizedLengthD)( size_type n );

   fft_handle     (API_Numerical_FFTCreateComplexTransformF)( size_type n );
   fft_handle     (API_Numerical_FFTCreateComplexTransformD)( size_type n );

   fft_handle     (API_Numerical_FFTCreateComplexInverseTransformF)( size_type n );
   fft_handle     (API_Numerical_FFTCreateComplexInverseTransformD)( size_type n );

   fft_handle     (API_Numerical_FFTCreateRealTransformF)( size_type n );
   fft_handle     (API_Numerical_FFTCreateRealTransformD)( size_type n );

   fft_handle     (API_Numerical_FFTCreateRealInverseTransformF)( size_type n );
   fft_handle     (API_Numerical_FFTCreateRealInverseTransformD)( size_type n );

   api_bool       (API_Numerical_FFTDestroyTransform)( fft_handle hFFT );

   api_bool       (API_Numerical_FFTComplexTransformF)( fft_handle hFFT, void* y, const void* x ); // void* = fcomplex*
   api_bool       (API_Numerical_FFTComplexTransformD)( fft_handle hFFT, void* y, const void* x ); // void* = dcomplex*

   api_bool       (API_Numerical_FFTComplexInverseTransformF)( fft_handle hFFT, void* y, const void* x ); // void* = fcomplex*
   api_bool       (API_Numerical_FFTComplexInverseTransformD)( fft_handle hFFT, void* y, const void* x ); // void* = dcomplex*

   api_bool       (API_Numerical_FFTRealTransformF)( fft_handle hFFT, void* y, const float* x );   // void* = fcomplex*
   api_bool       (API_Numerical_FFTRealTransformD)( fft_handle hFFT, void* y, const double* x );  // void* = dcomplex*

   api_bool       (API_Numerical_FFTRealInverseTransformF)( fft_handle hFFT, float* x, const void* y );  // void* = fcomplex*
   api_bool       (API_Numerical_FFTRealInverseTransformD)( fft_handle hFFT, double* x, const void* y ); // void* = dcomplex*

// ----------------------------------------------------------------------------
// GPUContext API
// ----------------------------------------------------------------------------

api_bool API_GPU_InitCUDARuntime(api_handle, uint32 /*flags*/);
api_bool API_GPU_IsCUDADeviceAvailable(api_handle);
api_bool API_GPU_EnumerateCUDADevices(api_handle, pcl::cuda_device_enumeration_callback, void* deviceProps/*cudaDeviceProp*/, size_type structSize, void* data);
cuda_device_handle API_GPU_GetCUDASelectedDevice(api_handle);
api_bool API_GPU_GetCUDADeviceProperties(api_handle, cuda_device_handle, void* deviceProps, size_type structSize);
size_type API_GPU_GetCUDADeviceTotalGlobalMem(api_handle, cuda_device_handle);
int32 API_GPU_GetCUDADeviceMaxThreadsPerBlock(api_handle, cuda_device_handle);
size_type API_GPU_GetCUDADeviceSharedMemoryPerBlock(api_handle, cuda_device_handle);

// ----------------------------------------------------------------------------
// SharedImageContext API
// ----------------------------------------------------------------------------

image_handle API_SharedImage_CreateImage(uint32 w, uint32 h, uint32 n, uint32 nbits, api_bool flt, uint32 cs, void*);
void *API_SharedImage_GetImageOwner(const_image_handle);
api_bool API_SharedImage_GetImageRefCount(const_image_handle, uint32*);
api_bool API_SharedImage_IsValidImageHandle(const_image_handle);
api_bool API_SharedImage_AttachToImage(image_handle, void*);
api_bool API_SharedImage_DetachFromImage(image_handle, void*);
api_bool API_SharedImage_GetImageFormat(const_image_handle, uint32* nbits, api_bool* flt);
api_bool API_SharedImage_GetImageGeometry(const_image_handle, uint32* w, uint32* h, uint32* n);
api_bool API_SharedImage_SetImageGeometry(image_handle, uint32 w, uint32 h, uint32 n);
api_bool API_SharedImage_GetImageColorSpace(const_image_handle, uint32* cs);
api_bool API_SharedImage_SetImageColorSpace(image_handle, uint32 cs);
api_bool API_SharedImage_GetImageRGBWS(const_image_handle, api_RGBWS*);
api_bool API_SharedImage_SetImageRGBWS(image_handle, const api_RGBWS*); // ### must be set through ImageWindow

   api_bool       (API_SharedImage_GetImagePixelData)( image_handle, void*** );
   api_bool       (API_SharedImage_SetImagePixelData)( image_handle, void** );

// ----------------------------------------------------------------------------
// ViewContext API
// ----------------------------------------------------------------------------

view_handle API_View_GetViewById(const char* fullId);
void API_View_EnumerateViews(pcl::view_enumeration_callback, void*, api_bool includeMainViews, api_bool includePreviews);
api_bool API_View_IsPreview(const_view_handle);
api_bool API_View_IsVolatilePreview(const_view_handle);
api_bool API_View_IsStoredPreview(const_view_handle);
window_handle API_View_GetViewParentWindow(const_view_handle);
api_bool API_View_GetViewId(const_view_handle, char*, size_type*);
api_bool API_View_GetViewFullId(const_view_handle, char*, size_type*);
api_bool API_View_SetViewId(view_handle, const char*);
void API_View_GetViewLocks(const_view_handle, api_bool*, api_bool*);
void API_View_LockView(view_handle, api_bool, api_bool, api_bool);
void API_View_UnlockView(view_handle, api_bool, api_bool, api_bool);
api_bool API_View_IsViewDynamicTarget(const_view_handle);
void API_View_AddViewToDynamicTargets(view_handle);
void API_View_RemoveViewFromDynamicTargets(view_handle);
image_handle API_View_GetViewImage(view_handle);
api_bool API_View_IsViewColorImage(const_view_handle);
api_bool API_View_GetViewDimensions(const_view_handle, int32*, int32*);
api_bool API_View_GetViewScreenTransferFunctions(const_view_handle, double* m, double* c0, double* c1, double* r0, double* r1);
api_bool API_View_SetViewScreenTransferFunctions(view_handle, const double* m, const double* c0, const double* c1, const double* r0, const double* r1, api_bool);
api_bool API_View_DestroyViewScreenTransferFunctions(view_handle, api_bool);
api_bool API_View_GetViewScreenTransferFunctionsEnabled(view_handle);
void API_View_SetViewScreenTransferFunctionsEnabled(view_handle, api_bool, api_bool);
api_bool API_View_IsReservedViewPropertyId(const char* id);
api_bool API_View_EnumerateViewProperties(const_view_handle, pcl::property_enumeration_callback, char*, size_type*, void*);
api_bool API_View_GetViewPropertyValue(api_handle hModule, const_view_handle, const char* id, api_property_value*);
api_bool API_View_GetViewPropertyAttributes(api_handle hModule, const_view_handle, const char* id, uint32* flags, uint64* type);
api_bool API_View_SetViewPropertyValue(api_handle hModule, view_handle, const char* id, const api_property_value*, uint32 flags, api_bool notify);
api_bool API_View_SetViewPropertyAttributes(api_handle hModule, view_handle, const char* id, uint32 flags, api_bool notify);
api_bool API_View_GetViewPropertyExists(api_handle hModule, const_view_handle, const char* id, uint64* type);
api_bool API_View_DeleteViewProperty(api_handle hModule, view_handle, const char* id, api_bool notify);
api_bool API_View_ComputeViewProperty(api_handle hModule, view_handle, const char* id, api_bool notify, api_property_value*);

// ----------------------------------------------------------------------------
// ImageWindowContext API
// ----------------------------------------------------------------------------

window_handle API_ImageWindow_CreateImageWindow(int32 width, int32 height, int32 numberOfChannels, int32 bitsPerSample, api_bool floatSample, api_bool color, api_bool initialProcessing, const char* id);
api_bool API_ImageWindow_LoadImageWindows(const char16_type* url, const char* id, const char* hints, api_bool asACopy, api_bool allowMessages, pcl::window_enumeration_callback, void*);
api_bool API_ImageWindow_CloseImageWindow(window_handle, api_bool force);
window_handle API_ImageWindow_GetImageWindowById(const char*);
window_handle API_ImageWindow_GetImageWindowByFilePath(const char16_type*);
window_handle API_ImageWindow_GetActiveImageWindow();
void API_ImageWindow_EnumerateImageWindows(pcl::window_enumeration_callback, void*, api_bool includeIconic);
void API_ImageWindow_EnumeratePreviews(const_window_handle, pcl::view_enumeration_callback, void*);
api_bool API_ImageWindow_GetImageWindowNewFlag(const_window_handle);
api_bool API_ImageWindow_GetImageWindowCopyFlag(const_window_handle);
api_bool API_ImageWindow_GetImageWindowFileURL(const_window_handle, char16_type*, size_type*);
api_bool API_ImageWindow_GetImageWindowFilePath(const_window_handle, char16_type*, size_type*);
api_bool API_ImageWindow_GetImageWindowFileInfo(const_window_handle, api_image_file_info*);
size_type API_ImageWindow_GetImageWindowModifyCount(const_window_handle);
view_handle API_ImageWindow_GetImageWindowMainView(const_window_handle);
view_handle API_ImageWindow_GetImageWindowCurrentView(const_window_handle);
void API_ImageWindow_SetImageWindowCurrentView(window_handle, view_handle);
int32 API_ImageWindow_GetImageType(const_window_handle);
api_bool API_ImageWindow_SetImageType(window_handle, int32 imageType, api_bool notify);
void API_ImageWindow_PurgeImageWindowProperties(window_handle);
api_bool API_ImageWindow_ValidateImageWindowView(const_window_handle, const_view_handle);
int32 API_ImageWindow_GetPreviewCount(const_window_handle);
view_handle API_ImageWindow_GetPreviewById(const_window_handle, const char*);
view_handle API_ImageWindow_GetSelectedPreview(const_window_handle);
void API_ImageWindow_SelectPreview(window_handle, view_handle);
view_handle API_ImageWindow_CreatePreview(window_handle, int32, int32, int32, int32, const char*);
void API_ImageWindow_ModifyPreview(window_handle, const char*, int32, int32, int32, int32, const char*);
void API_ImageWindow_GetPreviewRect(const_window_handle, const char*, int32*, int32*, int32*, int32*);
void API_ImageWindow_DeletePreview(window_handle, const char*);
void API_ImageWindow_DeletePreviews(window_handle);
window_handle API_ImageWindow_GetImageWindowMask(const_window_handle, api_bool* inverted);
void API_ImageWindow_SetImageWindowMask(window_handle, window_handle, api_bool inverted);
api_bool API_ImageWindow_GetImageWindowMaskEnabled(const_window_handle);
void API_ImageWindow_SetImageWindowMaskEnabled(window_handle, api_bool);
api_bool API_ImageWindow_GetImageWindowMaskVisible(const_window_handle);
void API_ImageWindow_SetImageWindowMaskVisible(window_handle, api_bool);
api_bool API_ImageWindow_ValidateImageWindowMask(const_window_handle, const_window_handle);
int32 API_ImageWindow_GetMaskReferenceCount(const_window_handle);
void API_ImageWindow_RemoveImageWindowMaskReferences(window_handle);
void API_ImageWindow_UpdateImageWindowMaskReferences(window_handle);
void API_ImageWindow_GetImageWindowSampleFormat(const_window_handle, uint32* nbits, api_bool* flt);
void API_ImageWindow_SetImageWindowSampleFormat(window_handle, uint32 nbits, api_bool flt);
void API_ImageWindow_GetImageWindowRGBWS(const_window_handle, api_RGBWS*);
void API_ImageWindow_SetImageWindowRGBWS(window_handle, const api_RGBWS*);
api_bool API_ImageWindow_GetImageWindowGlobalRGBWS(const_window_handle);
void API_ImageWindow_SetImageWindowGlobalRGBWS(window_handle);
void API_ImageWindow_GetGlobalRGBWS(api_RGBWS*);
void API_ImageWindow_SetGlobalRGBWS(const api_RGBWS*);
void API_ImageWindow_GetImageWindowCMEnabled(const_window_handle, api_bool* enableCM, api_bool* proofing, api_bool* gamutCheck);
void API_ImageWindow_SetImageWindowCMEnabled(window_handle, api_bool enableCM, api_bool proofing, api_bool gamutCheck);
uint32 API_ImageWindow_GetImageWindowICCProfileLength(const_window_handle);
void API_ImageWindow_GetImageWindowICCProfile(const_window_handle, void*);
void API_ImageWindow_SetImageWindowICCProfile(window_handle, const void*);
void API_ImageWindow_LoadImageWindowICCProfile(window_handle, const char16_type*);
void API_ImageWindow_DeleteImageWindowICCProfile(window_handle);
int32 API_ImageWindow_GetImageWindowKeywordCount(const_window_handle);
void API_ImageWindow_GetImageWindowKeyword(const_window_handle, int32, char*, size_type, char*, size_type, char*, size_type);
void API_ImageWindow_AddImageWindowKeyword(window_handle, const char*, const char*, const char*);
void API_ImageWindow_ResetImageWindowKeywords(window_handle);
api_bool API_ImageWindow_GetImageWindowHasAstrometricSolution(const_window_handle);
api_bool API_ImageWindow_RegenerateImageWindowAstrometricSolution(window_handle, api_bool, api_bool);
api_bool API_ImageWindow_CopyImageWindowAstrometricSolution(window_handle, const_window_handle, api_bool);
void API_ImageWindow_ClearImageWindowAstrometricSolution(window_handle, api_bool);
void API_ImageWindow_UpdateImageWindowAstrometryMetadata(window_handle, api_bool);
api_bool API_ImageWindow_ImageToCelestial(const_window_handle, double* x, double* y, api_bool rawRA);
api_bool API_ImageWindow_CelestialToImage(const_window_handle, double* ra, double* dec);
void API_ImageWindow_GetImageWindowResolution(const_window_handle, double*, double*, api_bool*);
void API_ImageWindow_SetImageWindowResolution(window_handle, double, double, api_bool);
void API_ImageWindow_GetDefaultResolution(double*, double*, api_bool*);
void API_ImageWindow_GetDefaultICCProfileEmbedding(api_bool* rgb, api_bool* grayscale);
api_bool API_ImageWindow_GetDefaultThumbnailEmbedding();
api_bool API_ImageWindow_GetDefaultPropertiesEmbedding();
api_bool API_ImageWindow_GetSwapDirectory(int32, char16_type*, size_type*);
api_bool API_ImageWindow_SetSwapDirectories(const char16_type**, int32);
int32 API_ImageWindow_GetCursorTolerance();
int32 API_ImageWindow_GetImageWindowTransparencyMode(const_window_handle, uint32*);
void API_ImageWindow_SetImageWindowTransparencyMode(window_handle, int32, uint32);
int32 API_ImageWindow_GetTransparencyBackgroundBrush(uint32* fgColor, uint32* bgColor);
void API_ImageWindow_SetTransparencyBackgroundBrush(int32 brush, uint32 fgColor, uint32 bgColor);
int32 API_ImageWindow_GetImageWindowMode();
void API_ImageWindow_SetImageWindowMode(int32);
int32 API_ImageWindow_GetImageWindowDisplayChannel(const_window_handle);
void API_ImageWindow_SetImageWindowDisplayChannel(window_handle, int32);
int32 API_ImageWindow_GetImageWindowMaskMode(const_window_handle);
void API_ImageWindow_SetImageWindowMaskMode(window_handle, int32);
void API_ImageWindow_FitImageWindow(window_handle);
void API_ImageWindow_ZoomImageWindowToFit(window_handle, api_bool, api_bool, api_bool, api_bool);
int32 API_ImageWindow_GetImageWindowZoomFactor(const_window_handle);
void API_ImageWindow_SetImageWindowZoomFactor(window_handle, int32);
void API_ImageWindow_UpdateImageWindowViewport(window_handle);
void API_ImageWindow_RegenerateImageWindowViewport(window_handle);
void API_ImageWindow_SetImageWindowViewport(window_handle, double cx, double cy, int32 zoom);
void API_ImageWindow_GetImageWindowViewportSize(const_window_handle, int32*, int32*);
void API_ImageWindow_GetImageWindowViewportOrigin(const_window_handle, int32*, int32*);
void API_ImageWindow_GetImageWindowViewportPosition(const_window_handle, int32*, int32*);
void API_ImageWindow_SetImageWindowViewportPosition(window_handle, int32, int32);
void API_ImageWindow_GetImageWindowVisibleViewportRect(const_window_handle, int32*, int32*, int32*, int32*);
api_bool API_ImageWindow_GetImageWindowVisible(const_window_handle);
void API_ImageWindow_SetImageWindowVisible(window_handle, api_bool);
api_bool API_ImageWindow_GetImageWindowIconic(const_window_handle);
void API_ImageWindow_SetImageWindowIconic(window_handle, api_bool);
void API_ImageWindow_BringImageWindowToFront(window_handle);
void API_ImageWindow_SendImageWindowToBack(window_handle);
interface_handle API_ImageWindow_GetActiveDynamicInterface();
api_bool API_ImageWindow_TerminateDynamicSession(api_bool closeInterface);
void API_ImageWindow_SetDynamicCursorXPM(window_handle, const char**, int32 hx, int32 hy); // ### deprecated
   void           (API_ImageWindow_SetDynamicCursor)( window_handle, const_bitmap_handle, int32 hx, int32 hy );

   bitmap_handle  (API_ImageWindow_GetDynamicCursorBitmap)( const_window_handle );
   void           (API_ImageWindow_GetDynamicCursorHotSpot)( const_window_handle, int32* hx, int32* hy );

   void           (API_ImageWindow_ViewportToImageArray)( const_window_handle, int32*, size_type n );
   void           (API_ImageWindow_ViewportToImageArrayD)( const_window_handle, double*, size_type n );

   void           (API_ImageWindow_ViewportToImage)( const_window_handle, int32* x, int32* y );
   void           (API_ImageWindow_ViewportToImageD)( const_window_handle, double* x, double* y );

   void           (API_ImageWindow_ImageToViewportArray)( const_window_handle, int32*, size_type n );
   void           (API_ImageWindow_ImageToViewportArrayD)( const_window_handle, double*, size_type n );

   void           (API_ImageWindow_ImageToViewport)( const_window_handle, int32* x, int32* y );
   void           (API_ImageWindow_ImageToViewportD)( const_window_handle, double* x, double* y );

   void           (API_ImageWindow_ViewportScalarToImageArray)( const_window_handle, int32*, size_type n );
   void           (API_ImageWindow_ViewportScalarToImageArrayD)( const_window_handle, double*, size_type n );

   void           (API_ImageWindow_ViewportScalarToImage)( const_window_handle, int32* );
   void           (API_ImageWindow_ViewportScalarToImageD)( const_window_handle, double* );

   void           (API_ImageWindow_ImageScalarToViewportArray)( const_window_handle, int32*, size_type n );
   void           (API_ImageWindow_ImageScalarToViewportArrayD)( const_window_handle, double*, size_type n );

   void           (API_ImageWindow_ImageScalarToViewport)( const_window_handle, int32* );
   void           (API_ImageWindow_ImageScalarToViewportD)( const_window_handle, double* );

   void           (API_ImageWindow_ViewportToGlobal)( const_window_handle, int32* x, int32* y );
   void           (API_ImageWindow_GlobalToViewport)( const_window_handle, int32* x, int32* y );

   void           (API_ImageWindow_UpdateViewportRect)( window_handle, int32, int32, int32, int32 );
   void           (API_ImageWindow_UpdateImageRect)( window_handle, double, double, double, double );

   void           (API_ImageWindow_RegenerateViewportRect)( window_handle, int32, int32, int32, int32 );
   void           (API_ImageWindow_RegenerateImageRect)( window_handle, double, double, double, double );

   void           (API_ImageWindow_CommitViewportUpdates)( window_handle );

   api_bool       (API_ImageWindow_GetViewportUpdateRect)( const_window_handle, int32*, int32*, int32*, int32* );

   void           (API_ImageWindow_BeginViewportSelection)( window_handle, int32 x, int32 y, uint32 flags );
   void           (API_ImageWindow_ModifyViewportSelection)( window_handle, int32 x, int32 y, uint32 flags );
   void           (API_ImageWindow_UpdateViewportSelection)( window_handle );
   void           (API_ImageWindow_CancelViewportSelection)( window_handle );
   void           (API_ImageWindow_EndViewportSelection)( window_handle );
   api_bool       (API_ImageWindow_GetViewportSelection)( const_window_handle, int32* x0, int32* y0, int32* x1, int32* y1, uint32* flags );

   bitmap_handle  (API_ImageWindow_GetViewportBitmap)( api_handle, const_window_handle, int32 x0, int32 y0, int32 x1, int32 y1, uint32 flags );

   api_bool       (API_ImageWindow_GetImageWindowDisplayPixelRatio)( const_window_handle, double* );
   api_bool       (API_ImageWindow_GetImageWindowResourcePixelRatio)( const_window_handle, double* );
   api_bool       (API_ImageWindow_GetImageWindowDevicePixelRatio)( const_window_handle, double* );

// ----------------------------------------------------------------------------
// ImageViewContext API
// ----------------------------------------------------------------------------

control_handle API_ImageView_CreateImageView(api_handle hModule, api_handle hClient, control_handle hParent, uint32 flags, int32 width, int32 height, int32 numberOfChannels, int32 bitsPerSample, api_bool floatSample, api_bool color);
control_handle API_ImageView_CreateImageViewViewport(control_handle, api_handle hClient);
image_handle API_ImageView_GetImageViewImage(const_control_handle);
api_bool API_ImageView_IsImageViewColorImage(const_control_handle);
api_bool API_ImageView_GetImageViewImageGeometry(const_control_handle hView, int32*, int32*, int32*);
api_bool API_ImageView_GetImageViewSampleFormat(const_control_handle, int32* nbits, api_bool* flt);
void API_ImageView_SetImageViewSampleFormat(control_handle, int32 nbits, api_bool flt);
void API_ImageView_GetImageViewRGBWS(const_control_handle, api_RGBWS*);
void API_ImageView_SetImageViewRGBWS(control_handle, const api_RGBWS*);
void API_ImageView_GetImageViewCMEnabled(const_control_handle, api_bool* enableCM, api_bool* proofing, api_bool* gamutCheck);
void API_ImageView_SetImageViewCMEnabled(control_handle, api_bool enableCM, api_bool proofing, api_bool gamutCheck);
uint32 API_ImageView_GetImageViewICCProfileLength(const_control_handle);
void API_ImageView_GetImageViewICCProfile(const_control_handle, void*);
void API_ImageView_SetImageViewICCProfile(control_handle, const void*);
void API_ImageView_LoadImageViewICCProfile(control_handle, const char16_type*);
void API_ImageView_DeleteImageViewICCProfile(control_handle);
api_bool API_ImageView_SetImageViewScrollEventRoutine(control_handle, api_handle, pcl::range_event_routine);
int32 API_ImageView_GetImageViewMode(const_control_handle);
void API_ImageView_SetImageViewMode(control_handle, int32);
int32 API_ImageView_GetImageViewDisplayChannel(const_control_handle);
void API_ImageView_SetImageViewDisplayChannel(control_handle, int32);
int32 API_ImageView_GetImageViewZoomFactor(const_control_handle);
void API_ImageView_SetImageViewZoomFactor(control_handle, int32);
int32 API_ImageView_GetImageViewTransparencyMode(const_control_handle, uint32*);
void API_ImageView_SetImageViewTransparencyMode(control_handle, int32, uint32);
void API_ImageView_UpdateImageViewViewport(control_handle);
void API_ImageView_RegenerateImageViewViewport(control_handle);
void API_ImageView_SetImageViewViewport(control_handle, double cx, double cy, int32 zoom);
void API_ImageView_GetImageViewViewportSize(const_control_handle, int32*, int32*);
void API_ImageView_GetImageViewViewportOrigin(const_control_handle, int32*, int32*);
void API_ImageView_GetImageViewViewportPosition(const_control_handle, int32*, int32*);
void API_ImageView_SetImageViewViewportPosition(control_handle, int32, int32);
void API_ImageView_GetImageViewVisibleViewportRect(const_control_handle, int32*, int32*, int32*, int32*);
void API_ImageView_ViewportToImageArray(const_control_handle, int32*, size_type n);
void API_ImageView_ViewportToImageArrayD(const_control_handle, double*, size_type n);
void API_ImageView_ViewportToImage(const_control_handle, int32* x, int32* y);
void API_ImageView_ViewportToImageD(const_control_handle, double* x, double* y);
void API_ImageView_ImageToViewportArray(const_control_handle, int32*, size_type n);
void API_ImageView_ImageToViewportArrayD(const_control_handle, double*, size_type n);
void API_ImageView_ImageToViewport(const_control_handle, int32* x, int32* y);
void API_ImageView_ImageToViewportD(const_control_handle, double* x, double* y);
void API_ImageView_ViewportScalarToImageArray(const_control_handle, int32*, size_type n);
void API_ImageView_ViewportScalarToImageArrayD(const_control_handle, double*, size_type n);
void API_ImageView_ViewportScalarToImage(const_control_handle, int32*);
void API_ImageView_ViewportScalarToImageD(const_control_handle, double*);
void API_ImageView_ImageScalarToViewportArray(const_control_handle, int32*, size_type n);
void API_ImageView_ImageScalarToViewportArrayD(const_control_handle, double*, size_type n);
void API_ImageView_ImageScalarToViewport(const_control_handle, int32*);
void API_ImageView_ImageScalarToViewportD(const_control_handle, double*);
void API_ImageView_ViewportToGlobal(const_control_handle, int32* x, int32* y);
void API_ImageView_GlobalToViewport(const_control_handle, int32* x, int32* y);
void API_ImageView_UpdateViewportRect(control_handle, int32, int32, int32, int32);
void API_ImageView_UpdateImageRect(control_handle, double, double, double, double);
void API_ImageView_RegenerateViewportRect(control_handle, int32, int32, int32, int32);
void API_ImageView_RegenerateImageRect(control_handle, double, double, double, double);
void API_ImageView_CommitViewportUpdates(control_handle);
api_bool API_ImageView_GetViewportUpdateRect(const_control_handle, int32*, int32*, int32*, int32*);
void API_ImageView_BeginViewportSelection(control_handle, int32 x, int32 y, uint32 flags);
void API_ImageView_ModifyViewportSelection(control_handle, int32 x, int32 y, uint32 flags);
void API_ImageView_UpdateViewportSelection(control_handle);
void API_ImageView_CancelViewportSelection(control_handle);
void API_ImageView_EndViewportSelection(control_handle);
api_bool API_ImageView_GetViewportSelection(const_control_handle, int32* x0, int32* y0, int32* x1, int32* y1, uint32* flags);
bitmap_handle API_ImageView_GetViewportBitmap(api_handle, const_control_handle, int32 x0, int32 y0, int32 x1, int32 y1, uint32 flags);

// ----------------------------------------------------------------------------
// CodeEditorContext API
// ----------------------------------------------------------------------------

control_handle API_CodeEditor_CreateCodeEditor(api_handle hModule, api_handle hClient, control_handle hParent, uint32 flags);
control_handle API_CodeEditor_CreateEditorLineNumbersControl(control_handle, api_handle hClient, control_handle hParent, uint32 flags);
api_bool API_CodeEditor_GetEditorFilePath(const_control_handle, char16_type*, size_type*);
void API_CodeEditor_SetEditorFilePath(control_handle, const char16_type*);
api_bool API_CodeEditor_GetEditorText(const_control_handle, char16_type*, size_type*);
void API_CodeEditor_SetEditorText(control_handle, const char16_type*);
api_bool API_CodeEditor_GetEditorEncodedText(const_control_handle, char*, size_type*, const char* encoding);
api_bool API_CodeEditor_SetEditorEncodedText(control_handle, const char*, const char* encoding);
void API_CodeEditor_ClearEditorText(control_handle);
api_bool API_CodeEditor_GetEditorReadOnly(const_control_handle);
void API_CodeEditor_SetEditorReadOnly(control_handle, api_bool);
api_bool API_CodeEditor_SaveEditorText(control_handle, const char16_type* filePath, const char* encoding);
api_bool API_CodeEditor_LoadEditorText(control_handle, const char16_type* filePath, const char* encoding);
int32 API_CodeEditor_GetEditorLineCount(const_control_handle);
int32 API_CodeEditor_GetEditorCharacterCount(const_control_handle);
void API_CodeEditor_GetEditorCursorCoordinates(const_control_handle, int32* line, int32* col);
void API_CodeEditor_SetEditorCursorCoordinates(control_handle, int32 line, int32 col);
api_bool API_CodeEditor_GetEditorInsertMode(const_control_handle);
void API_CodeEditor_SetEditorInsertMode(control_handle, api_bool);
api_bool API_CodeEditor_GetEditorBlockSelectionMode(const_control_handle);
void API_CodeEditor_SetEditorBlockSelectionMode(control_handle, api_bool);
api_bool API_CodeEditor_GetEditorDynamicWordWrapMode(const_control_handle);
void API_CodeEditor_SetEditorDynamicWordWrapMode(control_handle, api_bool);
int32 API_CodeEditor_GetEditorUndoSteps(const_control_handle);
int32 API_CodeEditor_GetEditorRedoSteps(const_control_handle);
api_bool API_CodeEditor_GetEditorHasSelection(const_control_handle);
void API_CodeEditor_GetEditorSelectionCoordinates(const_control_handle, int32* fromLine, int32* fromCol, int32* toLine, int32* toCol);
void API_CodeEditor_SetEditorSelectionCoordinates(control_handle, int32 fromLine, int32 fromCol, int32 toLine, int32 toCol);
api_bool API_CodeEditor_GetEditorSelectedText(const_control_handle, char16_type*, size_type*);
void API_CodeEditor_InsertEditorText(control_handle, const char16_type*);
void API_CodeEditor_EditorUndo(control_handle);
void API_CodeEditor_EditorRedo(control_handle);
void API_CodeEditor_EditorCut(control_handle);
void API_CodeEditor_EditorCopy(control_handle);
void API_CodeEditor_EditorPaste(control_handle);
void API_CodeEditor_EditorDelete(control_handle);
void API_CodeEditor_EditorSelectAll(control_handle);
void API_CodeEditor_EditorUnselect(control_handle);
api_bool API_CodeEditor_EditorGotoMatchedParenthesis(control_handle);
int32 API_CodeEditor_EditorHighlightAllMatches(control_handle, const char16_type*, uint32 flags);
void API_CodeEditor_EditorClearMatches(control_handle);
api_bool API_CodeEditor_EditorFind(control_handle, const char16_type*, uint32 flags);
api_bool API_CodeEditor_EditorReplace(control_handle, const char16_type*);
int32 API_CodeEditor_EditorReplaceAll(control_handle, const char16_type*, const char16_type*, uint32 flags);
api_bool API_CodeEditor_SetEditorTextUpdatedEventRoutine(control_handle, api_handle, pcl::event_routine);
api_bool API_CodeEditor_SetEditorCursorPositionUpdatedEventRoutine(control_handle, api_handle, pcl::range_event_routine);
api_bool API_CodeEditor_SetEditorSelectionUpdatedEventRoutine(control_handle, api_handle, pcl::rect_event_routine);
api_bool API_CodeEditor_SetEditorOverwriteModeUpdatedEventRoutine(control_handle, api_handle, pcl::state_event_routine);
api_bool API_CodeEditor_SetEditorSelectionModeUpdatedEventRoutine(control_handle, api_handle, pcl::state_event_routine);
api_bool API_CodeEditor_SetEditorDynamicWordWrapModeUpdatedEventRoutine(control_handle, api_handle, pcl::state_event_routine);

// ----------------------------------------------------------------------------
// WebViewContext API
// ----------------------------------------------------------------------------

control_handle API_WebView_CreateWebView(api_handle hModule, api_handle hClient, control_handle hParent, uint32 flags);
api_bool API_WebView_SetWebViewContent(control_handle, const void* data, size_type size, const char* mimeType);
api_bool API_WebView_LoadWebViewContent(control_handle, const char16_type* URI);
api_bool API_WebView_RequestWebViewPlainText(const_control_handle);
api_bool API_WebView_RequestWebViewHTML(const_control_handle);
api_bool API_WebView_SaveWebViewAsPDF(control_handle, const char16_type* filePath, const double* pageWidth, const double* pageHeight, const double* marginLeft, const double* marginTop, const double* marginRight, const double* marginBottom, int32 orientation);
api_bool API_WebView_GetWebViewHasSelection(const_control_handle);
api_bool API_WebView_GetWebViewSelectedText(const_control_handle, char16_type*, size_type*);
api_bool API_WebView_GetWebViewZoomFactor(const_control_handle, double*);
api_bool API_WebView_SetWebViewZoomFactor(control_handle, const double*);
uint32 API_WebView_GetWebViewBackgroundColor(const_control_handle);
api_bool API_WebView_SetWebViewBackgroundColor(control_handle, uint32);
api_bool API_WebView_ReloadWebView(control_handle);
api_bool API_WebView_StopWebView(control_handle);
api_bool API_WebView_EvaluateWebViewScript(control_handle, const char16_type* sourceCode, const char* language);
api_bool API_WebView_SetWebViewLoadStartedEventRoutine(control_handle, api_handle, pcl::event_routine);
api_bool API_WebView_SetWebViewLoadProgressEventRoutine(control_handle, api_handle, pcl::value_event_routine);
api_bool API_WebView_SetWebViewLoadFinishedEventRoutine(control_handle, api_handle, pcl::state_event_routine);
api_bool API_WebView_SetWebViewSelectionUpdatedEventRoutine(control_handle, api_handle, pcl::event_routine);
api_bool API_WebView_SetWebViewPlainTextAvailableEventRoutine(control_handle, api_handle, pcl::unicode_event_routine);
api_bool API_WebView_SetWebViewHTMLAvailableEventRoutine(control_handle, api_handle, pcl::unicode_event_routine);
api_bool API_WebView_SetWebViewScriptResultAvailableEventRoutine(control_handle, api_handle, pcl::property_event_routine);

// ----------------------------------------------------------------------------
// ExternalProcessContext API
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
};

   int32          (API_ExternalProcess_ExecuteProgram)( const char16_type* program, const char16_type** argv, size_type argc );

   api_bool       (API_ExternalProcess_StartProgram)( const char16_type* program, const char16_type** argv, size_type argc,
                                            const char16_type* workingDirectory, uint64* pid );

   external_process_handle (API_ExternalProcess_CreateExternalProcess)( api_handle hModule, api_handle hClient );

   api_bool       (API_ExternalProcess_StartExternalProcess)( external_process_handle,
                                                    const char16_type* program, const char16_type** argv, size_type argc );

   api_bool       (API_ExternalProcess_WaitForExternalProcessStarted)( external_process_handle, int32 ms );
   api_bool       (API_ExternalProcess_WaitForExternalProcessFinished)( external_process_handle, int32 ms );
   api_bool       (API_ExternalProcess_WaitForExternalProcessDataAvailable)( external_process_handle, int32 ms );
   api_bool       (API_ExternalProcess_WaitForExternalProcessDataWritten)( external_process_handle, int32 ms );

   api_bool       (API_ExternalProcess_TerminateExternalProcess)( external_process_handle );
   api_bool       (API_ExternalProcess_KillExternalProcess)( external_process_handle );

   api_bool       (API_ExternalProcess_CloseExternalProcessStream)( external_process_handle, int32 stream );

   api_bool       (API_ExternalProcess_RedirectExternalProcessToFile)( external_process_handle, int32 stream, const char16_type* fileName, api_bool append );
   api_bool       (API_ExternalProcess_PipeExternalProcess)( external_process_handle, int32 stream, external_process_handle toProcess );

   api_bool       (API_ExternalProcess_GetExternalProcessWorkingDirectory)( const_external_process_handle, char16_type*, size_type* );
   api_bool       (API_ExternalProcess_SetExternalProcessWorkingDirectory)( external_process_handle, const char16_type* );

   api_bool       (API_ExternalProcess_GetExternalProcessIsRunning)( const_external_process_handle );
   api_bool       (API_ExternalProcess_GetExternalProcessIsStarting)( const_external_process_handle );

   uint64         (API_ExternalProcess_GetExternalProcessPID)( const_external_process_handle );

   int32          (API_ExternalProcess_GetExternalProcessExitCode)( const_external_process_handle );
   int32          (API_ExternalProcess_GetExternalProcessExitStatus)( const_external_process_handle );
   int32          (API_ExternalProcess_GetExternalProcessErrorCode)( const_external_process_handle );

   size_type      (API_ExternalProcess_GetExternalProcessBytesAvailable)( const_external_process_handle );
   size_type      (API_ExternalProcess_GetExternalProcessBytesToWrite)( const_external_process_handle );

   // ### The following function returns data allocated by the caller module.
   api_bool       (API_ExternalProcess_ReadFromExternalProcess)( api_handle hModule, external_process_handle, int32 stream, void**, size_type* );
   api_bool       (API_ExternalProcess_WriteToExternalProcess)( external_process_handle, const void*, size_type count );

   api_bool       (API_ExternalProcess_EnumerateExternalProcessEnvironment)( const_external_process_handle, pcl::environment_enumeration_callback, void* );
   api_bool       (API_ExternalProcess_SetExternalProcessEnvironment)( external_process_handle, const char16_type** vars, size_type count );

   api_bool       (API_ExternalProcess_SetExternalProcessStartedEventRoutine)( external_process_handle, api_handle, pcl::external_process_event_routine );
   api_bool       (API_ExternalProcess_SetExternalProcessFinishedEventRoutine)( external_process_handle, api_handle, pcl::external_process_exit_status_event_routine );
   api_bool       (API_ExternalProcess_SetExternalProcessStandardOutputDataAvailableEventRoutine)( external_process_handle, api_handle, pcl::external_process_event_routine );
   api_bool       (API_ExternalProcess_SetExternalProcessStandardErrorDataAvailableEventRoutine)( external_process_handle, api_handle, pcl::external_process_event_routine );
   api_bool       (API_ExternalProcess_SetExternalProcessErrorEventRoutine)( external_process_handle, api_handle, pcl::external_process_status_event_routine );
  
/*  
   void        (API_ExternalProcess_EnterProcessDefinitionContext)();
   api_bool    (API_ExternalProcess_IsProcessDefinitionContextActive)();

   void        (API_ExternalProcess_BeginProcessDefinition)( meta_process_handle, const char* procId );
   api_bool    (API_ExternalProcess_GetProcessBeingDefined)( char*, size_type* );
   void        (API_ExternalProcess_SetProcessCategory)( const char* );
   void        (API_ExternalProcess_SetProcessVersion)( uint32 );
   void        (API_ExternalProcess_SetProcessAliasIdentifiers)( const char* );
   void        (API_ExternalProcess_SetProcessDescription)( const char16_type* );
   void        (API_ExternalProcess_SetProcessScriptComment)( const char16_type* );
   void        (API_ExternalProcess_SetProcessIconSVG)( const char* );
   void        (API_ExternalProcess_SetProcessIconSVGFile)( const char16_type* );
   void        (API_ExternalProcess_SetProcessIconImage)( const char** ); // ### deprecated
   void        (API_ExternalProcess_SetProcessIconImageFile)( const char16_type* ); // ### deprecated
   void        (API_ExternalProcess_SetProcessIconSmallImage)( const char** ); // ### deprecated
   void        (API_ExternalProcess_SetProcessIconSmallImageFile)( const char16_type* ); // ### deprecated

   void        (API_ExternalProcess_SetProcessClassInitializationRoutine)( pcl::process_class_initialization_routine );
   void        (API_ExternalProcess_SetProcessCreationRoutine)( pcl::process_creation_routine );
   void        (API_ExternalProcess_SetProcessDestructionRoutine)( pcl::process_destruction_routine );
   void        (API_ExternalProcess_SetProcessClonationRoutine)( pcl::process_clonation_routine);
   void        (API_ExternalProcess_SetProcessTestClonationRoutine)( pcl::process_test_clonation_routine );
   void        (API_ExternalProcess_SetProcessSetServerHandleRoutine)( pcl::process_set_handle_routine );
   void        (API_ExternalProcess_SetProcessAssignmentRoutine)( pcl::process_assignment_routine );
   void        (API_ExternalProcess_SetProcessInitializationRoutine)( pcl::process_initialization_routine );
   void        (API_ExternalProcess_SetProcessValidationRoutine)( pcl::process_validation_routine );

   void        (API_ExternalProcess_SetProcessCommandLineProcessingRoutine)( pcl::process_command_line_processing_routine, uint32 flags );
   void        (API_ExternalProcess_SetProcessEditPreferencesRoutine)( pcl::process_edit_preferences_routine );
   void        (API_ExternalProcess_SetProcessBrowseDocumentationRoutine)( pcl::process_browse_documentation_routine );
   void        (API_ExternalProcess_SetProcessExecutionPreferencesRoutine)( pcl::process_execution_preferences_routine );

   void        (API_ExternalProcess_SetProcessExecutionValidationRoutine)( pcl::process_execution_validation_routine );
   void        (API_ExternalProcess_SetProcessMaskValidationRoutine)( pcl::process_mask_validation_routine );
   void        (API_ExternalProcess_SetProcessHistoryUpdateValidationRoutine)( pcl::process_history_update_validation_routine );
   void        (API_ExternalProcess_SetProcessUndoModeRoutine)( pcl::process_undo_mode_routine );
   void        (API_ExternalProcess_SetProcessPreExecutionRoutine)( pcl::process_pre_execution_routine );
   void        (API_ExternalProcess_SetProcessExecutionRoutine)( pcl::process_execution_routine );
   void        (API_ExternalProcess_SetProcessPostExecutionRoutine)( pcl::process_post_execution_routine );

   void        (API_ExternalProcess_SetProcessGlobalExecutionValidationRoutine)( pcl::process_global_execution_validation_routine );
   void        (API_ExternalProcess_SetProcessPreGlobalExecutionRoutine)( pcl::process_pre_global_execution_routine );
   void        (API_ExternalProcess_SetProcessGlobalExecutionRoutine)( pcl::process_global_execution_routine );
   void        (API_ExternalProcess_SetProcessPostGlobalExecutionRoutine)( pcl::process_post_global_execution_routine );

   void        (API_ExternalProcess_SetProcessImageExecutionValidationRoutine)( pcl::process_image_execution_validation_routine );
   void        (API_ExternalProcess_SetProcessImageExecutionRoutine)( pcl::process_image_execution_routine );

   void        (API_ExternalProcess_SetProcessDefaultInterfaceSelectionRoutine)( pcl::process_default_interface_selection_routine );
   void        (API_ExternalProcess_SetProcessInterfaceSelectionRoutine)( pcl::process_interface_selection_routine );
   void        (API_ExternalProcess_SetProcessInterfaceValidationRoutine)( pcl::process_interface_validation_routine );

   void        (API_ExternalProcess_SetProcessPreReadingRoutine)( pcl::process_pre_reading_routine );
   void        (API_ExternalProcess_SetProcessPostReadingRoutine)( pcl::process_post_reading_routine );
   void        (API_ExternalProcess_SetProcessPreWritingRoutine)( pcl::process_pre_writing_routine );
   void        (API_ExternalProcess_SetProcessPostWritingRoutine)( pcl::process_post_writing_routine );

   void        (API_ExternalProcess_SetProcessIPCStartRoutine)( pcl::process_ipc_notification_routine );
   void        (API_ExternalProcess_SetProcessIPCStopRoutine)( pcl::process_ipc_notification_routine );
   void        (API_ExternalProcess_SetProcessIPCSetParametersRoutine)( pcl::process_ipc_notification_routine );
   void        (API_ExternalProcess_SetProcessIPCGetStatusRoutine)( pcl::process_ipc_status_routine );
   void        (API_ExternalProcess_BeginParameterDefinition)( meta_parameter_handle, const char* parId, uint32 parType );
   api_bool    (API_ExternalProcess_GetParameterBeingDefined)( char*, size_type* );
   void        (API_ExternalProcess_SetParameterProcessVersionRange)( uint32, uint32 );
   void        (API_ExternalProcess_SetParameterRequired)( api_bool );
   void        (API_ExternalProcess_SetParameterReadOnly)( api_bool );
   void        (API_ExternalProcess_SetParameterAliasIdentifiers)( const char* );
   void        (API_ExternalProcess_SetParameterDescription)( const char16_type* );
   void        (API_ExternalProcess_SetParameterScriptComment)( const char16_type* );
   void        (API_ExternalProcess_SetParameterLockRoutine)( pcl::parameter_lock_routine );
   void        (API_ExternalProcess_SetParameterUnlockRoutine)( pcl::parameter_unlock_routine );
   void        (API_ExternalProcess_SetParameterValidationRoutine)( pcl::parameter_validation_routine );
   void        (API_ExternalProcess_SetParameterAllocationRoutine)( pcl::parameter_allocation_routine );
   void        (API_ExternalProcess_SetParameterLengthQueryRoutine)( pcl::parameter_length_query_routine );
   void        (API_ExternalProcess_SetDefaultNumericValue)( double );
   void        (API_ExternalProcess_SetValidNumericRange)( double, double );
   void        (API_ExternalProcess_SetPrecision)( int32 );
   void        (API_ExternalProcess_SetScientificNotation)( api_bool );
   void        (API_ExternalProcess_SetDefaultBooleanValue)( api_bool );
   void        (API_ExternalProcess_DefineEnumerationElement)( const char*, api_enum );
   void        (API_ExternalProcess_DefineEnumerationAlias)( const char*, const char* );
   void        (API_ExternalProcess_SetDefaultEnumerationValueIndex)( uint32 );
   void        (API_ExternalProcess_SetDefaultStringValue)( const char16_type* );
   void        (API_ExternalProcess_SetStringAllowedCharacters)( const char16_type* );
   void        (API_ExternalProcess_SetStringLengthLimits)( size_type, size_type );
   void        (API_ExternalProcess_BeginTableColumnDefinition)( meta_parameter_handle, const char* colId, uint32 colType );
   void        (API_ExternalProcess_EndTableColumnDefinition)();
   void        (API_ExternalProcess_SetTableRowLimits)( size_type, size_type );
   void        (API_ExternalProcess_SetBlockSizeLimits)( size_type, size_type );

   void        (API_ExternalProcess_EndParameterDefinition)();
   void        (API_ExternalProcess_EndProcessDefinition)();
   void        (API_ExternalProcess_ExitProcessDefinitionContext)();
*/
  
// ----------------------------------------------------------------------------
// NetworkTransferContext API
// ----------------------------------------------------------------------------

network_transfer_handle API_NetworkTransfer_CreateNetworkTransfer(api_handle hModule, api_handle hClient);
api_bool API_NetworkTransfer_SetNetworkTransferURL(network_transfer_handle, const char16_type* url, const char16_type* userName, const char16_type* userPassword);
api_bool API_NetworkTransfer_SetNetworkTransferProxyURL(network_transfer_handle, const char16_type* proxy, const char16_type* userName, const char16_type* userPassword);
api_bool API_NetworkTransfer_SetNetworkTransferSSL(network_transfer_handle, api_bool useSSL, api_bool forceSSL, api_bool verifyPeer, api_bool verifyHost);
api_bool API_NetworkTransfer_SetNetworkTransferCustomHTTPHeaders(network_transfer_handle, const char16_type* nlsHeaders);
api_bool API_NetworkTransfer_SetNetworkTransferConnectionTimeout(network_transfer_handle, int32 seconds);
api_bool API_NetworkTransfer_PerformNetworkTransferDownload(network_transfer_handle);
api_bool API_NetworkTransfer_PerformNetworkTransferUpload(network_transfer_handle, fsize_type uploadSize);
api_bool API_NetworkTransfer_PerformNetworkTransferPOST(network_transfer_handle, const char16_type* postFields);
api_bool API_NetworkTransfer_PerformNetworkTransferSMTP(network_transfer_handle, const char16_type* mailFrom, const char16_type* mailRecipients);
void API_NetworkTransfer_CloseNetworkTransferConnection(network_transfer_handle);
api_bool API_NetworkTransfer_GetNetworkTransferURL(const_network_transfer_handle, char16_type*, size_type*);
api_bool API_NetworkTransfer_GetNetworkTransferProxyURL(const_network_transfer_handle, char16_type*, size_type*);
api_bool API_NetworkTransfer_GetNetworkTransferCustomHTTPHeaders(const_network_transfer_handle, char16_type*, size_type*);
api_bool API_NetworkTransfer_GetNetworkTransferStatus(const_network_transfer_handle);
api_bool API_NetworkTransfer_GetNetworkTransferIsAborted(const_network_transfer_handle);
int32 API_NetworkTransfer_GetNetworkTransferResponseCode(const_network_transfer_handle);
api_bool API_NetworkTransfer_GetNetworkTransferContentType(const_network_transfer_handle, char16_type*, size_type*);
fsize_type API_NetworkTransfer_GetNetworkTransferBytesTransferred(const_network_transfer_handle);
void API_NetworkTransfer_GetNetworkTransferTotalSpeed(const_network_transfer_handle, double*); // in KiB/s
   void           (API_NetworkTransfer_GetNetworkTransferTotalTime)( const_network_transfer_handle, double* ); // in s
   api_bool       (API_NetworkTransfer_GetNetworkTransferErrorInformation)( const_network_transfer_handle, char16_type*, size_type* );

   api_bool       (API_NetworkTransfer_SetNetworkTransferDownloadEventRoutine)( network_transfer_handle, api_handle, pcl::network_download_event_routine );
   api_bool       (API_NetworkTransfer_SetNetworkTransferUploadEventRoutine)( network_transfer_handle, api_handle, pcl::network_upload_event_routine );
   api_bool       (API_NetworkTransfer_SetNetworkTransferProgressEventRoutine)( network_transfer_handle, api_handle, pcl::network_progress_event_routine );

}  // extern "C"

// End global namespace

// ----------------------------------------------------------------------------

namespace pcl
{

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
extern "C" void* APIFunctionResolver( const char* );
}

#endif

// ----------------------------------------------------------------------------

#endif   // __PCL_API_APIInterface_h

// ----------------------------------------------------------------------------
// EOF pcl/APIInterface.h - Released 2025-04-07T08:52:44Z
