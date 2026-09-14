/*
 * Copyright (c) 2011, 2024, Oracle and/or its affiliates. All rights reserved.
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
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <gdk/gdk.h>

#include <com_sun_glass_ui_GlassRobot.h>
#include <com_sun_glass_ui_gtk_GtkRobot.h>
#include <com_sun_glass_events_MouseEvent.h>
#include "glass_general.h"
#include "glass_key.h"
#include "glass_screen.h"

#define MOUSE_BACK_BTN 8
#define MOUSE_FORWARD_BTN 9

extern "C" {

/*
 * Class:     com_sun_glass_ui_gtk_GtkRobot
 * Method:    _keyPress
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_sun_glass_ui_gtk_GtkRobot__1keyPress
  (JNIEnv *env, jobject obj, jint code)
{
    env->ThrowNew(env->FindClass("java/lang/UnsupportedOperationException"),
            "Global synthetic input is unavailable on Wayland");
}

/*
 * Class:     com_sun_glass_ui_gtk_GtkRobot
 * Method:    _keyRelease
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_sun_glass_ui_gtk_GtkRobot__1keyRelease
  (JNIEnv *env, jobject obj, jint code)
{
    env->ThrowNew(env->FindClass("java/lang/UnsupportedOperationException"),
            "Global synthetic input is unavailable on Wayland");
}

/*
 * Class:     com_sun_glass_ui_gtk_GtkRobot
 * Method:    _mouseMove
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_sun_glass_ui_gtk_GtkRobot__1mouseMove
  (JNIEnv *env, jobject obj, jint x, jint y)
{
    env->ThrowNew(env->FindClass("java/lang/UnsupportedOperationException"),
            "Global synthetic input is unavailable on Wayland");
}



/*
 * Class:     com_sun_glass_ui_gtk_GtkRobot
 * Method:    _mousePress
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_sun_glass_ui_gtk_GtkRobot__1mousePress
  (JNIEnv *env, jobject obj, jint buttons)
{
    env->ThrowNew(env->FindClass("java/lang/UnsupportedOperationException"),
            "Global synthetic input is unavailable on Wayland");
}

/*
 * Class:     com_sun_glass_ui_gtk_GtkRobot
 * Method:    _mouseRelease
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_sun_glass_ui_gtk_GtkRobot__1mouseRelease
  (JNIEnv *env, jobject obj, jint buttons)
{
    env->ThrowNew(env->FindClass("java/lang/UnsupportedOperationException"),
            "Global synthetic input is unavailable on Wayland");
}

/*
 * Class:     com_sun_glass_ui_gtk_GtkRobot
 * Method:    _mouseWheel
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_sun_glass_ui_gtk_GtkRobot__1mouseWheel
  (JNIEnv *env, jobject obj, jint amt)
{
    env->ThrowNew(env->FindClass("java/lang/UnsupportedOperationException"),
            "Global synthetic input is unavailable on Wayland");
}

/*
 * Class:     com_sun_glass_ui_gtk_GtkRobot
 * Method:    _getMouseX
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_sun_glass_ui_gtk_GtkRobot__1getMouseX
  (JNIEnv *env, jobject obj)
{
    (void)env;
    (void)obj;

    jint x;
    glass_gdk_display_get_pointer(gdk_display_get_default(), &x, NULL);
    x = rint(x / getUIScale(gdk_screen_get_default()));
    return x;
}

/*
 * Class:     com_sun_glass_ui_gtk_GtkRobot
 * Method:    _getMouseY
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_sun_glass_ui_gtk_GtkRobot__1getMouseY
  (JNIEnv *env, jobject obj)
{
    (void)env;
    (void)obj;

    jint y;
    glass_gdk_display_get_pointer(gdk_display_get_default(), NULL, &y);
    y = rint(y / getUIScale(gdk_screen_get_default()));
    return y;
}

/*
 * Class:     com_sun_glass_ui_gtk_GtkRobot
 * Method:    _getScreenCapture
 * Signature: (IIII[I)V
 */
JNIEXPORT void JNICALL Java_com_sun_glass_ui_gtk_GtkRobot__1getScreenCapture
  (JNIEnv * env, jobject obj, jint x, jint y, jint width, jint height, jintArray data)
{
    (void)obj;

    if (!data) {
        return;
    }
    if (width <= 0 || height <= 0) {
        return;
    }
    const int maxPixels = INT_MAX / 4;
    if (width >= maxPixels / height) {
        return;
    }

    const int numPixels = width * height;
    if (numPixels > env->GetArrayLength(data)) {
        return;
    }

    GdkPixbuf *screenshot, *tmp;
    GdkWindow *root_window = gdk_get_default_root_window();

    tmp = glass_pixbuf_from_window(root_window, x, y, width, height);
    if (!tmp) {
        return;
    }
    screenshot = gdk_pixbuf_add_alpha(tmp, FALSE, 0, 0, 0);
    g_object_unref(tmp);
    if (!screenshot) {
        return;
    }

    jint *pixels = (jint *)convert_BGRA_to_RGBA((int*)gdk_pixbuf_get_pixels(screenshot), width * 4, height);
    if (pixels) {
        env->SetIntArrayRegion(data, 0, numPixels, pixels);
        g_free(pixels);
    }
    g_object_unref(screenshot);
}

} // extern "C"
