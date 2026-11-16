cmake_minimum_required(VERSION 3.16)
project(PSBBVMedia LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_library(psbbv_media_catalogue STATIC
    src/Catalogue.cpp
    src/CatalogueStorage.cpp
)
target_include_directories(psbbv_media_catalogue PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_options(psbbv_media_catalogue PRIVATE -Wall -Wextra -Wpedantic)

add_executable(psbbv_media_catalogue_tests tests/CatalogueTests.cpp)
target_link_libraries(psbbv_media_catalogue_tests PRIVATE psbbv_media_catalogue)
target_compile_options(psbbv_media_catalogue_tests PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME psbbv_media_catalogue COMMAND psbbv_media_catalogue_tests)

add_executable(psbbv_media_storage_tests tests/CatalogueStorageTests.cpp)
target_link_libraries(psbbv_media_storage_tests PRIVATE psbbv_media_catalogue)
target_compile_options(psbbv_media_storage_tests PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME psbbv_media_storage COMMAND psbbv_media_storage_tests)
