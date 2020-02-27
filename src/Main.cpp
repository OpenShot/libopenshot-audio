/**
 * @file
 * @brief Source code to play a test sound
 * @author Jonathan Thomas <jonathan@openshot.org>
 *
 * @section LICENSE
 *
 * Copyright (c) 2008-2016 OpenShot Studios, LLC
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
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
int main()
{

	// Initialize audio devices
	std::cout << "Begin init" << std::endl;
	AudioDeviceManager deviceManager;
	String error = deviceManager.initialise (
		0, /* number of input channels */
		2, /* number of output channels */
		0, /* no XML settings.. */
		true  /* select default device on failure */
	);

	// Output error (if any)
	if (error.isNotEmpty()) {
		std::cout << "Error on initialise(): " << error.toStdString() << std::endl;
	}

	// Play test sound
	std::cout << "Playing sound now" << std::endl;
	deviceManager.playTestSound();
	for (int x = 1; x <= 5; x++) {
		std::cout << "... " << x << std::endl;
		juce::Thread::sleep(1000);
	}

	std::cout << "before device loop" << std::endl;
	for (int i = 0; i < deviceManager.getAvailableDeviceTypes().size(); ++i)
	{
		const AudioIODeviceType* t = deviceManager.getAvailableDeviceTypes()[i];
		const StringArray deviceNames = t->getDeviceNames ();

		for (int j = 0; j < deviceNames.size (); ++j )
		{
			const String deviceName = deviceNames[j];
			String menuName;

			menuName << deviceName << " (" << t->getTypeName() << ")";
			std::cout << menuName << std::endl;
		}
	}
	std::cout << "after device loop" << std::endl;

	// Stop audio devices
	deviceManager.closeAudioDevice();
	deviceManager.removeAllChangeListeners();
	deviceManager.dispatchPendingMessages();

	return 0;
}
