// inject_dylib, https://github.com/stephank/inject_dylib
// Copyright (c) 2015 Stéphan Kochen
// See the README.md for license details.

#ifndef sandbox_private_h
#define sandbox_private_h

// These are private interfaces of `/usr/lib/system/libsystem_sandbox.dylib`.
// The types and function signatures were discovered through reverse
// engineering, but semantics are still vague for the most part.

// Extension name constants.
extern const char *APP_SANDBOX_MACH;
extern const char *APP_SANDBOX_READ;
extern const char *APP_SANDBOX_READ_WRITE;

// The below functions wrap this internal function. Probably shouldn't call it
// directly until we know exact semantics.
// extern char *sandbox_extension_issue(const char *ext, int type, int flags, const char *subject);

// Issue an extension for the given file path.
// The `ext` parameter should be APP_SANDBOX_READ or APP_SANDBOX_READ_WRITE.
// The `reserved` and `flags` parameters should be 0.
// Returns an allocated token string or NULL on failure.
extern char *sandbox_extension_issue_file(const char *ext, const char *path, int reserved, int flags);

// Issue an extension for the given mach service name.
// The `ext` parameter should be APP_SANDBOX_MACH.
// The `reserved` and `flags` parameters should be 0.
// Returns an allocated token string or NULL on failure.
extern char *sandbox_extension_issue_mach(const char *ext, const char *name, int reserved, int flags);

// Consume an extension token. Returns -1 on failure.
extern int sandbox_extension_consume(const char *token);

// Invalidate an extension token. Returns -1 on failure.
extern int sandbox_extension_release(const char *token);

#endif
