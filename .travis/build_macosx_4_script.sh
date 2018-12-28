#!/usr/bin/env bash
#
# Copyright (c) 2018 The Bitcoin Core developers
# Copyright (c) 2018 The Bunnycoin developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

TRAVIS_COMMIT_LOG=$(git log --format=fuller -1)
export TRAVIS_COMMIT_LOG

VERSION_NUMBER=`cat CMakeLists.txt | grep "^    VERSION" | sed -E "s/\s+VERSION\s+([[:digit:]])\s*/\1/g" | sed 's/\r//'`

if [ "v$VERSION_NUMBER" == "$TRAVIS_BRANCH" ]
then
    VERSION_NAME="${VERSION_NUMBER}"
else
    VERSION_NAME="${VERSION_NUMBER}-${TRAVIS_BRANCH}-${TRAVIS_BUILD_NUMBER}"
fi

mkdir build
cd build || (echo "could not enter build directory"; exit 1)

PACKAGE_DIR="bunnycoin-${VERSION_NAME}"

BEGIN_FOLD cmake
cmake -GNinja -DCMAKE_INSTALL_PREFIX=../${PACKAGE_DIR} -DCMAKE_PREFIX_PATH=/usr/local/opt/qt -DOPENSSL_ROOT_DIR=/usr/local/openssl ..
END_FOLD

BEGIN_FOLD build
ninja
END_FOLD

BEGIN_FOLD install
ninja install
END_FOLD

BEGIN_FOLD upload
curl -T ${PACKAGE_FILE} -u${BINTRAY_USER}:${BINTRAY_API_KEY} https://api.bintray.com/content/bunnycoin/bunnycoin/bunnycoin/${VERSION_NAME}/${PACKAGE_FILE}
END_FOLD
