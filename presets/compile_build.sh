#!/usr/bin/env bash

# This script is called when cloning a repository
# If you don't know what that means, don't edit it!

cd "${BUILD_TARGET_DIR}/${BUILD_NAME}"
make -j$(nproc)
