/*-----------------------------------------------------------------------------

  NkScript.cpp

-----------------------------------------------------------------------------*/
#include "stdapp.h"
#include "include.h"

//�O���t�B�b�N
CObjArray<CNkImage> g_image(SCRIPT_IMAGE_NUM);
//�T�E���h
CObjArray<CNkSound> g_sound(SCRIPT_SOUND_NUM);
/*-----------------------------------------------------------------------------
	�����񏈗�
-----------------------------------------------------------------------------*/
//������ƕ����Q���ׁA��v���Ȃ��ŏ���Ԃ�
char* strstr2(char* str,char* chrArray)
{
	for(char* p1=str;p1[0]!='\0';++p1)
	{
		bool ret=true;
		for(char* p2=chrArray;p2[0]!='\0';++p2)
		{
			if(p1[0]==p2[0])
				ret=false;
		}
		if(ret)
			return p1;
	}
	return p1;
}
//�����ƕ�������ׂ�
bool chrstr(int chr,char* chrArray)
{
	for(char* p=chrArray;p[0];++p)
		if(p[0]==chr)
			return true;
	return false;
}
//������̍ŏ��ƍŌ���|�C���^�Ŏw�肵�Ăm��������
char* newstr(char* pFront,char* pRear)
{
	char* ret = new char[pRear-pFront+1];
	strncpy(ret,pFront,pRear-pFront);
	ret[pRear-pFront] = '\0';
	return ret;
}
//��؂蕶��c�ŋ�؂�ꂽ������Qstr2�ƍŏ��̂���������v���邩
bool strstrstr(char* str1,char* str2,int c)
{
	char* pFront=str2;
	char* pRear;
	while(pRear = strchr(pFront,c))
	{
		char* buffer=newstr(pFront,pRear);
		if(strstr(str1,buffer) == str1)
		{
			SAFE_DELETE_ARRAY(buffer);
			return true;
		}
		SAFE_DELETE_ARRAY(buffer);
		pFront=pRear+1;
	}
	//�Ō�
	if(strstr(str1,pFront)==str1)
		return true;
	return false;
}
//��؂蕶��c�ŋ�؂�ꂽ������Qstr2�Ɗ��S�Ɉ�v���邩
bool strstrstr2(char* str1,char* str2,int c)
{
	char* pFront=str2;
	char* pRear;
	while(pRear = strchr(pFront,c))
	{
		char* buffer=newstr(pFront,pRear);
		if(strcmp(str1,buffer) == 0)
		{
			SAFE_DELETE_ARRAY(buffer);
			return true;
		}
		SAFE_DELETE_ARRAY(buffer);
		pFront=pRear+1;
	}
	//�Ō�
	if(strcmp(str1,pFront)==0)
		return true;
	return false;
}
//�����񂪒萔������킷�g�[�N�����ǂ���
bool IsConst(char* str)
{
	return chrstr(str[0],"0123456789")||chrstr(str[0],"+-.") && chrstr(str[1],"0123456789");
}
//�����񂪕ϐ�������킷�g�[�N�����ǂ���
bool IsVar(char* str)
{
	return chrstr(str[0],"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_") &&
		strstr2(str,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789")[0]=='\0';
}
//�t�@�C���ǂݍ���
bool ReadFile(char* buffer,int size,const char* lpszFileName)
{
	HANDLE hFile;
	hFile = CreateFile(lpszFileName,
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;
	//��������ǂݍ���
	DWORD dwSize;
	int ret = ReadFile(
		hFile,
		buffer,
		size,
		&dwSize,
		NULL);
	ret &= CloseHandle(hFile);
	if(ret)
		buffer[dwSize] = NULL;
	return (ret!=0);
}
//�t�@�C���ǂݍ��݁E�����o�b�t�@�쐬�o�[�W����
char* ReadFile(const char* lpszFileName)
{
	HANDLE hFile;
	hFile = CreateFile(lpszFileName,
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;
	//�t�@�C���T�C�Y�𓾂�
	DWORD size;
	size=GetFileSize(hFile,NULL);
	char *ret=new char[size];
	//��������ǂݍ���
	DWORD dwSize;
	int flag = ReadFile(
		hFile,
		ret,
		size,
		&dwSize,
		NULL);
	flag &= CloseHandle(hFile);
	if(flag)
	{
		ret[dwSize] = NULL;
		return ret;
	}
	else
	{
		SAFE_DELETE_ARRAY(ret);
		return NULL;
	}
}
//�t�@�C���T�C�Y
int GetFileSize(const char* lpszFileName)
{
	HANDLE hFile;
	hFile = CreateFile(lpszFileName,
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;
	int ret=GetFileSize(hFile,NULL);
	CloseHandle(hFile);
	return ret;
}
/*-----------------------------------------------------------------------------
	�g�[�N���p�N���X�E�\���̂̎���
-----------------------------------------------------------------------------*/
CToken::CToken()
{
	m_pString=NULL;
}
//�g�[�N����؂�(������ւ̃|�C���^�A�O�̃g�[�N���j
char* CToken::Cut(char* str,char* former)
{
	if(str==NULL||str[0]=='\0')
	{
		m_pString=NULL;
		return NULL;
	}
	char* pFront = str;
	//str�𐮕������̂Ԃ�i�߂�
	while(chrstr(pFront[0]," \t\n\r")!=NULL)	//\���͉��s�����̈��
	{
		pFront++;
		if(pFront[0]=='\0')
			return NULL;
	}
	//C�R�����g�̏ꍇ
	if(strstr(pFront,"/*")==pFront)
	{
		char* p=strstr(pFront,"*/");
		if(p==NULL)
			return NULL;
		return Cut(p+2,former);
	}
	//C++�R�����g�̏ꍇ
	else if(strstr(pFront,"//")==pFront)
	{
		char* p=strstr(pFront,"\r\n");
		if(p==NULL)
			return NULL;
		return Cut(p+1,former);
	}
	//�萔�̏ꍇ
	else if(chrstr(pFront[0],"0123456789")||	//�ŏ�������
		chrstr(pFront[0],"+-")&&chrstr(pFront[1],"0123456789")&&
		(former==NULL||
		!IsConst(former)&&!IsVar(former)&&strcmp(former,")")!=0))	//�����{�����i���Z�q�́{�|�Ƌ�ʁj
	{
		char* pRear = strstr2(pFront+1,".0123456789");
		m_pString = newstr(pFront,pRear);
		return pRear;
	}
	//������̏ꍇ
	else if(strstr(pFront,"\"")==pFront)
	{
		char* pRear = strchr(pFront+1,'\"');
		if(pRear!=NULL)
			m_pString = newstr(pFront,pRear+1);
		return pRear+1;
	}
	//�񕶎����Z�q�̏ꍇ
	else if(strstrstr(pFront,"+=,-=,++,--,==,!=,<=,>=,||,&&",','))
	{
		m_pString = newstr(pFront,pFront+2);
		return pFront+2;
	}
	//���̑��̋L���̏ꍇ
	else if(chrstr(pFront[0],"()+-*=/<>&|!#%'~^[]{}@`?:;,."))
	{
		m_pString = newstr(pFront,pFront+1);
		return pFront+1;
	}
	//�֐�����ϐ����̏ꍇ
	else if(chrstr(pFront[0],"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"))
	{
		char* pRear = strstr2(pFront,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789");
		m_pString = newstr(pFront,pRear);
		return pRear;
	}
	else
	{
		char buffer[2]={pFront[0],'\0'};
		MBF_ERR("%s:�g�[�N���Ƃ��ĔF���ł��܂���B",buffer);
		return NULL;
	}
}
//�f�X�g���N�^
CTokenArray::~CTokenArray()
{
	for(int i=0;i<m_vctpToken.size();++i)
		SAFE_DELETE(m_vctpToken[i]);
}
//�g�[�N���֐؂蕪����
bool CTokenArray::CutToToken(char* str)
{
	char* p=str;
	do
	{
		char* former=NULL;
		if(m_vctpToken.size()>0)
			former=(*(m_vctpToken.end()-1))->GetStr();
		CToken* pToken = new CToken;
		p = pToken->Cut(p,former);
		if(p!=NULL&&pToken->GetStr()!=NULL)
			m_vctpToken.push_back(pToken);
		else
			SAFE_DELETE(pToken);
	}
	while(p);

	return true;
}
//�Sprintf
void CTokenArray::allprintf()
{
	for(int i=0;i<m_vctpToken.size();++i)
		m_vctpToken[i]->Printf();
}
//�i�A�o�ɑΉ�����j�A�p�𓾂�
int CTokenArray::GetCorrespondence(int index,const char* kakko1,const char* kakko2)
{
	int i;
	int depth=0;
	for(i=index;i!=m_vctpToken.size() && m_vctpToken[i]!=NULL;++i)
	{
		if(m_vctpToken[i]->GetStr()!=NULL)
		{
			if(*m_vctpToken[i]==kakko2)
			{
				depth--;
				if(depth==0)
					return i;
			}
			else if(*m_vctpToken[i]==kakko1)
				depth++;
		}
	}
	return -1;
}
//���������������
int CTokenArray::Search(int index,const char* str)
{
	for(int i=index;i!=m_vctpToken.size();++i)
		if(*m_vctpToken[i]==str)
			return i;
	return -1;
}
//�J���}����������B���̂����A�i�j�Ɓo�p�ɂ��u�[���v������Ă����疳������B
int CTokenArray::SearchCommma(int first,int last)
{
	int depth=0;
	for(int i=first;i<last;++i)
	{
		if(*m_vctpToken[i]=="," && depth==0)
			return i;
		if(*m_vctpToken[i]=="(")
			depth++;
		if(*m_vctpToken[i]==")")
			depth--;
	}
	return -1;
}
//��؂蕶�����ŋ�؂�ꂽ������S�������i���Z�q�j����������B
int CTokenArray::SearchEnzansi(char* str,int c,int first,int last)
{
	int depth=0;
	for(int i=first;i<last;++i)
	{
		if(strstrstr2(m_vctpToken[i]->GetStr(),str,c) && depth ==0)
			return i;
		if(*m_vctpToken[i]=="(")
			depth++;
		if(*m_vctpToken[i]==")")
			depth--;
	}
	return -1;
}
//�D�揇�ʂ�Ԃ�
int TokenIndex::Get()const
{
	char* p=(*m_pTokenArray)[m_index]->GetStr();
	if(strstrstr2(p,"=,+=,-=",',' ))
		return 0;
	if(strstr(p,"||")==p)
		return 10;
	if(strstr(p,"&&")==p)
		return 20;
	if(strstrstr2(p,"==,!=",','))
		return 30;
	if(strstrstr2(p,"<,<=,>,>=",','))
		return 40;
	if(strstrstr2(p,"+,-",','))
		return 50;
	if(strstrstr2(p,"*,/,%",','))
		return 60;
	//���̑�
	return -1;
}
/*-----------------------------------------------------------------------------
		��������{��
-----------------------------------------------------------------------------*/
//�����񂩂�ϐ������
CVar* CLogicalBlock::NewVariable(const char* str)
{
	//pBlock�̕ϐ����Əƍ�
	for(int i=0;i<GetVarNum();++i)
	{
		if(strcmp(str,GetVarNameArray()[i]) == 0)
		{
			return new CVariable(this,i);
		}
	}
	//pBlock�̐e�u���b�N�֊ۓ���
	if(GetBlock())
		return GetBlock()->NewVariable(str);
	return NULL;
}
//�u���b�N�I�u�W�F�N�g�̃f�X�g���N�^
CLogicalBlock::~CLogicalBlock()
{
	list<CFunc*>::iterator itr;
	for(itr = m_lstpFunc.begin(); itr != m_lstpFunc.end(); ++itr)
		SAFE_DELETE(*itr);
	m_lstpFunc.clear();
}
//�u���b�N�փg�[�N������ǂݍ���
bool CLogicalBlock::Read(CBlock* pBlock,CTokenArray* pTokenArray,int first)
{
	m_pBlock=pBlock;
	m_pVar=NULL;
	m_varnum=0;
	for(int i=0;i<VARNUM;++i)
		m_varname[i]=NULL;
	int last = pTokenArray->GetCorrespondence(first,"{","}");
	if(last==-1)
	{
		MB_ERR("{�ɑΉ�����}������܂���B");
		return false;
	}
	for(i=first+1;i!=last;++i)
	{
		if(*(*pTokenArray)[i]=="double" || *(*pTokenArray)[i]=="var")	//�ϐ��錾��
		{
			++i;
			for(;i!=last&&(*pTokenArray)[i]!=NULL;++i)
			{
				if(IsVar((*pTokenArray)[i]->GetStr()))
				{
					m_varname[m_varnum]=_strdup(((*pTokenArray)[i])->GetStr());
					m_varnum++;
					i++;
					if(*(*pTokenArray)[i]==";")
						break;
					else if(*(*pTokenArray)[i]==",")
						continue;
					else
					{
						MBF_ERR("%s:�ϐ��錾�����̃g�[�N���Ƃ��ĕs�K�؂ł��B",(*pTokenArray)[i]->GetStr());
						return false;;
					}
				}
				else
				{
					MBF_ERR("%s:�ϐ����Ƃ��ĕs�K�؂ł��B",(*pTokenArray)[i]->GetStr());
					return false;
				}
			}
		}
		else//�֐���
		{
			int last2=i;
			CFunc* pFunc = NewFunc2(this,pTokenArray,i,last2);
			if(pFunc)
				m_lstpFunc.push_back(pFunc);
			i=last2;
		}
	}
	for(i=0;i<VARNUM;++i)
	{
		if(m_varname[i])
		{
			SAFE_DELETE_ARRAY(m_varname[i]);
		}
	}
	return true;
}
//�g�[�N�����X�g���當�����(�ŏ��w��j
//if,while�n�̐��䕶�Ŏg��
CFunc* NewFunc2(CBlock* pBlock,CTokenArray* pTokenArray,int first,int& last)
{
	int i=first;
	if(*(*pTokenArray)[i]==";") //��
	{
		last++;
		return NewFunc2(pBlock,pTokenArray,i+1,last);
	}
	else if(*(*pTokenArray)[i]=="{") //�u���b�N��
	{
		last = pTokenArray->GetCorrespondence(i,"{","}");
		if(last<0) return NULL;
		CLogicalBlock* pBlock2 = new CLogicalBlock;
		if(pBlock2->Read(pBlock,pTokenArray,i))
			return pBlock2;
		else 
		{
			SAFE_DELETE(pBlock);
			return NULL;
		}
	}
	else if(*(*pTokenArray)[i]=="if") //if
	{
		if(*(*pTokenArray)[i+1]!="(")
		{
			MB_ERR("if��(������܂���B");
			return NULL;
		}
		int last2 = pTokenArray->GetCorrespondence(i+1,"(",")");
		if(last2<0)
		{
			MB_ERR("if��)������܂���B");
			return NULL;
		}
		CFunc* pJoken = NewFunc(pBlock,pTokenArray,i+2,last2);	//������
		CFunc* pFunc = NewFunc2(pBlock,pTokenArray,last2+1,last);
		CFunc* pElse = NULL;
		if(*(*pTokenArray)[last+1]=="else")
			pElse = NewFunc2(pBlock,pTokenArray,last+2,last);
		if(pJoken && pFunc)
			return new CIf(pJoken,pFunc,pElse);
		SAFE_DELETE(pJoken);
		SAFE_DELETE(pBlock);
		SAFE_DELETE(pElse);
		return NULL;
	}
	else if(*(*pTokenArray)[i]=="while") //while
	{
		if(*(*pTokenArray)[i+1]!="(")
		{
			MB_ERR("while��(������܂���B");
			return NULL;
		}
		int last2 = pTokenArray->GetCorrespondence(i+1,"(",")");
		if(last2<0)
		{
			MB_ERR("while��)������܂���B");
			return NULL;
		}
		CFunc* pJoken = NewFunc(pBlock,pTokenArray,i+2,last2);	//������
		CFunc* pFunc = NewFunc2(pBlock,pTokenArray,last2+1,last);
		if(pJoken && pFunc)
			return new CWhile(pJoken,pFunc);
		SAFE_DELETE(pJoken);
		SAFE_DELETE(pFunc);
		return NULL;
	}
	else if(*(*pTokenArray)[i]=="for") //for
	{
		if(*(*pTokenArray)[i+1]!="(")
		{
			MB_ERR("for��(������܂���B");
			return NULL;
		}
		int last2 = pTokenArray->GetCorrespondence(i+1,"(",")");
		if(last2<0)
		{
			MB_ERR("for��)������܂���B");
			return NULL;
		}
		int colon1=pTokenArray->Search(i+1,";");
		if(colon1<0)
		{
			MB_ERR("for��;������܂���B");
			return NULL;
		}
		int colon2=pTokenArray->Search(colon1+1,";");
		if(colon2<0)
		{
			MB_ERR("for��;������܂���B");
			return NULL;
		}
		CFunc* pFirst = NewFunc(pBlock,pTokenArray,i+2,colon1);		//��������
		CFunc* pJoken = NewFunc(pBlock,pTokenArray,colon1+1,colon2);//������
		CFunc* pStep = NewFunc(pBlock,pTokenArray,colon2+1,last2);	//��O��
		CFunc* pFunc = NewFunc2(pBlock,pTokenArray,last2+1,last);	//�u���b�N
		if(pFirst && pJoken && pStep && pFunc)
			return new CFor(pFirst,pJoken,pStep,pFunc);
		SAFE_DELETE(pFirst);
		SAFE_DELETE(pJoken);
		SAFE_DELETE(pStep);
		SAFE_DELETE(pFunc);
		return NULL;
	}
	else if(*(*pTokenArray)[i]=="break")
	{
		int gyou=pTokenArray->Search(i,";");
		if(gyou!=i+1)
		{
			MB_ERR("break��;������܂���B");
			return NULL;
		}
		last=gyou;
		return new CBreak();
	}
	else//�֐���
	{
		int gyou=pTokenArray->Search(i,";");
		last = gyou;
		if(gyou==-1)
		{
			MB_ERR(";������܂���B");
			return NULL;
		}
		CFunc* pFunc = NewFunc(pBlock,pTokenArray,i,gyou);
		if(pFunc)
			return pFunc;	//���̕����񂩂�֐��I�u�W�F�N�g�����
	}
	return NULL;
}
//�g�[�N�����X�g����֐������
CFunc* NewFunc(CBlock* pBlock,CTokenArray* pTokenArray,int first,int last)
{
	CFunc* ret=NULL;
	int i=first;
	//�萔
	if(IsConst((*pTokenArray)[i]->GetStr()) && last==i+1)
		return new CConst(atof((*pTokenArray)[i]->GetStr()));
	//�J�b�R��
	if(*(*pTokenArray)[i]=="(" && pTokenArray->GetCorrespondence(i,"(",")")==last-1)
	{
		return NewFunc(pBlock,pTokenArray,first+1,last-1);
	}
	//���Z�q
	if(pTokenArray->SearchEnzansi("=,+=,-=,||,&&,==,!=,<,<=,>,>=,+,-,*,/,%",',',first,last) >=0)
	{
		vector<TokenIndex> vctTokenIndex;	//�C���f�b�N�X���F�N�^�[�i�\�[�g�p�j
		int j=pTokenArray->SearchEnzansi("=,+=,-=,||,&&,==,!=,<,<=,>,>=,+,-,*,/,%",',',first,last);
		while(j>=0)
		{
			//printf("%d:%s\n",j,(*pTokenArray)[j]->GetStr());
			TokenIndex tokenIndex;
			tokenIndex.m_index = j;
			tokenIndex.m_pTokenArray = pTokenArray;
			vctTokenIndex.push_back(tokenIndex);	//���F�N�^�[�ɒǉ�
			j=pTokenArray->SearchEnzansi("=,+=,-=,||,&&,==,!=,<,<=,>,>=,+,-,*,/,%",',',j+1,last);
		}
		stable_sort(vctTokenIndex.begin(),vctTokenIndex.end());	//���l�ȕ��я��͕ێ����ă\�[�g
		int index=(vctTokenIndex.end()-1)->m_index;	//�ł��D�揇�ʂ̒Ⴂ���Z�q�ւ̃C���f�b�N�X
		int H1F=first;
		int H1L=index;
		int H2F=index+1;
		int H2L=last;
		//�d���������n���Z�q
		CVar* pVariable=NULL;
		CFunc* pFunc=NULL;
		if(*(*pTokenArray)[index]=="=")
		{
			pVariable = pBlock->NewVariable((*pTokenArray)[H1F]->GetStr());
			pFunc = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pVariable && pFunc)
				return new CEqual(pVariable,pFunc);
		}
		else if(*(*pTokenArray)[index]=="+=")
		{
			pVariable = pBlock->NewVariable((*pTokenArray)[H1F]->GetStr());
			pFunc = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pVariable && pFunc)
				return new CPlusEqual(pVariable,pFunc);
		}
		else if(*(*pTokenArray)[index]=="-=")
		{
			pVariable = pBlock->NewVariable((*pTokenArray)[H1F]->GetStr());
			pFunc = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pVariable && pFunc)
				return new CMinusEqual(pVariable,pFunc);
		}
		SAFE_DELETE(pVariable);
		SAFE_DELETE(pFunc);
		//�ʏ�n�Q�����Z�q
		CFunc* pFunc1=NULL;
		CFunc* pFunc2=NULL;
		if(*(*pTokenArray)[index]=="+")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CPlus(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="-")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CMinus(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="*")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CMultiply(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="/")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CDivide(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="%")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CSurplus(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="==")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CEqualEqual(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="!=")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CNotEqual(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]==">")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CIsLarger(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="<")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CIsSmaller(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]==">=")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CIsLargerEqual(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="<=")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CIsSmallerEqual(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="&&")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new CAndAnd(pFunc1,pFunc2);
		}
		else if(*(*pTokenArray)[index]=="||")
		{
			pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
			pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
			if(pFunc1 && pFunc2)
				return new COrOr(pFunc1,pFunc2);
		}
		SAFE_DELETE(pFunc1);
		SAFE_DELETE(pFunc2);
		MBF_ERR("%s: ���̉��Z�q�͖������ł��B",(*pTokenArray)[index]->GetStr());
		return NULL;
	}
	//�c��͕ϐ����֐�
	if(IsVar((*pTokenArray)[i]->GetStr()))	//���ʎq�ł��邱�Ƃ��m�F���Ă���
	{
		//�ϐ�
		CVar* pVar = pBlock->NewVariable((*pTokenArray)[i]->GetStr());
		if(pVar!=NULL)
			return pVar;
		//�֐�
		if(i+1==last)return NULL;
		int corKakko=pTokenArray->GetCorrespondence(i+1,"(",")");//�i�ɑΉ�����j
		if(*(*pTokenArray)[i+1]=="(" && corKakko!=-1)
		{
			if(corKakko!=last-1)
			{
				MBF_ERR("%s:�@�G������܂���B",(*pTokenArray)[i]->GetStr());
				return NULL;
			}
			{//�����Ȃ��֐�
				int H1L=pTokenArray->GetCorrespondence(first+1,"(",")");//�i�ɑΉ�����j
				if(H1L<0) return NULL;
				if(*(*pTokenArray)[i]=="rnd")
					return new CRnd;
				if(*(*pTokenArray)[i]=="WaitTime")
					return new CWaitTime;
				if(*(*pTokenArray)[i]=="UpdateInput")
					return new CUpdateInput;
				if(*(*pTokenArray)[i]=="DrawList")
					return new CDrawList;
				if(*(*pTokenArray)[i]=="ShowFPS")
					return new CShowFPS;
				if(*(*pTokenArray)[i]=="Flip")
					return new CFlip;
				if(*(*pTokenArray)[i]=="ShotToItem")
					return new CShotToItem;
			}
			{//�P�����֐�
				CFunc* ret=NULL;
				int H1F=first+2;	//����1�̎n�܂�
				if(H1F>=last) return NULL;
				int H1L=pTokenArray->GetCorrespondence(first+1,"(",")");//�i�ɑΉ�����j
				if(H1L<0) return NULL;

				if(*(*pTokenArray)[i]=="sin")
				{
					CFunc* pFunc = NewFunc(pBlock,pTokenArray,H1F,H1L);
					if(pFunc!=NULL)
						return new CSin(pFunc);
				}
				if(*(*pTokenArray)[i]=="cos")
				{
					CFunc* pFunc = NewFunc(pBlock,pTokenArray,H1F,H1L);
					if(pFunc!=NULL)
						return new CCos(pFunc);
				}
				if(*(*pTokenArray)[i]=="tan")
				{
					CFunc* pFunc = NewFunc(pBlock,pTokenArray,H1F,H1L);
					if(pFunc!=NULL)
						return new CTan(pFunc);
				}
				if(*(*pTokenArray)[i]=="atan")
				{
					CFunc* pFunc = NewFunc(pBlock,pTokenArray,H1F,H1L);
					if(pFunc!=NULL)
						return new CAtan(pFunc);
				}
				if(*(*pTokenArray)[i]=="sqrt")
				{
					CFunc* pFunc = NewFunc(pBlock,pTokenArray,H1F,H1L);
					if(pFunc!=NULL)
						return new CSqrt(pFunc);
				}
				if(*(*pTokenArray)[i]=="abs")
				{
					CFunc* pFunc = NewFunc(pBlock,pTokenArray,H1F,H1L);
					if(pFunc!=NULL)
						return new CAbs(pFunc);
				}
				if(*(*pTokenArray)[i]=="GetInputState")
				{
					CFunc* pFunc = NewFunc(pBlock,pTokenArray,H1F,H1L);
					if(pFunc!=NULL)
						return new CGetInputState(pFunc);
				}
				if(*(*pTokenArray)[i]=="GetInputEvent")
				{
					CFunc* pFunc = NewFunc(pBlock,pTokenArray,H1F,H1L);
					if(pFunc!=NULL)
						return new CGetInputEvent(pFunc);
				}
			}
			{//�Q�����֐�
				int H1F=i+2;	//����1�̎n�܂�
				int H1L=pTokenArray->SearchCommma(i+2,corKakko);	//����1�̍Ō�
				if(H1L!=-1)
				{
					int H2F=H1L+1;
					int H2L=corKakko;
					//�ʏ�֐�
					CFunc* pFunc1=NULL;
					CFunc* pFunc2=NULL;
					if(*(*pTokenArray)[i]=="Plus")
					{
						pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
						if(pFunc1 && pFunc2)
							return new CPlus(pFunc1,pFunc2);
					}
					else if(*(*pTokenArray)[i]=="Minus")
					{
						pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
						if(pFunc1 && pFunc2)
							return new CMinus(pFunc1,pFunc2);
					}
					else if(*(*pTokenArray)[i]=="atan2")
					{
						pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
						if(pFunc1 && pFunc2)
							return new CAtan2(pFunc1,pFunc2);
					}
					else if(*(*pTokenArray)[i]=="random")
					{
						pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
						if(pFunc1 && pFunc2)
							return new CRandom(pFunc1,pFunc2);
					}
					else if(*(*pTokenArray)[i]=="mod")
					{
						pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
						if(pFunc1 && pFunc2)
							return new CMod(pFunc1,pFunc2);
					}
					else if(*(*pTokenArray)[i]=="PlaySound")
					{
						pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
						if(pFunc1 && pFunc2)
							return new CPlaySound(pFunc1,pFunc2);
					}
					else if(*(*pTokenArray)[i]=="LoadImage")
					{
						pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						char* str=newstr((*pTokenArray)[H2L-1]->GetStr()+1,strchr((*pTokenArray)[H2L-1]->GetStr()+1,'\"'));
						if(pFunc1)
							return new CLoadImage(pFunc1,str);
					}
					else if(*(*pTokenArray)[i]=="LoadSound")
					{
						pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						char* str=newstr((*pTokenArray)[H2L-1]->GetStr()+1,strchr((*pTokenArray)[H2L-1]->GetStr()+1,'\"'));
						if(pFunc1)
							return new CLoadSound(pFunc1,str);
					}
					else if(*(*pTokenArray)[i]=="Tikei")
					{
						pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						pFunc2 = NewFunc(pBlock,pTokenArray,H2F,H2L);
						if(pFunc1 && pFunc2)
							return new CIsTikei(pFunc1,pFunc2);
					}
					SAFE_DELETE(pFunc1);
					SAFE_DELETE(pFunc2);

					if(*(*pTokenArray)[i]=="DrawImage")
					{
						int H2F=H1L+1;
						int H2L=pTokenArray->SearchCommma(H2F,corKakko);
						int H3F=H2L+1;
						int H3L=pTokenArray->SearchCommma(H3F,corKakko);
						int H4F=H3L+1;
						int H4L=corKakko;
						CFunc* pFunc1=NewFunc(pBlock,pTokenArray,H1F,H1L);
						CFunc* pFunc2=NewFunc(pBlock,pTokenArray,H2F,H2L);
						CFunc* pFunc3=NewFunc(pBlock,pTokenArray,H3F,H3L);
						CFunc* pFunc4=NewFunc(pBlock,pTokenArray,H4F,H4L);
						if(pFunc1&&pFunc2&&pFunc3&&pFunc4)
							return new CDraw(pFunc1,pFunc2,pFunc3,pFunc4);
						SAFE_DELETE(pFunc1);
						SAFE_DELETE(pFunc2);
						SAFE_DELETE(pFunc3);
						SAFE_DELETE(pFunc4);
					}
					/*else if(*(*pTokenArray)[i]=="DrawImageAdd")
					{
						int H2F=H1L+1;
						int H2L=pTokenArray->SearchCommma(H2F,corKakko);
						int H3F=H2L+1;
						int H3L=pTokenArray->SearchCommma(H3F,corKakko);
						int H4F=H3L+1;
						int H4L=corKakko;
						CFunc* pFunc1=NewFunc(pBlock,pTokenArray,H1F,H1L);
						CFunc* pFunc2=NewFunc(pBlock,pTokenArray,H2F,H2L);
						CFunc* pFunc3=NewFunc(pBlock,pTokenArray,H3F,H3L);
						CFunc* pFunc4=NewFunc(pBlock,pTokenArray,H4F,H4L);
						if(pFunc1&&pFunc2&&pFunc3&&pFunc4)
							return new CDrawAdd(pFunc1,pFunc2,pFunc3,pFunc4);
						SAFE_DELETE(pFunc1);
						SAFE_DELETE(pFunc2);
						SAFE_DELETE(pFunc3);
						SAFE_DELETE(pFunc4);
					}*/
					else if(*(*pTokenArray)[i]=="LoadImage2")
					{
						int H2F=H1L+1;
						int H2L=pTokenArray->SearchCommma(H2F,corKakko);
						int H3F=H2L+1;
						int H3L=pTokenArray->SearchCommma(H3F,corKakko);
						int H4F=H3L+1;
						int H4L=corKakko;
						CFunc* pFunc1 = NewFunc(pBlock,pTokenArray,H1F,H1L);
						char* str=newstr((*pTokenArray)[H2L-1]->GetStr()+1,strchr((*pTokenArray)[H2L-1]->GetStr()+1,'\"'));
						CFunc* pFunc3=NewFunc(pBlock,pTokenArray,H3F,H3L);
						CFunc* pFunc4=NewFunc(pBlock,pTokenArray,H4F,H4L);
						if(pFunc1&&str&&pFunc3&&pFunc4)
							return new CLoadImage2(pFunc1,str,pFunc3,pFunc4);
						SAFE_DELETE(pFunc1);
						SAFE_DELETE(pFunc3);
						SAFE_DELETE(pFunc4);
					}
				}
			}
			{//������vector�œn���������֐�
				int start=i+2;
				int final;
				vector<CFunc*> funcarray;//�m�d�v�����֐����X�g
				while((final=pTokenArray->SearchCommma(start,corKakko))> 0)
				{
					CFunc* pFunc=NewFunc(pBlock,pTokenArray,start,final);
					if(pFunc==NULL)
					{
						for(int i=0;i<funcarray.size();++i)
							SAFE_DELETE(funcarray[i]);
						return NULL;
					}
					funcarray.push_back(pFunc);
					start=final+1;
				}
				funcarray.push_back(NewFunc(pBlock,pTokenArray,start,corKakko));
				//funcarray����������
				if(*(*pTokenArray)[i]=="Shot")
					return new CShot(funcarray);
				else if(*(*pTokenArray)[i]=="DrawImage2")
					return new CDraw2(funcarray);
				else if(*(*pTokenArray)[i]=="Explode")
					return new CExplode(funcarray);
			}
			MBF_ERR("%s:�@���݂��Ȃ��֐��ł��B",(*pTokenArray)[i]->GetStr());
			return NULL;
		}
		else
		{
			MBF_ERR("%s:�֐��ɐ������J�b�R������܂���B",(*pTokenArray)[i]->GetStr());
			return NULL;
		}
	}
	else
	{
		MBF_ERR("%s%s:�����Ԉ���Ă܂��B",(*pTokenArray)[i]->GetStr(),(*pTokenArray)[last]->GetStr());
		return NULL;
	}
}
//�֐����X�g���s
double CLogicalBlock::Get()
{
	double* pVar = NULL;
	if(m_varnum != 0)
		m_pVar = new double[m_varnum];
	list<CFunc*>::iterator itr;
	//�S�֐������s
	for(itr = m_lstpFunc.begin(); itr != m_lstpFunc.end(); ++itr)
		if(*itr)
			(*itr)->Get();
	SAFE_DELETE_ARRAY(m_pVar);
	return 0;
}
//�����o�֐����s(�����Fthis�|�C���^
void CMemFunc::Run(CClassInstance* pInstance)
{
	m_pInstance = pInstance;	//CMemVar����Q�Ƃł���悤�Ƀ����o�ɓ���Ă���
	m_pBlock->Get();
}
//CMemVar��new����
CVar* CMemFunc::NewVariable(const char* str)
{
	//�f�t�H���g�����o�ϐ�
	//�i�����V���[�ǉ������j
	if(strcmp(str,"x")==0)
		return new CMemVar(this,-1);
	else if(strcmp(str,"y")==0)
		return new CMemVar(this,-2);
	else if(strcmp(str,"img")==0)
		return new CMemVar(this,-3);
	else if(strcmp(str,"hp")==0)
		return new CMemVar(this,-4);
	else if(strcmp(str,"anm")==0)
		return new CMemVar(this,-5);
	else if(strcmp(str,"px")==0)
		return new CMemVar(this,-10);
	else if(strcmp(str,"py")==0)
		return new CMemVar(this,-11);
	//�ϐ����Əƍ�
	for(int i=0;i<GetVarNum();++i)
	{
		if(strcmp(str,GetVarNameArray()[i]) == 0)
		{
			return new CMemVar(this,i);
		}
	}
	return NULL;
}
//�����o�֐��ǂݍ���
bool CMemFunc::Read(CClassDefine *pClassDef,CTokenArray* pTokenArray,int first)
{
	Outputf("%s\n",(*pTokenArray)[first]->GetStr());
	m_pClassDef = pClassDef;
	SAFE_DELETE(m_pBlock);
	m_pBlock = new CLogicalBlock;
	return m_pBlock->Read(this,pTokenArray,first);
}
//�C���X�^���X�̃R���X�g���N�^
/*CClassInstance::CClassInstance(CClassDefine* pDefine)
{
	m_pDefine=pDefine;
	m_vararray=new double[pDefine->GetVarNum()];
	for(int i=0;i<pDefine->GetVarNum();++i)
		m_vararray[i]=0;
	if(pDefine->GetConstructer()!=NULL)
		pDefine->GetConstructer()->Run(this);
}*/
//�����V���[��������
CClassInstance::CClassInstance(CClassDefine* pDefine,CEnemy* pEnemy)
{
	m_pDefine=pDefine;
	m_pEnemy=pEnemy;
	m_vararray=new double[pDefine->GetVarNum()];
	for(int i=0;i<pDefine->GetVarNum();++i)
		m_vararray[i]=0;
	if(pDefine->GetConstructer()!=NULL)
		pDefine->GetConstructer()->Run(this);
}
//�N���X��`�̓ǂݍ���
bool CClassDefine::Read(CTokenArray* pTokenArray,int first,char* str)
{
	m_varnum=0;
	for(int i=0;i<VARNUM;++i)
		m_varname[i]=NULL;
	int last = pTokenArray->GetCorrespondence(first,"{","}");
	if(last==-1)
	{
		MB_ERR("{�ɑΉ�����}������܂���B");
		return false;
	}
	//�����o�ϐ�
	for(i=first+1;i!=last;++i)
	{
		if(*(*pTokenArray)[i]=="double" || *(*pTokenArray)[i]=="var")//�����o�ϐ�
		{
			++i;
			for(;i!=last&&(*pTokenArray)[i]!=NULL;++i)
			{
				if(IsVar((*pTokenArray)[i]->GetStr()))
				{
					m_varname[m_varnum]=_strdup(((*pTokenArray)[i])->GetStr());
					m_varnum++;
					i++;
					if(*(*pTokenArray)[i]==";")
						break;
					else if(*(*pTokenArray)[i]==",")
						continue;
					else
					{
						MBF_ERR("%s:�ϐ��錾�����̃g�[�N���Ƃ��ĕs�K�؂ł��B",(*pTokenArray)[i]->GetStr());
						return false;;
					}
				}
				else
				{
					MBF_ERR("%s:�ϐ����Ƃ��ĕs�K�؂ł��B",(*pTokenArray)[i]->GetStr());
					return false;
				}
			}
		}
		else if(IsVar((*pTokenArray)[i]->GetStr()) && *(*pTokenArray)[i+1]=="(")//�����o�֐�
		{
			int kakko=pTokenArray->GetCorrespondence(i+1,"(",")");
			if(kakko<0)
			{
				MBF_ERR("%s:�����o�֐���)������܂���B",(*pTokenArray)[i]->GetStr());
				return false;
			}
			int tyukakko=pTokenArray->GetCorrespondence(kakko+1,"{","}");
			if(tyukakko<0)
			{
				MBF_ERR("%s:�����o�֐���{}������܂���B",(*pTokenArray)[i]->GetStr());
				return false;
			}
			i=tyukakko;
		}
		else
		{
			MBF_ERR("%s:���Ƃ��ĔF���ł��܂���B",(*pTokenArray)[i]->GetStr());
			return false;
		}
	}
	int funcname=pTokenArray->Search(first,str);
	if(funcname<0 || funcname>=last)
		m_pConstructer = NULL;
	else
	{
		m_pConstructer = new CMemFunc;
		if(!m_pConstructer->Read(this,pTokenArray,funcname+3))
			SAFE_DELETE(m_pConstructer);
	}
	funcname=pTokenArray->Search(first,"main");
	if(funcname<0 || funcname>=last)
	{
		MBF_ERR("main�֐�������܂���B");
		return false;
	}
	m_pMemFunc = new CMemFunc;
	return m_pMemFunc->Read(this,pTokenArray,funcname+3);
}
/*-----------------------------------------------------------------------------
	��������NkScript�֌W
-----------------------------------------------------------------------------*/
CNkScript::CNkScript(const char* filename)
{
	int size=GetFileSize(filename);
	char *buffer= new char [size+1];
	ReadFile(buffer,size,filename);
	if(buffer==NULL)
	{
		MBF_ERR("%s������܂���B",filename);
		throw;
	}
	CTokenArray tArray;
	tArray.CutToToken(buffer);
//	tArray.allprintf();
	for(int i=0;i<tArray.GetArray()->size()&&i>=0;++i)
	{
		if(*tArray[i]=="class")
		{
			m_pCDefine.push_back(new CLASSDEFINE(&tArray,i+1));
			i=tArray.GetCorrespondence(i+2,"{","}");
		}
		else if(IsVar(tArray[i]->GetStr()))
		{
			int kakko=tArray.GetCorrespondence(i+1,"(",")");
			if(kakko<0)
			{
				MBF_ERR("%s:�֐���)������܂���B",tArray[i]->GetStr());
				throw;
			}
			int tyukakko=tArray.GetCorrespondence(kakko+1,"{","}");
			if(tyukakko<0)
			{
				MBF_ERR("%s:�֐���{}������܂���B",tArray[i]->GetStr());
				throw;
			}
			m_pFunc.push_back(new FUNCDEFINE(&tArray,i));
			i=tyukakko;
		}
		else if(*tArray[i] != ";")
		{
			MBF_ERR("%s:���Ƃ��ĔF���ł��܂���B",tArray[i]->GetStr());
			throw;
		}
	}
	SAFE_DELETE_ARRAY(buffer);
}
CNkScript::~CNkScript()
{
	int i;
	for(i=0;i<m_pCDefine.size();++i)
		SAFE_DELETE(m_pCDefine[i]);
	for(i=0;i<m_pFunc.size();++i)
		SAFE_DELETE(m_pFunc[i]);
	g_image.DeleteAll();
	g_sound.DeleteAll();
}
int CNkScript::SearchClass(const char* name)
{
	for(int i=0;i<m_pCDefine.size();++i)
		if(strcmp(m_pCDefine[i]->m_name,name)==0)
			return i;
	return -1;
}
int CNkScript::SearchFunc(const char* name)
{
	for(int i=0;i<m_pFunc.size();++i)
		if(strcmp(m_pFunc[i]->m_name,name)==0)
			return i;
	return -1;
}
