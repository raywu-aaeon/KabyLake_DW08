/** @file
  Defines base cryptographic library APIs.
  The Base Cryptographic Library provides implementations of basic cryptography
  primitives (Hash Serials, HMAC, RSA, Diffie-Hellman, etc) for UEFI security
  functionality enabling.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2009 - 2014 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef __BASE_CRYPT_LIB_H__
#define __BASE_CRYPT_LIB_H__

///
/// MD5 digest size in bytes
///
#define MD5_DIGEST_SIZE     16

///
/// SHA-1 digest size in bytes.
///
#define SHA1_DIGEST_SIZE    20

///
/// SHA-256 digest size in bytes
///
#define SHA256_DIGEST_SIZE  32

///
/// TDES block size in bytes
///
#define TDES_BLOCK_SIZE     8

///
/// AES block size in bytes
///
#define AES_BLOCK_SIZE      16

///
/// RSA Key Tags Definition used in RsaSetKey() function for key component identification.
///
typedef enum {
  RsaKeyN,      ///< RSA public Modulus (N)
  RsaKeyE,      ///< RSA Public exponent (e)
  RsaKeyD,      ///< RSA Private exponent (d)
  RsaKeyP,      ///< RSA secret prime factor of Modulus (p)
  RsaKeyQ,      ///< RSA secret prime factor of Modules (q)
  RsaKeyDp,     ///< p's CRT exponent (== d mod (p - 1))
  RsaKeyDq,     ///< q's CRT exponent (== d mod (q - 1))
  RsaKeyQInv    ///< The CRT coefficient (== 1/q mod p)
} RSA_KEY_TAG;

//=====================================================================================
//    One-Way Cryptographic Hash Primitives
//=====================================================================================

/**
  Retrieves the size, in bytes, of the context buffer required for MD5 hash operations.

  @return  The size, in bytes, of the context buffer required for MD5 hash operations.

**/
UINTN
EFIAPI
Md5GetContextSize (
  VOID
  );

/**
  Initializes user-supplied memory pointed by Md5Context as MD5 hash context for
  subsequent use.

  If Md5Context is NULL, then ASSERT().

  @param[out]  Md5Context  Pointer to MD5 context being initialized.

  @retval TRUE   MD5 context initialization succeeded.
  @retval FALSE  MD5 context initialization failed.

**/
BOOLEAN
EFIAPI
Md5Init (
  OUT  VOID  *Md5Context
  );

/**
  Makes a copy of an existing MD5 context.

  If Md5Context is NULL, then ASSERT().
  If NewMd5Context is NULL, then ASSERT().

  @param[in]  Md5Context     Pointer to MD5 context being copied.
  @param[out] NewMd5Context  Pointer to new MD5 context.

  @retval TRUE   MD5 context copy succeeded.
  @retval FALSE  MD5 context copy failed.

**/
BOOLEAN
EFIAPI
Md5Duplicate (
  IN   CONST VOID  *Md5Context,
  OUT  VOID        *NewMd5Context
  );

/**
  Digests the input data and updates MD5 context.

  This function performs MD5 digest on a data buffer of the specified size.
  It can be called multiple times to compute the digest of long or discontinuous data streams.
  MD5 context should be already correctly intialized by Md5Init(), and should not be finalized
  by Md5Final(). Behavior with invalid context is undefined.

  If Md5Context is NULL, then ASSERT().

  @param[in, out]  Md5Context  Pointer to the MD5 context.
  @param[in]       Data        Pointer to the buffer containing the data to be hashed.
  @param[in]       DataSize    Size of Data buffer in bytes.

  @retval TRUE   MD5 data digest succeeded.
  @retval FALSE  MD5 data digest failed.

**/
BOOLEAN
EFIAPI
Md5Update (
  IN OUT  VOID        *Md5Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  );

/**
  Completes computation of the MD5 digest value.

  This function completes MD5 hash computation and retrieves the digest value into
  the specified memory. After this function has been called, the MD5 context cannot
  be used again.
  MD5 context should be already correctly intialized by Md5Init(), and should not be
  finalized by Md5Final(). Behavior with invalid MD5 context is undefined.

  If Md5Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  Md5Context  Pointer to the MD5 context.
  @param[out]      HashValue   Pointer to a buffer that receives the MD5 digest
                               value (16 bytes).

  @retval TRUE   MD5 digest computation succeeded.
  @retval FALSE  MD5 digest computation failed.

**/
BOOLEAN
EFIAPI
Md5Final (
  IN OUT  VOID   *Md5Context,
  OUT     UINT8  *HashValue
  );

/**
  Retrieves the size, in bytes, of the context buffer required for SHA-1 hash operations.

  @return  The size, in bytes, of the context buffer required for SHA-1 hash operations.

**/
UINTN
EFIAPI
Sha1GetContextSize (
  VOID
  );

/**
  Initializes user-supplied memory pointed by Sha1Context as SHA-1 hash context for
  subsequent use.

  If Sha1Context is NULL, then ASSERT().

  @param[out]  Sha1Context  Pointer to SHA-1 context being initialized.

  @retval TRUE   SHA-1 context initialization succeeded.
  @retval FALSE  SHA-1 context initialization failed.

**/
BOOLEAN
EFIAPI
Sha1Init (
  OUT  VOID  *Sha1Context
  );

/**
  Makes a copy of an existing SHA-1 context.

  If Sha1Context is NULL, then ASSERT().
  If NewSha1Context is NULL, then ASSERT().

  @param[in]  Sha1Context     Pointer to SHA-1 context being copied.
  @param[out] NewSha1Context  Pointer to new SHA-1 context.

  @retval TRUE   SHA-1 context copy succeeded.
  @retval FALSE  SHA-1 context copy failed.

**/
BOOLEAN
EFIAPI
Sha1Duplicate (
  IN   CONST VOID  *Sha1Context,
  OUT  VOID        *NewSha1Context
  );

