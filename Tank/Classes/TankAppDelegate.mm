//
//  TankAppDelegate.m
//  Tank
//
//  Created by wxl on 12-3-27.
//  Copyright 2012 zy. All rights reserved.
//

#import "TankAppDelegate.h"
#import "TankViewController.h"
#import "EAGLView.h"

@implementation TankAppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window.rootViewController = self.viewController;
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [(EAGLView *)self.viewController.view stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [(EAGLView *)self.viewController.view startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	[(EAGLView *)self.viewController.view stopAnimation];
}

- (void)dealloc
{
    [viewController release];
    [window release];
    
    [super dealloc];
}

@end
