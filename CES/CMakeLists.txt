cmake_minimum_required (VERSION 2.8)
     
project (CES C CXX)

find_package(OpenGL REQUIRED)

add_executable(CES CES.cpp LGApp.cpp LGApp.h Board.h Enums.h Poisk.h Pools.h)
add_subdirectory (LizardGraphics)

IF (WIN32)
  target_link_libraries(CES  LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/soil2-debug LizardGraphics/libs/glew32s LizardGraphics/libs/freetype)
ELSE()
  target_link_libraries(CES  LizardGraphics opengl32 LizardGraphics/libs/glfw3 LizardGraphics/libs/soil2-debug LizardGraphics/libs/glew32 LizardGraphics/libs/freetype)
ENDIF()
