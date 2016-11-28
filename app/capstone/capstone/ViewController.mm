//
//  ViewController.m
//  capstone
//
//  Created by Judy Chang on 11/27/16.
//  Copyright © 2016 Judy Chang. All rights reserved.
//

#import "ViewController.h"

// Include stdlib.h and std namespace so we can mix C++ code in here
#include <stdlib.h>
#include <iostream>

using namespace std;

@interface ViewController ()

@end
NSString *path = [[NSBundle mainBundle] pathForResource:@"IMG_5094" ofType:@"MOV"];
NSURL *url = [NSURL fileURLWithPath:path];

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    cout << self.view.frame.size.width << " " << self.view.frame.size.height << endl;
//    AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:url options:nil];
//    AVAssetImageGenerator *generator = [[AVAssetImageGenerator alloc] initWithAsset:asset];
//    generator.requestedTimeToleranceAfter =  kCMTimeZero;
//    generator.requestedTimeToleranceBefore =  kCMTimeZero;
//    for (Float64 i = 0; i < CMTimeGetSeconds(asset.duration) *  1 ; i++){
//        @autoreleasepool {
//            CMTime time = CMTimeMake(i, 1);
//            NSError *err;
//            CMTime actualTime;
//            CGImageRef image = [generator copyCGImageAtTime:time actualTime:&actualTime error:&err];
//            UIImage *generatedImage = [[UIImage alloc] initWithCGImage:image];
//            [self saveImage: generatedImage atTime:actualTime]; // Saves the image on document directory and not memory
//            CGImageRelease(image);
//        }
//    }
    
    
    
//    CMTime time = CMTimeMake(1, 1);
//    CGImageRef imageRef = [imageGenerator copyCGImageAtTime:time actualTime:NULL error:NULL];
//    UIImage *thumbnail = [UIImage imageWithCGImage:imageRef];
//    UIImageView *images =[[UIImageView alloc]initWithFrame:CGRectMake(0, 0, 320, 300)];
//    [images setImage:thumbnail];
//    [self.view addSubview:images];
//    CGImageRelease(imageRef);  // CGImageRef won't be released by ARC
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)btnPlay:(id)sender {

    player = [AVPlayer playerWithURL:url];
    AVPlayerLayer *playerLayer = [AVPlayerLayer playerLayerWithPlayer:player];
    playerLayer.frame = self.view.bounds;
    [self.view.layer addSublayer:playerLayer];
    [player play];
}


AVAsset *asset = [AVAsset assetWithURL:url];
AVAssetImageGenerator *imageGenerator = [[AVAssetImageGenerator alloc]initWithAsset:asset];
Float64 FPS=1;
Float64 idx=0;

- (IBAction)btnTakePhoto:(id)sender {
    if (idx<CMTimeGetSeconds(asset.duration)*FPS) {
//        cout << i << endl;
        @autoreleasepool {
            CMTime time = CMTimeMake(idx, FPS);
            NSError *err;
            CMTime actualTime;
            CGImageRef image = [imageGenerator copyCGImageAtTime:time actualTime:&actualTime error:&err];
            UIImage *generatedImage = [[UIImage alloc] initWithCGImage:image ];
            UIImageView *images =[[UIImageView alloc]initWithFrame:CGRectMake(0, 0, self.view.frame.size.height/2, self.view.frame.size.width/2)];
            [images setImage:generatedImage];
            [self.view addSubview:images];
            //            [self saveImage: generatedImage atTime:actualTime];
            CGImageRelease(image);
        }
        idx++;
    }
}

@end
