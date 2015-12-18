#Sparkle++
A Juce module for integration of the Sparkle update frameworks for Mac and Windows

##Setup

To integrate Sparkle++ into your app you will need to add sparkle.framework and WinSparkle.dll to you projects and add the relavent configuration options to your Introjucer project settings.


###Mac Project Setup

You will need to download the sparkle.framwork from [here](http://sparkle-project.org/), and copy the framework to either your Xcode frameworks folder, or a custom Frameworks folder (highly recommended so you don't loose it each time you update Xcode). If you use a custom frameworks folder you will need to add it's location using the `Custom Xcode Flags` option in Introjucer like this:

	FRAMEWORK_SEARCH_PATHS="/path/to/your/Frameworks/"

You will also need to set where to look for the framework within the aplication bundle itself. A common convention is to place a Frameworks folder within the bundles `Contents` folder. To set this as the search path you will also need to add this to the `Custom Xcode Flags` option in Introjucer:

	LD_RUNPATH_SEARCH_PATHS="@loader_path/../Frameworks"

You will also need to add a `Post-build shell script` to copy the framework into the application bundle. Here is an example which also codesigns the framework:

	target=$CONFIGURATION_BUILD_DIR/$FULL_PRODUCT_NAME/Contents/Frameworks
	
	if [ ! -d "$target" ]; then
		mkdir "$target"
	fi
	
	cp -rf "/path/to/your/Frameworks/Sparkle.framework" "$target/Sparkle.framework"
	
	codesign -f -s "Signing Identity" "$target/Sparkle.framework/Versions/A/Sparkle

