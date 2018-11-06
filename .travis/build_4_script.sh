#!/usr/bin/env bash
#
# Copyright (c) 2018 The Bitcoin Core developers
# Copyright (c) 2018 The Bunnycoin developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

TRAVIS_COMMIT_LOG=$(git log --format=fuller -1)
export TRAVIS_COMMIT_LOG

mkdir build
cd build || (echo "could not enter build directory"; exit 1)

BEGIN_FOLD qmake
DOCKER_EXEC qmake .. USE_DBUS=1 USE_QRCODE=1 USE_UPNP=1
END_FOLD

BEGIN_FOLD build
DOCKER_EXEC make
END_FOLD

