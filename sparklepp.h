/*
  ==============================================================================

    sparklepp.h
    Created: 28 Sep 2015 12:22:34pm
    Author:  Stephen Flower

  ==============================================================================
*/
#ifndef SPARKLEPP_H_INCLUDED
#define SPARKLEPP_H_INCLUDED

#include "JuceHeader.h"

class Sparkle
{
public:
    Sparkle(const juce::URL& appcastUrl);
    ~Sparkle();
    
    void checkForUpdates();
    
private:
    class Private;
    juce::ScopedPointer<Private> d;
};

#endif
