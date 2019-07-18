#!/bin/bash
# create multiresolution windows icon
#mainnet
ICON_SRC=../../src/qt/res/icons/axel.png
ICON_DST=../../src/qt/res/icons/axel.ico
convert ${ICON_SRC} -resize 16x16 axel-16.png
convert ${ICON_SRC} -resize 32x32 axel-32.png
convert ${ICON_SRC} -resize 48x48 axel-48.png
convert axel-16.png axel-32.png axel-48.png ${ICON_DST}
#testnet
ICON_SRC=../../src/qt/res/icons/axel_testnet.png
ICON_DST=../../src/qt/res/icons/axel_testnet.ico
convert ${ICON_SRC} -resize 16x16 axel-16.png
convert ${ICON_SRC} -resize 32x32 axel-32.png
convert ${ICON_SRC} -resize 48x48 axel-48.png
convert axel-16.png axel-32.png axel-48.png ${ICON_DST}
rm axel-16.png axel-32.png axel-48.png
