/*
 *  C Allocate, Retain, Release, and Destroy Management Functions
 *  Copyright (C) 2013 Bindle Binaries <syzdek@bindlebinaries.com>.
 *
 *  @BINDLE_BINARIES_BSD_LICENSE_START@
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Bindle Binaries nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BINDLE BINARIES BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 *  @BINDLE_BINARIES_BSD_LICENSE_END@
 */
/**
 *  @file
 */
#define _LIB_LIBCARRDM_LOCK_C 1
#include "libcarrdm-lock.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef CARRDM_PMARK
#pragma mark - Headers
#endif

#include <assert.h>
#include <pthread.h>

#include "libcarrdm-base.h"


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef CARRDM_PMARK
#pragma mark - Data Types
#endif

#ifdef USE_PTHREAD_SPINLOCK
typedef pthread_spinlock_t carrdm_spinlock_t;
#elif USE_OSSPINLOCK
#include <libkern/OSAtomic.h>
typedef OSSpinLock carrdm_spinlock_t;
#else
typedef pthread_mutex_t carrdm_spinlock_t;
#endif
typedef pthread_mutex_t carrdm_mutex_t;


struct carrdm_lock_data_struct
{
   union
   {
      carrdm_base     base;
   } supers;
   uint64_t             lock_count;
   pthread_t            lock_thread;
   carrdm_spinlock_t    lock_meta_lock;
   carrdm_mutex_t       lock_lock;
};


typedef struct carrdm_reallock_struct carrdm_reallock;


