############### TEST EXECUTABLES ################

add_executable(openshot-audio-test-sound Main.cpp)
target_link_libraries(openshot-audio-test-sound openshot-audio)

# Install Test Executable
INSTALL(TARGETS openshot-audio-test-sound DESTINATION bin)