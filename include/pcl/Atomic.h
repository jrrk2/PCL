//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Atomic.h - Fixed version with proper macOS atomics
// ----------------------------------------------------------------------------

#ifndef __PCL_Atomic_h
#define __PCL_Atomic_h

#include <pcl/Defs.h>

#ifdef __PCL_WINDOWS
# include <intrin.h>
# pragma intrinsic (_InterlockedIncrement)
# pragma intrinsic (_InterlockedDecrement)
# pragma intrinsic (_InterlockedCompareExchange)
# pragma intrinsic (_InterlockedExchange)
# pragma intrinsic (_InterlockedExchangeAdd)
#endif

// Modern approach: use C++11 atomic for macOS and fallback
#if defined(__PCL_MACOSX)
# include <atomic>
#endif

namespace pcl
{

class PCL_CLASS AtomicInt
{
public:

   AtomicInt( int value = 0 )
#ifdef __PCL_MACOSX
      : m_value( value )
#else
      : m_value( value )
#endif
   {
   }

   AtomicInt( const AtomicInt& x )
#ifdef __PCL_MACOSX
      : m_value( x.m_value )
#else
      : m_value( x.m_value )
#endif
   {
   }

   AtomicInt& operator =( const AtomicInt& x )
   {
#ifdef __PCL_MACOSX
      m_value = x.m_value;
#else
      m_value = x.m_value;
#endif
      return *this;
   }

   operator int() const
   {
#ifdef __PCL_MACOSX
      return m_value;
#else
      return m_value;
#endif
   }

   bool operator !() const
   {
#ifdef __PCL_MACOSX
      return m_value == 0;
#else
      return m_value == 0;
#endif
   }

   bool operator ==( int x ) const
   {
#ifdef __PCL_MACOSX
      return m_value == x;
#else
      return m_value == x;
#endif
   }

   bool operator !=( int x ) const
   {
#ifdef __PCL_MACOSX
      return m_value != x;
#else
      return m_value != x;
#endif
   }

   AtomicInt& operator =( int x )
   {
#ifdef __PCL_MACOSX
      m_value = x ;
#else
      m_value = x;
#endif
      return *this;
   }

   int Load()
   {
#ifdef __PCL_MACOSX
      return FetchAndAdd( 0 );
#else
      return FetchAndAdd( 0 );
#endif
   }

