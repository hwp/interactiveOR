# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2014, All rights reserved.

add_executable(gsltest gsltest.c)
target_link_libraries(gsltest ${GSL_LIBRARIES})

add_executable(stfttest stfttest.c stft.c utils.c)
target_link_libraries(stfttest ${GSL_LIBRARIES} ${ALSA_LIBRARIES})

add_executable(somtest somtest.c som.c)
target_link_libraries(somtest m ${GSL_LIBRARIES})

add_executable(audiosom audiosom.c som.c stft.c utils.c)
target_link_libraries(audiosom m ${GSL_LIBRARIES} ${ALSA_LIBRARIES})

