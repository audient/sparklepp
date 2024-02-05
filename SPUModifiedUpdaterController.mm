//
//  SPUStandardUpdaterController.m
//

//#include "SPUModifiedUpdaterController.h"
#import <Sparkle/Sparkle.h>
#import "SPUCommandLineUserDriver.h"

// We use public instance variables instead of properties for the updater / user driver delegates
// because we want them to be connectable outlets from Interface Builder, but we do not want their setters to be invoked
// programmatically.


NS_ASSUME_NONNULL_BEGIN

@class SPUUpdater;
@class SPUStandardUserDriver;
@protocol SPUUserDriver, SPUUpdaterDelegate, SPUStandardUserDriverDelegate;

/**
 A controller class that instantiates a `SPUUpdater`.

 This class can be instantiated programatically using `-initWithUpdaterDelegate:userDriverDelegate:` or `-initWithStartingUpdater:updaterDelegate:userDriverDelegate:`.

 The controller's updater targets the application's main bundle and uses text logging.

 The controller creates an `SPUUpdater` instance using a `SPUCommandLineUserDriver`.
 It also allows hooking up the updater's and user driver's delegates.

 If you need more control over what bundle you want to update, or you want to provide a custom user interface (via `SPUUserDriver`), please use `SPUUpdater` directly instead.
  */
SU_EXPORT @interface SPUModifiedUpdaterController : NSObject
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-interface-ivars"
    /**
     * Interface builder outlet for the updater's delegate.
     */
    IBOutlet __weak id<SPUUpdaterDelegate> updaterDelegate;

    /**
     * Interface builder outlet for the user driver's delegate.
     */
    IBOutlet __weak id<SPUStandardUserDriverDelegate> userDriverDelegate;
#pragma clang diagnostic pop
}

/**
 Accessible property for the updater. Some properties on the updater can be binded via KVO

 When instantiated from a nib, don't perform update checks before the application has finished launching in a MainMenu nib (i.e applicationDidFinishLaunching:) or before the corresponding window/view controller has been loaded (i.e, windowDidLoad or viewDidLoad). The updater is not guaranteed to be started yet before these points.
 */
@property (nonatomic, readonly) SPUUpdater *updater;

/**
 Accessible property for the updater's user driver.
 */
@property (nonatomic, readonly) SPUCommandLineUserDriver *userDriver;

/**
 Create a new `SPUStandardUpdaterController` from a nib.

 You cannot call this initializer directly. You must instantiate a `SPUStandardUpdaterController` inside of a nib (typically the MainMenu nib) to use it.

 To create a `SPUStandardUpdaterController` programatically, use `-initWithUpdaterDelegate:userDriverDelegate:` or `-initWithStartingUpdater:updaterDelegate:userDriverDelegate:` instead.
 */
- (instancetype)init NS_UNAVAILABLE;

/**
 Create a new `SPUStandardUpdaterController` programmatically.

 The updater is started automatically. See `-startUpdater`  for more information.
 */
- (instancetype)initWithUpdaterDelegate:(nullable id<SPUUpdaterDelegate>)updaterDelegate userDriverDelegate:(nullable id<SPUStandardUserDriverDelegate>)userDriverDelegate;

/**
 Create a new `SPUStandardUpdaterController` programmatically allowing you to specify whether or not to start the updater immediately.

 You can specify whether or not you want to start the updater immediately.
 If you do not start the updater, you must invoke `-startUpdater` at a later time to start it.
 */
- (instancetype)initWithStartingUpdater:(BOOL)startUpdater updaterDelegate:(nullable id<SPUUpdaterDelegate>)updaterDelegate userDriverDelegate:(nullable id<SPUStandardUserDriverDelegate>)userDriverDelegate;

/**
 Starts the updater if it has not already been started.

 You should only call this method yourself if you opted out of starting the updater on initialization.
 Hence, do not call this yourself if you are instantiating this controller from a nib.

 This invokes  `-[SPUUpdater startUpdater:]`. If the application is misconfigured with Sparkle, an error is logged and an alert is shown to the user (after a few seconds) to contact the developer.
 If you want more control over this behavior, you can create your own `SPUUpdater` instead of using `SPUStandardUpdaterController`.
 */
- (void)startUpdater;

@end

NS_ASSUME_NONNULL_END

/*

@interface SPUModifiedUpdaterController

// Needed for KVO
@property (nonatomic) SPUUpdater *updater;

@end*/

@implementation SPUModifiedUpdaterController

@synthesize updater = _updater;
@synthesize userDriver = _userDriver;

- (void)_initUpdater SPU_OBJC_DIRECT
{
    NSBundle *hostBundle = [NSBundle mainBundle];
    NSNumber *automaticDownloading = @(YES);
    
    SUUpdatePermissionResponse *updatePermissionResponse = [[SUUpdatePermissionResponse alloc] initWithAutomaticUpdateChecks:YES automaticUpdateDownloading: automaticDownloading sendSystemProfile:NO];
    
    id <SPUUserDriver> userDriver = [[SPUCommandLineUserDriver alloc] initWithUpdatePermissionResponse:updatePermissionResponse deferInstallation:YES verbose:YES];
            _updater = [[SPUUpdater alloc] initWithHostBundle:hostBundle applicationBundle:hostBundle userDriver:userDriver delegate:self->updaterDelegate];

    _userDriver = userDriver;
}

- (instancetype)initWithUpdaterDelegate:(nullable id<SPUUpdaterDelegate>)theUpdaterDelegate userDriverDelegate:(nullable id<SPUStandardUserDriverDelegate>)theUserDriverDelegate
{
    return [self initWithStartingUpdater:YES updaterDelegate:theUpdaterDelegate userDriverDelegate:theUserDriverDelegate];
}

- (instancetype)initWithStartingUpdater:(BOOL)startUpdater updaterDelegate:(nullable id<SPUUpdaterDelegate>)theUpdaterDelegate userDriverDelegate:(nullable id<SPUStandardUserDriverDelegate>)theUserDriverDelegate
{
    if ((self = [super init])) {
        self->updaterDelegate = theUpdaterDelegate;
        self->userDriverDelegate = theUserDriverDelegate;

        [self _initUpdater];

        if (startUpdater) {
            [self startUpdater];
        }
    }
    return self;
}

- (void)startUpdater
{
    NSError *updaterError = nil;
    if (![_updater startUpdater:&updaterError]) {
        NSLog(@"Fatal updater error (%ld): %@", updaterError.code, updaterError.localizedDescription);
    }
}

- (IBAction)checkForUpdates:(nullable id)__unused sender
{
    [_updater checkForUpdates];
}

@end
