file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_LIBRARY_ARCHITECTURE}")
add_executable(testscopeB testscopeB.cpp)
target_link_libraries(testscopeB ${UNITY_SCOPES_LIB})
set_target_properties(testscopeB
  PROPERTIES
    PREFIX "${CMAKE_LIBRARY_ARCHITECTURE}/"
)
configure_file(testscopeB.ini.in testscopeB.ini)
