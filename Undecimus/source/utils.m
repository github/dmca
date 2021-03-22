//
//  utils.m
//  Undecimus
//
//  Created by Sam Bingner on 11/23/18.
//  Copyright © 2018 - 2019 Sam Bingner. All rights reserved.
//

#import <mach/mach.h>
#import <sys/sysctl.h>
#import <Foundation/Foundation.h>
#import <CommonCrypto/CommonDigest.h>
#import <spawn.h>
#include <copyfile.h>
#include <common.h>
#include <libproc.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <MobileGestalt.h>
#import <inject.h>
#include <UIKit/UIKit.h>
#include <SystemConfiguration/SystemConfiguration.h>
#import "ArchiveFile.h"
#import "utils.h"
#import "KernelUtilities.h"
#include <snappy.h>
#include <sys/snapshot.h>

extern char **environ;
int logfd=-1;

bool injectedToTrustCache = false;
NSMutableArray *toInjectToTrustCache = nil;

NSData *lastSystemOutput=nil;
void injectDir(NSString *dir) {
    init_function();
    NSFileManager *fm = [NSFileManager defaultManager];
    NSMutableArray *toInject = [NSMutableArray new];
    for (NSString *filename in [fm contentsOfDirectoryAtPath:dir error:nil]) {
        NSString *file = [dir stringByAppendingPathComponent:filename];
        if (cdhashFor(file) != nil) {
            [toInject addObject:file];
        }
    }
    LOG("Will inject %lu files for %@", (unsigned long)toInject.count, dir);
    if (toInject.count > 0) {
        if (injectedToTrustCache) {
            LOG("Warning: Trust cache already injected");
        }
        for (NSString *path in toInject) {
            if (![toInjectToTrustCache containsObject:path]) {
                [toInjectToTrustCache addObject:path];
            }
        }
    }
}

int sha1_to_str(const unsigned char *hash, size_t hashlen, char *buf, size_t buflen)
{
    init_function();
    if (buflen < (hashlen*2+1)) {
        return -1;
    }
    
    int i;
    for (i=0; i<hashlen; i++) {
        sprintf(buf+i*2, "%02X", hash[i]);
    }
    buf[i*2] = 0;
    return ERR_SUCCESS;
}

NSString *sha1sum(NSString *file)
{
    init_function();
    uint8_t buffer[0x1000];
    unsigned char md[CC_SHA1_DIGEST_LENGTH];

    if (![[NSFileManager defaultManager] fileExistsAtPath:file])
        return nil;
    
    NSInputStream *fileStream = [NSInputStream inputStreamWithFileAtPath:file];
    [fileStream open];

    CC_SHA1_CTX c;
    CC_SHA1_Init(&c);
    while ([fileStream hasBytesAvailable]) {
        NSInteger read = [fileStream read:buffer maxLength:0x1000];
        CC_SHA1_Update(&c, buffer, (CC_LONG)read);
    }
    
    CC_SHA1_Final(md, &c);
    
    char checksum[CC_SHA1_DIGEST_LENGTH * 2 + 1];
    if (sha1_to_str(md, CC_SHA1_DIGEST_LENGTH, checksum, sizeof(checksum)) != ERR_SUCCESS)
        return nil;
    return @(checksum);
}

NSString *md5sum(NSString *file)
{
    init_function();
    uint8_t buffer[0x1000];
    unsigned char md[CC_SHA1_DIGEST_LENGTH];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:file])
        return nil;
    
    NSInputStream *fileStream = [NSInputStream inputStreamWithFileAtPath:file];
    [fileStream open];
    
    CC_MD5_CTX c;
    CC_MD5_Init(&c);
    while ([fileStream hasBytesAvailable]) {
        NSInteger read = [fileStream read:buffer maxLength:0x1000];
        CC_MD5_Update(&c, buffer, (CC_LONG)read);
    }
    
    CC_MD5_Final(md, &c);
    
    char checksum[CC_MD5_DIGEST_LENGTH * 2 + 1];
    if (sha1_to_str(md, CC_MD5_DIGEST_LENGTH, checksum, sizeof(checksum)) != ERR_SUCCESS)
        return nil;
    return @(checksum);
}

bool verifySha1Sums(NSString *sumFile) {
    init_function();
    return verifySums(sumFile, HASHTYPE_SHA1);
}

bool verifySums(NSString *sumFile, enum hashtype hash) {
    init_function();
    if (![[NSFileManager defaultManager] fileExistsAtPath:sumFile])
        return false;
    
    NSString *checksums = [NSString stringWithContentsOfFile:sumFile encoding:NSUTF8StringEncoding error:NULL];
    if (checksums == nil)
        return false;
    
    for (NSString *checksum in [checksums componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]]) {
        // Ignore blank lines
        if ([checksum isEqualToString:@""])
            continue;

        NSArray<NSString*> *suminfo = [checksum componentsSeparatedByString:@"  "];

        if ([suminfo count] != 2) {
            LOG("Invalid line \"%s\"", checksum.UTF8String);
            return false;
        }
        NSString *fileSum;
        switch (hash) {
            case HASHTYPE_SHA1:
                fileSum = sha1sum(suminfo[1]);
                break;
            case HASHTYPE_MD5:
                fileSum = md5sum(suminfo[1]);
                break;
        }
        if (![fileSum.lowercaseString isEqualToString:suminfo[0]]) {
            LOG("Corrupted \"%s\"", [suminfo[1] UTF8String]);
            return false;
        }
        LOG("Verified \"%s\"", [suminfo[1] UTF8String]);
    }
    LOG("No errors in verifying checksums");
    return true;
}

int _system(const char *cmd) {
    init_function();
    const char *argv[] = {"sh", "-c", (char *)cmd, NULL};
    return runCommandv("/bin/sh", 3, argv, NULL, true);
}

int systemf(const char *cmd, ...) {
    init_function();
    va_list ap;
    va_start(ap, cmd);
    NSString *cmdstr = [[NSString alloc] initWithFormat:@(cmd) arguments:ap];
    va_end(ap);
    return system([cmdstr UTF8String]);
}

bool pkgIsInstalled(char *packageID) {
    init_function();
    int rv = systemf("/usr/bin/dpkg -s \"%s\" 2>/dev/null | grep -i ^Status: | grep -q \"install ok\"", packageID);
    bool isInstalled = !WEXITSTATUS(rv);
    LOG("Deb: \"%s\" is%s installed", packageID, isInstalled?"":" not");
    return isInstalled;
}

