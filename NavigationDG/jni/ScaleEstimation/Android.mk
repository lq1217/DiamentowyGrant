LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)


# Extended Kalman Filter library
include ../OpenCV-2.4.8-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := ScaleEstimation
LOCAL_SRC_FILES := ScaleEKF/ScaleEKF.cpp ScaleEstimation_export.cpp 
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)