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
#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <stdlib.h>
#include <string.h>

static EGLDisplay egl_display = EGL_NO_DISPLAY;
static EGLConfig egl_config;
static EGLContext shared_context = EGL_NO_CONTEXT;



static ContextInfo *create_context(jboolean vsync) {
    EGLContext context = eglCreateContext(egl_display, egl_config, shared_context, NULL);
    EGLint size[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
    EGLSurface bootstrap = eglCreatePbufferSurface(egl_display, egl_config, size);
    if (context == EGL_NO_CONTEXT || bootstrap == EGL_NO_SURFACE || !eglMakeCurrent(egl_display, bootstrap, bootstrap, context)) {
        fprintf(stderr, "Wayland EGL context failed: 0x%x\n", eglGetError());
        if (context != EGL_NO_CONTEXT) eglDestroyContext(egl_display, context);
        if (bootstrap != EGL_NO_SURFACE) eglDestroySurface(egl_display, bootstrap);
        return NULL;
    }
    ContextInfo *ctxInfo = calloc(1, sizeof(ContextInfo));
    if (!ctxInfo) {
        eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(egl_display, bootstrap);
        eglDestroyContext(egl_display, context);
        return NULL;
    }
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *extensions = (const char *)glGetString(GL_EXTENSIONS);
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer = (const char *)glGetString(GL_RENDERER);
    if (!version || !extensions || !vendor || !renderer) goto failed;
    ctxInfo->versionStr = strdup(version);
    ctxInfo->vendorStr = strdup(vendor);
    ctxInfo->rendererStr = strdup(renderer);
    ctxInfo->glExtensionStr = strdup(extensions);
    char *parsed = strdup(version);
    if (!parsed || !ctxInfo->versionStr || !ctxInfo->vendorStr || !ctxInfo->rendererStr
            || !ctxInfo->glExtensionStr) { free(parsed); goto failed; }
    extractVersionInfo(parsed, ctxInfo->versionNumbers);
    free(parsed);
    if (ctxInfo->versionNumbers[0] < 2 || (ctxInfo->versionNumbers[0] == 2 && ctxInfo->versionNumbers[1] < 1)) goto failed;
    ctxInfo->gl2 = JNI_TRUE;
    ctxInfo->waylandContext = context;
    ctxInfo->vSyncRequested = vsync;
    /* set function pointers */
    ctxInfo->glActiveTexture = (PFNGLACTIVETEXTUREPROC)
            eglGetProcAddress( "glActiveTexture");
    ctxInfo->glAttachShader = (PFNGLATTACHSHADERPROC)
            eglGetProcAddress( "glAttachShader");
    ctxInfo->glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)
            eglGetProcAddress( "glBindAttribLocation");
    ctxInfo->glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)
            eglGetProcAddress( "glBindFramebuffer");
    ctxInfo->glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)
            eglGetProcAddress( "glBindRenderbuffer");
    ctxInfo->glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)
            eglGetProcAddress( "glCheckFramebufferStatus");
    ctxInfo->glCreateProgram = (PFNGLCREATEPROGRAMPROC)
            eglGetProcAddress( "glCreateProgram");
    ctxInfo->glCreateShader = (PFNGLCREATESHADERPROC)
            eglGetProcAddress( "glCreateShader");
    ctxInfo->glCompileShader = (PFNGLCOMPILESHADERPROC)
            eglGetProcAddress( "glCompileShader");
    ctxInfo->glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)
            eglGetProcAddress( "glDeleteBuffers");
    ctxInfo->glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)
            eglGetProcAddress( "glDeleteFramebuffers");
    ctxInfo->glDeleteProgram = (PFNGLDELETEPROGRAMPROC)
            eglGetProcAddress( "glDeleteProgram");
    ctxInfo->glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)
            eglGetProcAddress( "glDeleteRenderbuffers");
    ctxInfo->glDeleteShader = (PFNGLDELETESHADERPROC)
            eglGetProcAddress( "glDeleteShader");
    ctxInfo->glDetachShader = (PFNGLDETACHSHADERPROC)
            eglGetProcAddress( "glDetachShader");
    ctxInfo->glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)
            eglGetProcAddress( "glDisableVertexAttribArray");
    ctxInfo->glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)
            eglGetProcAddress( "glEnableVertexAttribArray");
    ctxInfo->glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)
            eglGetProcAddress( "glFramebufferRenderbuffer");
    ctxInfo->glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)
            eglGetProcAddress( "glFramebufferTexture2D");
    ctxInfo->glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)
            eglGetProcAddress( "glGenFramebuffers");
    ctxInfo->glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)
            eglGetProcAddress( "glGenRenderbuffers");
    ctxInfo->glGetProgramiv = (PFNGLGETPROGRAMIVPROC)
            eglGetProcAddress( "glGetProgramiv");
    ctxInfo->glGetShaderiv = (PFNGLGETSHADERIVPROC)
            eglGetProcAddress( "glGetShaderiv");
    ctxInfo->glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)
            eglGetProcAddress( "glGetUniformLocation");
    ctxInfo->glLinkProgram = (PFNGLLINKPROGRAMPROC)
            eglGetProcAddress( "glLinkProgram");
    ctxInfo->glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)
            eglGetProcAddress( "glRenderbufferStorage");
    ctxInfo->glShaderSource = (PFNGLSHADERSOURCEPROC)
            eglGetProcAddress( "glShaderSource");
    ctxInfo->glUniform1f = (PFNGLUNIFORM1FPROC)
            eglGetProcAddress( "glUniform1f");
    ctxInfo->glUniform2f = (PFNGLUNIFORM2FPROC)
            eglGetProcAddress( "glUniform2f");
    ctxInfo->glUniform3f = (PFNGLUNIFORM3FPROC)
            eglGetProcAddress( "glUniform3f");
    ctxInfo->glUniform4f = (PFNGLUNIFORM4FPROC)
            eglGetProcAddress( "glUniform4f");
    ctxInfo->glUniform4fv = (PFNGLUNIFORM4FVPROC)
            eglGetProcAddress( "glUniform4fv");
    ctxInfo->glUniform1i = (PFNGLUNIFORM1IPROC)
            eglGetProcAddress( "glUniform1i");
    ctxInfo->glUniform2i = (PFNGLUNIFORM2IPROC)
            eglGetProcAddress( "glUniform2i");
    ctxInfo->glUniform3i = (PFNGLUNIFORM3IPROC)
            eglGetProcAddress( "glUniform3i");
    ctxInfo->glUniform4i = (PFNGLUNIFORM4IPROC)
            eglGetProcAddress( "glUniform4i");
    ctxInfo->glUniform4iv = (PFNGLUNIFORM4IVPROC)
            eglGetProcAddress( "glUniform4iv");
    ctxInfo->glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)
            eglGetProcAddress( "glUniformMatrix4fv");
    ctxInfo->glUseProgram = (PFNGLUSEPROGRAMPROC)
            eglGetProcAddress( "glUseProgram");
    ctxInfo->glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)
            eglGetProcAddress( "glValidateProgram");
    ctxInfo->glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)
            eglGetProcAddress( "glVertexAttribPointer");
    ctxInfo->glGenBuffers = (PFNGLGENBUFFERSPROC)
            eglGetProcAddress( "glGenBuffers");
    ctxInfo->glBindBuffer = (PFNGLBINDBUFFERPROC)
            eglGetProcAddress( "glBindBuffer");
    ctxInfo->glBufferData = (PFNGLBUFFERDATAPROC)
            eglGetProcAddress( "glBufferData");
    ctxInfo->glBufferSubData = (PFNGLBUFFERSUBDATAPROC)
            eglGetProcAddress( "glBufferSubData");
    ctxInfo->glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)
            eglGetProcAddress( "glGetShaderInfoLog");
    ctxInfo->glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)
            eglGetProcAddress( "glGetProgramInfoLog");
    ctxInfo->glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)
            eglGetProcAddress("glTexImage2DMultisample");
    ctxInfo->glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)
            eglGetProcAddress("glRenderbufferStorageMultisample");
    ctxInfo->glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)
            eglGetProcAddress("glBlitFramebuffer");

    initState(ctxInfo);
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(egl_display, bootstrap);
    return ctxInfo;
