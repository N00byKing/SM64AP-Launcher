#!/usr/bin/env bash

# This script is called when cloning a repository
# If you don't know what that means, don't edit it!

echo Changing Directory
cd ${BUILD_TARGET_DIR}
echo Starting Download
git clone --recursive --depth=1 -b ${BUILD_BRANCH_LINK} ${BUILD_REPO_LINK} 
