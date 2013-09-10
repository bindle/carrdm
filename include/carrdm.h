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
#ifndef _CARRDM_H
#define _CARRDM_H 1
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


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef CARRDM_PMARK
#pragma mark - Definitions
#endif

#define CARRDM_MAGIC             0x43415252444d4600LLU // CARRDMF

#define carrdm_type(num)         (num &      0xffffffffffff0000LLU)
#define CARRDM_TYPE_ARRAY        carrdm_type(0x4341525244410000LLU) // CARRDA
#define CARRDM_TYPE_BASE         carrdm_type(0x4341525244420000LLU) // CARRDB
#define CARRDM_TYPE_BASELOCK     carrdm_type(0x43415252444c0000LLU) // CARRDL
#define CARRDM_TYPE_DATA         carrdm_type(0x4341525244440000LLU) // CARRDD
#define CARRDM_TYPE_REALLOCK     carrdm_type(0x4341525244520000LLU) // CARRDR
#define CARRDM_TYPE_RECLOCK      carrdm_type(0x4341525244720000LLU) // CARRDr


#define carrdm_ivar(type, ivar)  (carrdm_type(type) | (ivar & 0xffff))
#define CARRDM_OBJECT_DEF        carrdm_ivar(CARRDM_TYPE_BASE, 0x01)
#define CARRDM_OBJECT_TYPE       carrdm_ivar(CARRDM_TYPE_BASE, 0x02)
#define CARRDM_OBJECT_SIZE       carrdm_ivar(CARRDM_TYPE_BASE, 0x03)
#define CARRDM_DATA_SIZE         carrdm_ivar(CARRDM_TYPE_DATA, 0x01)
#define CARRDM_DATA_BIN          carrdm_ivar(CARRDM_TYPE_DATA, 0x02)


#define CARRDM_FALSE             0
#define CARRDM_TRUE              1


#define CARRDM_SUCCESS           0
#define CARRDM_FAILED            1
#define CARRDM_INVALID_OBJECT    2
#define CARRDM_UNKNOWN_PARAM     3
#define CARRDM_NO_MEMORY         4
#define CARRDM_RANGE             5
#define CARRDM_RONLY             6


//////////////
//          //
//  Macros  //
//          //
//////////////
#ifdef CARRDM_PMARK
#pragma mark - Macros
#endif

#undef CARRDM_BEGIN_C_DECLS
#undef CARRDM_END_C_DECLS
#if defined(__cplusplus) || defined(c_plusplus)
#   define CARRDM_BEGIN_C_DECLS  extern "C" {    ///< exports as C functions
#   define CARRDM_END_C_DECLS    }               ///< exports as C functions
#else
#   define CARRDM_BEGIN_C_DECLS  /* empty */     ///< exports as C functions
#   define CARRDM_END_C_DECLS    /* empty */     ///< exports as C functions
#endif


// Exports function type
#ifdef WIN32
#   ifdef _CARRDM_LIBS_DYNAMIC
#      define _CARRDM_F   extern __declspec(dllexport)   ///< used for library calls
#      define _CARRDM_V   extern __declspec(dllexport)   ///< used for library calls
#   else
#      define _CARRDM_F   extern __declspec(dllimport)   ///< used for library calls
#      define _CARRDM_V   extern __declspec(dllimport)   ///< used for library calls
#   endif
#else
#   ifdef _CARRDM_LIBS_DYNAMIC
#      define _CARRDM_F   /* empty */                    ///< used for library calls
#      define _CARRDM_V   /* empty */                    ///< used for library calls
#   else
#      define _CARRDM_F   extern                         ///< used for library calls
#      define _CARRDM_V   extern                         ///< used for library calls
#   endif
#endif


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef CARRDM_PMARK
#pragma mark - Data Types
#endif

CARRDM_BEGIN_C_DECLS

typedef struct carrdm_array_struct           carrdm_array;
typedef struct carrdm_base_struct            carrdm_base;
typedef struct carrdm_baselock_struct        carrdm_baselock;
typedef struct carrdm_data_struct            carrdm_data;
typedef struct carrdm_definition_struct      carrdm_definition;
typedef struct carrdm_reclock_struct         carrdm_reclock;


