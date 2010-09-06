/*-----------------------------------------------------------------------------

  utility.cpp

-----------------------------------------------------------------------------*/
#include "stdapp.h"
#include "utility.h"
/*-----------------------------------------------------------------------------

	VECT�\���̊֌W

-----------------------------------------------------------------------------*/
VECT VECT::operator=(const VECT& vect)
{
	x=vect.x;
	y=vect.y;
	return *this;
}

VECT VECT::operator+(const VECT& vect)const
{
	VECT ret;
	ret.x = x+vect.x;
	ret.y = y+vect.y;
	return ret;
}
VECT VECT::operator-(const VECT& vect)const
{
	VECT ret;
	ret.x = x-vect.x;
	ret.y = y-vect.y;
	return ret;
}
VECT VECT::operator*(const double& d)const
{
	VECT ret;
	ret.x = x*d;
	ret.y = y*d;
	return ret;
}
VECT operator*(double d,const VECT& vect)
{
	VECT ret;
	ret.x = vect.x*d;
	ret.y = vect.y*d;
	return ret;
}
VECT VECT::operator/(const double& d)const
{
	VECT ret;
	ret.x = x/d;
	ret.y = y/d;
	return ret;
}
VECT VECT::operator+=(const VECT& vect)
{
	x += vect.x;
	y += vect.y;
	return *this;
}
VECT VECT::operator-=(const VECT& vect)
{
	x -= vect.x;
	y -= vect.y;
	return *this;
}
double VECT::Distance(const VECT& vect)const
{
	return sqrt((x-vect.x)*(x-vect.x) + (y-vect.y)*(y-vect.y));
}
double VECT::Norn()const
{
	return sqrt(x*x+y*y);
}
double VECT::operator *(const VECT& vect)const
{
	return x*vect.x + y*vect.y;
}
void VECT::Goto(const VECT& vect,const double velocity)
{
	double dst = Distance(vect);
	if(dst < velocity)
	{
		*this = vect;
	}
	else
	{
		*this += (vect - *this)/dst * velocity;
	}
}
double VECT::Cos(const VECT& vect)const
{
	return (*this) * vect / (Norn() * vect.Norn());
}
/*-----------------------------------------------------------------------------
	CProfile�̎���
-----------------------------------------------------------------------------*/
CProfile::CProfile()
{
	strcpy(m_buffer,"");
	m_pBuffer = m_buffer;
}
CProfile::CProfile(const CProfile& prf)
{
	strcpy(m_buffer,(const char*)prf);
	m_pBuffer = m_buffer;
}
CProfile::CProfile(const char* str)
{
	strcpy(m_buffer,str);
	m_pBuffer = m_buffer;
}
CProfile CProfile::operator =(const CProfile& prf)
{
	strcpy(m_buffer,(const char*)prf);
	m_pBuffer = m_buffer;
	return *this;
}
//�������c�ŋ�؂��ĕԂ�
char* CProfile::GetString(char* buffer,int c)
{
	char *temp;
	char def[] = "DEFAULT";

	if((temp = strchr(m_pBuffer, c))==NULL)
	{
		//c��������Ȃ��ꍇ
		temp = strchr(m_pBuffer,NULL);
		strncpy(buffer,m_pBuffer,temp-m_pBuffer);
		m_pBuffer = m_buffer+strlen(m_buffer);//�|�C���^�𕶎��񖖂ɂ��Ă���
		return buffer;
	}
	else if(temp-m_pBuffer == 0)//���傤��c�������Ă�Ƃ�
	{
		strcpy( buffer,def);
		m_pBuffer++;
		return buffer;
	}
	else
	{
		//����ȊO(�ʏ펞�j
		strncpy(buffer,m_pBuffer,temp-m_pBuffer);
		buffer[temp-m_pBuffer]=0;
		m_pBuffer = temp;
		m_pBuffer++;
		return buffer;
	}
	return buffer;
}
//c2�̓X�g�b�p�[
char* CProfile::GetString(char* buffer,int c1,int c2)
{
	char *temp;
	char def[] = "DEFAULT";

	if((temp = strchr(m_pBuffer, c1))==NULL)
	{
		//c1��������Ȃ��ꍇ
		temp = strchr(m_pBuffer,NULL);
		strncpy(buffer,m_pBuffer,temp-m_pBuffer);
		m_pBuffer = m_buffer+strlen(m_buffer);//�|�C���^�𕶎��񖖂ɂ��Ă���
		return buffer;
	}
	else if(temp-m_pBuffer == 0)//���傤��c1�������Ă�Ƃ�
	{
		strcpy( buffer,def);
		m_pBuffer++;
		return buffer;
	}
	else
	{
		char *temp2;
		temp2 = strchr(m_pBuffer, c2);
		if(temp2 == NULL)
		{
			//c2��������Ȃ��ꍇ
			temp = strchr(m_pBuffer,NULL);
			strncpy(buffer,m_pBuffer,temp-m_pBuffer);
			m_pBuffer = m_buffer+strlen(m_buffer);//�|�C���^�𕶎��񖖂ɂ��Ă���
			return buffer;
		}
		else if(temp2-m_pBuffer == 0)//���傤��c2�������Ă�Ƃ�
		{
			strcpy( buffer,def);
			return buffer;
		}
		else if(temp2 < temp)//c2�̂ق���c1����ɂ���Ƃ�
		{
			strncpy(buffer,m_pBuffer,temp2-m_pBuffer);
			buffer[temp2-m_pBuffer]=NULL;
			m_pBuffer = temp2;//�|�C���^��c2�ɂ��킹�Ă���
			return buffer;
		}
		else//�ӂ[��c1�����������ꍇ
		{
			strncpy(buffer,m_pBuffer,temp-m_pBuffer);
			buffer[temp-m_pBuffer]=0;
			m_pBuffer = temp;
			m_pBuffer++;
			return buffer;
		}
	}
	return buffer;
}
//int�Ԃ�
int CProfile::GetInt(int c)
{
	char buffer[BUFFER_NUM];
	if(strcmp(GetString(buffer,c),"DEFAULT") == 0)
		return 0;
	else
		return atoi(buffer);
}
//int�Ԃ�2
int CProfile::GetInt(int c1,int c2)
{
	char buffer[BUFFER_NUM];
	if(strcmp(GetString(buffer,c1,c2),"DEFAULT") == 0)
		return 0;
	else
		return atoi(buffer);
}
/*-----------------------------------------------------------------------------
//�⏕�n�֐�
-----------------------------------------------------------------------------*/

