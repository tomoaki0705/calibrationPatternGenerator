# minimum requirement
cmake_minimum_required(VERSION 3.3)

# solution
project(powerPointGenerator)

SET(TARGET_NAME pptGenerate)

# find OpenCV
find_package(OpenCV REQUIRED)

# set include directory
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR})

set(EXTRACTED_TEMPLATE        ${CMAKE_BINARY_DIR}/template)
set(TEMPLATE_SOURCE           ${CMAKE_SOURCE_DIR}/templates/)
set(PACKED_TEMPLATE           ${TEMPLATE_SOURCE}/pptx)
set(GENERATED_FILE            generated)
set(OUTPUT_XML_FILENAME       "slide1.xml")
set(OUTPUT_THUMBNAIL_FILENAME "thumbnail.jpeg")

# build TARGET_NAME from source code
add_executable(${TARGET_NAME} main.cpp config.h)

if(MSVC)
    # set TARGET_NAME as start up property
    set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "/wd4819 /wd4996 /wd4710 /wd4514 /wd4350 /wd4820 /wd4365 /wd4625 /wd4626 /W4")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()


# set the link of OpenCV
target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS})

macro(ADD_POST_BUILD _TARGET _COMMAND)
    add_custom_command(TARGET ${_TARGET} POST_BUILD COMMAND ${_COMMAND} ${ARGN})
endmacro()

set(CM ${CMAKE_COMMAND})

ADD_POST_BUILD(${TARGET_NAME} ${TARGET_NAME})
ADD_POST_BUILD(${TARGET_NAME} ${CM} -E remove_directory ${EXTRACTED_TEMPLATE}) 
ADD_POST_BUILD(${TARGET_NAME} ${CM} -E make_directory ${EXTRACTED_TEMPLATE})
ADD_POST_BUILD(${TARGET_NAME} ${CM} -E copy_directory ${PACKED_TEMPLATE} ${EXTRACTED_TEMPLATE})
ADD_POST_BUILD(${TARGET_NAME} ${CM} -E copy ${OUTPUT_XML_FILENAME} ${EXTRACTED_TEMPLATE}/ppt/slides/)
ADD_POST_BUILD(${TARGET_NAME} ${CM} -E copy ${OUTPUT_THUMBNAIL_FILENAME} ${EXTRACTED_TEMPLATE}/docProps/)
ADD_POST_BUILD(${TARGET_NAME} ${CM} -E remove -f ${CMAKE_BINARY_DIR}/${GENERATED_FILE}.zip)
ADD_POST_BUILD(${TARGET_NAME} ${CM} -E chdir ${EXTRACTED_TEMPLATE} ${CM} -E tar cf ${CMAKE_BINARY_DIR}/${GENERATED_FILE}.zip --format=zip [Content_Types].xml _rels docProps ppt)
ADD_POST_BUILD(${TARGET_NAME} ${CM} -E rename ${CMAKE_BINARY_DIR}/${GENERATED_FILE}.zip ${CMAKE_BINARY_DIR}/${GENERATED_FILE}.pptx)
