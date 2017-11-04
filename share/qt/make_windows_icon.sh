#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/trollocoin.png
ICON_DST=../../src/qt/res/icons/trollocoin.ico
convert ${ICON_SRC} -resize 16x16 trollocoin-16.png
convert ${ICON_SRC} -resize 32x32 trollocoin-32.png
convert ${ICON_SRC} -resize 48x48 trollocoin-48.png
convert trollocoin-48.png trollocoin-32.png trollocoin-16.png ${ICON_DST}

