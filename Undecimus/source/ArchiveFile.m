//
//  Archive.m
//
//  Created by Sam Bingner on 1/4/19.
//  Copyright © 2019 Sam Bingner. All rights reserved.
//

#import "ArchiveFile.h"
#import <archive.h>
#import <archive_entry.h>
#import <inject.h> // Test static trust cache
#include <common.h>

#define DEFAULT_FLAGS (ARCHIVE_EXTRACT_TIME|ARCHIVE_EXTRACT_PERM|ARCHIVE_EXTRACT_ACL| \
                       ARCHIVE_EXTRACT_FFLAGS|ARCHIVE_EXTRACT_OWNER|ARCHIVE_EXTRACT_UNLINK)

static int
copy_data(struct archive *ar, struct archive *aw)
{
    init_function();
    int r;
    const void *buff;
    size_t size;
    off_t offset;
    
    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r < ARCHIVE_OK)
            return (r);
        if (archive_write_data_block(aw, buff, size, offset) < ARCHIVE_OK) {
            LOG("Archive: %s", archive_error_string(aw));
            return (r);
        }
    }
}

@implementation ArchiveFile {
    NSMutableDictionary *_files;
    int _fd;
    BOOL _hasReadFiles;
    BOOL _isPipe;
}

+(ArchiveFile*)archiveWithFile:(NSString *)filename
{
    init_function();
#if __has_feature(objc_arc)
    return [[ArchiveFile alloc] initWithFile:filename];
#else
    return [[[ArchiveFile alloc] initWithFile:filename] autorelease];
#endif
}

+(ArchiveFile*)archiveWithFd:(int)fd
{
    init_function();
#if __has_feature(objc_arc)
    return [[ArchiveFile alloc] initWithFd:fd];
#else
    return [[[ArchiveFile alloc] initWithFd:fd] autorelease];
#endif
}
-(void)addEntry:(struct archive_entry *)entry
{
    init_function();
    NSString *path = @(archive_entry_pathname(entry));
    _files[path] = [NSMutableDictionary new];
    _files[path][@"mode"] = @(archive_entry_mode(entry));
    _files[path][@"uid"] = @(archive_entry_uid(entry));
    _files[path][@"gid"] = @(archive_entry_gid(entry));
    time_t mtime = archive_entry_mtime(entry);
    if (mtime) {
        _files[path][@"mtime"] = [NSDate dateWithTimeIntervalSince1970:mtime];
    }
}
-(void)readContents
{
    init_function();
    struct archive *a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    if (archive_read_open_fd(a, _fd, 16384) != ARCHIVE_OK)
        return;
    
    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        [self addEntry:entry];
    }
    _hasReadFiles = YES;
    archive_read_close(a);
    archive_read_finish(a);
    lseek(_fd, 0, SEEK_SET);
}

-(ArchiveFile*)init
{
    init_function();
    self = [super init];
    _files = [NSMutableDictionary new];
    _hasReadFiles = NO;
    return self;
}

-(ArchiveFile*)initWithFile:(NSString*)filename
{
    init_function();
    if (![[NSFileManager defaultManager] fileExistsAtPath:filename]) {
        LOG("Archive: File \"%@\" does not exist", filename);
        return nil;
    }
    self = [self init];

    _fd = open(filename.UTF8String, O_RDONLY);
    if (_fd < 0) {
        perror("Archive open file returned error");
        return nil;
    }
    
    struct archive *a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    if (archive_read_open_fd(a, _fd, 16384) != ARCHIVE_OK)
        return nil;

    archive_read_close(a);
    archive_read_finish(a);
    lseek(_fd, 0, SEEK_SET);

    return self;
}

-(ArchiveFile*)initWithFd:(int)fd
{
    init_function();
    self = [self init];
    _isPipe = YES;
    
    _fd = fd;
    if (_fd < 0) {
        perror("Dup fd");
        return nil;
    }
    
    return self;
}

