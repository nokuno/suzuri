/*-----------------------------------------------------------------------------

	NkImage.cpp

-----------------------------------------------------------------------------*/
#include "stdapp.h"
#include "utility.h"
#include "NKLib.h"
#include "NkImage.h"
/*-----------------------------------------------------------------------------
	CNkImage�N���X�̃R���X�g���N�^
-----------------------------------------------------------------------------*/
CNkImage::CNkImage()
{
	m_pdds = NULL;
	m_szFileName[0] = NULL;
	g_pNkLib->AddList(this);
}
CNkImage::CNkImage(char* lpszFileName,int width,int height)
{
	m_pdds = NULL;
	Load(lpszFileName,width,height);
	//NkLib�̃��X�g�Ɏ�����ǉ�
	g_pNkLib->AddList(this);
}
int CNkImage::Load(char* lpszFileName,int width,int height)
{
	SAFE_RELEASE(m_pdds);
	SAFE_DELETE(m_pdds);
	if(m_szFileName != lpszFileName)
		strcpy(m_szFileName,lpszFileName);
	LoadPNG(lpszFileName);
	if(width > 0)
		m_width = width;
	else
		m_width = m_bmpWidth;
	if(height > 0)
		m_height = height;
	else
		m_height = m_bmpHeight;
	return 1;
}
CNkImage::~CNkImage()
{
	SAFE_RELEASE(m_pdds);
	if(g_pNkLib)
		g_pNkLib->RemoveList(this);
}
void CNkImage::Release()
{
	SAFE_RELEASE(m_pdds);
}
/*-----------------------------------------------------------------------------
	BMP�̃��[�h
-----------------------------------------------------------------------------*/
int CNkImage::LoadBMP(char* lpszFileName)
{
	HBITMAP 	   hBMP = NULL;
	BITMAP		   bmp;
	DDSURFACEDESC2 ddsd;
	//�r�b�g�}�b�v�n���h����BMP���[�h
	hBMP = (HBITMAP) LoadImage( GetModuleHandle(NULL), lpszFileName,
								IMAGE_BITMAP, 0, 0,
								LR_CREATEDIBSECTION );//���\�[�X���烍�[�h
	if( hBMP == NULL )
	{
		hBMP = (HBITMAP) LoadImage( NULL, lpszFileName, 
									IMAGE_BITMAP, 0, 0,
									LR_LOADFROMFILE | LR_CREATEDIBSECTION );
									//�t�@�C�����烍�[�h
		if( hBMP == NULL )
			return 0;
	}
	//BMP��������
	GetObject( hBMP, sizeof(bmp), &bmp );
	//�T�[�t�F�C�X�ɐݒ肷��\����
	ZeroMemory( &ddsd, sizeof(ddsd) );
	ddsd.dwSize 		= sizeof(ddsd);
	ddsd.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;//�V�X�e���������ɍ��
	if(g_pNkLib->GetDouble())
	{
		ddsd.dwWidth		= bmp.bmWidth * 2;
		ddsd.dwHeight		= bmp.bmHeight * 2;
	}
	else
	{
		ddsd.dwWidth		= bmp.bmWidth;
		ddsd.dwHeight		= bmp.bmHeight;
	}
	//�����o��BMP������
	m_bmpWidth = (int)bmp.bmWidth;
	m_bmpHeight = (int)bmp.bmHeight;
	//�T�[�t�F�C�X���쐬
	if(!g_pNkLib->CreateSurface(&ddsd, &m_pdds))
	{
		SAFE_RELEASE(m_pdds);
		return 0;
	}
	//BMP���T�[�t�F�C�X�ɕ`��
	if(!DrawBitmap(hBMP))
	{
		DeleteObject( hBMP );
		return 0;
	}
	DeleteObject( hBMP );
	//�J���[�L�[�ݒ�
	SetColorKey(RGB(255,0,255));
	return 1;
}
/*-----------------------------------------------------------------------------
	PNG�̃��[�h
-----------------------------------------------------------------------------*/
int CNkImage::LoadPNG(char* lpszFileName)
{
	HBITMAP 	   hBMP = NULL;
	BITMAP		   bmp;
	DDSURFACEDESC2 ddsd;
	//�r�b�g�}�b�v�n���h����PNG�����[�h
	hBMP = LoadPng( (string)lpszFileName);
	if( hBMP == NULL )
	{
		MB(lpszFileName);
		return 0;
	}
	//BMP��������
	GetObject( hBMP, sizeof(bmp), &bmp );
	//�T�[�t�F�C�X�ɐݒ肷��\����
	ZeroMemory( &ddsd, sizeof(ddsd) );
	ddsd.dwSize 		= sizeof(ddsd);
	ddsd.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;//�V�X�e���������ɍ��
	
	ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
	//RGB565�J���[
	ddsd.ddpfPixelFormat.dwRBitMask = 0x0000F800 ;
	ddsd.ddpfPixelFormat.dwGBitMask = 0x000007E0 ;
	ddsd.ddpfPixelFormat.dwBBitMask = 0x0000001F ;
	ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00000000 ;

	if(g_pNkLib->GetDouble())
	{
		ddsd.dwWidth		= bmp.bmWidth * 2;
		ddsd.dwHeight		= bmp.bmHeight * 2;
	}
	else
	{
		ddsd.dwWidth		= bmp.bmWidth;
		ddsd.dwHeight		= bmp.bmHeight;
	}
	//�����o��BMP������
	m_bmpWidth = (int)bmp.bmWidth;
	m_bmpHeight = (int)bmp.bmHeight;
	//�T�[�t�F�C�X���쐬
	if(!g_pNkLib->CreateSurface(&ddsd, &m_pdds))
	{
		SAFE_RELEASE(m_pdds);
		return 0;
	}
	//BMP���T�[�t�F�C�X�ɕ`��
	if(!DrawBitmap(hBMP))
	{
		DeleteObject( hBMP );
		return 0;
	}
	DeleteObject( hBMP );
	//�J���[�L�[�ݒ�
	SetColorKey(RGB(255,0,255));
	return 1;
}
/*-----------------------------------------------------------------------------
//�r�b�g�}�b�v�n���h������BMP��`��
-----------------------------------------------------------------------------*/
int CNkImage::DrawBitmap( HBITMAP hBMP )
{
	HDC 		   hDCImage;
	HDC 		   hDC;
	BITMAP		   bmp;
	DDSURFACEDESC2 ddsd;

	if( hBMP == NULL || m_pdds == NULL )
		return 0;
	//���X�g�A���Ă���
	if( FAILED( m_pdds->Restore() ) )
		return 0;
	//�T�[�t�F�C�X�����擾
	ddsd.dwSize  = sizeof(ddsd);
	m_pdds->GetSurfaceDesc( &ddsd );
	if( ddsd.ddpfPixelFormat.dwFlags == DDPF_FOURCC )
		return 0;
	//�g�p�\�Ȃa�l�o�`���𒊏o
	hDCImage = CreateCompatibleDC( NULL );
	if( NULL == hDCImage )
		return 0;
	SelectObject( hDCImage, hBMP );
	GetObject( hBMP, sizeof(bmp), &bmp );
	//�f�o�C�X�R���e�L�X�g
	if( FAILED( m_pdds->GetDC( &hDC ) ) )
		return 0;

	StretchBlt( hDC, 0, 0, 
				ddsd.dwWidth, ddsd.dwHeight, 
				hDCImage, 0, 0,
				bmp.bmWidth, bmp.bmHeight, SRCCOPY );

	if( FAILED( m_pdds->ReleaseDC( hDC ) ) )
		return 0;

	DeleteDC( hDCImage );

	return 1;
}
/*-----------------------------------------------------------------------------
//�J���[�L�[���Z�b�g
-----------------------------------------------------------------------------*/
int CNkImage::SetColorKey( DWORD dwColorKey )
{
	if( NULL == m_pdds )
		return 0;

	DDCOLORKEY ddck;
	ddck.dwColorSpaceLowValue  = ConvertGDIColor( dwColorKey );
	ddck.dwColorSpaceHighValue = ConvertGDIColor( dwColorKey );
	
	if(FAILED(m_pdds->SetColorKey( DDCKEY_SRCBLT, &ddck )))
		return 0;
	return 1;
}
//�����g�p�֐�
DWORD CNkImage::ConvertGDIColor( COLORREF dwGDIColor )
{
	if( m_pdds == NULL )
		return 0x00000000;

	COLORREF	   rgbT;
	HDC 		   hdc;
	DWORD		   dw = CLR_INVALID;
	DDSURFACEDESC2 ddsd;
	HRESULT 	   hr;

	//	Use GDI SetPixel to color match for us
	if( dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK)
	{
		rgbT = GetPixel(hdc, 0, 0); 	// Save current pixel value
		SetPixel(hdc, 0, 0, dwGDIColor);	   // Set our value
		m_pdds->ReleaseDC(hdc);
	}

	// Now lock the surface so we can read back the converted color
	ddsd.dwSize = sizeof(ddsd);
	hr = m_pdds->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );
	if( hr == DD_OK)
	{
		dw = *(DWORD *) ddsd.lpSurface; 
		if( ddsd.ddpfPixelFormat.dwRGBBitCount < 32 ) // Mask it to bpp
			dw &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;	
		m_pdds->Unlock(NULL);
	}

	//	Now put the color that was there back.
	if( dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK )
	{
		SetPixel( hdc, 0, 0, rgbT );
		m_pdds->ReleaseDC(hdc);
	}
	
	return dw;
}
/*-----------------------------------------------------------------------------
	�ʏ�`��i�\�[�g���j
-----------------------------------------------------------------------------*/
int CNkImage::DrawLayer(int iDestX,int iDestY,int layer,RECT* prcSrc,bool trans)
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
	dInfo.pDrawQue = new CDrawNormal(this,iDestX,iDestY,rctSrc,trans);
	g_pNkLib->AddDrawList(dInfo);
	return 1;
}
/*-----------------------------------------------------------------------------
	�g��k���`��i�\�[�g���j
-----------------------------------------------------------------------------*/
int CNkImage::DrawLayerStretch(LPRECT prcDest,LPRECT prcSrc,int layer,bool trans)
{
	//��ʊO
	if(prcDest&&(prcDest->right<=0)||prcDest->bottom<=0||prcDest->left>=SCREEN_WIDTH||prcDest->top>=SCREEN_HEIGHT)
		return 0;
	DRAWINFO dInfo;
	dInfo.spr = layer;
	RECT rctSrc,rctDest;
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
	if(prcDest)
	{
		rctDest=*prcDest;
	}
	else//NULL�̂Ƃ�
	{
		rctDest.left=0;
		rctDest.top=0;
		rctDest.right=m_width;
		rctDest.bottom=m_height;
	}
	dInfo.pDrawQue = new CDrawQueStretch(this,rctDest,rctSrc,trans);
	g_pNkLib->AddDrawList(dInfo);
	return 1;
}
/*-----------------------------------------------------------------------------
�C���[�W��h��Ԃ�
-----------------------------------------------------------------------------*/
int CNkImage::Fill(DWORD dwColor)
{
	DDBLTFX ddbltfx;
	ZeroMemory(&ddbltfx,sizeof ddbltfx);
	ddbltfx.dwSize = sizeof ddbltfx;
	ddbltfx.dwFillColor = dwColor;
	if(FAILED(m_pdds->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&ddbltfx)))
		return 0;
	return 1;
}
/*-----------------------------------------------------------------------------
	�s�N�Z�����Z
-----------------------------------------------------------------------------*/
//#define MASK1	0xf79ef79e	// 1111011110011110 half mask
#define MASK2	0x07e0f81f	// 0000011111100000 (565)Gmask + RBmask
#define MASK3	0xf81f07e0	// 1111100000011111 (565)RBmask + Gmask
//#define MASK4	0x001f07e0	// 0000000000011111 (565)Bmask + Gmask
//#define MASK5	0xf8000000	// 1111100000000000 (565)Rmask + 0
#define MASK6	0x07e00000	// G1MASK
#define MASK7	0xf81f0000	// 1111100000011111
#define MASK8	0x000007e0
#define MASK9	0x0000f81f