bool pkgIsConfigured(char *packageID) {
    init_function();
    int rv = systemf("/usr/bin/dpkg -s \"%s\" 2>/dev/null | grep -i ^Status: | grep -q \"install ok installed\"", packageID);
    bool isConfigured = !WEXITSTATUS(rv);
    LOG("Deb: \"%s\" is%s installed", packageID, isConfigured?"":" not");
    return isConfigured;
}

bool pkgIsBy(const char *maintainer, const char *packageID) {
    init_function();
    int rv = systemf("/usr/bin/dpkg -s \"%s\" 2>/dev/null | grep -i ^Maintainer: | grep -qi \"%s\"", packageID, maintainer);
    bool isBy = !WEXITSTATUS(rv);
    LOG("Deb: \"%s\" is%s by %s", packageID, isBy?"":" not", maintainer);
    return isBy;
}

bool compareInstalledVersion(const char *packageID, const char *op, const char *version) {
    init_function();
    int rv = systemf("/usr/bin/dpkg --compare-versions $(dpkg-query --showformat='${Version}' --show \"%s\") \"%s\" \"%s\"",
                      packageID, op, version);
    rv = !WEXITSTATUS(rv);
    LOG("Deb %s is%s %s %s", packageID, rv?"":" not", op, version);
    return rv;
}

bool runDpkg(NSArray <NSString*> *args, bool forceDeps, bool forceAll) {
    init_function();
    if ([args count] < 2) {
        LOG("%s: Nothing to do", __FUNCTION__);
        return false;
    }
    NSMutableArray <NSString*> *command = [NSMutableArray
                arrayWithArray:@[
                        @"/usr/bin/dpkg",
                        @"--force-bad-path",
                        @"--force-configure-any",
                        @"--no-triggers"
                     ]];
    
    if (forceAll) {
        [command addObject:@"--force-all"];
    } else if (forceDeps) {
        [command addObjectsFromArray:@[@"--force-depends", @"--force-remove-essential"]];
    }
    for (NSString *arg in args) {
        [command addObject:arg];
    }
    const char *argv[command.count];
    for (int i=0; i<[command count]; i++) {
        argv[i] = [command[i] UTF8String];
    }
    argv[command.count] = NULL;
    int rv = runCommandv("/usr/bin/dpkg", (int)[command count], argv, NULL, true);
    return !WEXITSTATUS(rv);
}

bool extractDeb(NSString *debPath, bool doInject) {
    init_function();
    if (![debPath hasSuffix:@".deb"]) {
        LOG("%@: not a deb", debPath);
        return NO;
    }
    if ([debPath containsString:@"firmware-sbin"]) {
        // No, just no.
        return YES;
    }
    NSPipe *pipe = [NSPipe pipe];
    if (pipe == nil) {
        LOG("Unable to make a pipe!");
        return NO;
    }
    ArchiveFile *deb = [ArchiveFile archiveWithFile:debPath];
    if (deb == nil) {
        return NO;
    }
    ArchiveFile *tar = [ArchiveFile archiveWithFd:pipe.fileHandleForReading.fileDescriptor];
    if (tar == nil) {
        return NO;
    }
    LOG("Extracting %@", debPath);
    dispatch_queue_t extractionQueue = dispatch_queue_create(NULL, NULL);
    dispatch_async(extractionQueue, ^{
        init_function();
        [deb extractFileNum:3 toFd:pipe.fileHandleForWriting.fileDescriptor];
    });
    bool result = [tar extractToPath:@"/"];
    if (doInject && result) {
        chdir("/");
        NSMutableArray *toInject = [NSMutableArray new];
        NSDictionary *files = tar.files;
        for (NSString *file in files.allKeys) {
            NSString *path = [@"/" stringByAppendingString:[file stringByStandardizingPath]];
            if (cdhashFor(path) != nil) {
                [toInject addObject:path];
            }
        }
        LOG("Will inject %lu files for %@", (unsigned long)toInject.count, debPath);
        if (toInject.count > 0) {
            if (injectedToTrustCache) {
                LOG("Warning: Trust cache already injected");
            }
            for (NSString *path in toInject) {
                if (![toInjectToTrustCache containsObject:path]) {
                    [toInjectToTrustCache addObject:path];
                }
            }
        }
    }
    return result;
}

bool extractDebs(NSArray <NSString *> *debPaths, bool doInject) {
    init_function();
    if ([debPaths count] < 1) {
        LOG("%s: Nothing to install", __FUNCTION__);
        return false;
    }
    for (NSString *debPath in debPaths) {
        if (!extractDeb(debPath, doInject))
            return NO;
    }
    return YES;
}

bool installDeb(const char *debName, bool forceDeps) {
    init_function();
    return runDpkg(@[@"-i", @(debName)], forceDeps, false);
}

bool installDebs(NSArray <NSString*> *debs, bool forceDeps, bool forceAll) {
    init_function();
    if ([debs count] < 1) {
        LOG("%s: Nothing to install", __FUNCTION__);
        return false;
    }
    return runDpkg([@[@"-i"] arrayByAddingObjectsFromArray:debs], forceDeps, forceAll);
}

bool removePkg(char *packageID, bool forceDeps) {
    init_function();
    return runDpkg(@[@"-r", @(packageID)], forceDeps, false);
}

bool removePkgs(NSArray <NSString*> *pkgs, bool forceDeps) {
    init_function();
    if ([pkgs count] < 1) {
        LOG("%s: Nothing to remove", __FUNCTION__);
        return false;
    }
    return runDpkg([@[@"-r"] arrayByAddingObjectsFromArray:pkgs], forceDeps, false);
}

bool runApt(NSArray <NSString*> *args) {
    init_function();
    if ([args count] < 1) {
        LOG("%s: Nothing to do", __FUNCTION__);
        return false;
    }
    NSMutableArray <NSString*> *command = [NSMutableArray arrayWithArray:@[
                        @"/usr/bin/apt-get",
                        @"-o", @"Dir::Etc::sourcelist=undecimus/undecimus.list",
                        @"-o", @"Dir::Etc::sourceparts=-",
                        @"-o", @"APT::Get::List-Cleanup=0"
                        ]];
    [command addObjectsFromArray:args];
    
    const char *argv[command.count];
    for (int i=0; i<[command count]; i++) {
        argv[i] = [command[i] UTF8String];
    }
    argv[command.count] = NULL;
    int rv = runCommandv(argv[0], (int)[command count], argv, NULL, true);
    return WIFEXITED(rv) && !WEXITSTATUS(rv);
}

