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


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef APUTILS_PMARK
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
      objref->magic = CARRDM_MAGIC;
      objref->def   = def;
   };
   
   assert(objref->magic     == CARRDM_MAGIC);
   assert(objref->def->size >= def->size);

   return(objref);
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
      if ((def->getter))
         if ((err = def->getter(ptr, valid, outval)) != CARRDM_UNKNOWN_PARAM)
            return(err);

   return(CARRDM_UNKNOWN_PARAM);
}


int carrdm_is_def(const void * ptr, const carrdm_definition * def)
{
   const carrdm_base       * objref;
   const carrdm_definition * obj_def;

   assert(def != NULL);
   assert(carrdm_is_object(ptr) == CARRDM_TRUE);

   objref  = ptr;
   obj_def = objref->def;

   while((obj_def))
   {
      if (obj_def->type == def->type)
         if (obj_def->size == def->size)
            return(CARRDM_TRUE);
      obj_def = obj_def->super_def;
   };

   return(CARRDM_FALSE);
}


int carrdm_is_object(const void * ptr)
{
   const carrdm_base       * objref = ptr;
   const carrdm_definition * def;
   int   count;

   if (objref == NULL)
      return(CARRDM_FALSE);
   if (objref->magic != CARRDM_MAGIC)
      return(CARRDM_FALSE);
   if (objref->def == NULL)
      return(CARRDM_FALSE);
   if (objref->def->size < sizeof(carrdm_base))
      return(CARRDM_FALSE);

   def   = objref->def;
   count = 0;
   while((def->super_def != NULL) && (++count < 100))
   {
      if (def->size < def->super_def->size)
         return(CARRDM_FALSE);
      def = def->super_def;
   };

   if (def->super_def != NULL)
      return(CARRDM_FALSE);
   if (def->type != CARRDM_TYPE_BASE)
      return(CARRDM_FALSE);

   return(CARRDM_TRUE);
}


int carrdm_is_type(const void * ptr, uint64_t type)
{
   const carrdm_base       * objref;
   const carrdm_definition * obj_def;

   assert(type != 0);
   assert(carrdm_is_object(ptr) == CARRDM_TRUE);

   objref  = ptr;
   obj_def = objref->def;

   while((obj_def))
   {
      if (obj_def->type == type)
         return(CARRDM_TRUE);
      obj_def = obj_def->super_def;
   };

   return(CARRDM_FALSE);
}


int carrdm_is_valid_object(const void * ptr)
{
   const carrdm_base * objref = ptr;
   if (carrdm_is_object(objref) == CARRDM_FALSE)
      return(CARRDM_FALSE);
   if (objref->retain_count < 1)
      return(CARRDM_FALSE);
   return(CARRDM_TRUE);
}


void carrdm_release(void * ptr)
{
   carrdm_base * objref = ptr;
   size_t count;
   assert(carrdm_is_valid_object(ptr) == CARRDM_TRUE);
   if ((count = __sync_fetch_and_sub(&objref->retain_count, 1)) <= 1)
   {
      carrdm_destroy(objref);
      return;
   };
   return;
}


void carrdm_retain(void * ptr)
{
   carrdm_base * objref = ptr;
   assert(carrdm_is_valid_object(ptr) == CARRDM_TRUE);
   __sync_fetch_and_add(&objref->retain_count, 1);
   return;
}


uint64_t carrdm_retain_count(const void * ptr)
{
   const carrdm_base * objref = ptr;
   assert(carrdm_is_valid_object(ptr) == CARRDM_TRUE);
   return(__sync_fetch_and_add(&objref->retain_count, 0));
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
      if ((def->setter))
         if ((err = def->setter(ptr, valid, inval)) != CARRDM_UNKNOWN_PARAM)
            return(err);

   return(CARRDM_UNKNOWN_PARAM);
}

/* end of source */
