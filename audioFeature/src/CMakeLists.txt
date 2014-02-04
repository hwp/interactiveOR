# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2014, All rights reserved.

add_executable(gsltest gsltest.c)
target_link_libraries(gsltest ${GSL_LIBRARIES})

add_executable(stfttest stfttest.c stft.c utils.c)
target_link_libraries(stfttest ${GSL_LIBRARIES} ${ALSA_LIBRARIES})

add_executable(istfttest istfttest.c stft.c utils.c)
target_link_libraries(istfttest ${GSL_LIBRARIES} ${ALSA_LIBRARIES})

add_executable(somtest somtest.c som.c)
target_link_libraries(somtest ${GSL_LIBRARIES})
if(UNIX)
  target_link_libraries(somtest m)
endif(UNIX)

add_executable(audiosom audiosom.c som.c stft.c utils.c)
target_link_libraries(audiosom ${GSL_LIBRARIES} ${ALSA_LIBRARIES})
if(UNIX)
  target_link_libraries(audiosom m)
endif(UNIX)

add_executable(audiomap audiomap.c som.c stft.c utils.c)
target_link_libraries(audiomap ${GSL_LIBRARIES} ${ALSA_LIBRARIES})
if(UNIX)
  target_link_libraries(audiomap m)
endif(UNIX)

add_executable(specsubtest specsubtest.c spectral_sub.c stft.c utils.c)
target_link_libraries(specsubtest ${GSL_LIBRARIES} ${ALSA_LIBRARIES})
if(UNIX)
  target_link_libraries(specsubtest m)
endif(UNIX)
