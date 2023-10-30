# This script is called when user requests patches to be applied
# If you don't know what that means, don't edit it!

set -e

FULL_BUILD_PATH="${BUILD_TARGET_DIR}/${BUILD_NAME}"

cd "${FULL_BUILD_PATH}"

IFS=$'\n'
for patch in ${BUILD_PATCHES}; do
    git apply "${patch}"
    echo "Applied: ${patch}"
done
