#include <string.h>
#include <stdio.h>
#include "lyrics.h"
#include "Utils.H"
//************************************
//���ߣ���Դ�ֿ�2012 copyright
//���ԣ�C++
//Blog��Http:\\www.qylk.blog.163.com
//Time��2012-10-19
//Ver�� v1.0
//************************************
JNIEXPORT jboolean JNICALL Java_cn_qylk_lrc_MediaLyric_LyricFromTag(JNIEnv *env, jobject obj, jstring title, jstring mp3path){//���ļ�β����ȡ���
	const char * mp3=env->GetStringUTFChars(mp3path, false);//��java�е��ַ���תΪC�����е��ַ�������(UFT)
	FILE * fp=fopen(mp3,"rh");//��mp3�ļ�
	env->ReleaseStringUTFChars(mp3path, mp3);//�ͷ��ַ�����Դ 
	if(fp==NULL)return JNI_FALSE;//��ʧ�ܷ���
	fseek(fp,-137,SEEK_END);//��λ���ļ�β������137���ֽ�
	char identifer[9];//��ʶ
	fread(identifer,1,sizeof(identifer),fp);//����LYRICS200��ʶ
if(ArrayEqual(identifer,"LYRICS200",sizeof(identifer))){//�Աȱ�־
	fseek(fp,-141,SEEK_END);//��λ���ļ�β������141���ֽ�
	int i=getInt(fp)-30;//��ȡ��ʳ��ȣ�30�ǹ̶���
	if(i<50){fclose(fp);return JNI_FALSE;}//����̫�̣�һ�㲻�Ǹ��
	char log[100];//дlog
	sprintf(log,"Reading Lyrics From %s",mp3);
	LOGI(log);
	char *lrc=new char[i];//���������ڴ�
	const char *file=env->GetStringUTFChars(title,false); //�ַ���ת��
	char *pathto=new char[17 +strlen(file)+5];//׼����ƴ�����·��
	fseek(fp,-143-i,SEEK_END);//��λ����ʿ�ʼλ��
	fread(lrc,1,i,fp);//���������
	memset(pathto,0x00,sizeof(pathto));//strcatǰ���������
	strcat(pathto,"/sdcard/qylk/lrc/");//���·��
	strcat(pathto,file);//�ļ���
	strcat(pathto,".lrc");//��׺
	FILE *fpp=fopen(pathto,"wb+");
	fwrite(lrc,i,1,fpp);//���д���ļ�
	sprintf(log,"Writting Lyrics to %s",pathto);
	LOGI(log);
	fclose(fpp);
	env->ReleaseStringUTFChars(title, file);
	delete[] lrc;//�ͷŶ�����Դ
	delete[] pathto;
	fclose(fp);
	return JNI_TRUE;
}else{
	fclose(fp);
	return JNI_FALSE;
}
	
}