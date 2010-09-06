/*-----------------------------------------------------------------------------

	EnemyShot.h

-----------------------------------------------------------------------------*/
#ifndef AFX_ENEMYSHOT_H
#define AFX_ENEMYSHOT_H

#define ENEMYSHOT_NUM 1000
class CEnemyShot;
extern CObjArray<CEnemyShot> g_enemyShot;

class CEnemyShot : public CObj
{
public:
	CEnemyShot(CNkImage* pimg,VECT pos,VECT vel,int anm=0);
	virtual int StepFrame();
	int Pull(VECT pos);
	VECT GetVel(){return m_vel;}
protected:
	VECT m_vel;		//�ŏI���x
	VECT m_velOrg;	//���̑��x
	VECT m_pul;		//�z�����݂̑��x
	int m_anm;
};
#endif
