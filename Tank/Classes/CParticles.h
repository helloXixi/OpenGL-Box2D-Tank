
/*-------------------------------------------------------

     File: CParticles.h  

     Prepared by wxl in  2,5th,2011

  ---------------------------------------------------------*/

#include "Vector3D.h"

#define  GLT_PI                3.14159265358979323846

class CParticleSystem;

class CParticle
{
private:
  
	//λ��
	Vector3D m_Position;
	//�ٶ�
	Vector3D m_Velocity;
	//���ٶ�  
	Vector3D m_Acceleration;

	//͸���� �� �仯��
	float      m_fAlpha;
	float	   m_fAlphaChange; 

	//ͬ��  ��ɫ �� ��ɫ�仯��
	Vector3D m_Color;   
	Vector3D m_ColorChange; 

	//��������  ��ǰ����
	float	   m_fDieAge;
	float	   m_fAge;	

	/*---------------------------
	*  m_bUseTexture=TRUE
	*  ʱ��  ʹ�ö����  ��ת
	*----------------------------*/
	//���������   ��ת�ĽǶ�  ���ٶ�  �Ǽ��ٶ�
	float	   m_fSpinAngle; 
	float	   m_fSpinSpeed;
	float      m_fSpinAcceleration;
 
	/*---------------------------
	*  m_bUseTexture=FALSE
	*  ʱ��  ʹ�õ�״����
	*----------------------------*/
	//��״����   ��Ĵ�С   ��С�仯��
	float	   m_fSize;
	float	   m_fSizeChange;	

	//������ϵͳ��ȡ��ʼ������
	CParticleSystem * m_ParentSystem;
	
public:
	bool	   m_bIsAlive; 
	void       Initialize(CParticleSystem * ParentSystem);
	void       Update(float timePassed);
	void       Render();  

};

/*-------------------------------
		����ϵͳ
  -------------------------------*/

class CParticleSystem
{

public: 
	//����ĵط�
	Vector3D		m_EmitterPosition;
	//����뾶�ڵ�ƫ��
	Vector3D		m_CreationDeviation;  

	//����ķ���
	Vector3D		m_EmitDirection;
	//���䷽���ƫ�� ����Ȼ�����ܶ���һ������԰ɣ�
	Vector3D		m_EmitDirectionDeviation; 
	//�������С����ٶ�
	float			m_fMinEmitSpeed;
	float			m_fMaxEmitSpeed;

	//���ٶȷ���
	Vector3D		m_AccelerationDirection;
	//��С�����ٶ�
	float			m_fMinAcceleration;
	float			m_fMaxAcceleration;
	
	//��С���  ����  ����  ʱ��� ͸����
	float			m_fMinEmitAlpha;
	float			m_fMaxEmitAlpha;
	float			m_fMinDieAlpha;
	float			m_fMaxDieAlpha;

	//��С���   ���� ����ʱ�����ɫ��RGB��
	Vector3D		m_MinEmitColor;
	Vector3D		m_MaxEmitColor;
	Vector3D		m_MinDieColor;
	Vector3D		m_MaxDieColor;

	//��С���   ����ʱ��
	float			m_fMinDieAge;
	float			m_fMaxDieAge;

	/*---------------------------
	*  m_bUseTexture=TRUE
	*  ʱ��  ʹ�ö����  ��ת
	*----------------------------*/
	//�������ת�ٶ�
	float			m_fMinEmitSpinSpeed;
	float			m_fMaxEmitSpinSpeed;
	//��ת�Ǽ��ٶ�
	float			m_fMinSpinAcceleration;
	float			m_fMaxSpinAcceleration;

	/*---------------------------
	*  m_bUseTexture=FALSE
	*  ʱ��  ʹ�õ�״����
	*----------------------------*/
	//��С��� ���� ����ʱ��Ĵ�С
	float			m_fMinEmitSize;
	float			m_fMaxEmitSize;
	float			m_fMinDieSize;
	float			m_fMaxDieSize;

/*---------------------------------------
*
*  ����ϵͳ����������
*
*----------------------------------------*/	
	//������������   ���½�ô
	bool			m_bRecreateWhenDied;  

	//��������ͼ  �����õ�״����
	bool			      m_bUseTexture;

	//��������������Ӹ���
	CParticle    *m_pParticles;
	//������ʱ�����ɶ��ٸ�����
	int				m_iMaxParticles;
	//��ǰʹ���е��ж��ٸ�
	int				m_iParticlesInUse;

/*--------------------------------------
*
*  ����  ����
*
*---------------------------------------*/
	void			SetEmitter              (Vector3D pos,Vector3D dev);
	void			SetEmissionDirection(Vector3D direction, Vector3D dev);

	void			SetAcceleration        (Vector3D acc, float Min, float Max);

	void            SetCreationDieColor  (Vector3D MinEmit, Vector3D MaxEmit,Vector3D MinDie, Vector3D MaxDie);
	void			SetAlphaValues        (float MinEmit, float MaxEmit, float MinDie, float MaxDie);


	void			SetSpinSpeed          (float Min, float Max);
	void			SetSizeValues          (float EmitMin, float EmitMax, float DieMin, float DieMax);		
	
/*-------------------------------------
*  ��ʼ��  ����
/--------------------------------------*/

	//���캯��
	CParticleSystem();
	~CParticleSystem();

	bool			Initialize(int iNumParticles);
	void            ActiveAllParticlesAfterInit();

/*-------------------------------------
*
*   ����  ��Ⱦ
*-------------------------------------*/

	void			UpdateSystem(float timePassed);
	void			Render();
};