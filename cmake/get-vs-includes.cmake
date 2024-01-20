# this is horrible and a piece of fucking shit
#  but it's the only way I was able to actually get
#  implicit include directories under Visual Studio generator
#  keep in mind this will not work under VS2015-
# We need this bullshit for h2inc

execute_process(COMMAND
    "C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe" -nologo -nocolor -format text -property installationPath
    OUTPUT_VARIABLE VS_INSTALL
    RESULT_VARIABLE RES_VARIABLE
    OUTPUT_STRIP_TRAILING_WHITESPACE)

if (NOT "${RES_VARIABLE}" STREQUAL "0")
    message(FATAL_ERROR "Cannot get Visual Studio installation directory")
endif()

string(SUBSTRING ${VS_INSTALL} 18 -1 INST_PATH)

execute_process(COMMAND
    cmd /c dump-inc.bat ${INST_PATH}
    OUTPUT_VARIABLE INCLUDES
    RESULT_VARIABLE RES_VARIABLE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)

if (NOT "${RES_VARIABLE}" STREQUAL "0")
    message(FATAL_ERROR "Cannot get Visual Studio include directories")
endif()

string(REGEX REPLACE "([A-Z]:)" \;\\1 INCLUDES ${INCLUDES})

set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES "${INCLUDES}" CACHE STRING "" FORCE)
