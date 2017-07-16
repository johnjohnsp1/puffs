#ifndef PUFFS_FLATE_H
#define PUFFS_FLATE_H

// Code generated by puffs-gen-c. DO NOT EDIT.

#ifndef PUFFS_BASE_HEADER_H
#define PUFFS_BASE_HEADER_H

// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Puffs requires a word size of at least 32 bits because it assumes that
// converting a u32 to usize will never overflow. For example, the size of a
// decoded image is often represented, explicitly or implicitly in an image
// file, as a u32, and it is convenient to compare that to a buffer size.
//
// Similarly, the word size is at most 64 bits because it assumes that
// converting a usize to u64 will never overflow.
#if __WORDSIZE < 32
#error "Puffs requires a word size of at least 32 bits"
#elif __WORDSIZE > 64
#error "Puffs requires a word size of at most 64 bits"
#endif

// PUFFS_VERSION is the major.minor version number as a uint32. The major
// number is the high 16 bits. The minor number is the low 16 bits.
//
// The intention is to bump the version number at least on every API / ABI
// backwards incompatible change.
//
// For now, the API and ABI are simply unstable and can change at any time.
//
// TODO: don't hard code this in base-header.h.
#define PUFFS_VERSION (0x00001)

// puffs_base_buf1 is a 1-dimensional buffer (a pointer and length) plus
// additional indexes into that buffer.
//
// A value with all fields NULL or zero is a valid, empty buffer.
typedef struct {
  uint8_t* ptr;  // Pointer.
  size_t len;    // Length.
  size_t wi;     // Write index. Invariant: wi <= len.
  size_t ri;     // Read  index. Invariant: ri <= wi.
  bool closed;   // No further writes are expected.
} puffs_base_buf1;

// puffs_base_limit1 provides a limited view of a 1-dimensional byte stream:
// its first N bytes. That N can be greater than a buffer's current read or
// write capacity. N decreases naturally over time as bytes are read from or
// written to the stream.
//
// A value with all fields NULL or zero is a valid, unlimited view.
typedef struct puffs_base_limit1 {
  uint64_t* ptr_to_len;            // Pointer to N.
  struct puffs_base_limit1* next;  // Linked list of limits.
} puffs_base_limit1;

typedef struct {
  puffs_base_buf1* buf;
  puffs_base_limit1 limit;
} puffs_base_reader1;

typedef struct {
  puffs_base_buf1* buf;
  puffs_base_limit1 limit;
} puffs_base_writer1;

#endif  // PUFFS_BASE_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif

// ---------------- Status Codes

// Status codes are int32_t values:
//  - the sign bit indicates a non-recoverable status code: an error
//  - bits 10-30 hold the packageid: a namespace
//  - bits 8-9 are reserved
//  - bits 0-7 are a package-namespaced numeric code
//
// Do not manipulate these bits directly. Use the API functions such as
// puffs_flate_status_is_error instead.
typedef int32_t puffs_flate_status;

#define puffs_flate_packageid 967230  // 0x000ec23e

#define PUFFS_FLATE_STATUS_OK 0                               // 0x00000000
#define PUFFS_FLATE_ERROR_BAD_PUFFS_VERSION -2147483647       // 0x80000001
#define PUFFS_FLATE_ERROR_BAD_RECEIVER -2147483646            // 0x80000002
#define PUFFS_FLATE_ERROR_BAD_ARGUMENT -2147483645            // 0x80000003
#define PUFFS_FLATE_ERROR_CONSTRUCTOR_NOT_CALLED -2147483644  // 0x80000004
#define PUFFS_FLATE_ERROR_CLOSED_FOR_WRITES -2147483643       // 0x80000005
#define PUFFS_FLATE_ERROR_UNEXPECTED_EOF -2147483642          // 0x80000006
#define PUFFS_FLATE_SUSPENSION_SHORT_READ 7                   // 0x00000007
#define PUFFS_FLATE_SUSPENSION_SHORT_WRITE 8                  // 0x00000008
#define PUFFS_FLATE_SUSPENSION_LIMITED_READ 9                 // 0x00000009
#define PUFFS_FLATE_SUSPENSION_LIMITED_WRITE 10               // 0x0000000a

