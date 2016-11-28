//
//  ViewController.h
//  capstone
//
//  Created by Judy Chang on 11/27/16.
//  Copyright © 2016 Judy Chang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <AVKit/AVKit.h>
#import <CoreMedia/CoreMedia.h>

@interface ViewController : UIViewController
{
    AVAudioRecorder *recorder;
    AVPlayer *player;
}

@property (strong, nonatomic) IBOutlet UIButton* btnPlay;
- (IBAction)btnPlay:(id)sender;
- (IBAction)btnTakePhoto:(id)sender;

@end

