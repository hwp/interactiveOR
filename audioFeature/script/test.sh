#!/bin/sh

svm-train -t 4 -c 100 ${1}train.libsvm ${1}.model
svm-predict ${1}test.libsvm ${1}.model result