/**
  Digests the input data and updates SHA-1 context.

  This function performs SHA-1 digest on a data buffer of the specified size.
  It can be called multiple times to compute the digest of long or discontinuous data streams.
  SHA-1 context should be already correctly intialized by Sha1Init(), and should not be finalized
  by Sha1Final(). Behavior with invalid context is undefined.

  If Sha1Context is NULL, then ASSERT().

  @param[in, out]  Sha1Context  Pointer to the SHA-1 context.
  @param[in]       Data         Pointer to the buffer containing the data to be hashed.
  @param[in]       DataSize     Size of Data buffer in bytes.

  @retval TRUE   SHA-1 data digest succeeded.
  @retval FALSE  SHA-1 data digest failed.

**/
BOOLEAN
EFIAPI
Sha1Update (
  IN OUT  VOID        *Sha1Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  );

/**
  Completes computation of the SHA-1 digest value.

  This function completes SHA-1 hash computation and retrieves the digest value into
  the specified memory. After this function has been called, the SHA-1 context cannot
  be used again.
  SHA-1 context should be already correctly intialized by Sha1Init(), and should not be
  finalized by Sha1Final(). Behavior with invalid SHA-1 context is undefined.

  If Sha1Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  Sha1Context  Pointer to the SHA-1 context.
  @param[out]      HashValue    Pointer to a buffer that receives the SHA-1 digest
                                value (20 bytes).

  @retval TRUE   SHA-1 digest computation succeeded.
  @retval FALSE  SHA-1 digest computation failed.

**/
BOOLEAN
EFIAPI
Sha1Final (
  IN OUT  VOID   *Sha1Context,
  OUT     UINT8  *HashValue
  );

/**
  Retrieves the size, in bytes, of the context buffer required for SHA-256 hash operations.

  @return  The size, in bytes, of the context buffer required for SHA-256 hash operations.

**/
UINTN
EFIAPI
Sha256GetContextSize (
  VOID
  );

/**
  Initializes user-supplied memory pointed by Sha256Context as SHA-256 hash context for
  subsequent use.

  If Sha256Context is NULL, then ASSERT().

  @param[out]  Sha256Context  Pointer to SHA-256 context being initialized.

  @retval TRUE   SHA-256 context initialization succeeded.
  @retval FALSE  SHA-256 context initialization failed.

**/
BOOLEAN
EFIAPI
Sha256Init (
  OUT  VOID  *Sha256Context
  );

/**
  Makes a copy of an existing SHA-256 context.

  If Sha256Context is NULL, then ASSERT().
  If NewSha256Context is NULL, then ASSERT().

  @param[in]  Sha256Context     Pointer to SHA-256 context being copied.
  @param[out] NewSha256Context  Pointer to new SHA-256 context.

  @retval TRUE   SHA-256 context copy succeeded.
  @retval FALSE  SHA-256 context copy failed.

**/
BOOLEAN
EFIAPI
Sha256Duplicate (
  IN   CONST VOID  *Sha256Context,
  OUT  VOID        *NewSha256Context
  );

/**
  Digests the input data and updates SHA-256 context.

  This function performs SHA-256 digest on a data buffer of the specified size.
  It can be called multiple times to compute the digest of long or discontinuous data streams.
  SHA-256 context should be already correctly intialized by Sha256Init(), and should not be finalized
  by Sha256Final(). Behavior with invalid context is undefined.

  If Sha256Context is NULL, then ASSERT().

  @param[in, out]  Sha256Context  Pointer to the SHA-256 context.
  @param[in]       Data           Pointer to the buffer containing the data to be hashed.
  @param[in]       DataSize       Size of Data buffer in bytes.

  @retval TRUE   SHA-256 data digest succeeded.
  @retval FALSE  SHA-256 data digest failed.

**/
BOOLEAN
EFIAPI
Sha256Update (
  IN OUT  VOID        *Sha256Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  );

/**
  Completes computation of the SHA-256 digest value.

  This function completes SHA-256 hash computation and retrieves the digest value into
  the specified memory. After this function has been called, the SHA-256 context cannot
  be used again.
  SHA-256 context should be already correctly intialized by Sha256Init(), and should not be
  finalized by Sha256Final(). Behavior with invalid SHA-256 context is undefined.

  If Sha256Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  Sha256Context  Pointer to the SHA-256 context.
  @param[out]      HashValue      Pointer to a buffer that receives the SHA-256 digest
                                  value (32 bytes).

  @retval TRUE   SHA-256 digest computation succeeded.
  @retval FALSE  SHA-256 digest computation failed.

**/
BOOLEAN
EFIAPI
Sha256Final (
  IN OUT  VOID   *Sha256Context,
  OUT     UINT8  *HashValue
  );


//=====================================================================================
//    MAC (Message Authentication Code) Primitive
//=====================================================================================

/**
  Retrieves the size, in bytes, of the context buffer required for HMAC-MD5 operations.

  @return  The size, in bytes, of the context buffer required for HMAC-MD5 operations.

**/
UINTN
EFIAPI
HmacMd5GetContextSize (
  VOID
  );

/**
  Initializes user-supplied memory pointed by HmacMd5Context as HMAC-MD5 context for
  subsequent use.

  If HmacMd5Context is NULL, then ASSERT().

  @param[out]  HmacMd5Context  Pointer to HMAC-MD5 context being initialized.
  @param[in]   Key             Pointer to the user-supplied key.
  @param[in]   KeySize         Key size in bytes.

  @retval TRUE   HMAC-MD5 context initialization succeeded.
  @retval FALSE  HMAC-MD5 context initialization failed.

**/
BOOLEAN
EFIAPI
HmacMd5Init (
  OUT  VOID         *HmacMd5Context,
  IN   CONST UINT8  *Key,
  IN   UINTN        KeySize
  );

/**
  Makes a copy of an existing HMAC-MD5 context.

  If HmacMd5Context is NULL, then ASSERT().
  If NewHmacMd5Context is NULL, then ASSERT().

  @param[in]  HmacMd5Context     Pointer to HMAC-MD5 context being copied.
  @param[out] NewHmacMd5Context  Pointer to new HMAC-MD5 context.

  @retval TRUE   HMAC-MD5 context copy succeeded.
  @retval FALSE  HMAC-MD5 context copy failed.

**/
BOOLEAN
EFIAPI
HmacMd5Duplicate (
  IN   CONST VOID  *HmacMd5Context,
  OUT  VOID        *NewHmacMd5Context
  );

