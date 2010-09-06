/*-----------------------------------------------------------------------------

	NkScript.h

-----------------------------------------------------------------------------*/
#ifndef AFX_NKSCRIPT_H
#define AFX_NKSCRIPT_H

#define MB_ERR(str) MessageBox(NULL,str,"�X�N���v�g�G���[",MB_OK|MB_ICONSTOP)

class CFunc;
class CBlock;
class CConst;
class CVar;
class CVariable;
class CToken;
class CTokenArray;

//MessageBox��printf�݂����g��
int msgf(char *text,char *caption,unsigned int type,...);
int MBF_ERR(char* str,...);
//�t�@�C���ǂݍ���
bool ReadFile(char* buffer,int size,const char* lpszFileName);
//�t�@�C���ǂݍ��݁E�����o�b�t�@�쐬�o�[�W����
char* ReadFile(const char* lpszFileName);

//�����ƕ����Q���ׁA��v���邩�ǂ�����Ԃ�
bool chrstr(int chr,char* chrArray);
//������ƕ����Q���ׁA��v���Ȃ��ŏ��̈ʒu��Ԃ�
char* strstr2(char* str,char* chrArray);
//������̍ŏ��ƍŌ���|�C���^�Ŏw�肵�Ăm��������
char* newstr(char* pFront,char* pRear);
//��؂蕶���ŋ�؂�ꂽ������Q�ƍŏ��̂���������v���邩
bool strstrstr(char* str1,char* str2,int c);
//��؂蕶��c�ŋ�؂�ꂽ������Qstr2�Ɗ��S�Ɉ�v���邩
bool strstrstr2(char* str1,char* str2,int c);

//�����񂪒萔������킷�g�[�N�����ǂ���
bool IsConst(char* str);
//�����񂪕ϐ�������킷�g�[�N�����ǂ���
bool IsVar(char* str);
//�g�[�N�����X�g����֐������(�ŏ��ƍŌ�w��j
CFunc* NewFunc(CBlock* pBlock,CTokenArray* pTokenArray,int first,int last);
//�g�[�N�����X�g���當�����(�ŏ��w��j
CFunc* NewFunc2(CBlock* pBlock,CTokenArray* pTokenArray,int first,int& last);

