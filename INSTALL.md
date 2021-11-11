# The OpenShot Audio Library

## Getting Started

The best way to get started with libopenshot-audio is to
learn about our build system, obtain all the source code,
install a development IDE and tools,
and better understand our dependencies.
So, please read through the following sections and follow the instructions.
And keep in mind, every computer system is different.
Keep an eye out for subtle file path differences in the commands you type.

## Build Tools

CMake is the backbone of our build system.
It is a cross-platform build system, which checks for dependencies,
locates header files and libraries, generates makefiles,
and supports the cross-platform compiling of libopenshot and libopenshot-audio.
CMake uses an out-of-source build concept where all temporary build files,
such as makefiles, object files, and even the final binaries,
are created outside of the source code folder.
(A subfolder named `build/` is a common convention, demonstrated here.)
This prevents the build process from cluttering up the source code.

These instructions have only been tested with
the GNU compiler (including MSYS2/MinGW for Windows)
and the Clang compiler (including AppleClang).

## Dependencies

The following libraries are required to build libopenshot-audio.
Instructions on how to install these dependencies vary by operating system.
Libraries and Executables have been labeled in the list below,
to help distinguish between them.

### Build dependencies

#### CMake

*   <http://www.cmake.org/> (**Executable**)

*   Used to automate the generation of Makefiles, check for dependencies,
    and is the backbone of libopenshot-audio’s cross-platform build process.

### Documentation dependencies

Only required if you wish to build a local copy of the API documentation.

#### Doxygen

*   <http://doxygen.nl/> (**Executable**)
*   Used to auto-generate the documentation used by libopenshot-audio.

### OS-specific dependencies

#### ALSA - Required, Linux only

*   `libalsa.so` (**Library**)
*   Audio hardware interface library, install with OS package manager

#### ASIO SDK - Optional, Windows only

*   <https://new.steinberg.net/developers/> ("ASIO SDK" download link) (**Library**)

*   Optional audio interface library.

*   When building, tell CMake where to find the SDK by either:

    1.  Setting the environment variable `ASIO_SDK_DIR`
        to the full path of the extracted SDK.

        Example: `ASIO_SDK_DIR="C:\Program Files\asiosdk_2.3.3_2019-06-14"`

    2.  Setting the path on the CMake command line, using `ASIO_ROOT`.

        Example:
        `cmake -DASIO_ROOT="C:\Users\Owner\Downloads\asiosdk_2.3.3_2019-06-14"`

## Obtaining Source Code

The first step in installing libopenshot-audio is to obtain the source code.
The source code is available on
[GitHub](https://github.com/OpenShot/libopenshot-audio).
Open a terminal and use the following command to obtain the latest source:

```sh
git clone https://github.com/OpenShot/libopenshot-audio.git
```

## Linux Build Instructions (libopenshot-audio)

Enter the source directory downloaded by git, and configure the `build` directory:

```sh
cd libopenshot-audio

# Add any other configuration to the next command. Some common options:
#   -DCMAKE_INSTALL_PREFIX=/usr
#   -DASIO_ROOT="C:\Program Files\ASIO SDK"
cmake -B build -S .
```

Once the build is successfully configured, compile the library:

```sh
cmake --build build
```

If there are no errors, you can test the new build using the demo program.
It will list the audio devices detected on your system, then use the
default device to play a series of short test tones:

```sh
./build/src/openshot-audio-demo
```

If everything is working as expected, you may install the library by running:

```sh
# (This installs to the default prefix configured in CMake,
# unless you changed CMAKE_INSTALL_PREFIX above)
cmake --install build
```

If your next step is to build libopenshot, installation is optional here.
libopenshot-audio can be used from the build directory by adding
`-DOpenShotAudio_ROOT="/path/to/libopenshot-audio/build"`
to the `cmake` command line, when configuring libopenshot.
