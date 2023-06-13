



//
//  EAGLView.m
//  Wxl
//
//  Created by wxl on 11-10-9.
//  Copyright 2011 hna. All rights reserved.
//

#import "EAGLView.h"

#include <vector>
using namespace std;

vector <b2Body   *> blockVector;
vector <b2Body   *> ropeVector;

#define DEGREES_TO_RADIANS(__ANGLE) ((__ANGLE) / 180.0 * M_PI)
#define RADIANS_TO_DEGREES(__ANGLE) ((__ANGLE) * 180.0 / M_PI)

#define USE_DEPTH_TEST  0

#define wheelRadius   20
#define wheelWidth    50
#define wheelHeight   20
#define UpRadius      14
#define LenWidth      50
#define LenHeight     10

#define BlockSize     16

#define RopeWidth     10
#define RopeHeight     8

#define BombSize      6.0

/*
 896 对应 10 整个屏幕的 box2D 坐标为 414*10/896 = 4.62
 
 (0,4.62)  (10,4.62)
 ---------|
 |        |
 |        |
 |--------|
 (0,0)    (10,0)
 
*/

#define Ratio4WH ([UIScreen mainScreen].bounds.size.height*10.0/[UIScreen mainScreen].bounds.size.width)
#define Box2DToScreen(__x__)   ([UIScreen mainScreen].bounds.size.width/10.0*(__x__))
#define ScreenToBox2D(__x__)   ((__x__)*10.0/[UIScreen mainScreen].bounds.size.width)

#define SAFE_DELETE_POINTER(p)			{if(p) delete p   ;p=NULL;}
#define SAFE_DELETE_POINTER_ARRAY(p)	{if(p) delete p [];p=NULL;}

@interface  EAGLView(PrivateMethods)
- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;
- (void)drawView;
@end

@implementation EAGLView

#pragma mark ------
#pragma mark Box2D

/**
 创建坦克
 */

- (void)CreateTank
{
    MyColor4f color=MyColor4f(1.0,0.5,0.0,0.6);
	frontWheelSp=[[MySprite alloc] initWithRadius:wheelRadius andColor:color];
	frontWheelTex=[[MySprite alloc] initWithFile:@"blocks.png" rect:CGSizeMake(wheelRadius, wheelRadius) texture:1];
	
	color=MyColor4f(0.0,0.5,1.0,0.6);
	backWheelSp=[[MySprite alloc] initWithRadius:wheelRadius andColor:color];
	backWheelTex=[[MySprite alloc]  initWithFile:@"blocks.png" rect:CGSizeMake(wheelRadius, wheelRadius) texture:2];
	
	color=MyColor4f(0.5,0.5,0.5,0.6);
	midWheelSp=[[MySprite alloc] initWithRect:CGSizeMake(wheelWidth,wheelHeight) andColor:color];
	
	color=MyColor4f(1.0,0.0,0.5,0.6);
	upWheelSp=[[MySprite alloc] initWithRadius:UpRadius andColor:color];
	upWheelTex=[[MySprite alloc] initWithFile:@"blocks.png" rect:CGSizeMake(UpRadius+2, UpRadius+2) texture:0];
	
	color=MyColor4f(0.0,1.0,1.0,0.6);
	lenWheelSp=[[MySprite alloc] initWithRect:CGSizeMake(LenWidth,LenHeight) andColor:color];
}

/**
 构建世界 物体
 */
