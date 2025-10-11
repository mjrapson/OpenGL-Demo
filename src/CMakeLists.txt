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
    behaviours/CameraMoveBehaviour.cpp
    behaviours/CameraMoveBehaviour.h
    behaviours/OscillationAnimationBehaviour.cpp
    behaviours/OscillationAnimationBehaviour.h
    core/Container.h
    core/FileSystem.cpp
    core/FileSystem.h
    core/Vertex.h
    data/AssetDatabase.cpp
    data/AssetDatabase.h
    data/Box.cpp
    data/Box.h
    data/DirectionalLight.h
    data/Material.h
    data/Mesh.cpp
    data/Mesh.h
    data/MeshFactory.cpp
    data/MeshFactory.h
    data/MeshInstance.h
    data/PointLight.h
    data/Ray.h
    data/Sphere.cpp
    data/Sphere.h
    data/Texture.cpp
    data/Texture.h
    input/InputHandler.cpp
    input/InputHandler.h
    loaders/GltfLoader.cpp
    loaders/GltfLoader.h
    loaders/SceneLoader.cpp
    loaders/SceneLoader.h
    loaders/TextureLoader.cpp
    loaders/TextureLoader.h
    physics/Collision.cpp
    physics/Collision.h
    rendering/renderpasses/DirectionalShadowRenderPass.cpp
    rendering/renderpasses/DirectionalShadowRenderPass.h
    rendering/renderpasses/GBufferRenderPass.cpp
    rendering/renderpasses/GBufferRenderPass.h
    rendering/renderpasses/LightingRenderPass.cpp
    rendering/renderpasses/LightingRenderPass.h
    rendering/renderpasses/PointLightShadowRenderPass.cpp
    rendering/renderpasses/PointLightShadowRenderPass.h
    rendering/Buffer.h
    rendering/Camera.h
    rendering/DrawCommand.h
    rendering/Framebuffer.cpp
    rendering/Framebuffer.h
    rendering/LightTransform.cpp
    rendering/LightTransform.h
    rendering/MeshBuffer.cpp
    rendering/MeshBuffer.h
    rendering/Renderer.cpp
    rendering/Renderer.h
    rendering/RenderPass.h
    rendering/Shader.cpp
    rendering/Shader.h
    rendering/VertexLayout.cpp
    rendering/VertexLayout.h
    world/components/BehaviourComponent.h
    world/components/CameraComponent.cpp
    world/components/CameraComponent.h
    world/components/DirectionalLightComponent.h
    world/components/MeshRenderingComponent.h
    world/components/PointLightComponent.h
    world/components/TransformComponent.h
    world/systems/BehaviourSystem.cpp    
    world/systems/BehaviourSystem.h
    world/systems/LightingSystem.cpp
    world/systems/LightingSystem.h
    world/systems/RenderSystem.cpp
    world/systems/RenderSystem.h
    world/Behaviour.h
    world/Entity.h
    world/SpatialTree.cpp
    world/SpatialTree.h
    world/World.h
	main.cpp
)

target_include_directories(OpenGLDemo
PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/3rd/glad/include
	${CMAKE_SOURCE_DIR}/3rd/glm
    ${CMAKE_SOURCE_DIR}/3rd/nlohmann/include
	${CMAKE_SOURCE_DIR}/3rd/stb
    ${CMAKE_SOURCE_DIR}/3rd/tinygltf
)

target_link_libraries(OpenGLDemo
PRIVATE
	OpenGL::GL
	glfw
)

add_custom_target(post_build ALL 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources/scenes ${CMAKE_CURRENT_BINARY_DIR}/resources/scenes
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources/shaders ${CMAKE_CURRENT_BINARY_DIR}/resources/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources/textures ${CMAKE_CURRENT_BINARY_DIR}/resources/textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources/data ${CMAKE_CURRENT_BINARY_DIR}/resources/data
)

add_dependencies(post_build OpenGLDemo)
