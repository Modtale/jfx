/*
 * Copyright (c) 2012, 2024, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#ifdef MODTALE_WAYLAND
#include "Wayland.h"

JNIEXPORT jlong JNICALL Java_com_sun_prism_es2_X11GLContext_nInitialize
(JNIEnv *env, jclass class, jlong nativeDInfo, jlong nativePFInfo,
        jboolean vSyncRequested) {
    return prism_wayland_context(nativeDInfo, nativePFInfo, vSyncRequested);
}

JNIEXPORT jlong JNICALL Java_com_sun_prism_es2_X11GLContext_nGetNativeHandle
(JNIEnv *env, jclass class, jlong nativeCtxInfo) {
    return prism_wayland_handle(nativeCtxInfo);
}

JNIEXPORT void JNICALL Java_com_sun_prism_es2_X11GLContext_nMakeCurrent
(JNIEnv *env, jclass class, jlong nativeCtxInfo, jlong nativeDInfo) {
    prism_wayland_make_current(nativeCtxInfo, nativeDInfo); return;
}

JNIEXPORT jlong JNICALL Java_com_sun_prism_es2_X11GLDrawable_nCreateDrawable
(JNIEnv *env, jclass class, jlong nativeWindow, jlong nativePFInfo) {
    return prism_wayland_drawable(nativeWindow, nativePFInfo);
}

JNIEXPORT jlong JNICALL Java_com_sun_prism_es2_X11GLDrawable_nGetDummyDrawable
(JNIEnv *env, jclass class, jlong nativePFInfo) {
    return prism_wayland_drawable(0, nativePFInfo);
}

JNIEXPORT jboolean JNICALL Java_com_sun_prism_es2_X11GLDrawable_nSwapBuffers
(JNIEnv *env, jclass class, jlong nativeDInfo) {
    return prism_wayland_swap(nativeDInfo);
}

JNIEXPORT void JNICALL Java_com_sun_prism_es2_X11GLDrawable_nReleaseDrawable
(JNIEnv *env, jclass class, jlong nativeDInfo) {
    prism_wayland_release_drawable(nativeDInfo); return;
}

JNIEXPORT jlong JNICALL Java_com_sun_prism_es2_X11GLFactory_nInitialize
(JNIEnv *env, jclass class, jintArray attrArr) {
    return prism_wayland_initialize(env, attrArr);
}

JNIEXPORT jint JNICALL Java_com_sun_prism_es2_X11GLFactory_nGetAdapterOrdinal
(JNIEnv *env, jclass class, jlong screen) {
    return 0;
}

JNIEXPORT jint JNICALL Java_com_sun_prism_es2_X11GLFactory_nGetAdapterCount
(JNIEnv *env, jclass class) {
    return 1;
}

JNIEXPORT jint JNICALL Java_com_sun_prism_es2_X11GLFactory_nGetDefaultScreen
(JNIEnv *env, jclass class, jlong nativeCtxInfo) {
    return 0;
}

JNIEXPORT jlong JNICALL Java_com_sun_prism_es2_X11GLFactory_nGetDisplay
(JNIEnv *env, jclass class, jlong nativeCtxInfo) {
    return 0;
}

JNIEXPORT jlong JNICALL Java_com_sun_prism_es2_X11GLFactory_nGetVisualID
(JNIEnv *env, jclass class, jlong nativeCtxInfo) {
    return 0;
}

JNIEXPORT jboolean JNICALL Java_com_sun_prism_es2_X11GLFactory_nGetIsGL2
(JNIEnv *env, jclass class, jlong nativeCtxInfo) {
    return ((ContextInfo *)jlong_to_ptr(nativeCtxInfo))->gl2;
}

JNIEXPORT jlong JNICALL Java_com_sun_prism_es2_X11GLPixelFormat_nCreatePixelFormat
(JNIEnv *env, jclass class, jlong nativeScreen, jintArray attrArr) {
    return prism_wayland_pixel_format(env, attrArr);
}

#endif
