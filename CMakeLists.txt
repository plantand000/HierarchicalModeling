CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Name of the project
PROJECT(Assignment2)

# Get the list of all files.
FILE(GLOB_RECURSE SOURCES "src/*.cpp")
FILE(GLOB_RECURSE HEADERS "src/*.h")
FILE(GLOB_RECURSE GLSL "shaders/*.glsl")

# Set the executable.
ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${GLSL})

# Setup GLM
SET(GLM_INCLUDE_DIR "$ENV{GLM_INCLUDE_DIR}")
INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIR})

# Setup GLFW
SET(GLFW_DIR "$ENV{GLFW_DIR}")
OPTION(GLFW_BUILD_EXAMPLES "GLFW_BUILD_EXAMPLES" OFF)
OPTION(GLFW_BUILD_TESTS "GLFW_BUILD_TESTS" OFF)
OPTION(GLFW_BUILD_DOCS "GLFW_BUILD_DOCS" OFF)
IF(CMAKE_BUILD_TYPE MATCHES Release)
	ADD_SUBDIRECTORY(${GLFW_DIR} ${GLFW_DIR}/release)
ELSE()
	ADD_SUBDIRECTORY(${GLFW_DIR} ${GLFW_DIR}/debug)
ENDIF()
INCLUDE_DIRECTORIES(${GLFW_DIR}/include)
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})

# Setup GLEW
SET(GLEW_DIR "$ENV{GLEW_DIR}")
INCLUDE_DIRECTORIES(${GLEW_DIR}/include)
IF(WIN32)
	# With prebuilt binaries
	TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${GLEW_DIR}/lib/Release/Win32/glew32s.lib)
ELSE()
	TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${GLEW_DIR}/lib/libGLEW.a)
ENDIF()

# OS specific options and libraries
IF(WIN32)
	# c++11 is enabled by default.
	# -Wall produces way too many warnings.
	# -pedantic is not supported.
	# Disable warning 4996.
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")
	TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} opengl32.lib)
ELSE()
	# Enable all pedantic warnings.
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")
	IF(APPLE)
		# Add required frameworks for GLFW.
		TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	ELSE()
		#Link the Linux OpenGL library
		TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} "GL")
	ENDIF()
ENDIF()
