vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO flintlib/flint
    REF v${VERSION}
    SHA512 fbb6f0945b589e237d707c3b6963f7bb7fc1b9e5b511f5f8ed805f14f85b317c79a9eedc7ae28d34837f7126eac86dd4a9c8e7258560da3413c599eb64d367f7
    HEAD_REF master
    PATCHES
)

if(VCPKG_TARGET_IS_WINDOWS)
    vcpkg_find_acquire_program(PYTHON3)
    vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
        DISABLE_PARALLEL_CONFIGURE # see configure_file(${CMAKE_CURRENT_SOURCE_DIR} ...
        OPTIONS
            "-DPython_EXECUTABLE=${PYTHON3}"
            -DVCPKG_LOCK_FIND_PACKAGE_CBLAS=OFF
            -DWITH_NTL=OFF
            # FLINT 3.5 enables /arch:AVX2 whenever MSVC accepts the flag,
            # which makes every function in the resulting DLL require AVX.
            # wxChaos installers must also run on machines and VMs without AVX.
            -DENABLE_AVX2=OFF
            -DENABLE_AVX512=OFF
    )
    vcpkg_cmake_install()
    vcpkg_copy_pdbs()
    vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/flint)
else()
    vcpkg_make_configure(
        SOURCE_PATH "${SOURCE_PATH}"
        AUTORECONF
        OPTIONS
            --with-ntl=no
            --with-blas=no
    )
    vcpkg_make_install()
endif()

vcpkg_fixup_pkgconfig()

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/COPYING")