/**
  Digests the input data and updates HMAC-MD5 context.

  This function performs HMAC-MD5 digest on a data buffer of the specified size.
  It can be called multiple times to compute the digest of long or discontinuous data streams.
  HMAC-MD5 context should be already correctly intialized by HmacMd5Init(), and should not be
  finalized by HmacMd5Final(). Behavior with invalid context is undefined.

  If HmacMd5Context is NULL, then ASSERT().

  @param[in, out]  HmacMd5Context  Pointer to the HMAC-MD5 context.
  @param[in]       Data            Pointer to the buffer containing the data to be digested.
  @param[in]       DataSize        Size of Data buffer in bytes.

  @retval TRUE   HMAC-MD5 data digest succeeded.
  @retval FALSE  HMAC-MD5 data digest failed.

**/
BOOLEAN
EFIAPI
HmacMd5Update (
  IN OUT  VOID        *HmacMd5Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  );

/**
  Completes computation of the HMAC-MD5 digest value.

  This function completes HMAC-MD5 hash computation and retrieves the digest value into
  the specified memory. After this function has been called, the HMAC-MD5 context cannot
  be used again.
  HMAC-MD5 context should be already correctly intialized by HmacMd5Init(), and should not be
  finalized by HmacMd5Final(). Behavior with invalid HMAC-MD5 context is undefined.

  If HmacMd5Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  HmacMd5Context  Pointer to the HMAC-MD5 context.
  @param[out]      HashValue       Pointer to a buffer that receives the HMAC-MD5 digest
                                   value (16 bytes).

  @retval TRUE   HMAC-MD5 digest computation succeeded.
  @retval FALSE  HMAC-MD5 digest computation failed.

**/
BOOLEAN
EFIAPI
HmacMd5Final (
  IN OUT  VOID   *HmacMd5Context,
  OUT     UINT8  *HmacValue
  );

/**
  Retrieves the size, in bytes, of the context buffer required for HMAC-SHA1 operations.

  @return  The size, in bytes, of the context buffer required for HMAC-SHA1 operations.

**/
UINTN
EFIAPI
HmacSha1GetContextSize (
  VOID
  );

/**
  Initializes user-supplied memory pointed by HmacSha1Context as HMAC-SHA1 context for
  subsequent use.

  If HmacSha1Context is NULL, then ASSERT().

  @param[out]  HmacSha1Context  Pointer to HMAC-SHA1 context being initialized.
  @param[in]   Key              Pointer to the user-supplied key.
  @param[in]   KeySize          Key size in bytes.

  @retval TRUE   HMAC-SHA1 context initialization succeeded.
  @retval FALSE  HMAC-SHA1 context initialization failed.

**/
BOOLEAN
EFIAPI
HmacSha1Init (
  OUT  VOID         *HmacSha1Context,
  IN   CONST UINT8  *Key,
  IN   UINTN        KeySize
  );

/**
  Makes a copy of an existing HMAC-SHA1 context.

  If HmacSha1Context is NULL, then ASSERT().
  If NewHmacSha1Context is NULL, then ASSERT().

  @param[in]  HmacSha1Context     Pointer to HMAC-SHA1 context being copied.
  @param[out] NewHmacSha1Context  Pointer to new HMAC-SHA1 context.

  @retval TRUE   HMAC-SHA1 context copy succeeded.
  @retval FALSE  HMAC-SHA1 context copy failed.

**/
BOOLEAN
EFIAPI
HmacSha1Duplicate (
  IN   CONST VOID  *HmacSha1Context,
  OUT  VOID        *NewHmacSha1Context
  );

/**
  Digests the input data and updates HMAC-SHA1 context.

  This function performs HMAC-SHA1 digest on a data buffer of the specified size.
  It can be called multiple times to compute the digest of long or discontinuous data streams.
  HMAC-SHA1 context should be already correctly intialized by HmacSha1Init(), and should not
  be finalized by HmacSha1Final(). Behavior with invalid context is undefined.

  If HmacSha1Context is NULL, then ASSERT().

  @param[in, out]  HmacSha1Context Pointer to the HMAC-SHA1 context.
  @param[in]       Data            Pointer to the buffer containing the data to be digested.
  @param[in]       DataSize        Size of Data buffer in bytes.

  @retval TRUE   HMAC-SHA1 data digest succeeded.
  @retval FALSE  HMAC-SHA1 data digest failed.

**/
BOOLEAN
EFIAPI
HmacSha1Update (
  IN OUT  VOID        *HmacSha1Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  );

/**
  Completes computation of the HMAC-SHA1 digest value.

  This function completes HMAC-SHA1 hash computation and retrieves the digest value into
  the specified memory. After this function has been called, the HMAC-SHA1 context cannot
  be used again.
  HMAC-SHA1 context should be already correctly intialized by HmacSha1Init(), and should
  not be finalized by HmacSha1Final(). Behavior with invalid HMAC-SHA1 context is undefined.

  If HmacSha1Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  HmacSha1Context  Pointer to the HMAC-SHA1 context.
  @param[out]      HashValue        Pointer to a buffer that receives the HMAC-SHA1 digest
                                    value (20 bytes).

  @retval TRUE   HMAC-SHA1 digest computation succeeded.
  @retval FALSE  HMAC-SHA1 digest computation failed.

**/
BOOLEAN
EFIAPI
HmacSha1Final (
  IN OUT  VOID   *HmacSha1Context,
  OUT     UINT8  *HmacValue
  );


//=====================================================================================
//    Symmetric Cryptography Primitive
//=====================================================================================

/**
  Retrieves the size, in bytes, of the context buffer required for TDES operations.

  @return  The size, in bytes, of the context buffer required for TDES operations.

**/
UINTN
EFIAPI
TdesGetContextSize (
  VOID
  );

