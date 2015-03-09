set(HEADER_FILES
    src/Config.h
)

set(SRC_FILES
    src/Config.cpp
)

#include_directories(${CMAKE_SOURCE_DIR}/dep)

add_library(configplus ${SRC_FILES} ${HEADER_FILES})