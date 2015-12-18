/*
  ==============================================================================

    sparklepp.cpp
    Created: 17 Dec 2015 5:45:43pm
    Author:  Steve Flower

  ==============================================================================
*/

#include "sparklepp.h"

#include "Native\Win\winsparkle.h"

class Sparkle::Private {
public:
    Private () :
        initialised (false)
    {

    }

    bool initialised;
};

Sparkle::Sparkle (const juce::URL& appcastUrl)
{
    d = new Private();
    win_sparkle_set_appcast_url (appcastUrl.toString (true).toUTF8());
}

Sparkle::~Sparkle ()
{
    if (d->initialised)
    {
        win_sparkle_init();
        d->initialised = false;
    }
}

void Sparkle::checkForUpdates ()
{
    if (!d->initialised)
    {
        win_sparkle_init ();
        d->initialised = true;
    }
}