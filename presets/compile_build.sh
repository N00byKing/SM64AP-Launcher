#!/usr/bin/env bash

# This script is called when cloning a repository
# If you don't know what that means, don't edit it!

FULL_BUILD_PATH="${BUILD_TARGET_DIR}/${BUILD_NAME}"

cp  "${BUILD_ROM}" "${FULL_BUILD_PATH}"
cd "${FULL_BUILD_PATH}"
make -j$(nproc) VERSION="${BUILD_REGION}"
