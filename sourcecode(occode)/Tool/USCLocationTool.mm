//
//  USCLocationTool.m
//  asr&nlu&tts
//
//  Created by yunzhisheng-zy on 15-3-12.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCLocationTool.h"

@interface USCLocationTool()<CLLocationManagerDelegate>
@property (nonatomic,strong) CLLocationManager *mgr;
@property (nonatomic,strong) NSString *city;
@property (nonatomic,assign) CLLocationCoordinate2D coordinate;
@property (nonatomic,strong) NSString *gps;
@end

@implementation USCLocationTool

- (CLLocationManager *)mgr
{
    if (!_mgr) {
        _mgr = [[CLLocationManager alloc]init];
        _mgr.delegate = self;
    }
    return _mgr;
}
- (instancetype)init
{
    if (self = [super init]) {
        [[NSUserDefaults standardUserDefaults] setObject:@"北京市" forKey:@"currentcity"];
        [[NSUserDefaults standardUserDefaults] setObject:@" 39.97768,116.35999" forKey:@"currentgps"];
    }
    return self;
}

- (void)start
{
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0 )
    {
        if([CLLocationManager authorizationStatus] != kCLAuthorizationStatusAuthorizedAlways)
           [self.mgr requestAlwaysAuthorization];
    }
    else
    {
        [self.mgr startUpdatingLocation]; //Will update location immediately
    }
}


#pragma mark - location delegate
- (void)locationManager:(CLLocationManager *)manager didChangeAuthorizationStatus:(CLAuthorizationStatus)status
{
    if (status == kCLAuthorizationStatusAuthorizedAlways )
        [self.mgr startUpdatingLocation];
}

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{
    CLLocation *location = [locations lastObject];
    self.coordinate = location.coordinate;
    //获取纬度
    NSString *latitude = [NSString stringWithFormat:@"%.5f", self.coordinate.latitude];
    //获取经度
    NSString *longitude = [NSString stringWithFormat:@"%.5f", self.coordinate.longitude];

    //产生所需要的gps格式
    _gps = [NSString stringWithFormat:@"%@,%@", latitude, longitude, nil];
    if(_gps)
       [[NSUserDefaults standardUserDefaults] setObject:_gps forKey:@"gps"];

     CLGeocoder *geoCoder = [[CLGeocoder alloc] init];
    [geoCoder reverseGeocodeLocation:location completionHandler:^(NSArray *placemarks, NSError *error) {
        if (!error) {
            CLPlacemark *place = [placemarks lastObject];
            _city = place.administrativeArea;

            if (_city)
               [[NSUserDefaults standardUserDefaults] setObject:self.city forKey:@"currentcity"];

            [self.mgr stopUpdatingLocation];
        }
    }];
}

@end
