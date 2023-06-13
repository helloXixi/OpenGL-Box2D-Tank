//
//  MySprite.h
//  Tank
//
//  Created by wxl on 12-3-27.
//  Copyright 2012 zy. All rights reserved.
//

#import <Foundation/Foundation.h>

struct MyColor4f {
	float r,g,b,a;
	
	MyColor4f() {}
	MyColor4f(float a,float b,float c,float d) : r(a), g(b) , b(c) , a(d) {}
	
};

@interface MySprite : NSObject {
	uint		textureImageID;
	
	CGSize		spriteSize;
	
	uint		textureStyle;   //100标识不用纹理绘制矩形
	
	float      *cycleVertexs;
	
	MyColor4f   cycleColor;
}

/**
 有纹理的矩形
 */
- (MySprite *)initWithFile:(NSString *)filePath rect:(CGSize)size texture:(uint)num;
/**
 无纹理的圆形
 */
- (MySprite *)initWithRadius:(float)radius andColor:(MyColor4f &)color;
/**
 无纹理的矩形
 */
- (MySprite *)initWithRect:(CGSize)size andColor:(MyColor4f &)color;
/**
 绘制
 */
- (void)drawWithCenter:(CGPoint)center angle:(float)ang;
/**
 绘制直线
 */
+ (void)drawWithPoint1:(CGPoint)point1 secondPoint:(CGPoint)point2 andColor:(MyColor4f &)color;


@end
