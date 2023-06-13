//
//  TankAppDelegate.h
//  Tank
//
//  Created by wxl on 12-3-27.
//  Copyright 2012 zy. All rights reserved.
//

#import <UIKit/UIKit.h>

@class TankViewController;

@interface TankAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    TankViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet TankViewController *viewController;

@end

