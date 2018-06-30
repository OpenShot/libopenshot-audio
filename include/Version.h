/**
 * @file
 * @brief Header file that includes the version number of libopenshot-audio
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
 */

#ifndef OPENSHOT_AUDIO_VERSION_H
#define OPENSHOT_AUDIO_VERSION_H

// Crazy c++ macro to convert an integer into a string
#ifndef STRINGIZE
	#define STRINGIZE_(x) #x
	#define STRINGIZE(x) STRINGIZE_(x)
#endif

#define OPENSHOT_AUDIO_VERSION_MAJOR 0;	/// Major version number is incremented when huge features are added or improved.
#define OPENSHOT_AUDIO_VERSION_MINOR 1;	/// Minor version is incremented when smaller (but still very important) improvements are added.
#define OPENSHOT_AUDIO_VERSION_BUILD 6;	/// Build number is incremented when minor bug fixes and less important improvements are added.
#define OPENSHOT_AUDIO_VERSION_SO 6;	/// Shared object version number. This increments any time the API and ABI changes (so old apps will no longer link)
#define OPENSHOT_AUDIO_VERSION_MAJOR_MINOR STRINGIZE(OPENSHOT_AUDIO_VERSION_MAJOR) "." STRINGIZE(OPENSHOT_AUDIO_VERSION_MINOR); /// A string of the "Major.Minor" version
#define OPENSHOT_AUDIO_VERSION_ALL STRINGIZE(OPENSHOT_AUDIO_VERSION_MAJOR) "." STRINGIZE(OPENSHOT_AUDIO_VERSION_MINOR) "." STRINGIZE(OPENSHOT_AUDIO_VERSION_BUILD); /// A string of the entire version "Major.Minor.Build"

#include <sstream>
using namespace std;

/// This struct holds version number information. Use the GetVersion() method to access the current version of libopenshot.
struct OpenShotAudioVersion
{
	int major; /// Major version number
	int minor; /// Minor version number
	int build; /// Build number
	int so; /// Shared Object Number (incremented when API or ABI changes)

	/// Get a string version of the version (i.e. "Major.Minor.Build")
	string ToString() {
		stringstream version_string;
		version_string << major << "." << minor << "." << build;
		return version_string.str();
	}
};

/// Get the current version number of libopenshot (major, minor, and build number)
static OpenShotAudioVersion GetVersion()
{
	OpenShotAudioVersion version;

	// Set version info
	version.major = OPENSHOT_AUDIO_VERSION_MAJOR;
	version.minor = OPENSHOT_AUDIO_VERSION_MINOR;
	version.build = OPENSHOT_AUDIO_VERSION_BUILD;
	version.so = OPENSHOT_AUDIO_VERSION_SO;

	return version;
}

#endif
