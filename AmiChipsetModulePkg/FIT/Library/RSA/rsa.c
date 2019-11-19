/*
 * RSA
 * Copyright (c) 2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "crypto.h"
#include "bignum.h"


struct crypto_rsa_key {
	int private_key; /* whether private key is set */
	struct bignum *n; /* modulus (p * q) */
	struct bignum *e; /* public exponent */
	/* The following parameters are available only if private_key is set */
//	struct bignum *d; /* private exponent */
//	struct bignum *p; /* prime p (factor of n) */
//	struct bignum *q; /* prime q (factor of n) */
//	struct bignum *dmp1; /* d mod (p - 1); CRT exponent */
//	struct bignum *dmq1; /* d mod (q - 1); CRT exponent */
//	struct bignum *iqmp; /* 1 / q mod p; CRT coefficient */
};

struct sha256_state {
    UINT64 length;
    UINT32 state[8], curlen;
    UINT8  buf[64];
};
void sha256_init(struct sha256_state *md);
int sha256_process(struct sha256_state *md, const unsigned char *in, unsigned long inlen);
int sha256_done(struct sha256_state *md, unsigned char *out);

/**
  Retrieves the size, in bytes, of the context buffer required for SHA-256 operations.

  @return  The size, in bytes, of the context buffer required for SHA-256 operations.

**/
UINTN
EFIAPI
Sha256GetContextSize(
    VOID)
{
    return sizeof(struct sha256_state);
}

/**
  Initializes user-supplied memory pointed by Sha256Context as SHA-256 hash context for
  subsequent use.

  If Sha256Context is NULL, then ASSERT().

  @param[in, out]  Sha256Context  Pointer to SHA-256 Context being initialized.

  @retval TRUE  SHA-256 context initialization succeeded.

**/
BOOLEAN
EFIAPI
Sha256Init (
    IN OUT  VOID  *Sha256Context)
{
    sha256_init((struct sha256_state *)Sha256Context);
    return TRUE;
}

/**
  Performs SHA-256 digest on a data buffer of the specified length. This function can
  be called multiple times to compute the digest of long or discontinuous data streams.

  If Sha256Context is NULL, then ASSERT().

  @param[in, out]  Sha256Context  Pointer to the SHA-256 context.
  @param[in]       Data           Pointer to the buffer containing the data to be hashed.
  @param[in]       DataLength     Length of Data buffer in bytes.

  @retval TRUE   SHA-256 data digest succeeded.
  @retval FALSE  Invalid SHA-256 context. After Sha256Final function has been called, the
                 SHA-256 context cannot be reused.

**/
BOOLEAN
EFIAPI
Sha256Update(
    IN OUT  VOID        *Sha256Context,
    IN      CONST VOID  *Data,
    IN      UINTN       DataLength)
{
    return sha256_process(
        (struct sha256_state *)Sha256Context, 
        (const unsigned char *)Data, 
        (unsigned long)DataLength);
}

/**
  Completes SHA-256 hash computation and retrieves the digest value into the specified
  memory. After this function has been called, the SHA-256 context cannot be used again.

  If Sha256Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  Sha256Context  Pointer to SHA-256 context
  @param[out]      HashValue      Pointer to a buffer that receives the SHA-256 digest
                                  value (32 bytes).

  @retval TRUE  SHA-256 digest computation succeeded.

**/
BOOLEAN
EFIAPI
Sha256Final(
    IN OUT  VOID   *Sha256Context,
    OUT     UINT8  *HashValue)
{
    return sha256_done(
        (struct sha256_state *)Sha256Context, 
        HashValue) == 0 ? TRUE:FALSE;
}

/**
 * crypto_rsa_free - Free RSA key
 * @key: RSA key to be freed
 *
 * This function frees an RSA key imported with either
 * crypto_rsa_import_public_key() or crypto_rsa_import_private_key().
 */
void BtGcrypto_rsa_free(struct crypto_rsa_key *key)
{
	if (key) {
		bignum_deinit(key->n);
		bignum_deinit(key->e);
/*
		bignum_deinit(key->d);
		bignum_deinit(key->p);
		bignum_deinit(key->q);
		bignum_deinit(key->dmp1);
		bignum_deinit(key->dmq1);
		bignum_deinit(key->iqmp);
*/
		os_free(key);
	}
}

