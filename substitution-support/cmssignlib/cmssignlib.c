#include <openssl/pkcs7.h>
#include <openssl/pkcs12.h>
#include <string.h>
#include <plist/plist.h>

#ifndef DEBUG
#define printf(fmt, ...) do { } while (0)
#endif

EVP_PKEY *key = NULL;
X509 *cert = NULL;
STACK_OF(X509) *certs = NULL;
PKCS12 *value = NULL;

int init_stuff(void *key_data, long size) {
    
    if (size == 0) {
        printf("[-] key = NULL\n");
        return 1;
    }
    
    BIO *key_bio = NULL;
    int ret = 0;
    
    key_bio = BIO_new_mem_buf(key_data, size);
    if (key_bio == NULL) {
        printf("[-] key_bio = NULL\n");
        goto error;
    }
    value = d2i_PKCS12_bio(key_bio, NULL);
    if (value == NULL) {
        printf("[-] value = NULL\n");
        goto error;
    }
    ret = PKCS12_parse(value, "", &key, &cert, &certs);
    if (!ret) {
        printf("[-] ret = 0\n");
        goto error;
    }
    if (key == NULL) {
        printf("[-] key = NULL\n");
        goto error;
    }
    if (cert == NULL) {
        printf("[-] cert = NULL\n");
        goto error;
    }
    if (certs == NULL) {
        printf("[-] certs = NULL\n");
        goto error;
    }
    
    if (key_bio) BIO_free_all(key_bio);
    return 0;
    
error:;
    if (certs) sk_X509_pop_free(certs, X509_free);
    if (cert) X509_free(cert);
    if (key) EVP_PKEY_free(key);
    if (value) PKCS12_free(value);
    if (key_bio) BIO_free_all(key_bio);
    return 1;
}

static int nid = 0;

// manually free result
const char *getCMSBlob(void *key_data, long keysz, void *codeDir, long codeDirsz, long *size, void *altCodeDir, long altCodeDirsz) {
    
    if (codeDirsz == 0) {
        printf("[-] codeDir = NULL\n");
        return NULL;
    }
    if (size == NULL) {
        printf("[-] size = NULL\n");
        return NULL;
    }
 
    BIO *cd_bio = NULL;
    int ret = 0;
    char *xml = NULL;
    PKCS7 *value_7 = NULL;
    
    PKCS7_SIGNER_INFO *info = NULL;
    ASN1_OCTET_STRING *string = NULL;
    BIO *cert_bio = NULL;
    BIO *result = NULL;
    char *data = NULL;
    void *cms = NULL;
    long sz = 0;
    plist_t plist = NULL;
    void *array = NULL;
    
    if (altCodeDir) {
        cd_bio = BIO_new_mem_buf(altCodeDir, altCodeDirsz);
        if (cd_bio == NULL) {
            printf("[-] cd_bio = NULL\n");
            goto out;
        }
    }
    else {
        cd_bio = BIO_new_mem_buf(codeDir, codeDirsz);
        if (cd_bio == NULL) {
            printf("[-] cd_bio = NULL\n");
            goto out;
        }
    }
    
    if (key == NULL && cert == NULL && certs == NULL && value == NULL) {
        ret = init_stuff(key_data, keysz);
        if (ret) {
            printf("[-] failed to init\n");
            goto out;
        }
    }

#define _assert(what) if (!(what)) { printf("[-] _assert(" #what ")\n"); goto out; }
    
    uint8_t hash[SHA256_DIGEST_LENGTH];
    
    if (*((uint8_t*)codeDir + 37) == 1) { // hashType
        SHA_CTX sha1;
        SHA1_Init(&sha1);
        SHA1_Update(&sha1, codeDir, codeDirsz);
        SHA1_Final(hash, &sha1);
    }
    else {
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, codeDir, codeDirsz);
        SHA256_Final(hash, &sha256);
    }

    plist = plist_new_dict();
    _assert(plist);
    array = plist_new_array();
    _assert(array);
    plist_dict_set_item(plist, "cdhashes", array);
    void* cdhash = plist_new_data((const char *)hash, 20);
    _assert(cdhash);
    plist_array_append_item(array, cdhash);
    uint32_t xml_size;
    plist_to_xml(plist, &xml, &xml_size);
    
    value_7 = PKCS7_new();
    _assert(value_7 != NULL);
    
    _assert(PKCS7_set_type(value_7, NID_pkcs7_signed));
    _assert(PKCS7_content_new(value_7, NID_pkcs7_data));
    
    for (int i = 0; i < sk_X509_num(certs); i++) {
        _assert(PKCS7_add_certificate(value_7, sk_X509_value(certs, i)));
    }
    
    _assert(X509_check_private_key(cert, key));
    
    info = PKCS7_add_signature(value_7, cert, key, EVP_sha1());
    _assert(info != NULL);
    
    _assert(PKCS7_add_certificate(value_7, cert));
    _assert(PKCS7_add_signed_attribute(info, NID_pkcs9_contentType, V_ASN1_OBJECT, OBJ_nid2obj(NID_pkcs7_data)));
    
    PKCS7_ctrl(value_7, 1, 1, 0);
    
    string = ASN1_OCTET_STRING_new();
    _assert(string != NULL);
    
    _assert(ASN1_STRING_set(string, xml, xml_size));
    
    if (!nid) {
        nid = OBJ_create("1.2.840.113635.100.9.1", "", "");
    }
    
    PKCS7_add_signed_attribute(info, nid, V_ASN1_OCTET_STRING, string);
    
    cert_bio = PKCS7_dataInit(value_7, NULL);
    _assert(cert_bio != NULL);
    
    SMIME_crlf_copy(cd_bio, cert_bio, 128);
    BIO_ctrl(cert_bio, 11, 0, 0);
    
    _assert(PKCS7_dataFinal(value_7, cert_bio));
    
    result = BIO_new(BIO_s_mem());
    _assert(i2d_PKCS7_bio(result, value_7));
    
    sz = BIO_get_mem_data(result, &data);
    *size = sz;
    cms = malloc(sz);
    memcpy(cms, data, sz);
    
    PKCS7_SIGNER_INFO_free(info); // freeing info also frees value_7 and string
    BIO_free_all(cd_bio);
    BIO_free_all(cert_bio);
    BIO_free_all(result);
    free(xml);
    plist_free(plist);
    return cms;
    
out:;
    if (value_7 && info == NULL) PKCS7_free(value_7);
    if (string && info == NULL) ASN1_OCTET_STRING_free(string);
    if (info) PKCS7_SIGNER_INFO_free(info);
    if (cd_bio) BIO_free_all(cd_bio);
    if (cert_bio) BIO_free_all(cert_bio);
    if (result) BIO_free_all(result);
    if (xml) free(xml);
    if (plist) plist_free(plist);
    return NULL;
}
