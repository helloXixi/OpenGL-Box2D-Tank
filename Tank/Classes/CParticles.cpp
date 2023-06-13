/*-------------------------------------------------------

     File: CParticles.cpp  

     Prepared by wxl in  2,5th,2011

  ---------------------------------------------------------*/

#include  "CParticles.h"

#import <OpenGLES/ES1/gl.h>

#include  "stdlib.h"
#include "stdio.h"

//����һ��  [0,1]֮��������
#define  RANDOM_FLOAT  (((float)rand())/RAND_MAX)

Vector3D  NULL_VECTOR3D=Vector3D(0.0f,0.0f,0.0f);

/*----------------------------------------
		CParticles   All   function
  ----------------------------------------*/
void CParticle::Initialize(CParticleSystem * ParentSystem)
{

	//ע����������  (RANDOM_FLOAT*2.0f-1.0f)  ��ֵΪ[-1,1] ��������

	//���� ������0
	m_fAge = 0.0f;

	//û�취 ��������������������
	m_fDieAge = ParentSystem->m_fMinDieAge + 
			     ((ParentSystem->m_fMaxDieAge - ParentSystem->m_fMinDieAge)*RANDOM_FLOAT);
	if (m_fDieAge == 0.0f) m_fDieAge=2.0;  //ʵ��û�и��������
	
	//��ʼ����
	m_Position = ParentSystem->m_EmitterPosition;
	//������  ����ƫ���� ���������ܴ���һ�����η�����
	//��Ȼ���е����Ӷ��Ǵ�һ����ð������  (-1,1)
	m_Position.x += ParentSystem->m_CreationDeviation.x*(RANDOM_FLOAT*2.0f-1.0f);
	m_Position.y += ParentSystem->m_CreationDeviation.y*(RANDOM_FLOAT*2.0f-1.0f);
	m_Position.z += ParentSystem->m_CreationDeviation.z*(RANDOM_FLOAT*2.0f-1.0f);


	//����ķ���   ���䷽���ƫ��  �ͷ������С�������  ������������ٶ�
	//���� ȷ����λ   Ȼ��ȷ����С
	m_Velocity.x = ParentSystem->m_EmitDirection.x + ParentSystem->m_EmitDirectionDeviation.x*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity.y = ParentSystem->m_EmitDirection.y + ParentSystem->m_EmitDirectionDeviation.y*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity.z = ParentSystem->m_EmitDirection.z + ParentSystem->m_EmitDirectionDeviation.z*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity = m_Velocity * ((ParentSystem->m_fMinEmitSpeed + 
		                         (ParentSystem->m_fMaxEmitSpeed - ParentSystem->m_fMinEmitSpeed)*RANDOM_FLOAT));
  

	//ͬ�� ������ ����*��С  ����û��ƫ��  û��Ҫ��
	m_Acceleration = ParentSystem->m_AccelerationDirection* 
		              (ParentSystem->m_fMinAcceleration + 
					  (ParentSystem->m_fMaxAcceleration-ParentSystem->m_fMinAcceleration)*RANDOM_FLOAT);
 

	//������ɫ
	m_Color = ParentSystem->m_MinEmitColor + 
		   ((ParentSystem->m_MaxEmitColor-ParentSystem->m_MinEmitColor) * RANDOM_FLOAT);
	//�������Ǽ����������ʱ�����ɫ ��RGB��  �������� ������ɫ�仯��
	Vector3D EndColor = ParentSystem->m_MinDieColor + 
		   ((ParentSystem->m_MaxDieColor-ParentSystem->m_MinDieColor) * RANDOM_FLOAT);
	m_ColorChange = (EndColor-m_Color) / m_fDieAge;

	//ͬ����ɫ
	m_fAlpha = ParentSystem->m_fMinEmitAlpha 
		       + ((ParentSystem->m_fMaxEmitAlpha - ParentSystem->m_fMinEmitAlpha) * RANDOM_FLOAT);
	float fEndAlpha = ParentSystem->m_fMinDieAlpha 
		       + ((ParentSystem->m_fMaxDieAlpha - ParentSystem->m_fMinDieAlpha) * RANDOM_FLOAT);
	m_fAlphaChange = (fEndAlpha - m_fAlpha) / m_fDieAge;

   //ͬ����ɫ
	m_fSize = ParentSystem->m_fMinEmitSize +
			 ((ParentSystem->m_fMaxEmitSize - ParentSystem->m_fMinEmitSize) * RANDOM_FLOAT);
	float fEndSize = ParentSystem->m_fMinDieSize +
			 ((ParentSystem->m_fMaxDieSize - ParentSystem->m_fMinDieSize) * RANDOM_FLOAT);
	m_fSizeChange = (fEndSize - m_fSize) / m_fDieAge;

	//������ת�Ƕȳ�ʼΪ0 �����û��ȼ���
	m_fSpinAngle = 0.0f;
	//���ٶ�  ��   �Ǽ��ٶ�   a=a0+wt;
	m_fSpinSpeed = ParentSystem->m_fMinEmitSpinSpeed +
			((ParentSystem->m_fMaxEmitSpinSpeed - ParentSystem->m_fMinEmitSpinSpeed) * RANDOM_FLOAT);
	m_fSpinAcceleration = ParentSystem->m_fMinSpinAcceleration +
			((ParentSystem->m_fMaxSpinAcceleration - ParentSystem->m_fMinSpinAcceleration) * RANDOM_FLOAT);

	//����  ���Լ�����   ��������   ����ʱ�����ʹ��
	m_bIsAlive = true;

	m_ParentSystem = ParentSystem;


}


