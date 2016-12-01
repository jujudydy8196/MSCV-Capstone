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
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

@interface ViewController () {
    UIImageView *imageView_; // Setup the image view
    UITextView *fpsView_; // Display the current FPS
    vector<Mat> data_image_cv;
    vector<Mat> data_org_img;
}
@end

@implementation ViewController

@synthesize videoCamera;

NSString *path = [[NSBundle mainBundle] pathForResource:@"IMG_5094" ofType:@"MOV"];
NSURL *url = [NSURL fileURLWithPath:path];
AVAsset *asset = [AVAsset assetWithURL:url];
AVAssetImageGenerator *imageGenerator = [[AVAssetImageGenerator alloc]initWithAsset:asset];
Float64 FPS=1;
Float64 idx=0;
vector<Mat> cosalResult;
Mat cosal;

- (void)viewDidLoad {
    [super viewDidLoad];

    // Initialize the view
    // Hacky way to initialize the view to ensure the aspect ratio looks correct
    // across all devices. Unfortunately, setting UIViewContentModeScaleAspectFill
    // does not work with the CvCamera Delegate so we have to hard code everything....
    //
    // Assuming camera input is 352x288 (set using AVCaptureSessionPreset)
    //    float cam_width = 288; float cam_height = 352;
    float cam_width = 480; float cam_height = 640;
    //    float cam_width = 720; float cam_height = 1280;
    
    // Take into account size of camera input
    int view_width = self.view.frame.size.width;
    int view_height = (int)(cam_height*self.view.frame.size.width/cam_width);
    int offset = (self.view.frame.size.height - view_height)/2;
    
//    imageView_ = [[UIImageView alloc] initWithFrame:CGRectMake(0.0, offset, view_width, view_height)];
    //[imageView_ setContentMode:UIViewContentModeScaleAspectFill]; (does not work)
//    [self.view addSubview:imageView_]; // Add the view
    
//    cout << self.view.frame.size.width << " " << self.view.frame.size.height << endl;
//    cosalResult = cosaliency();
    cosal = cosaliency_co();
    
    UIImage *resImage = MatToUIImage(cosal*255);
    NSArray *imgpaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *filePath = [[imgpaths objectAtIndex:0] stringByAppendingPathComponent:@"cosal_co.png"];
    const char* cPath = [filePath cStringUsingEncoding:NSMacOSRomanStringEncoding];
    
    
    const cv::string newPaths = (const cv::string)cPath;
    imwrite(newPaths, cosal*255);
    
    //        NSString *cname = [framePath stringByAppendingPathComponent:filename];
    UIImage *cosalImg = [UIImage imageNamed:filePath];
    CGFloat frameWidth = (self.view.frame.size.width / cosalImg.size.width) * cosalImg.size.width;
    CGFloat frameHeight = (self.view.frame.size.width / cosalImg.size.width) * cosalImg.size.height;
//    cout << frameWidth << " " << frameHeight << endl;
    imageView_ = [[UIImageView alloc] initWithFrame:CGRectMake(0.0, self.view.frame.size.height/2-frameHeight/2, frameWidth, frameHeight)];
    // Save image.
    //        [UIImagePNGRepresentation(resImage) writeToFile:filePath atomically:YES];
    NSLog(filePath);
    [imageView_ setImage:cosalImg];
    [self.view addSubview:imageView_];
    cout << "cosal result finish" << endl;
//    cointerest();
    
    
    
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
    
    
    

    

//    
//    // Initialize the video camera
//    self.videoCamera = [[CvVideoCamera alloc] initWithParentView:imageView_];
//    self.videoCamera.delegate = self;
//    self.videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
//    self.videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
//    self.videoCamera.defaultFPS = FPS; // Set the frame rate
//    self.videoCamera.grayscaleMode = NO; // Get grayscale
//    self.videoCamera.rotateVideo = YES; // Rotate video so everything looks correct
//    
//    // Choose these depending on the camera input chosen
//    //    self.videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
//    self.videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset640x480;
////    self.videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset1280x720;
//    
//    // Finally add the FPS text to the view
//    fpsView_ = [[UITextView alloc] initWithFrame:CGRectMake(0,15,view_width,std::max(offset,35))];
//    [fpsView_ setOpaque:false]; // Set to be Opaque
//    [fpsView_ setBackgroundColor:[UIColor clearColor]]; // Set background color to be clear
//    [fpsView_ setTextColor:[UIColor redColor]]; // Set text to be RED
//    [fpsView_ setFont:[UIFont systemFontOfSize:18]]; // Set the Font size
//    [self.view addSubview:fpsView_];
//    
//    [videoCamera start];
    
    
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)btnPlay:(id)sender {
//    [videoCamera stop];
//    player = [AVPlayer playerWithURL:url];
//    AVPlayerLayer *playerLayer = [AVPlayerLayer playerLayerWithPlayer:player];
//    playerLayer.frame = self.view.bounds;
//    [self.view.layer addSublayer:playerLayer];
//    [player play];
}


