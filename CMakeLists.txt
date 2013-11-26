cmake_minimum_required(VERSION 2.8)
project(ruminate)

set(CMAKE_MODULE_PATH
	"${CMAKE_MODULE_PATH}"
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)
#set(COMMON_FLAGS "-Wall -Wextra -Wnonnull -Werror -Wcast-qual -Wwrite-strings -Wconversion -Wformat=2 -Wstrict-overflow=5 -pedantic -Weverything -Wno-padded -Wno-unreachable-code -Wno-covered-switch-default -Wno-switch-enum")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -Wno-vla -Wno-vla-extension -Wno-variadic-macros")

#set(COMMON_FLAGS "-Wall -Wextra -Wnonnull -Werror -pedantic")
#set(COMMON_FLAGS "-Wall -Wextra -Wnonnull -pedantic -fsanitize=address -fsanitize=undefined")
set(COMMON_FLAGS "-Wall -Wextra -Wnonnull -pedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -Wno-vla -Wno-vla-extension -Wno-variadic-macros")
#set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -fsanitize=address -fsanitize=undefined")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
set(DEBUGGER_CONTROLLER_PATH "${CMAKE_INSTALL_PREFIX}/bin/ruminate-dbgsvr")
option(BUILD_SHARED_LIBS "Build Shared Libraries" ON)

find_package(GLIB2 REQUIRED)
find_package(Ice REQUIRED)
find_package(Jansson)

add_subdirectory(ice)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(python)
add_subdirectory(test)
add_subdirectory(util)
add_subdirectory(doc)
