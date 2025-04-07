//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// ModuleParameterRegistration.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ModuleParameterRegistration_h
#define __ModuleParameterRegistration_h

#define REGISTER_MODULE_PARAMETER( PARAMETER ) \
   PARAMETER* The##PARAMETER##Parameter = nullptr

#define DEFINE_INT32_PARAMETER_CLASS( PARAMETER, PARAMETER_NAME, DEFAULT, MIN, MAX ) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaInt32( P )                           \
   {                                                                                 \
      The##PARAMETER##Parameter = this;                                              \
   }                                                                                 \
   IsoString PARAMETER::Id() const                                                   \
   {                                                                                 \
      return PARAMETER_NAME;                                                         \
   }                                                                                 \
   double PARAMETER::DefaultValue() const                                            \
   {                                                                                 \
      return DEFAULT;                                                                \
   }                                                                                 \
   double PARAMETER::MinimumValue() const                                            \
   {                                                                                 \
      return MIN;                                                                    \
   }                                                                                 \
   double PARAMETER::MaximumValue() const                                            \
   {                                                                                 \
      return MAX;                                                                    \
   }

#define DEFINE_UINT32_PARAMETER_CLASS( PARAMETER, PARAMETER_NAME, DEFAULT, MIN, MAX ) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaUInt32( P )                           \
   {                                                                                  \
      The##PARAMETER##Parameter = this;                                               \
   }                                                                                  \
   IsoString PARAMETER::Id() const                                                    \
   {                                                                                  \
      return PARAMETER_NAME;                                                          \
   }                                                                                  \
   double PARAMETER::DefaultValue() const                                             \
   {                                                                                  \
      return DEFAULT;                                                                 \
   }                                                                                  \
   double PARAMETER::MinimumValue() const                                             \
   {                                                                                  \
      return MIN;                                                                     \
   }                                                                                  \
   double PARAMETER::MaximumValue() const                                             \
   {                                                                                  \
      return MAX;                                                                     \
   }

#define DEFINE_STRING_PARAMETER_CLASS( PARAMETER, PARAMETER_NAME ) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaString( P )        \
   {                                                               \
      The##PARAMETER##Parameter = this;                            \
   }                                                               \
   IsoString PARAMETER::Id() const                                 \
   {                                                               \
      return PARAMETER_NAME;                                       \
   }

#define DEFINE_STRING_PARAMETER_WITH_DEFAULT_CLASS( PARAMETER, PARAMETER_NAME , DEFAULT) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaString( P )                              \
   {                                                                                     \
      The##PARAMETER##Parameter = this;                                                  \
   }                                                                                     \
   IsoString PARAMETER::Id() const                                                       \
   {                                                                                     \
      return PARAMETER_NAME;                                                             \
   }                                                                                     \
   String PARAMETER::DefaultValue() const                                                \
   {                                                                                     \
      return DEFAULT;                                                                    \
   }

#define DEFINE_DOUBLE_PARAMETER_CLASS( PARAMETER, PARAMETER_NAME, PRECISION, DEFAULT, MIN, MAX ) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaDouble( P )                                      \
   {                                                                                             \
      The##PARAMETER##Parameter = this;                                                          \
   }                                                                                             \
   IsoString PARAMETER::Id() const                                                               \
   {                                                                                             \
      return PARAMETER_NAME;                                                                     \
   }                                                                                             \
   int PARAMETER::Precision() const                                                              \
   {                                                                                             \
      return PRECISION;                                                                          \
   }                                                                                             \
   double PARAMETER::DefaultValue() const                                                        \
   {                                                                                             \
      return DEFAULT;                                                                            \
   }                                                                                             \
   double PARAMETER::MinimumValue() const                                                        \
   {                                                                                             \
      return MIN;                                                                                \
   }                                                                                             \
   double PARAMETER::MaximumValue() const                                                        \
   {                                                                                             \
      return MAX;                                                                                \
   }

#define DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( PARAMETER, PARAMETER_NAME, PRECISION, DEFAULT, MIN, MAX ) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaDouble( P )                                                \
   {                                                                                                       \
      The##PARAMETER##Parameter = this;                                                                    \
   }                                                                                                       \
   IsoString PARAMETER::Id() const                                                                         \
   {                                                                                                       \
      return PARAMETER_NAME;                                                                               \
   }                                                                                                       \
   int PARAMETER::Precision() const                                                                        \
   {                                                                                                       \
      return PRECISION;                                                                                    \
   }                                                                                                       \
   double PARAMETER::DefaultValue() const                                                                  \
   {                                                                                                       \
      return DEFAULT;                                                                                      \
   }                                                                                                       \
   double PARAMETER::MinimumValue() const                                                                  \
   {                                                                                                       \
      return MIN;                                                                                          \
   }                                                                                                       \
   double PARAMETER::MaximumValue() const                                                                  \
   {                                                                                                       \
      return MAX;                                                                                          \
   }                                                                                                       \
   bool PARAMETER::IsReadOnly() const                                                                      \
   {                                                                                                       \
      return true;                                                                                         \
   }

