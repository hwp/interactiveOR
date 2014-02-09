#!/bin/sh

../script/calkernel.py -c 6 -r 6 ${1}train | sed 's/^\([0-9]*\) \([A-Z]*\) \(.*\)$/\1 \3/' > ${1}train.libsvm
../script/calkernel.py -c 6 -r 6 ${1}train ${1}test | sed 's/^\([0-9]*\) \([A-Z]*\) \(.*\)$/\1 \3/' > ${1}test.libsvm
svm-train -t 4 -c 100 ${1}train.libsvm ${1}.model
svm-predict ${1}test.libsvm ${1}.model result

