# Copyright (c) 2015-2016, Apple Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:  
#
# 1.  Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
# 2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
#     following disclaimer in the documentation and/or other materials provided with the distribution.
#
# 3.  Neither the name of the copyright holder(s) nor the names of any contributors may be used to endorse or
#     promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

INSTALL_PREFIX ?= /usr/local

BUILD_DIR := ./build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj

LZFSE_LIB := $(BIN_DIR)/liblzfse.a
LZFSE_CMD := $(BIN_DIR)/lzfse
LIB_OBJS := $(OBJ_DIR)/lzfse_encode.o  $(OBJ_DIR)/lzfse_decode.o \
            $(OBJ_DIR)/lzfse_encode_base.o $(OBJ_DIR)/lzfse_decode_base.o \
            $(OBJ_DIR)/lzvn_encode_base.o $(OBJ_DIR)/lzvn_decode_base.o \
            $(OBJ_DIR)/lzfse_fse.o
CMD_OBJS := $(OBJ_DIR)/lzfse_main.o
OBJS := $(LIB_OBJS) $(CMD_OBJS)

CFLAGS := -Os -Wall -Wno-unknown-pragmas -Wno-unused-variable -DNDEBUG -D_POSIX_C_SOURCE -std=c99 -fvisibility=hidden

all: $(LZFSE_LIB) $(LZFSE_CMD) $(OBJS)

install: $(LZFSE_LIB) $(LZFSE_CMD)
	@[ -d $(INSTALL_PREFIX)/include ] || mkdir -p $(INSTALL_PREFIX)/include
	@[ -d $(INSTALL_PREFIX)/lib ] || mkdir -p $(INSTALL_PREFIX)/lib
	@[ -d $(INSTALL_PREFIX)/bin ] || mkdir -p $(INSTALL_PREFIX)/bin
	install ./src/lzfse.h $(INSTALL_PREFIX)/include/lzfse.h
	install $(LZFSE_LIB) $(INSTALL_PREFIX)/lib/liblzfse.a
	install $(LZFSE_CMD) $(INSTALL_PREFIX)/bin/lzfse

$(LZFSE_LIB): $(LIB_OBJS)
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	$(LD) -r -o $(OBJ_DIR)/liblzfse_master.o $(LIB_OBJS)
	ar rvs $@ $(OBJ_DIR)/liblzfse_master.o

$(LZFSE_CMD): $(CMD_OBJS) $(LZFSE_LIB)
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(CMD_OBJS) $(LZFSE_LIB)

clean:
	/bin/rm -rf $(BUILD_DIR)

$(OBJ_DIR)/%.o: src/%.c
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
