/*
 * Copyright (c) 2006-2007 Apple Inc. All Rights Reserved.
 * 
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/*!
	@header CSCommon
	CSCommon is the common header of all Code Signing API headers.
	It defines types, constants, and error codes.
*/
#ifndef _H_CSCOMMON
#define _H_CSCOMMON

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <CoreFoundation/CoreFoundation.h>


/*
	Code Signing specific OSStatus codes.
	[Assigned range 0xFFFE_FAxx].
*/
enum {
	errSecCSUnimplemented = -67072,		/* unimplemented code signing feature */
	errSecCSInvalidObjectRef,			/* invalid API object reference */
	errSecCSInvalidFlags,				/* invalid or inapprpopriate API flag(s) specified */
	errSecCSObjectRequired,				/* a required pointer argument was NULL */
	errSecCSStaticCodeNotFound,			/* cannot find code object on disk */
	errSecCSUnsupportedGuestAttributes,	/* cannot locate guests using this attribute set */
	errSecCSInvalidAttributeValues,		/* given attribute values are invalid */
	errSecCSNoSuchCode,					/* host has no guest with the requested attributes */
	errSecCSMultipleGuests,				/* host has multiple guests with this attribute value */
	errSecCSGuestInvalid,				/* code identity has been invalidated */
	errSecCSUnsigned,					/* code object is not signed */
	errSecCSSignatureFailed,			/* code or signature modified */
	errSecCSSignatureNotVerifiable,			/* signature cannot be read, e.g., due to a filesystem that maps root to an unprivileged user */
	errSecCSSignatureUnsupported,		/* unsupported type or version of signature */
	errSecCSBadDictionaryFormat,		/* a required plist file or resource is malformed */
	errSecCSResourcesNotSealed,			/* resources are not sealed by signature */
	errSecCSResourcesNotFound,			/* cannot find sealed resources in code */
	errSecCSResourcesInvalid,			/* the sealed resource directory is invalid */
	errSecCSBadResource,				/* a sealed resource is missing or invalid */
	errSecCSResourceRulesInvalid,		/* invalid resource selection rule(s) */
	errSecCSReqInvalid,					/* invalid or corrupted code requirement(s) */
	errSecCSReqUnsupported,				/* unsupported type or version of code requirement(s) */
	errSecCSReqFailed,					/* failed to satisfy code requirement(s) */
	errSecCSBadObjectFormat,			/* object file format invalid or unsuitable */
	errSecCSInternalError,				/* internal error in Code Signing subsystem */
	errSecCSHostReject,					/* code rejected its host */
	errSecCSNotAHost,					/* this code is not a host */
	errSecCSSignatureInvalid,			/* invalid format for signature */
	errSecCSHostProtocolRelativePath,	/* host protocol violation - absolute guest path required */
	errSecCSHostProtocolContradiction,	/* host protocol violation - contradictory hosting modes */
	errSecCSHostProtocolDedicationError, /* host protocol violation - operation not allowed with/for a dedicated guest */
	errSecCSHostProtocolNotProxy,		/* host protocol violation - proxy hosting not engaged */
	errSecCSHostProtocolStateError,		/* host protocol violation - invalid guest state change request */
	errSecCSHostProtocolUnrelated,		/* host protocol violation - the given guest is not a guest of the given host */
	errSecCSInvalidOperation,			/* requested operation is not valid */
	errSecCSNotSupported,				/* operation not supported for this type of code */
};


/*
 * Code Signing specific CFError "user info" keys.
 * In calls that can return CFErrorRef indications, if a CFErrorRef is actually
 * returned, its "user info" dictionary will contain some (but not all) of the
 * following keys to more closely describe the circumstances of the failure.
 */
extern const CFStringRef kSecCFErrorPattern;		/* CFStringRef: invalid resource selection pattern encountered */
extern const CFStringRef kSecCFErrorResourceSeal;	/* CFTypeRef: invalid component in resource seal (CodeResources) */
extern const CFStringRef kSecCFErrorResourceAdded;	/* CFURLRef: unsealed resource found */
extern const CFStringRef kSecCFErrorResourceAltered; /* CFURLRef: modified resource found */
extern const CFStringRef kSecCFErrorResourceMissing; /* CFURLRef: sealed (non-optional) resource missing */
extern const CFStringRef kSecCFErrorInfoPlist;		/* CFTypeRef: Info.plist dictionary or component found invalid */
extern const CFStringRef kSecCFErrorGuestAttributes; /* CFTypeRef: Guest attribute set of element not accepted */
extern const CFStringRef kSecCFErrorRequirementSyntax; /* CFStringRef: compilation error for Requirement source */


