file(GLOB_RECURSE sources *.cpp *.h)

include_directories(
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(${include_Dirs})

add_executable(SimpleWoW
  ${sources}
)

target_link_libraries(SimpleWoW
)

#add_dependencies(SimpleWoW)

#if( UNIX )
#  install(TARGETS Tests DESTINATION bin)
#elseif( WIN32 )
#  install(TARGETS Tests DESTINATION "${CMAKE_INSTALL_PREFIX}")
#endif()