LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := luaproxy_static

LOCAL_MODULE_FILENAME := libluaproxy

LOCAL_SRC_FILES := \
	tolua/CCBProxy.cpp \
	tolua/LuaEventHandler.cpp \
	tolua/luaopen_LuaProxy.cpp \
	tolua/LuaTableView.cpp \
	ui/CursorTextField.cpp \
	ui/UIEventDispatcher.cpp \
	ui/UIUtil.cpp
          
          
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/tolua \
                           $(LOCAL_PATH)/ui
          
LOCAL_C_INCLUDES := $(LOCAL_PATH)/tolua \
                    $(LOCAL_PATH)/../scripting/lua/lua \
                    $(LOCAL_PATH)/../scripting/lua/cocos2dx_support \
                    $(LOCAL_PATH)/../scripting/lua/tolua \
                    $(LOCAL_PATH)/ui \
                    $(LOCAL_PATH)/../cocos2dx \
                    $(LOCAL_PATH)/../cocos2dx/include \
                    $(LOCAL_PATH)/../cocos2dx/platform \
                    $(LOCAL_PATH)/../cocos2dx/platform/android \
                    $(LOCAL_PATH)/../cocos2dx/kazmath/include \
                    $(LOCAL_PATH)/../extensions \
                    $(LOCAL_PATH)/../CocosDenshion/include

LOCAL_CFLAGS += -Wno-psabi
LOCAL_EXPORT_CFLAGS += -Wno-psabi

include $(BUILD_STATIC_LIBRARY)
