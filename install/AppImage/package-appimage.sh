#!/bin/bash
set -euo pipefail

readonly source_dir=/workspace
readonly build_dir="${source_dir}/build-appimage"
readonly app_dir="${build_dir}/wxChaos.AppDir"
readonly dist_dir="${source_dir}/dist"
readonly jobs="${JOBS:-$(nproc)}"

mkdir -p "${build_dir}/vcpkg-downloads"
export VCPKG_DOWNLOADS="${build_dir}/vcpkg-downloads"

cmake -S "${source_dir}" -B "${build_dir}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=x64-linux \
    -DBUILD_TESTING=OFF

cmake --build "${build_dir}" --target wxChaos --parallel "${jobs}"

rm -rf "${app_dir}"
DESTDIR="${app_dir}" cmake --install "${build_dir}"

# WebKitGTK helper executables are launched dynamically and therefore are not
# detected by linuxdeploy's ELF dependency scan.
webkit_dir=/usr/lib/x86_64-linux-gnu/webkit2gtk-4.1
if [[ -d "${webkit_dir}" ]]; then
    mkdir -p "${app_dir}${webkit_dir}"
    cp -a "${webkit_dir}/." "${app_dir}${webkit_dir}/"
fi

desktop-file-validate "${app_dir}/usr/share/applications/wxChaos.desktop"

/opt/linuxdeploy/AppRun \
    --appdir "${app_dir}" \
    --executable "${app_dir}/usr/bin/wxChaos" \
    --desktop-file "${app_dir}/usr/share/applications/wxChaos.desktop" \
    --icon-file "${app_dir}/usr/share/icons/hicolor/256x256/apps/wxChaos.png"

install -m 0755 "${source_dir}/install/AppImage/AppRun" "${app_dir}/AppRun"

mkdir -p "${dist_dir}"
rm -f "${dist_dir}"/wxChaos-*-x86_64.AppImage

export ARCH=x86_64
export OUTPUT="${dist_dir}/wxChaos-$(sed -n 's/^project(wxChaos VERSION \([^ ]*\).*/\1/p' "${source_dir}/CMakeLists.txt")-x86_64.AppImage"
(/opt/appimagetool/AppRun "${app_dir}" "${OUTPUT}")

(
    cd "${build_dir}"
    rm -rf squashfs-root
    "${OUTPUT}" --appimage-extract >/dev/null
    rm -rf squashfs-root
)

echo "Created ${OUTPUT}"
