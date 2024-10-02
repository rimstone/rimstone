// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://gliimly.github.io/ - this file is part of Gliimly framework.

// 
// Security-related functions for GLIIMLY and run-time
// (getting db credentials, encryption etc)
//


#include "gliim.h"
#include <openssl/err.h>

// Prototypes
void gg_sec_err (char *err);
EVP_MD *gg_get_digest(char *digest_name);

//
// Get digest based on digest_name, accounts for various OpenSSL versions
//
EVP_MD *gg_get_digest(char *digest_name)
{
    GG_TRACE("");
    EVP_MD *md = NULL;
#if OPENSSL_VERSION_MAJOR  >= 3
// in OpenSSL3, only if the implementation of digest exists, it will be non-NULL, while EV_get_digestbyname may return non-NULL
// even if not existing
    md = EVP_MD_fetch(NULL, digest_name, NULL);
#else
    md = EVP_get_digestbyname(digest_name);
#endif
    if (md == NULL) gg_sec_err ("Unknown digest");
    return md;
}


//
// Generate HMAC (hash-based message authentication code) for data based on key and hash algo digest_name
// Returns the HMAC.
// If binary is true, then output binary data (otherwise it's hex human readable by default)
//
char *gg_hmac (char *key, char *data, char *digest_name, bool binary)
{
    GG_TRACE("");
    unsigned char hmac[EVP_MAX_MD_SIZE + 1]; // result

    // final result, which may be hex, so it would be double size
    char *out = gg_malloc (binary ? (EVP_MAX_MD_SIZE + 1) : ((EVP_MAX_MD_SIZE + 1)*2)+ 2) ; // +2 is just in case for null

    // get digest
    EVP_MD *md = gg_get_digest(digest_name);

    // do hmac
    int key_len = (int)gg_mem_get_len(gg_mem_get_id( key) );
    int data_len = (int)gg_mem_get_len(gg_mem_get_id( data) );
    unsigned int res_len;
    if (HMAC(md, key, key_len, (unsigned char*)data, data_len, binary?(unsigned char*)out:hmac, &res_len) == NULL) gg_sec_err ("Cannot create HMAC");

#if OPENSSL_VERSION_MAJOR  >= 3
    EVP_MD_free(md); // EVP_get_digestbyname doesn't need freeing
#endif

    // if binary the output is done, set the output length
    if (binary) { gg_mem_set_len (gg_mem_get_id(out), (gg_num)res_len+1); return out; }

    char *p = out;
    // This is if not binary, convert hash to out, which is hexstring
    gg_num i;
    for ( i = 0; i < res_len; i++, p += 2 ) 
    {
        GG_HEX_FROM_BYTE (p, (unsigned int)hmac[i]);
    }
    *p = 0;
    gg_mem_set_len (gg_mem_get_id(out), p - out+1); 
    return out;
}

//
// CSPRNG random data generator. buf is the pre-allocated buffer,
// gg_num is the space available. Returns 1 if okay, otherwise couldn't do it
//
int gg_RAND_bytes(unsigned char *buf, int num) 
{
    GG_TRACE("");
    return RAND_bytes (buf, num);
}

//
// Load crypto cipher and digest algos on startup, called from generated stub for main
//
void gg_sec_load_algos(void) 
{
    GG_TRACE("");
    OpenSSL_add_all_algorithms(); // so algos are available when asked for
}

// 
// Errors out.
// 'err' is gliim messsage, which is supplemented with crypto-provider's message
// it doesn't return, rather ends with fatal error
//
void gg_sec_err (char *err)
{
    GG_TRACE("");
    BIO *bio = BIO_new (BIO_s_mem ()); // this can fail with NULL
    if (bio == NULL) gg_report_error ("%s [could not obtain error message]", err);
    ERR_print_errors (bio); // cannot fail
    char *buf = NULL;
    size_t len = BIO_get_mem_data (bio, &buf); // returns buf, len
    // to get the earliest error: ERR_error_string(ERR_get_error(), NULL)
    gg_report_error ("%s [%.*s]", err, (int)len, buf);
    BIO_free (bio); // never really gets here, but no matter, the program is gone
}


