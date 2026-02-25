cmake_minimum_required(VERSION 3.28)
project(antimony-event-sys VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 1. Dependencies (Submodules)
if(NOT TARGET sb::types)
    add_subdirectory(extern/antimony-types)
endif()
if(NOT TARGET sb::logger)
    add_subdirectory(extern/antimony-logger)
endif()

# 2. Define the library
add_library(antimony-event-sys STATIC 
    src/SignalManager.cpp
    include/event/Listen.hpp
    include/event/Listener.hpp
    include/event/Notifier.hpp
    include/system/SignalManager.hpp
    include/system/Signals.hpp
)

# 3. Configure Include Directories
target_include_directories(antimony-event-sys PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 4. Mandate C++23 for all consumers
target_compile_features(antimony-event-sys PUBLIC cxx_std_23)

# 5. Link against dependencies
target_link_libraries(antimony-event-sys PUBLIC sb::types sb::logger)

# 6. Create aliases
add_library(sb::event ALIAS antimony-event-sys)
add_library(sb::system ALIAS antimony-event-sys)

# Examples
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