//565MASK
#define R1MASK	0xf8000000
#define G1MASK	0x07e00000
#define B1MASK	0x001f0000
#define R2MASK	0x0000f800
#define G2MASK	0x000007e0
#define B2MASK	0x0000001f

#define OUTPUTCOLOR(a,b)/* Outputf("\n%s:R1=%u;G1=%u;B1=%u;R2=%u;G2=%u;B2=%u; \n",\
					b,\
					(a & R1MASK) / 0x08000000,\
					(a & G1MASK) / 0x00200000,\
					(a & B1MASK) / 0x00010000,\
					(a & R2MASK) / 0x00000800,\
					(a & G2MASK) / 0x00000020,\
					(a & B2MASK) / 0x00000001);*/
//���߂Ȃ��A���t�@�u�����h
DWORD CCalAlpha::Get(DWORD dest,DWORD src)
{
	DWORD ret1= (src&MASK2)*m_alpha/ALPHA_MAX+(dest&MASK2)*m_beta/ALPHA_MAX;
	DWORD ret2= (src&MASK3)/ALPHA_MAX*m_alpha+(dest&MASK3)/ALPHA_MAX*m_beta;
	DWORD ret = ret1&MASK2 | ret2&MASK3;
	return ret;
}
//���߂��A���t�@�u�����h
DWORD CCalTransAlpha::Get(DWORD dest,DWORD src)
{
	if(src==COLORKEY1 || (src & _mask01) == COLORKEY3 || (src & _mask10) == COLORKEY2)
		return dest;
	else
	{
		DWORD ret1= (src&MASK2)*m_alpha/ALPHA_MAX+(dest&MASK2)*m_beta/ALPHA_MAX;
		DWORD ret2= (src&MASK3)/ALPHA_MAX*m_alpha+(dest&MASK3)/ALPHA_MAX*m_beta;
		return ret1&MASK2 | ret2&MASK3;
	}
}