// 
// Return hash value of 'val'.
// 'digest_name' is the name of digest algorithm, see 'openssl list cipher-algorithm'
// The result is a zero-terminated string in hex representation - XX bytes + zero byte.
// XX can be 64, 96 or some other number depending on digest.
// if 'binary' is true, the output is binary.
//
char *gg_hash_data( char *val, char *digest_name, bool binary)
{
    GG_TRACE("");
    unsigned char hash[EVP_MAX_MD_SIZE + 1];

    EVP_MD_CTX *mdctx;
    if ((mdctx = EVP_MD_CTX_new()) == NULL) gg_sec_err ("Cannot allocate digest context");

    // get digest
    EVP_MD *md = gg_get_digest(digest_name);

    EVP_MD_CTX_init(mdctx);
    EVP_DigestInit_ex(mdctx, md, NULL);

    gg_num msg_length = gg_mem_get_len(gg_mem_get_id( val) );

    char *out = (char *) gg_malloc( binary ? (EVP_MAX_MD_SIZE + 1) : ((EVP_MAX_MD_SIZE + 1)*2)+ 2) ; // +2 is just in case for null
    gg_num id = gg_mem_get_id (out);

    char *p = out;
    EVP_DigestUpdate(mdctx, val,(unsigned int) msg_length);
    EVP_DigestFinal_ex(mdctx,binary?(unsigned char*)out:hash, (unsigned int*)&msg_length);
    EVP_MD_CTX_free (mdctx);
    out[msg_length] = 0; // place null after useful data anyway

#if OPENSSL_VERSION_MAJOR  >= 3
    EVP_MD_free(md); // EVP_get_digestbyname doesn't need freeing
#endif    

    // if binary the output is done, set the output length
    if (binary) { gg_mem_set_len (id, msg_length+1); return out; }

    // This is if not binary, convert hash to out, which is hexstring
    gg_num i;
    for ( i = 0; i < msg_length; i++, p += 2 ) 
    {
        GG_HEX_FROM_BYTE (p, (unsigned int)hash[i]);
    }
    *p = 0;
    gg_mem_set_len (id, p - out+1); 
    return out;
}


// 
// Return KD (key derivation) of 'val'. Uses PBKDF2. val_len is length of val or -1 if null-terminated.
// 'digest_name' is the name of digest algorithm, see 'openssl list cipher-algorithm'
// iter_count is gg_num of iterations, salt/salt_len is salt and its length (0 for salt_len if null-terminated)
// key_len is the desired length of output (though string is not of that length)
// if binary is true, the output is binary of key_len length
// The result is a zero-terminated string in hex representation.
//
char *gg_derive_key( char *val, gg_num val_len, char *digest_name, gg_num iter_count, char *salt, gg_num salt_len, gg_num key_len, bool binary )
{
    GG_TRACE("");
    // +1 as 0 is placed after each
    unsigned char *key = gg_malloc (key_len + 1);

    // get digest
    EVP_MD *dgst = gg_get_digest(digest_name);


    if (iter_count == -1) iter_count=1000; 
    if (salt != NULL) 
    {
        gg_num id = gg_mem_get_id(salt);
        if (salt_len == 0) salt_len = gg_mem_get_len(id);
        else if (salt_len > gg_mem_get_len(id))  gg_report_error ("Memory read requested salt of length [%ld] but only [%ld] allocated", salt_len, gg_mem_get_len(id));

    }
    gg_num vl;
    gg_num id = gg_mem_get_id(val);
    if (val_len != -1)
    {
        if (val_len > gg_mem_get_len(id)) gg_report_error ("Memory read requested value of length [%ld] but only [%ld] allocated", val_len, gg_mem_get_len(id));
        vl = val_len;
    } else vl = gg_mem_get_len(id); 
    if (!PKCS5_PBKDF2_HMAC (val, (size_t)vl, (const unsigned char *)salt, salt_len, iter_count, dgst, key_len, key))
    {
        gg_sec_err ("Cannot generate key");
    }
#if OPENSSL_VERSION_MAJOR  >= 3
    EVP_MD_free(dgst); // EVP_get_digestbyname doesn't need freeing
#endif

    if (binary) 
    {
        key[key_len] = 0; // this has the exact length from malloc above
        return (char*)key;
    }
    else
    {
        char *out = (char *) gg_malloc( sizeof(char) * (((key_len + 1)*2)+1) );
        gg_num id = gg_mem_get_id (out);
        char *p = out;
        gg_num i;
        for ( i = 0; i < key_len; i++, p += 2 ) 
        {
            GG_HEX_FROM_BYTE (p, (unsigned int)key[i]);
        }
        *p = 0;
        gg_mem_set_len (id, p - out+1); // set exact memory length
        gg_free (key);
        return out;
    }

}


