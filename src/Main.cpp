/**
 * @file
 * @brief Source code to play a test sound
 * @author Jonathan Thomas <jonathan@openshot.org>
 *
 * @section LICENSE
 *
 * Copyright (c) 2008-2014 OpenShot Studios, LLC
 * <http://www.openshotstudios.com/>. This file is part of OpenShot Audio
 * Library (libopenshot-audio), an open-source project dedicated to delivering
 * high quality audio editing and playback solutions to the world. For more
 * information visit <http://www.openshot.org/>.
 *
 * OpenShot Audio Library (libopenshot-audio) is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * OpenShot Audio Library (libopenshot-audio) is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenShot Library. If not, see <http://www.gnu.org/licenses/>.
 *
 * @mainpage OpenShot Audio Library C++ API
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