#define PUFFS_FLATE_ERROR_BAD_DISTANCE_CODE_COUNT -1157040128  // 0xbb08f800
#define PUFFS_FLATE_ERROR_BAD_FLATE_BLOCK -1157040127          // 0xbb08f801
#define PUFFS_FLATE_ERROR_BAD_LITERAL_LENGTH_CODE_COUNT \
  -1157040126  // 0xbb08f802
#define PUFFS_FLATE_ERROR_INCONSISTENT_STORED_BLOCK_LENGTH \
  -1157040125  // 0xbb08f803
#define PUFFS_FLATE_ERROR_INTERNAL_ERROR_INCONSISTENT_N_BITS \
  -1157040124                                                      // 0xbb08f804
#define PUFFS_FLATE_ERROR_TODO_FIXED_HUFFMAN_BLOCKS -1157040123    // 0xbb08f805
#define PUFFS_FLATE_ERROR_TODO_DYNAMIC_HUFFMAN_BLOCKS -1157040122  // 0xbb08f806

bool puffs_flate_status_is_error(puffs_flate_status s);

const char* puffs_flate_status_string(puffs_flate_status s);

// ---------------- Public Consts

// ---------------- Structs

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so. Instead, use the
  // puffs_flate_decoder_etc functions.
  //
  // In C++, these fields would be "private", but C does not support that.
  //
  // It is a struct, not a struct*, so that it can be stack allocated.
  struct {
    puffs_flate_status status;
    uint32_t magic;
    uint64_t scratch;

    uint32_t f_bits;
    uint32_t f_n_bits;
    uint8_t f_code_length_code_lengths[19];
    uint32_t f_wip0;
    uint32_t f_wip1;

    struct {
      uint32_t coro_susp_point;
      uint32_t v_final;
      uint32_t v_type;
    } c_decode[1];
    struct {
      uint32_t coro_susp_point;
      uint32_t v_n;
    } c_decode_uncompressed[1];
    struct {
      uint32_t coro_susp_point;
      uint32_t v_bits;
      uint32_t v_n_bits;
      uint32_t v_hlit;
      uint32_t v_hdist;
      uint32_t v_hclen;
      uint32_t v_i;
    } c_decode_dynamic[1];
  } private_impl;
} puffs_flate_decoder;

// ---------------- Public Constructor and Destructor Prototypes

// puffs_flate_decoder_constructor is a constructor function.
//
// It should be called before any other puffs_flate_decoder_* function.
//
// Pass PUFFS_VERSION and 0 for puffs_version and for_internal_use_only.
void puffs_flate_decoder_constructor(puffs_flate_decoder* self,
                                     uint32_t puffs_version,
                                     uint32_t for_internal_use_only);

void puffs_flate_decoder_destructor(puffs_flate_decoder* self);

// ---------------- Public Function Prototypes

puffs_flate_status puffs_flate_decoder_decode(puffs_flate_decoder* self,
                                              puffs_base_writer1 a_dst,
                                              puffs_base_reader1 a_src);

puffs_flate_status puffs_flate_decoder_decode_uncompressed(
    puffs_flate_decoder* self,
    puffs_base_writer1 a_dst,
    puffs_base_reader1 a_src);

puffs_flate_status puffs_flate_decoder_decode_dynamic(puffs_flate_decoder* self,
                                                      puffs_base_writer1 a_dst,
                                                      puffs_base_reader1 a_src);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // PUFFS_FLATE_H

// C HEADER ENDS HERE.

#ifndef PUFFS_BASE_IMPL_H
#define PUFFS_BASE_IMPL_H

// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

// TODO: look for (ifdef) the x86 architecture and cast the pointer? Only do so
// if a benchmark justifies the additional code path.
#define PUFFS_U32LE(p)                                 \
  (((uint32_t)(p[0]) << 0) | ((uint32_t)(p[1]) << 8) | \
   ((uint32_t)(p[2]) << 16) | ((uint32_t)(p[3]) << 24))

// Use switch cases for coroutine suspension points, similar to the technique
// in https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
//
// We use a trivial macro instead of an explicit assignment and case statement
// so that clang-format doesn't get confused by the unusual "case"s.
#define PUFFS_COROUTINE_SUSPENSION_POINT(n) \
  coro_susp_point = n;                      \
  case n:

