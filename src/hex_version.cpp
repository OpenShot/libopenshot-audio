/**
 * @file
 * @brief Build utility program to generate hex-format version numbers
 * @author FeRD (Frank Dana) <ferdnyc@gmail.com>
 *
 * @section LICENSE
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
 * Welcome to the OpenShot Audio Library C++ API.  This library is used by libopenshot to enable audio
 * features, which powers the <a href="http://www.openshot.org">OpenShot Video Editor</a> application.
 */

 #include <iostream>
 #include <ios>

// The following values must be defined at compile time:
// VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH

#if !defined(VERSION_MAJOR) || !defined(VERSION_MINOR) || !defined(VERSION_PATCH)
    #pragma error "Define version components on compiler command line!"
#endif

int main()
{
    
    int hex_version = (VERSION_MAJOR << 16) +
                      (VERSION_MINOR << 8) +
                      (VERSION_PATCH);
                      
    std::cout << std::hex << "0x" << hex_version;
}
