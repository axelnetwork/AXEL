#!/bin/sh
#----------------------------------------------
if [ $# == 0 ]; then
  echo "Usage: patchFramework /path/to/framework"
  exit
fi

#Identify framework
framework=$1
if [ ! -d "$framework" ]; then
  echo "Error, no such framework $framework"
  exit
fi

#Identify name of framework
name=`basename "$framework" | cut -d'.' -f1`
echo "Patching $name..."

#Switch to framework directory
cd "$framework"

#Create a symlink to the current version
cd Versions
for v in *; do 
  version=$v
  rm -fr Current;
  ln -s $v Current;
  break;
done
cd ..
  
#Rename a Contents folder to Resources (e.g. qntp.framework)
if [ -d Contents ]; then
  mv Contents Resources
fi

#Move Resources to the current version directory
mv Resources Versions/Current
  
#create Info.plist if it does not exist (Qt4)
plistFilename="Versions/Current/Resources/Info.plist"
if [ ! -e $plistFilename ]; then
  touch $plistFilename
  echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" > $plistFilename
  echo "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">" >> $plistFilename
  echo "<plist version=\"1.0\">" >> $plistFilename
  echo "<dict>" >> $plistFilename
  echo "        <key>CFBundlePackageType</key>" >> $plistFilename
  echo "        <string>FMWK</string>" >> $plistFilename
  echo "        <key>CFBundleShortVersionString</key>" >> $plistFilename
  echo "        <string>$version</string>" >> $plistFilename
  echo "        <key>CFBundleGetInfoString</key>" >> $plistFilename
  echo "        <string>Created by Qt/QMake</string>" >> $plistFilename
  echo "        <key>CFBundleSignature</key>" >> $plistFilename
  echo "        <string>????</string>" >> $plistFilename
  echo "        <key>CFBundleExecutable</key>" >> $plistFilename
  echo "        <string>$name</string>" >> $plistFilename
  echo "</dict>" >> $plistFilename
  echo "</plist>" >> $plistFilename
fi

#Setup top level required symlinks

#qntp.framework provides this, other frameworks usually don't
if [ ! -e $name ]; then
  ln -s Versions/Current/$name     $name
fi

ln -s Versions/Current/Resources Resources
