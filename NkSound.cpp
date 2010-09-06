/*-----------------------------------------------------------------------------

	NkSound.cpp

-----------------------------------------------------------------------------*/
#include "stdapp.h"
#include "utility.h"
#include "NkLib.h"
#include "NkSound.h"

/*-----------------------------------------------------------------------------

	CNkSound�N���X

-----------------------------------------------------------------------------*/
CNkSound::CNkSound()
{
	for(int i=0;i<SOUNDBUFFER_NUM;++i)
		m_pdsb[i] = NULL;
	m_pDS = g_pNkLib->GetDS();
	m_index=0;
}
CNkSound::CNkSound(char* lpszFileName)
{
	for(int i=0;i<SOUNDBUFFER_NUM;++i)
		m_pdsb[i] = NULL;
	m_pDS = g_pNkLib->GetDS();
	m_index=0;
	Load(lpszFileName);
}
CNkSound::~CNkSound()
{
	Release();
}
int CNkSound::Release()
{
	for(int i=0;i<SOUNDBUFFER_NUM;++i)
		SAFE_RELEASE(m_pdsb[i]);
	return 0;
}
int CNkSound::Play(DWORD dwFlags)
{
//	m_pdsb[m_index]->Stop();
	m_pdsb[m_index]->Play(0,0,dwFlags);
	m_index++;
	if(m_index==SOUNDBUFFER_NUM)
		m_index=0;
	return 0;
}
int CNkSound::Stop()
{
	DWORD dwFlags;
	for(int i=0;i<SOUNDBUFFER_NUM;++i)
	{
		if(FAILED(m_pdsb[i]->GetStatus(&dwFlags)))
			return 0;
		else
			if(dwFlags & DSBSTATUS_PLAYING)
				m_pdsb[i]->Stop();
	}
	return 1;
}
int CNkSound::Load(char* lpszFileName)
{
	//RIFF�\���̃t�@�C����ǂݍ��ނ̂ɕK�v�ȍ\���̂ł��B
	//�킩��₷�����O��t���܂����B
	MMCKINFO oya_chunk, ko_chunk;
	
	//wave�t�@�C���̃I�[�v��
	HMMIO hmmio = mmioOpen(lpszFileName, NULL, MMIO_READ|MMIO_ALLOCBUF);
	if(!hmmio) {
		return false;
	}
	
	//wave�t�@�C�����ǂ������ׂ�
	oya_chunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if(mmioDescend(hmmio, &oya_chunk, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR) {
		mmioClose(hmmio, 0);
		return false;
	}
	
	//fmt�`�����N�ֈړ�����
	ko_chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if(mmioDescend(hmmio, &ko_chunk, &oya_chunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) {
		mmioClose(hmmio, 0);
		return false;
	}
	
	//fmt�`�����N��ǂݎ��܂��B
	//WAVEFORMATEX�\���̂ɕK�v�ȏ�񂪃R�s�[����܂��B
	//�T�E���h�o�b�t�@����鎞�ɕK�v�ł��B
	//�摜�Ō����΂܂��A���A�����A�r�b�g�[�x�Ƃ�������ւ���擾�����񂾂Ǝv���Ă��������B
	WAVEFORMATEX wfex;
	if(mmioRead(hmmio, (HPSTR)&wfex, (LONG)ko_chunk.cksize) != (LONG)ko_chunk.cksize) {
		mmioClose(hmmio, 0);
		return false;
	}
	
	//fmt�T�u�`�����N�̊O���Ɉړ�����
	//�`�����N��������̂ŊO�ɏo�܂��B
	//�����A�Ȃ�ăZ���X�̖��������Ȃ�ł��傤(^^;
	//����������ƃX�}�[�g�ɏo����n�Y���낤�ƌ����Ă�肽���ł��ˁB
	mmioAscend(hmmio, &ko_chunk, 0);
	
	//data�`�����N�Ɉړ�
	ko_chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(mmioDescend(hmmio, &ko_chunk, &oya_chunk, MMIO_FINDCHUNK)) {
		mmioClose(hmmio, 0);
		return false;
	}
	
	//�K�v�ŏ����̃��������m�ۂ���
	LPBYTE pdata = (LPBYTE)malloc(ko_chunk.cksize);
	
	//data�`�����N��ǂݎ��
	//�������摜�t�@�C���Ō����ƈ�ԗ~�����摜�̕����ł��ˁB
	//�ۂ��Ƃ������肢���������ɂ��܂��B
	if(mmioRead(hmmio, (HPSTR)pdata, (LONG)ko_chunk.cksize) != (LONG)ko_chunk.cksize){
		free(pdata);
		mmioClose(hmmio, 0);
		return false;
	}
	
	//�K�v�ȃf�[�^����������̂Ť�E�F�[�u�t�@�C�������
	//�Z���X�̊������Ȃ��ǂݍ��݂���E�o�ł��B(^^)
	mmioClose(hmmio, 0);

//�������牺���T�E���h�o�b�t�@�̍쐬�H���ɂȂ�܂��B
	DSBUFFERDESC dsbdesk;
		
	//DirectX�ō\���̂��g���悤�Ȏ��̂��񑩂ł���(^^)
	ZeroMemory(&dsbdesk, sizeof(DSBUFFERDESC));
	dsbdesk.dwSize = sizeof(DSBUFFERDESC);
	
	//�T�E���h�o�b�t�@���ǂ̂悤�ȃo�b�t�@�Ȃ̂��w�肵�܂��B
	//����͕K�v�Œ���̕���I�����܂����B
	//DSBCAPS_CTRLPAN		�p�����삪�\
	//						�p�����Ă̂͊ȒP�Ɍ����Ή��̍��E�̒��߂̎��ł��B
	//DSBCAPS_CTRLVOLUME	�{�����[���̑��삪�\
	//DSBCAPS_GLOBALFOCUS	�A�v���P�[�V�������J�����g�������Ă�
	//						�T�E���h�o�b�t�@�͎����܂���B
	//						���̃t���O�͂����Ɛ[���Ӗ�������܂���
	//						�܂��A����͐������܂���B
	dsbdesk.dwFlags = DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_GLOBALFOCUS ;

	//�o�b�t�@�̃T�C�Y���w��
	//������������ǂ݂܂����B
	dsbdesk.dwBufferBytes = ko_chunk.cksize;

	//������wave�t�@�C������ǂݍ��񂾏��������ŋ����Ă��܂��B
	dsbdesk.lpwfxFormat = &wfex; 
	
	for(int i=0;i<SOUNDBUFFER_NUM;i++)
	{
		//�I�t�X�N���[���o�b�t�@�̐���
		if(FAILED(m_pDS->CreateSoundBuffer(&dsbdesk, &m_pdsb[i], NULL))){		
			return false;
		}

		//��قǓǂݍ���wave�̃f�[�^�������T�E���h�o�b�t�@�ɃR�s�[���܂��B
		LPVOID pmem;
		DWORD size;
		
		//�T�E���h�o�b�t�@�����b�N
		//���b�N�����T�C�Y�ƃo�b�t�@�̐擪�A�h���X���擾�o���܂��B
		m_pdsb[i]->Lock(0, ko_chunk.cksize, &pmem, &size, NULL, 0, 0);
		
		//�ǂݍ��ݍς݂�wave�f�[�^���T�E���h�o�b�t�@�ɃR�s�[���܂��B
		//�ǂݍ��񂾃T�C�Y���R�s�[
		memcpy(pmem, pdata, size);
		
		//�o�b�t�@���A�����b�N
		//Draw�̎��Ɠ����ł��B
		m_pdsb[i]->Unlock(pmem, size, NULL, 0);
	}
	
	//wave�f�[�^�̓ǂݍ��݂Ɏg�����̈���J�����܂��B
	//��n���͊m�肵�܂��傤�B
	free(pdata);

	return true;
}