// 
// Produce a key and IV out of password
// Then use this key/IV to actually encrypt or decrypt.
// 'password' is the password to encrypt with. 'e_ctx' is encrypt context
// (used to encrypt) and 'd_ctx' is decrypt context (used to decrypt).
// 'salt' is the salt used. salt_len is the length of salt, if 0 then strlen(salt), also if salt is NULL, salt_len must be 0.
// iter_count is the number of iterations in generating key and IV, if -1 then it's 1000
// 'cipher' is the cipher algorithm name. This is something one can see with 'openssl list cipher-algorithm'
// Gliimly does not restrict what this may be - which depends on what providers are present on the system,
// it only supplies the default, which is currently aes256.
// 'digest_name' is the digest algo name. This is something one can see with 'openssl list digest-algorithm'
// Gliimly does not restrict what this may be - which depends on what providers are present on the system,
// it only supplies the default, which is currently sha256
// Either e_ctx or d_ctx can be NULL (if we're only encrypting or decrypting).
// Returns 0 if cannot produce the context, 1 if okay.
//
gg_num gg_get_enc_key(char *password, char *salt, gg_num salt_len, gg_num iter_count, EVP_CIPHER_CTX *e_ctx, 
             EVP_CIPHER_CTX *d_ctx, char *cipher_name, char *digest_name)
{
    GG_TRACE("");


    // +1 as 0 is placed after each
    unsigned char key[EVP_MAX_KEY_LENGTH+EVP_MAX_IV_LENGTH+1];

#if OPENSSL_VERSION_MAJOR  >= 3
// in OpenSSL3, only if the implementation of cipher exists, it will be non-NULL, while EV_get_cipherbyname may return non-NULL
// even if not existing
    EVP_CIPHER *cipher;
    cipher = EVP_CIPHER_fetch(NULL, cipher_name, NULL);
#else
    const EVP_CIPHER *cipher;
    cipher = EVP_get_cipherbyname(cipher_name);
#endif
    if(!cipher) { 
        GG_TRACE("Cipher name [%s]", cipher_name);
        gg_sec_err ("Cipher not found");
    }

    // get digest
    EVP_MD *dgst = gg_get_digest(digest_name);

    if (salt != NULL && salt_len == 0) salt_len = gg_mem_get_len(gg_mem_get_id(salt));
    if (iter_count == -1) iter_count=1000; 

    int key_len = EVP_CIPHER_key_length(cipher);
    int iv_len = EVP_CIPHER_iv_length(cipher);

    if (!PKCS5_PBKDF2_HMAC (password, gg_mem_get_len(gg_mem_get_id(password)), (const unsigned char *)salt, salt_len, iter_count, dgst, key_len + iv_len, key))
    {
        gg_sec_err ("Cannot convert password to keyring");
    }

    // This is the reason for encrypt/decrypt in new-key: if we didn't have that
    // then these two would have to repeat for every encryption/decryption, which would
    // make it slower
    if (e_ctx != NULL)
    {
        EVP_CIPHER_CTX_init(e_ctx);
#if OPENSSL_VERSION_MAJOR  >= 3
        if (EVP_EncryptInit_ex2(e_ctx, cipher, key, key+key_len, NULL) !=1) 
#else
        if (EVP_EncryptInit_ex(e_ctx, cipher, NULL, key, key+key_len) !=1) 
#endif
        {
            gg_sec_err ("Cannot encrypt");
        }
    }
    if (d_ctx != NULL)
    {
        EVP_CIPHER_CTX_init(d_ctx);
#if OPENSSL_VERSION_MAJOR  >= 3
        if (EVP_DecryptInit_ex2(d_ctx, cipher, key, key+key_len, NULL) != 1) 
#else
        if (EVP_DecryptInit_ex(d_ctx, cipher, NULL, key, key+key_len) != 1) 
#endif
        {
            gg_sec_err ("Cannot decrypt");
        }
    }
#if OPENSSL_VERSION_MAJOR  >= 3
    EVP_MD_free(dgst); // EVP_get_digestbyname doesn't need freeing
    EVP_CIPHER_free(cipher); // EVP_get_cipherbyname doesn't need freeing
#endif

    return 1;
             
}

