/*
Copyright (c) 2015-2016, Apple Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:  

1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
    in the documentation and/or other materials provided with the distribution.

3.  Neither the name of the copyright holder(s) nor the names of any contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// LZFSE decode API

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "lzfse.h"
#include "lzfse_internal.h"

size_t lzfse_decode_scratch_size() { return sizeof(lzfse_decoder_state); }

size_t lzfse_decode_buffer_with_scratch(uint8_t *__restrict dst_buffer, 
                         size_t dst_size, const uint8_t *__restrict src_buffer,
                         size_t src_size, void *__restrict scratch_buffer) {
  lzfse_decoder_state *s = (lzfse_decoder_state *)scratch_buffer;
  memset(s, 0x00, sizeof(*s));

  // Initialize state
  s->src = src_buffer;
  s->src_begin = src_buffer;
  s->src_end = s->src + src_size;
  s->dst = dst_buffer;
  s->dst_begin = dst_buffer;
  s->dst_end = dst_buffer + dst_size;

  // Decode
  int status = lzfse_decode(s);
  if (status == LZFSE_STATUS_DST_FULL)
    return dst_size;
  if (status != LZFSE_STATUS_OK)
    return 0;                           // failed
  return (size_t)(s->dst - dst_buffer); // bytes written
}

#define DECODE_SRC_CHUNK_SIZE 0x40000
#define DECODE_DST_CHUNK_SIZE (DECODE_SRC_CHUNK_SIZE * 4)
size_t lzfse_decode_file_with_scratch(FILE *dst_file, FILE *src_file, size_t src_size,
                                      int *error, void *__restrict scratch_buffer) {
  lzfse_decoder_state *s = (lzfse_decoder_state *)scratch_buffer;
  memset(s, 0x00, sizeof(*s));

  uint8_t *src_buf = malloc(DECODE_SRC_CHUNK_SIZE);
  if (src_buf == NULL) {
      if (error) *error = 1;
      return 0;
  }

  uint8_t *dst_buf = malloc(DECODE_DST_CHUNK_SIZE + LZFSE_ENCODE_MAX_D_VALUE);
  if (dst_buf == NULL) {
      if (error) *error = 1;
      return 0;
  }
  size_t dst_size = DECODE_DST_CHUNK_SIZE;

  int nr = fread(src_buf, 1, src_size?(src_size<DECODE_SRC_CHUNK_SIZE?src_size:DECODE_SRC_CHUNK_SIZE):DECODE_SRC_CHUNK_SIZE, src_file);
  if (nr < 0) {
      if (error) *error = 2;
      return 0;
  }
  src_size -= nr;

  // Initialize state
  s->src = src_buf;
  s->src_begin = src_buf;
  s->src_end = s->src + DECODE_SRC_CHUNK_SIZE;
  s->dst = dst_buf;
  s->dst_begin = dst_buf;
  s->dst_end = dst_buf + DECODE_DST_CHUNK_SIZE + LZFSE_ENCODE_MAX_D_VALUE;

  size_t written = 0;

  int status;
  while (s->end_of_stream == 0) {
      // Decode a block
      int status = lzfse_decode(s);
      if (error) *error = status;

      if (status == LZFSE_STATUS_ERROR) {
          return written;
      }

      int processed_data = 0;

      size_t dst_used = s->dst - s->dst_begin;
      if (dst_used > LZFSE_ENCODE_MAX_D_VALUE) {
          int num_to_write = dst_used - LZFSE_ENCODE_MAX_D_VALUE;
          processed_data = 1;
          int nw = fwrite(s->dst_begin, 1, num_to_write, dst_file);
          if (nw != num_to_write) {
              if (error) *error = 3;
              return written + (nw>0?nw:0);
          }
          written += num_to_write;
          memmove(s->dst_begin, s->dst_begin + num_to_write, LZFSE_ENCODE_MAX_D_VALUE);
          s->dst -= num_to_write;
      }

      size_t src_used = s->src - s->src_begin;
      size_t src_unused = s->src_end - s->src;
      if (src_used > 0) {
          processed_data = 1;
          memmove((uint8_t*)s->src_begin, s->src, src_unused);
          s->src = s->src_begin;
      }
      if (!processed_data) {
          if (error) *error = 4;
          fprintf(stderr, "lzfse not making progress, aborting.  Maybe we need larger buffers?\n");
          return written;
      }
      if (src_unused < DECODE_SRC_CHUNK_SIZE && src_size > 0) {
          size_t size_wanted = DECODE_SRC_CHUNK_SIZE - src_unused;
          size_t size_to_read = src_size?(src_size<size_wanted?src_size:size_wanted):size_wanted;
          nr = fread(src_buf + src_unused, 1, size_to_read, src_file);
          if (nr < 0) {
              if (error) *error = 2;
              return written;
          }
          src_size -= nr;
          s->src_end = s->src + src_unused + nr;
      }
  }

  size_t dst_used = s->dst - s->dst_begin;
  if (dst_used > 0) {
      int nw = fwrite(s->dst_begin, 1, dst_used, dst_file);
      if (nw != dst_used) {
          if (error) *error = 3;
          return written + (nw>0?nw:0);
      }
  }
  free(dst_buf);
  free(src_buf);

  return written; // bytes written
}

size_t lzfse_decode_buffer(uint8_t *__restrict dst_buffer, size_t dst_size,
                           const uint8_t *__restrict src_buffer,
                           size_t src_size, void *__restrict scratch_buffer) {
  int has_malloc = 0;
  size_t ret = 0;

  // Deal with the possible NULL pointer
  if (scratch_buffer == NULL) {
    // +1 in case scratch size could be zero
    scratch_buffer = malloc(lzfse_decode_scratch_size() + 1);
    has_malloc = 1;
  }
  if (scratch_buffer == NULL)
    return 0;
  ret = lzfse_decode_buffer_with_scratch(dst_buffer, 
                               dst_size, src_buffer, 
                               src_size, scratch_buffer);
  if (has_malloc)
    free(scratch_buffer);
  return ret;
} 

size_t lzfse_decode_file(FILE *dst_file, FILE *src_file, size_t src_size,
                         int *error, void *__restrict scratch_buffer) {
  int has_malloc = 0;
  size_t ret = 0;

  // Deal with the possible NULL pointer
  if (scratch_buffer == NULL) {
    // +1 in case scratch size could be zero
    scratch_buffer = malloc(lzfse_decode_scratch_size() + 1);
    has_malloc = 1;
  }
  if (scratch_buffer == NULL) {
    if (error != NULL) *error = 1;
    return 0;
  }
  ret = lzfse_decode_file_with_scratch(dst_file, src_file, src_size, error, scratch_buffer);
  if (has_malloc)
    free(scratch_buffer);
  return ret;
} 