-(NSDictionary*)files {
    init_function();
    if (!_hasReadFiles) {
        [self readContents];
    }
    return [_files copy];
}

-(BOOL)extractFileNum:(int)fileNum toFd:(int)fd
{
    init_function();
    BOOL result = NO;
    /* Select which attributes we want to restore. */
    
    if (fd < 0) {
        LOG("Archive: invalid fd");
        return NO;
    }
    
    if (fileNum < 1) {
        LOG("Archive: invalid fileNum");
        return NO;
    }
    
    struct archive *a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    
    if (archive_read_open_fd(a, _fd, 16384) != ARCHIVE_OK) {
        LOG("Archive: unable to archive_read_open_fd: %s", archive_error_string(a));
        close(fd);
        return result;
    }
    
    // Seek to entry
    struct archive_entry *entry = NULL;
    int rv ;
    for (int i=1; (rv = archive_read_next_header(a, &entry)) == ARCHIVE_OK && i<fileNum; i++);
    
    if (rv == ARCHIVE_EOF) {
        LOG("Archive: no file %d", fileNum);
        goto out;
    }
    
    if (rv < ARCHIVE_OK) {
        LOG("Archive: %s", archive_error_string(a));
        if (rv < ARCHIVE_WARN)
            goto out;
    }
        
    if (archive_entry_size(entry) > 0) {
        rv = archive_read_data_into_fd(a, fd);
    }
    if (rv < ARCHIVE_OK) {
        LOG("Archive: %s", archive_error_string(a));
        if (rv < ARCHIVE_WARN)
            goto out;
    }
    result = YES;
    out:
    archive_read_close(a);
    archive_read_finish(a);
    close(fd);
    return result;
}

-(BOOL)extract:(NSString*)file toFd:(int)fd
{
    init_function();
    BOOL result = NO;
    /* Select which attributes we want to restore. */
    
    if (fd < 0) {
        LOG("Archive: invalid fd");
        return NO;
    }
    
    struct archive *a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    
    if (archive_read_open_fd(a, _fd, 16384) != ARCHIVE_OK) {
        LOG("Archive: unable to archive_read_open_fd: %s", archive_error_string(a));
        close(fd);
        return result;
    }
    
    // Seek to entry
    struct archive_entry *entry = NULL;
    int rv;
    while ((rv = archive_read_next_header(a, &entry)) == ARCHIVE_OK &&
           strcmp(archive_entry_pathname(entry), file.UTF8String) != 0
           );
    
    if (rv == ARCHIVE_EOF) {
        LOG("Archive: no such file \"%@\"", file);
        goto out;
    }
    
    if (rv < ARCHIVE_OK) {
        LOG("Archive: %s", archive_error_string(a));
        if (rv < ARCHIVE_WARN)
            goto out;
    }
    
    if (entry && (strcmp(archive_entry_pathname(entry), file.UTF8String) != 0) ) {
        LOG("Archive: Unable to find entry for %@", file);
        goto out;
    }

    if (archive_entry_size(entry) > 0) {
        rv = archive_read_data_into_fd(a, fd);
    }
    if (rv < ARCHIVE_OK) {
        LOG("Archive: %s", archive_error_string(a));
        if (rv < ARCHIVE_WARN)
            goto out;
    }
    result = YES;
    out:
    archive_read_close(a);
    archive_read_finish(a);
    close(fd);
    return result;
}

