#!/usr/bin/env bash
#
# Сборка AppImage для Linux x86_64.
#
# Использование:
#   ./scripts/build-appimage.sh        # сборка из уже установленного dist/
#   ./scripts/build-appimage.sh --full # полный цикл: configure -> build -> install -> AppImage
#
# Требования: cmake, ninja, Qt 6, linuxdeploy, linuxdeploy-plugin-qt,
# linuxdeploy-plugin-appimage. Скрипт сам скачивает linuxdeploy-инструменты
# в каталог .tools и переиспользует их при повторных запусках.
#
# AppImage-инструменты всегда извлекаются (--appimage-extract), поэтому
# FUSE на машине не требуется (важно для CI).

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOLS_DIR="${ROOT_DIR}/.tools"
APPDIR="${ROOT_DIR}/build/appdir"
DIST_DIR="${ROOT_DIR}/dist"

LINUXDEPLOY="${TOOLS_DIR}/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT="${TOOLS_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"
LINUXDEPLOY_APPIMAGE="${TOOLS_DIR}/linuxdeploy-plugin-appimage-x86_64.AppImage"

LINUXDEPLOY_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT_URL="https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
LINUXDEPLOY_APPIMAGE_URL="https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage"

VERSION="$(awk '/^project\(/{f=1} f && /VERSION/{print $2; exit}' "${ROOT_DIR}/CMakeLists.txt")"
VERSION="${VERSION:-0.0.0}"

APPIMAGE_NAME="YaMusic-${VERSION}-linux-x86_64.AppImage"

log() { printf '\033[1;36m[appimage]\033[0m %s\n' "$*"; }

ensure_tool() {
  local path="$1" url="$2"
  local extracted="${path%.AppImage}.squashfs-root"
  if [[ ! -x "${extracted}/AppRun" ]]; then
    log "Downloading $(basename "${path}")..."
    mkdir -p "${TOOLS_DIR}"
    curl -fsSL -o "${path}" "${url}"
    chmod +x "${path}"
    rm -rf "${extracted}"
    (cd "${TOOLS_DIR}" && "${path}" --appimage-extract >/dev/null)
    mv "${TOOLS_DIR}/squashfs-root" "${extracted}"
  fi
}

run_tool() {
  local path="$1"; shift
  "${path%.AppImage}.squashfs-root/AppRun" "$@"
}

main() {
  if [[ "${1:-}" == "--full" ]]; then
    log "Full build: configure -> build -> install"
    cmake -S "${ROOT_DIR}" -B "${ROOT_DIR}/build" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${DIST_DIR}"
    cmake --build "${ROOT_DIR}/build" --parallel
    rm -rf "${DIST_DIR}" "${APPDIR}"
    cmake --install "${ROOT_DIR}/build"
  fi

  if [[ ! -d "${DIST_DIR}" ]]; then
    log "Directory 'dist/' not found. Run './scripts/build-appimage.sh --full' first or install the build."
    exit 1
  fi

  ensure_tool "${LINUXDEPLOY}" "${LINUXDEPLOY_URL}"
  ensure_tool "${LINUXDEPLOY_QT}" "${LINUXDEPLOY_QT_URL}"
  ensure_tool "${LINUXDEPLOY_APPIMAGE}" "${LINUXDEPLOY_APPIMAGE_URL}"

  export VERSION
  export QML_SOURCES_PATHS="${ROOT_DIR}/qml"

  log "Bundling AppDir from dist/..."
  rm -rf "${APPDIR}"
  run_tool "${LINUXDEPLOY}" \
    --appdir "${APPDIR}" \
    --executable "${DIST_DIR}/bin/yamusic" \
    --desktop-file "${DIST_DIR}/share/applications/YaMusic.desktop" \
    --icon-file "${DIST_DIR}/share/icons/hicolor/scalable/apps/yamusic.svg" \
    --plugin qt

  log "Building ${APPIMAGE_NAME}..."
  run_tool "${LINUXDEPLOY_APPIMAGE}" \
    --appdir "${APPDIR}" \
    --output-file "${ROOT_DIR}/${APPIMAGE_NAME}"

  log "Done: ${ROOT_DIR}/${APPIMAGE_NAME}"
}

main "$@"