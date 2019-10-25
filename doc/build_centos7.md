Prepare the environment for Compile axel-coin on Centos 7.4
---------------------

Build requirements:
```
yum -y install automake openssl openssl-devel libevent libevent-devel
```

install Berkeley DB:
```
wget 'http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz'
cd db-4.8.30.NC/build_unix/
../dist/configure --enable-cxx --disable-shared --with-pic --prefix=/usr
make
make install
```

install boost:
```
yum install boost boost-devel boost-doc
```

install QT5:
```
yum install protobuf protobuf-devel qrencode qrencode-devel qt5-qtbase qt5-qtbase-devel qt5-linguist
```

To Build
---------------------

```bash
./autogen.sh
./configure
make
make install # optional
```