//������Ԃ�(���Ȃ�1�A���Ȃ�-1�A0�Ȃ�0)
int sign(int num)
{
	if(num == 0)
		return 0;
	else
		return num/abs(num);
}
//�l�̌ܓ�
int round(double n)
{
	if(n-0.5<(int)n)
		return (int)n;
	else
		return (int)n+1;
}
//��]�i�l�n�c
int mod(int a,int b)
{
	if(a>=0)
		return a%b;
	else
		return a%b +b;
}
//	������^�v���t�@�C���ǂݍ���
char *prfGetString(const char *file, const char *section, const char *key, char *buffer)
{
	char path[MAX_PATH+sizeof(file)];
	char *temp;
	GetModuleFileName(NULL, path, sizeof(path));
	temp = strrchr(path, '\\');
	if(temp){
		strcpy(temp, "\\");
		strcat(path, file);
	}
	else{
		strcat(path, "\\");
		strcat(path, file);
	}
	GetPrivateProfileString(section, key, "DEFAULT", buffer, 128, path);
	return buffer;
}
//	�����^�v���t�@�C���ǂݍ���
double prfGetDouble(const char *file, const char *section, const char *key)
{
	char buffer[256];
	char path[MAX_PATH+sizeof(file)];
	char *temp;
	GetModuleFileName(NULL, path, sizeof(path));
	temp = strrchr(path, '\\');
	if(temp){
		strcpy(temp, "\\");
		strcat(path, file);
	}
	else{
		strcat(path, "\\");
		strcat(path, file);
	}
	GetPrivateProfileString(section, key, "DEFAULT", buffer, 128, path);
	return atof(buffer);
}
//	������^�v���t�@�C����������
void prfSetString(const char *file, const char *section, const char *key, const char *buffer)
{
	char path[MAX_PATH+sizeof(file)];
	char *temp;
	GetModuleFileName(NULL, path, sizeof(path));
	temp = strrchr(path, '\\');
	if(temp){
		strcpy(temp, "\\");
		strcat(path, file);
	}
	else{
		strcat(path, "\\");
		strcat(path, file);
	}
	WritePrivateProfileString(section, key, buffer, path);
}
void prfSetInt(const char *file, const char *section, const char *key, int value)
{
	char path[MAX_PATH+sizeof(file)];
	char *temp;
	GetModuleFileName(NULL, path, sizeof(path));
	temp = strrchr(path, '\\');
	if(temp){
		strcpy(temp, "\\");
		strcat(path, file);
	}
	else{
		strcat(path, "\\");
		strcat(path, file);
	}
	char buffer[256];
	itoa(value,buffer,10);
	WritePrivateProfileString(section, key, buffer, path);
}
//	�����^�v���t�@�C���ǂݍ���
int prfGetInt(const char *file, const char *section, const char *key)
{
	char path[MAX_PATH+sizeof(file)];
	char *temp;

	GetModuleFileName(NULL, path, sizeof(path));
	temp = strrchr(path, '\\');
	if(temp){
		strcpy(temp, "\\");
		strcat(path, file);
	}
	else{
		strcat(path, "\\");
		strcat(path, file);
	}	
	return GetPrivateProfileInt(section, key, 0, path);
}
//�v���t�@�C���̃Z�N�V�������當�����u��������
void ReplaceFromProfile(char* buffer,const char *file, const char *section)
{
	char path[MAX_PATH+sizeof(file)];
	char *temp;
	GetModuleFileName(NULL, path, sizeof(path));
	temp = strrchr(path, '\\');
	if(temp){
		strcpy(temp, "\\");
		strcat(path, file);
	}
	else{
		strcat(path, "\\");
		strcat(path, file);
	}
	char str[256*8];
	GetPrivateProfileSection(section,str,256*8,file);
	char* p1=str;
	while(*p1!='\0')
	{
		p1=strchr(p1,'\0');
		*p1 = '\n';
		p1++;
	}
	char* pStr = strtok(str,"=\0");
	while(pStr!=NULL)
	{
		char* p = strstr(buffer,pStr);
		pStr = strtok(NULL,"=\n");
		if(p!=NULL)
		{ 
			strcpy(p,pStr);
		}
		pStr = strtok(NULL,"=\n");
	}
}
/*
	��������u,�v�Ȃǂŋ�؂��Đ����l��Ԃ��A�����񎩐g�́u,�v�̌��̂ݎc��
	�����_�̕����񓯎m�̈����Z(buffer=string-c)�ɓ�����B
*/
char *strdiv(char *buffer,char *&string,int c,const char *def)
{
	char *temp;
	
	if((temp = strchr(string, c))==NULL)
	{
		if((temp = strchr(string,NULL))==NULL)
		{//�s����NULL�������ꍇ�i�܂����̎��̗�O����
			strcpy( buffer,def);
			return buffer;
		}
		//c��������Ȃ��ꍇ
		strcpy( buffer,def);
		return buffer;
	}
	if(temp-string == 0)//�J���}�A��������ѕ�����I�[�ȍ~
		strcpy( buffer,"");
	else{//����ȊO
		strncpy(buffer,string,temp-string);
		buffer[temp-string]=0;
	}
	string = temp;
	string++;
	return buffer;
}
//�؂�o��������int��Ԃ��B
int strdiv2(char *&string,int c)
{
	char buffer[256];
	strdiv(buffer,string,c,"DEFAULT");
	if(strcmp(buffer,"DEFAULT") == 0)
		while(string[0] != NULL)
			string++;
	return atoi(buffer);
}
/*-----------------------------------------------------------------------------
//�f�o�b�O�⏕�֐�
-----------------------------------------------------------------------------*/
//	Int�^��\������MessageBox(�f�o�b�O�p)
void MessageInt(const int num,char *caption)
{
	char str[32];
	sprintf( str, "%d", num );
	MessageBox(NULL,str,caption,MB_OK);
}
//	Int�^��\������MessageBox(�f�o�b�O�p)
void OutputDebugInt(const int num)
{
	char str[32];
	sprintf( str, "%d", num );
	OutputDebugString(str);
}
void OutputDebugInt(const int num,const char* cap)
{
	char str[128];
	sprintf( str, "%s:%d\n", cap, num );
	OutputDebugString(str);
}
//MessageBox��printf�݂����g��
int msgf(char *text,char *caption,unsigned int type,...)
{
	char* buffer=new char[strlen(text)+256];
	va_list args;
	va_start(args, type);
	vsprintf(buffer,text,args);
	va_end(args);
	int ret = MessageBox(NULL,buffer,caption,type);
	SAFE_DELETE_ARRAY(buffer);
	return ret;
}
//MessageBox��printf�݂����g��
int MBF_ERR(char* str,...)
{
	char* buffer=new char[strlen(str)+256];
	va_list args;
	va_start(args, str);
	vsprintf(buffer,str,args);
	va_end(args);
	int ret = MessageBox(NULL,buffer,"�X�N���v�g�G���[",MB_OK|MB_ICONSTOP);
	SAFE_DELETE_ARRAY(buffer);
	return ret;
};
//OuptputDebugString��pfintf�g��
void Outputf(char* str,...)
{
	char* buffer=new char[strlen(str)+256];\
	va_list args;
	va_start(args, str);
	vsprintf(buffer,str,args);
	va_end(args);
	OutputDebugString(buffer);
	SAFE_DELETE_ARRAY(buffer);
}