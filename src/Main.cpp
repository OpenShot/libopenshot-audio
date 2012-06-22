/**
 * \file
 * \brief Source code to play a test sound
 * \author Copyright (c) 2011 Jonathan Thomas
 */
 
#include <iostream>
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
