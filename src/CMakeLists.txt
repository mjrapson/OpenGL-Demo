add_executable (OpenGLDemo)

target_sources(OpenGLDemo
PRIVATE
	${CMAKE_SOURCE_DIR}/3rd/glad/include/glad/gl.h
    ${CMAKE_SOURCE_DIR}/3rd/glad/include/KHR/khrplatform.h
    ${CMAKE_SOURCE_DIR}/3rd/glad/src/gl.c
    application/Application.cpp
    application/Application.h
    application/Window.cpp
    application/Window.h
    core/FileSystem.cpp
    core/FileSystem.h
    core/Vertex.h
    rendering/Buffer.h
    rendering/Renderer.cpp
    rendering/Renderer.h
    rendering/Shader.cpp
    rendering/Shader.h
	main.cpp
)

target_include_directories(OpenGLDemo
PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/3rd/glad/include
	${CMAKE_SOURCE_DIR}/3rd/glm
	${CMAKE_SOURCE_DIR}/3rd/stb
)

target_link_libraries(OpenGLDemo
PRIVATE
	OpenGL::GL
	glfw
)

add_custom_target(post_build ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources/shaders ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders
)

add_dependencies(post_build OpenGLDemo)