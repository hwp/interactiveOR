# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2014, All rights reserved.

add_executable(hello hello.c)
target_link_libraries(hello ${NOTGHMM_LIBRARIES} ${GSL_LIBRARIES})

add_executable(classify_test classify_test.c classify.c)

