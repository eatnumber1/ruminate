cmake_minimum_required(VERSION 2.8)
project(RUMINATE)

set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wnonnull -Werror -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wnonnull -Werror -pedantic")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
set(DEBUGGER_CONTROLLER_PATH ${CMAKE_INSTALL_PREFIX}/bin/ruminate-dbgsvr)
OPTION(BUILD_SHARED_LIBS "Build Shared Libraries" ON)

find_package(GLIB2 REQUIRED)
find_package(Ice REQUIRED)

include_directories(
	${GLIB2_INCLUDE_DIRS}
)

add_subdirectory(ice)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(python)