struct crypto_rsa_key *
BtGcrypto_import_rsa2048_public_key(const u8 *KeyN, size_t lenN, const u8 *KeyE, size_t lenE)
{
    struct crypto_rsa_key *key;
    u8 *KeyNmod;
    
    if(!KeyN || !KeyE)
        return NULL;
    
    KeyNmod = (u8*)KeyN;
// !!! NOTE !!!!
// KeyGen adds a leading ZERO if the msb of the first byte of the n-modulus is ONE.
// This is to avoid this integer to be treated as negative value.
// If your calculations produce a number with the high bit set to 1,
// just increase the length by another byte and pad the beginning with 0x00
// to keep it positive.
    if(KeyN[0] >> 7 == 1)
    {
        KeyNmod = os_malloc((UINTN)(lenN+1));
        if (KeyNmod == NULL)
            return NULL;
        memcpy(KeyNmod+1, (void*)KeyN, lenN);
        KeyNmod[0] = 0;
        lenN++;
    }

    key = os_malloc(sizeof(*key));
    if (key == NULL)
        return NULL;

    key->n = bignum_init();
    key->e = bignum_init();
    if (key->n == NULL || key->e == NULL) {
        goto error;
    }

//         * RSA2048PublicKey ::= 
//         *     modulus INTEGER, -- n

    if(bignum_set_unsigned_bin(key->n, KeyNmod, lenN) < 0 )
        goto error;
    if(bignum_set_unsigned_bin(key->e, KeyE, lenE) < 0 )
        goto error;

    return key;

error:
    if(KeyNmod != (u8*)KeyN)
        os_free(KeyNmod);
    BtGcrypto_rsa_free(key);
    return NULL;
}

/**
 * crypto_rsa_exptmod - RSA modular exponentiation
 * @in: Input data
 * @inlen: Input data length
 * @out: Buffer for output data
 * @outlen: Maximum size of the output buffer and used size on success
 * @key: RSA key
 * @use_private: 1 = Use RSA private key, 0 = Use RSA public key
 * Returns: 0 on success, -1 on failure
 */
int BtGcrypto_rsa_exptmod(const u8 *in, size_t inlen, u8 *out, size_t *outlen,
		       struct crypto_rsa_key *key, int use_private)
{
	struct bignum *tmp, *a = NULL, *b = NULL;
	int ret = -1;
	size_t modlen;

	if (use_private && !key->private_key)
		return -1;

	tmp = bignum_init();
	if (tmp == NULL)
		return -1;

	if (bignum_set_unsigned_bin(tmp, in, inlen) < 0)
		goto error;
	if (bignum_cmp(key->n, tmp) < 0) {
		/* Too large input value for the RSA key modulus */
		goto error;
	}

	if (use_private) {
#if 0 // only dealing with Pub keys
		/*
		 * Decrypt (or sign) using Chinese remainer theorem to speed
		 * up calculation. This is equivalent to tmp = tmp^d mod n
		 * (which would require more CPU to calculate directly).
		 *
		 * dmp1 = (1/e) mod (p-1)
		 * dmq1 = (1/e) mod (q-1)
		 * iqmp = (1/q) mod p, where p > q
		 * m1 = c^dmp1 mod p
		 * m2 = c^dmq1 mod q
		 * h = q^-1 (m1 - m2) mod p
		 * m = m2 + hq
		 */
		a = bignum_init();
		b = bignum_init();
		if (a == NULL || b == NULL)
			goto error;

		/* a = tmp^dmp1 mod p */
		if (bignum_exptmod(tmp, key->dmp1, key->p, a) < 0)
			goto error;

		/* b = tmp^dmq1 mod q */
		if (bignum_exptmod(tmp, key->dmq1, key->q, b) < 0)
			goto error;

		/* tmp = (a - b) * (1/q mod p) (mod p) */
		if (bignum_sub(a, b, tmp) < 0 ||
		    bignum_mulmod(tmp, key->iqmp, key->p, tmp) < 0)
			goto error;

		/* tmp = b + q * tmp */
		if (bignum_mul(tmp, key->q, tmp) < 0 ||
		    bignum_add(tmp, b, tmp) < 0)
			goto error;
#endif // #if 0 // only dealing with Pub keys
	} else {
		/* Encrypt (or verify signature) */
		/* tmp = tmp^e mod N */
		if (bignum_exptmod(tmp, key->e, key->n, tmp) < 0)
			goto error;
	}

	// modlen = crypto_rsa_get_modulus_len(key);
	modlen = bignum_get_unsigned_bin_len(key->n);
	if (modlen > *outlen) {
		*outlen = modlen;
		goto error;
	}

	if (bignum_get_unsigned_bin_len(tmp) > modlen)
		goto error; /* should never happen */

	*outlen = modlen;
	os_memset(out, 0, modlen);
	if (bignum_get_unsigned_bin(
		    tmp, out +
		    (modlen - bignum_get_unsigned_bin_len(tmp)), NULL) < 0)
		goto error;

	ret = 0;

error:
	bignum_deinit(tmp);
	bignum_deinit(a);
	bignum_deinit(b);
	return ret;
}