   void Store( int newValue )
   {
#ifdef __PCL_MACOSX
      (void)FetchAndStore( newValue );
#else
      (void)FetchAndStore( newValue );
#endif
   }
// Simple fix: just replace the macOS-specific "cheating" sections
// Keep everything else the same, just fix the atomic operations

void Increment()
{
#ifdef __PCL_WINDOWS
   (void)_InterlockedIncrement( &m_value );
#else
#ifdef __PCL_MACOSX
   __sync_fetch_and_add(&m_value, 1);  // Proper atomic increment
#else
   asm volatile( "lock\n\t"
                 "incl %0\n"
                  : "=m" (m_value)
                  : "m" (m_value)
                  : "memory", "cc" );
#endif
#endif
}

void Decrement()
{
#ifdef __PCL_WINDOWS
   (void)_InterlockedDecrement( &m_value );
#else
#ifdef __PCL_MACOSX
   __sync_fetch_and_sub(&m_value, 1);  // Proper atomic decrement
#else
   asm volatile( "lock\n\t"
                 "decl %0\n"
                  : "=m" (m_value)
                  : "m" (m_value)
                  : "memory", "cc" );
#endif
#endif
}

bool Reference()
{
#ifdef __PCL_WINDOWS
   return _InterlockedIncrement( &m_value ) != 0;
#else
#ifdef __PCL_MACOSX
   return __sync_add_and_fetch(&m_value, 1) != 0;  // Proper atomic increment and test
#else
   uint8 result;
   asm volatile( "lock\n\t"
                 "incl %0\n\t"
                 "setnz %1\n"
                  : "=m" (m_value), "=qm" (result)
                  : "m" (m_value)
                  : "memory", "cc" );
   return result != 0;
#endif
#endif
}

bool Dereference()
{
#ifdef __PCL_WINDOWS
   return _InterlockedDecrement( &m_value ) != 0;
#else
#ifdef __PCL_MACOSX
   return __sync_sub_and_fetch(&m_value, 1) != 0;  // Proper atomic decrement and test
#else
   uint8 result;
   asm volatile( "lock\n\t"
                 "decl %0\n\t"
                 "setnz %1\n"
                  : "=m" (m_value), "=qm" (result)
                  : "m" (m_value)
                  : "memory", "cc" );
   return result != 0;
#endif
#endif
}

bool TestAndSet( int expectedValue, int newValue )
{
#ifdef __PCL_WINDOWS
   return _InterlockedCompareExchange( &m_value, newValue, expectedValue ) == expectedValue;
#else
#ifdef __PCL_MACOSX
   return __sync_bool_compare_and_swap(&m_value, expectedValue, newValue);  // Proper atomic compare-and-swap
#else
   uint8 result;
   asm volatile( "lock\n\t"
                 "cmpxchgl %3,%2\n\t"
                 "setz %1\n"
                  : "=a" (newValue), "=qm" (result), "+m" (m_value)
                  : "r" (newValue), "0" (expectedValue)
                  : "memory", "cc" );
   return result != 0;
#endif
#endif
}

int FetchAndStore( int newValue )
{
#ifdef __PCL_WINDOWS
   return _InterlockedExchange( &m_value, newValue );
#else
#ifdef __PCL_MACOSX
   return __sync_lock_test_and_set(&m_value, newValue);  // Proper atomic exchange
#else
   asm volatile( "xchgl %0,%1\n"
                  : "=r" (newValue), "+m" (m_value)
                  : "0" (newValue)
                  : "memory" );
   return newValue;
#endif
#endif
}

int FetchAndAdd( int valueToAdd )
{
#ifdef __PCL_WINDOWS
   return _InterlockedExchangeAdd( &m_value, valueToAdd );
#else
#ifdef __PCL_MACOSX
   return __sync_fetch_and_add(&m_value, valueToAdd);  // Proper atomic fetch-and-add
#else
   asm volatile( "lock\n\t"
                 "xaddl %0,%1\n"
                  : "=r" (valueToAdd), "+m" (m_value)
                  : "0" (valueToAdd)
                  : "memory", "cc" );
   return valueToAdd;
#endif
#endif
}

private:

#ifdef _MSC_VER
   __declspec(align(4)) volatile long m_value;
#else
   volatile int m_value __attribute__ ((aligned (4)));
#endif
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup reentrancy_protection Thread-Safe Protection of Non-Reentrant Code
 */

// ----------------------------------------------------------------------------

/*!
 * \class AutoReentrancyGuard
 * \brief Automatic reentrancy guard sentinel.
 *
 * %AutoReentrancyGuard allows you to protect a block of code ensuring that it
 * cannot be reentrant. All you need is a static AtomicInt variable that works
 * as a 'busy state' flag persistent across function invocations.
 *
 * Consider the following example:
 *
 * \code void foo()
 * {
 *    static AtomicInt flag;
 *    AutoReentrancyGuard guard( flag );
 *    if ( guard )
 *    {
 *       // protected code
 *    }
 * }
 * \endcode
 *
 * The function \c foo is not reentrant, so we want to protect it against
 * possible reentrant invocations while the function's code is being executed.
 *
 * The \c flag variable is initially zero (because AtomicInt's default
 * constructor initializes its integer member to zero). The first time \c foo
 * is called, %AutoReentrancyGuard's constructor can change the value of
 * \c flag from zero to one as an atomic operation. When this happens,
 * AutoReentrancyGuard::operator bool() returns true, and the code protected
 * within the \c if block can be executed.
 *
 * When the \c guard object gets out of scope (just before the \c foo function
 * returns), its class destructor resets \c flag to zero automatically, which
 * permits the next non-reentrant execution of \c foo. However, if \c foo is
 * called again before \c guard is destroyed, a newly constructed
 * %AutoReentrancyGuard object cannot make a transition 0 -> 1 with the static
 * \c flag variable, and hence a reentrant execution of the protected code is
 * not allowed (in this case, the function simply does nothing and returns
 * after the \c if block). Note that the protected code can freely return from
 * the function or throw exceptions; the \c flag variable will be reset to zero
 * automatically when \c guard gets out of scope.
 *
 * Since %AutoReentrancyGuard uses AtomicInt to implement atomic transitions,
 * code blocks can be protected against reentrant execution in multithreaded
 * environments.
 *
 * The macros PCL_REENTRANCY_GUARDED_BEGIN and PCL_REENTRANCY_GUARDED_END
 * greatly simplify reentrancy protection. For example, the above code could be
 * implemented as follows:
 *
 * \code void foo()
 * {
 *    PCL_REENTRANCY_GUARDED_BEGIN
 *    // protected code
 *    PCL_REENTRANCY_GUARDED_END
 * } \endcode
 *
 * In addition, the macros PCL_CLASS_REENTRANCY_GUARD and
 * PCL_CLASS_REENTRANCY_GUARDED_BEGIN are useful for protection of all
 * non-reentrant member functions of a class, and the macros
 * PCL_MEMBER_REENTRANCY_GUARD and PCL_MEMBER_REENTRANCY_GUARDED_BEGIN provide
 * protection of specific member functions. See these macros for examples.
 *
 * \ingroup reentrancy_protection
 */
class AutoReentrancyGuard
{
public:

