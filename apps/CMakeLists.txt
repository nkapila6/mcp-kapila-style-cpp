# Main executable
add_executable(mcp_openvto mcp_openvto.cpp)
target_link_libraries(mcp_openvto PRIVATE mcp)
target_include_directories(mcp_openvto PRIVATE ${CMAKE_SOURCE_DIR}/include)

# SSL support 
if(OPENSSL_FOUND)
    target_link_libraries(mcp_openvto PRIVATE ${OPENSSL_LIBRARIES})
endif()

set_target_properties(mcp_openvto PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)