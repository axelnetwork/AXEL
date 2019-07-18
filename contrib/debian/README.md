
Debian
====================
This directory contains files used to package axeld/axel-qt
for Debian-based Linux systems. If you compile axeld/axel-qt yourself, there are some useful files here.

## axel: URI support ##


axel-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install axel-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your axelqt binary to `/usr/bin`
and the `../../share/pixmaps/axel128.png` to `/usr/share/pixmaps`

axel-qt.protocol (KDE)