/**
  Initializes user-supplied memory as TDES context for subsequent use.

  This function initializes user-supplied memory pointed by TdesContext as TDES context.
  In addtion, it sets up all TDES key materials for subsequent encryption and decryption
  operations.
  There are 3 key options as follows:
  KeyLength = 64,  Keying option 1: K1 == K2 == K3 (Backward compatibility with DES)
  KeyLength = 128, Keying option 2: K1 != K2 and K3 = K1 (Less Security)
  KeyLength = 192  Keying option 3: K1 != K2 != K3 (Strongest)

  If TdesContext is NULL, then ASSERT().
  If Key is NULL, then ASSERT().
  If KeyLength is not valid, then ASSERT().

  @param[out]  TdesContext  Pointer to TDES context being initialized.
  @param[in]   Key          Pointer to the user-supplied TDES key.
  @param[in]   KeyLength    Length of TDES key in bits.

  @retval TRUE   TDES context initialization succeeded.
  @retval FALSE  TDES context initialization failed.

**/
BOOLEAN
EFIAPI
TdesInit (
  OUT  VOID         *TdesContext,
  IN   CONST UINT8  *Key,
  IN   UINTN        KeyLength
  );

/**
  Performs TDES encryption on a data buffer of the specified size in ECB mode.

  This function performs TDES encryption on data buffer pointed by Input, of specified
  size of InputSize, in ECB mode.
  InputSize must be multiple of block size (8 bytes). This function does not perform
  padding. Caller must perform padding, if necessary, to ensure valid input data size.
  TdesContext should be already correctly initialized by TdesInit(). Behavior with
  invalid TDES context is undefined.

  If TdesContext is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If InputSize is not multiple of block size (8 bytes), then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   TdesContext  Pointer to the TDES context.
  @param[in]   Input        Pointer to the buffer containing the data to be encrypted.
  @param[in]   InputSize    Size of the Input buffer in bytes.
  @param[out]  Output       Pointer to a buffer that receives the TDES encryption output.

  @retval TRUE   TDES encryption succeeded.
  @retval FALSE  TDES encryption failed.

**/
BOOLEAN
EFIAPI
TdesEcbEncrypt (
  IN   VOID         *TdesContext,
  IN   CONST UINT8  *Input,
  IN   UINTN        InputSize,
  OUT  UINT8        *Output
  );

/**
  Performs TDES decryption on a data buffer of the specified size in ECB mode.

  This function performs TDES decryption on data buffer pointed by Input, of specified
  size of InputSize, in ECB mode.
  InputSize must be multiple of block size (8 bytes). This function does not perform
  padding. Caller must perform padding, if necessary, to ensure valid input data size.
  TdesContext should be already correctly initialized by TdesInit(). Behavior with
  invalid TDES context is undefined.

  If TdesContext is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If InputSize is not multiple of block size (8 bytes), then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   TdesContext  Pointer to the TDES context.
  @param[in]   Input        Pointer to the buffer containing the data to be decrypted.
  @param[in]   InputSize    Size of the Input buffer in bytes.
  @param[out]  Output       Pointer to a buffer that receives the TDES decryption output.

  @retval TRUE   TDES decryption succeeded.
  @retval FALSE  TDES decryption failed.

**/
BOOLEAN
EFIAPI
TdesEcbDecrypt (
  IN   VOID         *TdesContext,
  IN   CONST UINT8  *Input,
  IN   UINTN        InputSize,
  OUT  UINT8        *Output
  );

/**
  Performs TDES encryption on a data buffer of the specified size in CBC mode.

  This function performs TDES encryption on data buffer pointed by Input, of specified
  size of InputSize, in CBC mode.
  InputSize must be multiple of block size (8 bytes). This function does not perform
  padding. Caller must perform padding, if necessary, to ensure valid input data size.
  Initialization vector should be one block size (8 bytes).
  TdesContext should be already correctly initialized by TdesInit(). Behavior with
  invalid TDES context is undefined.

  If TdesContext is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If InputSize is not multiple of block size (8 bytes), then ASSERT().
  If Ivec is NULL, then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   TdesContext  Pointer to the TDES context.
  @param[in]   Input        Pointer to the buffer containing the data to be encrypted.
  @param[in]   InputSize    Size of the Input buffer in bytes.
  @param[in]   Ivec         Pointer to initialization vector.
  @param[out]  Output       Pointer to a buffer that receives the TDES encryption output.

  @retval TRUE   TDES encryption succeeded.
  @retval FALSE  TDES encryption failed.

**/
BOOLEAN
EFIAPI
TdesCbcEncrypt (
  IN   VOID         *TdesContext,
  IN   CONST UINT8  *Input,
  IN   UINTN        InputSize,
  IN   CONST UINT8  *Ivec,
  OUT  UINT8        *Output
  );

/**
  Performs TDES decryption on a data buffer of the specified size in CBC mode.

  This function performs TDES decryption on data buffer pointed by Input, of specified
  size of InputSize, in CBC mode.
  InputSize must be multiple of block size (8 bytes). This function does not perform
  padding. Caller must perform padding, if necessary, to ensure valid input data size.
  Initialization vector should be one block size (8 bytes).
  TdesContext should be already correctly initialized by TdesInit(). Behavior with
  invalid TDES context is undefined.

  If TdesContext is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If InputSize is not multiple of block size (8 bytes), then ASSERT().
  If Ivec is NULL, then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   TdesContext  Pointer to the TDES context.
  @param[in]   Input        Pointer to the buffer containing the data to be encrypted.
  @param[in]   InputSize    Size of the Input buffer in bytes.
  @param[in]   Ivec         Pointer to initialization vector.
  @param[out]  Output       Pointer to a buffer that receives the TDES encryption output.

  @retval TRUE   TDES decryption succeeded.
  @retval FALSE  TDES decryption failed.

**/
BOOLEAN
EFIAPI
TdesCbcDecrypt (
  IN   VOID         *TdesContext,
  IN   CONST UINT8  *Input,
  IN   UINTN        InputSize,
  IN   CONST UINT8  *Ivec,
  OUT  UINT8        *Output
  );

/**
  Retrieves the size, in bytes, of the context buffer required for AES operations.

  @return  The size, in bytes, of the context buffer required for AES operations.

**/
UINTN
EFIAPI
AesGetContextSize (
  VOID
  );

