cmake_minimum_required(VERSION 3.21)
project(RStudio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Test)

set(CORE_SOURCES src/Logger.cpp src/Database.cpp src/Theme.cpp src/RecordingEngine.cpp src/SceneModel.cpp)

add_library(rstudio_core STATIC ${CORE_SOURCES})
target_link_libraries(rstudio_core PUBLIC Qt6::Core Qt6::Sql Qt6::Widgets)
target_include_directories(rstudio_core PUBLIC src)
target_compile_definitions(rstudio_core PUBLIC RSTUDIO_VERSION="${PROJECT_VERSION}")

add_executable(RStudio WIN32 src/main.cpp src/MainWindow.cpp src/Pages.cpp)
target_link_libraries(RStudio PRIVATE rstudio_core)

enable_testing()
add_executable(test_logger tests/test_logger.cpp)
target_link_libraries(test_logger PRIVATE rstudio_core Qt6::Test)
add_test(NAME logger_redaction COMMAND test_logger)

add_executable(test_recording tests/test_recording.cpp)
target_link_libraries(test_recording PRIVATE rstudio_core Qt6::Test)
add_test(NAME recording_args COMMAND test_recording)

install(TARGETS RStudio RUNTIME DESTINATION .)
