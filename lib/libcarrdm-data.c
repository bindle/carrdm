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
#define _LIB_LIBCARRDM_DATA_C 1
#include "libcarrdm-data.h"

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


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef CARRDM_PMARK
#pragma mark - Prototypes
#endif

void carrdm_data_destroy(void * ptr);
int  carrdm_data_getter(const void * ptr, uint64_t valid, void * outval);
int  carrdm_data_setter(void * ptr, uint64_t valid, const void * inval);


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef APUTILS_PMARK
#pragma mark - Variables
#endif

carrdm_definition carrdm_data_def =
{

   1,                         // initialized;
   &carrdm_data_def,          // super_def;
   CARRDM_TYPE_DATA,          // type;
   sizeof(carrdm_data),       // size;
   carrdm_data_destroy,       // destroy
   carrdm_data_getter,        // getter
   carrdm_data_setter         // setter
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef APUTILS_PMARK
#pragma mark - Functions
#endif

void carrdm_data_destroy(void * ptr)
{
   carrdm_data * objref = ptr;

   assert(carrdm_is_def(objref, &carrdm_data_def) == CARRDM_TRUE);

   if (objref->data != NULL)
      free(objref->data);
   objref->data = NULL;
   objref->size = 0;

   return;
}


int carrdm_data_getter(const void * ptr, uint64_t valid, void * outval)
{
   const carrdm_data * objref = ptr;
   switch(valid)
   {
      case CARRDM_DATA_BIN:
      *((void **)outval) = objref->data;
      break;

      case CARRDM_DATA_SIZE:
      *((size_t *)outval) = objref->size;
      break;

      default:
      return(CARRDM_UNKNOWN_PARAM);
   };
   return(CARRDM_SUCCESS);
}


void * carrdm_data_initialize(void * ptr)
{
   void        * mem;
   carrdm_base * objref;

   if ((mem = carrdm_alloc(ptr, &carrdm_data_def)) == NULL)
      return(NULL);
   if ((objref = carrdm_base_initialize(mem)) == NULL)
   {
      if (ptr == NULL)
         carrdm_release(mem);
      return(NULL);
   };

   return(carrdm_base_initialize(ptr));
}


int carrdm_data_resize(carrdm_data * objref, size_t size)
{
   void * ptr;

   assert(carrdm_is_def(objref, &carrdm_data_def) == CARRDM_TRUE);

   if (size < objref->size)
      return(CARRDM_SUCCESS);

   if ((ptr = realloc(objref->data, size)) == NULL)
      return(CARRDM_NO_MEMORY);
   objref->data = ptr;
   objref->size = size;

   return(CARRDM_SUCCESS);
}


int carrdm_data_setter(void * ptr, uint64_t valid, const void * inval)
{
   carrdm_data * objref = ptr;
   switch(valid)
   {
      case CARRDM_DATA_BIN:
      return(CARRDM_RONLY);

      case CARRDM_DATA_SIZE:
      return(carrdm_data_resize(objref, *((size_t *)inval)));

      default:
      return(CARRDM_UNKNOWN_PARAM);
   };
   return(CARRDM_SUCCESS);
}

/* end of source */
