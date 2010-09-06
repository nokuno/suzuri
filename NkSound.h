// NkSound.h: CNkSound �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_SOUND_H
#define AFX_SOUND_H

//�ő哯��������
#define SOUNDBUFFER_NUM 5

class CNkSound  
{
public:
	CNkSound();
	CNkSound(char* lpszFileName);
	virtual ~CNkSound();
	int Load(char* lpszFileName);
	int Play(DWORD dwFlags);
	int Stop();
	int Release();
private:
	LPDIRECTSOUNDBUFFER m_pdsb[SOUNDBUFFER_NUM];
	LPDIRECTSOUND		m_pDS;
	int m_index;
};

#endif