struct carrdm_reallock_struct
{
   union
   {
      carrdm_base     base;
   } supers;
   uint64_t             lock_recursive;
   uint64_t             lock_count;
   pthread_t            lock_thread;
   carrdm_spinlock_t    lock_meta_lock;
   carrdm_mutex_t       lock_lock;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef CARRDM_PMARK
#pragma mark - Prototypes
#endif
CARRDM_BEGIN_C_DECLS

void carrdm_baselock_destroy(void * objref);

void   carrdm_mutex_destroy(carrdm_mutex_t * mutex);
int    carrdm_mutex_init(carrdm_mutex_t * mutex);
void   carrdm_mutex_lock(carrdm_mutex_t * mutex);
int    carrdm_mutex_trylock(carrdm_mutex_t * mutex);
void   carrdm_mutex_unlock(carrdm_mutex_t * mutex);

carrdm_reallock       * carrdm_reallock_cast(carrdm_base * objref);
const carrdm_reallock * carrdm_reallock_ccast(const carrdm_base * objref);
void                    carrdm_reallock_destroy(void * objref);
carrdm_reallock       * carrdm_reallock_initialize(void * ptr);

void   carrdm_spin_destroy(carrdm_spinlock_t * spinlock);
int    carrdm_spin_init(carrdm_spinlock_t * spinlock);
void   carrdm_spin_lock(carrdm_spinlock_t * spinlock);
int    carrdm_spin_trylock(carrdm_spinlock_t * spinlock);
void   carrdm_spin_unlock(carrdm_spinlock_t * spinlock);


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef APUTILS_PMARK
#pragma mark - Variables
#endif

carrdm_definition carrdm_baselock_def =
{

   1,                            // initialized
   &carrdm_base_def,             // super_def
   CARRDM_TYPE_BASELOCK,         // type
   sizeof(carrdm_baselock),      // size
   carrdm_baselock_destroy,      // destroy
   NULL,                         // getter
   NULL                          // setter
};


carrdm_definition carrdm_reallock_def =
{

   1,                            // initialized
   &carrdm_base_def,             // super_def
   CARRDM_TYPE_BASELOCK,         // type
   sizeof(carrdm_reallock),      // size
   carrdm_reallock_destroy,      // destroy
   NULL,                         // getter
   NULL                          // setter
};


carrdm_definition carrdm_reclock_def =
{

   1,                            // initialized
   &carrdm_baselock_def,         // super_def
   CARRDM_TYPE_RECLOCK,          // type
   sizeof(carrdm_reclock),       // size
   NULL,                         // destroy
   NULL,                         // getter
   NULL                          // setter
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef APUTILS_PMARK
#pragma mark - Baselock Objref Functions
#endif

carrdm_baselock * carrdm_baselock_cast(carrdm_base * objref)
{
   assert(carrdm_is_def(objref, &carrdm_baselock_def) == CARRDM_TRUE);
   return((carrdm_baselock *) objref);
}


const carrdm_baselock * carrdm_baselock_ccast(const carrdm_base * objref)
{
   assert(carrdm_is_def(objref, &carrdm_baselock_def) == CARRDM_TRUE);
   return((const carrdm_baselock *) objref);
}



void carrdm_baselock_destroy(void * ptr)
{
   carrdm_baselock * objref = ptr;

   assert(carrdm_is_def(objref, &carrdm_baselock_def) == CARRDM_FALSE);

   if (objref->reallock != NULL)
      carrdm_release(objref->reallock);

   return;
}


carrdm_baselock * carrdm_baselock_initialize(void * ptr)
{
   void            * mem;
   carrdm_baselock * objref;

   if ((mem = carrdm_alloc(ptr, &carrdm_baselock_def)) == NULL)
      return(NULL);
   if ((objref = (carrdm_baselock *)carrdm_base_initialize(mem)) == NULL)
   {
      if (ptr == NULL)
         carrdm_release(mem);
      return(NULL);
   };

   if ((objref->reallock = carrdm_reallock_initialize(NULL)) == NULL)
   {
      carrdm_destroy(objref);
      return(NULL);
   };

   return(objref);
}


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef APUTILS_PMARK
#pragma mark - Core Lock Functions
#endif

int carrdm_lock(void * ptr)
{
   carrdm_baselock * baselock = ptr;
   carrdm_reallock * reallock;
   pthread_t         thread;

   assert(carrdm_is_def(baselock, &carrdm_baselock_def) == CARRDM_TRUE);
   reallock = baselock->reallock;

   if (reallock->lock_recursive == CARRDM_FALSE)
   {
      carrdm_mutex_lock(&reallock->lock_lock);
      return(0);
   };

   thread = pthread_self();

   carrdm_spin_lock(&reallock->lock_meta_lock);
   if ((pthread_equal(thread, reallock->lock_thread) != 0) && (reallock->lock_count != 0))
   {
      reallock->lock_count++;
      carrdm_spin_unlock(&reallock->lock_meta_lock);
      return(0);
   };
   carrdm_spin_unlock(&reallock->lock_meta_lock);

   carrdm_mutex_lock(&reallock->lock_lock);

   carrdm_spin_lock(&reallock->lock_meta_lock);
   reallock->lock_thread = pthread_self();
   reallock->lock_count++;
   carrdm_spin_unlock(&reallock->lock_meta_lock);

   return(0);
}


int carrdm_trylock(void * ptr)
{
   carrdm_baselock * baselock = ptr;
   carrdm_reallock * reallock;
   int               err;
   pthread_t         thread;

   assert(carrdm_is_def(baselock, &carrdm_baselock_def) == CARRDM_TRUE);
   reallock = baselock->reallock;

   if (reallock->lock_recursive == CARRDM_TRUE)
      return(carrdm_mutex_trylock(&reallock->lock_lock));

   thread = pthread_self();

   carrdm_spin_lock(&reallock->lock_meta_lock);
   if ((err = carrdm_mutex_trylock(&reallock->lock_lock)) != 0)
   {
      if ((pthread_equal(thread, reallock->lock_thread) != 0) && (reallock->lock_count != 0))
      {
         reallock->lock_count++;
         err = 0;
      };
   }
   else
   {
      reallock->lock_thread = thread;
      reallock->lock_count++;
   };
   carrdm_spin_unlock(&reallock->lock_meta_lock);

   return(err);
}


int carrdm_unlock(void * ptr)
{
   carrdm_baselock * baselock = ptr;
   carrdm_reallock * reallock;

   assert(carrdm_is_def(baselock, &carrdm_baselock_def) == CARRDM_TRUE);
   reallock = baselock->reallock;

   if (reallock->lock_recursive == CARRDM_FALSE)
   {
      carrdm_mutex_unlock(&reallock->lock_lock);
      return(0);
   };

   carrdm_spin_lock(&reallock->lock_meta_lock);
   if (reallock->lock_count != 0)
      reallock->lock_count--;
   if (reallock->lock_count == 0)
   {
      carrdm_spin_unlock(&reallock->lock_meta_lock);
      carrdm_mutex_unlock(&reallock->lock_lock);
      return(0);
   };
   carrdm_spin_unlock(&reallock->lock_meta_lock);
   return(0);
}



/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef APUTILS_PMARK
#pragma mark - Reallock Objref Functions
#endif

void carrdm_reallock_destroy(void * ptr)
{
   carrdm_reallock * objref = ptr;

   assert(carrdm_is_def(objref, &carrdm_reallock_def) == CARRDM_FALSE);

   carrdm_spin_destroy(&objref->lock_meta_lock);
   carrdm_mutex_destroy(&objref->lock_lock);

   return;
}


carrdm_reallock * carrdm_reallock_cast(carrdm_base * objref)
{
   assert(carrdm_is_def(objref, &carrdm_reallock_def) == CARRDM_TRUE);
   return((carrdm_reallock *) objref);
}


const carrdm_reallock * carrdm_reallock_ccast(const carrdm_base * objref)
{
   assert(carrdm_is_def(objref, &carrdm_reallock_def) == CARRDM_TRUE);
   return((const carrdm_reallock *) objref);
}


carrdm_reallock * carrdm_reallock_initialize(void * ptr)
{
   void            * mem;
   carrdm_reallock * objref;

   if ((mem = carrdm_alloc(ptr, &carrdm_baselock_def)) == NULL)
      return(NULL);
   if ((objref = (carrdm_reallock *)carrdm_base_initialize(mem)) == NULL)
   {
      if (ptr == NULL)
         carrdm_release(mem);
      return(NULL);
   };

   carrdm_spin_init(&objref->lock_meta_lock);
   carrdm_mutex_init(&objref->lock_lock);

   return(objref);
}


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef APUTILS_PMARK
#pragma mark - Reclock Objref Functions
#endif


carrdm_reclock * carrdm_reclock_cast(carrdm_base * objref)
{
   assert(carrdm_is_def(objref, &carrdm_reclock_def) == CARRDM_TRUE);
   return((carrdm_reclock *) objref);
}


const carrdm_reclock * carrdm_reclock_ccast(const carrdm_base * objref)
{
   assert(carrdm_is_def(objref, &carrdm_reclock_def) == CARRDM_TRUE);
   return((const carrdm_reclock *) objref);
}


carrdm_reclock * carrdm_reclock_initialize(void * ptr)
{
   carrdm_reclock  * objref;
   carrdm_reallock * reallock;

   if ((ptr = carrdm_alloc(ptr, &carrdm_baselock_def)) == NULL)
      return(NULL);

   if ((objref = (carrdm_reclock *)carrdm_base_initialize(ptr)) == NULL)
   {
      carrdm_destroy(ptr);
      return(NULL);
   };

   reallock                 = objref->supers.baselock.reallock;
   reallock->lock_recursive = CARRDM_TRUE;

   return(objref);
}


///////////////////////
//                   //
//  Low-level Locks  //
//                   //
///////////////////////
#ifdef APUTILS_PMARK
#pragma mark - Low-level Locks
#endif

#ifdef USE_PTHREAD_SPINLOCK
void carrdm_spin_destroy(carrdm_spinlock_t * spinlock) { pthread_spin_destroy(spinlock); }
int  carrdm_spin_init(carrdm_spinlock_t * spinlock)    { return(pthread_spin_init(spinlock, NULL)); }
void carrdm_spin_lock(carrdm_spinlock_t * spinlock)    { pthread_spin_lock(spinlock); }
int  carrdm_spin_trylock(carrdm_spinlock_t * spinlock) { return(pthread_spin_trylock(spinlock)); }
void carrdm_spin_unlock(carrdm_spinlock_t * spinlock)  { pthread_spin_unlock(spinlock); }
#elif USE_OSSPINLOCK
#include <libkern/OSAtomic.h>
void carrdm_spin_destroy(carrdm_spinlock_t * spinlock) { assert(spinlock != NULL); }
int  carrdm_spin_init(carrdm_spinlock_t * spinlock)    { *spinlock = OS_SPINLOCK_INIT; return(0); }
void carrdm_spin_lock(carrdm_spinlock_t * spinlock)    { OSSpinLockLock(spinlock); }
int  carrdm_spin_trylock(carrdm_spinlock_t * spinlock) { return(OSSpinLockTry(spinlock)); }
void carrdm_spin_unlock(carrdm_spinlock_t * spinlock)  { OSSpinLockUnlock(spinlock); }
#else
void carrdm_spin_destroy(carrdm_spinlock_t * spinlock) { pthread_mutex_destroy(spinlock); }
int  carrdm_spin_init(carrdm_spinlock_t * spinlock)    { return(pthread_mutex_init(spinlock, NULL)); }
void carrdm_spin_lock(carrdm_spinlock_t * spinlock)    { pthread_mutex_lock(spinlock); }
int  carrdm_spin_trylock(carrdm_spinlock_t * spinlock) { return(pthread_mutex_trylock(spinlock)); }
void carrdm_spin_unlock(carrdm_spinlock_t * spinlock)  { pthread_mutex_unlock(spinlock); }
#endif
void carrdm_mutex_destroy(carrdm_mutex_t * mutex) { pthread_mutex_destroy(mutex); }
int  carrdm_mutex_init(carrdm_mutex_t * mutex)    { return(pthread_mutex_init(mutex, NULL)); }
void carrdm_mutex_lock(carrdm_mutex_t * mutex)    { pthread_mutex_lock(mutex); }
int  carrdm_mutex_trylock(carrdm_mutex_t * mutex) { return(pthread_mutex_trylock(mutex)); }
void carrdm_mutex_unlock(carrdm_mutex_t * mutex)  { pthread_mutex_unlock(mutex); }


/* end of source */