-(BOOL)extract:(NSString*)file toPath:(NSString*)path
{
    init_function();
    BOOL result = NO;
    /* Select which attributes we want to restore. */
    int flags = DEFAULT_FLAGS;
    
    int fd = dup(_fd);
    if (fd == -1) {
        LOG("Archive: unable to dupe fd");
        return NO;
    }

    struct archive *a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    
    if (archive_read_open_fd(a, _fd, 16384) != ARCHIVE_OK) {
        LOG("Archive: unable to archive_read_open_fd: %s", archive_error_string(a));
        close(fd);
        return result;
    }

    struct archive *ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    
    // Seek to entry
    struct archive_entry *entry = NULL;
    int rv;
    while ((rv = archive_read_next_header(a, &entry)) == ARCHIVE_OK &&
           strcmp(archive_entry_pathname(entry), file.UTF8String) != 0
           );

    if (rv == ARCHIVE_EOF) {
        LOG("Archive: no such file \"%@\"", file);
        goto out;
    }

    if (rv < ARCHIVE_OK) {
        LOG("Archive: %s", archive_error_string(a));
        if (rv < ARCHIVE_WARN)
            goto out;
    }
    
    if (entry && (strcmp(archive_entry_pathname(entry), file.UTF8String) != 0) ) {
        LOG("Archive: Unable to find entry for %@", file);
        goto out;
    }
    
    archive_entry_set_pathname(entry, path.UTF8String);
    rv = archive_write_header(ext, entry);
    if (rv < ARCHIVE_OK) {
        LOG("Archive: Unable to write header for %@: %s", path, archive_error_string(ext));
        if (rv < ARCHIVE_WARN)
            goto out;
    }
    if (archive_entry_size(entry) > 0) {
        rv = copy_data(a, ext);
        if (rv < ARCHIVE_OK) {
            LOG("Archive: Error copying data for %@: %s", path, archive_error_string(ext));
            if (rv < ARCHIVE_WARN)
                goto out;
        }
    }

    rv = archive_write_finish_entry(ext);
    if (rv < ARCHIVE_OK) {
        LOG("Archive: %s", archive_error_string(ext));
        if (rv < ARCHIVE_WARN)
            goto out;
    }
    result = YES;
out:
    archive_write_close(ext);
    archive_write_finish(ext);
    archive_read_close(a);
    archive_read_finish(a);
    close(fd);
    return result;
}

-(BOOL)extract
{
    init_function();
    return [self extractToPath:[[NSFileManager defaultManager] currentDirectoryPath]];
}

-(BOOL)extractWithFlags:(int)flags
{
    init_function();
    return [self extractToPath:[[NSFileManager defaultManager] currentDirectoryPath] withFlags:flags];
}

-(BOOL)extractToPath:(NSString*)path
{
    init_function();
    return [self extractToPath:path withFlags:DEFAULT_FLAGS];
}

-(BOOL)extractToPath:(NSString*)path withOwner:(id_t)owner andGroup:(id_t)group
{
    init_function();
    return [self extractToPath:path withFlags:DEFAULT_FLAGS overWriteDirectories:NO owner:owner andGroup:group];
}

-(BOOL)extractToPath:(NSString*)path overWriteDirectories:(BOOL)overwrite_dirs
{
    init_function();
    return [self extractToPath:path withFlags:DEFAULT_FLAGS overWriteDirectories:overwrite_dirs];
}

-(BOOL)extractToPath:(NSString*)path withFlags:(int)flags
{
    init_function();
    return [self extractToPath:path withFlags:flags overWriteDirectories:NO];
}