bool aptUpdate() {
    init_function();
    return runApt(@[@"update"]);
}

bool aptInstall(NSArray <NSString*> *pkgs) {
    init_function();
    return runApt([@[@"-y", @"--allow-unauthenticated", @"--allow-downgrades", @"install"]
                     arrayByAddingObjectsFromArray:pkgs]);
}

bool aptUpgrade() {
    init_function();
    return runApt(@[@"-y", @"--allow-unauthenticated", @"--allow-downgrades", @"-f", @"dist-upgrade"]);
}

bool aptRepair() {
    init_function();
    return runApt(@[@"-o", @"Dir::Etc::preferences=undecimus/preferences", @"-o", @"Dir::Etc::preferencesparts=''", @"-y", @"--allow-unauthenticated", @"--allow-remove-essential", @"--allow-downgrades", @"-f", @"dist-upgrade"]);
}

bool extractAptPkgList(NSString *path, ArchiveFile* listcache, id_t owner)
{
    init_function();
    struct stat buf;
    if (stat(path.UTF8String, &buf) != ERR_SUCCESS || !S_ISDIR(buf.st_mode)) {
        if (!ensure_directory(path.UTF8String, owner, 0755)) return false;
        return [listcache extractToPath:path withOwner:owner andGroup:owner];
    }
    return true;
}

bool ensureAptPkgLists() {
    init_function();
    if (is_directory("/var/lib/apt/lists")) return true;
    
    NSString *lists = pathForResource(@"lists.tar.lzma");
    if (!lists) return false;
    ArchiveFile *listsArchive = [ArchiveFile archiveWithFile:lists];
    if (!listsArchive) return false;
    bool success = extractAptPkgList(@"/var/lib/apt/lists", listsArchive, 0);
    return success && extractAptPkgList(@"/var/mobile/Library/Caches/com.saurik.Cydia/lists", listsArchive, 501);
}

bool removeURLFromSources(NSMutableString *sources, NSString *url)
{
    init_function();
    bool removed=false;
    NSString *pattern = [NSString stringWithFormat:@"[^\\n](?:(?!\\n\\n).)*%@(?:(?!\\n\\n).)*\\n\\n",
                         [url stringByReplacingOccurrencesOfString:@"." withString:@"\\."]
                         ];
    NSRegularExpression *sourceexp = [NSRegularExpression
                                      regularExpressionWithPattern:pattern
                                      options:NSRegularExpressionDotMatchesLineSeparators
                                      error:nil];
    
    for (NSTextCheckingResult *source in [sourceexp matchesInString:sources options:0 range:NSMakeRange(0, sources.length)])
    {
        removed = true;
        [sources deleteCharactersInRange:[source rangeAtIndex:0]];
    }
    return removed;
}

void deduplicateSillySources(void)
{
    init_function();
    NSString *cydia_list = [NSString stringWithContentsOfFile:@"/etc/apt/sources.list.d/cydia.list" encoding:NSUTF8StringEncoding error:nil];
    NSMutableString *sileo_sources = [NSMutableString stringWithContentsOfFile:@"/etc/apt/sources.list.d/sileo.sources" encoding:NSUTF8StringEncoding error:nil];
    if (cydia_list && sileo_sources) {
        NSFileManager *fm = [NSFileManager defaultManager];
        if (pkgIsInstalled("org.coolstar.sileo")) {
            NSString *orig_sileo_sources = [sileo_sources copy];
            NSRegularExpression *urlexp = [NSRegularExpression regularExpressionWithPattern:@"https?://(\\S+[^/\\s]|\\S+)/?\\s" options:0 error:nil];
            
            for (NSTextCheckingResult *match in [urlexp matchesInString:cydia_list options:0 range:NSMakeRange(0, cydia_list.length)])
            {
                NSString *url = [cydia_list substringWithRange:[match rangeAtIndex:1]];
                if ([url hasPrefix:@"apt.thebigboss.org"] && removeURLFromSources(sileo_sources, @"repounclutter.coolstar.org")) {
                    LOG("Removing duplicated source repounclutter from sileo.sources");
                }
                if (removeURLFromSources(sileo_sources, url)) {
                    LOG("Removing duplicated source %@ from sileo.sources", url);
                }
            }
            if (![sileo_sources isEqual:orig_sileo_sources]) {
                [fm createFileAtPath:@"/etc/apt/sources.list.d/sileo.sources"
                            contents:[sileo_sources dataUsingEncoding:NSUTF8StringEncoding]
                          attributes:@{ NSFileOwnerAccountID:@(0), NSFileGroupOwnerAccountID:@(0), NSFilePosixPermissions:@(0644) }
                 ];
            }
        } else {
            [fm removeItemAtPath:@"/etc/apt/sources.list.d/sileo.sources" error:nil];
        }
    }
}

bool is_symlink(const char *filename) {
    init_function();
    struct stat buf;
    if (lstat(filename, &buf) != ERR_SUCCESS) {
        return false;
    }
    return S_ISLNK(buf.st_mode);
}

bool is_directory(const char *filename) {
    init_function();
    struct stat buf;
    if (lstat(filename, &buf) != ERR_SUCCESS) {
        return false;
    }
    return S_ISDIR(buf.st_mode);
}

bool is_mountpoint(const char *filename) {
    init_function();
    struct stat buf;
    if (lstat(filename, &buf) != ERR_SUCCESS) {
        return false;
    }

    if (!S_ISDIR(buf.st_mode))
        return false;
    
    char *cwd = getcwd(NULL, 0);
    int rv = chdir(filename);
    assert(rv == ERR_SUCCESS);
    struct stat p_buf;
    rv = lstat("..", &p_buf);
    assert(rv == ERR_SUCCESS);
    if (cwd) {
        chdir(cwd);
        SafeFreeNULL(cwd);
    }
    return buf.st_dev != p_buf.st_dev || buf.st_ino == p_buf.st_ino;
}

