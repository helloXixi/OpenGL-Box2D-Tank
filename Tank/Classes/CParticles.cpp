/*-------------------------------------------------------

     File: CParticles.cpp  

     Prepared by wxl in  2,5th,2011

  ---------------------------------------------------------*/

#include  "CParticles.h"

#import <OpenGLES/ES1/gl.h>

#include  "stdlib.h"
#include "stdio.h"

//产生一个  [0,1]之间的随机数
#define  RANDOM_FLOAT  (((float)rand())/RAND_MAX)

Vector3D  NULL_VECTOR3D=Vector3D(0.0f,0.0f,0.0f);

/*----------------------------------------
		CParticles   All   function
  ----------------------------------------*/
void CParticle::Initialize(CParticleSystem * ParentSystem)
{

	//注意以下所有  (RANDOM_FLOAT*2.0f-1.0f)  的值为[-1,1] 创建正反

	//首先 年龄是0
	m_fAge = 0.0f;

	//没办法 它就是这样计算生命的
	m_fDieAge = ParentSystem->m_fMinDieAge + 
			     ((ParentSystem->m_fMaxDieAge - ParentSystem->m_fMinDieAge)*RANDOM_FLOAT);
	if (m_fDieAge == 0.0f) m_fDieAge=2.0;  //实在没有给它两秒吧
	
	//初始中心
	m_Position = ParentSystem->m_EmitterPosition;
	//别忘了  还有偏移呢 这样我们能创建一个球形发射区
	//不然所有的粒子都是从一个点冒出来的  (-1,1)
	m_Position.x += ParentSystem->m_CreationDeviation.x*(RANDOM_FLOAT*2.0f-1.0f);
	m_Position.y += ParentSystem->m_CreationDeviation.y*(RANDOM_FLOAT*2.0f-1.0f);
	m_Position.z += ParentSystem->m_CreationDeviation.z*(RANDOM_FLOAT*2.0f-1.0f);


	//发射的方向   发射方向的偏移  和发射的最小最大速率  来计算随机的速度
	//首先 确定方位   然后确定大小
	m_Velocity.x = ParentSystem->m_EmitDirection.x + ParentSystem->m_EmitDirectionDeviation.x*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity.y = ParentSystem->m_EmitDirection.y + ParentSystem->m_EmitDirectionDeviation.y*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity.z = ParentSystem->m_EmitDirection.z + ParentSystem->m_EmitDirectionDeviation.z*(RANDOM_FLOAT*2.0f-1.0f);
	m_Velocity = m_Velocity * ((ParentSystem->m_fMinEmitSpeed + 
		                         (ParentSystem->m_fMaxEmitSpeed - ParentSystem->m_fMinEmitSpeed)*RANDOM_FLOAT));
  

	//同理 这里用 方向*大小  但是没有偏移  没必要吧
	m_Acceleration = ParentSystem->m_AccelerationDirection* 
		              (ParentSystem->m_fMinAcceleration + 
					  (ParentSystem->m_fMaxAcceleration-ParentSystem->m_fMinAcceleration)*RANDOM_FLOAT);
 

	//发射颜色
	m_Color = ParentSystem->m_MinEmitColor + 
		   ((ParentSystem->m_MaxEmitColor-ParentSystem->m_MinEmitColor) * RANDOM_FLOAT);
	//首先我们计算出来死亡时候的颜色 （RGB）  除以生命 就是颜色变化率
	Vector3D EndColor = ParentSystem->m_MinDieColor + 
		   ((ParentSystem->m_MaxDieColor-ParentSystem->m_MinDieColor) * RANDOM_FLOAT);
	m_ColorChange = (EndColor-m_Color) / m_fDieAge;

	//同理颜色
	m_fAlpha = ParentSystem->m_fMinEmitAlpha 
		       + ((ParentSystem->m_fMaxEmitAlpha - ParentSystem->m_fMinEmitAlpha) * RANDOM_FLOAT);
	float fEndAlpha = ParentSystem->m_fMinDieAlpha 
		       + ((ParentSystem->m_fMaxDieAlpha - ParentSystem->m_fMinDieAlpha) * RANDOM_FLOAT);
	m_fAlphaChange = (fEndAlpha - m_fAlpha) / m_fDieAge;

   //同理颜色
	m_fSize = ParentSystem->m_fMinEmitSize +
			 ((ParentSystem->m_fMaxEmitSize - ParentSystem->m_fMinEmitSize) * RANDOM_FLOAT);
	float fEndSize = ParentSystem->m_fMinDieSize +
			 ((ParentSystem->m_fMaxDieSize - ParentSystem->m_fMinDieSize) * RANDOM_FLOAT);
	m_fSizeChange = (fEndSize - m_fSize) / m_fDieAge;

	//纹理旋转角度初始为0 我们用弧度计算
	m_fSpinAngle = 0.0f;
	//角速度  和   角加速度   a=a0+wt;
	m_fSpinSpeed = ParentSystem->m_fMinEmitSpinSpeed +
			((ParentSystem->m_fMaxEmitSpinSpeed - ParentSystem->m_fMinEmitSpinSpeed) * RANDOM_FLOAT);
	m_fSpinAcceleration = ParentSystem->m_fMinSpinAcceleration +
			((ParentSystem->m_fMaxSpinAcceleration - ParentSystem->m_fMinSpinAcceleration) * RANDOM_FLOAT);

	//好了  可以激活了   保留父类   重生时候继续使用
	m_bIsAlive = true;

	m_ParentSystem = ParentSystem;


}