/*!
	@typedef SecCodeRef
	This is the type of a reference to running code.

	In many (but not all) calls, this can be passed to a SecStaticCodeRef
	argument, which performs an implicit SecCodeCopyStaticCode call and
	operates on the result.
*/
typedef struct __SecCode *SecCodeRef;				/* running code */

/*!
	@typedef SecStaticCodeRef
	This is the type of a reference to static code on disk.
*/
typedef struct __SecCode const *SecStaticCodeRef;	/* code on disk */

/*!
	@typedef SecRequirementRef
	This is the type of a reference to a code requirement.
*/
typedef struct __SecRequirement *SecRequirementRef;	/* code requirement */


/*!
	@typedef SecGuestRef
	An abstract handle to identify a particular Guest in the context of its Host.
	
	Guest handles are assigned by the host at will, with kSecNoGuest (zero) being
	reserved as the null value). They can be reused for new children if desired.
*/
typedef u_int32_t SecGuestRef;

enum {
	kSecNoGuest = 0,		/* not a valid SecGuestRef */
};


/*!
	@typddef SecCSFlags
	This is the type of flags arguments to Code Signing API calls.
	It provides a bit mask of request and option flags. All of the bits in these
	masks are reserved to Apple; if you set any bits not defined in these headers,
	the behavior is generally undefined.
	
	This list describes the flags that are shared among several Code Signing API calls.
	Flags that only apply to one call are defined and documented with that call.
	Global flags are assigned from high order down (31 -> 0); call-specific flags
	are assigned from the bottom up (0 -> 31).

	@constant kSecCSDefaultFlags
	When passed to a flags argument throughout, indicates that default behavior
	is desired. Do not mix with other flags values.
	@constant kSecCSConsiderExpiration
	When passed to a call that performs code validation, requests that code signatures
	made by expired certificates be rejected. By default, expiration of participating
	certificates is not automatic grounds for rejection.
*/
typedef uint32_t SecCSFlags;

enum {
	kSecCSDefaultFlags = 0,					/* no particular flags (default behavior) */
	
	kSecCSConsiderExpiration = 1 << 31,		/* consider expired certificates invalid */
};


/*!
	@typedef SecCodeSignatureFlags
	This is the type of option flags that can be embedded in a code signature
	during signing, and that govern the use of the signature thereafter.
	Some of these flags can be set through the codesign(1) command's --options
	argument; some are set implicitly based on signing circumstances; and all
	can be set with the kSecCodeSignerFlags item of a signing information dictionary.
	
	@constant kSecCodeSignatureHost
	Indicates that the code may act as a host that controls and supervises guest
	code. If this flag is not set in a code signature, the code is never considered
	eligible to be a host, and any attempt to act like one will be ignored.
	@constant kSecCodeSignatureAdhoc
	The code has been sealed without a signing identity. No identity may be retrieved
	from it, and any code requirement placing restrictions on the signing identity
	will fail. This flag is set by the code signing API and cannot be set explicitly.
	@constant kSecCodeSignatureForceHard
	Implicitly set the "hard" status bit for the code when it starts running.
	This bit indicates that the code prefers to be denied access to a resource
	if gaining such access would cause its invalidation. Since the hard bit is
	sticky, setting this option bit guarantees that the code will always have
	it set.
	@constant kSecCodeSignatureForceKill
	Implicitly set the "kill" status bit for the code when it starts running.
	This bit indicates that the code wishes to be terminated with prejudice if
	it is ever invalidated. Since the kill bit is sticky, setting this option bit
	guarantees that the code will always be valid, since it will die immediately
	if it becomes invalid.
	@constant kSecCodeSignatureForceExpiration
	Forces the kSecCSConsiderExpiration on all validations of the code.
 */
typedef uint32_t SecCodeSignatureFlags;

enum {
	kSecCodeSignatureHost = 0x0001,			/* may host guest code */
	kSecCodeSignatureAdhoc = 0x0002,		/* must be used without signer */
	kSecCodeSignatureForceHard = 0x0100,	/* always set HARD mode on launch */
	kSecCodeSignatureForceKill = 0x0200,	/* always set KILL mode on launch */
	kSecCodeSignatureForceExpiration = 0x0400, /* force certificat expiration checks */
};


/*!
	@typedef SecRequirementType
	An enumeration indicating different types of internal requirements for code.
*/
typedef uint32_t SecRequirementType;

enum {
	kSecHostRequirementType = 1,			/* what hosts may run us */
	kSecGuestRequirementType = 2,			/* what guests we may run */
	kSecDesignatedRequirementType = 3,		/* designated requirement */
	kSecLibraryRequirementType = 4,			/* what libraries we may link against */
	kSecInvalidRequirementType,				/* invalid type of Requirement (must be last) */
	kSecRequirementTypeCount = kSecInvalidRequirementType /* number of valid requirement types */
};


#ifdef __cplusplus
}
#endif

#endif //_H_CSCOMMON
