add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(parsing)
add_subdirectory(model)

target_link_libraries(${PROJECT_NAME} PRIVATE parsing model)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(parsing PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(model PUBLIC ${CMAKE_CURRENT_LIST_DIR})
