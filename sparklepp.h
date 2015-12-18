/*
  ==============================================================================

    sparkle.h
    Created: 28 Sep 2015 12:22:34pm
    Author:  Stephen Flower

  ==============================================================================
*/
#ifndef SPARKLE_H_INCLUDED
#define SPARKLE_H_INCLUDED

#include "JuceHeader.h"

class Sparkle
{
public:
    Sparkle();
    Sparkle(const juce::URL& appcastUrl);
    ~Sparkle();
    
    void setAppCastURL(const juce::URL& appcastURL);
    
    void checkForUpdates();
    
private:
    class Private;
    juce::ScopedPointer<Private> d;
    
    void initialise ();
};

#endif