failed:
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(egl_display, bootstrap);
    eglDestroyContext(egl_display, context);
    free(ctxInfo->versionStr);
    free(ctxInfo->vendorStr);
    free(ctxInfo->rendererStr);
    free(ctxInfo->glExtensionStr);
    free(ctxInfo);
    return NULL;
}

jlong prism_wayland_initialize(JNIEnv *env, jintArray attrs) {
    (void)env; (void)attrs;
    // Initialize the display before Prism; Glass subsequently reuses it.
    gdk_set_allowed_backends("wayland");
    gdk_threads_init();
    if (!gtk_init_check(NULL, NULL)) return 0;
    GdkDisplay *display = gdk_display_get_default();
    if (!GDK_IS_WAYLAND_DISPLAY(display)) return 0;
    egl_display = eglGetDisplay((EGLNativeDisplayType)gdk_wayland_display_get_wl_display(display));
    if (egl_display == EGL_NO_DISPLAY || !eglInitialize(egl_display, NULL, NULL) || !eglBindAPI(EGL_OPENGL_API)) return 0;
    EGLint options[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_SAMPLE_BUFFERS, 0, EGL_SAMPLES, 0, EGL_NONE};
    EGLint count = 0;
    if (!eglChooseConfig(egl_display, options, &egl_config, 1, &count) || count == 0) return 0;
    ContextInfo *context = create_context(JNI_FALSE);
    if (context) {
        shared_context = context->waylandContext;
        fprintf(stderr, "Prism native Wayland EGL: %s / %s\n", context->vendorStr, context->rendererStr);
    }
    return ptr_to_jlong(context);
}