/**
  Initializes user-supplied memory as AES context for subsequent use.

  This function initializes user-supplied memory pointed by AesContext as AES context.
  In addtion, it sets up all AES key materials for subsequent encryption and decryption
  operations.
  There are 3 options for key length, 128 bits, 192 bits, and 256 bits.

  If AesContext is NULL, then ASSERT().
  If Key is NULL, then ASSERT().
  If KeyLength is not valid, then ASSERT().

  @param[out]  AesContext  Pointer to AES context being initialized.
  @param[in]   Key         Pointer to the user-supplied AES key.
  @param[in]   KeyLength   Length of AES key in bits.

  @retval TRUE   AES context initialization succeeded.
  @retval FALSE  AES context initialization failed.

**/
BOOLEAN
EFIAPI
AesInit (
  OUT  VOID         *AesContext,
  IN   CONST UINT8  *Key,
  IN   UINTN        KeyLength
  );

/**
  Performs AES encryption on a data buffer of the specified size in ECB mode.

  This function performs AES encryption on data buffer pointed by Input, of specified
  size of InputSize, in ECB mode.
  InputSize must be multiple of block size (16 bytes). This function does not perform
  padding. Caller must perform padding, if necessary, to ensure valid input data size.
  AesContext should be already correctly initialized by AesInit(). Behavior with
  invalid AES context is undefined.

  If AesContext is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If InputSize is not multiple of block size (16 bytes), then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   AesContext  Pointer to the AES context.
  @param[in]   Input       Pointer to the buffer containing the data to be encrypted.
  @param[in]   InputSize   Size of the Input buffer in bytes.
  @param[out]  Output      Pointer to a buffer that receives the AES encryption output.

  @retval TRUE   AES encryption succeeded.
  @retval FALSE  AES encryption failed.

**/
BOOLEAN
EFIAPI
AesEcbEncrypt (
  IN   VOID         *AesContext,
  IN   CONST UINT8  *Input,
  IN   UINTN        InputSize,
  OUT  UINT8        *Output
  );

/**
  Performs AES decryption on a data buffer of the specified size in ECB mode.

  This function performs AES decryption on data buffer pointed by Input, of specified
  size of InputSize, in ECB mode.
  InputSize must be multiple of block size (16 bytes). This function does not perform
  padding. Caller must perform padding, if necessary, to ensure valid input data size.
  AesContext should be already correctly initialized by AesInit(). Behavior with
  invalid AES context is undefined.

  If AesContext is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If InputSize is not multiple of block size (16 bytes), then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   AesContext  Pointer to the AES context.
  @param[in]   Input       Pointer to the buffer containing the data to be decrypted.
  @param[in]   InputSize   Size of the Input buffer in bytes.
  @param[out]  Output      Pointer to a buffer that receives the AES decryption output.

  @retval TRUE   AES decryption succeeded.
  @retval FALSE  AES decryption failed.

**/
BOOLEAN
EFIAPI
AesEcbDecrypt (
  IN   VOID         *AesContext,
  IN   CONST UINT8  *Input,
  IN   UINTN        InputSize,
  OUT  UINT8        *Output
  );

/**
  Performs AES encryption on a data buffer of the specified size in CBC mode.

  This function performs AES encryption on data buffer pointed by Input, of specified
  size of InputSize, in CBC mode.
  InputSize must be multiple of block size (16 bytes). This function does not perform
  padding. Caller must perform padding, if necessary, to ensure valid input data size.
  Initialization vector should be one block size (16 bytes).
  AesContext should be already correctly initialized by AesInit(). Behavior with
  invalid AES context is undefined.

  If AesContext is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If InputSize is not multiple of block size (16 bytes), then ASSERT().
  If Ivec is NULL, then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   AesContext  Pointer to the AES context.
  @param[in]   Input       Pointer to the buffer containing the data to be encrypted.
  @param[in]   InputSize   Size of the Input buffer in bytes.
  @param[in]   Ivec        Pointer to initialization vector.
  @param[out]  Output      Pointer to a buffer that receives the AES encryption output.

  @retval TRUE   AES encryption succeeded.
  @retval FALSE  AES encryption failed.

**/
BOOLEAN
EFIAPI
AesCbcEncrypt (
  IN   VOID         *AesContext,
  IN   CONST UINT8  *Input,
  IN   UINTN        InputSize,
  IN   CONST UINT8  *Ivec,
  OUT  UINT8        *Output
  );

/**
  Performs AES decryption on a data buffer of the specified size in CBC mode.

  This function performs AES decryption on data buffer pointed by Input, of specified
  size of InputSize, in CBC mode.
  InputSize must be multiple of block size (16 bytes). This function does not perform
  padding. Caller must perform padding, if necessary, to ensure valid input data size.
  Initialization vector should be one block size (16 bytes).
  AesContext should be already correctly initialized by AesInit(). Behavior with
  invalid AES context is undefined.

  If AesContext is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If InputSize is not multiple of block size (16 bytes), then ASSERT().
  If Ivec is NULL, then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   AesContext  Pointer to the AES context.
  @param[in]   Input       Pointer to the buffer containing the data to be encrypted.
  @param[in]   InputSize   Size of the Input buffer in bytes.
  @param[in]   Ivec        Pointer to initialization vector.
  @param[out]  Output      Pointer to a buffer that receives the AES encryption output.

  @retval TRUE   AES decryption succeeded.
  @retval FALSE  AES decryption failed.

**/
BOOLEAN
EFIAPI
AesCbcDecrypt (
  IN   VOID         *AesContext,
  IN   CONST UINT8  *Input,
  IN   UINTN        InputSize,
  IN   CONST UINT8  *Ivec,
  OUT  UINT8        *Output
  );

/**
  Retrieves the size, in bytes, of the context buffer required for ARC4 operations.

  @return  The size, in bytes, of the context buffer required for ARC4 operations.

**/
UINTN
EFIAPI
Arc4GetContextSize (
  VOID
  );

