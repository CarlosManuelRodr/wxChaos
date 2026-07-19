#!/bin/bash
set -euo pipefail

readonly project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
readonly image_name="${WXCHAOS_APPIMAGE_BUILDER_IMAGE:-wxchaos-appimage-builder:ubuntu-24.04}"

docker build \
    --file "${project_root}/install/AppImage/Dockerfile" \
    --tag "${image_name}" \
    "${project_root}"

docker run --rm \
    --user "$(id -u):$(id -g)" \
    --volume "${project_root}:/workspace" \
    --env HOME=/tmp \
    --env JOBS="${JOBS:-$(nproc)}" \
    "${image_name}"
