set(HEADER_FILES
    src/Config.h
    src/config.tab.h
)

set(SRC_FILES
    src/Config.cpp
    src/config.tab.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/dep)

add_library(config-plus STATIC ${SRC_FILES})