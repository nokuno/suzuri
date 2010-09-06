/*-----------------------------------------------------------------------------

  utility.h
  �����V���[�ȊO�A�܂��̓Q�[���ȊO�ɂ��g����悤�ȃ��[�e�B���e�B�n���܂Ƃ߂�

-----------------------------------------------------------------------------*/
#ifndef AFX_UTILITY_H
#define AFX_UTILITY_H

//�⏕�}�N��
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete [] (p);	  (p)=NULL; } }
/*-----------------------------------------------------------------------------
//�x�N�g���\����
-----------------------------------------------------------------------------*/
struct VECT
{
	//���̂�double�^�̂Q�����x�N�g��
	double x;
	double y;
	//�R���X�g���N�^
	VECT(){};
	VECT(const double x1,const double y1){x=x1;y=y1;};
	VECT(const VECT& vect){x=vect.x;y=vect.y;};
	~VECT(){};
	//���Z�q
	VECT operator=(const VECT& vect);
	VECT operator+(const VECT& vect)const;
	VECT operator-(const VECT& vect)const;
	VECT operator/(const double& d)const;
	VECT operator*(const double& d)const;
	VECT operator+=(const VECT& vect);
	VECT operator-=(const VECT& vect);
	double operator*(const VECT& vect)const;//����
	//�����o�֐�
	double Distance(const VECT& vect)const;
	double Norn()const;
	void Goto(const VECT& vect,const double velocity);
	double Cos(const VECT& vect)const;
};
VECT operator*(double d,const VECT& vect);

/*-----------------------------------------------------------------------------
//�⏕�n�֐�
-----------------------------------------------------------------------------*/
inline int Rand(int from,int to)
{
	return (to-from)*rand()/(RAND_MAX+1) + from;
}
char *prfGetString(const char *file, const char *section, const char *key, char *buffer);
int prfGetInt(const char *file, const char *section, const char *key);
double prfGetDouble(const char *file, const char *section, const char *key);
void prfSetString(const char *file, const char *section, const char *key, const char *buffer);
void prfSetInt(const char *file, const char *section, const char *key, int value);
void OutputDebugInt(const int num);
void OutputDebugInt(const int num,const char* cap);
char *strdiv(char *buffer,char *&string,int c,const char *def="DEFAULT");
int strdiv2(char *&string,int c);
int sign(int num);
int round(double n);
int mod(int a,int b);
void MessageInt(const int num,char *caption = "");
void ReplaceFromProfile(char* buffer,const char *file, const char *section);
void Outputf(char* str,...);

/*-----------------------------------------------------------------------------

  CProfile
  �v���t�@�C���p������N���X
  �������','��'/'�Ȃǂŋ�؂邱�Ƃ��ł���

-----------------------------------------------------------------------------*/
#define BUFFER_NUM 256
class CProfile
{
public:
	//�R���X�g���N�^
	CProfile();
	CProfile(const CProfile& prf);
	CProfile(const char* str);
	~CProfile(){};
	//���Z�q
	CProfile operator =(const CProfile& prf);
	operator char* () const {return (char*)m_buffer;};
	operator const char*() const {return m_buffer;};
	//�����o
	char* GetString(char* buffer,int c);				//����c��T���ċ�؂�
	char* GetString(char* buffer,int c1,int c2);		//c2�̓X�g�b�p�[����
	int GetInt(int c);
	int GetInt(int c1,int c2);
	char* Get(){return m_pBuffer;};
protected:
	//���̂̓o�b�t�@�Ƃ���ւ̃|�C���^�̃Z�b�g
	char m_buffer[BUFFER_NUM];
	char* m_pBuffer;
};
/*-----------------------------------------------------------------------------
	
	�I�u�W�F�N�g�z��e���v���[�g
	�s�^�I�u�W�F�N�g�z��̓��I�쐬�A����уf�[�^�̒ǉ��ƍ폜���ȒP�ɂł���B
	���I�쐬�Ƃ����Ă��������Ɏg�����x�B
	�f�[�^�̒ǉ���new�����A�h���X���󂯎���ĕۑ��B
	[]���Z�q�ɂ�蕁�ʂ̔z��̂悤�Ɏg����B

-----------------------------------------------------------------------------*/
template<class T>
class CObjArray
{
public:
	CObjArray();
	CObjArray(int size);
	~CObjArray();
	int Add(T* pObj);
	int StepFrame();
	void DeleteAll();
	bool IsAllNull();
public:
	int GetSize(){return m_size;};
	T*& operator[](int i){return m_ppObj[i];};	//T*�z��Ɠ����悤�Ɉ�����
protected:
	T** m_ppObj;
	int m_size;
	int m_index;	//NULL�ɂȂ��Ă�����
};
/*-----------------------------------------------------------------------------
	��������CObjArray�֌W�̎���
-----------------------------------------------------------------------------*/
template <class T>
CObjArray<T>::CObjArray()
{
	m_ppObj = NULL;
	m_size = 0;
	m_index = 0;
}
template <class T>
CObjArray<T>::CObjArray(int size)
{
	m_ppObj = new T* [size];
	for(int i=0;i<size;++i)
		m_ppObj[i] = NULL;
	m_size = size;
	m_index = 0;
}
template <class T>
CObjArray<T>::~CObjArray()
{
	DeleteAll();
	SAFE_DELETE_ARRAY(m_ppObj);
}
template <class T>
int CObjArray<T>::Add(T* pObj)
{
	if(m_ppObj == NULL) return 0;
	int old_index = m_index;
	for(;;)
	{
		if(m_ppObj[m_index] == NULL)
		{
			//�n���ꂽ�|�C���^��ۑ�
			m_ppObj[m_index] = pObj;
			break;
		}
		else
		{
			m_index++;
			if(m_index == m_size)
				m_index = 0;
			if(m_index == old_index)
			{
				delete pObj;
				return 0;
			}
		}
	}
	return 1;
}
template <class T>
int CObjArray<T>::StepFrame()
{
	if(m_ppObj == NULL) return 0;
	for(int i=m_size-1;i>=0;--i)
	{
		if(m_ppObj[i] != NULL)
		{
			//StepFrame�����s
			if(!m_ppObj[i]->StepFrame())
			{
				//StepFrame��0��Ԃ�����A�c�d�k�d�s�d����
				SAFE_DELETE(m_ppObj[i]);
			}
		}
	}
	return 1;
}
template <class T>
void CObjArray<T>::DeleteAll()
{
	for(int i=0;i<m_size;++i)
	{
		SAFE_DELETE(m_ppObj[i]);
	}
}
template <class T>
bool CObjArray<T>::IsAllNull()
{
	for(int i=0;i<m_size;++i)
		if(m_ppObj[i])
			return 0;
	return 1;
}
#endif