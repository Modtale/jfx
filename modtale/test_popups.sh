#!/usr/bin/env bash
set -euo pipefail

# Run against the packaged artifact, not Gradle's source-tree classes.
graphics=$(realpath "$1")
platform=$2
root=$(cd "$(dirname "$0")/.." && pwd)
work="$root/build/modtale/popup-test"
mkdir -p "$work"
for module in base controls; do
  curl --fail --location --retry 3 --silent --show-error \
    "https://repo.maven.apache.org/maven2/org/openjfx/javafx-$module/26.0.2/javafx-$module-26.0.2-$platform.jar" \
    --output "$work/javafx-$module.jar"
done
classpath="$graphics:$work/javafx-base.jar:$work/javafx-controls.jar"
"$JAVA_HOME/bin/javac" -cp "$classpath" -d "$work" "$root/modtale/PopupTransparencyTest.java"

unset DISPLAY
export GDK_BACKEND=wayland
if [[ -z "${WAYLAND_DISPLAY:-}" ]]; then
  export XDG_RUNTIME_DIR
  XDG_RUNTIME_DIR=$(mktemp -d)
  export WAYLAND_DISPLAY=modtale-popup-test
  weston --backend=headless-backend.so --use-pixman --socket="$WAYLAND_DISPLAY" \
    --idle-time=0 --log="$work/weston.log" &
  compositor=$!
  trap 'kill "$compositor" 2>/dev/null || true; wait "$compositor" 2>/dev/null || true; rm -rf "$XDG_RUNTIME_DIR"' EXIT
  for attempt in {1..100}; do
    [[ -S "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY" ]] && break
    kill -0 "$compositor"
    sleep 0.1
  done
  [[ -S "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY" ]]
fi
"$JAVA_HOME/bin/java" --enable-native-access=ALL-UNNAMED -Dprism.forceGPU=true \
  -cp "$work:$classpath" PopupTransparencyTest
