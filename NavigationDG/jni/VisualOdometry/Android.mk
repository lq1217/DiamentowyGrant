LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# Visual Odometry library

include C:/DiamentowyGrant/OpenCV-2.4.10-android-sdk/sdk/native/jni/OpenCV.mk
#include ../../../../DiamentowyGrant/OpenCV-2.4.10-android-sdk/sdk/native/jni/OpenCV.mk


#include $(LOCAL_PATH)/FivePoint/Android.mk
include $(LOCAL_PATH)/NonFree/Android.mk
#include $(LOCAL_PATH)/ScaleEstimation/Android.mk


LOCAL_MODULE    := VisualOdometryModule
LOCAL_SRC_FILES += FivePoint/FivePointAlgorithm.cpp DetectDescribe/DetectDescribe.cpp VisualOdometryModuleExport.cpp

LOCAL_SRC_FILES += LDB/ldb.cpp

#LOCAL_C_INCLUDES := Eigen
#LOCAL_SRC_FILES += FivePointMadeEasy/Rpoly.cpp FivePointMadeEasy/5point.cpp FivePointMadeEasy/main.cpp
#LOCAL_SHARED_LIBRARIES := nonFreeModule
#LOCAL_CFLAGS += -fopenmp -O3
LOCAL_LDLIBS +=  -llog -ldl -O3 -DNDEBUG
LOCAL_LDFLAGS +=  -pthread
LOCAL_CFLAGS += -DNDEBUG 

include $(BUILD_SHARED_LIBRARY)

