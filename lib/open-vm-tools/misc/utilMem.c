//depot/vdi/vdm30/framework/cdk/lib/open-vm-tools/misc/utilMem.c#1 - add change 23244 (text)
/*********************************************************
 * Copyright (C) 2009 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation version 2.1 and no later version.
 *
 * This program is released with an additional exemption that
 * compiling, linking, and/or using the OpenSSL libraries with this
 * program is allowed.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the Lesser GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.
 *
 *********************************************************/

/*
 * utilMem.c --
 *
 *    misc util functions
 */

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "vm_assert.h"

/*
 *-----------------------------------------------------------------------------
 *
 * Util_SafeInternalMalloc --
 *      Helper function for malloc
 *
 * Results:
 *      Pointer to the dynamically allocated memory.
 *
 * Side effects:
 *      May Panic if ran out of memory.
 *
 *-----------------------------------------------------------------------------
 */

void *
Util_SafeInternalMalloc(int bugNumber,  // IN:
                        size_t size,    // IN:
			char *file,     // IN:
                        int lineno)     // IN:
{
   void *result = malloc(size);

   if (result == NULL && size != 0) {
      if (bugNumber == -1) {
         Panic("Unrecoverable memory allocation failure at %s:%d\n",
               file, lineno);
      } else {
         Panic("Unrecoverable memory allocation failure at %s:%d.  Bug "
               "number: %d\n", file, lineno, bugNumber);
      }
   }
   return result;
}


/*
 *-----------------------------------------------------------------------------
 *
 * Util_SafeInternalRealloc --
 *      Helper function for realloc
 *
 * Results:
 *      Pointer to the dynamically allocated memory.
 *
 * Side effects:
 *      May Panic if ran out of memory.
 *
 *-----------------------------------------------------------------------------
 */

void *
Util_SafeInternalRealloc(int bugNumber,  // IN:
                         void *ptr,      // IN:
                         size_t size,    // IN:
                         char *file,     // IN:
                         int lineno)     // IN:
{
   void *result = realloc(ptr, size);

   if (result == NULL && size != 0) {
      if (bugNumber == -1) {
         Panic("Unrecoverable memory allocation failure at %s:%d\n",
               file, lineno);
      } else {
         Panic("Unrecoverable memory allocation failure at %s:%d.  Bug "
               "number: %d\n", file, lineno, bugNumber);
      }
   }
   return result;
}


/*
 *-----------------------------------------------------------------------------
 *
 * Util_SafeInternalCalloc --
 *      Helper function for calloc
 *
 * Results:
 *      Pointer to the dynamically allocated memory.
 *
 * Side effects:
 *      May Panic if ran out of memory.
 *
 *-----------------------------------------------------------------------------
 */

void *
Util_SafeInternalCalloc(int bugNumber,  // IN:
                        size_t nmemb,   // IN:
                        size_t size,    // IN:
			char *file,     // IN:
                        int lineno)     // IN:
{
   void *result = calloc(nmemb, size);

   if (result == NULL && nmemb != 0 && size != 0) {
      if (bugNumber == -1) {
         Panic("Unrecoverable memory allocation failure at %s:%d\n",
               file, lineno);
      } else {
         Panic("Unrecoverable memory allocation failure at %s:%d.  Bug "
               "number: %d\n", file, lineno, bugNumber);
      }
   }
   return result;
}


/*
 *-----------------------------------------------------------------------------
 *
 * Util_SafeInternalStrdup --
 *      Helper function for strdup
 *
 * Results:
 *      Pointer to the dynamically allocated, duplicate string
 *
 * Side effects:
 *      May Panic if ran out of memory.
 *
 *-----------------------------------------------------------------------------
 */

char *
Util_SafeInternalStrdup(int bugNumber,  // IN:
                        const char *s,  // IN:
                        char *file,     // IN:
                        int lineno)     // IN:
{
   char *result;

   if (s == NULL) {
      return NULL;
   }

#if defined(_WIN32)
   if ((result = _strdup(s)) == NULL) {
#else
   if ((result = strdup(s)) == NULL) {
#endif
      if (bugNumber == -1) {
         Panic("Unrecoverable memory allocation failure at %s:%d\n",
               file, lineno);
      } else {
         Panic("Unrecoverable memory allocation failure at %s:%d.  Bug "
               "number: %d\n", file, lineno, bugNumber);
      }
   }

   return result;
}


/*
 *-----------------------------------------------------------------------------
 *
 * UtilSafeStrndupInternal --
 *
 *      Returns a string consisting of first n characters of 's' if 's' has
 *      length >= 'n', otherwise returns a string duplicate of 's'.
 *
 * Results:
 *      Pointer to the duplicated string.
 *
 * Side effects:
 *      May Panic if ran out of memory.
 *
 *-----------------------------------------------------------------------------
 */

char *
Util_SafeInternalStrndup(int bugNumber,  // IN:
                         const char *s,  // IN:
                         size_t n,       // IN:
                         char *file,     // IN:
                         int lineno)     // IN:
{
   size_t size;
   char *copy;
   const char *null;

   if (s == NULL) {
      return NULL;
   }

   null = (char *) memchr(s, '\0', n);
   size = null ? null - s: n;
   copy = (char *) malloc(size + 1);

   if (copy == NULL) {
      if (bugNumber == -1) {
         Panic("Unrecoverable memory allocation failure at %s:%d\n",
               file, lineno);
      } else {
         Panic("Unrecoverable memory allocation failure at %s:%d.  Bug "
               "number: %d\n", file, lineno, bugNumber);
      }
   }

   copy[size] = '\0';

   return (char *) memcpy(copy, s, size);
}
