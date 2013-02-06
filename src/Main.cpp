/**
 * \file
 * \brief Source code to play a test sound
 * \author Copyright (c) 2011 Jonathan Thomas
 */

/**
 * \mainpage OpenShot Audio Library C++ API
 *
 * Welcome to the OpenShot Audio Library C++ API.  This library is used by libopenshot to enable audio
 * features, which powers the <a href="http://www.openshot.org">OpenShot Video Editor</a> application.
 */
 
#include <iostream>
#include <unistd.h>
#include "../JuceLibraryCode/JuceHeader.h"

#if JUCE_MINGW
	#define sleep(a) Sleep(a * 1000)
	#include <windows.h>
#endif

using namespace std;

//==============================================================================
int main()
{

	// Initialize audio devices
	AudioDeviceManager deviceManager;
	deviceManager.initialise (	0, /* number of input channels */
								2, /* number of output channels */
								0, /* no XML settings.. */
								true  /* select default device on failure */);

	// Play test sound
	deviceManager.playTestSound();

	// Sleep for 2 seconds
	cout << "Playing test sound now..." << endl;
	sleep(2);

	// Stop audio devices
    deviceManager.closeAudioDevice();
    deviceManager.removeAllChangeListeners();
    deviceManager.dispatchPendingMessages();

    return 0;
}