// 
// Encrypt string 'plaintext' and return encrypted value, with output parameter 
// 'len' being the length of this encrypted value returned by the function. *len is
// the length of data to encrypt, and also the output length after being done (excluding zero byte at the end).
// is_binary is 1 if the encrypted value is binary and not hex-string, otherwise 0.
// We allocate new memory for encrypted value - caller must de-allocate (this is gg_
// allocation, so you may just leave it to be collected by GLIIMLY memory garbage collector.
// 'e' is the encryption context, produced as e_ctx in gg_get_enc_key().
// iv is the nonce (IV), the length of which should be sufficient for the algorithm
// The maximum length of encrypted data is 2*(input_len+AES_BLOCK_SIZE)+1. Note that 
// AES_BLOCK_SIZE is always 16 bytes. So the maximum is 2*(input_len+16)+1 for the purposes
// of sizing.
//
char *gg_encrypt(EVP_CIPHER_CTX *e, const unsigned char *plaintext, gg_num *len, gg_num is_binary, unsigned char *iv)
{
    gg_num id = gg_mem_get_id((char*)plaintext);
    if (*len == -1) *len = gg_mem_get_len(id);
    else if (*len > gg_mem_get_len(id)) gg_report_error ("Memory used is of length [%ld] but only [%ld] allocated", *len, gg_mem_get_len(id));

    /* maximum length for encrypted value is *len + BLOCK_SIZE -1 
      and block size depends on cipher with EVP_MAX_IV_LENGTH being the max for all if this ever fails*/
    int p_len = *len + EVP_CIPHER_CTX_block_size (e);
    int f_len = 0;
    unsigned char *ciphertext = gg_malloc(p_len+1);  // even for binary, there's +1 allocated
           
    // reuse the values already set, so same context can be reused for multiple encryptions
    // except nonce (IV), which is unique each time
    // if iv is NULL, Gliimly allows that only if cache is not used. In that case, the password is
    // computed each time and salt must be different each time, so new iv is generated every time.
    if (iv != NULL)
    {
        // make sure IV length is sufficient, or otherwise EVP_EncryptInit_ex2 will SIGSEG trying to copy nonexistent bytes
        int req_ivlen = EVP_CIPHER_CTX_get_iv_length(e);
        gg_num iv_len = gg_mem_get_len (gg_mem_get_id(iv)); 
        if (iv_len < req_ivlen) gg_report_error ("Length of Initialization Vector (IV) must be [%d] but only [%ld] allocated", req_ivlen, iv_len);
    }
#if OPENSSL_VERSION_MAJOR  >= 3
    EVP_EncryptInit_ex2(e, NULL, NULL, iv, NULL);
#else
    EVP_EncryptInit_ex(e, NULL, NULL, NULL, iv);
#endif
                
    /* update ciphertext, p_len is filled with the length of encrypted text
      len is the size of plaintext in bytes */
    EVP_EncryptUpdate(e, ciphertext, &p_len, plaintext, *len);
                     
    /* add to ciphertext the final remaining bytes */
    EVP_EncryptFinal_ex(e, ciphertext+p_len, &f_len);
                          
    *len = p_len + f_len;

    if (is_binary == 0)
    {
        //
        // Make encrypted text as hex-string for db storage
        //
        char *hex_ciphertext = gg_malloc(2*(*len)+1); 
        gg_num id = gg_mem_get_id (hex_ciphertext);
        gg_num i;
        //
        // Update progress by 2 bytes in hex-string mode
        //
        gg_num tot_len = 0;

        for (i = 0; i < *len; i++)
        {
            GG_HEX_FROM_BYTE (hex_ciphertext+tot_len, (unsigned int)ciphertext[i]);
            tot_len += 2;
        }

        hex_ciphertext[*len = tot_len] = 0;
        gg_mem_set_len (id, tot_len+1);
        gg_free (ciphertext); // free binary encrypted value
        return hex_ciphertext; // return hex value
    }
    else
    {
        //
        // The encrypted value is binary, not a hex-string, typically for files
        //
        gg_num id = gg_mem_get_id (ciphertext);
        ciphertext[p_len + f_len] =0; // add 0 at the end
        gg_mem_set_len (id, p_len+f_len+1);
        return (char*)ciphertext;
    }

    return GG_EMPTY_STRING; // no purpose, eliminate compiler warning about non-return from function
}


