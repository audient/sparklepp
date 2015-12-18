/*
  ==============================================================================

    Sparkle.mm
    Created: 17 Dec 2015 5:45:43pm
    Author:  Steve Flower

  ==============================================================================
*/

#import <Sparkle/Sparkle.h>

#include "sparklepp.h"

class Sparkle::Private {
public:
    SUUpdater* updater;
};

Sparkle::Sparkle ()
{
    initialise ();
}

Sparkle::Sparkle (const juce::URL& appcastURL)
{
    initialise();
    setAppCastURL (appcastURL);
}

void Sparkle::initialise()
{
    d = new Private;
    d->updater = [[SUUpdater sharedUpdater] retain];

}

Sparkle::~Sparkle()
{
    [d->updater release];
}

void Sparkle::setAppCastURL(const juce::URL& appcastURL)
{
    NSURL* url = [NSURL URLWithString:
                  [NSString stringWithUTF8String: appcastURL.toString(true).toRawUTF8()]];
    [d->updater setFeedURL: url];
}

void Sparkle::checkForUpdates()
{
    [d->updater checkForUpdatesInBackground];
}