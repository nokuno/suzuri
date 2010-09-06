/*-----------------------------------------------------------------------------

  Player.h

-----------------------------------------------------------------------------*/
#ifndef AFX_PLAYER_H
#define AFX_PLAYER_H

class CPlayer;
class CSurasuta;
//�O���[�o���ϐ�
extern CPlayer* g_pPlayer;

//�X���X�^�̒x��
#define SURASUTA_LATE 5
//�X���X�^�N���X
class CSurasuta
{
public:
	CSurasuta(CNkImage* pimg,VECT pos);
	~CSurasuta(){};
	int StepFrame(VECT pos,int state);	//state: 0:normal,1;suikomi,2:shot,3:gattai,4:apear
	VECT GetPos(){return m_posGun;};
	VECT GetSurasutaPos(){return m_posSurasuta;};
protected:
	CNkImage* m_pimg;
	VECT m_posSurasuta;
	VECT m_posGun;
	VECT m_playerpos[SURASUTA_LATE];	//�v���C���[�ʒu�̕ۑ�
	int m_index;		//���݂̃X���X�^�̃A�j���ʒu�i�Y�����j
	int m_anmtime;		//�ӂ�ӂ�A�j���p�^�C�}�[
	int m_state;		//�P�t���[���O��state
};

//CPlayer�v���C���[�N���X
class CPlayer  : public CObj
{
public:
	CPlayer(CNkImage* pimg, CNkImage* pimgSurasuta, int zanki,int item);
	virtual ~CPlayer();
	virtual int StepFrame();
	int Damage();
	int IsColl2(CObj* pobj);	//�`�[�g�p�i�����ڂ�菬�������������]
	int IsColl2(CEnemy* pEnemy);	//�`�[�g�p�i�����ڂ�菬�������������]
	int CollTikei();
	int GetState(){return m_state;}
protected:
	CSurasuta m_surasuta;	//�X���X�^
	int m_shottime;		//�V���b�g�p�^�C�}�[
	int m_anmtime;		//�A�j���p�^�C�}�[
	int m_zanki;		//�c�
	int m_state;		//��ԃt���O(0:normal,1:apear,2:fall
	int m_muteki;		//���G����
	int m_item;			//�Ƃ����A�C�e���̐�
	int m_power;		//�V���b�g�̃p���[(0�Ȃ�T�u�V���b�g�Ȃ��A�Ȍ�͘A�˃A�b�v�j
};

//PlayerShot�I�u�W�F�N�g
#define PSHOT_NUM 100
class CPlayerShot : public CObj  
{
public:
	CPlayerShot():CObj(){};
	CPlayerShot(CNkImage* pimg, VECT pos, VECT vel):CObj(pimg,pos){m_vel=vel;};
	int StepFrame();
protected:
	VECT m_vel;	//���x
};
#endif