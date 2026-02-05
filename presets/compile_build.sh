# This script is called when compiling the game, after downloading
# If you don't know what that means, don't edit it!

FULL_BUILD_PATH="${BUILD_TARGET_DIR}/${BUILD_NAME}"

# Force MSYS2 compilers if on Windows
if [[ -n "${MSYSTEM}" ]]; then
    export CC=/mingw64/bin/gcc
    export CXX=/mingw64/bin/g++
fi

cp  "${BUILD_ROM}" "${FULL_BUILD_PATH}/baserom.${BUILD_REGION}.z64"
cd "${FULL_BUILD_PATH}"
make ${BUILD_MAKE_FLAGS} VERSION="${BUILD_REGION}"
