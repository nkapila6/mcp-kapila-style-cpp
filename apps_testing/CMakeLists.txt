cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find all .cpp files in the current directory
file(GLOB TEST_SOURCES "*.cpp")

# Create an executable for each .cpp file
foreach(TEST_SOURCE ${TEST_SOURCES})
    # Get filename without extension (test.cpp → test)
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    # Create executable
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE mcp)
    target_link_libraries(${TEST_NAME} PRIVATE Eigen3::Eigen)
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    
    if(OPENSSL_FOUND)
        target_link_libraries(${TEST_NAME} PRIVATE ${OPENSSL_LIBRARIES})
    endif()
endforeach()