-(BOOL)extractToPath:(NSString*)path withFlags:(int)flags overWriteDirectories:(BOOL)overwrite_dirs
{
    init_function();
    return [self extractToPath:path withFlags:flags overWriteDirectories:overwrite_dirs owner:-1 andGroup:-1];
}
-(BOOL)extractToPath:(NSString*)path withFlags:(int)flags overWriteDirectories:(BOOL)overwrite_dirs owner:(id_t)owner andGroup:(id_t)group
{
    init_function();
    BOOL result = NO;

    int fd = dup(_fd);
    if (fd == -1) {
        LOG("Archive: unable to dupe fd");
        return NO;
    }
    
    struct archive *a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    
    if (archive_read_open_fd(a, _fd, 16384) != ARCHIVE_OK) {
        LOG("Archive: unable to archive_read_open_fd: %s", archive_error_string(a));
        close(fd);
        return result;
    }
    
    struct archive *ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    
    // Seek to entry
    struct archive_entry *entry = NULL;
    int rv;

    NSFileManager *fm = [NSFileManager defaultManager];
    NSString *cwd = [fm currentDirectoryPath];
    if (![fm changeCurrentDirectoryPath:path]) {
        LOG("Archive: unable to change cwd to %@", path);
        goto out;
    }
    while ((rv = archive_read_next_header(a, &entry)) == ARCHIVE_OK) {
        const char *overwrite_temp = NULL;
        if (rv < ARCHIVE_OK) {
            LOG("Archive \"%s\": %s", archive_entry_pathname(entry), archive_error_string(ext));
            if (rv < ARCHIVE_WARN)
                goto out;
        }
        if (owner >= 0) {
            archive_entry_set_uid(entry, owner);
        }
        if (group >= 0) {
            archive_entry_set_gid(entry, group);
        }
        [self addEntry:entry];
        
        NSString *filenameobj = @(archive_entry_pathname(entry));
        const char *filename = filenameobj.UTF8String;
        LOG("Processing %s", filename);
        
        struct stat st;
        rv = stat(filename, &st);
        if (rv == 0) {
            if (!overwrite_dirs) {
                if (S_ISDIR(st.st_mode)) {
                    // Directory already exists, don't mess with it
                    LOG("Archive: skipping directory: %s", filename);
                    continue;
                }
            }
            if (S_ISREG(st.st_mode)) {
                if (isInAMFIStaticCache(filenameobj)) {
                    // --BootLoop
                    LOG("Archive: cowardly refusing to overwrite stock file: %s", filename);
                    continue;
                }
                LOG("Archive: Overwriting file %s", filename);
                overwrite_temp = [[filenameobj stringByAppendingString:@".archive-new"] UTF8String];
                archive_entry_set_pathname(entry, overwrite_temp);
            }
        }
        rv = archive_write_header(ext, entry);
        if (rv < ARCHIVE_OK) {
            LOG("Archive \"%s\": %s", filename, archive_error_string(ext));
        }
        if (archive_entry_size(entry) > 0) {
            rv = copy_data(a, ext);
            if (rv < ARCHIVE_OK) {
                LOG("Archive: Error copying data for %s: %s", filename, archive_error_string(ext));
                if (rv < ARCHIVE_WARN)
                    goto out;
            }
        }
        rv = archive_write_finish_entry(ext);
        if (rv < ARCHIVE_OK) {
            LOG("Archive \"%s\": %s", filename, archive_error_string(ext));
            if (rv < ARCHIVE_WARN)
                goto out;
        }
        if (overwrite_temp) {
            NSString *tmpFile = [filenameobj stringByAppendingString:@".archive-tmp"];
            LOG("renaming out for %s\n", filename);
            if (rename(filename, tmpFile.UTF8String)) {
                unlink(overwrite_temp);
                LOG("Archive: Unable to rename original file %s", filename);
                goto out;
            }
            LOG("renaming in for %s\n", filename);
            if (rename(overwrite_temp, filename)) {
                unlink(overwrite_temp);
                LOG("Archive: Unable to rename new file %s", filename);
                rename(tmpFile.UTF8String, filename);
                goto out;
            }
            LOG("unlinking for %s\n", filename);
            if (unlink(tmpFile.UTF8String)) {
                LOG("Archive: Unable to remove temp file %s", tmpFile.UTF8String);
                goto out;
            }
            overwrite_temp = NULL;
        }
        LOG("%s: OK", filename);
    }
    result = YES;
    _hasReadFiles = YES;
    out:
    [fm changeCurrentDirectoryPath:cwd];
    archive_write_close(ext);
    archive_write_finish(ext);
    archive_read_close(a);
    archive_read_finish(a);
    close(fd);
    return result;
}

-(BOOL)contains:(NSString*)file {
    init_function();
    if (!_hasReadFiles) {
        [self readContents];
    }
    return (_files[file] != nil);
}

-(void)dealloc {
    init_function();
    close(_fd);
}

@end