bool ensure_directory(const char *directory, int owner, mode_t mode) {
    init_function();
    NSString *path = @(directory);
    NSFileManager *fm = [NSFileManager defaultManager];
    id attributes = [fm attributesOfItemAtPath:path error:nil];
    if (attributes &&
        [attributes[NSFileType] isEqual:NSFileTypeDirectory] &&
        [attributes[NSFileOwnerAccountID] isEqual:@(owner)] &&
        [attributes[NSFileGroupOwnerAccountID] isEqual:@(owner)] &&
        [attributes[NSFilePosixPermissions] isEqual:@(mode)]
        ) {
        // Directory exists and matches arguments
        return true;
    }
    if (attributes) {
        if ([attributes[NSFileType] isEqual:NSFileTypeDirectory]) {
            // Item exists and is a directory
            return [fm setAttributes:@{
                           NSFileOwnerAccountID: @(owner),
                           NSFileGroupOwnerAccountID: @(owner),
                           NSFilePosixPermissions: @(mode)
                           } ofItemAtPath:path error:nil];
        } else if (![fm removeItemAtPath:path error:nil]) {
            // Item exists and is not a directory but could not be removed
            return false;
        }
    }
    // Item does not exist at this point
    return [fm createDirectoryAtPath:path withIntermediateDirectories:YES attributes:@{
                   NSFileOwnerAccountID: @(owner),
                   NSFileGroupOwnerAccountID: @(owner),
                   NSFilePosixPermissions: @(mode)
               } error:nil];
}

bool ensure_symlink(const char *to, const char *from) {
    init_function();
    ssize_t wantedLength = strlen(to);
    ssize_t maxLen = wantedLength + 1;
    char link[maxLen];
    ssize_t linkLength = readlink(from, link, sizeof(link));
    if (linkLength != wantedLength ||
        strncmp(link, to, maxLen) != ERR_SUCCESS
        ) {
        if (!clean_file(from)) {
            return false;
        }
        if (symlink(to, from) != ERR_SUCCESS) {
            return false;
        }
    }
    return true;
}

bool ensure_file(const char *file, int owner, mode_t mode) {
    init_function();
    NSString *path = @(file);
    NSFileManager *fm = [NSFileManager defaultManager];
    id attributes = [fm attributesOfItemAtPath:path error:nil];
    if (attributes &&
        [attributes[NSFileType] isEqual:NSFileTypeRegular] &&
        [attributes[NSFileOwnerAccountID] isEqual:@(owner)] &&
        [attributes[NSFileGroupOwnerAccountID] isEqual:@(owner)] &&
        [attributes[NSFilePosixPermissions] isEqual:@(mode)]
        ) {
        // File exists and matches arguments
        return true;
    }
    if (attributes) {
        if ([attributes[NSFileType] isEqual:NSFileTypeRegular]) {
            // Item exists and is a file
            return [fm setAttributes:@{
                                       NSFileOwnerAccountID: @(owner),
                                       NSFileGroupOwnerAccountID: @(owner),
                                       NSFilePosixPermissions: @(mode)
                                       } ofItemAtPath:path error:nil];
        } else if (![fm removeItemAtPath:path error:nil]) {
            // Item exists and is not a file but could not be removed
            return false;
        }
    }
    // Item does not exist at this point
    return [fm createFileAtPath:path contents:nil attributes:@{
                               NSFileOwnerAccountID: @(owner),
                               NSFileGroupOwnerAccountID: @(owner),
                               NSFilePosixPermissions: @(mode)
                               }];
}

bool mode_is(const char *filename, mode_t mode) {
    init_function();
    struct stat buf;
    if (lstat(filename, &buf) != ERR_SUCCESS) {
        return false;
    }
    return buf.st_mode == mode;
}

int runCommandv(const char *cmd, int argc, const char * const* argv, void (^unrestrict)(pid_t), bool wait) {
    init_function();
    pid_t pid;
    posix_spawn_file_actions_t *actions = NULL;
    posix_spawn_file_actions_t actionsStruct;
    int out_pipe[2];
    bool valid_pipe = false;
    posix_spawnattr_t *attr = NULL;
    posix_spawnattr_t attrStruct;
    
    NSMutableString *cmdstr = [NSMutableString stringWithCString:cmd encoding:NSUTF8StringEncoding];
    for (int i=1; i<argc; i++) {
        [cmdstr appendFormat:@" \"%s\"", argv[i]];
    }

    valid_pipe = pipe(out_pipe) == ERR_SUCCESS;
    if (valid_pipe && posix_spawn_file_actions_init(&actionsStruct) == ERR_SUCCESS) {
        actions = &actionsStruct;
        posix_spawn_file_actions_adddup2(actions, out_pipe[1], 1);
        posix_spawn_file_actions_adddup2(actions, out_pipe[1], 2);
        posix_spawn_file_actions_addclose(actions, out_pipe[0]);
        posix_spawn_file_actions_addclose(actions, out_pipe[1]);
    }
    
    if (unrestrict && posix_spawnattr_init(&attrStruct) == ERR_SUCCESS) {
        attr = &attrStruct;
        posix_spawnattr_setflags(attr, POSIX_SPAWN_START_SUSPENDED);
    }
    
    char *dt_mode = getenv("OS_ACTIVITY_DT_MODE");
    if (dt_mode) {
        dt_mode = strdup(dt_mode); // I didn't check for failure because that will just permanently unset DT_MODE
        unsetenv("OS_ACTIVITY_DT_MODE"); // This causes all NSLog entries go to STDERR and breaks firmware.sh
    }
	

    int rv = posix_spawn(&pid, cmd, actions, attr, (char *const *)argv, environ);

    if (dt_mode) {
        setenv("OS_ACTIVITY_DT_MODE", dt_mode, 1);
        free(dt_mode);
    }

    LOG("%s(%d) command: %@", __FUNCTION__, pid, cmdstr);
    
    if (unrestrict) {
        unrestrict(pid);
        kill(pid, SIGCONT);
    }
    
    if (valid_pipe) {
        close(out_pipe[1]);
    }
    
    if (rv != ERR_SUCCESS) {
        LOG("%s(%d): ERROR posix_spawn failed (%d): %s", __FUNCTION__, pid, rv, strerror(rv));
        rv <<= 8; // Put error into WEXITSTATUS
    } else if (wait) {
        if (valid_pipe) {
            NSMutableData *outData = [NSMutableData new];
            char c;
            char s[2] = {0, 0};
            NSMutableString *line = [NSMutableString new];
            while (read(out_pipe[0], &c, 1) == 1) {
                [outData appendBytes:&c length:1];
                if (c == '\n') {
                    LOG("%s(%d): %@", __FUNCTION__, pid, line);
                    [line setString:@""];
                } else {
                    s[0] = c;
					NSString *str = @(s);
					if (str == nil)
						continue;
                    [line appendString:str];
                }
            }
            if ([line length] > 0) {
                LOG("%s(%d): %@", __FUNCTION__, pid, line);
            }
            lastSystemOutput = [outData copy];
        }
        if (waitpid(pid, &rv, 0) == -1) {
            LOG("ERROR: Waitpid failed");
        } else {
            LOG("%s(%d) completed with exit status %d", __FUNCTION__, pid, WEXITSTATUS(rv));
        }
    }
    if (valid_pipe) {
        close(out_pipe[0]);
    }
    return rv;
}

