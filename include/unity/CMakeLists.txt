add_subdirectory(scopes)

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

set(UNITY_SCOPES_LIB_HDRS ${UNITY_SCOPES_LIB_HDRS} ${headers} PARENT_SCOPE)