- (void)configMyBox2d
{
	/*--世界--*/
	b2Vec2 worldDef=b2Vec2(0.0,-10.0);
	myWorld=new b2World(worldDef);
	myWorld->SetAllowSleeping(true);
	/*--世界--*/
	
	/*---四边包围盒-----*/
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0,0);
	b2Body *groundBody = myWorld->CreateBody(&groundBodyDef);
	
	b2EdgeShape groundBox;
	b2FixtureDef boxShapeDef;
	boxShapeDef.friction=1.0;
	boxShapeDef.shape = &groundBox;
	
	groundBox.Set(b2Vec2(0,0), b2Vec2(10, 0));
	boxShapeDef.userData=(void *)"bottomGround";
	groundBody->CreateFixture(&boxShapeDef);
	
	groundBox.Set(b2Vec2(0,0), b2Vec2(0, Ratio4WH));
	boxShapeDef.userData=(void *)"leftGround";
	groundBody->CreateFixture(&boxShapeDef);
	
	groundBox.Set(b2Vec2(0, Ratio4WH),b2Vec2(10, Ratio4WH));
	boxShapeDef.userData=(void *)"topGround";
	groundBody->CreateFixture(&boxShapeDef);
	
	groundBox.Set(b2Vec2(10, 0), b2Vec2(10, Ratio4WH));
	boxShapeDef.userData=(void *)"rightGround";
	groundBody->CreateFixture(&boxShapeDef);
	/*---四边包围盒-----*/
	
	/*---坦克-----*/
	//前轮
	b2BodyDef tankBodyDef;
	tankBodyDef.type=b2_dynamicBody;
	tankBodyDef.linearDamping=0.2;
	tankBodyDef.position.Set(5.0,ScreenToBox2D(wheelRadius));  //初始前轮位于中间
	frontWheelBody=myWorld->CreateBody(&tankBodyDef);
	
	b2CircleShape wheelShape;
	wheelShape.m_radius=ScreenToBox2D(wheelRadius);
	
	b2FixtureDef wheelFixtureDef;
	wheelFixtureDef.shape=&wheelShape;
	wheelFixtureDef.density=6.0;
	wheelFixtureDef.restitution=0.0;
	wheelFixtureDef.friction = 0.5;
	frontWheelBody->CreateFixture(&wheelFixtureDef);
	//后轮
	tankBodyDef.position.Set(5.0+ScreenToBox2D(wheelWidth),ScreenToBox2D(wheelRadius));  
	backWheelBody=myWorld->CreateBody(&tankBodyDef);
	backWheelBody->CreateFixture(&wheelFixtureDef);
	//中间夹的矩形
	tankBodyDef.position.Set(5.0+ScreenToBox2D(wheelWidth/2.0),ScreenToBox2D(wheelHeight));
	midWheelBody=myWorld->CreateBody(&tankBodyDef);
	
	b2PolygonShape midShape;
	midShape.SetAsBox(ScreenToBox2D(wheelWidth/2.0), ScreenToBox2D(wheelHeight/2.0));
	
	wheelFixtureDef.shape=&midShape;
	midWheelBody->CreateFixture(&wheelFixtureDef);
	//炮塔
	tankBodyDef.position.Set(5.0+ScreenToBox2D(wheelWidth/2.0),ScreenToBox2D(wheelHeight+UpRadius*2));
	upWheelBody=myWorld->CreateBody(&tankBodyDef);
	
	wheelShape.m_radius=ScreenToBox2D(UpRadius);
	wheelFixtureDef.shape=&wheelShape;
	upWheelBody->CreateFixture(&wheelFixtureDef);
	//炮管
	tankBodyDef.position.Set(5.0+ScreenToBox2D(wheelWidth/2.0+LenWidth/2.0),ScreenToBox2D(wheelHeight+UpRadius*2));
	lenWheelBody=myWorld->CreateBody(&tankBodyDef);
	
	midShape.SetAsBox(ScreenToBox2D(LenWidth/2.0), ScreenToBox2D(LenHeight/2.0));
	
	wheelFixtureDef.shape=&midShape;
	lenWheelBody->CreateFixture(&wheelFixtureDef);
	
	/**
	 关节
	 */
	b2RevoluteJointDef jointDef1;
	
	//炮塔和炮管 焊接
	b2WeldJointDef upLenDef;
	upLenDef.Initialize(lenWheelBody, upWheelBody, upWheelBody->GetWorldCenter());
	myWorld->CreateJoint(&upLenDef);
	
	//炮塔和基座
	jointDef1.Initialize(midWheelBody, upWheelBody, upWheelBody->GetWorldCenter());
	myWorld->CreateJoint(&jointDef1);
	
	//前轮与基座
	jointDef1.Initialize(midWheelBody, frontWheelBody, frontWheelBody->GetWorldCenter());
	myWorld->CreateJoint(&jointDef1);
	
	//后轮与基座
	jointDef1.Initialize(midWheelBody, backWheelBody, backWheelBody->GetWorldCenter());
	myWorld->CreateJoint(&jointDef1);
	
	//炮管和基座 旋转炮塔
	jointDef1.motorSpeed = 0.0f;
	jointDef1.maxMotorTorque = 1000.f;
	jointDef1.lowerAngle= - b2_pi;
	jointDef1.upperAngle = 0.0;
	jointDef1.enableMotor = true;
	jointDef1.enableLimit= true;
	jointDef1.Initialize(lenWheelBody, midWheelBody, upWheelBody->GetWorldCenter()); //饶炮塔旋转
	upAndLenJoint=(b2RevoluteJoint *)myWorld->CreateJoint(&jointDef1);
	
	
	[self CreateTank];
	/*---坦克-----*/
	
	/*----创建方块-------*/
	for(int i=0;i<20;i++)
	{
		int textureNum = rand()%4+1;
		MySprite * blocksSP = [[MySprite alloc] initWithFile:@"blocks.png" rect:CGSizeMake(BlockSize, BlockSize) texture:textureNum];
		
		tankBodyDef.type=b2_dynamicBody;
		tankBodyDef.userData=blocksSP;
		tankBodyDef.position.Set(2.0,ScreenToBox2D(BlockSize)*i);
		
		b2Body * blockBody = myWorld->CreateBody(&tankBodyDef);
		
		midShape.SetAsBox(ScreenToBox2D(BlockSize/2.0), ScreenToBox2D(BlockSize/2.0));
		wheelFixtureDef.shape=&midShape;
		wheelFixtureDef.density=2.0;
		blockBody->CreateFixture(&wheelFixtureDef);
		
		blockVector.push_back(blockBody);
	}
	/*----创建方块-------*/
    
	//增加监听
	myListener=new myContactListener;
	myWorld->SetContactListener(myListener);
}


