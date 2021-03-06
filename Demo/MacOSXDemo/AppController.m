//
//  AppController.m
//  MacOSXDemo
//
//  Created by 潘李亮 on 10-5-5.
//  Copyright __MyCompanyName__ 2010 . All rights reserved.
//

#import "AppController.h"

@implementation AppController
 
- (void) awakeFromNib
{
	if(![qcView loadCompositionFromFile:[[NSBundle mainBundle] pathForResource:@"Introduction" ofType:@"qtz"]]) {
		NSLog(@"Could not load composition");
	}
}

- (void)windowWillClose:(NSNotification *)notification 
{
	[NSApp terminate:self];
}


// An example of programmatically setting the value of input ports
// Notice that the QCPatchParameterView is automatically updated by setting the QCView inputs
- (IBAction) changeColorToBlue:(id)sender
{
	// Colors
	[qcView setValue:[NSColor blueColor] forInputKey:@"Top_Color"];
	[qcView setValue:[NSColor cyanColor] forInputKey:@"Middle_Color"];
	[qcView setValue:[NSColor blueColor] forInputKey:@"Bottom_Color"];
	
	// Numbers
	[qcView setValue:[NSNumber numberWithDouble:.5] forInputKey:@"Font_Size"];

	// Strings
	[qcView setValue:@"Hello World" forInputKey:@"String"];

}


@end
