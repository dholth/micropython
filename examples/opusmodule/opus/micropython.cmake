# Create an INTERFACE library for our C module.
add_library(usermod_opus INTERFACE)

# Add our source files to the lib
target_sources(usermod_opus INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/examplemodule.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_opus INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# build with libopus
include(FetchContent)
FetchContent_Declare(arduino_libopus GIT_REPOSITORY "https://github.com/pschatzmann/arduino-libopus.git" GIT_TAG main )
FetchContent_GetProperties(arduino_libopus)
if(NOT arduino_libopus_POPULATED)
    FetchContent_Populate(arduino_libopus)
    add_subdirectory(${arduino_libopus_SOURCE_DIR})
endif()

target_link_libraries(usermod INTERFACE arduino_libopus)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_opus)