/*-----------------------------------------------------------------------------
�g�[�N��������֌W
-----------------------------------------------------------------------------*/
//������N���X
class CToken
{
public:
	CToken();
	~CToken(){SAFE_DELETE_ARRAY(m_pString);};
	char* Cut(char* str,char* former);
	char* GetStr()const{return m_pString;};
	void Printf(){Outputf("%s\n",m_pString);};
	bool operator==(const char* str)const{return m_pString!=NULL && strcmp(m_pString,str)==0;};
	bool operator!=(const char* str)const{return m_pString!=NULL && strcmp(m_pString,str)!=0;};
	operator char*()const{return GetStr();};
	operator const char*()const{return GetStr();};
private:
	char*	m_pString;
};
inline bool operator==(const char* str,CToken token){return token==str;};
//�g�[�N���z��
class CTokenArray
{
public:
	CTokenArray(){};
	~CTokenArray();
	bool CutToToken(char* str);
	int GetCorrespondence(int index,const char* kakko1,const char* kakko2);
	int Search(int index,const char* str);
	int SearchCommma(int first,int last);
	int SearchEnzansi(char* str,int c,int first,int last);
	vector<CToken*>* GetArray(){return &m_vctpToken;};
	CToken* operator[](int i){return m_vctpToken[i];};
	void allprintf();
private:
	vector<CToken*> m_vctpToken;
};
//���Z�q�̗D�揇�ʂɂ��\�[�g�p�\����
//�Ή����Z�q�F=,+=,-=,||,&&,==,!=,<,<=,>,>=,+,-,*,/,%
struct TokenIndex
{
	int m_index;
	CTokenArray* m_pTokenArray;
	int Get()const;	//�D�揇�ʂ𓾂�
	//���Z�q�͕��ʍ�����]�����邽�߁A�t�ɂ���K�v������
	bool operator<(const TokenIndex& index)const{return Get()>index.Get();};
	bool operator>(const TokenIndex& index)const{return Get()<index.Get();};
};
/*-----------------------------------------------------------------------------

	�֐��N���X�n

-----------------------------------------------------------------------------*/
//�C���^�[�t�F�[�X
class CFunc
{
public:
	virtual double Get()=0;
};
//�ꎞ�ϐ��ő吔
#define VARNUM 256
//�ϐ��������́i�u���b�N�E��`�N���X�j�̃C���^�[�t�F�[�X
class CBlock : public CFunc
{
public:
	virtual double Get()=0;
	virtual CVar* NewVariable(const char* str)=0;
	virtual double* GetVarArray()=0;
	virtual int GetVarNum()=0;
};
//�_���u���b�N�N���X
class CLogicalBlock : public CBlock
{
public:
	CLogicalBlock(){m_pVar=NULL;m_varnum=0;m_pBlock=NULL;};
	~CLogicalBlock();
	virtual double Get();
	virtual CVar* NewVariable(const char* str);
	bool Read(CBlock* pBlock,CTokenArray* pTokenArray,int first);
public:
	virtual double* GetVarArray(){return m_pVar;};
	virtual int GetVarNum(){return m_varnum;};
	virtual char** GetVarNameArray(){return m_varname;};
	virtual CBlock* GetBlock(){return m_pBlock;};
protected:
	list<CFunc*> m_lstpFunc;	//�����X�g
	double* m_pVar;	//�ꎞ�ϐ��z��ւ̃|�C���^
	int m_varnum;		//�ꎞ�ϐ��̐�
	char* m_varname[VARNUM];	//�ꎞ�ϐ���
	CBlock* m_pBlock;	//�e�u���b�N
};
/*-----------------------------------------------------------------------------
	����q
-----------------------------------------------------------------------------*/
//if
class CIf : public CFunc
{
public:
	CIf(CFunc* pJoken,CFunc* pBlock,CFunc* pElse){m_pJoken=pJoken;m_pBlock=pBlock;m_pElse=pElse;};
	~CIf(){SAFE_DELETE(m_pJoken);SAFE_DELETE(m_pBlock);SAFE_DELETE(m_pElse);};
	virtual double Get()
	{
		if(m_pJoken->Get())
		{
			m_pBlock->Get();
			return 1;
		}
		else if(m_pElse)
			m_pElse->Get();
		return 0;
	};
private:
	CFunc* m_pJoken;	//������
	CFunc* m_pBlock;	//�{��
	CFunc* m_pElse;		//else���i�m�t�k�k�Ȃ�Ȃ�
};
//while
class CWhile : public CFunc
{
public:
	CWhile(CFunc* pJoken,CFunc* pBlock){m_pJoken=pJoken;m_pBlock=pBlock;};
	~CWhile(){SAFE_DELETE(m_pJoken);SAFE_DELETE(m_pBlock);};
	virtual double Get()
	{
		while(m_pJoken->Get())
		{
			try
			{
				m_pBlock->Get();
			}
			catch(const char* str)
			{
				if(strcmp(str,"__BREAK!")==0)
					break;
				else if(strcmp(str,"__CONTINUE!")==0)
					continue;
				else
					throw(str);
			}
		}
		return 0;
	};
private:
	CFunc* m_pJoken;	//������
	CFunc* m_pBlock;	//�{��
};
//for
class CFor : public CFunc
{
public:
	CFor(CFunc* pFirst,CFunc* pJoken,CFunc* pStep,CFunc* pBlock){m_pFirst=pFirst;m_pJoken=pJoken;m_pStep=pStep;m_pBlock=pBlock;};
	~CFor(){SAFE_DELETE(m_pJoken);SAFE_DELETE(m_pBlock);SAFE_DELETE(m_pFirst);SAFE_DELETE(m_pStep);};
	virtual double Get()
	{
		for(m_pFirst->Get();m_pJoken->Get();m_pStep->Get())
		{
			try
			{
				m_pBlock->Get();
			}catch(const char* str)
			{
				if(strcmp(str,"__BREAK!")==0)
					break;
				else if(strcmp(str,"__CONTINUE!")==0)
					continue;
				else
					throw(str);
			}
		}
		return 0;
	};
private:
	CFunc* m_pFirst;	//������
	CFunc* m_pJoken;	//������
	CFunc* m_pStep;		//��O��
	CFunc* m_pBlock;	//�{��
};
//break
class CBreak : public CFunc
{
public:
	virtual double Get(){throw("__BREAK!");};	//for,while�u���b�N�ŗ�O���L���b�`������
};
//continue
class CContinue : public CFunc
{
public:
	virtual double Get(){throw("__CONTINUE!");};	//for,while�u���b�N�ŗ�O���L���b�`������
};
/*-----------------------------------------------------------------------------
	�萔�E�ϐ�
-----------------------------------------------------------------------------*/
//�萔
class CConst : public CFunc
{
public:
	CConst(double value){m_value = value;};
	~CConst(){};
	virtual double Get(){return m_value;};
private:
	double m_value;
};
//�ϐ��̃C���^�[�t�F�[�X
class CVar : public CFunc
{
public:
	virtual double Get()=0;
	virtual double Set(CFunc* pValue)=0;
	virtual double Set(double value)=0;
};
//�ꎞ�ϐ�
class CVariable : public CVar
{
public:
	CVariable(CBlock* pBlock,int index){m_pBlock=pBlock;m_index=index;};
	~CVariable(){};
	virtual double Get(){return m_pBlock->GetVarArray()[m_index];};
	virtual double Set(CFunc* pValue){m_pBlock->GetVarArray()[m_index] = pValue->Get();return Get();};
	virtual double Set(double value){m_pBlock->GetVarArray()[m_index] = value;return Get();};
private:
	CBlock* m_pBlock;	//�e�I�u�W�F�N�g
	int m_index;		//CBlock�̕ϐ��z��̃C���f�b�N�X
};
/*-----------------------------------------------------------------------------
	���Z�q
-----------------------------------------------------------------------------*/
//������Z�q=
class CEqual : public CFunc
{
public:
	CEqual(CVar* pVariable,CFunc* pValue){m_pVariable=pVariable;m_pValue=pValue;};
	~CEqual(){SAFE_DELETE(m_pVariable);SAFE_DELETE(m_pValue);};
	virtual double Get(){return m_pVariable->Set(m_pValue);};
private:
	CVar* m_pVariable;	//���ӂ͕ϐ�
	CFunc* m_pValue;
};
//���Z������Z�q+=
class CPlusEqual : public CFunc
{
public:
	CPlusEqual(CVar* pVariable,CFunc* pValue){m_pVariable=pVariable;m_pValue=pValue;};
	~CPlusEqual(){SAFE_DELETE(m_pVariable);SAFE_DELETE(m_pValue);};
	virtual double Get(){return m_pVariable->Set(m_pVariable->Get()+m_pValue->Get());};
private:
	CVar* m_pVariable;	//���ӂ͕ϐ�
	CFunc* m_pValue;
};
//���Z������Z�q-=
class CMinusEqual : public CFunc
{
public:
	CMinusEqual(CVar* pVariable,CFunc* pValue){m_pVariable=pVariable;m_pValue=pValue;};
	~CMinusEqual(){SAFE_DELETE(m_pVariable);SAFE_DELETE(m_pValue);};
	virtual double Get(){return m_pVariable->Set(m_pVariable->Get()-m_pValue->Get());};
private:
	CVar* m_pVariable;	//���ӂ͕ϐ�
	CFunc* m_pValue;
};
//���Z���Z�q+
class CPlus : public CFunc
{
public:
	CPlus(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CPlus(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() + m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//���Z���Z�q-
class CMinus : public CFunc
{
public:
	CMinus(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CMinus(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() - m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//��Z���Z�q*
class CMultiply : public CFunc
{
public:
	CMultiply(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CMultiply(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() * m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//���@���Z�q/
class CDivide : public CFunc
{
public:
	CDivide(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CDivide(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() / m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//��]���Z�q%
class CSurplus : public CFunc
{
public:
	CSurplus(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CSurplus(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return (int)m_pValue1->Get() % (int)m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//�������Z�q==
class CEqualEqual : public CFunc
{
public:
	CEqualEqual(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CEqualEqual(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() == m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//�������Z�q!=
class CNotEqual : public CFunc
{
public:
	CNotEqual(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CNotEqual(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() != m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//��r���Z�q<
class CIsSmaller : public CFunc
{
public:
	CIsSmaller(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CIsSmaller(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() < m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//��r���Z�q>
class CIsLarger : public CFunc
{
public:
	CIsLarger(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CIsLarger(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() > m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//��r���Z�q>=
class CIsLargerEqual : public CFunc
{
public:
	CIsLargerEqual(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CIsLargerEqual(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() >= m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//��r���Z�q<=
class CIsSmallerEqual : public CFunc
{
public:
	CIsSmallerEqual(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CIsSmallerEqual(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() <= m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//�_�����Z�q&&
class CAndAnd : public CFunc
{
public:
	CAndAnd(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CAndAnd(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() && m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//�_�����Z�q||
class COrOr : public CFunc
{
public:
	COrOr(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~COrOr(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return m_pValue1->Get() || m_pValue2->Get();};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
/*-----------------------------------------------------------------------------
	�⏕�֐�
-----------------------------------------------------------------------------*/
//�T�C���֐�sin
class CSin : public CFunc
{
public:
	CSin(CFunc* pValue){m_pValue=pValue;};
	~CSin(){SAFE_DELETE(m_pValue);};
	virtual double Get(){return sin(m_pValue->Get());};
private:
	CFunc* m_pValue;
};
//�R�T�C���֐�cos
class CCos : public CFunc
{
public:
	CCos(CFunc* pValue){m_pValue=pValue;};
	~CCos(){SAFE_DELETE(m_pValue);};
	virtual double Get(){return cos(m_pValue->Get());};
private:
	CFunc* m_pValue;
};
//�^���W�F���g�֐�tan
class CTan : public CFunc
{
public:
	CTan(CFunc* pValue){m_pValue=pValue;};
	~CTan(){SAFE_DELETE(m_pValue);};
	virtual double Get(){return tan(m_pValue->Get());};
private:
	CFunc* m_pValue;
};
//�A�[�N�^���W�F���g�֐�atan
class CAtan : public CFunc
{
public:
	CAtan(CFunc* pValue){m_pValue=pValue;};
	~CAtan(){SAFE_DELETE(m_pValue);};
	virtual double Get(){return atan(m_pValue->Get());};
private:
	CFunc* m_pValue;
};
//�����֐�sqrt
class CSqrt : public CFunc
{
public:
	CSqrt(CFunc* pValue){m_pValue=pValue;};
	~CSqrt(){SAFE_DELETE(m_pValue);};
	virtual double Get(){return sqrt(m_pValue->Get());};
private:
	CFunc* m_pValue;
};
//�A�[�N�^���W�F���g�֐��Qatan2
class CAtan2 : public CFunc
{
public:
	CAtan2(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CAtan2(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return atan2(m_pValue1->Get(),m_pValue2->Get());};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//��Βlabs
class CAbs : public CFunc
{
public:
	CAbs(CFunc* pValue){m_pValue=pValue;};
	~CAbs(){SAFE_DELETE(m_pValue);};
	virtual double Get(){return abs(m_pValue->Get());};
private:
	CFunc* m_pValue;
};
//�����_��rnd(0-1)
class CRnd : public CFunc
{
public:
	virtual double Get(){return (double)rand()/RAND_MAX;};
};
//�����_��random
class CRandom : public CFunc
{
public:
	CRandom(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CRandom(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return Rand(m_pValue1->Get(),m_pValue2->Get());};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
//MOD
class CMod : public CFunc
{
public:
	CMod(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CMod(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return mod(m_pValue1->Get(),m_pValue2->Get());};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
/*-----------------------------------------------------------------------------
	DirectX�֌W
-----------------------------------------------------------------------------*/
class CWaitTime:public CFunc
{
public:
	virtual double Get(){return g_pNkLib->WaitTime();};
};
class CUpdateInput:public CFunc
{
public:
	virtual double Get(){return g_pNkLib->UpdateInput();};
};
class CDrawList:public CFunc
{
public:
	virtual double Get(){return g_pNkLib->DrawList();};
};
class CShowFPS:public CFunc
{
public:
	virtual double Get(){return g_pNkLib->ShowFPS();};
};
class CFlip:public CFunc
{
public:
	virtual double Get(){return g_pNkLib->Flip();};
};
//���͌n
class CGetInputState:public CFunc
{
public:
	CGetInputState(CFunc* pFunc){m_pFunc=pFunc;};
	~CGetInputState(){SAFE_DELETE(m_pFunc);};
	virtual double Get(){return g_pNkLib->GetInputState2(m_pFunc->Get());};
private:
	CFunc* m_pFunc;
};
class CGetInputEvent:public CFunc
{
public:
	CGetInputEvent(CFunc* pFunc){m_pFunc=pFunc;};
	~CGetInputEvent(){SAFE_DELETE(m_pFunc);};
	virtual double Get(){return g_pNkLib->GetInputEvent(m_pFunc->Get());};
private:
	CFunc* m_pFunc;
};
//�O���t�B�b�N
#define SCRIPT_IMAGE_NUM 128
extern CObjArray<CNkImage> g_image;
//���[�h
class CLoadImage:public CFunc
{
public:
	CLoadImage(CFunc* pFunc,char* str){m_pFunc=pFunc;m_str=str;};
	~CLoadImage(){SAFE_DELETE(m_pFunc);SAFE_DELETE_ARRAY(m_str);};
	virtual double Get()
	{
		int index=m_pFunc->Get();
		if(g_image[index]==NULL)
			g_image[index]=new CNkImage;
		return g_image[index]->Load(m_str);
	};
private:
	CFunc* m_pFunc;
	char* m_str;
};
//���[�h
class CLoadImage2:public CFunc
{
public:
	CLoadImage2(CFunc* pFunc,char* str,CFunc* pWidth,CFunc* pHeight){m_pFunc=pFunc;m_str=str;m_pWidth=pWidth;m_pHeight=pHeight;};
	~CLoadImage2(){SAFE_DELETE(m_pFunc);SAFE_DELETE_ARRAY(m_str);SAFE_DELETE(m_pWidth);SAFE_DELETE(m_pHeight);};
	virtual double Get()
	{
		int index=m_pFunc->Get();
		if(g_image[index]==NULL)
			g_image[index]=new CNkImage;
		return g_image[index]->Load(m_str,m_pWidth->Get(),m_pHeight->Get());
	};
private:
	CFunc* m_pFunc;
	char* m_str;
	CFunc* m_pWidth;
	CFunc* m_pHeight;
};
//�`��
class CDraw:public CFunc
{
public:
	CDraw(CFunc* pIndex,CFunc* pLayer,CFunc* pFuncX,CFunc* pFuncY){m_pIndex=pIndex;m_pLayer=pLayer;m_pFuncX=pFuncX;m_pFuncY=pFuncY;};
	~CDraw(){SAFE_DELETE(m_pIndex);SAFE_DELETE(m_pLayer);SAFE_DELETE(m_pFuncX);SAFE_DELETE(m_pFuncY);};
	virtual double Get(){return g_image[m_pIndex->Get()]->Draw2(m_pLayer->Get(),m_pFuncX->Get(),m_pFuncY->Get());};
private:
	CFunc* m_pIndex;
	CFunc* m_pLayer;
	CFunc* m_pFuncX;
	CFunc* m_pFuncY;
};
//�`��
class CDraw2:public CFunc
{
public:
	CDraw2(vector<CFunc*> pFuncArray){m_pFuncArray=pFuncArray;};
	~CDraw2(){for(int i=0;i<m_pFuncArray.size();++i)SAFE_DELETE(m_pFuncArray[i]);};
	virtual double Get()
	{
		return g_image[m_pFuncArray[0]->Get()]->Draw2(
			m_pFuncArray[1]->Get(),
			m_pFuncArray[2]->Get(),
			m_pFuncArray[3]->Get(),
			m_pFuncArray[4]->Get(),
			m_pFuncArray[5]->Get());
	};
private:
	vector<CFunc*> m_pFuncArray;
};
//�T�E���h
#define SCRIPT_SOUND_NUM 128
extern CObjArray<CNkSound> g_sound;
class CLoadSound:public CFunc
{
public:
	CLoadSound(CFunc* pFunc,char* str){m_pFunc=pFunc;m_str=str;};
	~CLoadSound(){SAFE_DELETE(m_pFunc);SAFE_DELETE_ARRAY(m_str);};
	double Get()
	{
		int index=m_pFunc->Get();
		if(g_sound[index]==NULL)
			g_sound[index]=new CNkSound;
		return g_sound[index]->Load(m_str);
	}
private:
	CFunc* m_pFunc;
	char* m_str;
};
class CPlaySound:public CFunc
{
public:
	CPlaySound(CFunc* pFunc1,CFunc* pFunc2){m_pFunc1=pFunc1;m_pFunc2=pFunc2;};
	~CPlaySound(){SAFE_DELETE(m_pFunc1);SAFE_DELETE(m_pFunc2);};
	double Get(){return g_sound[m_pFunc1->Get()]->Play(m_pFunc2->Get());}
private:
	CFunc* m_pFunc1;
	CFunc* m_pFunc2;
};
//�ǉ��֐�
//�V���b�g����
class CShot:public CFunc
{
public:
	CShot(vector<CFunc*> pFuncArray){m_pFuncArray=pFuncArray;};
	~CShot(){for(int i=0;i<m_pFuncArray.size();++i)SAFE_DELETE(m_pFuncArray[i]);};
	virtual double Get()
	{
		g_enemyShot.Add(new CEnemyShot(
			g_image[m_pFuncArray[0]->Get()],
			VECT(m_pFuncArray[1]->Get(),m_pFuncArray[2]->Get()),
			VECT(m_pFuncArray[3]->Get(),m_pFuncArray[4]->Get()),
			m_pFuncArray[5]->Get()));
		return 1;
	};
private:
	vector<CFunc*> m_pFuncArray;
};
//��������
class CExplode:public CFunc
{
public:
	CExplode(vector<CFunc*> pFuncArray){m_pFuncArray=pFuncArray;};
	~CExplode(){for(int i=0;i<m_pFuncArray.size();++i)SAFE_DELETE(m_pFuncArray[i]);};
	virtual double Get()
	{
		g_effect.Add(new CEffect(
			g_image[m_pFuncArray[0]->Get()],
			VECT(m_pFuncArray[1]->Get(),m_pFuncArray[2]->Get()),
			m_pFuncArray[3]->Get(),m_pFuncArray[4]->Get()));
		return 1;
	};
private:
	vector<CFunc*> m_pFuncArray;
};
//�S�Ă̓G�e���A�C�e����
class CShotToItem:public CFunc
{
public:
	CShotToItem(){};
	virtual double Get(){ShotToItem();return 1;}
};
//�n�`
class CIsTikei : public CFunc
{
public:
	CIsTikei(CFunc* pValue1,CFunc* pValue2){m_pValue1=pValue1;m_pValue2=pValue2;};
	~CIsTikei(){SAFE_DELETE(m_pValue1);SAFE_DELETE(m_pValue2);};
	virtual double Get(){return g_pStage->CollTikei(m_pValue1->Get(),m_pValue2->Get());};
private:
	CFunc* m_pValue1;
	CFunc* m_pValue2;
};
/*-----------------------------------------------------------------------------
	�N���X��`�N���X�i�Ȃ�̂�������
-----------------------------------------------------------------------------*/
class CClassDefine;
class CClassInstance;
class CMemFunc;
class CMemVar;
//�N���X��`
class CClassDefine
{
public:
	CClassDefine(){m_pMemFunc=NULL;m_varnum=0;}
	inline ~CClassDefine();
	bool Read(CTokenArray* pTokenArray,int first,char* str);
public:
	int GetVarNum(){return m_varnum;};
	char** GetVarNameArray(){return m_varname;};
	CMemFunc* GetMemFunc(){return m_pMemFunc;};
	CMemFunc* GetConstructer(){return m_pConstructer;};
private:
	CMemFunc* m_pMemFunc;			//�����o�֐�func
	CMemFunc* m_pConstructer;		//�R���X�g���N�^
	int m_varnum;					//�����o�ϐ��̐�
	char* m_varname[VARNUM];
};
/*//�N���X�̃C���X�^���X
class CClassInstance
{
public:
	CClassInstance(CClassDefine* pDefine);
	~CClassInstance(){SAFE_DELETE_ARRAY(m_vararray);};
	inline void Run();//���s
	double* GetVarArray(){return m_vararray;};
private:
	CClassDefine* m_pDefine;	//�^�̎��(vtable)
	double* m_vararray;		//�����o�ϐ�
};
*/
//�����V���[������������
class CEnemy;
class CClassInstance
{
public:
	CClassInstance(CClassDefine* pDefine,CEnemy* pEnemy);	//�������Ⴄ
	~CClassInstance(){SAFE_DELETE_ARRAY(m_vararray);};
	inline void Run();//���s
	double* GetVarArray(){return m_vararray;};
	CEnemy* GetEnemy(){return m_pEnemy;};
private:
	CClassDefine* m_pDefine;	//�^�̎��(vtable)]
	double* m_vararray;		//�����o�ϐ�
	CEnemy* m_pEnemy;		//�Ή�����G�l�~�[�N���X
};

//�����o�֐�
class CMemFunc : public CBlock
{
public:
	CMemFunc(){m_pClassDef=NULL;m_pInstance=NULL;m_pBlock=NULL;}
	~CMemFunc(){SAFE_DELETE(m_pBlock);};
	virtual double Get(){return 0;};
	virtual CVar* NewVariable(const char* str);	//CMemVar��new����
	virtual char** GetVarNameArray(){return m_pClassDef->GetVarNameArray();};
	virtual int GetVarNum(){return m_pClassDef->GetVarNum();};
	virtual double* GetVarArray(){return m_pInstance->GetVarArray();};
	bool Read(CClassDefine *pClassDef,CTokenArray* pTokenArray,int first);
	void Run(CClassInstance* pInstance);		//this���󂯎���Ď��s����֐�
public:
	CClassInstance* GetInst(){return m_pInstance;};
private:
	CClassDefine* m_pClassDef;
	CClassInstance* m_pInstance;	//this��ۑ����邽�߂̃|�C���^
	CLogicalBlock* m_pBlock;
};
//�����o�ϐ�
class CMemVar : public CVar
{
public:
	CMemVar(CMemFunc* pMemFunc,int index){m_pMemFunc=pMemFunc;m_index=index;};
	double &GetVar()
	{
		if(m_index>=0)
			return m_pMemFunc->GetInst()->GetVarArray()[m_index];
		switch(m_index)
		{
		case -1:
			return m_pMemFunc->GetInst()->GetEnemy()->GetX();
			break;
		case -2:
			return m_pMemFunc->GetInst()->GetEnemy()->GetY();
			break;
		case -3:
			return m_pMemFunc->GetInst()->GetEnemy()->GetImage();
			break;
		case -4:
			return m_pMemFunc->GetInst()->GetEnemy()->GetHP();
			break;
		case -5:
			return m_pMemFunc->GetInst()->GetEnemy()->GetAnm();
			break;
		case -10:
			return g_pPlayer->GetX();
			break;
		case -11:
			return g_pPlayer->GetY();
			break;
		}
	}
	virtual double Get(){return GetVar();};
	virtual double Set(CFunc* pValue){return GetVar()=pValue->Get();};
	virtual double Set(double value){return GetVar()=value;};
private:
	CMemFunc* m_pMemFunc;	//�����郁���o�֐��ithis���󂯎���Ă�
	int m_index;
};
//�C�����C���̎���
void CClassInstance::Run(){m_pDefine->GetMemFunc()->Run(this);}//���s
CClassDefine::~CClassDefine()
{
	SAFE_DELETE(m_pMemFunc);
	for(int i=0;i<VARNUM;++i)SAFE_DELETE_ARRAY(m_varname[i]);
}

/*-----------------------------------------------------------------------------

	�܂Ƃ߃N���X
	CNkScript
	�P�t�@�C���ɂP���Ή����A
	�L�q���ꂽ�֐���N���X���b�{�{������g����悤�ɂ���B

-----------------------------------------------------------------------------*/
//�⏕�p�\����
struct CLASSDEFINE
{
	CClassDefine m_define;
	char* m_name;
	CLASSDEFINE(CTokenArray* pTA,int first)
	{
		m_name=_strdup((*pTA)[first]->GetStr());
		m_define.Read(pTA,first+1,m_name);
	}
	~CLASSDEFINE(){SAFE_DELETE_ARRAY(m_name);};
};
struct FUNCDEFINE
{
	CLogicalBlock m_func;
	char* m_name;
	FUNCDEFINE(CTokenArray* pTA,int first)
	{
		m_name=_strdup((*pTA)[first]->GetStr());
		if(*(*pTA)[first+1]!="(")
			MBF_ERR("%s:�֐��Ɂi������܂���B\n",(*pTA)[first]->GetStr());
		int i=pTA->Search(first,")");
		if(i>=0)
			m_func.Read(NULL,pTA,i+1);
	}
	~FUNCDEFINE(){SAFE_DELETE_ARRAY(m_name);};
};
//�{��
class CNkScript
{
public:
	CNkScript(const char* filename);
	~CNkScript();
	int SearchClass(const char* name);
	int SearchFunc(const char* name);
	vector<CLASSDEFINE*>* GetClass(){return &m_pCDefine;};
	vector<FUNCDEFINE*>* GetFunc(){return &m_pFunc;};
private:
	vector<CLASSDEFINE*> m_pCDefine;
	vector<FUNCDEFINE*> m_pFunc;
};
#endif