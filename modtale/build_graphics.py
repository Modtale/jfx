#!/usr/bin/env python3
"""Build the pinned OpenJFX Linux backend and retain the published Java classes."""
import argparse
import os
import re
from pathlib import Path
import shutil
import subprocess
import zipfile

VERSION = "26.0.2-modtale.2"
LIBRARIES = ("libglass.so", "libglassgtk3.so", "libprism_es2.so")


def run(*args, cwd=None, env=None):
    subprocess.run([str(arg) for arg in args], cwd=cwd, env=env, check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--base-jar", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--java-home", type=Path, required=True)
    parser.add_argument("--gradle", type=Path, required=True)
    parser.add_argument("--native-dir", type=Path)
    args = parser.parse_args()
    source = Path(__file__).resolve().parent.parent
    work = source / "build/modtale"
    work.mkdir(parents=True, exist_ok=True)
    native_dir = args.native_dir
    if native_dir is None:
        (source / "build/linux_gtk3.properties").unlink(missing_ok=True)
        # OpenJFX links every object in this directory, including removed sources.
        shutil.rmtree(source / "modules/javafx.graphics/build/native", ignore_errors=True)
        env = dict(os.environ, JAVA_HOME=str(args.java_home.resolve()))
        run(args.gradle.resolve(), "--no-daemon", ":graphics:compileFullJava", ":graphics:nativeGlass",
            ":graphics:nativePrismES2", "-PCONF=Release", "-PCOMPILE_MEDIA=false", "-PCOMPILE_WEBKIT=false",
            cwd=source, env=env)
        native_dir = work / "natives"
        native_dir.mkdir(exist_ok=True)
        libraries = source / "modules/javafx.graphics/build/libs"
        for name in LIBRARIES:
            component = "prismES2" if name == "libprism_es2.so" else "glass"
            shutil.copy2(libraries / component / "linux" / name, native_dir / name)
            run("strip", "--strip-unneeded", native_dir / name)
    for name in LIBRARIES:
        library = native_dir / name
        symbols = subprocess.check_output(["nm", "-D", "--undefined-only", str(library)], text=True)
        dependencies = subprocess.check_output(["readelf", "-d", str(library)], text=True)
        if re.search(r"\b(?:X[A-Z]\w*|glX\w*|gdk_x11_\w*)(?:@|$)", symbols, re.MULTILINE):
            raise RuntimeError(f"{name} still imports an X11 API")
        if re.search(r"\[(?:libX[^]]*|libGLX[^]]*|libGL\.so[^]]*)\]", dependencies):
            raise RuntimeError(f"{name} still links an X11/GLX library")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    temporary = args.output.with_suffix(".tmp")
    revision = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=source, text=True)
    with zipfile.ZipFile(args.base_jar) as base, zipfile.ZipFile(
        temporary, "w", zipfile.ZIP_DEFLATED, compresslevel=9
    ) as output:
        for entry in base.infolist():
            if entry.filename not in LIBRARIES and entry.filename != "META-INF/modtale-native-wayland":
                output.writestr(entry, base.read(entry.filename))
        for name in LIBRARIES:
            output.write(native_dir / name, name)
        output.writestr("META-INF/modtale-native-wayland", VERSION + "\n" + revision)
    temporary.replace(args.output)


if __name__ == "__main__":
    main()