/**
  Initializes user-supplied memory as ARC4 context for subsequent use.

  This function initializes user-supplied memory pointed by Arc4Context as ARC4 context.
  In addtion, it sets up all ARC4 key materials for subsequent encryption and decryption
  operations.

  If Arc4Context is NULL, then ASSERT().
  If Key is NULL, then ASSERT().
  If KeySize does not in the range of [5, 256] bytes, then ASSERT().

  @param[out]  Arc4Context  Pointer to ARC4 context being initialized.
  @param[in]   Key          Pointer to the user-supplied ARC4 key.
  @param[in]   KeySize      Size of ARC4 key in bytes.

  @retval TRUE   ARC4 context initialization succeeded.
  @retval FALSE  ARC4 context initialization failed.

**/
BOOLEAN
EFIAPI
Arc4Init (
  OUT  VOID         *Arc4Context,
  IN   CONST UINT8  *Key,
  IN   UINTN        KeySize
  );

/**
  Performs ARC4 encryption on a data buffer of the specified size.

  This function performs ARC4 encryption on data buffer pointed by Input, of specified
  size of InputSize.
  Arc4Context should be already correctly initialized by Arc4Init(). Behavior with
  invalid ARC4 context is undefined.

  If Arc4Context is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   Arc4Context  Pointer to the ARC4 context.
  @param[in]   Input        Pointer to the buffer containing the data to be encrypted.
  @param[in]   InputSize    Size of the Input buffer in bytes.
  @param[out]  Output       Pointer to a buffer that receives the ARC4 encryption output.

  @retval TRUE   ARC4 encryption succeeded.
  @retval FALSE  ARC4 encryption failed.

**/
BOOLEAN
EFIAPI
Arc4Encrypt (
  IN OUT  VOID         *Arc4Context,
  IN      CONST UINT8  *Input,
  IN      UINTN        InputSize,
  OUT     UINT8        *Output
  );

/**
  Performs ARC4 decryption on a data buffer of the specified size.

  This function performs ARC4 decryption on data buffer pointed by Input, of specified
  size of InputSize.
  Arc4Context should be already correctly initialized by Arc4Init(). Behavior with
  invalid ARC4 context is undefined.

  If Arc4Context is NULL, then ASSERT().
  If Input is NULL, then ASSERT().
  If Output is NULL, then ASSERT().

  @param[in]   Arc4Context  Pointer to the ARC4 context.
  @param[in]   Input        Pointer to the buffer containing the data to be decrypted.
  @param[in]   InputSize    Size of the Input buffer in bytes.
  @param[out]  Output       Pointer to a buffer that receives the ARC4 decryption output.

  @retval TRUE   ARC4 decryption succeeded.
  @retval FALSE  ARC4 decryption failed.

**/
BOOLEAN
EFIAPI
Arc4Decrypt (
  IN OUT  VOID   *Arc4Context,
  IN      UINT8  *Input,
  IN      UINTN  InputSize,
  OUT     UINT8  *Output
  );

/**
  Resets the ARC4 context to the initial state.

  The function resets the ARC4 context to the state it had immediately after the
  ARC4Init() function call.
  Contrary to ARC4Init(), Arc4Reset() requires no secret key as input, but ARC4 context
  should be already correctly initialized by ARC4Init().

  If Arc4Context is NULL, then ASSERT().

  @param[in, out]  Arc4Context  Pointer to the ARC4 context.

  @retval TRUE   ARC4 reset succeeded.
  @retval FALSE  ARC4 reset failed.

**/
BOOLEAN
EFIAPI
Arc4Reset (
  IN OUT  VOID  *Arc4Context
  );

//=====================================================================================
//    Asymmetric Cryptography Primitive
//=====================================================================================

/**
  Allocates and initializes one RSA context for subsequent use.

  @return  Pointer to the RSA context that has been initialized.
           If the allocations fails, RsaNew() returns NULL.

**/
VOID *
EFIAPI
RsaNew (
  VOID
  );

/**
  Release the specified RSA context.

  If RsaContext is NULL, then ASSERT().

  @param[in]  RsaContext  Pointer to the RSA context to be released.

**/
VOID
EFIAPI
RsaFree (
  IN  VOID  *RsaContext
  );

/**
  Sets the tag-designated key component into the established RSA context.

  This function sets the tag-designated RSA key component into the established
  RSA context from the user-specified non-negative integer (octet string format
  represented in RSA PKCS#1).
  If BigNumber is NULL, then the specified key componenet in RSA context is cleared.

  If RsaContext is NULL, then ASSERT().

  @param[in, out]  RsaContext  Pointer to RSA context being set.
  @param[in]       KeyTag      Tag of RSA key component being set.
  @param[in]       BigNumber   Pointer to octet integer buffer.
                               If NULL, then the specified key componenet in RSA
                               context is cleared.
  @param[in]       BnSize      Size of big number buffer in bytes.
                               If BigNumber is NULL, then it is ignored.

  @retval  TRUE   RSA key component was set successfully.
  @retval  FALSE  Invalid RSA key component tag.

**/
BOOLEAN
EFIAPI
RsaSetKey (
  IN OUT  VOID         *RsaContext,
  IN      RSA_KEY_TAG  KeyTag,
  IN      CONST UINT8  *BigNumber,
  IN      UINTN        BnSize
  );

/**
  Gets the tag-designated RSA key component from the established RSA context.

  This function retrieves the tag-designated RSA key component from the
  established RSA context as a non-negative integer (octet string format
  represented in RSA PKCS#1).
  If specified key component has not been set or has been cleared, then returned
  BnSize is set to 0.
  If the BigNumber buffer is too small to hold the contents of the key, FALSE
  is returned and BnSize is set to the required buffer size to obtain the key.

  If RsaContext is NULL, then ASSERT().
  If BnSize is NULL, then ASSERT().
  If BnSize is large enough but BigNumber is NULL, then ASSERT().

  @param[in, out]  RsaContext  Pointer to RSA context being set.
  @param[in]       KeyTag      Tag of RSA key component being set.
  @param[out]      BigNumber   Pointer to octet integer buffer.
  @param[in, out]  BnSize      On input, the size of big number buffer in bytes.
                               On output, the size of data returned in big number buffer in bytes.

  @retval  TRUE   RSA key component was retrieved successfully.
  @retval  FALSE  Invalid RSA key component tag.
  @retval  FALSE  BnSize is too small.

**/
BOOLEAN
EFIAPI
RsaGetKey (
  IN OUT  VOID         *RsaContext,
  IN      RSA_KEY_TAG  KeyTag,
  OUT     UINT8        *BigNumber,
  IN OUT  UINTN        *BnSize
  );

