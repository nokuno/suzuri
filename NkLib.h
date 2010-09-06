/*-----------------------------------------------------------------------------

	NkLib.h

-----------------------------------------------------------------------------*/

#ifndef AFX_NKLIB_H
#define AFX_NKLIB_H

//�Q�[�����
#define GAMETITLE "SUZURI SHOOTING VER 1.00��"
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240
#define SCREEN_BPP      16
#define _FPS 16.6
#define FPSSHOW_WIDTH	24
#define FPSSHOW_HEIGHT	16

//�⏕�}�N��
#define MB(str) MessageBox(NULL,str,GAMETITLE,MB_OK)
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete [] (p);	  (p)=NULL; } }
#define SAFE_ACQUIRE(p)  { if(p) { (p)->Acquire();	} }
#define SAFE_UNACQUIRE(p) { if(p) { (p)->Unacquire();} }

//���͏��
#define INP_NEUTRAL	-1
#define INP_RIGHT	0
#define INP_LEFT	1
#define INP_DOWN	2
#define INP_UP		3
#define INP_B0		4
#define INP_B1		5
#define INP_B2		6
#define INP_B3		7
#define INP_MAX		8

//�t�H���g�t���O
#define FONT_ITALIC		0x01
#define FONT_ULINE		0x02
#define FONT_LINEOUT	0x04

// 565 �� 555 �ϊ��}�N��
#define	to555(_a_) (((_a_ & _mask4) >> 1) | (_a_ & _mask5))
#define	to565(_a_) (((_a_ & _mask4) << 1) | (_a_ & _mask5))

class CNkLib;
class CNkImage;
class CDrawQue;
class CCalculate;
extern CNkLib* g_pNkLib;//�O���[�o���ȃ|�C���^
/*-----------------------------------------------------------------------------
	WinMain�֐�
	�A�v���P�[�V�����E�G���g���[�|�C���g
-----------------------------------------------------------------------------*/
int APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow );
/*-----------------------------------------------------------------------------
	WndProc�֐�
	�E�B���h�E�v���V�[�W��
-----------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

/*-----------------------------------------------------------------------------
  DRAWINFO�\����
  CDrawQue�ɃX�v���C�g���x�����������āA�\�[�g�ł���B
-----------------------------------------------------------------------------*/
typedef struct tagDRAWINFO
{
	int spr;	//�X�v���C�g���x���i�傫���قǎ�O�j
	CDrawQue *pDrawQue;
	
	bool operator<(tagDRAWINFO rinfo){return spr<rinfo.spr;}

	tagDRAWINFO(){}
	tagDRAWINFO(int s,CDrawQue *que){spr=s;pDrawQue=que;}
}DRAWINFO, *LPDRAWINFO;

