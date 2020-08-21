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
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

include $(CLEAR_VARS)
LOCAL_MODULE := modloader
LOCAL_SRC_FILES := extern/libmodloader.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := beatsaber-hook_0_4_4
LOCAL_SRC_FILES := extern/libbeatsaber-hook_0_4_4.so
LOCAL_EXPORT_CFLAGS := -DNEED_UNSAFE_CSHARP
LOCAL_EXPORT_C_INCLUDES := extern/beatsaber-hook/shared
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := il2cpp-codegen_0_1_4
LOCAL_SRC_FILES := extern/codegen/obj/libil2cpp_codegen_0_1_4.so
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_EXPORT_C_INCLUDES := extern/codegen/include
LOCAL_EXPORT_CFLAGS := -Wno-inaccessible-base
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := mappingextensions
LOCAL_SRC_FILES += $(call rwildcard,src/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook/,*.c)
LOCAL_SHARED_LIBRARIES := modloader beatsaber-hook_0_4_4 il2cpp-codegen_0_1_4
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS    += -isystem 'extern' -isystem 'extern\libil2cpp\il2cpp\libil2cpp' -Wall -Wextra -isystem 'extern'
include $(BUILD_SHARED_LIBRARY)
