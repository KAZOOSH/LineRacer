/* Copyright 2016 Google Inc. All Rights Reserved.

   Distributed under MIT license.
   See file LICENSE for detail or copy at https://opensource.org/licenses/MIT
*/

/* Constants and formulas that affect speed-ratio trade-offs and thus define
   quality levels. */

#ifndef BROTLI_ENC_QUALITY_H_
#define BROTLI_ENC_QUALITY_H_

#include "./encode.h"

#define FAST_ONE_PASS_COMPRESSION_QUALITY 0
#define FAST_TWO_PASS_COMPRESSION_QUALITY 1
#define ZOPFLIFICATION_QUALITY 10
#define HQ_ZOPFLIFICATION_QUALITY 11

#define MAX_QUALITY_FOR_STATIC_ENRTOPY_CODES 2
#define MIN_QUALITY_FOR_BLOCK_SPLIT 4
#define MIN_QUALITY_FOR_OPTIMIZE_HISTOGRAMS 4
#define MIN_QUALITY_FOR_EXTENSIVE_REFERENCE_SEARCH 5
#define MIN_QUALITY_FOR_CONTEXT_MODELING 5
#define MIN_QUALITY_FOR_HQ_CONTEXT_MODELING 7
#define MIN_QUALITY_FOR_HQ_BLOCK_SPLITTING 10
/* Only for "font" mode. */
#define MIN_QUALITY_FOR_RECOMPUTE_DISTANCE_PREFIXES 10

/* For quality below MIN_QUALITY_FOR_BLOCK_SPLIT there is no block splitting,
   so we buffer at most this much literals and commands. */
#define MAX_NUM_DELAYED_SYMBOLS 0x2fff

/* Encoding parameters */
typedef struct BrotliEncoderParams {
  BrotliEncoderMode mode;
  int quality;
  int lgwin;
  int lgblock;
} BrotliEncoderParams;

/* Returns hashtable size for quality levels 0 and 1. */
static BROTLI_INLINE size_t MaxHashTableSize(int quality) {
  return quality == FAST_ONE_PASS_COMPRESSION_QUALITY ? 1 << 15 : 1 << 17;
}

/* The maximum length for which the zopflification uses distinct distances. */
#define MAX_ZOPFLI_LEN_QUALITY_10 150
#define MAX_ZOPFLI_LEN_QUALITY_11 325

static BROTLI_INLINE size_t MaxZopfliLen(const BrotliEncoderParams* settings) {
  return settings->quality <= 10 ?
      MAX_ZOPFLI_LEN_QUALITY_10 :
      MAX_ZOPFLI_LEN_QUALITY_11;
}

/* Number of best candidates to evaluate to expand zopfli chain. */
static BROTLI_INLINE size_t MaxZopfliCandidates(
  const BrotliEncoderParams* settings) {
  return settings->quality <= 10 ? 1 : 5;
}

static BROTLI_INLINE void SanitizeParams(BrotliEncoderParams* settings) {
  settings->quality = BROTLI_MIN(int, BROTLI_MAX_QUALITY,
      BROTLI_MAX(int, BROTLI_MIN_QUALITY, settings->quality));
  if (settings->lgwin < kBrotliMinWindowBits) {
    settings->lgwin = kBrotliMinWindowBits;
  } else if (settings->lgwin > kBrotliMaxWindowBits) {
    settings->lgwin = kBrotliMaxWindowBits;
  }
}

/* Returns optimized lg_block value. */
static BROTLI_INLINE int ComputeLgBlock(const BrotliEncoderParams* settings) {
  int lgblock = settings->lgblock;
  if (settings->quality == FAST_ONE_PASS_COMPRESSION_QUALITY ||
      settings->quality == FAST_TWO_PASS_COMPRESSION_QUALITY) {
    lgblock = settings->lgwin;
  } else if (settings->quality < MIN_QUALITY_FOR_BLOCK_SPLIT) {
    lgblock = 14;
  } else if (lgblock == 0) {
    lgblock = 16;
    if (settings->quality >= 9 && settings->lgwin > lgblock) {
      lgblock = BROTLI_MIN(int, 18, settings->lgwin);
    }
  } else {
    lgblock = BROTLI_MIN(int, kBrotliMaxInputBlockBits,
        BROTLI_MAX(int, kBrotliMinInputBlockBits, lgblock));
  }
  return lgblock;
}

/* Returns log2 of the size of main ring buffer area.
   Allocate at least lgwin + 1 bits for the ring buffer so that the newly
   added block fits there completely and we still get lgwin bits and at least
   read_block_size_bits + 1 bits because the copy tail length needs to be
   smaller than ringbuffer size. */
static BROTLI_INLINE int ComputeRbBits(const BrotliEncoderParams* settings) {
  return 1 + BROTLI_MAX(int, settings->lgwin, settings->lgblock);
}

static BROTLI_INLINE size_t MaxMetablockSize(
    const BrotliEncoderParams* settings) {
  int bits = BROTLI_MIN(int, ComputeRbBits(settings), kBrotliMaxInputBlockBits);
  return (size_t)1 << bits;
}

/* When searching for backward references and have not seen matches for a long
   time, we can skip some match lookups. Unsuccessful match lookups are very
   expensive and this kind of a heuristic speeds up compression quite a lot.
   At first 8 byte strides are taken and every second byte is put to hasher.
   After 4x more literals stride by 16 bytes, every put 4-th byte to hasher.
   Applied only to qualities 2 to 9. */
static BROTLI_INLINE size_t LiteralSpreeLengthForSparseSearch(
    const BrotliEncoderParams* settings) {
  return settings->quality < 9 ? 64 : 512;
}

static BROTLI_INLINE int ChooseHasher(const BrotliEncoderParams* settings) {
  if (settings->quality > 9) {
    return 10;
  } else if (settings->quality < 5) {
    return settings->quality;
  } else if (settings->lgwin <= 16) {
    return settings->quality < 7 ? 40 : settings->quality < 9 ? 41 : 42;
  }
  return settings->quality;
}

#endif  /* BROTLI_ENC_QUALITY_H_ */
