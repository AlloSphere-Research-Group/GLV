cmake_minimum_required(VERSION 2.8)
project(GLV)

# Set window binding and openGL ES usage here
set(WINDOW_BINDING GLUT)
set(USE_OPENGL_ES 0)

set(WINDOW_BINDING_SRC src/glv_binding.cpp)

if(${USE_OPENGL_ES} MATCHES 1)
add_definitions(-DGLV_USE_OPENGL_ES)
endif(${USE_OPENGL_ES} MATCHES 1)

# GLV Library
set(GLV_SRC src/glv_buttons.cpp
        src/glv_color.cpp
        src/glv_color_controls.cpp
        src/glv_core.cpp
        src/glv_draw.cpp
        src/glv_font.cpp
        src/glv_glv.cpp
        src/glv_grid.cpp
        src/glv_inputdevice.cpp
        src/glv_layout.cpp
        src/glv_model.cpp
        src/glv_notification.cpp
        src/glv_plots.cpp
        src/glv_preset_controls.cpp
        src/glv_sliders.cpp
        src/glv_sono.cpp
        src/glv_texture.cpp
        src/glv_textview.cpp
        src/glv_view.cpp
        src/glv_view3D.cpp
        src/glv_widget.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLV)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

# Dependencies

find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${GLEW_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR})

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  find_library(AGL_FM AGL)
  find_library(OPENGL_FM OpenGL)
  set(GLV_LINK_LIBRARIES ${OPENGL_FM} ${AGL_FM})
  if(${WINDOW_BINDING} MATCHES "GLUT")
    message("USING GLUT")
    set(WINDOW_BINDING_SRC ${WINDOW_BINDING_SRC} src/glv_binding_glut.cpp)
    find_library(GLUT_FM GLUT)
    set(GLV_LINK_LIBRARIES ${GLV_LINK_LIBRARIES} GLUT_FM)
  endif(${WINDOW_BINDING} MATCHES "GLUT")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(GLV_LINK_LIBRARIES GLEW GLEW GL)
  if(${WINDOW_BINDING} MATCHES "GLUT")
    message("USING GLUT")
    set(WINDOW_BINDING_SRC ${WINDOW_BINDING_SRC} src/glv_binding_glut.cpp)
    set(GLV_LINK_LIBRARIES ${GLV_LINK_LIBRARIES} glut)
  endif(${WINDOW_BINDING} MATCHES "GLUT")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_library(GLV ${GLV_SRC} ${WINDOW_BINDING_SRC})
target_link_libraries(GLV ${GLV_LINK_LIBRARIES})

# Examples
file(GLOB EXAMPLE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} example/*.cpp)
foreach(example ${EXAMPLE_FILES})
  get_filename_component(EXAMPLE_NAME ${example} NAME_WE) # Get name w/o extension
  add_executable(${EXAMPLE_NAME} ${example})
  target_link_libraries(${EXAMPLE_NAME} GLV ${GLV_LINK_LIBRARIES})
endforeach(example)

# installation
install(DIRECTORY GLV/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GLV)
install(TARGETS GLV DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
