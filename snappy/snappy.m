/* Copyright 2018 Sam Bingner All Rights Reserved
	 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/snapshot.h>
#include <getopt.h>
#ifdef __arm64__
#include <dlfcn.h>
#endif
#include <IOKit/IOKit.h>
#import <Foundation/Foundation.h>
#include "snappy.h"

enum operation {
	OP_UNDEFINED = 0,
	OP_LIST,
	OP_CREATE,
	OP_DELETE,
	OP_MOUNT,
	OP_RENAME,
	OP_REVERT,
	OP_SHOWHASH
};

static struct option long_options[] = {
	{"help",    no_argument,       0, 'h'},
	{"filesystem", required_argument, 0, 'f'},
	{"list", no_argument, 0, 'l'},
	{"create",  required_argument, 0, 'c'},
	{"delete",  required_argument, 0, 'd'},
	{"rename",  required_argument, 0, 'r'},
	{"showhash", no_argument, 0, 's'},
	{"to",  required_argument, 0, 't'},
	{"to-system",  no_argument, 0, 'x'},
	{"mount",  required_argument, 0, 'm'},
	{"revert", required_argument, 0, 'v'},
	{0,        0,                 0,  0 }
};

void usage(void);
int main(int argc, char **argv, char **envp);

#ifdef __arm64__
void patch_setuid() {
	void* libjb = dlopen("/usr/lib/libjailbreak.dylib", RTLD_LAZY);
	if (!libjb)
		return;

	// Reset errors
	dlerror();
	typedef void (*fix_setuid_prt_t)(pid_t pid);
	fix_setuid_prt_t jb_oneshot_fix_setuid_now = (fix_setuid_prt_t)dlsym(libjb, "jb_oneshot_fix_setuid_now");

	const char *dlsym_error = dlerror();
	if (dlsym_error || jb_oneshot_fix_setuid_now == NULL)
		return;

	jb_oneshot_fix_setuid_now(getpid());
	setuid(0);
}
#endif

void usage(void)
{
	printf("Usage: snappy -f DIR [OPTIONS...]\n"
			"\t-h, --help\t\tPrint this help\n"
			"\t-f, --filesystem DIR\tFilesystem to operate on (mountpoint)\n"
			"\t-l, --list\t\tList snapshots on filesystem\n"
			"\t-c, --create NAME\tCreate a snapshot named NAME\n"
			"\t-d, --delete NAME\tDelete a snapshot named NAME\n"
			"\t-r, --rename NAME\tRename a snapshot named NAME to name supplied by --to\n"
			"\t-m, --mount NAME\tMount snapshot named NAME to path specified by --to\n"
#if TARGET_IPHONE
			"\t\t\t\t\t(Mount currently not working on iOS)\n"
#endif
			"\t-t, --to NAME\n"
			"\t-v, --revert NAME\tRevert to snapshot named NAME\n"
			"\t-s, --showhash\t\tShow the name of the system snapshot for this boot-manifest-hash\n"
			"\t-x, --to-system\t\tSet the target snapshot name to be the iOS system-snapshot\n"
			);
}

int main(int argc, char **argv, char **envp)
{
#ifdef __arm64__
	if (geteuid()!=0)
		patch_setuid();
#endif
	int option_index = 0;
	int dirfd = -1;
	char *hashsnap = NULL;
	char *hash=NULL;
	char *fspath = NULL;
	char *snapName = NULL;
	char *to = NULL;

	enum operation op = OP_UNDEFINED;
	int c;
	if (argc<2) {
		usage();
		exit(1);
	}
	while ((c = getopt_long(argc, argv, "hf:c:d:r:st:lm:v:x", long_options, &option_index)) != -1) {
		switch (c) {
			case 'c':
				if (op != OP_UNDEFINED) {
					fprintf(stderr, "Error: multiple operations not supported\n");
					usage();
					exit(1);
				}
				op = OP_CREATE;
				snapName = optarg;
				break;
			case 'd':
				if (op != OP_UNDEFINED) {
					fprintf(stderr, "Error: multiple operations not supported\n");
					usage();
					exit(1);
				}
				op = OP_DELETE;
				snapName = optarg;
				break;
			case 'f':
				if (fspath != NULL) {
					fprintf(stderr, "Multiple --filesystem statements\n");
					usage();
					exit(1);
				}
				fspath = optarg;
				break;
			case 'h':
				usage();
				exit(0);
				break;
			case 'l':
				if (op != OP_UNDEFINED) {
					fprintf(stderr, "Error: multiple operations not supported\n");
					usage();
					exit(1);
				}
				op = OP_LIST;
				break;
			case 'm':
				if (op != OP_UNDEFINED) {
					fprintf(stderr, "Error: multiple operations not supported\n");
					usage();
					exit(1);
				}
				op = OP_MOUNT;
				snapName = optarg;
				break;
			case 'r':
				if (op != OP_UNDEFINED) {
					fprintf(stderr, "Error: multiple operations not supported\n");
					usage();
					exit(1);
				}
				op = OP_RENAME;
				snapName = optarg;
				break;
			case 's':
				if (op != OP_UNDEFINED) {
					fprintf(stderr, "Error: multiple operations not supported\n");
					usage();
					exit(1);
				}
				op = OP_SHOWHASH;
				break;
			case 't':
				if (to != NULL) {
					fprintf(stderr, "Multiple --to statements\n");
					usage();
					exit(1);
				}
				to = optarg;
				break;
			case 'v':
				if (op != OP_UNDEFINED) {
					fprintf(stderr, "Error: multiple operations not supported\n");
					usage();
					exit(1);
				}
				op = OP_REVERT;
				snapName = optarg;
				break;
			case 'x':
				if (to != NULL) {
					fprintf(stderr, "Multiple --to statements\n");
					usage();
					exit(1);
				}
				hashsnap = copySystemSnapshot();
				if (hashsnap == NULL) {
					fprintf(stderr, "Error: Unable to generate destination snapshot name\n");
					exit(1);
				}
				to = hashsnap;
				break;
			default:
				usage();
				exit(1);
				break;
		}
	}

	bool error=false;
	setuid(0);

	if (op != OP_SHOWHASH) {
		if (fspath != NULL) {
			dirfd = open(fspath, O_RDONLY);
			if (dirfd == -1) {
				fprintf(stderr, "Error unable to open path %s: %s", fspath, strerror(errno));
				exit(1);
			}
		} else {
			fprintf(stderr, "Error: no --fspath specified\n");
			usage();
			exit(1);
		}
	}
	switch (op) {
		case OP_CREATE:
			printf("Will create snapshot named \"%s\" on fs \"%s\"...\n", snapName, fspath);
			if (fs_snapshot_create(dirfd, snapName, 0) == ERR_SUCCESS) {
				printf("Success\n");
			} else {
				perror("fs_snapshot_create");
				printf("Failure\n");
				error=true;
			}
			break;
		case OP_DELETE:
			printf("Will delete snapshot named %s from fs %s\n", snapName, fspath);
			if (fs_snapshot_delete(dirfd, snapName, 0) == ERR_SUCCESS) {
				printf("Success\n");
			} else {
				perror("fs_snapshot_delete");
				printf("Failure\n");
				error=true;
			}
			break;
		case OP_RENAME:
			if (to == NULL) {
				fprintf(stderr, "Error: rename requested but no new name (--to) provided\n");
				usage();
				error=true;
				break;
			}
			printf("Will rename snapshot %s on fs %s to %s\n", snapName, fspath, to);
			if (fs_snapshot_rename(dirfd, snapName, to, 0) == ERR_SUCCESS) {
				printf("Success\n");
			} else {
				perror("fs_snapshot_rename");
				printf("Failure\n");
				error=true;
			}
			break;
		case OP_SHOWHASH:
			hash = copySystemSnapshot();
			if (hash) {
				printf("System Snapshot: %s\n", hash);
				free(hash);
			} else {
				perror("Unable to get boot-manifest-hash");
				error=true;
			}
			break;
		case OP_MOUNT:
			if (to == NULL) {
				fprintf(stderr, "Error: mount requested but no mount path (--to) provided\n");
				usage();
				error=true;
				break;
			}
			printf("Will mount snapshot %s on fs %s to %s\n", snapName, fspath, to);
			if (fs_snapshot_mount(dirfd, to, snapName, 0) == ERR_SUCCESS) {
				printf("Success\n");
			} else {
				perror("fs_snapshot_mount");
				printf("Failure\n");
				error=true;
			}
			break;
		case OP_REVERT:
#if TARGET_IPHONE
			fprintf(stderr, "Cowardly refusing to revert snapshot on iOS because it would permanently corrupt the filesystem (something is broken on the devices)\n");
			error=true;
			break;
#endif
			printf("Will revert to snapshot %s on fs %s\n", snapName, fspath);
			if (fs_snapshot_revert(dirfd, snapName, 0) == ERR_SUCCESS) {
				printf("Success\n");
			} else {
				perror("fs_snapshot_revert");
				printf("Failure\n");
				error=true;
			}
			break;
		case OP_LIST:
			printf("Will list snapshots on %s fs\n", fspath);
			const char **snapshots = snapshot_list(dirfd);
			if (snapshots==NULL || *snapshots == NULL) {
				error=true;
			} else {
				for (const char **snapshot = snapshots; *snapshot; snapshot++) {
					printf("%s\n", *snapshot);
				}
			}
			if (snapshots != NULL) {
				free(snapshots);
			}
			break;
		default:
			fprintf(stderr, "Error: please provide an operation (create, delete, rename, mount, revert, list)\n");
			error=true;
			break;
	}
	if (hashsnap != NULL)
		free(hashsnap);
	if (op != OP_SHOWHASH)
		close(dirfd);
	return(error);
}

// vim:ft=objc;