void CParticle::Update(float timePassed)
{
	//���ȵ����䳤����
	m_fAge += timePassed;

	//�����Ҫ����
	if (m_fAge >= m_fDieAge) 
	{
			m_fAge = 0.0f;
			m_bIsAlive = false;

			m_ParentSystem->m_iParticlesInUse--;
			return;
	}

	//�ı�����Ӧ�øı��
	m_fAlpha += m_fAlphaChange*timePassed;
	m_Color = m_Color + m_ColorChange*timePassed;

	// V= V0+at
	m_Velocity = m_Velocity + m_Acceleration*timePassed;

	//s=vt+1/2a*t*t  Ӧ�����������  �ǵ�  ����������  ����������͹���  �������
	//��ô����������  ������Ҫ��   s+=vt;  ע��Ŷ  ���ǿ�û������+=
	m_Position = m_Position + (m_Velocity*timePassed);


	//�����Ӧ��ע����    �ǵ���ʹ�õ��������������� ���ǵ�״����
	if(m_ParentSystem->m_bUseTexture==true)
	{
		//ͬ��  v=v0+at    s=so+vt
		m_fSpinSpeed += m_fSpinAcceleration*timePassed;
		m_fSpinAngle  += m_fSpinSpeed*timePassed;

		//����ת��һȦ�� �ʹ����� һֱ��Ҳû��˼ ������
		if(m_fSpinAngle>2*GLT_PI)
			m_fSpinAngle=0.0f;
	}
	else 
		;
		m_fSize  += m_fSizeChange *timePassed;
}


void CParticle::Render()
{
	//��״����
	if (!m_ParentSystem->m_bUseTexture) 
	{

		glPointSize(m_fSize);

		float color[4];
		color[0] = m_Color.x;
		color[1] = m_Color.y;
		color[2] = m_Color.z;
		color[3] = m_fAlpha;

		glColorPointer(	4,   //(rgba)
							GL_FLOAT,
							0,
							&color[0]);

		glVertexPointer(	3,   //vertex (x,y,z)
						GL_FLOAT,
						0,   //���
						&m_Position.x);

		glDrawArrays(GL_POINTS,
							0,
							1);

	}
	else
	{
		GLfloat colorer[4][4];
		for(int i=0;i<4;i++)
		{
			colorer[i][0] = m_Color.x;
			colorer[i][1] = m_Color.y;
			colorer[i][2] = m_Color.z;
			colorer[i][3] = m_fAlpha;
		}
		
		//��������
		GLshort coord[4][2]={0,0,1,0,1,1,0,1};
		
		float  lengthOF=(float)(m_fSize *0.707);
		float  xOF=(float)cos(m_fSpinAngle);
		float  yOF=(float)sin(m_fSpinAngle);
		
		//��ת����Ե�λ���ĵ� �ı��ε�λ��
		GLfloat  vertex[4][3];
		
		vertex[0][0]=lengthOF*(yOF-xOF)-m_Position.x;
		vertex[0][1]=lengthOF*(-xOF-yOF)-m_Position.y;
		vertex[0][2]=m_Position.z;
		
		vertex[1][0]=lengthOF*(xOF+yOF)-m_Position.x;
		vertex[1][1]=lengthOF*(yOF-xOF)-m_Position.y;
		vertex[1][2]=m_Position.z;
		
		vertex[2][0]=lengthOF*(xOF-yOF)-m_Position.x;
		vertex[2][1]=lengthOF*(xOF+yOF)-m_Position.y;
		vertex[2][2]=m_Position.z;
		
		vertex[3][0]=lengthOF*(-xOF-yOF)-m_Position.x;
		vertex[3][1]=lengthOF*(xOF-yOF)-m_Position.y;
		vertex[3][2]=m_Position.z;
		
		glColorPointer(4,GL_FLOAT,0,&colorer[0][0]);
		
		glTexCoordPointer(
						  2,
						  GL_SHORT,
						  0,
						  &coord[0][0]
						  );
		
		glVertexPointer(	3,   //vertex (x,y,z)
						GL_FLOAT,
						0,   //���
						&vertex[0][0]);
		
		glDrawArrays(GL_TRIANGLE_FAN,
					 0,
					 4);
		
	}


}

/*----------------------------------------
		CParticleSystem   All   function
  ----------------------------------------*/

void  CParticleSystem::SetEmitter(Vector3D pos,Vector3D dev)
{
		m_EmitterPosition=pos;
		m_CreationDeviation=dev;  
}

void  CParticleSystem::SetEmissionDirection(Vector3D direction, Vector3D dev)
{
		m_EmitDirection=direction;
		m_EmitDirectionDeviation=dev; 
}