int runCommand(const char *cmd, ...) {
    init_function();
    va_list ap, ap2;
    int argc = 1;

    va_start(ap, cmd);
    va_copy(ap2, ap);

    while (va_arg(ap, const char *) != NULL) {
        argc++;
    }
    va_end(ap);
    
    const char *argv[argc+1];
    argv[0] = cmd;
    for (int i=1; i<argc; i++) {
        argv[i] = va_arg(ap2, const char *);
    }
    va_end(ap2);
    argv[argc] = NULL;

    void (^unrestrict)(pid_t pid) = NULL;
    unrestrict = ^(pid_t pid) {
        kptr_t proc = get_proc_struct_for_pid(pid);
        set_platform_binary(proc, true);
        set_cs_platform_binary(proc, true);
    };
    int rv = runCommandv(cmd, argc, argv, unrestrict, true);
    return WEXITSTATUS(rv);
}

NSString *pathForResource(NSString *resource) {
    init_function();
    static NSString *sourcePath;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        init_function();
        sourcePath = [[NSBundle mainBundle] bundlePath];
    });
    
    NSString *path = [[sourcePath stringByAppendingPathComponent:resource] stringByStandardizingPath];
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) {
        return nil;
    }
    return path;
}

pid_t pidOfProcess(const char *name) {
    init_function();
    char real[PROC_PIDPATHINFO_MAXSIZE];
    bzero(real, sizeof(real));
    realpath(name, real);
    int numberOfProcesses = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
    pid_t pids[numberOfProcesses];
    bzero(pids, sizeof(pids));
    proc_listpids(PROC_ALL_PIDS, 0, pids, (int)sizeof(pids));
    bool foundProcess = false;
    pid_t processPid = 0;
    for (int i = 0; i < numberOfProcesses && !foundProcess; ++i) {
        if (pids[i] == 0) {
            continue;
        }
        char *path = get_path_for_pid(pids[i]);
        if (path != NULL) {
            if (strlen(path) > 0 && strcmp(path, real) == 0) {
                processPid = pids[i];
                foundProcess = true;
            }
            SafeFreeNULL(path);
        }
    }
    return processPid;
}

char *getKernelVersion() {
    init_function();
    return sysctlWithName("kern.version");
}

char *getMachineName() {
    init_function();
    return sysctlWithName("hw.machine");
}
char *getModelName() {
    init_function();
    return sysctlWithName("hw.model");
}

bool kernelVersionContains(const char *string) {
    init_function();
    char *kernelVersion = getKernelVersion();
    if (kernelVersion == NULL) return false;
    bool ret = strstr(kernelVersion, string) != NULL;
    SafeFreeNULL(kernelVersion);
    return ret;
}

bool machineNameContains(const char *string) {
    init_function();
    char *machineName = getMachineName();
    if (machineName == NULL) return false;
    bool ret = strstr(machineName, string) != NULL;
    SafeFreeNULL(machineName);
    return ret;
}

#define AF_MULTIPATH 39

bool multi_path_tcp_enabled() {
    init_function();
    static bool enabled = false;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        init_function();
        int sock = socket(AF_MULTIPATH, SOCK_STREAM, 0);
        if (sock < 0) {
            return;
        }
        struct sockaddr* sockaddr_src = malloc(sizeof(struct sockaddr));
        memset(sockaddr_src, 'A', sizeof(struct sockaddr));
        sockaddr_src->sa_len = sizeof(struct sockaddr);
        sockaddr_src->sa_family = AF_INET6;
        struct sockaddr* sockaddr_dst = malloc(sizeof(struct sockaddr));
        memset(sockaddr_dst, 'A', sizeof(struct sockaddr));
        sockaddr_dst->sa_len = sizeof(struct sockaddr);
        sockaddr_dst->sa_family = AF_INET;
        sa_endpoints_t eps = {0};
        eps.sae_srcif = 0;
        eps.sae_srcaddr = sockaddr_src;
        eps.sae_srcaddrlen = sizeof(struct sockaddr);
        eps.sae_dstaddr = sockaddr_dst;
        eps.sae_dstaddrlen = sizeof(struct sockaddr);
        connectx(sock, &eps, SAE_ASSOCID_ANY, 0, NULL, 0, NULL, NULL);
        enabled = (errno != EPERM);
        SafeFreeNULL(sockaddr_src);
        SafeFreeNULL(sockaddr_dst);
        close(sock);
    });
    return enabled;
}

bool jailbreakEnabled() {
    init_function();
    return kernelVersionContains(DEFAULT_VERSION_STRING) ||
    access(SLIDE_FILE, F_OK) == ERR_SUCCESS;
}

NSString *getKernelBuildVersion() {
    init_function();
    NSString *cleanString = nil;
    char *kernelVersion = NULL;
    kernelVersion = getKernelVersion();
    if (kernelVersion == NULL) return nil;
    cleanString = @(kernelVersion);
    SafeFreeNULL(kernelVersion);
    cleanString = [cleanString componentsSeparatedByString:@"; "][1];
    cleanString = [cleanString componentsSeparatedByString:@"-"][1];
    cleanString = [cleanString componentsSeparatedByString:@"/"][0];
    return cleanString;
}

bool daemonIsLoaded(char *daemonID) {
    init_function();
    int rv = systemf("/bin/launchctl list | grep %s", daemonID);
    bool isLoaded = !WEXITSTATUS(rv);
    LOG("Daemon: \"%s\" is%s loaded", daemonID, isLoaded?"":" not");
    return isLoaded;
}

NSString *bundledResourcesVersion() {
    init_function();
    NSBundle *bundle = [NSBundle mainBundle];
    return [bundle objectForInfoDictionaryKey:@"BundledResources"];
}

NSString *appVersion() {
    init_function();
    NSBundle *bundle = [NSBundle mainBundle];
    return [bundle objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
}

bool debuggerEnabled() {
    init_function();
    return (getppid() != 1);
}

NSString *getLogFile() {
    init_function();
    static NSString *logfile;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        init_function();
        logfile = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents/log_file.txt"];
    });
    return logfile;
}

