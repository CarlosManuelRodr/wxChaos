#!/bin/bash
set -euo pipefail

readonly project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

exec "${project_root}/install/AppImage/build-appimage.sh" "$@"
