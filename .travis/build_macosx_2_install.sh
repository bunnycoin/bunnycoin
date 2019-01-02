#!/usr/bin/env bash
#
# Copyright (c) 2018 The Bitcoin Core developers
# Copyright (c) 2018 The Bunnycoin developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

brew upgrade boost cmake openssl || true
brew install qrencode miniupnpc qt berkeley-db ninja || true