#define DEFINE_FLOAT_PARAMETER_CLASS( PARAMETER, PARAMETER_NAME, PRECISION, DEFAULT, MIN, MAX ) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaFloat( P )                                      \
   {                                                                                             \
      The##PARAMETER##Parameter = this;                                                          \
   }                                                                                             \
   IsoString PARAMETER::Id() const                                                               \
   {                                                                                             \
      return PARAMETER_NAME;                                                                     \
   }                                                                                             \
   int PARAMETER::Precision() const                                                              \
   {                                                                                             \
      return PRECISION;                                                                          \
   }                                                                                             \
   double PARAMETER::DefaultValue() const                                                        \
   {                                                                                             \
      return DEFAULT;                                                                            \
   }                                                                                             \
   double PARAMETER::MinimumValue() const                                                        \
   {                                                                                             \
      return MIN;                                                                                \
   }                                                                                             \
   double PARAMETER::MaximumValue() const                                                        \
   {                                                                                             \
      return MAX;                                                                                \
   }

#define DEFINE_FLOAT_READ_ONLY_PARAMETER_CLASS( PARAMETER, PARAMETER_NAME, PRECISION, DEFAULT, MIN, MAX ) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaFloat( P )                                                \
   {                                                                                                       \
      The##PARAMETER##Parameter = this;                                                                    \
   }                                                                                                       \
   IsoString PARAMETER::Id() const                                                                         \
   {                                                                                                       \
      return PARAMETER_NAME;                                                                               \
   }                                                                                                       \
   int PARAMETER::Precision() const                                                                        \
   {                                                                                                       \
      return PRECISION;                                                                                    \
   }                                                                                                       \
   double PARAMETER::DefaultValue() const                                                                  \
   {                                                                                                       \
      return DEFAULT;                                                                                      \
   }                                                                                                       \
   double PARAMETER::MinimumValue() const                                                                  \
   {                                                                                                       \
      return MIN;                                                                                          \
   }                                                                                                       \
   double PARAMETER::MaximumValue() const                                                                  \
   {                                                                                                       \
      return MAX;                                                                                          \
   }                                                                                                       \
   bool PARAMETER::IsReadOnly() const                                                                      \
   {                                                                                                       \
      return true;                                                                                         \
   }

#define DEFINE_BOOLEAN_PARAMETER_CLASS( PARAMETER, PARAMETER_NAME, DEFAULT ) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaBoolean( P )                 \
   {                                                                         \
      The##PARAMETER##Parameter = this;                                      \
   }                                                                         \
   IsoString PARAMETER::Id() const                                           \
   {                                                                         \
      return PARAMETER_NAME;                                                 \
   }                                                                         \
   bool PARAMETER::DefaultValue() const                                      \
   {                                                                         \
      return DEFAULT;                                                        \
   }

#define DEFINE_ENUM_PARAMETER_CLASS( PARAMETER, PARAMETER_NAME, ENUM_LIST, DEFAULT_ITEM ) \
   PARAMETER::PARAMETER( MetaProcess* P ) : MetaEnumeration( P )                          \
   {                                                                                      \
      The##PARAMETER##Parameter = this;                                                   \
   }                                                                                      \
   IsoString PARAMETER::ElementId( size_type i ) const                                    \
   {                                                                                      \
      switch ( i )                                                                        \
      {                                                                                   \
         ENUM_LIST                                                                        \
      default:                                                                            \
         return PARAMETER_NAME "_" #DEFAULT_ITEM;                                         \
      }                                                                                   \
   }                                                                                      \
   IsoString PARAMETER::Id() const                                                        \
   {                                                                                      \
      return PARAMETER_NAME;                                                              \
   }                                                                                      \
   size_type PARAMETER::NumberOfElements() const                                          \
   {                                                                                      \
      return NumberOfEnumElements;                                                        \
   }                                                                                      \
   int PARAMETER::ElementValue( size_type i ) const                                       \
   {                                                                                      \
      return int( i );                                                                    \
   }                                                                                      \
   size_type PARAMETER::DefaultValueIndex() const                                         \
   {                                                                                      \
      return size_type( Default );                                                        \
   }

// ============================================================================

#define DECLARE_INT32_PARAMETER_CLASS( PARAMETER ) \
   class PARAMETER : public MetaInt32              \
   {                                               \
   public:                                         \
      PARAMETER( MetaProcess* );                   \
      virtual IsoString Id() const;                \
      virtual double DefaultValue() const;         \
      virtual double MinimumValue() const;         \
      virtual double MaximumValue() const;         \
   };                                              \
   extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_UINT32_PARAMETER_CLASS( PARAMETER ) \
   class PARAMETER : public MetaUInt32              \
   {                                                \
   public:                                          \
      PARAMETER( MetaProcess* );                    \
      virtual IsoString Id() const;                 \
      virtual double DefaultValue() const;          \
      virtual double MinimumValue() const;          \
      virtual double MaximumValue() const;          \
   };                                               \
   extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_STRING_PARAMETER_CLASS( PARAMETER ) \
   class PARAMETER : public MetaString              \
   {                                                \
   public:                                          \
      PARAMETER( MetaProcess* );                    \
      virtual IsoString Id() const;                 \
   };                                               \
   extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_STRING_PARAMETER_WITH_DEFAULT_CLASS( PARAMETER ) \
   class PARAMETER : public MetaString                           \
   {                                                             \
   public:                                                       \
      PARAMETER( MetaProcess* );                                 \
      virtual IsoString Id() const;                              \
      virtual String DefaultValue() const;                       \
   };                                                            \
   extern PARAMETER* The##PARAMETER##Parameter


