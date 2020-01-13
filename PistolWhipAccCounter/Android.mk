# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#include $(CLEAR_VARS)
#LOCAL_MODULE := testil2cpp
#LOCAL_SRC_FILES := libil2cpp.so
#LOCAL_EXPORT_C_INCLUDES := ../beatsaber-hook/shared/libil2cpp
#include $(PREBUILT_SHARED_LIBRARY)

#LOCAL_SRC_FILES := $(LOCAL_PATH)/../obj/local/armeabi-v7a/libhook.a
#LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

LOCAL_LDLIBS    := -llog
LOCAL_CFLAGS    := -DMOD_ID='"AccCounter"' -DVERSION='"1.0.0"'
LOCAL_MODULE    := acccounter
LOCAL_CPPFLAGS  := -std=c++2a -fno-rtti -Os

LOCAL_C_INCLUDES := ../shared/
LOCAL_SRC_FILES  := $(call rwildcard,../pistol-whip-hook/shared/,*.cpp) $(call rwildcard,../pistol-whip-hook/shared/,*.c) $(call rwildcard,./,*.cpp)

include $(BUILD_SHARED_LIBRARY)