# Variables that are common to all Linux-based targets.
TARGET_EXE_EXT :=
TARGET_LIB_EXT := .so

TARGET_LDFLAGS_DYNAMICLIB = -shared -Wl,-soname,$(1)
TARGET_CFLAGS_DYNAMICLIB = -fPIC
