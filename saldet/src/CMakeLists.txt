# Salient Object Detection
  
add_executable(saldet saldet.cpp segment-image.cpp)
target_link_libraries(saldet ${OpenCV_LIBS})