void enableLogging() {
    init_function();
    if (!debuggerEnabled()) {
        int old_logfd = logfd;
        int newfd = open(getLogFile().UTF8String, O_WRONLY|O_CREAT|O_APPEND, 0644);
        if (newfd < 0) {
            LOG("Error opening logfile: %s", strerror(errno));
        }
        logfd = newfd;
        if (old_logfd > 0)
            close(old_logfd);
    }
}

void disableLogging() {
    init_function();
    if (!debuggerEnabled()) {
        int old_logfd = logfd;
        logfd = -1;
        if (old_logfd > 0)
            close(old_logfd);
    }
}

void cleanLogs() {
    init_function();
    const char *logFile = getLogFile().UTF8String;
    clean_file(logFile);
    enableLogging();
}

bool modifyPlist(NSString *filename, void (^function)(id)) {
    init_function();
	assert(function != NULL);
    LOG("%s: Will modify plist: %@", __FUNCTION__, filename);
    NSData *data = [NSData dataWithContentsOfFile:filename];
    if (data == nil) {
        LOG("%s: Failed to read file: %@", __FUNCTION__, filename);
        return false;
    }
    NSPropertyListFormat format = 0;
    NSError *error = nil;
    id plist = [NSPropertyListSerialization propertyListWithData:data options:NSPropertyListMutableContainersAndLeaves format:&format error:&error];
    if (plist == nil) {
        LOG("%s: Failed to generate plist data: %@", __FUNCTION__, error);
        return false;
    }
	id new_plist = [plist mutableCopy];
	function(new_plist);
	if ([new_plist isEqualToDictionary:plist]) {
		LOG("%s: Already modified", __FUNCTION__);
		return true;
	}
    NSData *newData = [NSPropertyListSerialization dataWithPropertyList:new_plist format:format options:0 error:&error];
    if (newData == nil) {
        LOG("%s: Failed to generate new plist data: %@", __FUNCTION__, error);
        return false;
    }
	LOG("%s: Writing to file: %@", __FUNCTION__, filename);
	pid_t cfprefsdPid = pidOfProcess("/usr/sbin/cfprefsd");
	if (cfprefsdPid != 0) {
		kill(cfprefsdPid, SIGSTOP);
	}
	BOOL result = [newData writeToFile:filename atomically:YES];
	if (cfprefsdPid != 0) {
		kill(cfprefsdPid, SIGKILL);
	}
	if (!result) {
		LOG("%s: Failed to write to file: %@", __FUNCTION__, filename);
		return false;
	}
    LOG("%s: Success", __FUNCTION__);
    return true;
}

void list(NSString *directory) {
    init_function();
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSArray *listArray = [fileManager contentsOfDirectoryAtPath:directory error:nil];
    LOG("%s(%@): %@", __FUNCTION__, directory, listArray);
}

bool canRead(const char *file) {
    init_function();
    NSString *path = @(file);
    NSFileManager *fileManager = [NSFileManager defaultManager];
    return ([fileManager attributesOfItemAtPath:path error:nil]);
}

bool restartSpringBoard() {
    init_function();
    pid_t backboardd_pid = pidOfProcess("/usr/libexec/backboardd");
    if (!(backboardd_pid > 1)) {
        LOG("Unable to find backboardd pid.");
        return false;
    }
    if (kill(backboardd_pid, SIGTERM) != ERR_SUCCESS) {
        LOG("Unable to terminate backboardd.");
        return false;
    }
    return true;
}

bool uninstallRootLessJB() {
    init_function();
    BOOL foundRootLessJB = NO;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *rootLessJBBootstrapMarkerFile = @"/var/containers/Bundle/.installed_rootlessJB3";
    NSArray *rootLessJBFileList = @[@"/var/LIB", @"/var/ulb", @"/var/bin", @"/var/sbin", @"/var/libexec", @"/var/containers/Bundle/tweaksupport/Applications", @"/var/Apps", @"/var/profile", @"/var/motd", @"/var/dropbear", @"/var/containers/Bundle/tweaksupport", @"/var/containers/Bundle/iosbinpack64", @"/var/log/testbin.log", @"/var/log/jailbreakd-stdout.log", @"/var/log/jailbreakd-stderr.log", @"/var/log/pspawn_payload_xpcproxy.log", @"/var/lib", @"/var/etc", @"/var/usr", rootLessJBBootstrapMarkerFile];
    if ([fileManager fileExistsAtPath:rootLessJBBootstrapMarkerFile]) {
        LOG("Found RootLessJB.");
        foundRootLessJB = YES;
    }
    if (foundRootLessJB) {
        LOG("Uninstalling RootLessJB...");
        for (NSString *file in rootLessJBFileList) {
            if ([fileManager fileExistsAtPath:file] && ![fileManager removeItemAtPath:file error:nil]) {
                LOG("Unable to remove file: %@", file);
                return false;
            }
        }
    }
    return true;
}

bool verifyECID(NSString *ecid) {
    init_function();
    CFStringRef value = MGCopyAnswer(kMGUniqueChipID);
    if (value == nil) {
        LOG("Unable to get ECID.");
        return false;
    }
    if (![ecid isEqualToString:CFBridgingRelease(value)]) {
        LOG("Unable to verify ECID.");
        return false;
    }
    return true;
}

bool canOpen(const char *URL) {
    init_function();
    __block bool canOpenURL = false;
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    dispatch_block_t block = ^{
        init_function();
        if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:@(URL)]]) {
            canOpenURL = true;
        }
        dispatch_semaphore_signal(semaphore);
    };
    if ([[NSThread currentThread] isMainThread]) {
        block();
    } else {
        dispatch_async(dispatch_get_main_queue(), block);
    }
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    return canOpenURL;
}

bool airplaneModeEnabled() {
    init_function();
    struct sockaddr_in zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin_len = sizeof(zeroAddress);
    zeroAddress.sin_family = AF_INET;
    SCNetworkReachabilityRef reachability = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (const struct sockaddr *)&zeroAddress);
    if (reachability == NULL)
        return false;
    SCNetworkReachabilityFlags flags;
    if (!SCNetworkReachabilityGetFlags(reachability, &flags)) {
        return false;
    }
    if (flags == 0) {
        return true;
    } else {
        return false;
    }
}

