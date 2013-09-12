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
#define _LIB_LIBCARRDM_BASE_C 1
#include "libcarrdm-base.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef CARRDM_PMARK
#pragma mark - Headers
#endif

#include <assert.h>


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef CARRDM_PMARK
#pragma mark - Prototypes
#endif

int carrdm_base_getter(const void * objref, uint64_t valid, void * outval);


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef APUTILS_PMARK
#pragma mark - Variables
#endif

carrdm_definition carrdm_base_def =
{

   1,                         // initialized;
   NULL,                      // super_def;
   CARRDM_TYPE_BASE,          // type;
   sizeof(carrdm_base),       // size;
   NULL,                      // destroy
   carrdm_base_getter,        // getter
   NULL                       // setter
};


////////////////////////
//                    //
//  Inline Functions  //
//                    //
////////////////////////
#ifdef CARRDM_PMARK
#pragma mark - Inline Functions
#endif

extern inline carrdm_base       * carrdm_base_cast(void * objref);
extern inline const carrdm_base * carrdm_base_ccast(const void * objref);


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef APUTILS_PMARK
#pragma mark - Functions
#endif

int carrdm_base_getter(const void * ptr, uint64_t valid, void * outval)
{
   const carrdm_base * objref = ptr;
   switch(valid)
   {
      case CARRDM_OBJECT_SIZE:
      *((size_t *)outval) = objref->def->size;
      break;

      case CARRDM_OBJECT_TYPE:
      *((uint64_t *)outval) = objref->def->type;
      break;

      default:
      return(CARRDM_UNKNOWN_PARAM);
   };
   return(CARRDM_SUCCESS);
}


carrdm_base * carrdm_base_initialize(void * ptr)
{
   carrdm_base * objref;
   if ((objref = carrdm_alloc(ptr, &carrdm_base_def)) == NULL)
      return(NULL);
   objref->retain_count = 1;
   return(objref);
}

/* end of source */