/*-----------------------------------------------------------------------------

	CNkLib
	�E�B���h�E�̊Ǘ��̂ق��A�_�u���o�b�t�@�����O�E
	�L�[�{�[�h���R���g���[�����́E�l�h�c�h�Đ��ȂǁA
	�P�̃A�v���P�[�V�����ɂ��P�ł��ފ�{�I�ȋ@�\������

-----------------------------------------------------------------------------*/
class CNkLib  
{
public:
	CNkLib(HINSTANCE hInst);
	virtual ~CNkLib();
	//��{�@�\
	int InitWindow(int nCmdShow);
	int InitDirectDraw(bool bWindow,bool bDouble);
	int WaitTime();
	int Flip();
	int ShowFPS();
	int Text(char *txt,int x,int y,int size,int weight,DWORD dwFlag,COLORREF color,int layer);
	int Circle(int width,COLORREF color,int left,int top,int right,int bottom,int layer);
		int PlusGamma(int red,int green,int blue);	//����
		//�����g�p
		int UpdateBounds();
		int CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc2,LPDIRECTDRAWSURFACE7* lplpDDSurface);
		int BltFast(int iDestX,int iDestY,LPDIRECTDRAWSURFACE7 pdds,LPRECT prcSrc,bool trans);
		int BltStretch(LPRECT prcDest,LPDIRECTDRAWSURFACE7 pdds,LPRECT prcSrc,bool trans);
		int TextOutBase(char* text,int x,int y,int size,int weight,DWORD dwFlag,COLORREF color);
		int DrawCircle(int width,COLORREF color,int left,int top,int right,int bottom);
		//�u���b�N�]���֌W
		int BltEffect(int iDestX,int iDestY,LPDIRECTDRAWSURFACE7 pdds,LPRECT prcSrc,CCalculate* pCal);
		int BltEffectBase(int iDestX,int iDestY,LPDIRECTDRAWSURFACE7 pdds,LPRECT prcSrc,CCalculate* pCal);
		int Blt0To0(DWORD *pDest,DWORD *pSrc,int a_skip,int b_skip,int w,int h,CCalculate* pCal);
		int Blt1To0(DWORD *pDest,DWORD *pSrc,int a_skip,int b_skip,int w,int h,CCalculate* pCal);
		int Blt0To1(DWORD *pDest,DWORD *pSrc,int a_skip,int b_skip,int w,int h,CCalculate* pCal);
		int Blt1To1(DWORD *pDest,DWORD *pSrc,int a_skip,int b_skip,int w,int h,CCalculate* pCal);
		//�E�B���h�E�v���V�[�W�����t�����h�֐��ɂ��Ƃ�
		friend LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
public:
	//�C�����C��
	int		GetWindowWidth(){return SCREEN_WIDTH*(1+(int)m_bDouble);};
	int		GetWindowHeight(){return SCREEN_HEIGHT*(1+(int)m_bDouble);};
	int		UpdateDirectDraw(){return InitDirectDraw(m_bWindow,m_bDouble);};
	int		ChangeMode(){m_bWindow=!m_bWindow;m_bDouble=m_bWindow;return InitDirectDraw(m_bWindow,m_bDouble);};
	int		ChangeScreenMode(){m_bWindow=!m_bWindow;return InitDirectDraw(m_bWindow,m_bDouble);};
	int		ChangeScreenSize(){m_bDouble=!m_bDouble;return InitDirectDraw(m_bWindow,m_bDouble);};
	bool	GetWindow(){return m_bWindow;};
	bool	GetDouble(){return m_bDouble;};
	bool	SetWindow(bool bWindow){return m_bWindow = bWindow;};
	bool	SetDouble(bool bDouble){return m_bDouble = bDouble;};
	bool	IsOk(){return m_pDD && m_pddsPrimaryBuffer && m_pddsPrimaryBuffer;};
		//�K���}�R���g���[���i����
		int		InitGamma(){return m_pGammaControl->SetGammaRamp(0,&m_gammaInit);}
		int		SetGamma(int red,int green,int blue){InitGamma();return PlusGamma(red,green,blue);}
		int		SetGamma(double red,double green,double blue){return SetGamma(red*60000,green*60000,blue*6000);}
		//�C���[�W���X�g�֌W
		void	AddList(CNkImage* pimg){m_lstpImage.push_front(pimg);};
		void	ClearList(){m_lstpImage.clear();};
		void	RemoveList(CNkImage* pimg){m_lstpImage.remove(pimg);};
		int		ReloadAllImage();
		void 	ReleaseAllImage();
public:
	//�`��L���[�֌W
	int		DrawList();
	void	ClearDrawList(){m_lstDInfo.clear();};	//�C���[�W���j�������Ƃ��p
		void	AddDrawList(tagDRAWINFO dInfo){m_lstDInfo.push_front(dInfo);};
public:
	//Input�֌W
	int	InitInput();
	int UpdateInput();
	bool GetInputState2(int inp);
		bool	GetKeyboardState(int inp);
		bool	GetJoystickState(int inp);
		HRESULT CreateDeviceEx(REFGUID rguid);
		HRESULT SetJoystickProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph);
		HRESULT InitializeJoystick(REFGUID rguid);
		int ClearInput();