bool installApp(const char *bundle) {
    init_function();
    NSString *bundle_path = @(bundle);
    NSURL *URL = [NSURL URLWithString:bundle_path];
    NSString *info_plist_path = [bundle_path stringByAppendingPathComponent:@"Info.plist"];
    NSMutableDictionary *info_plist = [NSMutableDictionary dictionaryWithContentsOfFile:info_plist_path];
    NSString *bundle_identifier = info_plist[@"CFBundleIdentifier"];
    NSMutableDictionary *options = [NSMutableDictionary new];
    options[@"CFBundleIdentifier"] = bundle_identifier;
    LSApplicationWorkspace *applicationWorkspace = [LSApplicationWorkspace defaultWorkspace];
    if ([applicationWorkspace installApplication:URL withOptions:options]) {
        return true;
    } else {
        LOG("Failed to install application");
        return false;
    }
}

bool rebuildApplicationDatabases() {
    init_function();
    LSApplicationWorkspace *applicationWorkspace = [LSApplicationWorkspace defaultWorkspace];
    if ([applicationWorkspace _LSPrivateRebuildApplicationDatabasesForSystemApps:YES internal:YES user:NO]) {
        return true;
    } else {
        LOG("Failed to rebuild application databases");
        return false;
    }
}

char *get_path_for_pid(pid_t pid) {
    init_function();
    char *ret = NULL;
    uint32_t path_size = PROC_PIDPATHINFO_MAXSIZE;
    char *path = malloc(path_size);
    if (path != NULL) {
        if (proc_pidpath(pid, path, path_size) >= 0) {
            ret = strdup(path);
        }
        SafeFreeNULL(path);
    }
    return ret;
}

NSString *getECID() {
    init_function();
    NSString *ECID = nil;
    CFStringRef value = MGCopyAnswer(kMGUniqueChipID);
    if (value != nil) {
        ECID = [NSString stringWithFormat:@"%@", value];
        CFRelease(value);
    }
    return ECID;
}

NSString *getUDID() {
    init_function();
    NSString *UDID = nil;
    CFStringRef value = MGCopyAnswer(kMGUniqueDeviceID);
    if (value != nil) {
        UDID = [NSString stringWithFormat:@"%@", value];
        CFRelease(value);
    }
    return UDID;
}

char *sysctlWithName(const char *name) {
    init_function();
    kern_return_t kr = KERN_FAILURE;
    char *ret = NULL;
    size_t *size = NULL;
    size = (size_t *)malloc(sizeof(size_t));
    if (size == NULL) goto out;
    bzero(size, sizeof(size_t));
    if (sysctlbyname(name, NULL, size, NULL, 0) != ERR_SUCCESS) goto out;
    ret = (char *)malloc(*size);
    if (ret == NULL) goto out;
    bzero(ret, *size);
    if (sysctlbyname(name, ret, size, NULL, 0) != ERR_SUCCESS) goto out;
    kr = KERN_SUCCESS;
out:
    if (kr == KERN_FAILURE) SafeFreeNULL(ret);
    SafeFreeNULL(size);
    return ret;
}

char *getOSVersion() {
    init_function();
    return sysctlWithName("kern.osversion");
}

char *getOSProductVersion() {
    init_function();
    return sysctlWithName("kern.osproductversion");
}

void printOSDetails() {
    init_function();
    char *machineName = NULL;
    char *modelName = NULL;
    char *kernelVersion = NULL;
    char *OSProductVersion = NULL;
    char *OSVersion = NULL;
    machineName = getMachineName();
    if (machineName == NULL) goto out;
    modelName = getModelName();
    if (modelName == NULL) goto out;
    kernelVersion = getKernelVersion();
    if (kernelVersion == NULL) goto out;
    OSProductVersion = getOSProductVersion();
    if (OSProductVersion == NULL) goto out;
    OSVersion = getOSVersion();
    if (OSVersion == NULL) goto out;
    LOG("Machine Name: %s", machineName);
    LOG("Model Name: %s", modelName);
    LOG("Kernel Version: %s", kernelVersion);
    LOG("Kernel Page Size: 0x%lx", get_kernel_page_size());
    LOG("System Version: iOS %s (%s) (Build: %s)", OSProductVersion, isBetaFirmware() ? "Beta" : "Stable", OSVersion);
out:
    SafeFreeNULL(machineName);
    SafeFreeNULL(modelName);
    SafeFreeNULL(kernelVersion);
    SafeFreeNULL(OSProductVersion);
    SafeFreeNULL(OSVersion);
}

bool isBetaFirmware() {
    init_function();
    bool ret = false;
    char *OSVersion = getOSVersion();
    if (OSVersion == NULL) return false;
    if (strlen(OSVersion) > 6) ret = true;
    SafeFreeNULL(OSVersion);
    return ret;
}

double getUptime() {
    init_function();
    double uptime = 0;
    size_t *size = NULL;
    struct timeval *boottime = NULL;
    size = (size_t *)malloc(sizeof(size_t));
    if (size == NULL) goto out;
    bzero(size, sizeof(size_t));
    *size = sizeof(struct timeval);
    boottime = (struct timeval *)malloc(*size);
    if (boottime == NULL) goto out;
    bzero(boottime, *size);
    int mib[2] = { CTL_KERN, KERN_BOOTTIME };
    if (sysctl(mib, 2, boottime, size, NULL, 0) != ERR_SUCCESS) goto out;
    time_t bsec = boottime->tv_sec, csec = time(NULL);
    uptime = difftime(csec, bsec);
out:
    SafeFreeNULL(size);
    SafeFreeNULL(boottime);
    return uptime;
}

vm_size_t get_kernel_page_size() {
    init_function();
    vm_size_t kernel_page_size = 0;
    vm_size_t *out_page_size = NULL;
    host_t host = mach_host_self();
    if (!MACH_PORT_VALID(host)) goto out;
    out_page_size = (vm_size_t *)malloc(sizeof(vm_size_t));
    if (out_page_size == NULL) goto out;
    bzero(out_page_size, sizeof(vm_size_t));
    if (_host_page_size(host, out_page_size) != KERN_SUCCESS) goto out;
    kernel_page_size = *out_page_size;
out:
    if (MACH_PORT_VALID(host)) mach_port_deallocate(mach_task_self(), host); host = HOST_NULL;
    SafeFreeNULL(out_page_size);
    return kernel_page_size;
}

