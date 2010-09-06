/*-----------------------------------------------------------------------------

	NkImage.h

-----------------------------------------------------------------------------*/

#ifndef AFX_NKIMAGE_H
#define AFX_NKIMAGE_H

/*-----------------------------------------------------------------------------

	CNkImage
	�摜�N���X
	�P�̉摜�ɂ��P�̃C���X�^���X�����N���X
	�A���t�@�u�����h�Ȃǂ̍��x�ȕ`��ɉ����A
	�b�m���k�����ƘA�g���ĕ`��\�[�g�@�\�����`��L���[��������B

-----------------------------------------------------------------------------*/
class CNkImage  
{
public:
	CNkImage();
	CNkImage(char* lpszFileName,int width=0,int height=0);
	virtual ~CNkImage();
	void Release();
	int Load(char* lpszFileName,int width=0,int height=0);
	int Fill( DWORD dwColor);	//�F��_RGB�}�N���ō��B
	//�\�[�g���`��i�ʏ�͂�����g�p�j
	int DrawLayer(int iDestX,int iDestY,int layer,RECT* prcSrc,bool trans);
	int DrawLayerStretch(LPRECT prcDest,LPRECT prcSrc,int layer,bool trans);
		//�\�[�g�Ȃ�
		int Draw(int iDestX,int iDestY,RECT *prcSrc,bool trans){return g_pNkLib->BltFast(iDestX,iDestY,m_pdds,prcSrc,trans);}
		int DrawEffect(int iDestX,int iDestY,RECT *prcSrc,CCalculate* pCal){return g_pNkLib->BltEffect(iDestX,iDestY,m_pdds,prcSrc,pCal);}
		int DrawStretch(LPRECT prcDest,LPRECT prcSrc,bool trans){return g_pNkLib->BltStretch(prcDest,m_pdds,prcSrc,trans);}
		//�݊��p
		int Draw2(int sprLevel,int iDestX,int iDestY,int flameX=0,int flameY=0,bool trans=true)
		{
			flameY=flameY +  flameX / (m_bmpWidth / m_width);flameX =flameX % (m_bmpWidth / m_width);
			RECT rctSrc={flameX*GetWidth(),flameY*GetHeight(),(flameX+1)*GetWidth(),(flameY+1)*GetHeight()};
			return DrawLayer(iDestX,iDestY,sprLevel,&rctSrc,trans);
		}
		//�݊��p
		int Draw2(int sprLevel,VECT pos,int flameX=0,int flameY=0){ return Draw2(sprLevel,pos.x,pos.y,flameX,flameY);};
		//�����g�p
		int LoadBMP(char* lpszFileName);
		int LoadPNG(char* lpszFileName);
		int DrawBitmap( HBITMAP hBMP);
		int SetColorKey( DWORD dwColorKey );
		DWORD ConvertGDIColor( COLORREF dwGDIColor );
public:
	int Reload(){if(m_szFileName[0]!=NULL)return LoadPNG(m_szFileName);return 1;};
	int GetWidth(){return m_width;};
	int GetHeight(){return m_height;};
	void SetWH(int w,int h){m_width=w;m_height=h;}
	VECT GetVect(){return VECT(m_width,m_height);}
	LPDIRECTDRAWSURFACE7 GetSurface(){return m_pdds;}
	//�A�j���[�V�����t���[�����g������������
	int DrawLayer(VECT pos,int layer,RECT* prcSrc,bool trans=true)
	{ return DrawLayer(pos.x,pos.y,layer,prcSrc,trans);}
	int DrawLayer(VECT pos,int layer,int flameX=0,int flameY=0,bool trans=true)
	{
		int flame =flameX + flameY*(m_bmpWidth / m_width);
		flame = mod(flame,(m_bmpWidth / m_width)*(m_bmpHeight / m_height));
		flameX = flame % (m_bmpWidth / m_width);
		flameY = flame / (m_bmpWidth / m_width);
		RECT rcSrc={flameX*GetWidth(),flameY*GetHeight(),(flameX+1)*GetWidth(),(flameY+1)*GetHeight()};
		return DrawLayer(pos,layer,&rcSrc,trans);
	}
	int DrawLayer(int iDestX,int iDestY,int layer,int flameX,int flameY,bool trans)
	{return DrawLayer(VECT(iDestX,iDestY),layer,flameX,flameY,trans);}
	int DrawLayerStretch(int iDestX,int iDestY,double a,int layer,LPRECT prcSrc,bool trans=true)
	{
		RECT rcDest={
			iDestX-(prcSrc->right-prcSrc->left)*a/2,
			iDestY-(prcSrc->bottom-prcSrc->top)*a/2,
			iDestX+(prcSrc->right-prcSrc->left)*a/2,
			iDestY+(prcSrc->bottom-prcSrc->top)*a/2};
		return DrawLayerStretch(&rcDest,prcSrc,layer,trans);
	}
	int DrawLayerStretch(VECT pos,double a,int layer,LPRECT prcSrc,bool trans=true)
	{return DrawLayerStretch(pos.x,pos.y,a,layer,prcSrc,trans);}
	int DrawLayerStretch(VECT pos,double a,int layer,int flameX=0,int flameY=0,bool trans=true)
	{
		RECT rcSrc={flameX*GetWidth(),flameY*GetHeight(),(flameX+1)*GetWidth(),(flameY+1)*GetHeight()};
		return DrawLayerStretch(pos,a,layer,&rcSrc,trans);
	}
	int DrawLayerStretch(int iDestX,int iDestY,double a,int layer,int flameX=0,int flameY=0,bool trans=true)
	{return DrawLayerStretch(VECT(iDestX,iDestY),a,layer,flameX,flameY,trans);}
protected:
	LPDIRECTDRAWSURFACE7 m_pdds;
	int		m_bmpWidth;				//�a�l�o�̕�
	int		m_bmpHeight;			//�a�l�o�̍���
	int		m_width;				//��(�A�j���[�V�����p�j
	int		m_height;				//����(�A�j���[�V�����p�j
	char	m_szFileName[MAX_PATH]; //BMP�t�@�C����
public:
	/*-----------------------------------------------------------------------------
		�G�t�F�N�g�`��i�\�[�g���j
		�e���v���[�g�����o�֐��̎���(�������N���X�錾�ɕ`���˂΂Ȃ�Ȃ��Ƃ́E�E
	-----------------------------------------------------------------------------*/
	template<typename T> //CCalculate���e���v���[�g�ɂ���
	int DrawLayerEffect(T cal,int iDestX,int iDestY,int layer,RECT *prcSrc)
	{
		//��ʊO
		if(iDestX>=SCREEN_WIDTH || iDestY>=SCREEN_HEIGHT || iDestX+m_width<=0 || iDestY+m_height<=0)
			return 0;
		DRAWINFO dInfo;
		dInfo.spr = layer;
		RECT rctSrc;
		if(prcSrc)
		{
			rctSrc=*prcSrc;
		}
		else//NULL�̂Ƃ�
		{
			rctSrc.left=0;
			rctSrc.top=0;
			rctSrc.right=m_width;
			rctSrc.bottom=m_height;
		}
		dInfo.pDrawQue = new CDrawQueEffect<T>(this,iDestX,iDestY,rctSrc,cal);
		g_pNkLib->AddDrawList(dInfo);
		return 1;
	}
	template<typename T>
	int DrawLayerEffect( T cal,int iDestX,int iDestY,int layer=100, int flameX=0,int flameY=0)
	{
		RECT rctSrc={flameX*GetWidth(),flameY*GetHeight(),(flameX+1)*GetWidth(),(flameY+1)*GetHeight()};
		return DrawLayerEffect(cal,iDestX,iDestY,layer,&rctSrc);
	}
	template<typename T>
	int DrawLayerEffect(T cal,VECT pos,int layer=100, int flameX=0,int flameY=0)
	{return DrawLayerEffect(cal,pos.x,pos.y,layer,flameX,flameY);}
};
/*-----------------------------------------------------------------------------
  CDrawQue
  �`��L���[�̃C���^�[�t�F�[�X�B
  ���ꂳ������΃o�b�N�o�b�t�@�ɕ`��\�Ȃ�������́B
-----------------------------------------------------------------------------*/
class CDrawQue
{
public:
	virtual ~CDrawQue(){};
	virtual int Draw()=0;
};
/*-----------------------------------------------------------------------------
	�ʏ�`��̕`��L���[
-----------------------------------------------------------------------------*/
class CDrawNormal : public CDrawQue
{
public:
	CDrawNormal(){}
	CDrawNormal(CNkImage* pimg,int iDestX,int iDestY,RECT rctSrc,bool trans){m_pimg=pimg;m_iDestX=iDestX;m_iDestY=iDestY;m_rctSrc=rctSrc;m_trans=trans;}
	virtual int Draw(){return m_pimg->Draw(m_iDestX,m_iDestY,&m_rctSrc,m_trans);}
private:
	CNkImage* m_pimg;
	int m_iDestX;
	int m_iDestY;
	RECT m_rctSrc;
	bool m_trans;
};
/*-----------------------------------------------------------------------------
	�G�t�F�N�g�`��̕`��L���[�iCCaluculate�̃N���X�ɂ���ăe���v���[�g�ɂ���j
-----------------------------------------------------------------------------*/
template<typename T>
class CDrawQueEffect : public CDrawQue
{
public:
	CDrawQueEffect(){}
	CDrawQueEffect(CNkImage* pimg,int iDestX,int iDestY,RECT rctSrc,T cal)
	{
		m_pimg=pimg;m_iDestX=iDestX;m_iDestY=iDestY;m_rctSrc=rctSrc;m_cal=cal;
	}
	virtual int Draw(){return m_pimg->DrawEffect(m_iDestX,m_iDestY,&m_rctSrc,&m_cal);}
private:
	CNkImage* m_pimg;
	int m_iDestX;
	int m_iDestY;
	RECT m_rctSrc;
	T m_cal;
};
/*-----------------------------------------------------------------------------
	�g��k���`��̕`��L���[
-----------------------------------------------------------------------------*/
class CDrawQueStretch : public CDrawQue
{
public:
	CDrawQueStretch(){}
	CDrawQueStretch(CNkImage* pimg,RECT rctDest,RECT rctSrc,bool trans){m_pimg=pimg;m_rctDest=rctDest;m_rctSrc=rctSrc;m_trans=trans;}
	virtual int Draw(){return m_pimg->DrawStretch(&m_rctDest,&m_rctSrc,m_trans);}
private:
	CNkImage* m_pimg;
	RECT m_rctDest;
	RECT m_rctSrc;
	bool m_trans;
};
/*-----------------------------------------------------------------------------
	�e�L�X�g�`��̕`��L���[
-----------------------------------------------------------------------------*/
class CDrawQueText : public CDrawQue
{
public:
	CDrawQueText(){m_text=NULL;}
	CDrawQueText(char* text,int x,int y,int size,int weight,DWORD dwFlag,COLORREF color)
	{m_text=_strdup(text);m_x=x;m_y=y;m_size=size;m_weight=weight;m_dwFlag=dwFlag;m_color=color;}
	~CDrawQueText(){SAFE_DELETE_ARRAY(m_text);}
	virtual int Draw(){return g_pNkLib->TextOutBase(m_text,m_x,m_y,m_size,m_weight,m_dwFlag,m_color);}
private:
	char* m_text;
	int m_x,m_y,m_size,m_weight;
	DWORD m_dwFlag;
	COLORREF m_color;
};
/*-----------------------------------------------------------------------------
	�~�`��̕`��L���[
-----------------------------------------------------------------------------*/
class CDrawQueCircle : public CDrawQue
{
public:
	CDrawQueCircle(){}
	CDrawQueCircle(int width,COLORREF color,int left,int top,int right,int bottom)
	{m_width=width;m_color=color;m_left=left;m_top=top;m_right=right;m_bottom=bottom;}
	virtual int Draw(){return g_pNkLib->DrawCircle(m_width,m_color,m_left,m_top,m_right,m_bottom);}
private:
	int m_width;
	COLORREF m_color;
	int m_left,m_top,m_right,m_bottom;
	int layer;
};
/*=============================================================================
	�ȉ��A���Z�֐��ƂȂ�܂��B
	CDrawQue�C���^�[�t�F�[�X�N���X���p�����Ă܂��B
	�Q�s�N�Z���R�Q�r�b�g�𓯎��v�Z���č�������}���Ă܂��B
=============================================================================*/

