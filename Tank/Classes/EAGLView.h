//
//  EAGLView.h
//  Wxl
//
//  Created by wxl on 11-10-9.
//  Copyright 2011 hna. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "Box2D.h"
#import "MySprite.h"
#import "myContactListener.h"

#import "CParticles.h"

@interface EAGLView : UIView
{
@private
	// The pixel dimensions of the backbuffer
	GLint backingWidth;
	GLint backingHeight;
	
	EAGLContext *context;
	
	// OpenGL names for the renderbuffer and framebuffers used to render to this view
	GLuint viewRenderbuffer, viewFramebuffer;
	
	// OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist)
	GLuint depthRenderbuffer;
	
	CADisplayLink  * displayLink;
	NSInteger animationFrameInterval;
	
	b2World        * myWorld;
	
	/*------------------------------*/
	b2Body		   * frontWheelBody;
	MySprite       * frontWheelSp;
	MySprite       * frontWheelTex;
	
	b2Body         * upWheelBody;
	b2Body         * lenWheelBody;
	MySprite       * upWheelSp;
	MySprite       * upWheelTex;
	MySprite       * lenWheelSp;
	b2RevoluteJoint* upAndLenJoint;
	
	b2Body         * backWheelBody;
	MySprite       * backWheelSp;
	MySprite       * backWheelTex;
	
	b2Body         * midWheelBody;
	MySprite       * midWheelSp;
	
	b2Body         * bombBody;
	/*------------------------------*/
	
	myContactListener * myListener;
	
	/*--轨迹粒子效果-------------------------*/
	GLuint            textureID[1];
	CParticleSystem * g_ParticleSystem1;
	CFAbsoluteTime    systemTimeAbsolute;
}

- (void)btnActionWithTag:(int)flag;

-(void)startAnimation;
-(void)stopAnimation;
@end
