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
VERSION_NAME="${VERSION_NUMBER}-${TRAVIS_BRANCH}-${TRAVIS_BUILD_NUMBER}"
PACKAGE_NAME="bunnycoin-${VERSION_NAME}"
DEB_DIR=${PACKAGE_NAME}/DEBIAN
DEB_CONTROL_FILE=${DEB_DIR}/control

mkdir build
cd build || (echo "could not enter build directory"; exit 1)

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

BEGIN_FOLD uploading
curl -T ${PACKAGE_NAME}.deb -u${BINTRAY_USER}:${BINTRAY_API_KEY} https://api.bintray.com/content/bunnycoin/bunnycoin/bunnycoin/${VERSION_NAME}/${PACKAGE_NAME}.deb
END_FOLD

