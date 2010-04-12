//
//  glvAppDelegate.h
//  glv
//
//  Created by Graham Wakefield on 4/12/10.
//  Copyright UCSB 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EAGLView;

@interface glvAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    EAGLView *glView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;

@end

