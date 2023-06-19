/*
==============================================================================

Sparkle.mm
      Created: 17 Dec 2015 5:45:43pm
  Author:  Steve Flower

  ==============================================================================
  */

#if SPARKLE_UPDATER_ENABLE

#import <Sparkle/Sparkle.h>

#include "sparklepp.h"

@interface SparkleDelegate : NSObject <SPUUpdaterDelegate>
{
    Sparkle* delegateHandler;
}

- (id)init:(Sparkle*)handler;

/*!
Returns whether Sparkle should prompt the user about automatic update checks.

Use this to override the default behavior.

\param updater The SPUUpdater instance.
*/
- (BOOL)updaterShouldPromptForPermissionToCheckForUpdates:(SPUUpdater*)updater;

/*!
Called when a valid update is found by the update driver.

\param updater The SPUUpdater instance.
\param item The appcast item corresponding to the update that is proposed to be installed.
*/
- (void)updater:(SPUUpdater*)updater didFindValidUpdate:(SUAppcastItem*)item;

/*!
Called when a valid update is not found.

\param updater The SPUUpdater instance.
*/
- (void)updaterDidNotFindUpdate:(SPUUpdater*)updater error:(NSError*)error;

/*!
Called after the specified update failed to download.

\param updater The SPUUpdater instance.
\param item The appcast item corresponding to the update that failed to download.
\param error The error generated by the failed download.
*/
- (void)updater:(SPUUpdater*)updater failedToDownloadUpdate:(SUAppcastItem*)item error:(NSError*)error;

/*!ee
Called when the user clicks the cancel button while and update is being downloaded.

\param updater The SPUUpdater instance.
*/
- (void)userDidCancelDownload:(SPUUpdater*)updater;

/*!
Called after an update is aborted due to an error.

\param updater The SUUpdater instance.
\param error The error that caused the abort
*/
- (void)updater:(SPUUpdater*)updater didAbortWithError:(NSError*)error;

@end

@implementation SparkleDelegate
- (id)init:(Sparkle*)handler
{
    self = [super init];
    if (self)
    {
        delegateHandler = handler;
    }
    return self;
}

/*!
Returns whether Sparkle should prompt the user about automatic update checks.

Use this to override the default behavior.

\param updater The SPUUpdater instance.
*/
- (BOOL)updaterShouldPromptForPermissionToCheckForUpdates:(SPUUpdater*)updater
{
    return 0;
}

/*!
Called when a valid update is found by the update driver.

\param updater The SPUUpdater instance.
\param item The appcast item corresponding to the update that is proposed to be installed.
*/
- (void)updater:(SPUUpdater*)updater didFindValidUpdate:(SUAppcastItem*)item
{
    delegateHandler->didFindValidUpdate();
}

/*!
Called when a valid update is not found.

\param updater The SPUUpdater instance.
*/
- (void)updaterDidNotFindUpdate:(SPUUpdater*)updater error:(NSError*)error
{
    delegateHandler->updaterDidNotFindUpdate();
}

/*!
Called after the specified update failed to download.

\param updater The SPUUpdater instance.
\param item The appcast item corresponding to the update that failed to download.
\param error The error generated by the failed download.
*/
- (void)updater:(SPUUpdater*)updater failedToDownloadUpdate:(SUAppcastItem*)item error:(NSError*)error;
{
    delegateHandler->updaterDidNotFindUpdate();
}

/*!
Called when the user clicks the cancel button while and update is being downloaded.

\param updater The SPUUpdater instance.
*/
- (void)userDidCancelDownload:(SPUUpdater*)updater
{
    delegateHandler->updaterDidNotFindUpdate();
}

/*!
Called after an update is aborted due to an error.

\param updater The SPUUpdater instance.
\param error The error that caused the abort
*/
- (void)updater:(SPUUpdater*)updater didAbortWithError:(NSError*)error
{
    delegateHandler->updaterDidNotFindUpdate();
}
@end

@interface AppUpdaterDelegate : NSObject <SPUUpdaterDelegate>
@property (nonatomic, strong) SPUStandardUpdaterController* updaterController;
@end

@implementation AppUpdaterDelegate
@end

Sparkle::Sparkle()
{
    updaterDelegate = [[AppUpdaterDelegate alloc] init];
    updaterDelegate.updaterController = [[SPUStandardUpdaterController alloc] initWithStartingUpdater:YES updaterDelegate:updaterDelegate userDriverDelegate:nil];

    [updaterDelegate.updaterController.updater clearFeedURLFromUserDefaults];
}

Sparkle::~Sparkle()
{
    [updaterDelegate.updaterController release];
    [updaterDelegate release];
}

void Sparkle::checkForUpdateInBackground()
{
    [updaterDelegate.updaterController.updater checkForUpdatesInBackground];
}

void Sparkle::checkForUpdateInformation()
{
    [updaterDelegate.updaterController.updater checkForUpdateInformation];
}

void Sparkle::didFindValidUpdate()
{
    listeners.call (&Listener::didFindValidUpdate);
}

void Sparkle::updaterDidNotFindUpdate()
{
    listeners.call (&Listener::updaterDidNotFindUpdate);
}

void Sparkle::addListener (Listener* listener)
{
    listeners.add (listener);
}

void Sparkle::removeListener (Listener* listener)
{
    listeners.remove (listener);
}

#endif