// Clang also defines "__GNUC__".
#if defined(__GNUC__)
#define PUFFS_LIKELY(expr) (__builtin_expect(!!(expr), 1))
#define PUFFS_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))
#else
#define PUFFS_LIKELY(expr) (expr)
#define PUFFS_UNLIKELY(expr) (expr)
#endif

#endif  // PUFFS_BASE_IMPL_H

// ---------------- Status Codes Implementations

bool puffs_flate_status_is_error(puffs_flate_status s) {
  return s < 0;
}

const char* puffs_flate_status_strings0[11] = {
    "flate: ok",
    "flate: bad puffs version",
    "flate: bad receiver",
    "flate: bad argument",
    "flate: constructor not called",
    "flate: closed for writes",
    "flate: unexpected EOF",
    "flate: short read",
    "flate: short write",
    "flate: limited read",
    "flate: limited write",
};

const char* puffs_flate_status_strings1[7] = {
    "flate: bad distance code count",
    "flate: bad flate block",
    "flate: bad literal/length code count",
    "flate: inconsistent stored block length",
    "flate: internal error: inconsistent n_bits",
    "flate: TODO: fixed Huffman blocks",
    "flate: TODO: dynamic Huffman blocks",
};

const char* puffs_flate_status_string(puffs_flate_status s) {
  const char** a = NULL;
  uint32_t n = 0;
  switch ((s >> 10) & 0x1fffff) {
    case 0:
      a = puffs_flate_status_strings0;
      n = 11;
      break;
    case puffs_flate_packageid:
      a = puffs_flate_status_strings1;
      n = 7;
      break;
  }
  uint32_t i = s & 0xff;
  return i < n ? a[i] : "flate: unknown status";
}

// ---------------- Private Consts

static const uint8_t puffs_flate_code_order[19] = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15,
};

// ---------------- Private Constructor and Destructor Prototypes

// ---------------- Private Function Prototypes

// ---------------- Constructor and Destructor Implementations

// PUFFS_MAGIC is a magic number to check that constructors are called. It's
// not foolproof, given C doesn't automatically zero memory before use, but it
// should catch 99.99% of cases.
//
// Its (non-zero) value is arbitrary, based on md5sum("puffs").
#define PUFFS_MAGIC (0xCB3699CCU)

// PUFFS_ALREADY_ZEROED is passed from a container struct's constructor to a
// containee struct's constructor when the container has already zeroed the
// containee's memory.
//
// Its (non-zero) value is arbitrary, based on md5sum("zeroed").
#define PUFFS_ALREADY_ZEROED (0x68602EF1U)

void puffs_flate_decoder_constructor(puffs_flate_decoder* self,
                                     uint32_t puffs_version,
                                     uint32_t for_internal_use_only) {
  if (!self) {
    return;
  }
  if (puffs_version != PUFFS_VERSION) {
    self->private_impl.status = PUFFS_FLATE_ERROR_BAD_PUFFS_VERSION;
    return;
  }
  if (for_internal_use_only != PUFFS_ALREADY_ZEROED) {
    memset(self, 0, sizeof(*self));
  }
  self->private_impl.magic = PUFFS_MAGIC;
}

void puffs_flate_decoder_destructor(puffs_flate_decoder* self) {
  if (!self) {
    return;
  }
}

// ---------------- Function Implementations

