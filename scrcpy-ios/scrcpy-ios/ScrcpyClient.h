//
//  ScrcpyClient.h
//  scrcpy-ios
//
//  Created by Ethan on 2022/6/2.
//

#import <Foundation/Foundation.h>
#import <SDL2/SDL_events.h>
#import <SDL2/SDL_system.h>

NS_ASSUME_NONNULL_BEGIN

#define ScrcpySharedClient      [ScrcpyClient sharedClient]

@interface ScrcpyClient : NSObject
// ADB Setup
@property (nonatomic, assign)   NSInteger   adbDaemonPort;
@property (nonatomic, copy)     NSString    *adbHomePath;

// ADB Callbacks
@property (nonatomic, copy) void (^onADBConnecting)(NSString *serial);
@property (nonatomic, copy) void (^onADBConnected)(NSString *serial);
@property (nonatomic, copy) void (^onADBUnauthorized)(NSString *serial);
@property (nonatomic, copy) void (^onADBDisconnected)(NSString *serial);
@property (nonatomic, copy) void (^onADBConnectFailed)(NSString *serial, NSString *message);

// Scrcpy Callbacks
@property (nonatomic, copy) void (^onScrcpyConnected)(NSString *serial);
@property (nonatomic, copy) void (^onScrcpyConnectFailed)(NSString *serial);
@property (nonatomic, copy) void (^onScrcpyDisconnected)(NSString *serial);

// Pending URL Scheme
@property (nonatomic, strong, nullable)   NSURL   *pendingScheme;

// Switch Always Show Navigation Buttons
@property (nonatomic, assign)   BOOL    shouldAlwaysShowNavButtons;

// Switch to enable power saving mode on iPhone, with lower refresh rate and lower interactive response
@property (nonatomic, assign)   BOOL    enablePowerSavingMode;

// Shared instance
+(instancetype)sharedClient;

// ADB trace log
-(void)enableADBVerbose;

// Start ADB Server
-(void)startADBServer;

// Start scrcpy
-(void)startWith:(NSString *)adbHost adbPort:(NSString *)adbPort options:(NSArray *)scrcpyOptions;

// Stop scrcpy
-(void)stopScrcpy;

// Execute ADB commands
-(BOOL)adbExecute:(NSArray <NSString *> *)commands message:(NSString *_Nullable*_Nullable)message;

// Check pending scheme and start scrcpy
-(void)checkStartScheme;

// Default scrcpy options
-(NSArray *)defaultScrcpyOptions;

// Set scrcpy options
-(NSArray *)setScrcpyOption:(NSArray *)options name:(NSString *)name value:(NSString *)value;

// Send Home Button
-(void)sendHomeButton;

// Send Back Button
-(void)sendBackButton;

// Send SwitchApp Button
-(void)sendSwitchAppButton;

// Send Key Events
-(void)sendKeycodeEvent:(SDL_Scancode)scancode keycode:(SDL_Keycode)keycode keymod:(SDL_Keymod)keymod;

@end

NS_ASSUME_NONNULL_END
