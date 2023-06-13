//
//  TankViewController.m
//  Tank
//
//  Created by wxl on 12-3-27.
//  Copyright 2012 zy. All rights reserved.
//

#import "TankViewController.h"
#import "EAGLView.h"


@implementation TankViewController

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	UIButton * btn=nil;
	
	btn =[UIButton buttonWithType:UIButtonTypeCustom];
	btn.frame=CGRectMake(39, 82, 79, 46);
	btn.tag=1;
	[btn addTarget:self action:@selector(btnAction:) forControlEvents:UIControlEventTouchUpInside];
	[btn setBackgroundImage:[UIImage imageNamed:@"leftBtn.png"] forState:UIControlStateNormal];
	[self.view addSubview:btn];
	
	btn =[UIButton buttonWithType:UIButtonTypeCustom];
	btn.frame=CGRectMake(148, 82, 49, 47);
	btn.tag=2;
	[btn addTarget:self action:@selector(btnAction:) forControlEvents:UIControlEventTouchUpInside];
	[btn setBackgroundImage:[UIImage imageNamed:@"midBtn.png"] forState:UIControlStateNormal];
	[self.view addSubview:btn];
	
	btn =[UIButton buttonWithType:UIButtonTypeCustom];
	btn.frame=CGRectMake(233, 82, 79, 46);
	btn.tag=3;
	[btn addTarget:self action:@selector(btnAction:) forControlEvents:UIControlEventTouchUpInside];
	[btn setBackgroundImage:[UIImage imageNamed:@"rightBtn.png"] forState:UIControlStateNormal];
	[self.view addSubview:btn];

}

- (void)btnAction:(id)sender
{
	int tag =((UIButton *)sender).tag;
	
	[(EAGLView *)self.view btnActionWithTag:tag];
}


// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	
	return UIInterfaceOrientationIsLandscape(interfaceOrientation);
	
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
	
    [super dealloc];
}

@end