#define DECLARE_DOUBLE_PARAMETER_CLASS( PARAMETER ) \
   class PARAMETER : public MetaDouble              \
   {                                                \
   public:                                          \
      PARAMETER( MetaProcess* );                    \
      virtual IsoString Id() const;                 \
      virtual int Precision() const;                \
      virtual double DefaultValue() const;          \
      virtual double MinimumValue() const;          \
      virtual double MaximumValue() const;          \
   };                                               \
   extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( PARAMETER ) \
   class PARAMETER : public MetaDouble                        \
   {                                                          \
   public:                                                    \
      PARAMETER( MetaProcess* );                              \
      virtual IsoString Id() const;                           \
      virtual int Precision() const;                          \
      virtual double DefaultValue() const;                    \
      virtual double MinimumValue() const;                    \
      virtual double MaximumValue() const;                    \
      virtual bool IsReadOnly() const;                        \
   };                                                         \
   extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_FLOAT_PARAMETER_CLASS( PARAMETER )  \
   class PARAMETER : public MetaFloat               \
   {                                                \
   public:                                          \
      PARAMETER( MetaProcess* );                    \
      virtual IsoString Id() const;                 \
      virtual int Precision() const;                \
      virtual double DefaultValue() const;          \
      virtual double MinimumValue() const;          \
      virtual double MaximumValue() const;          \
   };                                               \
   extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_FLOAT_READ_ONLY_PARAMETER_CLASS( PARAMETER )  \
   class PARAMETER : public MetaFloat                         \
   {                                                          \
   public:                                                    \
      PARAMETER( MetaProcess* );                              \
      virtual IsoString Id() const;                           \
      virtual int Precision() const;                          \
      virtual double DefaultValue() const;                    \
      virtual double MinimumValue() const;                    \
      virtual double MaximumValue() const;                    \
      virtual bool IsReadOnly() const;                        \
   };                                                         \
   extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_BOOLEAN_PARAMETER_CLASS( PARAMETER ) \
   class PARAMETER : public MetaBoolean              \
   {                                                 \
   public:                                           \
      PARAMETER( MetaProcess* );                     \
      virtual IsoString Id() const;                  \
      virtual bool DefaultValue() const;             \
   };                                                \
   extern PARAMETER* The##PARAMETER##Parameter

#define ENUM_ITEM( X ) X,

#define DECLARE_ENUM_PARAMETER( PARAMETER, ENUM_LIST, DEFAULT_ITEM ) \
   class PARAMETER : public MetaEnumeration                          \
   {                                                                 \
   public:                                                           \
      enum                                                           \
      {                                                              \
         ENUM_LIST                                                   \
            NumberOfEnumElements,                                    \
         Default = DEFAULT_ITEM                                      \
      };                                                             \
      PARAMETER( MetaProcess* );                                     \
      virtual IsoString Id() const;                                  \
      virtual size_type NumberOfElements() const;                    \
      virtual IsoString ElementId( size_type ) const;                \
      virtual int ElementValue( size_type ) const;                   \
      virtual size_type DefaultValueIndex() const;                   \
   };                                                                \
   extern PARAMETER* The##PARAMETER##Parameter

#define DECLARE_ENUM_READ_ONLY_PARAMETER( PARAMETER, ENUM_LIST, DEFAULT_ITEM ) \
   class PARAMETER : public MetaEnumeration                                    \
   {                                                                           \
   public:                                                                     \
      enum                                                                     \
      {                                                                        \
         ENUM_LIST                                                             \
            NumberOfEnumElements,                                              \
         Default = DEFAULT_ITEM                                                \
      };                                                                       \
      PARAMETER( MetaProcess* );                                               \
      virtual IsoString Id() const;                                            \
      virtual size_type NumberOfElements() const;                              \
      virtual IsoString ElementId( size_type ) const;                          \
      virtual int ElementValue( size_type ) const;                             \
      virtual size_type DefaultValueIndex() const;                             \
      virtual bool IsReadOnly() const                                          \
      {                                                                        \
         return true;                                                          \
      }                                                                        \
   };                                                                          \
   extern PARAMETER* The##PARAMETER##Parameter

#endif // __ModuleParameterRegistration_h

// ----------------------------------------------------------------------------
// EOF ModuleParameterRegistration.h - Released 2025-04-07T08:53:56Z
