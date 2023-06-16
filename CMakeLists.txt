
cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 20)
set(TARGETNAME "EcsX")
project(EcsX)

macro( add_recursive dir retVal )
	file( GLOB ${retVal} ${dir}/*.h ${dir}/*.cpp ${dir}/*.c )
endmacro()

function( add_filtered_h filterName fileString )
	file(GLOB TMP_FILES ${fileString})
	set(HEADER_FILES ${HEADER_FILES} ${TMP_FILES} PARENT_SCOPE)
	source_group(${filterName} FILES ${TMP_FILES})
endfunction( add_filtered_h )

function( add_filtered_src filterName fileString )
	file(GLOB TMP_FILES ${fileString})
	set(SOURCE_FILES ${SOURCE_FILES} ${TMP_FILES} PARENT_SCOPE)
	source_group(${filterName} FILES ${TMP_FILES})
endfunction( add_filtered_src )

function( add_filtered_std relativePath )
	string(REPLACE "/" "\\" filterPart ${relativePath})
	add_filtered_h("Header Files\\${filterPart}" "${CMAKE_CURRENT_SOURCE_DIR}/${relativePath}/*.h")
	add_filtered_h("Header Files\\${filterPart}" "${CMAKE_CURRENT_SOURCE_DIR}/${relativePath}/*.hpp")
	add_filtered_h("Header Files\\${filterPart}" "${CMAKE_CURRENT_SOURCE_DIR}/${relativePath}/*.inl")
	set(HEADER_FILES ${HEADER_FILES} PARENT_SCOPE)
	add_filtered_src("Source Files\\${filterPart}" "${CMAKE_CURRENT_SOURCE_DIR}/${relativePath}/*.cpp")
	set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
endfunction( add_filtered_std )

add_recursive( ./ SOURCE_FILES )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Util)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

add_filtered_std("Util")
add_filtered_std("Util/Traits")

add_filtered_std("ThirdParty")
add_filtered_std("ThirdParty/core")
add_filtered_std("ThirdParty/core/algorithm")
add_filtered_std("ThirdParty/utility")

add_executable(${TARGETNAME} ${SOURCE_FILES} ${HEADER_FILES})