// 
// Decrypt string 'ciphertext' and return decrypted value, with output parameter 
// 'len' being the length of this decrypted value returned by the function (excluding zero byte at the end -
// the decrypted value doesn't have to be a string, but it will have a zero byte at the end).
// 'len' is also input parameter - it's the length of input data to decrypt (-1 if not given).
// We allocate new memory for decrypted value - caller must de-allocate (this is gg_
// allocation, so you may just leave it to be collected by GLIIMLY memory garbage collector.
// 'e' is the encryption context, produced as d_ctx in gg_get_enc_key().
// iv is the nonce (IV), the length of which should be sufficient for the algorithm
// 'is_binary' is 1 if encrypted value was encrypted in binary mode, and 0 if as a hex string.
// Note that ciphertext must be the result of gg_encrypt() 
//
char *gg_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, gg_num *len, gg_num is_binary, unsigned char *iv)
{
   unsigned char *cipher_bin;
   gg_num id = gg_mem_get_id(ciphertext);
   if (*len == -1) *len = gg_mem_get_len(id);
   else if (*len > gg_mem_get_len(id)) gg_report_error ("Memory used is of length [%ld] but only [%ld] allocated", *len, gg_mem_get_len(id));
   if (is_binary == 0)
   {
       cipher_bin = gg_malloc (*len/2 + 2); // actually needs only *len/2, 1 for odd, 1 for null just in case
       //
       // convert ciphertext from hex back to binary
       // (if the encrypted value is hex-string)
       //
       gg_num i;
       gg_num curr_byte = 0;
       for (i = 0; i < *len; i+=2)
       {
           cipher_bin[curr_byte] = ((unsigned int)(GG_CHAR_FROM_HEX(ciphertext[i]))<<4) + (unsigned int)GG_CHAR_FROM_HEX(ciphertext[i+1]);
           curr_byte++;
       }
       cipher_bin[*len = curr_byte] = 0;
   } else cipher_bin = ciphertext;


   /* plaintext is equal or lesser length than that of cipher_bin*/
   int p_len = *len, f_len = 0;
   unsigned char *plaintext = gg_malloc(p_len);
   gg_num pid = gg_mem_get_id (plaintext);
           
   // reuse decryption context in case multiple decryptions done with the same context
#if OPENSSL_VERSION_MAJOR  >= 3
   EVP_DecryptInit_ex2(e, NULL, NULL, iv, NULL);
#else
   EVP_DecryptInit_ex(e, NULL, NULL, NULL, iv);
#endif
   // decrypt data
   EVP_DecryptUpdate(e, plaintext, &p_len, cipher_bin, *len);
   EVP_DecryptFinal_ex(e, plaintext+p_len, &f_len);
                  
   *len = p_len + f_len;
   plaintext[*len] = 0; 
   gg_mem_set_len (pid, *len+1);

   if (is_binary == 0) gg_free (cipher_bin); // deallocate binary encrypted if allocated
   return (char*)plaintext;
}





// 
// Encode data in Base64. 'in' is the input, of length in_len. 'out' is allocated
// here and is the result of encoding.
// Note: all b64 data must be produced on a single line, per openssl docs.
// If in_len is -1, then it become the length of string in.
//
void gg_b64_encode(char* in, gg_num in_len, char** out)
{ 
    GG_TRACE("");
    gg_num id = gg_mem_get_id(in);
    if (in_len == -1) in_len = gg_mem_get_len(id);
    else if (in_len > gg_mem_get_len(id)) gg_report_error ("Memory read requested of length [%ld] but only [%ld] allocated", in_len, gg_mem_get_len(id));

    *out = gg_malloc(4*((in_len+2)/3)+2); 
    gg_num oid = gg_mem_get_id (*out);
    gg_num out_len = EVP_EncodeBlock((unsigned char*)(*out), (unsigned char*)in, in_len);
    gg_mem_set_len (oid, out_len+1);
}
 
// 
// Decode Base64 data. 'in' is b64 data of length 'in_len', and the output is allocated
// as 'out'.
// Note: all b64 data must be on a single line, per openssl docs.
// If in_len is -1, then it become the length of string in.
//
void gg_b64_decode (char* in, gg_num ilen, char** out)
{
    GG_TRACE("");
    gg_num id = gg_mem_get_id(in);
    if (ilen == -1) ilen = gg_mem_get_len(id);
    else if (ilen > gg_mem_get_len(id)) gg_report_error ("Memory read requested of length [%ld] but only [%ld] allocated", ilen, gg_mem_get_len(id));

    *out = gg_malloc(3*ilen/4+4 + 1);
    gg_num oid = gg_mem_get_id (*out);
    // remove trailing =s because they are added if the original input is not of size multiple of 3
    gg_num final = 0;
    while (in[ilen-1-final] == '=') final++;
    gg_num out_len = EVP_DecodeBlock((unsigned char*)(*out), (unsigned char*)in, ilen);
    (*out)[out_len - final] = 0; // we added +1 in gg_malloc above to account for 0 always at the end (not part of output length)
    gg_mem_set_len (oid, out_len - final+1);
}

