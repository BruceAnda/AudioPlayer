##Դ�ļ��ڵ�ǰĿ¼
LOCAL_PATH := $(call my-dir)
##��ʼ���������
include $(CLEAR_VARS)
##����Ŀ��
LOCAL_MODULE := tagjni
##֧���쳣����
##LOCAL_CPPFLAGS += -fexceptions
##Ҫ�����Դ�ļ�
LOCAL_SRC_FILES := Utils.cpp\
		   APIC.cpp\
		   lyrics.cpp
##���log��־���
LOCAL_LDLIBS += -llog
##����ɾ�̬��		
include $(BUILD_SHARED_LIBRARY)

##�ڶ���
include $(CLEAR_VARS)
LOCAL_MODULE := ID3rw
LOCAL_SRC_FILES := reWriteID3.cpp\
		   Utils.cpp
LOCAL_LDLIBS += -llog		
include $(BUILD_SHARED_LIBRARY)

##������
include $(CLEAR_VARS)
LOCAL_MODULE := feedback
LOCAL_SRC_FILES := feedback.cpp
LOCAL_LDLIBS += -llog	
include $(BUILD_SHARED_LIBRARY)