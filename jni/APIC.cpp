#include <string.h>
#include <stdio.h>
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


const static BYTE m_wmaHeader[16] =
{
		0x30, 0x26, 0xB2, 0x75,
		0x8E, 0x66, 0xCF, 0x11,
		0xA6, 0xD9, 0x00, 0xAA,
		0x00, 0x62, 0xCE, 0x6C
};

const static BYTE m_stdFrameHeader[16] =
{
		0x33, 0x26, 0xB2, 0x75,
		0x8E, 0x66, 0xCF, 0x11,
		0xA6, 0xD9, 0x00, 0xAA,
		0x00, 0x62, 0xCE, 0x6C
};

const static BYTE m_exFrameHeader[16] =
{
		0x40, 0xA4, 0xD0, 0xD2,
		0x07, 0xE3, 0xD2, 0x11,
		0x97, 0xF0, 0x00, 0xA0,
		0xC9, 0x5E, 0xA8, 0x50
};

int WMA_calSize(const BYTE* buf)
{
    return buf[19] * 0x01000000+ buf[18] * 0x00010000+ buf[17] * 0x00000100+ buf[16];
}
bool readWMAAPIC(FILE *fp,const char *save){
		BYTE head[24];
		fread(head,1,sizeof(head),fp);
    if(!ArrayEqual(head, m_wmaHeader, 16)) return false;
    int frmsLen=WMA_calSize(head)-30;
    fseek(fp,6,SEEK_CUR);//����Unknown
    fread(head,1,sizeof(head),fp);
    if(!ArrayEqual(head,m_stdFrameHeader,16)) return false;
    int frmsNonExtLen=WMA_calSize(head)-sizeof(head);
    fseek(fp,frmsNonExtLen,SEEK_CUR);
    fread(head,1,sizeof(head),fp);
    if(!ArrayEqual(head,m_exFrameHeader,16)) return false;
    int frmslen=WMA_calSize(head);
    int FrmNum=fgetc(fp)+fgetc(fp)*0x100;
    BYTE fs[4];
    while(FrmNum-->0)
    {
        int namelen=fgetc(fp)+fgetc(fp)*0x100;
        BYTE *name=new BYTE[namelen];
        fread(name,1,namelen,fp);
        fread(fs,1,sizeof(fs),fp);
        //int flag=fs[0]+fs[1]*0x100;
        int sizev=fs[2]+fs[3]*0x100;
				if(ArrayEqual(name,(const BYTE*)L"WM/Picture",namelen)==false){
					delete [] name;
					fseek(fp,sizev,SEEK_CUR);//�ļ��Ƶ���һ֡
					continue;
					}
					delete [] name;
				fgetc(fp);                     //Picture typeλ
    		size_t datalen=(BYTE)fgetc(fp)+(BYTE)fgetc(fp)*0x100;
    		int i=3;
    		unsigned short dw[1];
    	while(i-->0)
    		{
        fread(dw,2,1,fp);
        while(dw[0]!=0)
        {
            fread(dw,2,1,fp);
        }
    	}
    	FILE *fpp=fopen(save,"wb");
    	copy(fp,fpp,(long)datalen);//����ԭʼ����
    	fclose(fpp);
    	return true;
    }
    return false;
}


bool readMP3APIC(FILE *fp,const char *save){
	BYTE head[10];//id3��ǩͷ
	fread(head,sizeof(head),1,fp);//����ǩͷ
 	if(!ArrayEqual(head,(const BYTE*)"ID3",3)) {//�Աȱ�ʶ
 		fclose(fp);
 		return false;
	}	
 	int id3v2size=getID3size(head);//��ǩ��С
 	BYTE frame[4];//֡��ʶ
 	int i=10;//֡��ʼλ��
 	int size;//��¼֡��С
 	while(i<id3v2size){
		fread(frame,4,1,fp);//��֡��ʶ
        	if(ArrayEqual(frame,(const BYTE*)"APIC",4))//�Ա�"APIC"
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
	BYTE coding=fgetc(fp);//Text encodingλ
	while(fgetc(fp)!=0x00);//����MIME type�ַ�����β
	fgetc(fp);//Picture typeλ
	if(coding==0x01||coding ==0x02)//UTF-16 coding
	  {	unsigned short t[1];
	  	fread(t,2,1,fp);
	    while(t[0]!=0)
	     	fread(t,2,1,fp);
	  }
	else
	     while(fgetc(fp)!=0x00);//����Description��β
	size-=(ftell(fp)-start);//�ؼ���ͼƬʵ�ʴ�С
	//������Picture data
	FILE *fpp=fopen(save,"wb");
	copy(fp,fpp,size);//����Picture data
	fclose(fpp);//���ļ����ļ�������ڴ�д�ش���
	}
	else{
	  fclose(fp);
	  return false;
	}
	fclose(fp);
	return true;//���سɹ���־
}

JNIEXPORT jboolean JNICALL Java_cn_qylk_media_ArtistInfo_ApicFromTag(JNIEnv *env, jobject obj, jstring artist, jstring mp3path)//��ȡר��ͼƬ����������artist�͸����ļ�����·��
{		const char * mp3=env->GetStringUTFChars(mp3path, false);//��java�е��ַ���תΪC�����е��ַ���(UTF)
		const char * martist=env->GetStringUTFChars(artist,false);
		char * pathto=new char[17+strlen(martist)+4+1];//�����ڴ棬������ͼƬ�Ĵ��λ��
		memset(pathto,0x00,17+strlen(martist)+4+1);//strcatǰ���������
		strcat(pathto,"/sdcard/qylk/pic/");//Ŀ¼
		strcat(pathto,martist);//�ļ���
		strcat(pathto,".jpg");//��׺
		jboolean suc;
			FILE * fp=fopen(mp3,"rb");//ֻ�����ļ� 
	if(fp==NULL) return false;
		if(checkExt(mp3)==MP3){
			suc= readMP3APIC(fp,pathto);
		}else if(checkExt(mp3)==WMA)
		{
			suc= readWMAAPIC(fp,pathto);
			}
	fclose(fp);
	delete[] pathto;
	env->ReleaseStringUTFChars(mp3path, mp3);
	env->ReleaseStringUTFChars(artist, martist);
return suc;
}



