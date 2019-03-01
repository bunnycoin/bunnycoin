#!/usr/bin/env bash
#
# Copyright (c) 2018 The Bitcoin Core developers
# Copyright (c) 2018-2019 The Bunnycoin developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

TRAVIS_COMMIT_LOG=$(git log --format=fuller -1)
export TRAVIS_COMMIT_LOG

VERSION_NUMBER=`cat CMakeLists.txt | grep "^    VERSION" | sed -E "s/\s+VERSION\s+([[:digit:]])\s*/\1/g" | sed 's/\r//'`

if [ "v$VERSION_NUMBER" == "$TRAVIS_BRANCH" ]
then
    VERSION_NAME="${VERSION_NUMBER}"
    BINTRAY_VERSION="${VERSION_NUMBER}"
    BINTRAY_PACKAGE="releases"
else
    VERSION_NAME="${VERSION_NUMBER}-${TRAVIS_BRANCH}-${TRAVIS_BUILD_NUMBER}"
    BINTRAY_VERSION="${TRAVIS_BRANCH}"
    BINTRAY_PACKAGE="ci"
fi

mkdir build

if [[ $HOST = *-mingw32 ]]; then
    if [[ $HOST = x86_64-w64-mingw32 ]]; then
        PACKAGE_DIR="bunnycoin-windows-64bit-${VERSION_NAME}"
    elif [[ $HOST = i686-w64-mingw32 ]]; then
        PACKAGE_DIR="bunnycoin-windows-32bit-${VERSION_NAME}"
    else
        echo "unsupported architecture $HOST"
        exit 1
    fi

    BEGIN_FOLD dependencies
    cd build || (echo "could not enter build directory"; exit 1)
    DOCKER_EXEC wget https://bintray.com/bunnycoin/downloads/download_file?file_path=bunnycoin-deps-20181126-${HOST}.tar.xz -nv -O dependencies.tar.xz
    DOCKER_EXEC tar -xf dependencies.tar.xz
    DOCKER_EXEC wget https://bintray.com/bunnycoin/downloads/download_file?file_path=${HOST}-toolchain.cmake -nv -O ${HOST}-toolchain.cmake
    cd ..
    END_FOLD

    BEGIN_FOLD cmake
    DOCKER_EXEC cmake -H. -Bbuild -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=./${PACKAGE_DIR} \
        -DCMAKE_TOOLCHAIN_FILE=${HOST}-toolchain.cmake \
        -DSTATIC_BUILD=ON \
        -DUNIT_TESTS=OFF
    END_FOLD

    BEGIN_FOLD build
    DOCKER_EXEC cmake --build build
    END_FOLD

    BEGIN_FOLD install
    DOCKER_EXEC cmake --build build --target install
    END_FOLD

    BEGIN_FOLD package
    PACKAGE_FILE=${PACKAGE_DIR}.zip
    zip -r ${PACKAGE_FILE} ${PACKAGE_DIR}
    END_FOLD
else
    PACKAGE_DIR="bunnycoin-${VERSION_NAME}"

    BEGIN_FOLD cmake
    DOCKER_EXEC cmake -H. -Bbuild -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=${PACKAGE_DIR}/usr
    END_FOLD

    BEGIN_FOLD build
    DOCKER_EXEC cmake --build build
    END_FOLD

    BEGIN_FOLD test
    DOCKER_EXEC cmake --build build --target test
    END_FOLD

    BEGIN_FOLD install
    DOCKER_EXEC cmake --build build --target install
    END_FOLD

    BEGIN_FOLD package
    DEB_DIR=${PACKAGE_DIR}/DEBIAN
    DEB_CONTROL_FILE=${DEB_DIR}/control
    PACKAGE_FILE=${PACKAGE_DIR}.deb
    DOCKER_EXEC mkdir -p ${DEB_DIR}
    DOCKER_EXEC cp .travis/deb-control-bionic ${DEB_CONTROL_FILE}
    DEB_BUILD_ARCH=`DOCKER_EXEC dpkg-architecture -q DEB_BUILD_ARCH`
    DOCKER_EXEC sed "s/DEB_BUILD_ARCH/${DEB_BUILD_ARCH}/g" -i ${DEB_CONTROL_FILE}
    DOCKER_EXEC sed "s/DEB_VERSION/${VERSION_NAME}/g" -i ${DEB_CONTROL_FILE}
    DOCKER_EXEC dpkg-deb --build ${PACKAGE_DIR}
    END_FOLD
fi

BEGIN_FOLD upload
curl -v -T ${PACKAGE_FILE} -u${BINTRAY_USER}:${BINTRAY_API_KEY} https://api.bintray.com/content/bunnycoin/downloads/${BINTRAY_PACKAGE}/${BINTRAY_VERSION}/${PACKAGE_FILE}
END_FOLD
