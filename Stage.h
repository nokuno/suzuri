/*-----------------------------------------------------------------------------

	Stage.h

-----------------------------------------------------------------------------*/
#ifndef AFX_STAGE_H
#define AFX_STAGE_H

class CStage;
class CNkScript;
extern CStage* g_pStage;
//�P�X�e�[�W�̓G�̑����ő�
#define ENEMY_ALLNUM 400
//�P�X�e�[�W�̓G�Ɏg���摜��
#define ENEMYIMAGE_NUM 10
#define TIKEI_ALLNUM 100
//�G�f�[�^�\����
//�G�̏o���́A�o��������p���ĊǗ�����B
//�����O��CEnemy��new���Ă����Ag_enemy�ɓo�^�������_��
//���ۂɓ����o���A�\���������B
struct EnemyData
{
	int time;		//�o������
	CEnemy* pEnemy;	//�G�I�u�W�F�N�g�ւ̃|�C���^
	EnemyData(){};
	EnemyData(int t,CEnemy* p){time=t;pEnemy=p;};//�R���X�g���N�^
};
//�n�`�\����
struct Tikei
{
	int anm[12];	//�A�j���[�V����
	int n;	//��ԍ�
};
//�X�e�[�W�N���X�B
//�X�e�[�W�t�@�C����v����f�[�^��S�Ď��B
//�G�̏o����w�i�E�n�`���Ǘ�����B
class CStage
{
public:
	CStage(int n);
	virtual ~CStage();
	virtual int StepFrame();
	int End();
	int EnemyApear();
	int DrawTikei();
	int StageRead(char* lpszFileName);
	int CollTikei(VECT pos);
	int CollTikei(double x,double y){return CollTikei(VECT(x,y));}
protected:
	//�G
	EnemyData m_enemyData[ENEMY_ALLNUM];	//�G�o���e�[�u��
	int m_index;
	int m_time;
	CNkScript* m_pScript;	//�X�N���v�g
	//�n�`
	CNkImage m_imgTikei;	//�n�`�C���[�W
	Tikei* m_pTikei[17];					//�A�N�e�B�u�Ȓn�`
	Tikei* m_pTikeiTable[TIKEI_ALLNUM];		//�n�`�e�[�u��
	double m_tikeiSpeed;
	double m_tikeiScroll;
	int m_tikeiIndex;		//�A�N�e�B�u���̃C���f�b�N�X�i�e�[�u���̏d���g�p��������)
	int m_tikeiIndexTable;	//�e�[�u�����̃C���f�b�N�X
	//�w�i
	CNkImage m_imgBg;		//�w�i�ꖇ�G
	double m_scroll;
	double m_speed;
	//�X�e�[�W�J�n�̕���
	CNkImage m_imgStart;
};
#endif
