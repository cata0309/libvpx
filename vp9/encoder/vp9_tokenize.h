/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_ENCODER_VP9_TOKENIZE_H_
#define VP9_ENCODER_VP9_TOKENIZE_H_

#include "vp9/common/vp9_entropy.h"

#include "vp9/encoder/vp9_block.h"
#include "vp9/encoder/vp9_treewriter.h"

#ifdef __cplusplus
extern "C" {
#endif

void vp9_tokenize_initialize();

#define EOSB_TOKEN 127     // Not signalled, encoder only

#if CONFIG_VP9_HIGHBITDEPTH
  typedef int32_t EXTRABIT;
#else
  typedef int16_t EXTRABIT;
#endif


typedef struct {
  int16_t token;
  EXTRABIT extra;
} TOKENVALUE;

typedef struct {
  const vp9_prob *context_tree;
  EXTRABIT extra;
  uint8_t token;
  uint8_t skip_eob_node;
} TOKENEXTRA;

extern const vp9_tree_index vp9_coef_tree[];
extern const vp9_tree_index vp9_coef_con_tree[];
extern struct vp9_token vp9_coef_encodings[];

int vp9_is_skippable_in_plane(MACROBLOCK *x, BLOCK_SIZE bsize, int plane);
int vp9_has_high_freq_in_plane(MACROBLOCK *x, BLOCK_SIZE bsize, int plane);

struct VP9_COMP;
struct ThreadData;

void vp9_tokenize_sb(struct VP9_COMP *cpi, struct ThreadData *td,
                     TOKENEXTRA **t, int dry_run, BLOCK_SIZE bsize);

extern const int16_t *vp9_dct_value_cost_ptr;
/* TODO: The Token field should be broken out into a separate char array to
 *  improve cache locality, since it's needed for costing when the rest of the
 *  fields are not.
 */
extern const TOKENVALUE *vp9_dct_value_tokens_ptr;
extern const TOKENVALUE *vp9_dct_cat_lt_10_value_tokens;
#if CONFIG_VP9_HIGHBITDEPTH
extern const int16_t *vp9_dct_value_cost_high10_ptr;
extern const TOKENVALUE *vp9_dct_value_tokens_high10_ptr;
extern const int16_t *vp9_dct_value_cost_high12_ptr;
extern const TOKENVALUE *vp9_dct_value_tokens_high12_ptr;
#endif  // CONFIG_VP9_HIGHBITDEPTH

static INLINE void vp9_get_token_extra(int v, int16_t *token, EXTRABIT *extra) {
  if (v >= CAT6_MIN_VAL || v <= -CAT6_MIN_VAL) {
    *token = CATEGORY6_TOKEN;
    if (v >= CAT6_MIN_VAL)
      *extra = 2 * v - 2 * CAT6_MIN_VAL;
    else
      *extra = -2 * v - 2 * CAT6_MIN_VAL + 1;
    return;
  }
  *token = vp9_dct_cat_lt_10_value_tokens[v].token;
  *extra = vp9_dct_cat_lt_10_value_tokens[v].extra;
}
static INLINE int16_t vp9_get_token(int v) {
  if (v >= CAT6_MIN_VAL || v <= -CAT6_MIN_VAL)
    return 10;
  return vp9_dct_cat_lt_10_value_tokens[v].token;
}


#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // VP9_ENCODER_VP9_TOKENIZE_H_
