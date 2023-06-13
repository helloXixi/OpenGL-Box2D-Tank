/*
 *  myContactListener.cpp
 *  Ball
 *
 *  Created by wxl on 12-2-14.
 *  Copyright 2012 zy. All rights reserved.
 *
 */

#include "myContactListener.h"
#include "stdio.h"

void myContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	b2Fixture * fixtureA=contact->GetFixtureA();
	b2Fixture * fixtureB=contact->GetFixtureB();
	
	const char * str1=(char *)fixtureB->GetUserData();
	const char * str2=(char *)fixtureA->GetUserData();
	
	if(str1 && str2 && strcmp(str1, "bomb") == 0  && strcmp(str2, "bottomGround") == 0)
	{
		boolContactBombGround=true;
	}	
}

void myContactListener::BeginContact(b2Contact* contact)
{
}


void myContactListener::EndContact(b2Contact* contact)
{
}
