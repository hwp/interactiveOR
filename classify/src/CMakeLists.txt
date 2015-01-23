# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2014, All rights reserved.

add_executable(hello hello.c)
target_link_libraries(hello ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES})

add_executable(classify_test classify_test.c classify.c)
target_link_libraries(classify_test ${GSL_LIBRARIES})

add_executable(seq_test seq_test.c classify.c sequence.c debug_utils.c)
target_link_libraries(seq_test ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES})

add_executable(stab_test stab_test.c sequence.c debug_utils.c)
target_link_libraries(stab_test ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES})

add_executable(tag_test tag_test.c classify.c tagged.c sequence.c
  sequence_tag.c debug_utils.c)
target_link_libraries(tag_test ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES})