struct carrdm_definition_struct
{

   uint64_t                  initialized;
   const carrdm_definition * super_def;
   uint64_t                  type;
   size_t                    size;
   void   (* destroy)(void * objref);
   int    (* getter)(const void * objref, uint64_t valid, void * outval);
   int    (* setter)(void * objref, uint64_t valid, const void * inval);
};


struct carrdm_base_struct
{
   uint64_t                  magic;
   const carrdm_definition * def;
   uint64_t                  retain_count;
};


struct carrdm_baselock_struct
{
   union
   {
      carrdm_base  base;
   } supers;
   void * reallock;
};


struct carrdm_data_struct
{
   union
   {
      carrdm_base  base;
   } supers;
   size_t   size;
   void   * data;
};


struct carrdm_reclock_struct
{
   union
   {
      carrdm_base     base;
      carrdm_baselock baselock;
   } supers;
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef CARRDM_PMARK
#pragma mark - Variables
#endif

_CARRDM_V carrdm_definition carrdm_base_def;
_CARRDM_V carrdm_definition carrdm_baselock_def;
_CARRDM_V carrdm_definition carrdm_data_def;
_CARRDM_V carrdm_definition carrdm_reclock_def;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef CARRDM_PMARK
#pragma mark - Prototypes
#endif

// array functions
_CARRDM_F int            carrdm_array_add(carrdm_array * array, void * ptr, size_t idx);
_CARRDM_F int            carrdm_array_append(carrdm_array * array, void * ptr);
_CARRDM_F size_t         carrdm_array_count(carrdm_array * array);
_CARRDM_F ssize_t        carrdm_array_index(carrdm_array * array, void * ptr);
_CARRDM_F carrdm_array * carrdm_array_initialize(void * mem);
_CARRDM_F carrdm_array * carrdm_array_initialize_with_array(void * mem, carrdm_array * src);
_CARRDM_F carrdm_array * carrdm_array_initialize_with_capacity(void * mem, size_t len);
_CARRDM_F carrdm_base  * carrdm_array_object(carrdm_array * array, size_t idx);
_CARRDM_F int            carrdm_array_remove(carrdm_array * array, size_t idx);


// base functions
_CARRDM_F carrdm_base  * carrdm_base_initialize(void * mem);


// core functions
_CARRDM_F void *         carrdm_alloc(void * mem, const carrdm_definition * def);
_CARRDM_F void           carrdm_destroy(void * mem);
_CARRDM_F int            carrdm_get_value(void * objref, uint64_t valid, void * outval);
_CARRDM_F int            carrdm_is_def(const void * objref, const carrdm_definition * def);
_CARRDM_F int            carrdm_is_object(const void * objref);
_CARRDM_F int            carrdm_is_type(const void * objref, uint64_t type);
_CARRDM_F int            carrdm_is_valid_object(const void * objref);
_CARRDM_F void           carrdm_release(void * objref);
_CARRDM_F void           carrdm_retain(void * objref);
_CARRDM_F uint64_t       carrdm_retain_count(const void * objref);
_CARRDM_F int            carrdm_set_value(void * objref, uint64_t valid, const void * inval);


// data functions
_CARRDM_F carrdm_data  * carrdm_data_initialize(void * mem);
_CARRDM_F carrdm_data  * carrdm_data_initialize_with_data(void * mem, const void * data);
_CARRDM_F carrdm_data  * carrdm_data_initialize_with_memory(void * mem, const void * src, size_t size);
_CARRDM_F carrdm_data  * carrdm_data_initialize_with_size(void * mem, size_t size);
_CARRDM_F int            carrdm_data_resize(carrdm_data * objref, size_t size);


// lock functions
_CARRDM_F carrdm_baselock * carrdm_baselock_initialize(void * mem);
_CARRDM_F int               carrdm_lock(void * objref);
_CARRDM_F carrdm_reclock  * carrdm_reclock_initialize(void * mem);
_CARRDM_F int               carrdm_trylock(void * objref);
_CARRDM_F int               carrdm_unlock(void * objref);


CARRDM_END_C_DECLS
#endif /* end of header */
