/*
  ==============================================================================

    sparklepp.cpp
    Created: 17 Dec 2015 5:45:43pm
    Author:  Steve Flower

  ==============================================================================
*/

#include "Native\Win\winsparkle.h"
#include <algorithm>

#include "sparklepp.h"

class Sparkle::Private {
public:
    Private (URL appcastURL) :
        initialised (false),
        appcastURL (appcastURL)
    {

    }

    bool initialised;
    const URL appcastURL;

private:
    Private operator=(Private) = delete;
};

Sparkle::Sparkle (const juce::URL& appcastUrl)
{
    d = new Private (appcastUrl);
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

void Sparkle::checkForUpdateInBackground ()
{
    if (!d->initialised)
    {
        win_sparkle_init ();
        d->initialised = true;
    }
}



static int isVersionNumberGreater (const String &firstVersionNumber, const String& secondVersionNumber)
{
    auto first = StringArray::fromTokens (firstVersionNumber, "vV.", "");
    auto second = StringArray::fromTokens (secondVersionNumber, "vV.", "");

    first.removeString ("");
    second.removeString ("");

    for (int i = 0; i < first.size (); ++i)
    {
        if (first[i].getIntValue () > second[i].getIntValue ())
        {
            return -1;
        }
        else if (first[i].getIntValue () < second[i].getIntValue ())
        {
            return 1;
        }
    }

    return 0;
}

class VersionNumberDescendingComparitor
{
public:
    VersionNumberDescendingComparitor () {}

    int compareElements (XmlElement* first, XmlElement* second)
    {
        auto firstEnclosure = first->getChildByName ("enclosure");
        auto firstVersionNumber = firstEnclosure->getStringAttribute ("sparkle:version", juce::String::empty);

        auto secondEnclosure = second->getChildByName ("enclosure");
        auto secondVersionNumber = secondEnclosure->getStringAttribute ("sparkle:version", juce::String::empty);

        return isVersionNumberGreater (firstVersionNumber, secondVersionNumber);
    }
};

static String getLatestVersionNumber (XmlElement* xml)
{
    if (xml == nullptr)
    {
        return String::empty;
    }

    if (xml->hasTagName ("rss") == false)
    {
        Logger::outputDebugString ("getLatestVersionNumber: RSS tag not found");
        return String::empty;
    }

    auto channel = xml->getChildByName ("channel");

    if (channel == nullptr)
    {
        Logger::outputDebugString ("getLatestVersionNumber: Channel not found");
        return String::empty;
    }

    auto item = channel->getChildByName ("item");

    if (item == nullptr)
    {
        Logger::outputDebugString ("getLatestVersionNumber: No items found");
        return String::empty;
    }

    juce::ScopedPointer<juce::Array<juce::XmlElement*>> items = new juce::Array<XmlElement*> ();

    while (item != nullptr)
    {
        items->add (item);
        item = item->getNextElementWithTagName ("item");
    }

    VersionNumberDescendingComparitor versionNumberDescendingComparitor;
    items->sort (versionNumberDescendingComparitor);

    auto latestRelease = items->getFirst()->getChildByName ("enclosure");
    
    if (latestRelease == nullptr)
    {
        Logger::outputDebugString ("getLatestVersionNumber: Latest release not found");
        return String::empty;
    }

    return latestRelease->getStringAttribute ("sparkle:version", juce::String::empty);
}

static String getCurrentInstalledVersion ()
{
    return JUCEApplicationBase::getInstance ()->getApplicationVersion ();
}

void Sparkle::checkForUpdateInformation ()
{
    ScopedPointer<XmlElement> xml = d->appcastURL.readEntireXmlStream ();

    if (xml == nullptr)
    {
        updaterDidNotFindUpdate ();
        return;
    }

    auto latestReleaseVersion = getLatestVersionNumber (xml);

    if ((latestReleaseVersion == String::empty) || 
        isVersionNumberGreater (getCurrentInstalledVersion (), latestReleaseVersion) != 1)
    { 
        updaterDidNotFindUpdate ();
    }
    else
    {
        didFindValidUpdate ();
    }
}

void Sparkle::didFindValidUpdate ()
{
    listeners.call (&Listener::didFindValidUpdate);
}

void Sparkle::updaterDidNotFindUpdate ()
{
    listeners.call (&Listener::updaterDidNotFindUpdate);
}

class AppcaspParserTests : public UnitTest
{
public:
    AppcaspParserTests () : UnitTest ("Appcasp Parser testing")
    {
        oneReleaseFixture = XmlDocument::parse (oneReleaseFixtureContent);
        twoReleaseFixture = XmlDocument::parse (twoReleaseFixtureContent);
    }

    void runTest () override
    {
        isVersionNumberGreaterTest ();

        appcastParsingTest ();
    }

    void isVersionNumberGreaterTest ()
    {
        beginTest ("isVersionNumberGreater");
        expect (isVersionNumberGreater ("v1.0.0", "v0.0.1") == -1);
        expect (isVersionNumberGreater ("1.0.0", "0.0.1") == -1);
        expect (isVersionNumberGreater ("V1.0.0", "V0.0.1") == -1);
        expect (isVersionNumberGreater ("v1.0.1", "v1.0.0") == -1);
        expect (isVersionNumberGreater ("1.1.1", "v1.1.0") == -1);

        expect (isVersionNumberGreater ("v0.0.1", "v1.0.0") == 1);
        expect (isVersionNumberGreater ("0.0.1", "1.0.0") == 1);
        expect (isVersionNumberGreater ("V0.0.1", "V1.0.0") == 1);
        expect (isVersionNumberGreater ("v1.0.0", "v1.0.1") == 1);
        expect (isVersionNumberGreater ("1.1.0", "v1.1.1") == 1);

        expect (isVersionNumberGreater ("v0.0.1", "v0.0.1") == 0);
        expect (isVersionNumberGreater ("1.0.0", "1.0.0") == 0);
        expect (isVersionNumberGreater ("1.0.0", "V1.0.0") == 0);
    }

    void appcastParsingTest ()
    {
        beginTest ("getLatestVersionNumber");
        expect (getLatestVersionNumber (oneReleaseFixture) == String ("2.0"));

        expect (getLatestVersionNumber (twoReleaseFixture) == String ("2.1"));
    }

private:
    ScopedPointer<XmlElement> oneReleaseFixture;
    ScopedPointer<XmlElement> twoReleaseFixture;

    const String oneReleaseFixtureContent = { "<?xml version=\"1.0\" encoding=\"utf-8\"?><rss version=\"2.0\" xmlns:sparkle=\"http://www.andymatuschak.org/xml-namespaces/sparkle\"  xmlns:dc=\"http://purl.org/dc/elements/1.1/\"><channel><title>Sparkle Test App Changelog</title><link>http://sparkle-project.org/files/sparkletestcast.xml</link><description>Most recent changes with links to updates.</description><language>en</language><item><title>Version 2.0</title><description><![CDATA[<ul><li>Lorem ipsum dolor sit amet, consectetur adipiscing elit.</li><li>Suspendisse sed felis ac ante ultrices rhoncus.Etiam quis elit vel nibh placerat facilisis in id leo.</li><li>Vestibulum nec tortor odio, nec malesuada libero.Cras vel convallis nunc.</li><li>Suspendisse tristique massa eget velit consequat tincidunt.Praesent sodales hendrerit pretium.</li></ul>]]></description><pubDate>Sat, 26 Jul 2014 15:20 : 11 + 0000</pubDate><enclosure url=\"https://sparkle-project.org/files/Sparkle%20Test%20App.zip\" sparkle:version=\"2.0\" length=\"107758\" type=\"application/octet-stream\" sparkle:dsaSignature=\"MCwCFCdoW13VBGJWIfIklKxQVyetgxE7AhQTVuY9uQT0KOV1UEk21epBsGZMPg==\"/></item></channel></rss>" };
    const String twoReleaseFixtureContent = { "<?xml version=\"1.0\" encoding=\"utf-8\"?><rss version=\"2.0\" xmlns:sparkle=\"http://www.andymatuschak.org/xml-namespaces/sparkle\"  xmlns:dc=\"http://purl.org/dc/elements/1.1/\"><channel><title>Sparkle Test App Changelog</title><link>http://sparkle-project.org/files/sparkletestcast.xml</link><description>Most recent changes with links to updates.</description><language>en</language><item><title>Version 2.0</title><description><![CDATA[<ul><li>Lorem ipsum dolor sit amet, consectetur adipiscing elit.</li><li>Suspendisse sed felis ac ante ultrices rhoncus.Etiam quis elit vel nibh placerat facilisis in id leo.</li><li>Vestibulum nec tortor odio, nec malesuada libero.Cras vel convallis nunc.</li><li>Suspendisse tristique massa eget velit consequat tincidunt.Praesent sodales hendrerit pretium.</li></ul>]]></description><pubDate>Sat, 26 Jul 2014 15:20 : 11 + 0000</pubDate><enclosure url=\"https://sparkle-project.org/files/Sparkle%20Test%20App.zip\" sparkle:version=\"2.0\" length=\"107758\" type=\"application/octet-stream\" sparkle:dsaSignature=\"MCwCFCdoW13VBGJWIfIklKxQVyetgxE7AhQTVuY9uQT0KOV1UEk21epBsGZMPg==\"/></item><item><title>Version 2.1</title><description><![CDATA[<ul><li>Lorem ipsum dolor sit amet, consectetur adipiscing elit.</li><li>Suspendisse sed felis ac ante ultrices rhoncus.Etiam quis elit vel nibh placerat facilisis in id leo.</li><li>Vestibulum nec tortor odio, nec malesuada libero.Cras vel convallis nunc.</li><li>Suspendisse tristique massa eget velit consequat tincidunt.Praesent sodales hendrerit pretium.</li></ul>]]></description><pubDate>Tues, 29 Jul 2014 15:20 : 11 + 0000</pubDate><enclosure url=\"https://sparkle-project.org/files/Sparkle%20Test%20App.zip\" sparkle:version=\"2.1\" length=\"107758\" type=\"application/octet-stream\"/></item></channel></rss>" };

};

static AppcaspParserTests appcastParserTests;