#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include "APIC.H"
#include "Utils.H"
//************************************
//���ߣ���Դ�ֿ�2012 copyright
//���ԣ�C++
//Blog��Http:\\www.qylk.blog.163.com
//Time��2012-10-18
//Ver�� v1.1
//************************************

//APIC Frame Struct
/*<Header for 'Attached picture', ID: "APIC">
     Standard Frame Header 	10byte
     Text encoding      	1byte
     MIME type          	<text string> end with 0x00(like"image/jpeg",only JPG and PNG are mentioned specifically in the ID3 v22 Spec,other types may not be portable)
     Picture type      		1byte
     Description        	<text string according to encoding> end with 0x00(0x00)(generally is just one 0x00)
     Picture data       	<binary data>
     
     ID3 supported Text encoding:
     	0x00=>"ISO-8859-1"
    	0x01=>"UTF-16"  
    	0x02=>"UTF-16BE" //Unicode strings must begin with the Unicode BOM($FF FE or $FE FF) to identify the byte order
   	0x03=>"UTF-8"
*/

JNIEXPORT jboolean JNICALL Java_cn_qylk_media_ArtistInfo_ApicFromTag(JNIEnv *env, jobject obj, jstring artist, jstring mp3path)//��ȡר��ͼƬ����������artist�͸����ļ�����·��
{
	const char * mp3=env->GetStringUTFChars(mp3path, false);//��java�е��ַ���תΪC�����е��ַ���(UTF)
	FILE * fp=fopen(mp3,"rb");//ֻ�����ļ� 
	env->ReleaseStringUTFChars(mp3path, mp3);
	if(fp==NULL) return JNI_FALSE;
	char head[10];//id3��ǩͷ
	fread(head,sizeof(head),1,fp);//����ǩͷ
 	if(!ArrayEqual(head,"ID3",3)) {//�Աȱ�ʶ
 		fclose(fp);
 		return JNI_FALSE;
	}	
 	int id3v2size=getID3size(head);//��ǩ��С
 	char frame[4];//֡��ʶ
 	int i=10;//֡��ʼλ��
 	int size;//��¼֡��С
 	while(i<id3v2size){
		fread(frame,4,1,fp);//��֡��ʶ
        	if(ArrayEqual(frame,"APIC",4))//�Ա�"APIC"
 	 		 break;		
       		else{
          	  fread(frame,4,1,fp);//��֡��С
          	  size=getFramesize(frame);//ת��Ϊint
          	  fseek(fp,size+2,SEEK_CUR);//����Flags
          	  i+=size;
          	  i+=10;//֡ͷռ��10�ֽ�
        }
 }
   if(i<id3v2size){//�ҵ���APIC֡
	fread(frame,4,1,fp);//��APIC��С
	size=getFramesize(frame);//ȡ֡��С
	fseek(fp,2,SEEK_CUR);//����Flags��
	int start=ftell(fp);//֡������ʼλ��
	char coding=fgetc(fp);//Text encodingλ
	while(fgetc(fp)!=0x00);//����MIME type�ַ�����β
	fgetc(fp);//Picture typeλ
	if(coding==0x01||coding ==0x02)//UTF-16 coding
	  {
	     while(getwc(fp)!=0x0000)
	     	fgetc(fp);
	     	fgetc(fp);
	  }
	else
	     while(fgetc(fp)!=0x00);//����Description��β
	size-=(ftell(fp)-start);//�ؼ���ͼƬʵ�ʴ�С
	//������Picture data
	const char * martist=env->GetStringUTFChars(artist,false);
	char * pathto=new char[17+strlen(martist)+4+1];//�����ڴ棬������ͼƬ�Ĵ��λ��
	memset(pathto,0x00,17+strlen(martist)+4+1);//strcatǰ���������
	strcat(pathto,"/sdcard/qylk/pic/");//Ŀ¼
	strcat(pathto,martist);//�ļ���
	strcat(pathto,".jpg");//��׺
	FILE *fpp=fopen(pathto,"wb");
	copy(fp,fpp,size);//����Picture data
	fclose(fpp);//���ļ����ļ�������ڴ�д�ش���
	delete[] pathto;
	env->ReleaseStringUTFChars(artist, martist);
	}
	else{
	  fclose(fp);
	  return JNI_FALSE;
	}
	fclose(fp);
	return JNI_TRUE;//���سɹ���־
}