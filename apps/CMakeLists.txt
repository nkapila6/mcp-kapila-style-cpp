# Main executable
add_executable(mcp_kapila_style mcp_kapila_style.cpp)
target_link_libraries(mcp_kapila_style PRIVATE mcp)
target_include_directories(mcp_kapila_style PRIVATE ${CMAKE_SOURCE_DIR}/include)

# SSL support 
if(OPENSSL_FOUND)
    target_link_libraries(mcp_kapila_style PRIVATE ${OPENSSL_LIBRARIES})
endif()

set_target_properties(mcp_kapila_style PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)