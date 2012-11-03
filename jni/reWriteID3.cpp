#include <string.h>
#include <stdio.h>
#include "Utils.H"
#include "reWriteID3.h"
//************************************
//���ߣ���Դ�ֿ�2012 copyright
//���ԣ�C++
//Blog��Http:\\www.qylk.blog.163.com
//Time��2012-10-19
//Ver�� v1.0
//************************************

//************************************
// Method:    reCalculateSize
// FullName:  reCalculateSize
// Access:    public 
// Returns:   void
// Qualifier: ��дID3��ǩ��С
// Parameter: char head[] 10���ֽ�
// Parameter: int id3size ��ǩ��С
//************************************
void reCalculateID3Size(char head[],int id3size){
	head[6]=id3size/0x200000;
	id3size-=head[6]*0x200000;
	head[7]=id3size/0x4000;
	id3size-=head[7]*0x4000;
	head[8]=id3size/0x80;
	id3size-=head[8]*0x80;
	head[9]=id3size;
}


//************************************
// Method:    getHeaderSize
// FullName:  getHeaderSize
// Access:    public 
// Returns:   char *
// Qualifier: ��д֡��С
// Parameter: char header[] 10���ֽ�
// Parameter: int size	֡��С
//************************************
void reCalculateHeaderSize(char header[],int size)
{								  
	 header[4]=size/0x1000000;
	 size-=header[4]*0x1000000;
	 header[5]=size/0x10000;
	 size-=header[5]*0x10000;
	 header[6]=size/0x100;
	 size-=header[6]*0x100;
	 header[7]=size;
}

//************************************
// Method:    writeHead
// FullName:  writeHead
// Access:    public 
// Returns:   void
// Qualifier: дID3��ǩͷ��10�ֽ�
// Parameter: FILE * fp
// Parameter: char header[] Ҫд�ı�ǩͷ��10�ֽ�
//************************************
void writeHead(FILE *fp,char header[]){
	fwrite(header,10,1,fp);
}

//************************************
// Method:    writeFrame
// FullName:  writeFrame
// Access:    public 
// Returns:   void
// Qualifier: д֡
// Parameter: FILE * fp
// Parameter: char frameheader[]֡ͷ
// Parameter: char * value ֡����
//************************************
void writeFrame(FILE *fp,char frameheader[],char const *value){
	reCalculateHeaderSize(frameheader,strlen(value)+1);
	fwrite(frameheader,10,1,fp);
	fputc(0x03,fp);//ʹ��UTF-8���룬�����ʶ0x03��ĳЩ�����֧�����ֱ��룬����Winamp,��Android֧�־�����
	fwrite(value,strlen(value),1,fp);
}


//************************************
// Method:    writeID
// FullName:  writeID
// Access:    public 
// Returns:   void
// Qualifier: д��ǩ(��֡)��ʶ
// Parameter: char head[] ��ǩͷ/֡ͷ
// Parameter: const char * Id������"APIC"
//************************************
void writeID(char head[],const char *Id){
	int len=strlen(Id);
	for(int i=0;i<len;i++)
	  head[i]=Id[i];
}
//************************************
// Method:    writeID
// FullName:  writeID
// Access:    public 
// Returns:   void
// Qualifier: дID3�汾�źͱ�־
// Parameter: char head[]
// Parameter: const char * Id
//************************************
void writeID3Ver(char *head,char ver){
	head[3]=ver-'0';//�汾
	head[4]=0x00;//�ޱ�ʶ
	head[5]=0x00;
}
//************************************
// Method:    setFrameFlags
// FullName:  setFrameFlags
// Access:    public 
// Returns:   void
// Qualifier: д֡�ı�־λFlag
// Parameter: char head[]
//************************************
void setFrameFlags(char head[]){
	head[8]=0x00;//FlagλΪ0
	head[9]=0x00;
}

void JNICALL Java_cn_qylk_app_APPUtils_SaveToID3v2(JNIEnv *env, jobject obj, jstring title,jstring artist,jstring album,jstring mp3path){				     
	const char *mp3=env->GetStringUTFChars(mp3path, false);//jstringתUTF��UTF�������ַ�ռ3�ֽ�
	FILE * fp=fopen(mp3,"rb");//��MP3�ļ�
	if(fp==NULL) {env->ReleaseStringUTFChars(mp3path, mp3);return;}//��ʧ�ܷ��أ��ǵ��ͷ��ڴ�
	char head[10];//ID3��ǩͷ
	fread(head,sizeof(head),1,fp);//����ǩͷ
	long id3size=getID3size(head);//����ǩ��С
	int start=10;//copyĬ����ʼλ�ã�Ϊ��ǩͷ���
	if(!ArrayEqual(head,"ID3",3)) {//�Ƚϱ�ʾ
		id3size=0;
		writeID(head,"ID3");//д��ǩ��ʶ
		writeID3Ver(head,'3');//д�汾�źͱ�־λ
		start=0;//��ͷ��ʼcopy
	}
	const char * mtitle=env->GetStringUTFChars(title, false);//�ַ���ת��
	const char * martist=env->GetStringUTFChars(artist, false);
	const char * malbum=env->GetStringUTFChars(album, false);
	char * tmp=new char[strlen(mp3)+4];//��ʱ�ļ���λ�ã�ȡmp3ͬĿ¼
	memset(tmp,0x00,strlen(mp3)+4);//strcatǰ����������
	strcat(tmp,mp3);
	strcat(tmp,"tmp");//����ͬ�����Ӹ���׺'tmp'
	reCalculateID3Size(head,33+id3size+strlen(mtitle)+strlen(malbum)+strlen(martist));//��д��ǩ��С,33Ϊ3��֡ͷ�����(10byte per)+�����ʶλ(1byte per)
	FILE * newfp=fopen(tmp,"wb");//�½���ʱ�ļ�
	writeHead(newfp,head);//дID3��ǩͷ
	setFrameFlags(head);//�ظ�����head[],��ǰ����֡Flagλ
	writeID(head,"TIT2");//д����֡��ʶ
	writeFrame(newfp,head,mtitle);//д֡
	writeID(head,"TPE1");//д����֡��ʶ
	writeFrame(newfp,head,martist);//д֡
	writeID(head,"TALB");//дר��֡��ʶ
	writeFrame(newfp,head,malbum);//д֡
	fseek(fp,start,SEEK_SET);//�ļ�ָ���Ƶ�copy��ʼλ�ã�׼��copy
	copy(fp,newfp,filesize(fp)-start);//�ļ����ݶԿ�
	fclose(fp);
	fclose(newfp);
	remove(mp3);//ɾ��Դ�ļ�
	rename(tmp,mp3);//��������ʱ�ļ�=>Դ�ļ�
	LOGI("reBuild ID3...");
	//�ͷ���Դ
	delete[] tmp;
	env->ReleaseStringUTFChars(mp3path, mp3);
	env->ReleaseStringUTFChars(title, mtitle);
	env->ReleaseStringUTFChars(artist, martist);
	env->ReleaseStringUTFChars(album, malbum);
}
