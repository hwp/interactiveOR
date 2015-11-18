Interactive Object Recognition
=============

This is a library of functions for learning the visual and audio features for object recognition.

## Directory Structure
* **`audioFeature`**: (deprecated) Some functions for extracting STFT from audio files, learning SOM and classifying with SVM.
* **`classify`**: The main part of this library.
  * **`src`**: Source codes.
  * **`scripts`**: Bash scripts for recording data, extracting feature and refining results.
* **`data/old/scripts`**: (deprecated) Some scripts for extracting audio features.
* **`saldet`**: (deprecated) Saliency detection.

## Installation Guide
### Dependency
Make sure you have these dependent libraries installed and can be found by pkg-config:

* [notghmm](https://github.com/hwp/notghmm)
* [GSL](http://www.gnu.org/software/gsl/)
* [LibSVM](https://www.csie.ntu.edu.tw/~cjlin/libsvm/)

For building the library you also need:

* [CMake](http://www.cmake.org/)

### Build
The following commands builds the library:
```
mkdir build
cd build
cmake ..
make
```

## Data Recording
The bash script `classify/scripts/record.sh` records video data from the designated webcam and microphone. The result will be saved to the working directory.

On default, the script uses the `hw:0,0` as the audio capture device and `/dev/video0` as the video capture device. To change the default setting, you can modify the values of `audio_device` and `video_device` on the 3rd and 4th lines.

Upon start, you need to type in the name of the object. The name will be the prefix of the recorded video files. Entering an empty name will terminate the program.

## Feature Extraction

## Learning
