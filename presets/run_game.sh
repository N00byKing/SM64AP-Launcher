#!/usr/bin/env bash

# This script is called when a build is selected to play
# Can be edited to include 'permanent' launch options

FULL_BUILD_PATH="${BUILD_TARGET_DIR}/${BUILD_NAME}"

cd "${FULL_BUILD_PATH}"
cd 'build/'"${BUILD_REGION}"'_pc/'
ARGS=$(echo "$@" | tr '\n' ' ')
echo "${ARGS}"
./sm64."${BUILD_REGION}".f3dex2e ${ARGS}