/**
  Generates RSA key components.

  This function generates RSA key components. It takes RSA public exponent E and
  length in bits of RSA modulus N as input, and generates all key components.
  If PublicExponent is NULL, the default RSA public exponent (0x10001) will be used.

  Before this function can be invoked, pseudorandom number generator must be correctly
  initialized by RandomSeed().

  If RsaContext is NULL, then ASSERT().

  @param[in, out]  RsaContext           Pointer to RSA context being set.
  @param[in]       ModulusLength        Length of RSA modulus N in bits.
  @param[in]       PublicExponent       Pointer to RSA public exponent.
  @param[in]       PublicExponentSize   Size of RSA public exponent buffer in bytes.

  @retval  TRUE   RSA key component was generated successfully.
  @retval  FALSE  Invalid RSA key component tag.

**/
BOOLEAN
EFIAPI
RsaGenerateKey (
  IN OUT  VOID         *RsaContext,
  IN      UINTN        ModulusLength,
  IN      CONST UINT8  *PublicExponent,
  IN      UINTN        PublicExponentSize
  );

/**
  Validates key components of RSA context.

  This function validates key compoents of RSA context in following aspects:
  - Whether p is a prime
  - Whether q is a prime
  - Whether n = p * q
  - Whether d*e = 1  mod lcm(p-1,q-1)

  If RsaContext is NULL, then ASSERT().

  @param[in]  RsaContext  Pointer to RSA context to check.

  @retval  TRUE   RSA key components are valid.
  @retval  FALSE  RSA key components are not valid.

**/
BOOLEAN
EFIAPI
RsaCheckKey (
  IN  VOID  *RsaContext
  );

/**
  Carries out the RSA-SSA signature generation with EMSA-PKCS1-v1_5 encoding scheme.

  This function carries out the RSA-SSA signature generation with EMSA-PKCS1-v1_5 encoding scheme defined in
  RSA PKCS#1.
  If the Signature buffer is too small to hold the contents of signature, FALSE
  is returned and SigSize is set to the required buffer size to obtain the signature.

  If RsaContext is NULL, then ASSERT().
  If MessageHash is NULL, then ASSERT().
  If HashSize is not equal to the size of MD5, SHA-1, SHA-256, SHA-224, SHA-512 or SHA-384 digest, then ASSERT().
  If SigSize is large enough but Signature is NULL, then ASSERT().

  @param[in]      RsaContext   Pointer to RSA context for signature generation.
  @param[in]      MessageHash  Pointer to octet message hash to be signed.
  @param[in]      HashSize     Size of the message hash in bytes.
  @param[out]     Signature    Pointer to buffer to receive RSA PKCS1-v1_5 signature.
  @param[in, out] SigSize      On input, the size of Signature buffer in bytes.
                              On output, the size of data returned in Signature buffer in bytes.

  @retval  TRUE   Signature successfully generated in PKCS1-v1_5.
  @retval  FALSE  Signature generation failed.
  @retval  FALSE  SigSize is too small.

**/
BOOLEAN
EFIAPI
RsaPkcs1Sign (
  IN      VOID         *RsaContext,
  IN      CONST UINT8  *MessageHash,
  IN      UINTN        HashSize,
  OUT     UINT8        *Signature,
  IN OUT  UINTN        *SigSize
  );

/**
  Verifies the RSA-SSA signature with EMSA-PKCS1-v1_5 encoding scheme defined in
  RSA PKCS#1.

  If RsaContext is NULL, then ASSERT().
  If MessageHash is NULL, then ASSERT().
  If Signature is NULL, then ASSERT().
  If HashSize is not equal to the size of MD5, SHA-1, SHA-256, SHA-224, SHA-512 or SHA-384 digest, then ASSERT().

  @param[in]  RsaContext   Pointer to RSA context for signature verification.
  @param[in]  MessageHash  Pointer to octet message hash to be checked.
  @param[in]  HashSize     Size of the message hash in bytes.
  @param[in]  Signature    Pointer to RSA PKCS1-v1_5 signature to be verified.
  @param[in]  SigSize      Size of signature in bytes.

  @retval  TRUE   Valid signature encoded in PKCS1-v1_5.
  @retval  FALSE  Invalid signature or invalid RSA context.

**/
BOOLEAN
EFIAPI
RsaPkcs1Verify (
  IN  VOID         *RsaContext,
  IN  CONST UINT8  *MessageHash,
  IN  UINTN        HashSize,
  IN  UINT8        *Signature,
  IN  UINTN        SigSize
  );

/**
  Verifies the validility of a PKCS#7 signed data as described in "PKCS #7: Cryptographic
  Message Syntax Standard".

  If P7Data is NULL, then ASSERT().

  @param[in]  P7Data       Pointer to the PKCS#7 message to verify.
  @param[in]  P7Size       Size of the PKCS#7 message in bytes.
  @param[in]  TrustedCert  Pointer to a trusted/root certificate encoded in DER, which
                           is used for certificate chain verification.
  @param[in]  CertSize     Size of the trusted certificate in bytes.
  @param[in]  InData       Pointer to the content to be verified.
  @param[in]  DataSize     Size of InData in bytes.

  @retval  TRUE  The specified PKCS#7 signed data is valid.
  @retval  FALSE Invalid PKCS#7 signed data.

**/
BOOLEAN
EFIAPI
Pkcs7Verify (
  IN  CONST UINT8  *P7Data,
  IN  UINTN        P7Size,
  IN  CONST UINT8  *TrustedCert,
  IN  UINTN        CertSize,
  IN  CONST UINT8  *InData,
  IN  UINTN        DataSize
  );

//=====================================================================================
//    DH Key Exchange Primitive
//=====================================================================================

/**
  Allocates and Initializes one Diffie-Hellman Context for subsequent use.

  @return  Pointer to the Diffie-Hellman Context that has been initialized.
           If the allocations fails, DhNew() returns NULL.

**/
VOID *
EFIAPI
DhNew (
  VOID
  );

