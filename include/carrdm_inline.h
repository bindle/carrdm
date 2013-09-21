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
#ifndef _CARRDM_INLINE_H
#define _CARRDM_INLINE_H 1
#undef CARRDM_PMARK


///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef CARRDM_PMARK
#pragma mark - Headers
#endif

#include <inttypes.h>
#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>


////////////////////////
//                    //
//  Inline Functions  //
//                    //
////////////////////////
#ifdef CARRDM_PMARK
#pragma mark - Inline Functions
#endif

_CARRDM_I carrdm_array * carrdm_array_cast(void * objref)
{
   if (carrdm_is_def(objref, &carrdm_array_def) == CARRDM_TRUE)
      return((carrdm_array *)objref);
   return(NULL);
}


_CARRDM_I const carrdm_array * carrdm_array_ccast(const void * objref)
{
   if (carrdm_is_def(objref, &carrdm_array_def) == CARRDM_TRUE)
      return((const carrdm_array *)objref);
   return(NULL);
}


_CARRDM_I carrdm_base * carrdm_base_cast(void * objref)
{
   return((carrdm_base *)carrdm_cast(objref));
}


_CARRDM_I const carrdm_base * carrdm_base_ccast(const void * objref)
{
   return((const carrdm_base *)carrdm_ccast(objref));
}


_CARRDM_I carrdm_baselock * carrdm_baselock_cast(void * objref)
{
   if (carrdm_is_def(objref, &carrdm_baselock_def) == CARRDM_TRUE)
      return((carrdm_baselock *)objref);
   return(NULL);
}


_CARRDM_I const carrdm_baselock * carrdm_baselock_ccast(const void * objref)
{
   if (carrdm_is_def(objref, &carrdm_baselock_def) == CARRDM_TRUE)
      return((const carrdm_baselock *)objref);
   return(NULL);
}


_CARRDM_I carrdm * carrdm_cast(void * objref)
{
   if (carrdm_is_def(objref, &carrdm_base_def) == CARRDM_TRUE)
      return((carrdm *)objref);
   return(NULL);
}


_CARRDM_I const carrdm * carrdm_ccast(const void * objref)
{
   if (carrdm_is_def(objref, &carrdm_base_def) == CARRDM_TRUE)
      return((const carrdm *)objref);
   return(NULL);
}


_CARRDM_I carrdm_data * carrdm_data_cast(void * objref)
{
   if (carrdm_is_def(objref, &carrdm_data_def) == CARRDM_TRUE)
      return((carrdm_data *)objref);
   return(NULL);
}


_CARRDM_I const carrdm_data  * carrdm_data_ccast(const void * objref)
{
   if (carrdm_is_def(objref, &carrdm_data_def) == CARRDM_TRUE)
      return((const carrdm_data *)objref);
   return(NULL);
}


_CARRDM_I int carrdm_is_def(const void * ptr, const carrdm_definition * def)
{
   const carrdm_base       * objref;
   const carrdm_definition * obj_def;

   assert(def != NULL);
   assert(carrdm_is_object(ptr) == CARRDM_TRUE);

   objref  = (const carrdm_base *) ptr;
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


_CARRDM_I int carrdm_is_object(const void * ptr)
{
   const carrdm_base       * objref;
   const carrdm_definition * obj_def;
   int   count;

   objref  = (const carrdm_base *) ptr;

   if (objref == NULL)
      return(CARRDM_FALSE);
   if (objref->magic != CARRDM_MAGIC)
      return(CARRDM_FALSE);
   if (objref->def == NULL)
      return(CARRDM_FALSE);
   if (objref->def->def_size < sizeof(carrdm_definition))
      return(CARRDM_FALSE);
   if ((CARRDM_API_CURRENT - CARRDM_API_AGE)  > objref->def->def_api_current)
      return(CARRDM_FALSE);
   if ((objref->def->def_api_current - objref->def->def_api_age)  > CARRDM_API_CURRENT)
      return(CARRDM_FALSE);
   if (objref->def->size < sizeof(carrdm_base))
      return(CARRDM_FALSE);

   obj_def = objref->def;

   count = 0;
   while((obj_def->super_def != NULL) && (++count < 100))
   {
      if (obj_def->size < obj_def->super_def->size)
         return(CARRDM_FALSE);
      if ((obj_def->is_object))
         if (obj_def->is_object(ptr, obj_def) == CARRDM_FALSE)
            return(CARRDM_FALSE);
      obj_def = obj_def->super_def;
   };

   if (obj_def->super_def != NULL)
      return(CARRDM_FALSE);
   if (obj_def->type != CARRDM_TYPE_BASE)
      return(CARRDM_FALSE);

   return(CARRDM_TRUE);
}


_CARRDM_I int carrdm_is_type(const void * ptr, uint64_t type)
{
   const carrdm_base       * objref;
   const carrdm_definition * obj_def;

   assert(type != 0);
   assert(carrdm_is_object(ptr) == CARRDM_TRUE);

   objref  = (const carrdm_base *) ptr;
   obj_def = objref->def;

   while((obj_def))
   {
      if (obj_def->type == type)
         return(CARRDM_TRUE);
      obj_def = obj_def->super_def;
   };

   return(CARRDM_FALSE);
}


_CARRDM_I int carrdm_is_valid_object(const void * ptr)
{
   const carrdm_base * objref = (const carrdm_base *) ptr;
   if (carrdm_is_object(objref) == CARRDM_FALSE)
      return(CARRDM_FALSE);
   if (objref->retain_count < 1)
      return(CARRDM_FALSE);
   return(CARRDM_TRUE);
}


_CARRDM_I carrdm_reclock * carrdm_reclock_cast(void * objref)
{
   if (carrdm_is_def(objref, &carrdm_reclock_def) == CARRDM_TRUE)
      return((carrdm_reclock *)objref);
   return(NULL);
}


_CARRDM_I const carrdm_reclock  * carrdm_reclock_ccast(const void * objref)
{
   if (carrdm_is_def(objref, &carrdm_reclock_def) == CARRDM_TRUE)
      return((const carrdm_reclock *)objref);
   return(NULL);
}


_CARRDM_I void carrdm_release(void * ptr)
{
   carrdm_base * objref = (carrdm_base *) ptr;
   uint64_t count;
   assert(carrdm_is_valid_object(ptr) == CARRDM_TRUE);
   if ((count = __sync_fetch_and_sub(&objref->retain_count, 1)) <= 1)
   {
      carrdm_destroy(objref);
      return;
   };
   return;
}


_CARRDM_I void carrdm_retain(void * ptr)
{
   carrdm_base * objref = (carrdm_base *) ptr;
   assert(carrdm_is_valid_object(ptr) == CARRDM_TRUE);
   __sync_fetch_and_add(&objref->retain_count, 1);
   return;
}


_CARRDM_I uint64_t carrdm_retain_count(const void * ptr)
{
   const carrdm_base * objref = (carrdm_base *) ptr;
   assert(carrdm_is_valid_object(ptr) == CARRDM_TRUE);
   return(__sync_fetch_and_add(&objref->retain_count, 0));
}


#endif /* end of header */
