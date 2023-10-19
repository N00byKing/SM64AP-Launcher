#!/usr/bin/env bash

# This script is called when cloning a repository
# If you don't know what that means, don't edit it!

cd ${BUILD_TARGET_DIR}
git clone --recursive --depth=1 -b "${BUILD_BRANCH_NAME}" --  "${BUILD_REPO_LINK}" "${BUILD_NAME}"