#pragma mark ------
#pragma mark Draw 

- (void)loadTexture
{
	glGenTextures(1, textureID);
	CGImageRef		brushImage = [UIImage imageNamed:@"shape1.png"].CGImage;
	
	if(brushImage) 
	{
		size_t          width  = CGImageGetWidth(brushImage);
		size_t          height = CGImageGetHeight(brushImage);
		GLubyte			*brushData = (GLubyte *) calloc(width * height * 4, sizeof(GLubyte));
		CGContextRef	brushContext = CGBitmapContextCreate(brushData, width, height, 8, width * 4, CGImageGetColorSpace(brushImage), kCGImageAlphaPremultipliedLast);
		//翻转画布
		CGContextTranslateCTM(brushContext, 0, height);
		CGContextScaleCTM(brushContext, 1.0, -1.0);
		CGContextDrawImage(brushContext, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), brushImage);
		CGContextRelease(brushContext);
		
		glBindTexture(GL_TEXTURE_2D, textureID[0]);
		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, brushData);
		free(brushData);
	}
	else
		NSLog(@"b2Sprite init brushImage error!");	
	
}

//初始化opengles
- (void)initAllStatus
{
	glClearColor(0.2,0.2,0.2,1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	g_ParticleSystem1=nil;
	blockVector.clear();
	ropeVector.clear();
	
	[self loadTexture];
}


//创建正视图
- (void)setupView
{
	glViewport(0,0,backingWidth,backingHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrthof(0, backingWidth, 0 , backingHeight, -1, 1);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/**
 绘制边界
 */
- (void)drawEdges
{
	glLineWidth(8);
	
	MyColor4f color=MyColor4f(1.0,0.0,0.0,0.5);
	[MySprite drawWithPoint1:CGPointMake(0,0) 
				 secondPoint:CGPointMake(0, backingHeight) 
					andColor:color];
	
	color=MyColor4f(0.0,1.0,0.0,0.5);
	[MySprite drawWithPoint1:CGPointMake(0,0) 
				 secondPoint:CGPointMake(backingWidth, 0)  
					andColor:color];
	color=MyColor4f(0.0,0.0,1.0,0.5);
	[MySprite drawWithPoint1:CGPointMake(0,backingHeight) 
				 secondPoint:CGPointMake(backingWidth, backingHeight) 
					andColor:color];
	color=MyColor4f(1.0,1.0,0.0,0.5);
	[MySprite drawWithPoint1:CGPointMake(backingWidth,0) 
				 secondPoint:CGPointMake(backingWidth, backingHeight) 
					andColor:color];
	
	glLineWidth(3);
}


/**
 创建粒子效果图
 */
- (void)createColorful:(CGPoint)loc
{
	if(!g_ParticleSystem1)
	{
		/*-------------*/
		g_ParticleSystem1 = new CParticleSystem();
		g_ParticleSystem1->Initialize(50);
		g_ParticleSystem1->m_bRecreateWhenDied = true;
		g_ParticleSystem1->m_fMinDieAge   = 2.0f;
		g_ParticleSystem1->m_fMaxDieAge   = 0.5f;
		g_ParticleSystem1->SetCreationDieColor(Vector3D(1.0f,0.0f,0.0f),Vector3D(1.0f,0.5f,0.0f),
											   Vector3D(1.0f,1.0f,1.0f),Vector3D(1.0f,1.0f,1.0f));
		
		g_ParticleSystem1->SetAlphaValues(1.0f,1.0f,0.0f,0.0f);
		g_ParticleSystem1->SetAcceleration(Vector3D(0.0f,-5.0f,0.0f),1.3f,0.4f);
		g_ParticleSystem1->m_fMaxEmitSpeed = 8.0f;
		g_ParticleSystem1->m_fMinEmitSpeed = 2.0f;
		g_ParticleSystem1->SetEmissionDirection(Vector3D(0.0f,-1.0f,0.0f),Vector3D(2.0,2.0,0.0));
		g_ParticleSystem1->SetSpinSpeed(-0.82*GLT_PI,0.82*GLT_PI);
		g_ParticleSystem1->SetSizeValues(40,20,16,8);
		
		/*--------------------*/
		
		g_ParticleSystem1->SetEmitter(Vector3D(-loc.x,-loc.y,0.0),Vector3D(0.0,0.0,0.0));
		g_ParticleSystem1->ActiveAllParticlesAfterInit();
	}
}


- (void)updateSence
{
	//模拟
	myWorld->Step(1.0f/60.0f, 6, 2);
	
	//bomb落地
	if(myListener->boolContactBombGround)
	{
		delete g_ParticleSystem1,g_ParticleSystem1=nil;
		
		if(bombBody)
		{
			myWorld->DestroyBody(bombBody);
			bombBody=nil;
		}
		myListener->boolContactBombGround=false;
		
	}
	
	/**
	 绘制带颜色的
	 */
	//边界
	[self drawEdges];
	
	b2Vec2 loc = frontWheelBody->GetPosition();
	[frontWheelSp drawWithCenter:CGPointMake(Box2DToScreen(loc.x), Box2DToScreen(loc.y)) 
						   angle:RADIANS_TO_DEGREES(frontWheelBody->GetAngle())];

	loc = midWheelBody->GetPosition();
	[midWheelSp drawWithCenter:CGPointMake(Box2DToScreen(loc.x), Box2DToScreen(loc.y))
						 angle:RADIANS_TO_DEGREES(midWheelBody->GetAngle())];

	loc = backWheelBody->GetPosition();
	[backWheelSp drawWithCenter:CGPointMake(Box2DToScreen(loc.x), Box2DToScreen(loc.y)) 
						   angle:RADIANS_TO_DEGREES(backWheelBody->GetAngle())];

	loc = upWheelBody->GetPosition();
	[upWheelSp drawWithCenter:CGPointMake(Box2DToScreen(loc.x), Box2DToScreen(loc.y)) 
						  angle:RADIANS_TO_DEGREES(upWheelBody->GetAngle())];
	
	loc = lenWheelBody->GetPosition();
	[lenWheelSp drawWithCenter:CGPointMake(Box2DToScreen(loc.x), Box2DToScreen(loc.y)) 
						angle:RADIANS_TO_DEGREES(lenWheelBody->GetAngle())];
	//bomb
	if(bombBody)
	{
		b2Vec2 loc = bombBody->GetPosition();
		loc.x=Box2DToScreen(loc.x);
		loc.y=Box2DToScreen(loc.y);
	
		//更新轨迹
		g_ParticleSystem1->SetEmitter(Vector3D(-loc.x,-loc.y,0.0),Vector3D(0.0,0.0,0.0));
		
		CFAbsoluteTime currentTime=CFAbsoluteTimeGetCurrent();
		g_ParticleSystem1->UpdateSystem(currentTime-systemTimeAbsolute);
		systemTimeAbsolute=currentTime;
	}
	
	/**
	 绘制带纹理的
	 */
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	loc = frontWheelBody->GetPosition();
	[frontWheelTex drawWithCenter:CGPointMake(Box2DToScreen(loc.x), Box2DToScreen(loc.y)) 
							angle:RADIANS_TO_DEGREES(frontWheelBody->GetAngle())];
	loc = backWheelBody->GetPosition();
	[backWheelTex drawWithCenter:CGPointMake(Box2DToScreen(loc.x), Box2DToScreen(loc.y)) 
						   angle:RADIANS_TO_DEGREES(frontWheelBody->GetAngle())];
	
	loc = upWheelBody->GetPosition();
	[upWheelTex drawWithCenter:CGPointMake(Box2DToScreen(loc.x), Box2DToScreen(loc.y)) 
						 angle:RADIANS_TO_DEGREES(upWheelBody->GetAngle())];
	
	//绘制方块
	for (int a=0; a < blockVector.size(); a++)
	{
		b2Body  * bodys = (b2Body *)blockVector[a];
		MySprite * tmp=(MySprite *)bodys->GetUserData();

		[tmp drawWithCenter:CGPointMake(Box2DToScreen(bodys->GetPosition().x),
										Box2DToScreen(bodys->GetPosition().y))
					  angle:RADIANS_TO_DEGREES(bodys->GetAngle())];
	}
	
	//绘制炮弹路径
	if(g_ParticleSystem1)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnableClientState(GL_COLOR_ARRAY);
		glBindTexture(GL_TEXTURE_2D, textureID[0]);
		if(g_ParticleSystem1->m_iParticlesInUse>0)
			g_ParticleSystem1->Render();
		glDisableClientState(GL_COLOR_ARRAY);
		glDisable(GL_BLEND);
	}
	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


//绘制
- (void)drawView
{	
	[EAGLContext setCurrentContext:context];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	
	[self updateSence];
	
	// Display the buffer
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

#pragma mark ------
#pragma mark UserInput

- (void)btnActionWithTag:(int)flag
{
	switch (flag)
	{
		case 1:
		{
			b2Vec2 tmp = frontWheelBody->GetWorldCenter()-midWheelBody->GetWorldCenter();
			tmp*=20.0;
			midWheelBody->ApplyLinearImpulse(tmp,midWheelBody->GetWorldCenter());

		}
		break;
		case 2:
		{			
			if(lenWheelBody -> GetAngle() < b2_pi*0.5)
			{
				upAndLenJoint ->SetMotorSpeed(-2);
			}
			else if(lenWheelBody ->GetAngle() > b2_pi*0.5)
			{
				upAndLenJoint ->SetMotorSpeed(2);
			}
		}
		break;
		case 3:
		{
			b2Vec2 tmp = backWheelBody->GetWorldCenter()-midWheelBody->GetWorldCenter();
			tmp*=20.0;
			midWheelBody->ApplyLinearImpulse(tmp,midWheelBody->GetWorldCenter());
			
		}
		break;
		default:
		break;
	}
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	
	if(bombBody)
	{
		delete g_ParticleSystem1,g_ParticleSystem1=nil;
		
		MySprite * sp = (MySprite *)bombBody->GetUserData();
		[sp release],sp=nil;
		
		myWorld->DestroyBody(bombBody);
		bombBody=nil;
		
		myListener->boolContactBombGround=false;
	}
	
	UITouch*	touch    = [[event touchesForView:self] anyObject];
	CGPoint     location = [touch locationInView:self];
	location.y           = backingHeight-location.y;
	
	float32 angleOfTank = lenWheelBody->GetAngle();
	b2Vec2  lenWheelLoc = lenWheelBody->GetWorldCenter();
	b2Vec2  speedV = b2Vec2(location.x,location.y);
	float32 speedF = speedV.Length();
	
	b2BodyDef bombDef;
	bombDef.type=b2_dynamicBody;
	bombDef.userData=nil;
	bombDef.bullet=true;//发射的是快速的子弹
	bombDef.position.Set(lenWheelLoc.x,lenWheelLoc.y);
	bombDef.angle=angleOfTank;
	bombBody=myWorld->CreateBody(&bombDef);
	
	b2CircleShape bombShape;
	bombShape.m_radius=ScreenToBox2D(BombSize);
	
	b2FixtureDef bombFixtureDef;
	bombFixtureDef.shape=&bombShape;
	bombFixtureDef.density=1.0;
	bombFixtureDef.restitution=0.3;
	bombFixtureDef.friction = 0.2;
	bombFixtureDef.userData=(void *)"bomb";
	bombBody->CreateFixture(&bombFixtureDef);
	
	b2Vec2 tmp = b2Vec2(speedF*cosf(angleOfTank),speedF*sinf(angleOfTank));
	bombBody->ApplyLinearImpulse(tmp,bombBody->GetWorldCenter());
	//固定炮塔方向
	upAndLenJoint ->SetMotorSpeed(0);
	//反作用力
	tmp*=0.6;
	midWheelBody->ApplyForceToCenter(-tmp);
	//创建粒子
	[self createColorful:CGPointMake(Box2DToScreen(lenWheelLoc.x), Box2DToScreen(lenWheelLoc.y))];
	
}

#pragma mark ------
#pragma mark LifeCycle

// Implement this to override the default layer class (which is [CALayer class]).
// We do this so that our view will be backed by a layer that is capable of OpenGL ES rendering.
+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

// The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder *)coder 
{
	
	if ((self = [super initWithCoder:coder]))
	{
		CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
		
		eaglLayer.opaque = YES;
		// In this application, we want to retain the EAGLDrawable contents after a call to presentRenderbuffer.
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		
		if (!context || ![EAGLContext setCurrentContext:context]) 
		{
			[self release];
			return nil;
		}
		
		animationFrameInterval=1;
		systemTimeAbsolute=CFAbsoluteTimeGetCurrent();
		
		[self initAllStatus];
	}
	
	return self;
}


-(void)startAnimation
{
	displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView)];
	[displayLink setFrameInterval:animationFrameInterval];
	[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void)stopAnimation
{
	[displayLink invalidate];
	displayLink=nil;
}


// If our view is resized, we'll be asked to layout subviews.
// This is the perfect opportunity to also update the framebuffer so that it is
// the same size as our display area.
-(void)layoutSubviews
{
	[EAGLContext setCurrentContext:context];
	[self destroyFramebuffer];
	[self createFramebuffer];
	[self setupView];
	[self configMyBox2d];
}

- (BOOL)createFramebuffer
{
	// Generate IDs for a framebuffer object and a color renderbuffer
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	// This call associates the storage for the current render buffer with the EAGLDrawable (our CAEAGLLayer)
	// allowing us to draw into a buffer that will later be rendered to screen wherever the layer is (which corresponds with our view).
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	if(USE_DEPTH_TEST)
	{
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	}
	
	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
	return YES;
}

// Clean up any buffers we have allocated.
- (void)destroyFramebuffer
{
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;
	
	if(depthRenderbuffer)
	{
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
}

// Releases resources when they are not longer needed.
- (void) dealloc
{	
	[self stopAnimation];
	
	SAFE_DELETE_POINTER(myWorld);
	SAFE_DELETE_POINTER(myListener);
	
	if(g_ParticleSystem1)
		delete g_ParticleSystem1,g_ParticleSystem1=nil;
	glDeleteTextures(1, textureID);
	
	for(int i=0;i<blockVector.size();i++)
	{
		b2Body   * bod = (b2Body *)blockVector[i];
		MySprite * sp  = (MySprite *)bod->GetUserData();
		[sp release];
	}
	
	blockVector.clear();
	ropeVector.clear();
	
	[frontWheelSp release];
	[frontWheelTex release];
	[upWheelSp release];
	[upWheelTex release];
	[lenWheelSp release];
	[backWheelSp release];
	[backWheelTex release];
	[midWheelSp release];
	
	if([EAGLContext currentContext] == context)
	{
		[EAGLContext setCurrentContext:nil];
	}
	
	[context release];
	[super dealloc];
}

@end
