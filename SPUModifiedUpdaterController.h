//
//  SPUModifiedUpdaterController.h
//
//

#import <Foundation/Foundation.h>
#import <Sparkle/SUExport.h>

NS_ASSUME_NONNULL_BEGIN

@class SPUUpdater;
@class SPUStandardUserDriver;
@class NSMenuItem;
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
@property (nonatomic, readonly) SPUStandardUserDriver *userDriver;

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