puffs_flate_status puffs_flate_decoder_decode(puffs_flate_decoder* self,
                                              puffs_base_writer1 a_dst,
                                              puffs_base_reader1 a_src) {
  if (!self) {
    return PUFFS_FLATE_ERROR_BAD_RECEIVER;
  }
  if (self->private_impl.magic != PUFFS_MAGIC) {
    self->private_impl.status = PUFFS_FLATE_ERROR_CONSTRUCTOR_NOT_CALLED;
  }
  if (self->private_impl.status < 0) {
    return self->private_impl.status;
  }
  puffs_flate_status status = PUFFS_FLATE_STATUS_OK;

  uint32_t v_final;
  uint32_t v_type;

  uint8_t* b_rptr_src = NULL;
  uint8_t* b_rend_src = NULL;
  if (a_src.buf) {
    b_rptr_src = a_src.buf->ptr + a_src.buf->ri;
    size_t len = a_src.buf->wi - a_src.buf->ri;
    puffs_base_limit1* lim;
    for (lim = &a_src.limit; lim; lim = lim->next) {
      if (lim->ptr_to_len && (len > *lim->ptr_to_len)) {
        len = *lim->ptr_to_len;
      }
    }
    b_rend_src = b_rptr_src + len;
  }

  uint32_t coro_susp_point = self->private_impl.c_decode[0].coro_susp_point;
  if (coro_susp_point) {
    v_final = self->private_impl.c_decode[0].v_final;
    v_type = self->private_impl.c_decode[0].v_type;
  }
  switch (coro_susp_point) {
    PUFFS_COROUTINE_SUSPENSION_POINT(0);

    while (true) {
      while (self->private_impl.f_n_bits < 3) {
        PUFFS_COROUTINE_SUSPENSION_POINT(1);
        if (PUFFS_UNLIKELY(b_rptr_src == b_rend_src)) {
          goto short_read_src;
        }
        uint8_t t_0 = *b_rptr_src++;
        self->private_impl.f_bits |=
            (((uint32_t)(t_0)) << self->private_impl.f_n_bits);
        self->private_impl.f_n_bits += 8;
      }
      v_final = (self->private_impl.f_bits & 1);
      v_type = ((self->private_impl.f_bits >> 1) & 3);
      self->private_impl.f_bits >>= 3;
      self->private_impl.f_n_bits -= 3;
      if (v_type == 0) {
        PUFFS_COROUTINE_SUSPENSION_POINT(2);
        if (a_src.buf) {
          size_t n = b_rptr_src - (a_src.buf->ptr + a_src.buf->ri);
          a_src.buf->ri += n;
          puffs_base_limit1* lim;
          for (lim = &a_src.limit; lim; lim = lim->next) {
            if (lim->ptr_to_len) {
              *lim->ptr_to_len -= n;
            }
          }
        }
        status = puffs_flate_decoder_decode_uncompressed(self, a_dst, a_src);
        if (a_src.buf) {
          b_rptr_src = a_src.buf->ptr + a_src.buf->ri;
          size_t len = a_src.buf->wi - a_src.buf->ri;
          puffs_base_limit1* lim;
          for (lim = &a_src.limit; lim; lim = lim->next) {
            if (lim->ptr_to_len && (len > *lim->ptr_to_len)) {
              len = *lim->ptr_to_len;
            }
          }
          b_rend_src = b_rptr_src + len;
        }
        if (status) {
          goto suspend;
        }
      } else if (v_type == 1) {
        status = PUFFS_FLATE_ERROR_TODO_FIXED_HUFFMAN_BLOCKS;
        goto exit;
      } else if (v_type == 2) {
        PUFFS_COROUTINE_SUSPENSION_POINT(3);
        if (a_src.buf) {
          size_t n = b_rptr_src - (a_src.buf->ptr + a_src.buf->ri);
          a_src.buf->ri += n;
          puffs_base_limit1* lim;
          for (lim = &a_src.limit; lim; lim = lim->next) {
            if (lim->ptr_to_len) {
              *lim->ptr_to_len -= n;
            }
          }
        }
        status = puffs_flate_decoder_decode_dynamic(self, a_dst, a_src);
        if (a_src.buf) {
          b_rptr_src = a_src.buf->ptr + a_src.buf->ri;
          size_t len = a_src.buf->wi - a_src.buf->ri;
          puffs_base_limit1* lim;
          for (lim = &a_src.limit; lim; lim = lim->next) {
            if (lim->ptr_to_len && (len > *lim->ptr_to_len)) {
              len = *lim->ptr_to_len;
            }
          }
          b_rend_src = b_rptr_src + len;
        }
        if (status) {
          goto suspend;
        }
      } else {
        status = PUFFS_FLATE_ERROR_BAD_FLATE_BLOCK;
        goto exit;
      }
      if (v_final != 0) {
        status = PUFFS_FLATE_STATUS_OK;
        goto suspend;
      }
    }
    self->private_impl.c_decode[0].coro_susp_point = 0;
    goto exit;
  }

  goto suspend;
suspend:
  self->private_impl.c_decode[0].coro_susp_point = coro_susp_point;
  self->private_impl.c_decode[0].v_final = v_final;
  self->private_impl.c_decode[0].v_type = v_type;

exit:
  if (a_src.buf) {
    size_t n = b_rptr_src - (a_src.buf->ptr + a_src.buf->ri);
    a_src.buf->ri += n;
    puffs_base_limit1* lim;
    for (lim = &a_src.limit; lim; lim = lim->next) {
      if (lim->ptr_to_len) {
        *lim->ptr_to_len -= n;
      }
    }
  }

  self->private_impl.status = status;
  return status;

short_read_src:
  if (a_src.limit.ptr_to_len) {
    status = PUFFS_FLATE_SUSPENSION_LIMITED_READ;
  } else if (a_src.buf->closed) {
    status = PUFFS_FLATE_ERROR_UNEXPECTED_EOF;
    goto exit;
  } else {
    status = PUFFS_FLATE_SUSPENSION_SHORT_READ;
  }
  goto suspend;
}