//NSArray *writeDir = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//NSString *documentsDirectory = [writeDir objectAtIndex:0];



- (IBAction)btnTakePhoto:(id)sender {
//    for (int i=0; i<cosalResult.size(); i++) {
    if (idx<cosalResult.size()) {
//        cout << cosalResult[idx]*255 << endl << endl;
        cout << idx << endl;
        Mat img=cosalResult[idx];
//        NSString *cvpath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:[NSString stringWithFormat:@"%03d_cosal.png", idx]];
//        const char* cPath = [cvpath cStringUsingEncoding:NSMacOSRomanStringEncoding];
//        
//        const cv::string newPaths = (const cv::string)cPath;
//        cout << newPaths << endl;
//        imwrite(newPaths, img*255);
//        _debug(img*255);
//        cvtColor( img, img, CV_BGR2RGB);
        UIImage *resImage = MatToUIImage(img*255);
        NSArray *imgpaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *filePath = [[imgpaths objectAtIndex:0] stringByAppendingPathComponent:[ NSString stringWithFormat: @"%03f_cosal.png", idx ]];
        const char* cPath = [filePath cStringUsingEncoding:NSMacOSRomanStringEncoding];
        

        const cv::string newPaths = (const cv::string)cPath;
        imwrite(newPaths, img*255);

//        NSString *cname = [framePath stringByAppendingPathComponent:filename];
        UIImage *cosalImg = [UIImage imageNamed:filePath];
        // Save image.
//        [UIImagePNGRepresentation(resImage) writeToFile:filePath atomically:YES];
        NSLog(filePath);
        [imageView_ setImage:cosalImg];
        [self.view addSubview:imageView_];
        
//        imageView_.image =  [UIImage imageWithCGImage:[resImage CGImage]
//                                                scale:1.0
//                                          orientation: UIImageOrientationLeftMirrored];
        idx++;
        
    }
//    [videoCamera stop];
//    if (idx<CMTimeGetSeconds(asset.duration)*FPS) {
////        cout << i << endl;
//        @autoreleasepool {
//            CMTime time = CMTimeMake(idx, FPS);
//            NSError *err;
//            CMTime actualTime;
//            CGImageRef image = [imageGenerator copyCGImageAtTime:time actualTime:&actualTime error:&err];
//            UIImage *generatedImage = [[UIImage alloc] initWithCGImage:image
//                                                                 scale:1
//                                                           orientation:UIImageOrientationRight];
//            UIImageView *images =[[UIImageView alloc]initWithFrame:CGRectMake(0, 0, self.view.frame.size.width/2, self.view.frame.size.height/2)];
//            [images setImage:generatedImage];
//            [self.view addSubview:images];
//            
//            //            [self saveImage: generatedImage atTime:actualTime];
//            CGImageRelease(image);
//        }
//        idx++;
//    }
}


// Function to run apply image on
- (void) processImage:(cv:: Mat &)image
{
    
    NSString *fps_NSStr = [NSString stringWithFormat:@"FPS = %2.2f",idx];
    
    // Have to do this so as to communicate with the main thread
    // to update the text display
    dispatch_sync(dispatch_get_main_queue(), ^{
        fpsView_.text = fps_NSStr;
    });
//    cout << image.size() << endl;
    data_org_img.push_back(image);
    idx++;
    //TODO cosaliency
//    if (idx==10)
//        cosaliency();
    if (idx==15)
        [videoCamera stop];
    
}

@end
