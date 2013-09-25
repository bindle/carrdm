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
#define _LIB_LIBCARRDM_CORE_C 1
#include "libcarrdm-core.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef CARRDM_PMARK
#pragma mark - Headers
#endif

#include <assert.h>
#include <string.h>
#include <stdio.h>


////////////////////////
//                    //
//  Inline Functions  //
//                    //
////////////////////////
#ifdef CARRDM_PMARK
#pragma mark - Inline Functions
#endif

extern inline carrdm       * carrdm_cast(void * objref);
extern inline const carrdm * carrdm_ccast(const void * objref);
extern inline int            carrdm_is_def(const void * objref, const carrdm_definition * def);
extern inline int            carrdm_is_object(const void * objref);
extern inline int            carrdm_is_type(const void * objref, uint64_t type);
extern inline int            carrdm_is_valid_object(const void * objref);
extern inline void           carrdm_release(void * objref);
extern inline void           carrdm_retain(void * objref);
extern inline uint64_t       carrdm_retain_count(const void * objref);


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef CARRDM_PMARK
#pragma mark - Variables
#endif

carrdm_magic const carrdm_magic_value = { .number = CARRDM_MAGIC };


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef CARRDM_PMARK
#pragma mark - Functions
#endif

void * carrdm_alloc(void * mem, const carrdm_definition * def)
{
   carrdm_base * objref;

   assert(def       != NULL);
   assert(def->size >= sizeof(carrdm_base));

   if ((objref = mem) == NULL)
   {
      if ((objref = malloc(def->size)) == NULL)
         return(NULL);
      memset(objref, 0, def->size);
      objref->magic.number = CARRDM_MAGIC;
      objref->def          = def;
   };
   
   assert(objref->magic.number == CARRDM_MAGIC);
   assert(objref->def->size    >= def->size);

   return(objref);
}


void * carrdm_copy(const void * objref)
{
   assert(carrdm_is_valid_object(objref) == CARRDM_TRUE);
   return(carrdm_duplicate(objref, CARRDM_FALSE));
}


void * carrdm_deep_copy(const void * objref)
{
   assert(carrdm_is_valid_object(objref) == CARRDM_TRUE);
   return(carrdm_duplicate(objref, CARRDM_TRUE));
}


void carrdm_destroy(void * mem)
{
   carrdm_base             * objref = mem;
   const carrdm_definition * def;

   assert(carrdm_is_object(mem) == CARRDM_TRUE);
   assert(objref->def          != NULL);
   assert(objref->retain_count <= 1);

   if (carrdm_is_object(mem) == CARRDM_FALSE)
      abort();

   def = objref->def;
   while((def))
   {
      if ((def->destroy))
         def->destroy(objref);
      def = def->super_def;
   };

   free(objref);

   return;
}


void * carrdm_duplicate(const void * objref, int deep)
{
   const carrdm_base       * srcref = objref;
   carrdm_base             * dstref;
   void                    * mem;
   const carrdm_definition * def;
   int                       err;

   assert(carrdm_is_valid_object(objref) == CARRDM_TRUE);

   // initializes bare object
   if ((mem = carrdm_alloc(NULL, srcref->def)) == NULL)
      return(NULL);
   if ((dstref = (carrdm_base *)carrdm_base_initialize(mem)) == NULL)
   {
      if (dstref == NULL)
         carrdm_release(mem);
      return(NULL);
   };

   // copies parameters
   def = dstref->def;
   while (def != NULL)
   {
      if ((def->copy))
      {
         if ((err = def->copy(dstref, srcref, deep)) != CARRDM_SUCCESS)
         {
            carrdm_release(dstref);
            return(NULL);
         };
      };
      def = def->super_def;
   };

   return(dstref);
}


int carrdm_get_value(void * ptr, uint64_t valid, void * outval)
{
   carrdm_base             * objref = ptr;
   const carrdm_definition * def;
   int                       err;

   assert(objref != NULL);
   assert(valid  != 0);
   assert(outval != NULL);
   assert(carrdm_is_valid_object(ptr) == CARRDM_TRUE);

   def = objref->def;
   while (def != NULL)
   {
      if ((def->getter))
         if ((err = def->getter(ptr, valid, outval)) != CARRDM_UNKNOWN_PARAM)
            return(err);
      def = def->super_def;
   };

   return(CARRDM_UNKNOWN_PARAM);
}


int carrdm_set_value(void * ptr, uint64_t valid, const void * inval)
{
   carrdm_base             * objref = ptr;
   const carrdm_definition * def;
   int                       err;

   assert(objref != NULL);
   assert(valid  != 0);
   assert(inval  != NULL);
   assert(carrdm_is_valid_object(ptr) == CARRDM_TRUE);

   def = objref->def;
   while (def != NULL)
   {
      if ((def->setter))
         if ((err = def->setter(ptr, valid, inval)) != CARRDM_UNKNOWN_PARAM)
            return(err);
      def = def->super_def;
   };

   return(CARRDM_UNKNOWN_PARAM);
}

/* end of source */
