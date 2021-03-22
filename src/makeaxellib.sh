#!/bin/sh
if [ $# != 1 ] ; then 
echo "USAGE: $0 linux/macos/windows" 
echo " e.g.: $0 linux" 
exit 1; 
fi 

PWD=`pwd`
SHELL_FOLDER=$PWD/$(dirname $0)


echo "++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "Plesae run script with linux/macos/windows parameter"
echo "This scrip is verified on Linux ubuntu 1804/MAC OS X 10.14/Windows 10"
echo "On other OS"
echo "Please check and make changes to this script for system compatibility"
echo "++++++++++++++++++++++++++++++++++++++++++++++++++"

echo $1

cd ${SHELL_FOLDER}

RESC_LIB=./resck/libaxel_resck_$1.a
RESC_MAKEFILE=./resck/Makefile
if [ -f ${RESC_MAKEFILE} ];then
	cd resck
	make platform=$1
	make platform=$1 clean
else
	if [ -f ${RESC_LIB} ];then
	    cp ${RESC_LIB} ./libaxel_resck.a
	fi
fi

cd ${SHELL_FOLDER}

if [ -f libaxel_resck.a ];then
    echo "File libaxel_rescj.a already exist"
else
    echo "++++ERROR:Please contact AXEL to retrieve file libaxel_resck.a"
fi

cd ${PWD}

