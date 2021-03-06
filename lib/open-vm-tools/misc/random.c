/*********************************************************
 * Copyright (C) 1998 VMware, Inc. All rights reserved.
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
 * random.c --
 *
 *    Random bits generation. --hpreg
 */

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#   include <windows.h>
#   include <wincrypt.h>
#else
#   include <errno.h>
#   include <fcntl.h>
#   include <unistd.h>
#endif

#include "vmware.h"
#include "random.h"

#define ESX_RANDOM_DEVICE     "/vmfs/devices/char/vmkdriver/urandom"
#define GENERIC_RANDOM_DEVICE "/dev/urandom"


/*
 *-----------------------------------------------------------------------------
 *
 * Random_Crypto --
 *
 *      Generate 'size' bytes of cryptographically strong random bits in
 *      'buffer'. Use this function when you need non-predictable random
 *      bits, typically in security applications. Use Random_Quick below
 *      otherwise.
 *
 * Results:
 *      TRUE on success
 *      FALSE on failure
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

Bool
Random_Crypto(unsigned int size,  // IN:
              void *buffer)       // OUT:
{
#if defined(_WIN32)
   HCRYPTPROV csp;

   if (CryptAcquireContext(&csp, NULL, NULL, PROV_RSA_FULL,
                           CRYPT_VERIFYCONTEXT) == FALSE) {
      Log("%s: CryptAcquireContext failed %d\n", __FUNCTION__,
          GetLastError());
      return FALSE;
   }

   if (CryptGenRandom(csp, size, buffer) == FALSE) {
      CryptReleaseContext(csp, 0);
      Log("%s: CryptGenRandom failed %d\n", __FUNCTION__, GetLastError());

      return FALSE;
   }

   if (CryptReleaseContext(csp, 0) == FALSE) {
      Log("%s: CryptReleaseContext failed %d\n", __FUNCTION__,
          GetLastError());

      return FALSE;
   }
#else
   int fd;

   /*
    * We use /dev/urandom and not /dev/random because it is good enough and
    * because it cannot block. --hpreg
    *
    * Bug 352496: On ESX, /dev/urandom is proxied into COS, and hence is
    * expensive. The VMkernel RNG is available through VMFS and is not
    * proxied. So we use that instead.
    */

#if defined(VMX86_SERVER)
   /*
    * ESX: attempt to use the wonderful random device.
    */

   fd = open(ESX_RANDOM_DEVICE, O_RDONLY);

#if defined(VMX86_DEVEL)
   /*
    * On developer builds, attempt to fall back to the generic random
    * device, even if it is much slower. This has a nice side-effect -
    * some things built for ESX will actually work in a Linux hosted
    * environment.
    */

   if (fd == -1) {
      Log("%s: open of %s failed, attempting to use %s\n", __FUNCTION__,
          ESX_RANDOM_DEVICE, GENERIC_RANDOM_DEVICE);

      fd = open(GENERIC_RANDOM_DEVICE, O_RDONLY);
   }
#endif
#else
   fd = open(GENERIC_RANDOM_DEVICE, O_RDONLY);
#endif

   if (fd == -1) {
      Log("%s: Failed to open random device: %d\n", __FUNCTION__, errno);

      return FALSE;
   }

   /* Although /dev/urandom does not block, it can return short reads. */
   while (size > 0) {
      ssize_t bytesRead = read(fd, buffer, size);

      if (bytesRead == 0 || (bytesRead == -1 && errno != EINTR)) {
         int error = errno;

         close(fd);
         Log("%s: Short read: %d\n", __FUNCTION__, error);

         return FALSE;
      }
      if (bytesRead > 0) {
         size -= bytesRead;
         buffer = ((uint8 *) buffer) + bytesRead; 
      }
   }

   if (close(fd) == -1) {
      Log("%s: Failed to close: %d\n", __FUNCTION__, errno);

      return FALSE;
   }
#endif

   return TRUE;
}


/*
 *-----------------------------------------------------------------------------
 *
 * Random_QuickSeed --
 *
 *      Creates a context for the quick random number generator and returns it.
 *
 * Results:
 *      A pointer to the context used for the random number generator. The
 *      context is dynamically allocated memory that must be freed by caller.
 *
 * Side Effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

#define N 25
#define M 18

#define A 0x8EBFD028
#define S 7
#define B 0x2B5B2500
#define T 15
#define C 0xDB8B0000
#define L 16

struct rngstate {
  uint32 x[N];
  int p, q;
};

void *
Random_QuickSeed(uint32 seed)  // IN:
{
   struct rngstate *rs;

   const uint32 xx[N] = {
      0x95F24DAB, 0x0B685215, 0xE76CCAE7, 0xAF3EC239, 0x715FAD23,
      0x24A590AD, 0x69E4B5EF, 0xBF456141, 0x96BC1B7B, 0xA7BDF825,
      0xC1DE75B7, 0x8858A9C9, 0x2DA87693, 0xB657F9DD, 0xFFDC8A9F,
      0x8121DA71, 0x8B823ECB, 0x885D05F5, 0x4E20CD47, 0x5A9AD5D9,
      0x512C0C03, 0xEA857CCD, 0x4CC1D30F, 0x8891A8A1, 0xA6B7AADB
   };

   rs = (struct rngstate *) malloc(sizeof *rs);

   if (rs != NULL) {
      uint32 i;

      for (i = 0; i < N; i++) {
         rs->x[i] = xx[i] ^ seed;
      }

      rs->p = N - 1;
      rs->q = N - M - 1;
   }

   return (void *) rs;
}


/*
 *-----------------------------------------------------------------------------
 *
 * Random_Quick --
 *
 *      Return uniformly distributed pseudo-random numbers in the range of 0
 *      and 2^32-1 using the (research grade) tGFSR algorithm tt800. The period
 *      of this RNG is 2^(32*N) - 1 while generally having lower overhead than
 *      Random_Crypto().
 *
 * Results:
 *      A random number in the specified range is returned.
 *
 * Side Effects:
 *      The RNG context is modified for later use by Random_Quick.
 *
 *-----------------------------------------------------------------------------
 */

uint32
Random_Quick(void *context)  // IN/OUT:
{
   uint32 y, z;

   struct rngstate *rs = (struct rngstate *) context;

   ASSERT(context);

   if (rs->p == N - 1) {
      rs->p = 0;
   } else {
      (rs->p)++;
   }

   if (rs->q == N - 1) {
      rs->q = 0;
   } else {
      (rs->q)++;
   }

   z = rs->x[(rs->p)];
   y = rs->x[(rs->q)] ^ ( z >> 1 );

   if (z % 2) {
      y ^= A;
   }

   if (rs->p == N - 1) {
      rs->x[0] = y;
   } else {
      rs->x[(rs->p) + 1] = y;
   }

   y ^= ( ( y << S ) & B );
   y ^= ( ( y << T ) & C );

   y ^= ( y >> L ); // improves bits

   return y;
}

