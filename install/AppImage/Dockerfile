FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG VCPKG_ROOT=/opt/vcpkg
ARG VCPKG_COMMIT=e9f78231ca3bfb81ac6559a586aa2d80120327a6

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        autoconf \
        autoconf-archive \
        automake \
        bison \
        build-essential \
        ca-certificates \
        cmake \
        curl \
        desktop-file-utils \
        file \
        flex \
        gettext \
        gperf \
        git \
        libgl1-mesa-dev \
        libasound2-dev \
        libgstreamer-plugins-base1.0-dev \
        libgstreamer1.0-dev \
        libgtk-3-dev \
        libtool \
        libwebkit2gtk-4.1-dev \
        libwxgtk-webview3.2-dev \
        ninja-build \
        patchelf \
        pkg-config \
        python3 \
        tar \
        unzip \
        xz-utils \
        zip \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/microsoft/vcpkg.git "${VCPKG_ROOT}" \
    && git -C "${VCPKG_ROOT}" checkout "${VCPKG_COMMIT}" \
    && "${VCPKG_ROOT}/bootstrap-vcpkg.sh" -disableMetrics \
    && mkdir -p "${VCPKG_ROOT}/buildtrees" "${VCPKG_ROOT}/downloads" "${VCPKG_ROOT}/packages" \
    && chmod -R a+rwX "${VCPKG_ROOT}/buildtrees" "${VCPKG_ROOT}/downloads" "${VCPKG_ROOT}/packages"

RUN curl -fsSL \
        https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage \
        -o /usr/local/bin/linuxdeploy \
    && chmod +x /usr/local/bin/linuxdeploy \
    && cd /tmp \
    && /usr/local/bin/linuxdeploy --appimage-extract \
    && mv squashfs-root /opt/linuxdeploy

RUN curl -fsSL \
        https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage \
        -o /usr/local/bin/appimagetool \
    && chmod +x /usr/local/bin/appimagetool \
    && cd /tmp \
    && /usr/local/bin/appimagetool --appimage-extract \
    && mv squashfs-root /opt/appimagetool

WORKDIR /workspace

ENTRYPOINT ["/workspace/install/AppImage/package-appimage.sh"]
