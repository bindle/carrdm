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
#define _CARRDM_TESTS_TEST_ARRAY01_C 1

#include <inttypes.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <carrdm.h>
#include <assert.h>

#define carrdm_log(...) carrdm_test_log(__FILE__, __LINE__, __VA_ARGS__)
int carrdm_test_log(const char * file, int line, const char * fmt, ...);

int main(void);


int carrdm_test_log(const char * file, int line, const char * fmt, ...)
{
   va_list args;
   fprintf(stderr, "%s: %i: ", file, line);
   va_start (args, fmt);
   vfprintf(stderr, fmt, args);
   va_end (args);
   return(1);
}


int main(void)
{
   carrdm_array * list;
   carrdm_base  * objref;

   assert((list = carrdm_array_initialize(NULL)) != NULL);
   assert(carrdm_retain_count(list) == 1);

   assert((objref = carrdm_base_initialize(NULL)) != NULL);
   assert(carrdm_retain_count(objref) == 1);

   carrdm_array_append(list, objref);
   assert(carrdm_retain_count(objref) == 2);

   carrdm_release(objref);
   assert(carrdm_retain_count(objref) == 1);

   carrdm_retain(objref);
   assert(carrdm_retain_count(objref) == 2);

   carrdm_release(list);
   assert(carrdm_retain_count(objref) == 1);

   carrdm_release(objref);

   return(0);
}

/* end of source */