// �u�����f�B���O�p�r�b�g�}�X�N
#define	_mask01	0x0000ffff	// H off mask
#define	_mask10	0xffff0000	// L off mask
#define	_mask1	0x3def3def	// 0011110111101111 rgb  mask 1
#define	_mask2	0x42104210	// 0100001000010000 rgb  mask 2
#define	_mask3	0xf79ef79e	// 1111011110011110 half mask
#define	_mask4	0xffc0ffc0	// 1111111111000000 565  mask 1
#define	_mask5	0x001f001f	// 0000000000011111 565  mask 2
#define	_mask6	0x7fff7fff	// 0111111111111111 sub  mask
#define COLORKEY1 0xf81ff81f
#define COLORKEY2 0xf81f0000
#define COLORKEY3 0x0000f81f
/*-----------------------------------------------------------------------------
	�s�N�Z�����Z�̃C���^�[�t�F�[�X
-----------------------------------------------------------------------------*/
class CCalculate
{
public:
	virtual DWORD Get(DWORD dest,DWORD src)=0;
};
/*-----------------------------------------------------------------------------
	�e�X�g�p�̒P���]��
-----------------------------------------------------------------------------*/
class CCalEqual : public CCalculate
{
public:
	virtual DWORD Get(DWORD dest,DWORD src){return src;};
};
/*-----------------------------------------------------------------------------
	NOT
-----------------------------------------------------------------------------*/
class CCalNot : public CCalculate
{
public:
	virtual DWORD Get(DWORD dest,DWORD src){return ~src;};
};
/*-----------------------------------------------------------------------------
	XOR
-----------------------------------------------------------------------------*/
class CCalXOr : public CCalculate
{
public:
	virtual DWORD Get(DWORD dest,DWORD src){return dest^src;};
};
/*-----------------------------------------------------------------------------
	AND
-----------------------------------------------------------------------------*/
class CCalAnd : public CCalculate
{
public:
	virtual DWORD Get(DWORD dest,DWORD src){return dest&src;};
};
/*-----------------------------------------------------------------------------
	Half
-----------------------------------------------------------------------------*/
class CCalHalf : public CCalculate
{
public:
	virtual DWORD Get(DWORD dest,DWORD src){return (dest & src) + (((dest ^ src) & _mask3) >> 1);};
};
/*-----------------------------------------------------------------------------
	TransHalf
-----------------------------------------------------------------------------*/
class CCalTransHalf : public CCalculate
{
public:
	virtual DWORD Get(DWORD dest,DWORD src)
	{
		if(src==COLORKEY1)
			return dest;
		if ((src & _mask10) == COLORKEY2) {
			return (dest & (src | _mask10)) +
				(((dest ^ (src | (dest & _mask10))) & _mask3) >> 1);
		}
		if ((src & _mask01) == COLORKEY3) {
			return (dest & (src | _mask01)) +
				(((dest ^ (src | (dest & _mask01))) & _mask3) >> 1);
		}
		return (dest & src) + (((dest ^ src) & _mask3) >> 1);
	};
};
/*-----------------------------------------------------------------------------
	���Z����
-----------------------------------------------------------------------------*/
class CCalAdd : public CCalculate
{
public:
	virtual DWORD Get(DWORD dest,DWORD src)
	{
		DWORD	c,d;

		dest = to555 (dest);
		src = to555 (src);

		c = ((dest & _mask1) + (src & _mask1)) ^ ((dest ^ src) & _mask2);
		d = (dest + src) - c;
		c = c | (d - (d >> 5));

		return to565 (c);
	};
};
/*-----------------------------------------------------------------------------
	���Z����
-----------------------------------------------------------------------------*/
class CCalSub : public CCalculate
{
public:
	virtual DWORD Get(DWORD dest,DWORD src)
	{
		DWORD	c,d;

		dest = to555 (dest);
		src = to555 (src);

		dest ^= _mask6;
		src ^= _mask6;

		c = ((dest & _mask1) + (src & _mask1)) ^ ((dest ^ src) & _mask2);
		d = dest + src - c;
		c = (c | (d - (d >> 5))) ^ _mask6;

		return to565 (c);
	};
};
/*-----------------------------------------------------------------------------
	���߂Ȃ��A���t�@�u�����h
	����:�O�`�R�Q�i�傫���قǓ����j
-----------------------------------------------------------------------------*/
#define ALPHA_MAX 32
class CCalAlpha : public CCalculate
{
public:
	CCalAlpha(){};
	CCalAlpha(DWORD alpha){if(alpha>ALPHA_MAX)alpha=ALPHA_MAX;m_alpha=ALPHA_MAX-alpha;m_beta=alpha;}
	virtual DWORD Get(DWORD dest,DWORD src);	//������NkImage.cpp��
private:
	DWORD m_alpha;
	DWORD m_beta;
};
/*-----------------------------------------------------------------------------
	���߂��A���t�@�u�����h
	����:�O�`�R�Q�i�傫���قǓ����j
-----------------------------------------------------------------------------*/
class CCalTransAlpha : public CCalculate
{
public:
	CCalTransAlpha(){};
	CCalTransAlpha(DWORD alpha){if(alpha>ALPHA_MAX)alpha=ALPHA_MAX;m_alpha=ALPHA_MAX-alpha;m_beta=alpha;}
	virtual DWORD Get(DWORD dest,DWORD src);	//������NkImage.cpp��
private:
	DWORD m_alpha;
	DWORD m_beta;
};
#endif