puffs_flate_status puffs_flate_decoder_decode_uncompressed(
    puffs_flate_decoder* self,
    puffs_base_writer1 a_dst,
    puffs_base_reader1 a_src) {
  if (!self) {
    return PUFFS_FLATE_ERROR_BAD_RECEIVER;
  }
  if (self->private_impl.magic != PUFFS_MAGIC) {
    self->private_impl.status = PUFFS_FLATE_ERROR_CONSTRUCTOR_NOT_CALLED;
  }
  if (self->private_impl.status < 0) {
    return self->private_impl.status;
  }
  puffs_flate_status status = PUFFS_FLATE_STATUS_OK;

  uint32_t v_n;

  uint8_t* b_wptr_dst = NULL;
  uint8_t* b_wend_dst = NULL;
  if (a_dst.buf) {
    b_wptr_dst = a_dst.buf->ptr + a_dst.buf->wi;
    size_t len = a_dst.buf->len - a_dst.buf->wi;
    puffs_base_limit1* lim;
    for (lim = &a_dst.limit; lim; lim = lim->next) {
      if (lim->ptr_to_len && (len > *lim->ptr_to_len)) {
        len = *lim->ptr_to_len;
      }
    }
    b_wend_dst = b_wptr_dst + len;
  }
  uint8_t* b_rptr_src = NULL;
  uint8_t* b_rend_src = NULL;
  if (a_src.buf) {
    b_rptr_src = a_src.buf->ptr + a_src.buf->ri;
    size_t len = a_src.buf->wi - a_src.buf->ri;
    puffs_base_limit1* lim;
    for (lim = &a_src.limit; lim; lim = lim->next) {
      if (lim->ptr_to_len && (len > *lim->ptr_to_len)) {
        len = *lim->ptr_to_len;
      }
    }
    b_rend_src = b_rptr_src + len;
  }

  uint32_t coro_susp_point =
      self->private_impl.c_decode_uncompressed[0].coro_susp_point;
  if (coro_susp_point) {
    v_n = self->private_impl.c_decode_uncompressed[0].v_n;
  }
  switch (coro_susp_point) {
    PUFFS_COROUTINE_SUSPENSION_POINT(0);

    if (self->private_impl.f_n_bits >= 8) {
      status = PUFFS_FLATE_ERROR_INTERNAL_ERROR_INCONSISTENT_N_BITS;
      goto exit;
    }
    self->private_impl.f_n_bits = 0;
    PUFFS_COROUTINE_SUSPENSION_POINT(1);
    uint32_t t_1;
    if (PUFFS_LIKELY(b_rend_src - b_rptr_src >= 4)) {
      t_1 = PUFFS_U32LE(b_rptr_src);
      b_rptr_src += 4;
    } else {
      self->private_impl.scratch = 0;
      PUFFS_COROUTINE_SUSPENSION_POINT(2);
      while (true) {
        if (PUFFS_UNLIKELY(b_rptr_src == b_rend_src)) {
          goto short_read_src;
        }
        uint32_t t_0 = self->private_impl.scratch >> 56;
        self->private_impl.scratch <<= 8;
        self->private_impl.scratch >>= 8;
        self->private_impl.scratch |= ((uint64_t)(*b_rptr_src++)) << t_0;
        if (t_0 == 24) {
          t_1 = self->private_impl.scratch;
          break;
        }
        t_0 += 8;
        self->private_impl.scratch |= ((uint64_t)(t_0)) << 56;
      }
    }
    v_n = t_1;
    if ((((v_n) & ((1 << (16)) - 1)) + ((v_n) >> (32 - (16)))) != 65535) {
      status = PUFFS_FLATE_ERROR_INCONSISTENT_STORED_BLOCK_LENGTH;
      goto exit;
    }
    PUFFS_COROUTINE_SUSPENSION_POINT(3);
    {
      self->private_impl.scratch = ((v_n) & ((1 << (16)) - 1));
      PUFFS_COROUTINE_SUSPENSION_POINT(4);
      size_t t_2 = self->private_impl.scratch;
      if (t_2 > b_wend_dst - b_wptr_dst) {
        t_2 = b_wend_dst - b_wptr_dst;
        status = PUFFS_FLATE_SUSPENSION_SHORT_WRITE;
      }
      if (t_2 > b_rend_src - b_rptr_src) {
        t_2 = b_rend_src - b_rptr_src;
        status = PUFFS_FLATE_SUSPENSION_SHORT_READ;
      }
      memmove(b_wptr_dst, b_rptr_src, t_2);
      b_wptr_dst += t_2;
      b_rptr_src += t_2;
      if (status) {
        self->private_impl.scratch -= t_2;
        goto suspend;
      }
    }
    self->private_impl.c_decode_uncompressed[0].coro_susp_point = 0;
    goto exit;
  }

  goto suspend;
suspend:
  self->private_impl.c_decode_uncompressed[0].coro_susp_point = coro_susp_point;
  self->private_impl.c_decode_uncompressed[0].v_n = v_n;

exit:
  if (a_dst.buf) {
    size_t n = b_wptr_dst - (a_dst.buf->ptr + a_dst.buf->wi);
    a_dst.buf->wi += n;
    puffs_base_limit1* lim;
    for (lim = &a_dst.limit; lim; lim = lim->next) {
      if (lim->ptr_to_len) {
        *lim->ptr_to_len -= n;
      }
    }
  }
  if (a_src.buf) {
    size_t n = b_rptr_src - (a_src.buf->ptr + a_src.buf->ri);
    a_src.buf->ri += n;
    puffs_base_limit1* lim;
    for (lim = &a_src.limit; lim; lim = lim->next) {
      if (lim->ptr_to_len) {
        *lim->ptr_to_len -= n;
      }
    }
  }

  self->private_impl.status = status;
  return status;

short_read_src:
  if (a_src.limit.ptr_to_len) {
    status = PUFFS_FLATE_SUSPENSION_LIMITED_READ;
  } else if (a_src.buf->closed) {
    status = PUFFS_FLATE_ERROR_UNEXPECTED_EOF;
    goto exit;
  } else {
    status = PUFFS_FLATE_SUSPENSION_SHORT_READ;
  }
  goto suspend;
}

