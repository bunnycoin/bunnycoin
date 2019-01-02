#!/usr/bin/env bash
#
# Copyright (c) 2018 The Bitcoin Core developers
# Copyright (c) 2018 The Bunnycoin developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

echo "Getting git log"
TRAVIS_COMMIT_LOG=$(git log --format=fuller -1)
export TRAVIS_COMMIT_LOG
echo "TRAVIS_COMMIT_LOG=${TRAVIS_COMMIT_LOG}"

echo "Getting version number"
VERSION_NUMBER=`cat CMakeLists.txt | grep "^    VERSION" | sed -E "s/^    VERSION ([0-9]+\.[0-9]+\.[0-9]+)/\1/g" | tr -d '\r'`
echo "VERSION_NUMBER=${VERSION_NUMBER}"

echo "Getting version name"
if [ "v$VERSION_NUMBER" == "$TRAVIS_BRANCH" ]
then
    VERSION_NAME="${VERSION_NUMBER}"
else
    VERSION_NAME="${VERSION_NUMBER}-${TRAVIS_BRANCH}-${TRAVIS_BUILD_NUMBER}"
fi
echo "VERSION_NAME=${VERSION_NAME}"

echo "Package dir variable"
PACKAGE_DIR="bunnycoin-${VERSION_NAME}"
echo "PACKAGE_DIR=${PACKAGE_DIR}"

echo "Invoking cmake"
BEGIN_FOLD cmake
cmake -H. -Bbuild -GNinja -DCMAKE_INSTALL_PREFIX=./${PACKAGE_DIR} -DCMAKE_PREFIX_PATH=/usr/local/opt/qt -DOPENSSL_ROOT_DIR=/usr/local/openssl
END_FOLD

BEGIN_FOLD build
cmake --build build
END_FOLD

BEGIN_FOLD install
cmake --build build --target install
END_FOLD

BEGIN_FOLD upload
curl -T ${PACKAGE_FILE} -u${BINTRAY_USER}:${BINTRAY_API_KEY} https://api.bintray.com/content/bunnycoin/bunnycoin/bunnycoin/${VERSION_NAME}/${PACKAGE_FILE}
END_FOLD
