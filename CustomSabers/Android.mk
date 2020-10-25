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
TARGET_ARCH_ABI := $(APP_ABI)

include $(CLEAR_VARS)
LOCAL_MODULE := hook

#include $(CLEAR_VARS)
#LOCAL_MODULE := testil2cpp
#LOCAL_SRC_FILES := libil2cpp.so
#LOCAL_EXPORT_C_INCLUDES := ../beatsaber-hook/shared/libil2cpp
#include $(PREBUILT_SHARED_LIBRARY)

#LOCAL_SRC_FILES := $(LOCAL_PATH)/../obj/local/armeabi-v7a/libhook.a
#LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
include $(CLEAR_VARS)
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS    := -DMOD_ID='"CustomSabers"' -DVERSION='"0.0.1"' -I'C:\Unity\Editor\Data\il2cpp\libil2cpp'
LOCAL_MODULE    := customsabers

#LOCAL_SHARED_LIBRARIES := testil2cpp
LOCAL_SRC_FILES  += $(call rwildcard,../beatsaber-hook/shared/inline-hook/,*.cpp) $(call rwildcard,../beatsaber-hook/shared/utils/,*.cpp) $(call rwildcard,../beatsaber-hook/shared/inline-hook/,*.c)
# In order to add configuration support to your project, uncomment the following line:
#LOCAL_SRC_FILES  += $(call rwildcard,../beatsaber-hook/shared/config/,*.cpp)
# In order to add custom UI support to your project, uncomment the following line:
#LOCAL_SRC_FILES  += $(call rwildcard,../beatsaber-hook/shared/customui/,*.cpp)
# Add any new SRC includes from beatsaber-hook or other external libraries here
LOCAL_SRC_FILES  += AssetImporter.cpp main.cpp
#LOCAL_STATIC_LIBRARIES := libhook
include $(BUILD_SHARED_LIBRARY)
