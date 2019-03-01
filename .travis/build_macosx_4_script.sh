#!/usr/bin/env bash
#
# Copyright (c) 2018 The Bitcoin Core developers
# Copyright (c) 2018-2019 The Bunnycoin developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

TRAVIS_COMMIT_LOG=$(git log --format=fuller -1)
export TRAVIS_COMMIT_LOG

VERSION_NUMBER=`cat CMakeLists.txt | grep "^    VERSION" | sed -E "s/^    VERSION ([0-9]+\.[0-9]+\.[0-9]+)/\1/g" | tr -d '\r'`

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

PACKAGE_DIR="bunnycoin-macosx-${VERSION_NAME}"

BEGIN_FOLD cmake
cmake -H. -Bbuild -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=./${PACKAGE_DIR} \
    -DCMAKE_PREFIX_PATH=/usr/local/opt/qt \
    -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl \
    -DBOOST_ROOT=/usr/local/opt/boost
END_FOLD

BEGIN_FOLD build
cmake --build build
END_FOLD

BEGIN_FOLD test
cmake --build build --target test
END_FOLD

BEGIN_FOLD install
cmake --build build --target install
END_FOLD

for app in ./${PACKAGE_DIR}/*.app
do
    BEGIN_FOLD deploy $(basename $app)
    /usr/local/opt/qt/bin/macdeployqt ${app} -dmg && rm -r ${app}
    END_FOLD
done

BEGIN_FOLD package
PACKAGE_FILE=${PACKAGE_DIR}.zip
zip -r ${PACKAGE_FILE} ${PACKAGE_DIR}
END_FOLD

BEGIN_FOLD upload
curl -v -T ${PACKAGE_FILE} -u${BINTRAY_USER}:${BINTRAY_API_KEY} https://api.bintray.com/content/bunnycoin/downloads/${BINTRAY_PACKAGE}/${BINTRAY_VERSION}/${PACKAGE_FILE}
END_FOLD
