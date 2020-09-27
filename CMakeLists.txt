cmake_minimum_required(VERSION 3.10)

project(cpp_template_gear LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRE ON)
set(CMAKE_CXX_STANDARD 17)

add_library(cpp_template_gear INTERFACE)
add_library(CPPTemplateGear::CPPTemplateGear ALIAS cpp_template_gear)

target_include_directories(
	cpp_template_gear
	INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include
)

add_subdirectory(test)
