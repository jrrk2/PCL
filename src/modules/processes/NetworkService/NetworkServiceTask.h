//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceTask.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NetworkServiceTask_h
#define __NetworkServiceTask_h

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct NetworkServiceLogger;

// ----------------------------------------------------------------------------

/*!
 * \class TaskData
 * \brief Task request data.
 *
 * This structure holds the data sent by a task provider to describe a task
 * request. The \a taskType member identifies a derived class of
 * NetworkServiceTask able to perform a particular task (see the
 * NetworkServiceTask::TaskType() virtual member function).
 */
struct TaskData
{
   String serialNumber; //!< Identifies the task request. Should be unique for the task type.
   String requestUTC;   //!< UTC date and time this request was created, in ISO 8601 format.
   String workerName;   //!< Identifies the machine that is being processing this task.
   String taskType;     //!< The task type, which is used to select a NetworkServiceTask derived class.
   String parameters;   //!< Task-specific parameters.
   String callbackURL;  //!< The URL of the server that receives task results.
};

// ----------------------------------------------------------------------------

/*!
 * \class NetworkServiceTask
 * \brief Base class of all network service tasks.
 *
 * %NetworkServiceTask defines the prototype required for all classes
 * implementing network service tasks. The default implementation is only
 * useful for testing purposes. The virtual member functions in this class must
 * be reimplemented in a derived class to implement an actual task in a
 * customized version of the NetworkService module.
 */
class NetworkServiceTask
{
public:

   /*!
    * Default constructor.
    */
   NetworkServiceTask() = default;

   /*!
    * Virtual destructor. This destructor can throw exceptions.
    */
   virtual ~NetworkServiceTask() noexcept( false )
   {
   }

   /*!
    * The task type. This member function must be reimplemented by derived
    * classes.
    */
   virtual String TaskType() const;

   /*!
    * The version of this task implementation. This member function must be
    * reimplemented by derived classes.
    */
   virtual String TaskVersion() const;

   /*!
    * Performs a new task.
    *
    * \param data    Task request data. Describes the task request and includes
    *                task-specific parameters.
    *
    * \param log     The logger object being used by the network service. A
    *                reimplementation of this member function must use this
    *                object to provide detailed information on the work
    *                performed, as well as on any errors or warning conditions
    *                that may arise.
    */
   virtual void Perform( const TaskData& data, NetworkServiceLogger& log );

   /*!
    * Returns an instance of a derived class able to perform a task of the
    * specified \a type, or \c nullptr if no derived class can process such
    * task in this module.
    */
   static NetworkServiceTask* TaskByType( const String& type );

   /*!
    * Equality relational operator. Returns true if the specified object
    * \a task and this object can process tasks of the same type.
    */
   bool operator ==( const NetworkServiceTask& task ) const
   {
      return TaskType() == task.TaskType();
   }

   /*!
    * Less than relational operator. Compares task types, for list sorting
    * purposes.
    */
   bool operator <( const NetworkServiceTask& task ) const
   {
      return TaskType() < task.TaskType();
   }

protected:

   /*!
    * Registers the specified \a task with the network service in this module.
    *
    * \note  This static member function must be invoked by all derived classes
    * upon construction. This is necessary for the automatic selection of task
    * instances selected by task types.
    */
   static void RegisterTask( NetworkServiceTask* task );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NetworkServiceTask_h

// ----------------------------------------------------------------------------
// EOF NetworkServiceTask.h - Released 2025-04-07T08:53:56Z
