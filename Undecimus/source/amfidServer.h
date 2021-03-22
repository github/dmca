#ifndef	_amfid_server_
#define	_amfid_server_

/* Module amfid */

#include <string.h>
#include <mach/ndr.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/notify.h>
#include <mach/mach_types.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/port.h>
	
/* BEGIN VOUCHER CODE */

#ifndef KERNEL
#if defined(__has_include)
#if __has_include(<mach/mig_voucher_support.h>)
#ifndef USING_VOUCHERS
#define USING_VOUCHERS
#endif
#ifndef __VOUCHER_FORWARD_TYPE_DECLS__
#define __VOUCHER_FORWARD_TYPE_DECLS__
#ifdef __cplusplus
extern "C" {
#endif
	extern boolean_t voucher_mach_msg_set(mach_msg_header_t *msg) __attribute__((weak_import));
#ifdef __cplusplus
}
#endif
#endif // __VOUCHER_FORWARD_TYPE_DECLS__
#endif // __has_include(<mach/mach_voucher_types.h>)
#endif // __has_include
#endif // !KERNEL
	
/* END VOUCHER CODE */

	
/* BEGIN MIG_STRNCPY_ZEROFILL CODE */

#if defined(__has_include)
#if __has_include(<mach/mig_strncpy_zerofill_support.h>)
#ifndef USING_MIG_STRNCPY_ZEROFILL
#define USING_MIG_STRNCPY_ZEROFILL
#endif
#ifndef __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__
#define __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__
#ifdef __cplusplus
extern "C" {
#endif
	extern int mig_strncpy_zerofill(char *dest, const char *src, int len) __attribute__((weak_import));
#ifdef __cplusplus
}
#endif
#endif /* __MIG_STRNCPY_ZEROFILL_FORWARD_TYPE_DECLS__ */
#endif /* __has_include(<mach/mig_strncpy_zerofill_support.h>) */
#endif /* __has_include */
	
/* END MIG_STRNCPY_ZEROFILL CODE */


#ifdef AUTOTEST
#ifndef FUNCTION_PTR_T
#define FUNCTION_PTR_T
typedef void (*function_ptr_t)(mach_port_t, char *, mach_msg_type_number_t);
typedef struct {
        char            *name;
        function_ptr_t  function;
} function_table_entry;
typedef function_table_entry   *function_table_t;
#endif /* FUNCTION_PTR_T */
#endif /* AUTOTEST */

#ifndef	amfid_MSG_COUNT
#define	amfid_MSG_COUNT	5
#endif	/* amfid_MSG_COUNT */

#include "amfid_types.h"

#ifdef __BeforeMigServerHeader
__BeforeMigServerHeader
#endif /* __BeforeMigServerHeader */


/* Routine verify_code_directory */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t verify_code_directory
(
	mach_port_t amfid_port,
	amfid_path_t path,
	uint64_t file_offset,
	int32_t a4,
	int32_t a5,
	int32_t a6,
	int32_t *entitlements_valid,
	int32_t *signature_valid,
	int32_t *unrestrict,
	int32_t *signer_type,
	int32_t *is_apple,
	int32_t *is_developer_code,
	amfid_a13_t a13,
	amfid_cdhash_t cdhash,
	audit_token_t audit
);

/* Routine permit_unrestricted_debugging */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t permit_unrestricted_debugging
(
	mach_port_t amfid_port,
	int32_t *unrestricted_debugging,
	audit_token_t audit
);

#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
boolean_t amfid_server(
		mach_msg_header_t *InHeadP,
		mach_msg_header_t *OutHeadP);

#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
mig_routine_t amfid_server_routine(
		mach_msg_header_t *InHeadP);


/* Description of this subsystem, for use in direct RPC */
extern const struct amfid_subsystem {
	mig_server_routine_t	server;	/* Server routine */
	mach_msg_id_t	start;	/* Min routine number */
	mach_msg_id_t	end;	/* Max routine number + 1 */
	unsigned int	maxsize;	/* Max msg size */
	vm_address_t	reserved;	/* Reserved */
	struct routine_descriptor	/*Array of routine descriptors */
		routine[5];
} amfid_subsystem;

/* typedefs for all requests */

#ifndef __Request__amfid_subsystem__defined
#define __Request__amfid_subsystem__defined

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		mach_msg_type_number_t pathOffset; /* MiG doesn't use it */
		mach_msg_type_number_t pathCnt;
		char path[1024];
		uint64_t file_offset;
		int32_t a4;
		int32_t a5;
		int32_t a6;
	} __Request__verify_code_directory_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
	} __Request__permit_unrestricted_debugging_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif
#endif /* !__Request__amfid_subsystem__defined */


/* union of all requests */

#ifndef __RequestUnion__amfid_subsystem__defined
#define __RequestUnion__amfid_subsystem__defined
union __RequestUnion__amfid_subsystem {
	__Request__verify_code_directory_t Request_verify_code_directory;
	__Request__permit_unrestricted_debugging_t Request_permit_unrestricted_debugging;
};
#endif /* __RequestUnion__amfid_subsystem__defined */
/* typedefs for all replies */

#ifndef __Reply__amfid_subsystem__defined
#define __Reply__amfid_subsystem__defined

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
		int32_t entitlements_valid;
		int32_t signature_valid;
		int32_t unrestrict;
		int32_t signer_type;
		int32_t is_apple;
		int32_t is_developer_code;
		mach_msg_type_number_t a13Offset; /* MiG doesn't use it */
		mach_msg_type_number_t a13Cnt;
		char a13[4096];
		amfid_cdhash_t cdhash;
	} __Reply__verify_code_directory_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
		int32_t unrestricted_debugging;
	} __Reply__permit_unrestricted_debugging_t __attribute__((unused));
#ifdef  __MigPackStructs
#pragma pack()
#endif
#endif /* !__Reply__amfid_subsystem__defined */


/* union of all replies */

#ifndef __ReplyUnion__amfid_subsystem__defined
#define __ReplyUnion__amfid_subsystem__defined
union __ReplyUnion__amfid_subsystem {
	__Reply__verify_code_directory_t Reply_verify_code_directory;
	__Reply__permit_unrestricted_debugging_t Reply_permit_unrestricted_debugging;
};
#endif /* __RequestUnion__amfid_subsystem__defined */

#ifndef subsystem_to_name_map_amfid
#define subsystem_to_name_map_amfid \
    { "verify_code_directory", 1000 },\
    { "permit_unrestricted_debugging", 1001 }
#endif

#ifdef __AfterMigServerHeader
__AfterMigServerHeader
#endif /* __AfterMigServerHeader */

#endif	 /* _amfid_server_ */
