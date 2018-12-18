#!/usr/bin/env bash
#
# Copyright (c) 2018 The Bitcoin Core developers
# Copyright (c) 2018 The Bunnycoin developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

TRAVIS_COMMIT_LOG=$(git log --format=fuller -1)
export TRAVIS_COMMIT_LOG

VERSION_NUMBER=`cat bunnycoin-qt.pro | grep VERSION\ = | sed "s/VERSION\ = //g"`

if [ "v$VERSION_NUMBER" == "$TRAVIS_BRANCH" ]
then
    VERSION_NAME="${VERSION_NUMBER}"
else
    VERSION_NAME="${VERSION_NUMBER}-${TRAVIS_BRANCH}-${TRAVIS_BUILD_NUMBER}"
fi

mkdir build
cd build || (echo "could not enter build directory"; exit 1)

if [[ $HOST = *-mingw32 ]]; then
    if [[ $HOST = x86_64-w64-mingw32 ]]; then
        PACKAGE_NAME="bunnycoin-windows-64bit-${VERSION_NAME}"
    else
        PACKAGE_NAME="bunnycoin-windows-32bit-${VERSION_NAME}"
    fi

    BEGIN_FOLD dependencies
    DOCKER_EXEC wget https://bintray.com/bunnycoin/bunnycoin/download_file?file_path=bunnycoin-deps-20181126-x86_64-w64-mingw32.tar.xz -nv -O dependencies.tar.xz
    DOCKER_EXEC tar -xf dependencies.tar.xz
    DOCKER_EXEC wget https://bintray.com/bunnycoin/bunnycoin/download_file?file_path=x86_64-w64-mingw32-toolchain.cmake -nv -O x86_64-w64-mingw32-toolchain.cmake
    END_FOLD

    BEGIN_FOLD cmake
    DOCKER_EXEC cmake -DCMAKE_TOOLCHAIN_FILE=x86_64-w64-mingw32-toolchain.cmake -GNinja -DCMAKE_INSTALL_PREFIX=../${PACKAGE_NAME} ..
    END_FOLD

    BEGIN_FOLD build
    DOCKER_EXEC ninja $MAKEJOBS install
    END_FOLD

    BEGIN_FOLD external dependencies
    DOCKER_EXEC install -m 755 -D -t ../${PACKAGE_NAME} /usr/i686-w64-mingw32/lib/libwinpthread-1.dll
    DOCKER_EXEC install -m 755 -D -t ../${PACKAGE_NAME} /usr/lib/gcc/x86_64-w64-mingw32/7.3-win32/libgcc_s_seh-1.dll
    DOCKER_EXEC install -m 755 -D -t ../${PACKAGE_NAME} /usr/lib/gcc/x86_64-w64-mingw32/7.3-win32/libstdc++-6.dll
    END_FOLD

    BEGIN_FOLD package
    cd ..
    zip -r ${PACKAGE_NAME}.zip ${PACKAGE_NAME}
    END_FOLD

    BEGIN_FOLD upload
    curl -T ${PACKAGE_NAME}.zip -u${BINTRAY_USER}:${BINTRAY_API_KEY} https://api.bintray.com/content/bunnycoin/bunnycoin/bunnycoin/${VERSION_NAME}/${PACKAGE_NAME}.zip
    END_FOLD
else
    PACKAGE_NAME="bunnycoin-${VERSION_NAME}"
    DEB_DIR=${PACKAGE_NAME}/DEBIAN
    DEB_CONTROL_FILE=${DEB_DIR}/control

    BEGIN_FOLD qmake
    DOCKER_EXEC qmake .. USE_DBUS=1 USE_QRCODE=1 USE_UPNP=1 PREFIX=$PACKAGE_NAME
    END_FOLD

    BEGIN_FOLD build
    DOCKER_EXEC make $MAKEJOBS
    END_FOLD

    BEGIN_FOLD install
    DOCKER_EXEC install -m 755 -D -t ../${PACKAGE_NAME}/bin ./bunnycoin-qt
    DOCKER_EXEC install -m 644 -D ../contrib/debian/bunnycoin-qt.desktop.desktop ../${PACKAGE_NAME}/share/applications/bunnycoin-qt.desktop
    DOCKER_EXEC install -m 644 -D -t ../${PACKAGE_NAME}/share/pixmaps ../share/pixmaps/bunnycoin64.png
    DOCKER_EXEC install -m 644 -D -t ../${PACKAGE_NAME}/share/pixmaps ../share/pixmaps/bunnycoin128.png
    DOCKER_EXEC install -m 644 -D -t ../${PACKAGE_NAME}/share/pixmaps ../share/pixmaps/bunnycoin256.png
    DOCKER_EXEC install -m 644 -D -t ../${PACKAGE_NAME}/share/bunnycoin ../release/bunnycoin.conf
    END_FOLD

    BEGIN_FOLD package
    cd ..
    DOCKER_EXEC mkdir -p ${DEB_DIR}
    DOCKER_EXEC cp .travis/deb-control-bionic ${DEB_CONTROL_FILE}
    DEB_BUILD_ARCH=`DOCKER_EXEC dpkg-architecture -q DEB_BUILD_ARCH`
    DOCKER_EXEC sed "s/DEB_BUILD_ARCH/${DEB_BUILD_ARCH}/g" -i ${DEB_CONTROL_FILE}
    DOCKER_EXEC sed "s/DEB_VERSION/${VERSION_NAME}/g" -i ${DEB_CONTROL_FILE}
    DOCKER_EXEC dpkg-deb --build ${PACKAGE_NAME}
    END_FOLD

    BEGIN_FOLD upload
    curl -T ${PACKAGE_NAME}.deb -u${BINTRAY_USER}:${BINTRAY_API_KEY} https://api.bintray.com/content/bunnycoin/bunnycoin/bunnycoin/${VERSION_NAME}/${PACKAGE_NAME}.deb
    END_FOLD
fi
