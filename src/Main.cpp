/**
 * @file
 * @brief Example code to list available devices and play a test sound
 * @author Jonathan Thomas <jonathan@openshot.org>
 * @author FeRD (Frank Dana) <ferdnyc@gmail.com>
 *
 * Copyright (c) 2008-2020 OpenShot Studios, LLC
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
 * Welcome to the OpenShot Audio Library C++ API.
 * This library is used by libopenshot to enable audio features, which powers the
 * <a href="http://www.openshot.org">OpenShot Video Editor</a> application.
 */

#include <iostream>
#include <thread>
#include <chrono>

// Avoid an implicit (and deprecated) 'using namespace juce;'
#ifndef DONT_SET_USING_JUCE_NAMESPACE
    #define DONT_SET_USING_JUCE_NAMESPACE 1
#endif
#include "OpenShotAudio.h"


void display_device_probe(juce::AudioDeviceManager& deviceManager) {
    std::cout << "Audio device probe results:\n";
    int device_count = 0;
    for (const auto& t : deviceManager.getAvailableDeviceTypes()) {
        juce::String typeString;
        typeString << "Type " << t->getTypeName() << ":";
        std::cout << typeString << std::endl;
        auto deviceNames = t->getDeviceNames();
        device_count += deviceNames.size();
        for (const auto deviceName : deviceNames) {
            std::cout << "  - " << deviceName << std::endl;
        }
    }
    std::cout << "Discovered " << device_count << " total audio devices.\n";
}


int main(int argc, char* argv[])
{
    juce::ignoreUnused (argc, argv);

    // Initialize default audio device
    std::cout << "Initialising audio playback device.\n";
    juce::AudioDeviceManager deviceManager;

    auto error = deviceManager.initialise (
        0,        // numInputChannelsNeeded
        2,        // numOutputChannelsNeeded
        nullptr,  // savedState
        true      // selectDefaultDeviceOnFailure
    );

    // Output error (if any)
    if (error.isNotEmpty()) {
        std::cout << "Error on initialise(): " << error.toStdString() << std::endl;
        exit(-1);
    }

    // Show detected audio devices
    display_device_probe(deviceManager);

    // Pause to take all that in
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Play test sound

    const int pause(2);
    using stride = std::chrono::duration<double, std::ratio<pause, 1>>;

    std::cout << "\nYou should hear five test tones, precisely "
              << pause << " second" << (pause > 1 ? "s" : "")
              << " apart.\n";

    auto start = std::chrono::steady_clock::now();

    for (auto s = static_cast<stride>(1); s < static_cast<stride>(6); ++s) {
        std::cout << s.count() << (s.count() < 5 ? "... " : ".") << std::flush;
        deviceManager.playTestSound();
        std::this_thread::sleep_until(start + s);
    }

    std::cout << "\nDemo complete. Shutting down device manager.\n";

    // Stop audio devices
    deviceManager.closeAudioDevice();
    deviceManager.removeAllChangeListeners();
    deviceManager.dispatchPendingMessages();

    return 0;
}
