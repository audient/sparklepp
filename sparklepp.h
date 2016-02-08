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
    
    /* This will asynchronously launch an update GUI if an update is available */
    void checkForUpdateInBackground();
    
    /* This will asynchronously check if an update is available.
     * If an update is available the didFindValidUpdate method will becalled on 
     * listeners.
     * If an update is not available the updaterDidNotFindUpdate method will 
     * be called on listeners.
     */
    void checkForUpdateInformation();
    
    class Listener
    {
    public:
        virtual ~Listener() {}
        
        virtual void didFindValidUpdate() {}
        virtual void updaterDidNotFindUpdate() {}
    };
    
    void addListener (Listener* listener)
    {
        listeners.add (listener);
    }

    void removeListener (Listener* listener)
    {
        listeners.remove (listener);
    }
    
    /* internal */
    void didFindValidUpdate();
    void updaterDidNotFindUpdate();
private:
    class Private;
    juce::ScopedPointer<Private> d;
    
    juce::ListenerList<Listener> listeners;
};

#endif
