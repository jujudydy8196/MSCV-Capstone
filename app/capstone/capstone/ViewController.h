//
//  ViewController.h
//  capstone
//
//  Created by Judy Chang on 11/27/16.
//  Copyright © 2016 Judy Chang. All rights reserved.
//
#ifdef __cplusplus
//#import "opencv2/videoio/cap_ios.h"
#import "opencv2/highgui/ios.h"
#endif
#ifdef __OBJC__
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <AVKit/AVKit.h>
#import <CoreMedia/CoreMedia.h>
#endif
#include "cosaliency.h"
#include "cointerest.h"

//#import "opencv2/highgui/ios.h"



// Slightly changed things here to employ the CvVideoCameraDelegate
@interface ViewController : UIViewController<CvVideoCameraDelegate>
{
    CvVideoCamera *videoCamera; // OpenCV class for accessing the camera
    AVAudioRecorder *recorder;
    AVPlayer *player;
    
}
// Declare internal property of videoCamera
@property (nonatomic, retain) CvVideoCamera *videoCamera;


//@property (strong, nonatomic) IBOutlet UIButton* btnPlay;
//- (IBAction)btnPlay:(id)sender;
//- (IBAction)btnTakePhoto:(id)sender;





@end

