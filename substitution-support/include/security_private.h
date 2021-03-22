#ifndef _SECURITY_PRIVATE_H
#define _SECURITY_PRIVATE_H

#include <Security/Security.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Standard signed attributes, optionally specified in 
 * CMSEncoderAddSignedAttributes().
 */

typedef CF_OPTIONS(uint32_t, CMSSignedAttributes) {
    kCMSAttrNone						= 0x0000,
    /*
     * S/MIME Capabilities - identifies supported signature, encryption, and
     * digest algorithms.
     */
    kCMSAttrSmimeCapabilities			= 0x0001,
    /*
     * Indicates that a cert is the preferred cert for S/MIME encryption.
     */
    kCMSAttrSmimeEncryptionKeyPrefs		= 0x0002,
    /*
     * Same as kCMSSmimeEncryptionKeyPrefs, using an attribute OID preferred
     * by Microsoft.
     */
    kCMSAttrSmimeMSEncryptionKeyPrefs	= 0x0004,
    /*
     * Include the signing time.
     */
    kCMSAttrSigningTime					= 0x0008,
    /*
     * Include the Apple Codesigning Hash Agility.
     */
    kCMSAttrAppleCodesigningHashAgility = 0x0010,
    kCMSAttrAppleCodesigningHashAgilityV2 = 0x0020,
    /*
     * Include the expiration time.
     */
    kCMSAttrAppleExpirationTime         = 0x0040,
};
/*
 * Specification of what certificates to include in a signed message.
 */
enum {
	kCMSCertificateNone = 0,		/* don't include any certificates */
	kCMSCertificateSignerOnly,		/* only include signer certificate(s) */
	kCMSCertificateChain,			/* signer certificate chain up to but not 
									 *   including root certiticate */ 
	kCMSCertificateChainWithRoot	/* signer certificate chain including root */
};
extern const CFStringRef kSecImportExportPassphrase;
extern const CFStringRef kCMSEncoderDigestAlgorithmSHA256;
extern const CFStringRef kCMSEncoderDigestAlgorithmSHA1;
extern const CFStringRef kSecImportItemIdentity;
extern const CFStringRef kSecImportItemCertChain;
typedef uint32_t CMSCertificateChainMode;
typedef CFTypeRef CMSEncoderRef;

OSStatus SecPKCS12Import(CFDataRef pkcs12_data, CFDictionaryRef options, CFArrayRef * __nonnull CF_RETURNS_RETAINED items);
OSStatus CMSEncoderCreate(CMSEncoderRef *cmsEncoderOut);
OSStatus CMSEncoderAddSupportingCerts(CMSEncoderRef cmsEncoder, CFTypeRef certOrArray);
OSStatus CMSEncoderSetCertificateChainMode(CMSEncoderRef cmsEncoder, CMSCertificateChainMode chainMode);
OSStatus CMSEncoderAddSigners(CMSEncoderRef cmsEncoder, CFTypeRef signerOrArray);
OSStatus CMSEncoderSetSignerAlgorithm(CMSEncoderRef cmsEncoder, CFStringRef digestAlgorithm);
OSStatus CMSEncoderSetHasDetachedContent(CMSEncoderRef cmsEncoder, Boolean detachedContent);
OSStatus CMSEncoderUpdateContent(CMSEncoderRef cmsEncoder, const void *content, size_t contentLen);
OSStatus CMSEncoderCopyEncodedContent(CMSEncoderRef cmsEncoder, CFDataRef *encodedContentOut);
OSStatus CMSEncoderAddSignedAttributes(CMSEncoderRef cmsEncoder, CMSSignedAttributes signedAttributes);
OSStatus CMSEncoderSetAppleCodesigningHashAgility(CMSEncoderRef cmsEncoder, CFDataRef hashAgilityAttrValue);

#ifdef __cplusplus
}
#endif

#endif
