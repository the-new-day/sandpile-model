add_executable(${PROJECT_NAME}
    main.cpp 
    parsing/argparsing.cpp
    parsing/utils.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC parsing)