/**
  Release the specified DH context.

  If DhContext is NULL, then ASSERT().

  @param[in]  DhContext  Pointer to the DH context to be released.

**/
VOID
EFIAPI
DhFree (
  IN  VOID  *DhContext
  );

/**
  Generates DH parameter.

  Given generator g, and length of prime number p in bits, this function generates p,
  and sets DH context according to value of g and p.

  Before this function can be invoked, pseudorandom number generator must be correctly
  initialized by RandomSeed().

  If DhContext is NULL, then ASSERT().
  If Prime is NULL, then ASSERT().

  @param[in, out]  DhContext    Pointer to the DH context.
  @param[in]       Generator    Value of generator.
  @param[in]       PrimeLength  Length in bits of prime to be generated.
  @param[out]      Prime        Pointer to the buffer to receive the generated prime number.

  @retval TRUE   DH pamameter generation succeeded.
  @retval FALSE  Value of Generator is not supported.
  @retval FALSE  PRNG fails to generate random prime number with PrimeLength.

**/
BOOLEAN
EFIAPI
DhGenerateParameter (
  IN OUT  VOID   *DhContext,
  IN      UINTN  Generator,
  IN      UINTN  PrimeLength,
  OUT     UINT8  *Prime
  );

/**
  Sets generator and prime parameters for DH.

  Given generator g, and prime number p, this function and sets DH
  context accordingly.

  If DhContext is NULL, then ASSERT().
  If Prime is NULL, then ASSERT().

  @param[in, out]  DhContext    Pointer to the DH context.
  @param[in]       Generator    Value of generator.
  @param[in]       PrimeLength  Length in bits of prime to be generated.
  @param[in]       Prime        Pointer to the prime number.

  @retval TRUE   DH pamameter setting succeeded.
  @retval FALSE  Value of Generator is not supported.
  @retval FALSE  Value of Generator is not suitable for the Prime.
  @retval FALSE  Value of Prime is not a prime number.
  @retval FALSE  Value of Prime is not a safe prime number.

**/
BOOLEAN
EFIAPI
DhSetParameter (
  IN OUT  VOID         *DhContext,
  IN      UINTN        Generator,
  IN      UINTN        PrimeLength,
  IN      CONST UINT8  *Prime
  );

/**
  Generates DH public key.

  This function generates random secret exponent, and computes the public key, which is
  returned via parameter PublicKey and PublicKeySize. DH context is updated accordingly.
  If the PublicKey buffer is too small to hold the public key, FALSE is returned and
  PublicKeySize is set to the required buffer size to obtain the public key.

  If DhContext is NULL, then ASSERT().
  If PublicKeySize is NULL, then ASSERT().
  If PublicKeySize is large enough but PublicKey is NULL, then ASSERT().

  @param[in, out]  DhContext      Pointer to the DH context.
  @param[out]      PublicKey      Pointer to the buffer to receive generated public key.
  @param[in, out]  PublicKeySize  On input, the size of PublicKey buffer in bytes.
                                 On output, the size of data returned in PublicKey buffer in bytes.

  @retval TRUE   DH public key generation succeeded.
  @retval FALSE  DH public key generation failed.
  @retval FALSE  PublicKeySize is not large enough.

**/
BOOLEAN
EFIAPI
DhGenerateKey (
  IN OUT  VOID   *DhContext,
  OUT     UINT8  *PublicKey,
  IN OUT  UINTN  *PublicKeySize
  );

/**
  Computes exchanged common key.

  Given peer's public key, this function computes the exchanged common key, based on its own
  context including value of prime modulus and random secret exponent.

  If DhContext is NULL, then ASSERT().
  If PeerPublicKey is NULL, then ASSERT().
  If KeySize is NULL, then ASSERT().
  If KeySize is large enough but Key is NULL, then ASSERT().

  @param[in, out]  DhContext          Pointer to the DH context.
  @param[in]       PeerPublicKey      Pointer to the peer's public key.
  @param[in]       PeerPublicKeySize  Size of peer's public key in bytes.
  @param[out]      Key                Pointer to the buffer to receive generated key.
  @param[in, out]  KeySize            On input, the size of Key buffer in bytes.
                                     On output, the size of data returned in Key buffer in bytes.

  @retval TRUE   DH exchanged key generation succeeded.
  @retval FALSE  DH exchanged key generation failed.
  @retval FALSE  KeySize is not large enough.

**/
BOOLEAN
EFIAPI
DhComputeKey (
  IN OUT  VOID         *DhContext,
  IN      CONST UINT8  *PeerPublicKey,
  IN      UINTN        PeerPublicKeySize,
  OUT     UINT8        *Key,
  IN OUT  UINTN        *KeySize
  );

//=====================================================================================
//    Pseudo-Random Generation Primitive
//=====================================================================================

/**
  Sets up the seed value for the pseudorandom number generator.

  This function sets up the seed value for the pseudorandom number generator.
  If Seed is not NULL, then the seed passed in is used.
  If Seed is NULL, then default seed is used.

  @param[in]  Seed      Pointer to seed value.
                        If NULL, default seed is used.
  @param[in]  SeedSize  Size of seed value.
                        If Seed is NULL, this parameter is ignored.

  @retval TRUE   Pseudorandom number generator has enough entropy for random generation.
  @retval FALSE  Pseudorandom number generator does not have enough entropy for random generation.

**/
BOOLEAN
EFIAPI
RandomSeed (
  IN  CONST  UINT8  *Seed  OPTIONAL,
  IN  UINTN         SeedSize
  );

/**
  Generates a pseudorandom byte stream of the specified size.

  If Output is NULL, then ASSERT().

  @param[out]  Output  Pointer to buffer to receive random value.
  @param[in]   Size    Size of randome bytes to generate.

  @retval TRUE   Pseudorandom byte stream generated successfully.
  @retval FALSE  Pseudorandom number generator fails to generate due to lack of entropy.

**/
BOOLEAN
EFIAPI
RandomBytes (
  OUT  UINT8  *Output,
  IN   UINTN  Size
  );

#endif // __BASE_CRYPT_LIB_H__
