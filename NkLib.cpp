/*-----------------------------------------------------------------------------

	NkLib.cpp
	�A�v���P�[�V�����̊�{�I�ȍ\����
	DirectX�֌W�́A������B��̃I�u�W�F�N�g������

-----------------------------------------------------------------------------*/
#include "stdapp.h"
#include "utility.h"
#include "NkLib.h"
#include "NkImage.h"
#include "NkSound.h"
/*-----------------------------------------------------------------------------
	CNkLib�N���X
	�B��̃O���[�o���ϐ��Ƃ��č쐬����
-----------------------------------------------------------------------------*/
CNkLib* g_pNkLib = NULL;

/*-----------------------------------------------------------------------------
	CNkLib�N���X�̃R���X�g���N�^
-----------------------------------------------------------------------------*/
CNkLib::CNkLib(HINSTANCE hInst)
{
	//���������[�N�`�F�b�N
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//�E�B���h�E�֌W
	m_hInst = hInst;
	m_hWnd = NULL;
	m_hAccel = NULL;
	m_bWindow = true;
	m_bDouble = false;
	m_bActive = true;
	m_time = 0;
	m_dwOldTime = timeGetTime();
	m_dwFPS = 0;
	m_dwCount = 0;

	//�c�������֌W
	m_pDD = NULL;
	m_pddsPrimaryBuffer = NULL;
	m_pddsBackBuffer = NULL;
	m_pddsFPS = NULL;
	m_pGammaControl = NULL;

	//�h���������֌W
	m_pDI = NULL;
	m_pdidKeyboard = NULL;
	m_pdidJoystick = NULL;
	for(int i=0;i<256;++i)
		m_bKeyState[i] = 0;
	ZeroMemory(&m_diJoyState, sizeof DIJOYSTATE2);
	for(i=0;i<INP_MAX;++i)
	{
		m_bPresentState[i] = FALSE;
		m_bFormerState[i] = FALSE;
	}
	m_iLR = 0;
	m_iUD = 0;

	//�r���������֌W
	CoInitialize(NULL);
	m_pDS = NULL;
	m_pdml = NULL;
	m_pdmp = NULL;
	m_pdms = NULL;
	srand(time(0));
}
/*-----------------------------------------------------------------------------
	CNkLib�N���X�̃f�X�g���N�^
-----------------------------------------------------------------------------*/
CNkLib::~CNkLib()
{
	ReleaseAllImage();
	SAFE_RELEASE(m_pGammaControl);
	SAFE_RELEASE(m_pddsFPS);
	SAFE_RELEASE(m_pddsBackBuffer);
	SAFE_RELEASE(m_pddsPrimaryBuffer);
	SAFE_RELEASE(m_pDD);
	SAFE_UNACQUIRE(m_pdidKeyboard);
	SAFE_UNACQUIRE(m_pdidJoystick);
	SAFE_RELEASE(m_pdidKeyboard);
	SAFE_RELEASE(m_pdidJoystick);
	SAFE_RELEASE(m_pDI);
	SAFE_RELEASE(m_pDS);
	SAFE_RELEASE(m_pdml);
	SAFE_RELEASE(m_pdmp);
	SAFE_RELEASE(m_pdms);
	CoUninitialize();
}
/*-----------------------------------------------------------------------------
	�E�B���h�E��������
-----------------------------------------------------------------------------*/
int CNkLib::InitWindow(int nCmdShow)
{
    WNDCLASSEX wc;

	//�E�B���h�E�N���X�̓o�^
    wc.cbSize        = sizeof(wc);
    wc.lpszClassName = TEXT(GAMETITLE);
    wc.lpfnWndProc   = WndProc;
    wc.style         = CS_VREDRAW | CS_HREDRAW;
    wc.hInstance     = m_hInst;
    wc.hIcon         = LoadIcon( m_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON) );
    wc.hIconSm       = LoadIcon( m_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON) );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName  = "";
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;

    if( RegisterClassEx( &wc ) == 0 )
	{
		MB("�E�B���h�E�N���X�̓o�^�Ɏ��s���܂���");
        return 0;
	}

	//�L�[�{�[�h�A�N�Z�����[�^�[
    m_hAccel = LoadAccelerators( m_hInst, MAKEINTRESOURCE(IDR_MAIN_ACCEL) );

	//�E�B���h�E�T�C�Y���v�Z
    DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
    DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
    DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );
    DWORD dwWindowWidth   = (DWORD)GetWindowWidth()  + dwFrameWidth * 2;
    DWORD dwWindowHeight  = (DWORD)GetWindowHeight() + dwFrameHeight * 2 + 
                            + dwCaptionHeight;

	//�E�B���h�E���쐬�E�\��
    DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX;
    m_hWnd = CreateWindowEx( 0, TEXT(GAMETITLE), 
                           TEXT(GAMETITLE),
                           dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
  	                       dwWindowWidth, dwWindowHeight, NULL, NULL, m_hInst, NULL );
    if( m_hWnd == NULL )
	{
		MB("�E�B���h�E�쐬�Ɏ��s");
		return 0;
	}
    ShowWindow( m_hWnd, nCmdShow );
    UpdateWindow( m_hWnd );
    GetWindowRect( m_hWnd, &m_rcWindow );
	return 1;
}
/*-----------------------------------------------------------------------------
	WaitTime�֐�
	�U�O�e�o�r�ɂȂ�悤�ɂ��Ȃ��烁�b�Z�[�W�̏���������
-----------------------------------------------------------------------------*/
int CNkLib::WaitTime()
{
    MSG	   msg;
	while(true)
	{
		while(true)
		{
			if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	        {
				if( 0 == GetMessage(&msg, NULL, 0, 0 ) )
				{
				    // WM_QUIT�����m
		            return 0;
			    }

	            //���b�Z�[�W��|��
	            if( 0 == TranslateAccelerator( m_hWnd, m_hAccel, &msg ) )
	            {
		            TranslateMessage( &msg ); 
			        DispatchMessage( &msg );
				}
			}
			else
				break;
		}
		//�A�N�e�B�u���ǂ������擾
		m_bActive = (GetActiveWindow()!=NULL);

        if( m_bActive )
        {
			int time = (int)timeGetTime();
			int timeDifferent = time - m_time;
			if( timeDifferent > _FPS )
			{
				m_time = time;
				break;
			}
        }
        else
        {
			//��A�N�e�B�u
			ClearInput();
            WaitMessage();
            m_time = timeGetTime();
        }
	}
	return 1;
}
/*-----------------------------------------------------------------------------
	InitDirectDraw�֐�
	DirectDraw������
-----------------------------------------------------------------------------*/
int CNkLib::InitDirectDraw(bool bWindow,bool bDouble)
{
	//���łɏ���������Ă�΁A�̂Ă�B
	SAFE_RELEASE(m_pGammaControl);
	SAFE_RELEASE(m_pddsFPS);
	SAFE_RELEASE(m_pddsBackBuffer);
	SAFE_RELEASE(m_pddsPrimaryBuffer);
	if(m_pDD )
	   m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
	SAFE_RELEASE(m_pDD);
	//�����o���X�V
	m_bWindow = bWindow;
	m_bDouble = bDouble;
	//�c�����������c�������I�u�W�F�N�g����
	if(FAILED(DirectDrawCreateEx(NULL, (VOID**)&m_pDD,IID_IDirectDraw7,NULL)))
	{
		MB("DirectDraw�I�u�W�F�N�g�쐬�Ɏ��s���܂����B");
		return 0;
	}
	//��������̓E�B���h�E���[�h�ƃt���X�N���[�h�ŏꍇ����
	if(bWindow)	//�E�B���h�E���[�h
	{
		//�������x���̐ݒ�
		if(FAILED(m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL)))
		{
			MB("DirectDraw��Window���[�h�̋������x����ݒ�ł��܂���ł����B");
			return 0;
		}
		//�E�B���h�E�X�^�C���̕ύX
		DWORD dwStyle;	//�E�B���h�E�X�^�C��
		dwStyle  = GetWindowStyle(m_hWnd);
		dwStyle &= ~WS_POPUP;
		dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
		//�E�B���h�E�T�C�Y�E�ʒu�̕ύX
		RECT rc;	//�E�B���h�E�̈�
		SetRect( &rc, 0, 0, (DWORD)this->GetWindowWidth(), (DWORD)this->GetWindowWidth());
		AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), GetMenu(m_hWnd) != NULL, GetWindowExStyle(m_hWnd));
		SetWindowPos(m_hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		//��ʊO�ɏo�Ă��璼��
		RECT rcWork;	//��Ɨp�q�d�b�s
		SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
		GetWindowRect( m_hWnd, &rc );
		if( rc.left < rcWork.left ) rc.left = rcWork.left;
		if( rc.top	< rcWork.top )	rc.top	= rcWork.top;
		SetWindowPos( m_hWnd, NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
		//�v���C�}���o�b�t�@���쐬
		DDSURFACEDESC2 ddsd;
		ZeroMemory( &ddsd, sizeof( ddsd ) );
		ddsd.dwSize 		= sizeof( ddsd );
		ddsd.dwFlags		= DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;	//�V�X�e���������ɍ��
		if( FAILED( m_pDD->CreateSurface( &ddsd, &m_pddsPrimaryBuffer, NULL)))
		{
			MB("�v���C�}���o�b�t�@�쐬�Ɏ��s���܂����B");
			return 0;
		}
		//�o�b�N�o�b�t�@���쐬
		ddsd.dwFlags		= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;	//�V�X�e���������ɍ��
		ddsd.dwWidth		= (DWORD)this->GetWindowWidth();
		ddsd.dwHeight		= (DWORD)this->GetWindowHeight();
		
		ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		//RGB565�J���[
		ddsd.ddpfPixelFormat.dwRBitMask = 0x0000F800 ;
		ddsd.ddpfPixelFormat.dwGBitMask = 0x000007E0 ;
		ddsd.ddpfPixelFormat.dwBBitMask = 0x0000001F ;
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00000000 ;

		if( FAILED(m_pDD->CreateSurface( &ddsd, &m_pddsBackBuffer, NULL)))
		{
			MB("�o�b�N�o�b�t�@�쐬�Ɏ��s���܂����B");
			return 0;
		}
		//�N���b�p�[�̐ݒ�
		LPDIRECTDRAWCLIPPER pcClipper;
		if( FAILED(m_pDD->CreateClipper( 0, &pcClipper, NULL ) ) )
		{
			MB("�N���b�p�[�쐬�Ɏ��s���܂����B");
			return 0;
		}
		//�N���b�p�[�ɃE�B���h�E�n���h�����Z�b�g
		if( FAILED(pcClipper->SetHWnd( 0, m_hWnd)))
		{
			MB("�N���b�p�[�쐬�Ɏ��s���܂����B");
			pcClipper->Release();
			return E_FAIL;
		}
		//�v���C�}���ɃN���b�p�[���Z�b�g
		if( FAILED(m_pddsPrimaryBuffer->SetClipper( pcClipper ) ) )
		{
			MB("�N���b�p�[�쐬�Ɏ��s���܂����B");
			pcClipper->Release();
			return E_FAIL;
		}
		//�p�ς݂Ȃ̂ŃN���b�p�[���폜
		pcClipper->Release();
		//���j���[������
        SetMenu( m_hWnd, NULL );
        dwStyle = GetWindowLong( m_hWnd, GWL_STYLE );
        dwStyle |= WS_SYSMENU;
        SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );
#ifdef _WIN64
        m_hInst = (HINSTANCE) GetWindowLongPtr( m_hWnd, GWLP_HINSTANCE );
#else
        m_hInst = (HINSTANCE) GetWindowLong( m_hWnd, GWL_HINSTANCE );
#endif
        HMENU hMenu = LoadMenu( m_hInst, "");
        SetMenu( m_hWnd, hMenu );
	}
	//�t���X�N���[�����[�h
	else
	{
		//�������x���̐ݒ�
		if(FAILED( m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN)))
		{
			SetWindow(true);
			int ret = InitDirectDraw(m_bWindow,m_bDouble);
			MB("DirectDraw��FullScreen���[�h�̋������x����ݒ�ł��܂���ł����B");
			return E_FAIL;
		}
		//�f�B�X�v���C���[�h�̐ݒ�
		if( FAILED( m_pDD->SetDisplayMode((DWORD)GetWindowWidth(),(DWORD)GetWindowHeight(),SCREEN_BPP, 0, 0)))
		{
			SetWindow(true);
			int ret = InitDirectDraw(m_bWindow,m_bDouble);
			MB("16�r�b�g�̃t���X�N���[�����[�h��ݒ�ł��܂���ł����B");
			return 0;
		}
		//�v���C�}���o�b�t�@�쐬
		DDSURFACEDESC2 ddsd;
		ZeroMemory( &ddsd, sizeof( ddsd ) );
		ddsd.dwSize 		   = sizeof( ddsd );
		ddsd.dwFlags		   = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE;

		if(FAILED(m_pDD->CreateSurface( &ddsd, &m_pddsPrimaryBuffer,NULL)))
		{
			SetWindow(true);
			int ret = InitDirectDraw(m_bWindow,m_bDouble);
			MB("�v���C�}���o�b�t�@�쐬�Ɏ��s���܂����B");
			return 0;
		}
		//�o�b�N�o�b�t�@���쐬
		ddsd.dwFlags		= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;	//�V�X�e���������ɍ��
		ddsd.dwWidth		= (DWORD)this->GetWindowWidth();
		ddsd.dwHeight		= (DWORD)this->GetWindowHeight();

		ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		//RGB565�J���[
		ddsd.ddpfPixelFormat.dwRBitMask = 0x0000F800 ;
		ddsd.ddpfPixelFormat.dwGBitMask = 0x000007E0 ;
		ddsd.ddpfPixelFormat.dwBBitMask = 0x0000001F ;
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00000000 ;
		if( FAILED(m_pDD->CreateSurface( &ddsd, &m_pddsBackBuffer, NULL)))
		{
			SetWindow(true);
			int ret = InitDirectDraw(m_bWindow,m_bDouble);
			MB("�o�b�N�o�b�t�@�쐬�Ɏ��s���܂����B");
			return 0;
		}
		//���j���[������
        SetMenu( m_hWnd, NULL );
        DWORD dwStyle = GetWindowLong( m_hWnd, GWL_STYLE );
        dwStyle &= ~WS_SYSMENU;
        SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );       
	}
	//FPS�\���p�T�[�t�F�C�X���쐬
	DDSURFACEDESC2 ddsd;
	ZeroMemory( &ddsd, sizeof( ddsd ) );
	ddsd.dwSize 		   = sizeof( ddsd );
	ddsd.dwFlags		= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth		= FPSSHOW_WIDTH;
	ddsd.dwHeight		= FPSSHOW_HEIGHT;
	ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
	//RGB565�J���[
	ddsd.ddpfPixelFormat.dwRBitMask = 0x0000F800 ;
	ddsd.ddpfPixelFormat.dwGBitMask = 0x000007E0 ;
	ddsd.ddpfPixelFormat.dwBBitMask = 0x0000001F ;
	ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00000000 ;

	if( FAILED(m_pDD->CreateSurface( &ddsd, &m_pddsFPS, NULL)))
		return 0;
	
	//�K���}�R���g���[���I�u�W�F�N�g����
	m_pddsPrimaryBuffer->QueryInterface(IID_IDirectDrawGammaControl,(LPVOID*)&m_pGammaControl);
	m_pGammaControl->GetGammaRamp(0,&m_gammaInit);

	UpdateBounds();
	return 1;
}
/*-----------------------------------------------------------------------------
	UpdateBounds
-----------------------------------------------------------------------------*/
int CNkLib::UpdateBounds()
{
	if(m_bWindow)
	{
		GetClientRect( m_hWnd, &m_rcClient );
		ClientToScreen( m_hWnd, (POINT*)&m_rcClient );
		ClientToScreen( m_hWnd, (POINT*)&m_rcClient+1 );
	}
	else
	{
		SetRect( &m_rcClient, 0, 0, GetSystemMetrics(SM_CXSCREEN),
				 GetSystemMetrics(SM_CYSCREEN) );
	}
	return 1;
}
/*-----------------------------------------------------------------------------
	�t���b�v
-----------------------------------------------------------------------------*/
int CNkLib::Flip()
{
	HRESULT hr;
	if( NULL == m_pddsPrimaryBuffer || NULL == m_pddsBackBuffer )
		return 0;

	HDC hdcDest,hdcSrc;
	m_pddsPrimaryBuffer->GetDC(&hdcDest);
	m_pddsBackBuffer->GetDC(&hdcSrc);
	int ret=BitBlt(hdcDest,m_rcClient.left,m_rcClient.top,GetWindowWidth(),GetWindowHeight(),hdcSrc,0,0,SRCCOPY);
	m_pddsPrimaryBuffer->ReleaseDC(hdcDest);
	m_pddsBackBuffer->ReleaseDC(hdcSrc);
	return ret;

	while( 1 )
	{
		hr = m_pddsPrimaryBuffer->Blt( &m_rcClient, m_pddsBackBuffer, NULL, DDBLT_WAIT, NULL );
		if( hr == DDERR_SURFACELOST )
		{
			MB("�T�[�t�F�C�X���X�g");
			m_pDD->RestoreAllSurfaces();
			m_pddsPrimaryBuffer->Restore();
			m_pddsBackBuffer->Restore();
			m_pddsFPS->Restore();
			return 1;
		}
		else
		{
			if( hr != DDERR_WASSTILLDRAWING )
			{
				return 1;
			}
		}
	}
}
/*-----------------------------------------------------------------------------
	��̃T�[�t�F�C�X���쐬
-----------------------------------------------------------------------------*/
int CNkLib::CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc2,LPDIRECTDRAWSURFACE7* lplpDDSurface)
{
	if(FAILED(m_pDD->CreateSurface(lpDDSurfaceDesc2,lplpDDSurface,NULL)))
		return 0;
	return 1;
}
/*-----------------------------------------------------------------------------
	�o�b�N�o�b�t�@�ɓ]��
-----------------------------------------------------------------------------*/
int CNkLib::BltFast(int iDestX,int iDestY,LPDIRECTDRAWSURFACE7 pdds,LPRECT prcSrc,bool trans)
{
	//���߃t���O
	DWORD dwTrans = trans ? DDBLTFAST_SRCCOLORKEY : 0;
	if(prcSrc)
	{
		RECT rect=*prcSrc;
		//������ʊO�Ȃ炻�̂Ԃ��؂藎�Ƃ�
		if(iDestX<0) { rect.left += abs(iDestX); iDestX = 0;}
		if(iDestX+rect.right-rect.left > SCREEN_WIDTH)
			rect.right -= (iDestX+rect.right-rect.left - SCREEN_WIDTH);
		if(iDestY<0) { rect.top += abs(iDestY);  iDestY = 0;}
		if(iDestY+rect.bottom-rect.top > SCREEN_HEIGHT)
			rect.bottom -= (iDestY+rect.bottom-rect.top - SCREEN_HEIGHT);
		//�{�E�B���h�E���͓]������W�̂Q�{
		DWORD dwDestX = (DWORD)iDestX * (1+m_bDouble);
		DWORD dwDestY = (DWORD)iDestY * (1+m_bDouble);
		//�{�E�B���h�E���͓]�����̈�̂Q�{
		rect.left	*= (1+m_bDouble);
		rect.right	*= (1+m_bDouble);
		rect.top	*= (1+m_bDouble);
		rect.bottom	*= (1+m_bDouble);
		if(FAILED(m_pddsBackBuffer->BltFast(dwDestX,dwDestY,pdds,&rect,dwTrans)))
		{
			Outputf("x=%d\n",dwDestX);
			Outputf("y=%d\n",dwDestY);
			return 0;
		}
	}
	else
	{
		if(iDestX<0) iDestX=0;
		if(iDestY<0) iDestY=0;
		//�{�E�B���h�E���͓]������W�̂Q�{
		DWORD dwDestX = (DWORD)iDestX * (1+m_bDouble);
		DWORD dwDestY = (DWORD)iDestY * (1+m_bDouble);
		if(FAILED(m_pddsBackBuffer->BltFast(dwDestX,dwDestY,pdds,prcSrc,dwTrans)))
		{
			Outputf("x=%d\n",dwDestX);
			Outputf("y=%d\n",dwDestY);
			return 0;
		}
	}
	return 1;
}
/*-----------------------------------------------------------------------------
	�o�b�N�o�b�t�@�Ɋg��k���]��
-----------------------------------------------------------------------------*/
int CNkLib::BltStretch(LPRECT prcDest,LPDIRECTDRAWSURFACE7 pdds,LPRECT prcSrc,bool trans)
{
	//���߃t���O
	DWORD dwTrans = trans ? DDBLT_KEYSRC : 0;

	//�Ƃ肠�����m�t�k�k�n�����z��
	RECT rcDest,rcSrc;
	LPRECT prSrc,prDest;
	{
		if(prcSrc)
		{
			rcSrc = *prcSrc;
			prSrc = &rcSrc;
		}
		else
			prSrc = NULL;
		if(prcDest)
		{
			rcDest = *prcDest;
			prDest = &rcDest;
		}
		else
		{
			SetRect(&rcDest,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
			prDest = &rcDest;
		}
	}
	//��ʊO�؂藎�Ƃ�������
	{
		if(rcDest.left<0)
		{
			rcSrc.left +=abs(rcDest.left)*(rcSrc.right-rcSrc.left)/(rcDest.right-rcDest.left);
			rcDest.left=0;
		}
		if(rcDest.top<0)
		{
			rcSrc.top +=abs(rcDest.top)*(rcSrc.bottom-rcSrc.top)/(rcDest.bottom-rcDest.top);
			rcDest.top=0;
		}
		if(rcDest.right > SCREEN_WIDTH)
		{
			rcSrc.right -= (rcDest.right-SCREEN_WIDTH)*(rcSrc.right-rcSrc.left)/(rcDest.right-rcDest.left);
			rcDest.right = SCREEN_WIDTH;
		}
		if(rcDest.bottom > SCREEN_HEIGHT)
		{
			rcSrc.bottom -= (rcDest.bottom-SCREEN_HEIGHT)*(rcSrc.bottom-rcSrc.top)/(rcDest.bottom-rcDest.top);
			rcDest.bottom = SCREEN_HEIGHT;
		}
	}
	//�Q�{���[�h������
	if(m_bDouble)
	{
		rcDest.left		*= 2;
		rcDest.right	*= 2;
		rcDest.top		*= 2;
		rcDest.bottom	*= 2;
		rcSrc.left		*= 2;
		rcSrc.right		*= 2;
		rcSrc.top		*= 2;
		rcSrc.bottom	*= 2;
	}
	//�`��
	if(FAILED(m_pddsBackBuffer->Blt(prDest,pdds,prSrc,dwTrans,NULL)))
		return 0;
	return 1;
}
/*-----------------------------------------------------------------------------
	�u���b�N�]��
-----------------------------------------------------------------------------*/
// �I�t�Z�b�g���v�Z(�⏕�p
int	offset (const DDSURFACEDESC2	&desc,int x,int y)
{
	// ���������Q�̒P�ʂɊۂ߂�
	x &= 0xfffffffe;

	return y * desc.lPitch / 2 + x;
}
//�u���b�N�]��
//�s�N�Z�����Z�N���XCCalculate�̃|�C���^��n���ĉ��Z�E���Z�E�A���t�@�����\
int CNkLib::BltEffectBase(int iDestX,int iDestY,LPDIRECTDRAWSURFACE7 pdds,LPRECT prcSrc,CCalculate* pCal)
{
	DDSURFACEDESC2	ddsdDest;
	DDSURFACEDESC2	ddsdSrc;
	DWORD *a,*b;
	int		a_skip,b_skip;
	int		w = (int)(prcSrc->right-prcSrc->left);
	int		h = (int)(prcSrc->bottom-prcSrc->top);
	
	//�]�������b�N
	memset (&ddsdSrc,0,sizeof (ddsdSrc));
	ddsdSrc.dwSize = sizeof ddsdSrc;
	if(FAILED(pdds->Lock(NULL,&ddsdSrc,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR ,NULL)))
		return 0;

	//�]���惍�b�N
	memset (&ddsdDest,0,sizeof (ddsdDest));
	ddsdDest.dwSize = sizeof ddsdDest;
	if(FAILED(m_pddsBackBuffer->Lock(NULL,&ddsdDest,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR ,NULL)))
		return 0;

	// �]����A�h���X�ƃX�L�b�v�����Z�o
	a = (DWORD*)ddsdDest.lpSurface;
	a += offset (ddsdDest, iDestX, iDestY) / 2;
	a_skip = (ddsdDest.lPitch / 2 - w) / 2;

	// �]�����A�h���X�ƃX�L�b�v�����Z�o
	b = (DWORD*)ddsdSrc.lpSurface;
	b += offset (ddsdSrc, prcSrc->left,prcSrc->top) / 2;
	b_skip = (ddsdSrc.lPitch / 2 - w) / 2;

	//�]�����W�̋����Ɗ�̑g�ݍ��킹�ŏꍇ�����E�E�E
	if ((iDestX & 1) == 0 && (prcSrc->left & 1) == 0)
		Blt0To0 (a,b, a_skip,b_skip, w, h, pCal);
	if ((iDestX & 1) == 1 && (prcSrc->left & 1) == 0)
		Blt1To0 (a,b, a_skip,b_skip, w, h, pCal);
	if ((iDestX & 1) == 0 && (prcSrc->left & 1) == 1)
		Blt0To1 (a,b, a_skip,b_skip, w, h, pCal);
	if ((iDestX & 1) == 1 && (prcSrc->left & 1) == 1)
		Blt1To1 (a,b, a_skip,b_skip, w, h, pCal);
	//�T�[�t�F�C�X�̃A�����b�N
	if(FAILED(m_pddsBackBuffer->Unlock(NULL)))
		return 0;
	if(FAILED(pdds->Unlock(NULL)))
		return 0;
	return 1;
}
/*-----------------------------------------------------------------------------
	�u���b�N�]���̃N���b�v�Ή��C���^�[�t�F�[�X
-----------------------------------------------------------------------------*/
int CNkLib::BltEffect(int iDestX,int iDestY,LPDIRECTDRAWSURFACE7 pdds,LPRECT prcSrc,CCalculate* pCal)
{
		RECT rect=*prcSrc;
		//������ʊO�Ȃ炻�̂Ԃ��؂藎�Ƃ�
		if(iDestX<0) { rect.left += abs(iDestX); iDestX = 0;}
		if(iDestX+rect.right-rect.left > SCREEN_WIDTH)
			rect.right -= (iDestX+rect.right-rect.left - SCREEN_WIDTH);
		if(iDestY<0) { rect.top += abs(iDestY);  iDestY = 0;}
		if(iDestY+rect.bottom-rect.top > SCREEN_HEIGHT)
			rect.bottom -= (iDestY+rect.bottom-rect.top - SCREEN_HEIGHT);
		//�{�E�B���h�E���͓]������W�̂Q�{
		int dx = iDestX * (1+m_bDouble);
		int dy = iDestY * (1+m_bDouble);
		//�{�E�B���h�E���͓]�����̈�̂Q�{
		rect.left	*= (1+m_bDouble);
		rect.right	*= (1+m_bDouble);
		rect.top	*= (1+m_bDouble);
		rect.bottom	*= (1+m_bDouble);
		return BltEffectBase(dx,dy,pdds,&rect,pCal);
}
//��������BltEffectBase�p�̕⏕�֐�
//�����������
int CNkLib::Blt0To0(DWORD *pDest,DWORD *pSrc,int a_skip,int b_skip,int w,int h,CCalculate* pCal)
{
	int	wx1 = w & 1;	// ��������ǂ���
	int	wxc = w / 2;	// ���̌J��Ԃ���
	int	i,j;
    DWORD *&a=pDest;
	DWORD *&b=pSrc;
	for (j=0; j<h; j++)
	{
		for (i=0; i<wxc; i++)
		{
			*a = pCal->Get(*a,*b);
			a ++;
			b ++;
		}

		if (wx1)
		{
			*a = (*a & _mask10) | (pCal->Get(*a, *b) & _mask01);
			a ++;
			b ++;
		}

		a += a_skip;
		b += b_skip;
	}
	return 1;
}
//��������
int CNkLib::Blt1To0(DWORD *pDest,DWORD *pSrc,int a_skip,int b_skip,int w,int h,CCalculate* pCal)
{
	int	wx1 = w & 1;	// ��������ǂ���
	int	wxc = w / 2;	// ���̌J��Ԃ���
	DWORD	hh,l;
	int	i,j;
    DWORD *&a=pDest;
	DWORD *&b=pSrc;

	if (wx1 == 0) wxc --;

	for (j=0; j<h; j++) {

		l = *b << 16;
		hh = *b >> 16;
	
		*a = (*a & _mask01) | (pCal->Get (*a,l) & _mask10);

		for (i=0; i<wxc; i++) {

			a ++;
			b ++;

			l = *b << 16;
		
			*a = pCal->Get (*a,l | hh);

			hh = *b >> 16;
		}

		a ++;
		b ++;

		if (wx1 == 0) {
			*a = (*a & _mask10) | (pCal->Get (*a,hh) & _mask01);
		}

		a += a_skip;
		b += b_skip;
	}

	return 1;
}
//��������
int CNkLib::Blt0To1(DWORD *pDest,DWORD *pSrc,int a_skip,int b_skip,int w,int h,CCalculate* pCal)
{
	int	wx1 = w & 1;	// ��������ǂ���
	int	wxc = w / 2;	// ���̌J��Ԃ���
	DWORD	hh,l;
	int	i,j;
    DWORD *&a=pDest;
	DWORD *&b=pSrc;

	for (j=0; j<h; j++) {
		for (i=0; i<wxc; i++) {

			hh = *b >> 16;

			b ++;

			l = *b << 16;

			*a = pCal->Get (*a,hh | l);

			a ++;
		}

		if (wx1) {
			hh = *b >> 16;
			*a = (*a & _mask10) | (pCal->Get (*a,hh) & _mask01);
			a ++;
			b ++;
		}

		a += a_skip;
		b += b_skip;
	}
	return 1;
}
//�����
int CNkLib::Blt1To1(DWORD *pDest,DWORD *pSrc,int a_skip,int b_skip,int w,int h,CCalculate* pCal)
{
	int	wx1 = w & 1;	// ��������ǂ���
	int	wxc = w / 2;	// ���̌J��Ԃ���
	int	i,j;
    DWORD *&a=pDest;
	DWORD *&b=pSrc;

	if (wx1 == 0) wxc --;

	for (j=0; j<h; j++) {

		*a = (*a & _mask01) | (pCal->Get(*a,*b) & _mask10);

		a ++;
		b ++;

		for (i=0; i<wxc; i++) {

			*a = pCal->Get (*a,*b);

			a ++;
			b ++;
		}

		if (wx1 == 0) {
			*a = (*a & _mask10) | (pCal->Get (*a,*b) & _mask01);
		}

		a += a_skip;
		b += b_skip;
	}

	return 1;
}
/*-----------------------------------------------------------------------------
	�K���}�l�␳
-----------------------------------------------------------------------------*/
int CNkLib::PlusGamma(int red,int green,int blue)
{
	DDGAMMARAMP Gamma;
	m_pGammaControl->GetGammaRamp(0,&Gamma);
	if(red>0)//���b�h�v���X
		for(int i=0;i<=255;i++){
			if(Gamma.red[i]<65535-red)	Gamma.red[i] +=red;
			else Gamma.red[i]=65535;
	}
	else if(red<0)//���b�h�}�C�i�X
		for(int i=0;i<=255;i++){
			if(Gamma.red[i]>0+abs(red))	Gamma.red[i] -=abs(red);
			else Gamma.red[i]=0;
	}
	if(green>0)//�O���[���v���X
		for(int i=0;i<=255;i++){
			if(Gamma.green[i]<65535-green)	Gamma.green[i] +=green;
			else Gamma.green[i]=65535;
	}
	else if(green<0)//�O���[���}�C�i�X
		for(int i=0;i<=255;i++){
			if(Gamma.green[i]>0+abs(green))	Gamma.green[i] -=abs(green);
			else Gamma.green[i]=0;
	}
	if(blue>0)//�u���[�v���X
		for(int i=0;i<=255;i++){
			if(Gamma.blue[i]<65535-blue)	Gamma.blue[i] +=blue;
			else Gamma.blue[i]=65535;
	}
	else if(blue<0)//�u���[�}�C�i�X
		for(int i=0;i<=255;i++){
			if(Gamma.blue[i]>0+abs(blue))	Gamma.blue[i] -=abs(blue);
			else Gamma.blue[i]=0;
	}
	return m_pGammaControl->SetGammaRamp(0,&Gamma);
}
/*-----------------------------------------------------------------------------
	���ׂẴC���[�W�����[�h���Ȃ���
-----------------------------------------------------------------------------*/
int CNkLib::ReloadAllImage()
{
	list<CNkImage*>::iterator itr;
	for(itr = m_lstpImage.begin();itr != m_lstpImage.end(); ++itr)
		if(!(*itr)->Reload())
			return 0;
	return 1;
}
/*-----------------------------------------------------------------------------
	���ׂẴC���[�W��Release����
-----------------------------------------------------------------------------*/
void CNkLib::ReleaseAllImage()
{
	list<CNkImage*>::iterator itr;
	for(itr = m_lstpImage.begin();itr != m_lstpImage.end(); ++itr)
		(*itr)->Release();
}

/*-----------------------------------------------------------------------------
	���ׂĂ̕`��L���[��`�悷��
-----------------------------------------------------------------------------*/
int CNkLib::DrawList()
{
	list<tagDRAWINFO>::iterator itr;
	m_lstDInfo.sort();
	for(itr = m_lstDInfo.begin();itr != m_lstDInfo.end(); ++itr)
	{
		if(!itr->pDrawQue->Draw())
			return 0;
		SAFE_DELETE(itr->pDrawQue);
	}
	m_lstDInfo.clear();
	return 1;
}
/*-----------------------------------------------------------------------------
	TextOut�̓����g�p�o�[�W����
	�Q�{���[�h�����ƃt���O�̒u�����������Ă����
-----------------------------------------------------------------------------*/
int CNkLib::TextOutBase(char* text,int x,int y,int size,int weight,DWORD dwFlag,COLORREF color)
{
	if(GetDouble()) {x*=2;y*=2;size*=2;}
	HFONT hFont = CreateFont(
		size,0,0,0,weight,
		dwFlag & FONT_ITALIC,
		dwFlag & FONT_ULINE,
		dwFlag & FONT_LINEOUT,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY,
		FIXED_PITCH | FF_MODERN,
		"HG�ۺ޼��M-PRO");
	HDC hdc;
	m_pddsBackBuffer->GetDC(&hdc);
	SelectObject(hdc, hFont);
	SetTextColor(hdc,color);
	SetBkMode(hdc, TRANSPARENT);
	int ret= (int)TextOut(hdc,x,y,text,strlen(text));
	m_pddsBackBuffer->ReleaseDC(hdc);
	DeleteObject(hFont);
	return ret;
}
/*-----------------------------------------------------------------------------
	Text
	�`��L���[�Ή�
-----------------------------------------------------------------------------*/
int CNkLib::Text(char *txt,int x,int y,int size,int weight,DWORD dwFlag,COLORREF color,int layer)
{
	DRAWINFO dInfo;
	dInfo.spr=layer;
	dInfo.pDrawQue = new CDrawQueText(txt,x,y,size,weight,dwFlag,color);
	AddDrawList(dInfo);
	return 0;
}
/*-----------------------------------------------------------------------------
	DrawCircle
	�~�E�ȉ~��`��
-----------------------------------------------------------------------------*/
int CNkLib::DrawCircle(int width,COLORREF color,int left,int top,int right,int bottom)
{
	if(GetDouble()) {width*=2;left*=2;top*=2;right*=2;bottom*=2;}
	HDC hdc;
	m_pddsBackBuffer->GetDC(&hdc);

	HPEN hPen = CreatePen(PS_SOLID,width,color);
    SelectObject(hdc, hPen);
	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    SelectObject(hdc, hBrush);

    int ret=Ellipse(hdc, left, top, right, bottom);
    DeleteObject(hPen);
	m_pddsBackBuffer->ReleaseDC(hdc);
	return ret;
}
/*-----------------------------------------------------------------------------
	Circle
	�`��L���[�Ή���
-----------------------------------------------------------------------------*/
int CNkLib::Circle(int width,COLORREF color,int left,int top,int right,int bottom,int layer)
{
	DRAWINFO dInfo;
	dInfo.spr=layer;
	dInfo.pDrawQue = new CDrawQueCircle(width,color,left,top,right,bottom);
	AddDrawList(dInfo);
	return 1;
}
/*-----------------------------------------------------------------------------

  ��������h���������֘A�ł���I

-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
	Input������
-----------------------------------------------------------------------------*/
int CNkLib::InitInput()
{
	// DirectInput �I�u�W�F�N�g���쐬����B
	if(FAILED(DirectInput8Create(m_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL)))
	{
		MB("DirectInput Init Failed.");
		return 0;
	}
	/*---------��������L�[�{�[�h������---------------*/

	// �f�o�C�X���擾����B
	if(FAILED(m_pDI->CreateDevice(GUID_SysKeyboard,&m_pdidKeyboard, NULL)))
		MB("Create Keyboard Failed.");
	// �f�[�^�`�����Z�b�g����B
	m_pdidKeyboard->SetDataFormat(&c_dfDIKeyboard);
	// �������x����ݒ肷��B
	m_pdidKeyboard->SetCooperativeLevel(m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	//�A�N�Z�X�����擾����B
	if(FAILED(m_pdidKeyboard->Acquire()))
		MB("keyboard acquire failed.");
	/*---------��������W���C�X�e�B�b�N������---------------*/

	//�f�o�C�X�̗�
	if(FAILED(m_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,(VOID*)this, DIEDFL_ATTACHEDONLY)))
		return 0;
	//�W���C�X�e�B�b�N���ڑ�����Ă���΁AEnumJoystickCallback�R�[���o�b�N�֐�����
	//m_pdidJoystick�͎擾����Ă���B

	if(m_pdidJoystick)
	{
		//�f�[�^�t�H�[�}�b�g
		if(FAILED(m_pdidJoystick->SetDataFormat( &c_dfDIJoystick2 )))
			return 0;
		//�������x��
		if(FAILED(m_pdidJoystick->SetCooperativeLevel( m_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND )))
			return 0;
		//�R�[���o�b�N�֐����Z�b�g
		if(FAILED(m_pdidJoystick->EnumObjects( EnumObjectsCallback, (VOID*)this, DIDFT_ALL )))
			return 0;
	}
	return 1;
}
/*-----------------------------------------------------------------------------
	�h�����������X�V����
-----------------------------------------------------------------------------*/
int CNkLib::UpdateInput()
{
	HRESULT hr;
	//�L�[�{�[�h
	if( FAILED( hr = m_pdidKeyboard->GetDeviceState(256,(LPVOID)&m_bKeyState)))
	{
		MB("keyboard GetDeviceState failed.");
		if( hr == DIERR_INPUTLOST ) 
		{
			if(FAILED(hr = m_pdidKeyboard->Acquire()))
				return 0;
		}
		else
			return 0;
	}
	if(FAILED(m_pdidKeyboard->Acquire()))
	{
		MB("acquire failed.");
		return 0;
	}
	//�W���C�X�e�B�b�N
	if(m_pdidJoystick)
	{
		if( FAILED( m_pdidJoystick->Poll()))
		{
			hr = m_pdidJoystick->Acquire();
			while( hr == DIERR_INPUTLOST ) 
				hr = m_pdidJoystick->Acquire();
		}
		if(FAILED(m_pdidJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &m_diJoyState )))
			return 0;
	}
	//����
	for(int i = 0; i < INP_MAX; ++i)
	{
		m_bFormerState[i] = m_bPresentState[i];
		m_bPresentState[i] = GetKeyboardState(i) || GetJoystickState(i);
	}
	//�㉟���D��ɂ���
	//���E��
	//�����ꂽ�u�Ԃł���B
	if(GetInputEvent(INP_LEFT))
		m_iLR = INP_LEFT;
	else if(GetInputEvent(INP_RIGHT))
		m_iLR = INP_RIGHT;
	//�O����A�N�e�B�u�ŁA���݂������ꑱ���Ă���B
	else if(m_iLR == INP_LEFT && m_bPresentState[INP_LEFT])
		m_iLR = INP_LEFT;
	else if(m_iLR == INP_RIGHT && m_bPresentState[INP_RIGHT])
		m_iLR = INP_RIGHT;
	//�O����A�N�e�B�u�ȃL�[�͉�����Ă��Ȃ���������Ă���L�[������B
	else if(m_bPresentState[INP_LEFT])
		m_iLR = INP_LEFT;
	else if(m_bPresentState[INP_RIGHT])
		m_iLR = INP_RIGHT;
	else
		m_iLR = INP_NEUTRAL;//�Ȃɂ�������Ă��Ȃ��B

	//�㉟���D��ɂ���
	//�㉺��
	//�����ꂽ�u�Ԃł���B
	if(GetInputEvent(INP_UP))
		m_iUD = INP_UP;
	else if(GetInputEvent(INP_DOWN))
		m_iUD = INP_DOWN;
	//�O����A�N�e�B�u�ŁA���݂������ꑱ���Ă���B
	else if(m_iUD == INP_UP && m_bPresentState[INP_UP])
		m_iUD = INP_UP;
	else if(m_iUD == INP_DOWN && m_bPresentState[INP_DOWN])
		m_iUD = INP_DOWN;
	//�O����A�N�e�B�u�ȃL�[�͉�����Ă��Ȃ���������Ă���L�[������B
	else if(m_bPresentState[INP_UP])
		m_iUD = INP_UP;
	else if(m_bPresentState[INP_DOWN])
		m_iUD = INP_DOWN;
	else
		m_iUD = INP_NEUTRAL;//�Ȃɂ�������Ă��Ȃ��B

	return 1;
}
/*-----------------------------------------------------------------------------
	�h���������֌W�̏������⏕�֐�
-----------------------------------------------------------------------------*/

