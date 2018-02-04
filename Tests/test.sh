#!/usr/bin/env bash

# These tests will attempt to process pre-generated patterns.

# 256 pixels of Blue from 255 to 0
../Histogram_v2 -f 1_256R000G000B255000.jpg -o FadeBlueFrom255To0.txt

# 256 pixels of Green from 255 to 0
../Histogram_v2 -f 1_256R000G255000B000.jpg -o FadeGreenFrom255To0.txt

# 256 pixels of Red from 255 to 0
../Histogram_v2 -f 1_256R255000G000B000.jpg -o FadeRedFrom255To0.txt

# NearMap sample file. The sample file was removed to minimize file size.
# ../Histogram_v2 -f DSC_0278.JPG -o NearMapSample.txt

# Ultralarge image to stress test multi threading (there is an R123G123B123 dot hidden somewhere in the image)
../Histogram_v2 -f ultralargetestR123G123B123.jpg -o ultralargetestR123G123B123.txt 
