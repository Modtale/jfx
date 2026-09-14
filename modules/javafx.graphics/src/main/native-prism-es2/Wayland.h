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

#ifndef MODTALE_WAYLAND_H
#define MODTALE_WAYLAND_H
#include "PrismES2Defs.h"
jlong prism_wayland_initialize(JNIEnv *, jintArray);
jlong prism_wayland_pixel_format(JNIEnv *, jintArray);
jlong prism_wayland_drawable(jlong, jlong);
jboolean prism_wayland_swap(jlong);
void prism_wayland_release_drawable(jlong);
jlong prism_wayland_context(jlong, jlong, jboolean);
jlong prism_wayland_handle(jlong);
void prism_wayland_make_current(jlong, jlong);
void prism_wayland_destroy_context(ContextInfo *);
#endif
