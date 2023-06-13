/*
 *  myContactListener.h
 *  Ball
 *
 *  Created by wxl on 12-2-14.
 *  Copyright 2012 zy. All rights reserved.
 *
 */

#ifndef myContactListener_H
#define myContactListener_H

#import "Box2D.h"

class myContactListener:public b2ContactListener
{
public:
	myContactListener():b2ContactListener()
	{
		boolContactBombGround=false;
	}
	~myContactListener()
	{
	}
	
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	
	void BeginContact(b2Contact* contact);
		
	void EndContact(b2Contact* contact);
	
	bool boolContactBombGround;
	
};

#endif
