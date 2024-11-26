cmake_minimum_required(VERSION 3.10)

project(CppDatabase VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define directories for include and source files
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/tests")

# Add include directory
include_directories(${INCLUDE_DIR})

# Source files
set(SOURCES
        ${SRC_DIR}/database.cpp
        ${SRC_DIR}/table.cpp
        ${SRC_DIR}/row.cpp
        ${SRC_DIR}/query_executor.cpp
        ${SRC_DIR}/serializer.cpp
        ${SRC_DIR}/data_types.cpp
        ${SRC_DIR}/expression.cpp
)

# Include headers
file(GLOB HEADERS "${INCLUDE_DIR}/*.h")

# Create the library target
add_library(InMemoryDatabase STATIC ${SOURCES} ${HEADERS})

# Enable warnings for the project
if (MSVC)
    target_compile_options(InMemoryDatabase PRIVATE /W4 /WX)
else()
    target_compile_options(InMemoryDatabase PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Installation rules
install(TARGETS InMemoryDatabase
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY ${INCLUDE_DIR}/ DESTINATION include)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE InMemoryDatabase)