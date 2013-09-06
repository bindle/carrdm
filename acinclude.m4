#
#   C Allocate, Retain, Release, and Destroy Management Functions
#   Copyright (C) 2013 Bindle Binaries <syzdek@bindlebinaries.com>.
#
#   @BINDLE_BINARIES_BSD_LICENSE_START@
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are
#   met:
#
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of Bindle Binaries nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BINDLE BINARIES BE LIABLE FOR
#   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#   SUCH DAMAGE.
#
#   @BINDLE_BINARIES_BSD_LICENSE_END@
#
#   acinclude.m4 - custom m4 macros used by configure.ac
#

# AC_CARRDM_SPINLOCK
# ______________________________________________________________________________
AC_DEFUN([AC_CARRDM_SPINLOCK],[dnl

   # prerequists
   AC_REQUIRE([AC_PROG_CC])

   USE_PTHREAD_MUTEX="yes"
   AC_CHECK_HEADERS([pthread.h],                  [], [USE_PTHREAD_MUTEX="no"])
   AC_CHECK_TYPE([pthread_mutex_t],               [], [USE_PTHREAD_MUTEX="no"], [#include <pthread.h>])
   AC_SEARCH_LIBS([pthread_mutex_destroy],   [c], [], [USE_PTHREAD_MUTEX="no"])
   AC_SEARCH_LIBS([pthread_mutex_init],      [c], [], [USE_PTHREAD_MUTEX="no"])
   AC_SEARCH_LIBS([pthread_mutex_lock],      [c], [], [USE_PTHREAD_MUTEX="no"])
   AC_SEARCH_LIBS([pthread_mutex_trylock],   [c], [], [USE_PTHREAD_MUTEX="no"])
   AC_SEARCH_LIBS([pthread_mutex_unlock],    [c], [], [USE_PTHREAD_MUTEX="no"])
   if test "x${USE_PTHREAD_MUTEX}" == "xno";then
      AC_MSG_ERROR([Missing pthread mutex support required by ${PACKAGE_NAME}.])
   fi
   AC_DEFINE_UNQUOTED(USE_PTHREAD_MUTEX, 1, [Use pthread_mutex_t])

   if test "x${CARRDM_SPINLOCK_TYPE}" == "x";then
      USE_PTHREAD_SPINLOCK="yes"
      AC_CHECK_TYPE([pthread_spinlock_t],               [], [USE_PTHREAD_SPINLOCK="no"], [#include <pthread.h>])
      AC_SEARCH_LIBS([pthread_spin_destroy],       [c], [], [USE_PTHREAD_SPINLOCK="no"])
      AC_SEARCH_LIBS([pthread_spin_init],          [c], [], [USE_PTHREAD_SPINLOCK="no"])
      AC_SEARCH_LIBS([pthread_spin_lock],          [c], [], [USE_PTHREAD_SPINLOCK="no"])
      AC_SEARCH_LIBS([pthread_spin_trylock],       [c], [], [USE_PTHREAD_SPINLOCK="no"])
      AC_SEARCH_LIBS([pthread_spin_unlock],        [c], [], [USE_PTHREAD_SPINLOCK="no"])
      if test "x${USE_PTHREAD_SPINLOCK}" == "xyes";then
         CARRDM_SPINLOCK_TYPE=pthread_spinlock_t
         AC_DEFINE_UNQUOTED(USE_PTHREAD_SPINLOCK, 1, [Use pthread_spinlock_t])
      fi
   fi

   if test "x${CARRDM_SPINLOCK_TYPE}" == "x";then
      USE_OSSPINLOCK="yes"
      AC_CHECK_HEADERS([libkern/OSAtomic.h],            [], [USE_OSSPINLOCK="no"])
      AC_CHECK_TYPE([OSSpinLock],                       [], [USE_OSSPINLOCK="no"], [#include <libkern/OSAtomic.h>])
      AC_SEARCH_LIBS([OSSpinLockLock],             [c], [], [USE_OSSPINLOCK="no"])
      AC_SEARCH_LIBS([OSSpinLockTry],              [c], [], [USE_OSSPINLOCK="no"])
      AC_SEARCH_LIBS([OSSpinLockUnlock],           [c], [], [USE_OSSPINLOCK="no"])
      if test "x${USE_OSSPINLOCK}" == "xyes";then
         CARRDM_SPINLOCK_TYPE=OSSpinLock
         AC_DEFINE_UNQUOTED(USE_OSSPINLOCK, 1, [Use OSSpinLock])
      fi
   fi

   if test "x${CARRDM_SPINLOCK_TYPE}" == "x";then
      CARRDM_SPINLOCK_TYPE=pthread_mutex_t
   fi
   CARRDM_LOCK_TYPE=pthread_mutex_t

])dnl


# end of m4 file