void CParticle::Update(float timePassed)
{
	//首先的年龄长大了
	m_fAge += timePassed;

	//如果都要死了
	if (m_fAge >= m_fDieAge) 
	{
			m_fAge = 0.0f;
			m_bIsAlive = false;

			m_ParentSystem->m_iParticlesInUse--;
			return;
	}

	//改变所有应该改变的
	m_fAlpha += m_fAlphaChange*timePassed;
	m_Color = m_Color + m_ColorChange*timePassed;

	// V= V0+at
	m_Velocity = m_Velocity + m_Acceleration*timePassed;

	//s=vt+1/2a*t*t  应该用这个对吗  是的  但是相信我  这里用这个就够了  方便计算
	//这么大量的数据  性能重要嘛   s+=vt;  注意哦  我们可没有重载+=
	m_Position = m_Position + (m_Velocity*timePassed);


	//下面的应该注意下    记得你使用的是三角形纹理呢 还是点状纹理
	if(m_ParentSystem->m_bUseTexture==true)
	{
		//同样  v=v0+at    s=so+vt
		m_fSpinSpeed += m_fSpinAcceleration*timePassed;
		m_fSpinAngle  += m_fSpinSpeed*timePassed;

		//弧度转过一圈了 就从来吧 一直加也没意思 数更大
		if(m_fSpinAngle>2*GLT_PI)
			m_fSpinAngle=0.0f;
	}
	else 
		;
		m_fSize  += m_fSizeChange *timePassed;
}


void CParticle::Render()
{
	//点状纹理
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
						0,   //跨距
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
		
		//纹理坐标
		GLshort coord[4][2]={0,0,1,0,1,1,0,1};
		
		float  lengthOF=(float)(m_fSize *0.707);
		float  xOF=(float)cos(m_fSpinAngle);
		float  yOF=(float)sin(m_fSpinAngle);
		
		//旋转后的以点位中心的 四边形的位置
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
						0,   //跨距
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
	
//构造函数
CParticleSystem::CParticleSystem()
{
	m_EmitterPosition=NULL_VECTOR3D;
	m_CreationDeviation=NULL_VECTOR3D;  

	m_EmitDirection=NULL_VECTOR3D;
	m_EmitDirectionDeviation=NULL_VECTOR3D;
	
	/**********************************
	这两对重要参数将直接在类初始化
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
	//匀变速圆周运动
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

	//首先 每个粒子都是死的
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
		//更新那些活着的
		if (m_pParticles[i].m_bIsAlive)
		{
			m_pParticles[i].Update(timePassed);
		}
		//如果系统允许你可以重生
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
