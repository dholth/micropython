# Create an INTERFACE library for our C module.
add_library(usermod_opus INTERFACE)

# Add our source files to the lib
target_sources(usermod_opus INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/opusmodule.c
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

# the arduino-libopus distribution doesn't include opusfile
include(FetchContent)
FetchContent_Declare(opusfile GIT_REPOSITORY "https://github.com/xiph/opusfile.git" GIT_TAG master )
FetchContent_GetProperties(opusfile)
if(NOT opusfile)
    FetchContent_Populate(opusfile)
    add_subdirectory(${opusfile_SOURCE_DIR})
endif()

target_compile_definition(opusfile PRIVATE -DOP_DISABLE_FLOAT_API=1 -DOP_FIXED_POINT=1)

target_link_libraries(usermod INTERFACE arduino_libopus opusfile)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_opus)