public:
	//Input�֌W�̃C�����C���֐�
	bool	GetInputEvent(int inp){return m_bPresentState[inp] && !m_bFormerState[inp];};
	bool	GetInputHanasi(int inp){return !m_bPresentState[inp] && m_bFormerState[inp];};
	bool	GetKeyboardState(BYTE key);
		bool	GetInputState(int inp){return m_bPresentState[inp];};
		int		GetLR(){return m_iLR;};
		int		GetUD(){return m_iUD;};
public:
	//Sound�֌W
	int InitSound();
	int InitMidi();
	int LoadMidi(char* lpszFileName, int num);
	int PlayMidi();
	int StopMidi();
		LPDIRECTSOUND GetDS(){return m_pDS;};
private:
	//�E�B���h�E�֌W�f�[�^�����o
	HWND		m_hWnd;		//�E�B���h�E�n���h��
	HINSTANCE	m_hInst;	//�C���X�^���X�n���h��
	HACCEL		m_hAccel;	//�L�[�{�[�h�A�N�Z�����[�^
	bool		m_bWindow;	//�E�B���h�E���[�h�t���O
	bool		m_bDouble;	//�Q�{���[�h�t���O
	bool		m_bActive;	//�A�N�e�B�u�t���O
	int			m_time;		//�P�t���O��timeGetTime�̒l
	RECT		m_rcClient;	//�N���C�A���g�̈�
	RECT		m_rcWindow;	//�E�B���h�E�̈�
	DWORD		m_dwOldTime;//�P�t���O�̂s������
	DWORD		m_dwFPS;	//�e�o�r
	DWORD		m_dwCount;	//�t���[���J�E���g
private:
	//Draw�֌W�f�[�^�����o
	LPDIRECTDRAW7			m_pDD;
	LPDIRECTDRAWSURFACE7	m_pddsPrimaryBuffer;	//�v���C�}���o�b�t�@
	LPDIRECTDRAWSURFACE7	m_pddsBackBuffer;		//�o�b�N�o�b�t�@
	LPDIRECTDRAWSURFACE7	m_pddsFPS;				//FPS�\���p

	list<CNkImage*>		m_lstpImage;	//�S�ẴC���[�W�̃|�C���^
	list<tagDRAWINFO>	m_lstDInfo;	//�`��L���[���X�g

	LPDIRECTDRAWGAMMACONTROL	m_pGammaControl;//�K���}�R���g���[���I�u�W�F�N�g�|�C���^
	DDGAMMARAMP					m_gammaInit;     //�����K���}�l
private:
	//Input�֌W�f�[�^�����o
	LPDIRECTINPUT8			m_pDI;
	LPDIRECTINPUTDEVICE8	m_pdidKeyboard;
	LPDIRECTINPUTDEVICE8	m_pdidJoystick;

	BYTE		m_bKeyState[256];	//�L�[�{�[�h���
	DIJOYSTATE2	m_diJoyState;		//�W���C�X�e�B�b�N���
	bool		m_bPresentState[INP_MAX];	//���݂̓������͏��
	bool		m_bFormerState[INP_MAX];	//�P�t���O�̓������͏��
	int			m_iLR;	//���݃A�N�e�B�u��LR
	int			m_iUD;	//���݃A�N�e�B�u��UD
private:
	//Sound�֌W�f�[�^�����o
	LPDIRECTSOUND m_pDS;
	IDirectMusicLoader*			m_pdml;
	IDirectMusicPerformance*	m_pdmp;
	IDirectMusicSegment*		m_pdms;
};

//�h���������֌W�R�[���o�b�N�֐��B
//�A�v�����g�p�����EpContext�ɂ�this��n���B

BOOL CALLBACK	EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
BOOL CALLBACK	EnumJoysticksCallback2( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
BOOL CALLBACK	EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );

#endif