jlong prism_wayland_pixel_format(JNIEnv *env, jintArray attrs) {
    (void)env; (void)attrs;
    PixelFormatInfo *format = calloc(1, sizeof(PixelFormatInfo));
    if (format) format->waylandConfig = egl_config;
    return ptr_to_jlong(format);
}

jlong prism_wayland_drawable(jlong native_window, jlong native_format) {
    (void)native_format;
    DrawableInfo *drawable = calloc(1, sizeof(DrawableInfo));
    if (!drawable) return 0;
    if (native_window) {
        GdkWindow *window = (GdkWindow *)jlong_to_ptr(native_window);
        struct wl_surface *surface = gdk_wayland_window_get_wl_surface(window);
        int scale = gdk_window_get_scale_factor(window);
        struct wl_egl_window *egl_window = wl_egl_window_create(surface,
            gdk_window_get_width(window) * scale, gdk_window_get_height(window) * scale);
        if (!egl_window) { free(drawable); return 0; }
        wl_surface_set_buffer_scale(surface, scale);
        drawable->waylandSurface = eglCreateWindowSurface(egl_display, egl_config, (EGLNativeWindowType)egl_window, NULL);
        drawable->waylandWindow = egl_window;
        drawable->waylandGdkWindow = g_object_ref(window);
        drawable->onScreen = JNI_TRUE;
    } else {
        EGLint size[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
        drawable->waylandSurface = eglCreatePbufferSurface(egl_display, egl_config, size);
    }
    if (drawable->waylandSurface == EGL_NO_SURFACE) {
        fprintf(stderr, "Wayland EGL surface failed: 0x%x\n", eglGetError());
        prism_wayland_release_drawable(ptr_to_jlong(drawable));
        return 0;
    }
    return ptr_to_jlong(drawable);
}

jlong prism_wayland_context(jlong drawable, jlong format, jboolean vsync) {
    (void)drawable; (void)format;
    return ptr_to_jlong(create_context(vsync));
}

jlong prism_wayland_handle(jlong context) {
    ContextInfo *info = jlong_to_ptr(context);
    return info ? ptr_to_jlong(info->waylandContext) : 0;
}

void prism_wayland_make_current(jlong context, jlong drawable) {
    ContextInfo *ctx = jlong_to_ptr(context);
    DrawableInfo *target = jlong_to_ptr(drawable);
    if (!ctx || !target) return;
    if (target->waylandGdkWindow) {
        GdkWindow *window = target->waylandGdkWindow;
        int scale = gdk_window_get_scale_factor(window);
        wl_egl_window_resize(target->waylandWindow,
            MAX(1, gdk_window_get_width(window) * scale),
            MAX(1, gdk_window_get_height(window) * scale), 0, 0);
    }
    if (!eglMakeCurrent(egl_display, target->waylandSurface, target->waylandSurface, ctx->waylandContext)) {
        fprintf(stderr, "Wayland EGL make-current failed: 0x%x\n", eglGetError());
        return;
    }
    jboolean vsync = ctx->vSyncRequested && target->onScreen;
    eglSwapInterval(egl_display, vsync ? 1 : 0);
    ctx->state.vSyncEnabled = vsync;
}

jboolean prism_wayland_swap(jlong drawable) {
    DrawableInfo *target = jlong_to_ptr(drawable);
    return target && eglSwapBuffers(egl_display, target->waylandSurface) ? JNI_TRUE : JNI_FALSE;
}

static gboolean release_window(gpointer window) { g_object_unref(window); return G_SOURCE_REMOVE; }
void prism_wayland_release_drawable(jlong drawable) {
    DrawableInfo *target = jlong_to_ptr(drawable);
    if (!target) return;
    if (target->waylandSurface) eglDestroySurface(egl_display, target->waylandSurface);
    if (target->waylandWindow) wl_egl_window_destroy(target->waylandWindow);
    if (target->waylandGdkWindow) g_idle_add(release_window, target->waylandGdkWindow);
    free(target);
}

void prism_wayland_destroy_context(ContextInfo *context) {
    if (context->waylandContext == shared_context) shared_context = EGL_NO_CONTEXT;
    eglDestroyContext(egl_display, context->waylandContext);
    context->waylandContext = NULL;
}

#endif
