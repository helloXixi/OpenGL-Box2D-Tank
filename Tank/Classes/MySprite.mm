//
//  MySprite.m
//  Tank
//
//  Created by wxl on 12-3-27.
//  Copyright 2012 zy. All rights reserved.
//

#import "MySprite.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

//360度圆划分为 36份
#define angleDivide  (10)
#define angleNum     (360/angleDivide)

@implementation MySprite


//方块4个顶点
float   vertexs[4][2]=
{
	0.0,0.0,
	0.0,0.0,
	0.0,0.0,
	0.0,0.0,
};

//分别的纹理坐标
float   coords[5][8]=
{
	//rope
	0.0, 1.0,
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	//A
	0.0, 1.0,
	0.0, 0.5,
	0.5, 0.5,
	0.5, 1.0,
	//B
	0.5, 1.0,
	0.5, 0.5,
	1.0, 0.5,
	1.0, 1.0,
	//C
	0.0, 0.5,
	0.0, 0.0,
	0.5, 0.0,
	0.5, 0.5,
	//D
	0.5, 0.5,
	0.5, 0.0,
	1.0, 0.0,
	1.0, 0.5,
};

- (MySprite *)initWithFile:(NSString *)filePath rect:(CGSize)size texture:(uint)num
{
	if ((self = [super init]))
	{
		cycleVertexs=NULL;
		spriteSize=size;
		textureStyle=num;
		
		CGImageRef		brushImage = [UIImage imageNamed:filePath].CGImage;
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
			
			glGenTextures(1, &textureImageID);
			glBindTexture(GL_TEXTURE_2D, textureImageID);
			
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, brushData);
			free(brushData);
		}
		else
			NSLog(@"b2Sprite init brushImage error!");

	}
	return self;
}


- (void)drawWithCenter:(CGPoint)center angle:(float)ang
{
	float x		 =center.x;
	float y		 =center.y;
	
	//矩形
	if(cycleVertexs == NULL)
	{
		//不使用纹理
		if(textureStyle == 100)
		{
			float width  =spriteSize.width/2.0;
			float height =spriteSize.height/2.0;
			
			vertexs[0][0] = -width;   vertexs[0][1] = height;
			vertexs[1][0] = -width;   vertexs[1][1] = -height;
			vertexs[2][0] = width;    vertexs[2][1] = -height;
			vertexs[3][0] = width;    vertexs[3][1] = height;
			
			glVertexPointer(2,GL_FLOAT,0,vertexs);
			glColor4f(cycleColor.r, cycleColor.g, cycleColor.b, cycleColor.a);
			glPushMatrix();
			glTranslatef(x, y, 0);
			glRotatef(ang, 0.0, 0.0, 1.0);
			glDrawArrays(GL_TRIANGLE_FAN,0,4);
			glPopMatrix();
		}
		else 
		{
			glBindTexture(GL_TEXTURE_2D, textureImageID);
			
			float width  =spriteSize.width/2.0;
			float height =spriteSize.height/2.0;
			
			vertexs[0][0] = -width;   vertexs[0][1] = height;
			vertexs[1][0] = -width;   vertexs[1][1] = -height;
			vertexs[2][0] = width;    vertexs[2][1] = -height;
			vertexs[3][0] = width;    vertexs[3][1] = height;
			
			glVertexPointer(2,GL_FLOAT,0,vertexs);
			glTexCoordPointer(2, GL_FLOAT, 0, &coords[textureStyle][0]);
			glPushMatrix();
			glTranslatef(x, y, 0);
			glRotatef(ang, 0.0, 0.0, 1.0);
			glDrawArrays(GL_TRIANGLE_FAN,0,4);
			glPopMatrix();
		}
	}
	//圆形
	else 
	{
		glPushMatrix();
		
		glVertexPointer(2,GL_FLOAT,0,cycleVertexs);
		glTranslatef(x, y, 0);
		glRotatef(ang, 0.0, 0.0, 1.0);

		glColor4f(cycleColor.r, cycleColor.g, cycleColor.b, cycleColor.a);
		glDrawArrays(GL_TRIANGLE_FAN,0,angleNum);
		
		glPopMatrix();
	}
}

+ (void)drawWithPoint1:(CGPoint)point1 secondPoint:(CGPoint)point2 andColor:(MyColor4f &)color
{
	float lineVertexs[2][2];
	
	lineVertexs[0][0] = point1.x;
	lineVertexs[0][1] = point1.y;
	lineVertexs[1][0] = point2.x;
	lineVertexs[1][1] = point2.y;
	
	glColor4f(color.r, color.g, color.b, color.a);
	glVertexPointer(2,GL_FLOAT,0,lineVertexs);
	glDrawArrays(GL_LINES,0,2);
}

- (MySprite *)initWithRadius:(float)radius andColor:(MyColor4f &)color
{
	if ((self = [super init]))
	{
		cycleColor = color;
		cycleVertexs =  new float[angleNum*2];
		
		if(cycleVertexs)
		{		
			for(int i=0;i<angleNum;i++)
			{
				float x=radius*cos(i*angleDivide/180.0*M_PI);
				float y=radius*sin(i*angleDivide/180.0*M_PI);
				
				*(cycleVertexs+i*2+0)=x;
				*(cycleVertexs+i*2+1)=y;
			}
		}
		else
		{
			NSLog(@"angleNum err!");
			return nil;
		}
	}
	return self;
}


- (MySprite *)initWithRect:(CGSize)size andColor:(MyColor4f &)color
{
	if ((self = [super init]))
	{
		cycleVertexs=NULL;
		spriteSize=size;
		textureStyle=100;
		cycleColor = color;
	}
	return self;
}


- (void)dealloc
{
	glDeleteTextures(1, &textureImageID);
	[super dealloc];
}

@end
