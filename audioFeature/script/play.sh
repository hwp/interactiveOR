#!/bin/sh

aplay -fS16_LE -r8000 -c 1 `head -$1 ../data/trimlist | tail -1`

