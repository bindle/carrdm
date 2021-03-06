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
#define _LIB_LIBCARRDM_ARRAY_C 1
#include "libcarrdm-array.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef CARRDM_PMARK
#pragma mark - Headers
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "libcarrdm-core.h"


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef CARRDM_PMARK
#pragma mark - Data Types
#endif

struct carrdm_array_struct
{
   union
   {
      carrdm_magic   magic;
      carrdm_base    base;
   } sup;
   carrdm_base ** list;
   size_t         len;
   size_t         capacity;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef CARRDM_PMARK
#pragma mark - Prototypes
#endif

int  carrdm_array_copy(void * dst, const void * src, int deep);
void carrdm_array_destroy(void * ptr);


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef CARRDM_PMARK
#pragma mark - Variables
#endif

carrdm_definition carrdm_array_def =
{
   sizeof(carrdm_definition), // def_size
   CARRDM_API_CURRENT,        // def_api_current
   CARRDM_API_REVISION,       // def_api_revision
   CARRDM_API_AGE,            // def_api_age
   CARRDM_API_CURRENT,        // obj_api_current
   CARRDM_API_REVISION,       // obj_api_revision
   CARRDM_API_AGE,            // obj_api_age
   NULL,                      // obj_data
   0LLU,                      // obj_data_size
   &carrdm_base_def,          // super_def;
   CARRDM_TYPE_ARRAY,         // type;
   sizeof(carrdm_array),      // size;
   carrdm_array_destroy,      // destroy
   NULL,                      // getter
   NULL,                      // setter
   NULL,                      // is_object
   NULL                       // duplicate
};


////////////////////////
//                    //
//  Inline Functions  //
//                    //
////////////////////////
#ifdef CARRDM_PMARK
#pragma mark - Inline Functions
#endif

extern inline carrdm_array       * carrdm_array_cast(void * objref);
extern inline const carrdm_array * carrdm_array_ccast(const void * objref);


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef CARRDM_PMARK
#pragma mark - Functions
#endif

int carrdm_array_copy(void * dst, const void * src, int deep)
{
   carrdm_array       * dstref = dst;
   const carrdm_array * srcref = src;
   carrdm_base        * objref;
   size_t               pos;
   int                  err;

   assert(carrdm_is_def(dst, &carrdm_array_def) == CARRDM_TRUE);
   assert(carrdm_is_def(src, &carrdm_array_def) == CARRDM_TRUE);

   for(pos = 0; pos < srcref->len; pos++)
   {
      if ((objref = carrdm_array_object(srcref, pos)) == NULL)
         return(CARRDM_FAILED);
      if (deep == CARRDM_TRUE)
         if ((objref = carrdm_duplicate(objref, CARRDM_TRUE)) == NULL)
            return(CARRDM_FAILED);
      if ((err = carrdm_array_append(dstref, objref)) != CARRDM_SUCCESS)
         return(err);
   };

   if (dstref->len != srcref->len)
      return(CARRDM_FAILED);

   return(CARRDM_SUCCESS);
}


int carrdm_array_add(carrdm_array * array, void * ptr, size_t idx)
{
   carrdm_base  * base = ptr;
   size_t         size;
   carrdm_base ** list;
   size_t         pos;

   assert(carrdm_is_def(array, &carrdm_array_def) == CARRDM_TRUE);
   assert(carrdm_is_valid_object(base)            == CARRDM_TRUE);
   assert(array->len >= idx);

   // allocates memory
   if ((array->len+1) > array->capacity)
   {
      size = sizeof(carrdm_base *) * (array->len + 1);
      if ((list = realloc(array->list, size)) == NULL)
         return(CARRDM_NO_MEMORY);
      array->list     = list;
      array->capacity = array->len + 1;
   };

   // shifts contents of array
   for(pos = array->len; pos > idx; pos--)
      array->list[pos] = array->list[pos-1];

   // add obj to array
   carrdm_retain(base);
   array->list[idx] = base;
   array->len++;

   return(CARRDM_SUCCESS);
}


int carrdm_array_append(carrdm_array * array, void * ptr)
{
   assert(carrdm_is_def(array, &carrdm_array_def) == CARRDM_TRUE);
   assert(carrdm_is_valid_object(ptr)             == CARRDM_TRUE);
   return(carrdm_array_add(array, ptr, array->len));
}


size_t carrdm_array_count(carrdm_array * array)
{
   assert(carrdm_is_def(array, &carrdm_array_def) == CARRDM_TRUE);
   return(array->len);
}


void carrdm_array_destroy(void * ptr)
{
   carrdm_array * objref = ptr;
   size_t         pos;

   assert(carrdm_is_def(objref, &carrdm_array_def) == CARRDM_TRUE);

   for(pos = 0; pos < objref->len; pos++)
      carrdm_release(objref->list[pos]);
   if ((objref->list))
      free(objref->list);

   return;
}


carrdm_base * carrdm_array_object(const carrdm_array * array, size_t idx)
{
   assert(carrdm_is_def(array, &carrdm_array_def) == CARRDM_TRUE);
   if (idx >= array->len)
      return(NULL);
   return(array->list[idx]);
}


int carrdm_array_remove(carrdm_array * array, size_t idx)
{
   size_t pos;
   assert(carrdm_is_def(array, &carrdm_array_def) == CARRDM_TRUE);
   if (idx >= array->len)
      return(CARRDM_RANGE);
   carrdm_release(array->list[idx]);
   array->len--;
   for(pos = idx; pos < array->len; pos++)
      array->list[pos] = array->list[pos+1];
   if ((array->list))
      array->list[array->len] = NULL;
   return(CARRDM_SUCCESS);
}


ssize_t carrdm_array_index(const carrdm_array * array, void * ptr)
{
   size_t pos;
   assert(carrdm_is_def(array, &carrdm_array_def) == CARRDM_TRUE);
   for(pos = 0; pos < array->len; pos++)
      if (array->list[pos] == ptr)
         return((ssize_t)pos);
   return(-1);
}


carrdm_array * carrdm_array_initialize(void * ptr)
{
   return(carrdm_array_initialize_with_capacity(ptr, 0));
}


carrdm_array * carrdm_array_initialize_with_array(void * mem, carrdm_array * src)
{
   carrdm_array * objref;
   size_t         pos;
   assert(carrdm_is_def(src, &carrdm_array_def) == CARRDM_TRUE);
   if ((objref = carrdm_array_initialize_with_capacity(mem, src->len)) == NULL)
      return(NULL);
   for(pos = 0; pos < src->len; pos++)
   {
      carrdm_retain(src->list[pos]);
      objref->list[pos] = src->list[pos];
   };
   return(objref);
}


carrdm_array * carrdm_array_initialize_with_capacity(void * ptr, size_t len)
{
   void         * mem;
   carrdm_array * array;
   size_t         size;

   if ((mem = carrdm_alloc(ptr, &carrdm_array_def)) == NULL)
      return(NULL);
   if ((array = (carrdm_array *)carrdm_base_initialize(mem)) == NULL)
   {
      if (ptr == NULL)
         carrdm_release(mem);
      return(NULL);
   };

   if (len > 0)
   {
      size = sizeof(carrdm_array *) * len;
      if ((array->list = malloc(size)) == NULL)
      {
         carrdm_release(array);
         return(NULL);
      };
      memset(array->list, 0, size);
      array->capacity = len;
   };

   return(array);
}

/* end of source */
