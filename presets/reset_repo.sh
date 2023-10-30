# This script attempts to reset the repository to a clean state as if it was just cloned
# If you don't know what that means, don't edit it!

FULL_BUILD_PATH="${BUILD_TARGET_DIR}/${BUILD_NAME}"

cd "${FULL_BUILD_PATH}"
rm -rf build/
rm -rf build/lib/APCpp/build
git reset --hard

echo "### RESET REPO ###"
