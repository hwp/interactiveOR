// sequence_tag.h
// sequence with tagged data.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2015, All rights reserved.

#ifndef SEQUENCE_TAG_H_
#define SEQUENCE_TAG_H_

#include "sequence.h"
#include "tagged.h"

#define SEQ_TAG_TRAIN ((tagged_train_func) seq_tag_train)

double seq_tag_prob(seq_hmm_attr* attr, seq_t* seq);

/**
 * @warning returned pointer should be freed after use.
 */
tagged_model* seq_tag_train(tagged_dataset* train_data,
    unsigned int tag, seq_hmm_train_param* param);

char* seq_tag_description(seq_hmm_train_param* param);

#endif  // SEQUENCE_TAG_H_