cpu_subtype_t get_cpu_subtype() {
    init_function();
    cpu_subtype_t ret = 0;
    cpu_subtype_t *cpu_subtype = NULL;
    size_t *cpu_subtype_size = NULL;
    cpu_subtype = (cpu_subtype_t *)malloc(sizeof(cpu_subtype_t));
    if (cpu_subtype == NULL) goto out;
    bzero(cpu_subtype, sizeof(cpu_subtype_t));
    cpu_subtype_size = (size_t *)malloc(sizeof(size_t));
    if (cpu_subtype_size == NULL) goto out;
    bzero(cpu_subtype_size, sizeof(size_t));
    *cpu_subtype_size = sizeof(cpu_subtype_size);
    if (sysctlbyname("hw.cpusubtype", cpu_subtype, cpu_subtype_size, NULL, 0) != 0) goto out;
    ret = *cpu_subtype;
out:;
    SafeFreeNULL(cpu_subtype);
    SafeFreeNULL(cpu_subtype_size);
    return ret;
}

int waitForFile(const char *filename) {
    init_function();
    int rv = access(filename, F_OK);
    for (int i = 0; !(i >= 100 || rv == ERR_SUCCESS); i++) {
        usleep(100000);
        rv = access(filename, F_OK);
    }
    return rv;
}

NSString *hexFromInt(NSInteger val) {
    init_function();
    return [NSString stringWithFormat:@"0x%lX", (long)val];
}

void waitFor(int seconds) {
    init_function();
    for (int i = 1; i <= seconds; i++) {
        LOG("Waiting (%d/%d)", i, seconds);
        sleep(1);
    }
}

bool blockDomainWithName(const char *name) {
    init_function();
    if (!unblockDomainWithName(name)) {
        LOG("%s: Unable to clean hosts file", __FUNCTION__);
        return false;
    }
    NSString *domain = @(name);
    NSString *hosts_file = @"/etc/hosts";
    NSString *hosts = [NSString stringWithContentsOfFile:hosts_file encoding:NSUTF8StringEncoding error:nil];
    if (hosts == nil) {
        LOG("%s: Unable to read hosts file", __FUNCTION__);
        return false;
    }
    NSArray *redirects = @[@"127.0.0.1", @"n::1"];
    for (NSString *redirect in redirects) {
        NSString *line = [NSString stringWithFormat:@"\n%@\t%@\n", redirect, domain];
        hosts = [hosts stringByAppendingString:line];
    }
    if (![hosts writeToFile:hosts_file atomically:YES encoding:NSUTF8StringEncoding error:nil]) {
        LOG("%s: Unable to update hosts file", __FUNCTION__);
        return false;
    }
    return true;
}

bool unblockDomainWithName(const char *name) {
    init_function();
    NSString *domain = @(name);
    NSString *hosts_file = @"/etc/hosts";
    NSString *hosts = [NSString stringWithContentsOfFile:hosts_file encoding:NSUTF8StringEncoding error:nil];
    if (hosts == nil) {
        LOG("%s: Unable to read hosts file", __FUNCTION__);
        return false;
    }
    for (NSString *line in [hosts componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]]) {
        for (NSString *string in [line componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]]) {
            if ([string isEqualToString:domain]) {
                hosts = [hosts stringByReplacingOccurrencesOfString:line withString:@""];
            }
        }
    }
    if (![hosts writeToFile:hosts_file atomically:YES encoding:NSUTF8StringEncoding error:nil]) {
        LOG("%s: Unable to update hosts file", __FUNCTION__);
        return false;
    }
    return true;
}

bool cydiaIsInstalled() {
    init_function();
    if (access("/Applications/Cydia.app", F_OK) != ERR_SUCCESS) {
        return false;
    }
    if (!canOpen("cydia://")) {
        return false;
    }
    return true;
}

NSString *localize(NSString *str, ...) {
    init_function();
    va_list ap;
    va_start(ap, str);
    NSString *localized_str = [[NSString alloc] initWithFormat:NSLocalizedString(str, @"") arguments:ap];
    va_end(ap);
    return localized_str;
}

bool resetSystemCodeSignatures(void) {
    bool rv = false;
    char *systemSnapshot = NULL;
    const char **snapshots = NULL;
    int rootfd = 0;
    bool unsetSystemSnapshot = true;
    int numberOfProcesses = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
    pid_t pids[numberOfProcesses];
    bzero(pids, sizeof(pids));
    int ret = proc_listpids(PROC_ALL_PIDS, 0, pids, (int)sizeof(pids));
    if (ret == -1) goto out;
    systemSnapshot = copySystemSnapshot();
    if (systemSnapshot == NULL) goto out;
    rootfd = open("/", O_RDONLY);
    if (rootfd == -1) goto out;
    snapshots = snapshot_list(rootfd);
    if (snapshots == NULL) goto out;
    const char *snapshot = snapshots[0];
    if (snapshot == NULL) goto out;
    ret = fs_snapshot_rename(rootfd, snapshot, systemSnapshot, 0);
    if (ret == -1) goto out;
    for (int i = 0; i < numberOfProcesses && pids[i] > 1; i++) {
        char path[PROC_PIDPATHINFO_MAXSIZE];
        bzero(path, sizeof(path));
        ret = proc_pidpath(pids[i], path, sizeof(path));
        if (ret == -1) continue;
        if (strncmp(path, "/Developer", sizeof("Developer") - 1) == 0) continue;
        NSString *renameIn = @(path);
        NSString *renameOut = [renameIn stringByAppendingString:@".jailbreak-tmp"];
        NSFileManager *fileManager = [NSFileManager defaultManager];
        if (![fileManager fileExistsAtPath:renameIn]) continue;
        if ([fileManager fileExistsAtPath:renameOut]) continue;
        unsetSystemSnapshot = false;
        if (![fileManager moveItemAtPath:renameIn toPath:renameOut error:nil]) goto out;
        if (![fileManager moveItemAtPath:renameOut toPath:renameIn error:nil]) goto out;
        if (![fileManager fileExistsAtPath:renameIn]) goto out;
        if ([fileManager fileExistsAtPath:renameOut]) goto out;
        unsetSystemSnapshot = true;
    }
    if (!unsetSystemSnapshot) goto out;
    ret = fs_snapshot_rename(rootfd, systemSnapshot, snapshot, 0);
    if (ret == -1) goto out;
    rv = true;
out:;
    SafeFreeNULL(systemSnapshot);
    if (rootfd != -1) close(rootfd);
    SafeFreeNULL(snapshots);
    return rv;
}

__attribute__((constructor))
static void ctor() {
    init_function();
    toInjectToTrustCache = [NSMutableArray new];
}