puffs_flate_status puffs_flate_decoder_decode_dynamic(
    puffs_flate_decoder* self,
    puffs_base_writer1 a_dst,
    puffs_base_reader1 a_src) {
  if (!self) {
    return PUFFS_FLATE_ERROR_BAD_RECEIVER;
  }
  if (self->private_impl.magic != PUFFS_MAGIC) {
    self->private_impl.status = PUFFS_FLATE_ERROR_CONSTRUCTOR_NOT_CALLED;
  }
  if (self->private_impl.status < 0) {
    return self->private_impl.status;
  }
  puffs_flate_status status = PUFFS_FLATE_STATUS_OK;

  uint32_t v_bits;
  uint32_t v_n_bits;
  uint32_t v_hlit;
  uint32_t v_hdist;
  uint32_t v_hclen;
  uint32_t v_i;

  uint8_t* b_rptr_src = NULL;
  uint8_t* b_rend_src = NULL;
  if (a_src.buf) {
    b_rptr_src = a_src.buf->ptr + a_src.buf->ri;
    size_t len = a_src.buf->wi - a_src.buf->ri;
    puffs_base_limit1* lim;
    for (lim = &a_src.limit; lim; lim = lim->next) {
      if (lim->ptr_to_len && (len > *lim->ptr_to_len)) {
        len = *lim->ptr_to_len;
      }
    }
    b_rend_src = b_rptr_src + len;
  }

  uint32_t coro_susp_point =
      self->private_impl.c_decode_dynamic[0].coro_susp_point;
  if (coro_susp_point) {
    v_bits = self->private_impl.c_decode_dynamic[0].v_bits;
    v_n_bits = self->private_impl.c_decode_dynamic[0].v_n_bits;
    v_hlit = self->private_impl.c_decode_dynamic[0].v_hlit;
    v_hdist = self->private_impl.c_decode_dynamic[0].v_hdist;
    v_hclen = self->private_impl.c_decode_dynamic[0].v_hclen;
    v_i = self->private_impl.c_decode_dynamic[0].v_i;
  }
  switch (coro_susp_point) {
    PUFFS_COROUTINE_SUSPENSION_POINT(0);

    v_bits = self->private_impl.f_bits;
    v_n_bits = self->private_impl.f_n_bits;
    while (v_n_bits < 14) {
      PUFFS_COROUTINE_SUSPENSION_POINT(1);
      if (PUFFS_UNLIKELY(b_rptr_src == b_rend_src)) {
        goto short_read_src;
      }
      uint8_t t_0 = *b_rptr_src++;
      v_bits |= (((uint32_t)(t_0)) << v_n_bits);
      v_n_bits += 8;
    }
    v_hlit = (((v_bits) & ((1 << (5)) - 1)) + 257);
    if (v_hlit > 286) {
      status = PUFFS_FLATE_ERROR_BAD_LITERAL_LENGTH_CODE_COUNT;
      goto exit;
    }
    v_bits >>= 5;
    v_hdist = (((v_bits) & ((1 << (5)) - 1)) + 1);
    if (v_hdist > 30) {
      status = PUFFS_FLATE_ERROR_BAD_DISTANCE_CODE_COUNT;
      goto exit;
    }
    v_bits >>= 5;
    v_hclen = (((v_bits) & ((1 << (4)) - 1)) + 4);
    v_bits >>= 4;
    v_n_bits -= 14;
    v_i = 0;
    while (v_i < v_hclen) {
      while (v_n_bits < 3) {
        PUFFS_COROUTINE_SUSPENSION_POINT(2);
        if (PUFFS_UNLIKELY(b_rptr_src == b_rend_src)) {
          goto short_read_src;
        }
        uint8_t t_1 = *b_rptr_src++;
        v_bits |= (((uint32_t)(t_1)) << v_n_bits);
        v_n_bits += 8;
      }
      self->private_impl
          .f_code_length_code_lengths[puffs_flate_code_order[v_i]] =
          ((uint8_t)((v_bits & 7)));
      v_bits >>= 3;
      v_n_bits -= 3;
      v_i += 1;
    }
    while (v_i < 19) {
      self->private_impl
          .f_code_length_code_lengths[puffs_flate_code_order[v_i]] = 0;
      v_i += 1;
    }
    self->private_impl.f_wip0 = v_hlit;
    self->private_impl.f_wip1 = v_hdist;
    self->private_impl.f_bits = v_bits;
    self->private_impl.f_n_bits = v_n_bits;
    self->private_impl.c_decode_dynamic[0].coro_susp_point = 0;
    goto exit;
  }

  goto suspend;
suspend:
  self->private_impl.c_decode_dynamic[0].coro_susp_point = coro_susp_point;
  self->private_impl.c_decode_dynamic[0].v_bits = v_bits;
  self->private_impl.c_decode_dynamic[0].v_n_bits = v_n_bits;
  self->private_impl.c_decode_dynamic[0].v_hlit = v_hlit;
  self->private_impl.c_decode_dynamic[0].v_hdist = v_hdist;
  self->private_impl.c_decode_dynamic[0].v_hclen = v_hclen;
  self->private_impl.c_decode_dynamic[0].v_i = v_i;

exit:
  if (a_src.buf) {
    size_t n = b_rptr_src - (a_src.buf->ptr + a_src.buf->ri);
    a_src.buf->ri += n;
    puffs_base_limit1* lim;
    for (lim = &a_src.limit; lim; lim = lim->next) {
      if (lim->ptr_to_len) {
        *lim->ptr_to_len -= n;
      }
    }
  }

  self->private_impl.status = status;
  return status;

short_read_src:
  if (a_src.limit.ptr_to_len) {
    status = PUFFS_FLATE_SUSPENSION_LIMITED_READ;
  } else if (a_src.buf->closed) {
    status = PUFFS_FLATE_ERROR_UNEXPECTED_EOF;
    goto exit;
  } else {
    status = PUFFS_FLATE_SUSPENSION_SHORT_READ;
  }
  goto suspend;
}