void  CParticleSystem::SetAcceleration(Vector3D acc, float Min, float Max)
{
		m_AccelerationDirection=acc;
		m_fMinAcceleration=Min;
		m_fMaxAcceleration=Max;
}


void  CParticleSystem::SetCreationDieColor(Vector3D MinEmit, Vector3D MaxEmit,Vector3D MinDie, Vector3D MaxDie)
{
		m_MinEmitColor=MinEmit;
		m_MaxEmitColor=MaxEmit;
		m_MinDieColor=MinDie;
		m_MaxDieColor=MaxDie;
}

void  CParticleSystem::SetAlphaValues(float MinEmit, float MaxEmit, float MinDie, float MaxDie)
{
	m_fMinEmitAlpha=MinEmit;
	m_fMaxEmitAlpha=MaxEmit;
	m_fMinDieAlpha=MinDie;
	m_fMaxDieAlpha=MaxDie;
}

void  CParticleSystem::SetSpinSpeed(float Min, float Max)
{
	m_fMinEmitSpinSpeed=Min;
	m_fMaxEmitSpinSpeed=Max;
}


void  CParticleSystem::SetSizeValues(float EmitMin, float EmitMax, float DieMin, float DieMax)
{
	m_fMinEmitSize=EmitMin;
	m_fMaxEmitSize=EmitMax;
	m_fMinDieSize=DieMin;
	m_fMaxDieSize=DieMax;
}
	
//���캯��
CParticleSystem::CParticleSystem()
{
	m_EmitterPosition=NULL_VECTOR3D;
	m_CreationDeviation=NULL_VECTOR3D;  

	m_EmitDirection=NULL_VECTOR3D;
	m_EmitDirectionDeviation=NULL_VECTOR3D;
	
	/**********************************
	��������Ҫ������ֱ�������ʼ��
	/**********************************/
	m_fMinEmitSpeed=m_fMaxEmitSpeed=0.0f;
	m_fMinDieAge=m_fMaxDieAge=0.0f;
	/**********************************/

	m_AccelerationDirection=NULL_VECTOR3D;
	m_fMinAcceleration=m_fMaxAcceleration=0.0f;
	
	m_fMinEmitAlpha=m_fMaxEmitAlpha=m_fMinDieAlpha=m_fMaxDieAlpha=0.0f;

	m_MinEmitColor=NULL_VECTOR3D;
	m_MaxEmitColor=NULL_VECTOR3D;
	m_MinDieColor=NULL_VECTOR3D;
	m_MaxDieColor=NULL_VECTOR3D;

	m_fMinEmitSpinSpeed=m_fMaxEmitSpinSpeed=0.0f;
	//�ȱ���Բ���˶�
	m_fMinSpinAcceleration=m_fMaxSpinAcceleration=0.0f;

	m_fMinEmitSize=m_fMaxEmitSize=m_fMinDieSize=m_fMaxDieSize=1.0f;

	m_bRecreateWhenDied=true;  

	m_bUseTexture=true;

	m_pParticles=NULL;
	m_iMaxParticles=m_iParticlesInUse=0;
}

CParticleSystem::~CParticleSystem()
{
	delete m_pParticles;
	m_pParticles=NULL;

}

bool	CParticleSystem::Initialize(int iNumParticles)
{
	m_pParticles = new CParticle[iNumParticles];

	if (m_pParticles == NULL) 
	{
		return false;
		m_iMaxParticles = 0;
		m_iParticlesInUse = 0;
	}

	m_iMaxParticles = iNumParticles;
	m_iParticlesInUse = 0;

	//���� ÿ�����Ӷ�������
	for (int i = 0; i < iNumParticles; i++)
	{
		m_pParticles[i].m_bIsAlive = false;
	}

	return true;
}

void	CParticleSystem::ActiveAllParticlesAfterInit()
{
	m_iParticlesInUse=0;

	for (int i = 0; i < m_iMaxParticles; i++)
	{
		m_pParticles[i].Initialize(this);
		m_iParticlesInUse++;
	}

}

void	CParticleSystem::UpdateSystem(float timePassed)
{

	for (int i = 0; i < m_iMaxParticles; i++)
	{
		//������Щ���ŵ�
		if (m_pParticles[i].m_bIsAlive)
		{
			m_pParticles[i].Update(timePassed);
		}
		//���ϵͳ�������������
		else if (m_bRecreateWhenDied)
		{
				m_pParticles[i].Initialize(this);
				m_iParticlesInUse++;
				
				//Update the particle: This has an effect, as if the particle would have
				//been emitted some milliseconds ago. This is very useful on slow PCs:
				//Especially if you simulate something like rain, then you could see that 
				//many particles are emitted at the same time (same "UpdateSystem" call),
				//if you would not call this function:	
				m_pParticles[i].Update(RANDOM_FLOAT*timePassed);  
		}
	}
}


void	CParticleSystem::Render()
{
	for (int i = 0; i < m_iMaxParticles; i++)
	{
		if (m_pParticles[i].m_bIsAlive)
			m_pParticles[i].Render();
	}
}