HRESULT CNkLib::CreateDeviceEx(REFGUID rguid)
{
	return m_pDI->CreateDevice(rguid,&m_pdidJoystick, NULL );
}
HRESULT CNkLib::SetJoystickProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	return m_pdidJoystick->SetProperty(rguidProp, pdiph);
}
HRESULT CNkLib::InitializeJoystick(REFGUID rguid)
{
	return m_pdidJoystick->Initialize(m_hInst,DIRECTINPUT_VERSION,rguid);
}
/*-----------------------------------------------------------------------------
	�W���C�X�e�B���b�N���
-----------------------------------------------------------------------------*/
bool CNkLib::GetJoystickState(int inp)
{
	switch(inp)
	{
	case INP_RIGHT:
		return m_diJoyState.lX>0;
		break;
	case INP_LEFT:
		return m_diJoyState.lX<0;
		break;
	case INP_DOWN:
		return m_diJoyState.lY>0;
		break;
	case INP_UP:
		return m_diJoyState.lY<0;
		break;
	case INP_B0:
		return (m_diJoyState.rgbButtons[0] & 0x80) !=0;
		break;
	case INP_B1:
		return (m_diJoyState.rgbButtons[1] & 0x80) !=0;
		break;
	case INP_B2:
		return (m_diJoyState.rgbButtons[2] & 0x80) !=0;
		break;
	case INP_B3:
		return (m_diJoyState.rgbButtons[3] & 0x80) !=0;
		break;
	}
	return false;
}
/*-----------------------------------------------------------------------------
	�L�[�{�[�h���
-----------------------------------------------------------------------------*/
bool CNkLib::GetKeyboardState(int inp)
{
	switch(inp)
	{
	case INP_RIGHT:
		return (m_bKeyState[DIK_RIGHT] & 0x80) !=0;
		break;
	case INP_LEFT:
		return (m_bKeyState[DIK_LEFT] & 0x80) !=0;
		break;
	case INP_DOWN:
		return (m_bKeyState[DIK_DOWN] & 0x80) !=0;
		break;
	case INP_UP:
		return (m_bKeyState[DIK_UP] & 0x80) !=0;
		break;
	case INP_B0:
		return (m_bKeyState[DIK_Z] & 0x80) || (m_bKeyState[DIK_A] & 0x80);
		break;
	case INP_B1:
		return (m_bKeyState[DIK_X] & 0x80) || (m_bKeyState[DIK_A] & 0x80);
		break;
	case INP_B2:
		return (m_bKeyState[DIK_C] & 0x80) !=0;
		break;
	case INP_B3:
		return (m_bKeyState[DIK_V] & 0x80) !=0;
		break;
	}
	return false;
}
/*-----------------------------------------------------------------------------
	�L�[�{�[�h�{�W���C�X�e�B�b�N���
-----------------------------------------------------------------------------*/
bool CNkLib::GetInputState2(int inp)
{
	if(inp==INP_LEFT||inp==INP_RIGHT||inp==INP_UP||inp==INP_DOWN)
	{
		if(inp == m_iLR)
			return true;
		if(inp == m_iUD)
			return true;
		return false;
	}
	return GetInputState(inp);
}
/*-----------------------------------------------------------------------------
	�h���������f�[�^���N���A�[
-----------------------------------------------------------------------------*/
int CNkLib::ClearInput()
{
	for(int i=0;i<256;++i)
		m_bKeyState[i] = 0;
	ZeroMemory(&m_diJoyState, sizeof DIJOYSTATE2);
	for(i=0;i<INP_MAX;++i)
	{
		m_bPresentState[i] = FALSE;
		m_bFormerState[i] = FALSE;
	}
	m_iLR = 0;
	m_iUD = 0;
	return 1;
}
/*-----------------------------------------------------------------------------

	Input�R�[���o�b�N�ʏ�֐�

-----------------------------------------------------------------------------*/
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
    HRESULT hr;
	CNkLib *pInput = (CNkLib*)pContext;

    hr = pInput->CreateDeviceEx( pdidInstance->guidInstance);

    if( FAILED(hr) ) 
        return DIENUM_CONTINUE;		//�񋓂𑱂���
    return DIENUM_STOP;				//�񋓂��I���
}
BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	CNkLib *pInput = (CNkLib*)pContext;

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = -1000; 
        diprg.lMax              = +1000; 
    
        // Set the range for the axis
        if( FAILED( pInput->SetJoystickProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
            return DIENUM_STOP;
         
    }
    return DIENUM_CONTINUE;
}
/*-----------------------------------------------------------------------------

  ��������Sound,MIDI�֘A�ł���I

-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
	�r��������������
-----------------------------------------------------------------------------*/
int CNkLib::InitSound()
{
	if(FAILED(DirectSoundCreate(NULL, &m_pDS, NULL)))
		return 0;
	if(FAILED(m_pDS->SetCooperativeLevel(m_hWnd, DSSCL_NORMAL)))
		return 0;
	return 1;
}
/*-----------------------------------------------------------------------------
	MIDI������
-----------------------------------------------------------------------------*/
int CNkLib::InitMidi()
{
	//�p�t�H�[�}���X�̍쐬
	if(FAILED(CoCreateInstance(
				CLSID_DirectMusicPerformance,
				NULL,
				CLSCTX_INPROC,
				IID_IDirectMusicPerformance2,
				(LPVOID*)&m_pdmp)))
		return 0;
	//�p�t�H�[�}���X��������
	if(FAILED(m_pdmp->Init(NULL, m_pDS, m_hWnd)))
		return 0;
	//�|�[�g���쐬
	if(FAILED(m_pdmp->AddPort(NULL)))
		return 0;

	//���[�_�[�̍쐬
    if(FAILED(CoCreateInstance(
				CLSID_DirectMusicLoader,
				NULL,
				CLSCTX_INPROC,
				IID_IDirectMusicLoader,
				(LPVOID*)&m_pdml)))
		return 0;

	return 1;
}
/*-----------------------------------------------------------------------------
	MIDI���[�h
-----------------------------------------------------------------------------*/
int CNkLib::LoadMidi(char* lpszFileName, int num)
{
	DMUS_OBJECTDESC odesc;
	ZeroMemory(&odesc, sizeof(DMUS_OBJECTDESC));
	odesc.dwSize		= sizeof(DMUS_OBJECTDESC);
	odesc.guidClass		= CLSID_DirectMusicSegment;	
	odesc.dwValidData	= DMUS_OBJ_CLASS|DMUS_OBJ_FILENAME;
	//UNICODE
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszFileName, -1, odesc.wszFileName, MAX_PATH);	

	if(FAILED(m_pdml->GetObject(&odesc, IID_IDirectMusicSegment, (LPVOID*)&m_pdms)))
        return 0;

	m_pdms->SetParam(GUID_StandardMIDIFile, -1, 0, 0, (LPVOID)m_pdmp);

	m_pdms->SetParam(GUID_Download, -1, 0, 0, (LPVOID)m_pdmp);
	//�Z�O�����g�̃��s�[�g�񐔂�ݒ�
	m_pdms->SetRepeats(num);

	return 1;
}
/*-----------------------------------------------------------------------------
	MIDI�Đ�
-----------------------------------------------------------------------------*/
int CNkLib::PlayMidi()
{
	StopMidi();
	//�Z�O�����g�̍Đ�
	m_pdmp->PlaySegment(m_pdms, 0, 0, NULL);
	return 1;
}
/*-----------------------------------------------------------------------------
	MIDI��~
-----------------------------------------------------------------------------*/
int CNkLib::StopMidi()
{
	m_pdmp->Stop(NULL, NULL, 0, 0);
	return 1;
}
/*-----------------------------------------------------------------------------
	FPS�\��
-----------------------------------------------------------------------------*/
int CNkLib::ShowFPS()
{
	DWORD dwTime=timeGetTime();
	m_dwCount++;
	char str[256];
	if(dwTime-m_dwOldTime >= 1000)
	{
		m_dwFPS = 1000*m_dwCount/(dwTime-m_dwOldTime);
		m_dwCount = 0;
		sprintf(str,"%3d",m_dwFPS);
		m_dwOldTime = timeGetTime();

		//��������FPS�\��
		HDC hDC;
		m_pddsFPS->GetDC( &hDC );
		SetBkColor( hDC, 0 );
		SetTextColor( hDC, RGB(255,255,255) );
		TextOut( hDC, 0, 0, str, strlen(str));
		m_pddsFPS->ReleaseDC( hDC );
	}
	RECT rect={0,0,FPSSHOW_WIDTH,FPSSHOW_HEIGHT};
	m_pddsBackBuffer->BltFast(0,0,m_pddsFPS,&rect,DDBLTFAST_WAIT);
	return 1;
}