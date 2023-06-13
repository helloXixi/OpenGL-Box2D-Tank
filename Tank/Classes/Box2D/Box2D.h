/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BOX2D_H
#define BOX2D_H

/**
\mainpage Box2D API Documentation

\section intro_sec Getting Started

For documentation please see http://box2d.org/documentation.html

For discussion please visit http://box2d.org/forum
*/

// These include files constitute the main Box2D API

#import "b2Settings.h"
#import "b2Draw.h"
#import "b2Timer.h"

#import "b2CircleShape.h"
#import "b2EdgeShape.h"
#import "b2ChainShape.h"
#import "b2PolygonShape.h"

#import "b2BroadPhase.h"
#import "b2Distance.h"
#import "b2DynamicTree.h"
#import "b2TimeOfImpact.h"

#import "b2Body.h"
#import "b2Fixture.h"
#import "b2WorldCallbacks.h"
#import "b2TimeStep.h"
#import "b2World.h"

#import "b2Contact.h"

#import "b2DistanceJoint.h"
#import "b2FrictionJoint.h"
#import "b2GearJoint.h"
#import "b2WheelJoint.h"
#import "b2MouseJoint.h"
#import "b2PrismaticJoint.h"
#import "b2PulleyJoint.h"
#import "b2RevoluteJoint.h"
#import "b2RopeJoint.h"
#import "b2WeldJoint.h"

#endif
