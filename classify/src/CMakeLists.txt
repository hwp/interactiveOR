# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2014, All rights reserved.

add_executable(classify_test classify_test.c classify.c)
target_link_libraries(classify_test ${GSL_LIBRARIES})

add_executable(seq_test seq_test.c classify.c sequence.c debug_utils.c)
target_link_libraries(seq_test ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES})

add_executable(bimodal_test bimodal_test.c classify.c sequence.c debug_utils.c)
target_link_libraries(bimodal_test ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES})

add_executable(stab_test stab_test.c sequence.c debug_utils.c)
target_link_libraries(stab_test ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES})

add_executable(hmm_test hmm_test.c classify.c tagged.c sequence.c
                        sequence_tag.c debug_utils.c)
target_link_libraries(hmm_test ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES})

add_executable(svm_test svm_test.c classify.c tagged.c svm_tag.c
                        sequence.c debug_utils.c)
target_link_libraries(svm_test ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES}
                               ${LIBSVM_LIBRARIES})