   /*!
    * Constructs an %AutoReentrancyGuard object to monitor the specified
    * \a guard variable. If \c guard is zero, its value is set to one as an
    * atomic operation. If \c guard is nonzero, its value is not changed.
    *
    * \warning The monitored guard variable *must* be either a static variable
    * local to the function being protected, or a data member of the same class
    * to which a protected member function belongs. Otherwise the protection
    * mechanism will not work. This can be dangerous, especially because you
    * may erroneously think that your code is being protected when it is not.
    * In addition, the guard variable must be zero initially, or the protected
    * code will never be allowed to work. We strongly recommend you don't use
    * this class directly, but the PCL_REENTRANCY_GUARDED_BEGIN and
    * PCL_REENTRANCY_GUARDED_END macros to implement function level protection,
    * or PCL_CLASS_REENTRANCY_GUARD, PCL_CLASS_REENTRANCY_GUARDED_BEGIN and
    * PCL_CLASS_REENTRANCY_GUARDED_END to implement per-instance function
    * member protection.
    */
   AutoReentrancyGuard( AtomicInt& guard )
      : m_guard( guard )
   {
      m_guarded = m_guard.TestAndSet( 0, 1 );
   }

   /*!
    * Destroys this object. If the value of the monitored guard variable (see
    * the class constructor) was zero when this object was constructed, its
    * value is reset to zero as an atomic operation.
    */
   ~AutoReentrancyGuard()
   {
      if ( m_guarded )
         m_guard.Store( 0 );
   }

   /*!
    * Returns true iff the value of the monitored guard variable (see the class
    * constructor) was zero when this object was constructed.
    */
   operator bool() const volatile
   {
      return m_guarded;
   }

private:

   AtomicInt& m_guard;
   bool       m_guarded = false;
};

/*!
 * \def PCL_REENTRANCY_GUARDED_BEGIN
 *
 * This macro along with PCL_REENTRANCY_GUARDED_END simplifies protection of
 * non-reentrant code. See the AutoReentrancyGuard class for detailed
 * information and examples.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_REENTRANCY_GUARDED_BEGIN                                          \
   {                                                                          \
      static pcl::AtomicInt __r_g__( 0 );                                     \
      volatile pcl::AutoReentrancyGuard __a_r_g__( __r_g__ );                 \
      if ( __a_r_g__ )                                                        \
      {

/*!
 * \def PCL_REENTRANCY_GUARDED_END
 *
 * This macro, along with PCL_REENTRANCY_GUARDED_BEGIN, simplifies protection
 * of non-reentrant code. See the AutoReentrancyGuard class for detailed
 * information and examples.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_REENTRANCY_GUARDED_END                                            \
      }                                                                       \
   }

/*!
 * \def PCL_CLASS_REENTRANCY_GUARD
 *
 * Declares a class data member for class-wide protection of non-reentrant
 * member functions with the PCL_CLASS_REENTRANCY_GUARDED_BEGIN and
 * PCL_REENTRANCY_GUARDED_END macros.
 *
 * Example:
 *
 * \code class foo
 * {
 * public:
 *    // ...
 * private:
 *    PCL_CLASS_REENTRANCY_GUARD
 *
 *    void bar1()
 *    {
 *       PCL_CLASS_REENTRANCY_GUARDED_BEGIN
 *       // Protected code
 *       PCL_REENTRANCY_GUARDED_END
 *    }
 *
 *    void bar2() const
 *    {
 *       PCL_CLASS_REENTRANCY_GUARDED_BEGIN
 *       // Protected code
 *       PCL_REENTRANCY_GUARDED_END
 *    }
 * }; \endcode
 *
 * In this example the bar1 and bar2 member functions are protected against
 * reentrant execution. Note that reentrancy protection is a per-instance,
 * class-wide property in this case: the bar1() and bar2() functions can be
 * executed simultaneously for different objects, but they cannot be re-entered
 * for the same instance. Furthermore, one of these functions cannot call the
 * other for the same object, since both share the same reentrancy guard member
 * in the foo class.
 *
 * See the AutoReentrancyGuard class for more information on the PCL
 * implementation of reentrancy protection.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_CLASS_REENTRANCY_GUARD                                            \
   mutable pcl::AtomicInt __pcl_guard__;

/*!
 * \def PCL_CLASS_REENTRANCY_GUARDED_BEGIN
 *
 * This macro, along with PCL_CLASS_REENTRANCY_GUARD and
 * PCL_REENTRANCY_GUARDED_END, simplifies per-instance protection of
 * non-reentrant class member functions. See PCL_CLASS_REENTRANCY_GUARD for an
 * example.
 *
 * See the AutoReentrancyGuard class for more information on the PCL
 * implementation of reentrancy protection.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_CLASS_REENTRANCY_GUARDED_BEGIN                                    \
   {                                                                          \
      volatile pcl::AutoReentrancyGuard __a_r_g__( __pcl_guard__ );           \
      if ( __a_r_g__ )                                                        \
      {

/*!
 * \def PCL_MEMBER_REENTRANCY_GUARD
 *
 * Declares a class data member for protection of a specific non-reentrant
 * member function with the PCL_MEMBER_REENTRANCY_GUARDED_BEGIN and
 * PCL_REENTRANCY_GUARDED_END macros.
 *
 * Example:
 *
 * \code class foo
 * {
 * public:
 *    // ...
 * private:
 *    PCL_MEMBER_REENTRANCY_GUARD( bar1 )
 *    PCL_MEMBER_REENTRANCY_GUARD( bar2 )
 *
 *    void bar1()
 *    {
 *       PCL_MEMBER_REENTRANCY_GUARDED_BEGIN( bar1 )
 *       // Protected code
 *       PCL_REENTRANCY_GUARDED_END
 *    }
 *
 *    void bar2() const
 *    {
 *       PCL_MEMBER_REENTRANCY_GUARDED_BEGIN( bar2 )
 *       // Protected code
 *       PCL_REENTRANCY_GUARDED_END
 *    }
 * }; \endcode
 *
 * In this example the bar1 and bar2 member functions are protected against
 * reentrant execution. Note that reentrancy protection is a per-instance,
 * function-specific property in this case: the bar1() and bar2() functions can
 * be executed simultaneously for different objects, but they cannot be
 * re-entered for the same instance. Since each member function uses its own
 * reentrancy guard member in the foo class, each of them can safely call the
 * other for the same object.
 *
 * See the AutoReentrancyGuard class for more information on the PCL
 * implementation of reentrancy protection.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_MEMBER_REENTRANCY_GUARD( member )                                 \
   mutable pcl::AtomicInt __pcl_guard_##member##__;

/*!
 * \def PCL_MEMBER_REENTRANCY_GUARDED_BEGIN
 *
 * This macro, along with PCL_MEMBER_REENTRANCY_GUARD and
 * PCL_REENTRANCY_GUARDED_END, simplifies per-instance protection of specific
 * non-reentrant member functions. See PCL_MEMBER_REENTRANCY_GUARD for an
 * example.
 *
 * See the AutoReentrancyGuard class for more information on the PCL
 * implementation of reentrancy protection.
 *
 * \ingroup reentrancy_protection
 */
#define PCL_MEMBER_REENTRANCY_GUARDED_BEGIN( member )                         \
   {                                                                          \
      volatile pcl::AutoReentrancyGuard __a_r_g__( __pcl_guard_##member##__ );\
      if ( __a_r_g__ )                                                        \
      {

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Atomic_h

// ----------------------------------------------------------------------------
// EOF pcl/Atomic.h - Released 2025-04-07T08:52:44Z
