cmake_minimum_required(VERSION 3.21)

project(iceengine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_PATH {$CMAKE_LIBRARY_PATH} ../external/linux)
#set(GCC_COMPILER_FLAGS "-O3 -std=c++2b")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COMPILER_FLAGS}")

add_library(iceengine SHARED
Utils/FinalAction.cpp
Utils/SDLTools.cpp
Utils/ScopedTimeMeasurement.cpp
ShadersGL/ModelShaderConfigurator.cpp
ShadersGL/SkyboxShaderConfigurator.cpp
ShadersGL/LightShaderConfigurator.cpp
ShadersGL/ShadowShaderConfigurator.cpp
ShadersGL/SunShaderConfigurator.cpp
ShadersGL/ShaderProgramGL.cpp
ShadersGL/TexDisplayShaderConfigurator.cpp
ShadersGL/UIElementShaderConfigurator.cpp
ShadersGL/WaterShaderConfigurator.cpp
ShadersGL/AnimatedModelShaderConfigurator.cpp
ShadersGL/Shader3dConfigurator.cpp
ShadersGL/ParticleShaderConfigurator.cpp
ShadersGL/PathSegmentShaderConfigurator.cpp
ShadersGL/TerrainShaderConfigurator.cpp
ShadersGL/TextShaderConfigurator.cpp
ShadersGL/WidgetShaderConfigurator.cpp
ShadersGL/TerrainHighlightShaderConfigurator.cpp
Entities/BaseEntityManager.cpp
Entities/EntityManager.cpp
Entities/EntityComponentBuffer.cpp
Entities/BaseComponentSystem.cpp
Animation/Animation.cpp
Animation/JumpAnimation.cpp
Animation/AnimationManager.cpp
Animation/GravityAnimation.cpp
Animation/MoveAnimation.cpp
Renderer/TextureManagerGL.cpp
Renderer/OpenGL/TerrainRendererGL.cpp
Renderer/OpenGL/ModelManagerGL.cpp
Renderer/OpenGL/ShadowMapRendererGL.cpp
Renderer/OpenGL/RenderToolsGL.cpp
Renderer/OpenGL/WaterFramebuffersGL.cpp
Renderer/OpenGL/ParticleRendererGL.cpp
Renderer/OpenGL/WaterRendererGL.cpp
Renderer/OpenGL/ModelRendererGL.cpp
Renderer/OpenGL/SkyboxRendererGL.cpp
Renderer/OpenGL/GraphicsSystemGL.cpp
Renderer/OpenGL/RenderObjectGL.cpp
Renderer/OpenGL/FramebufferObjectGL.cpp
Renderer/OpenGL/TextureGL.cpp
Renderer/OpenGL/AnimatedModelRendererGL.cpp
Renderer/OpenGL/CommonMatrixUBO_GL.cpp
Renderer/OpenGL/WidgetRendererGL.cpp
Renderer/OpenGL/PathSegmentRendererGL.cpp
Renderer/OpenGL/TextRendererGL.cpp
Renderer/OpenGL/SunRendererGL.cpp
Renderer/OpenGL/UniformBufferObjectGL.cpp
Renderer/OpenGL/VertexAttributeGL.cpp
Renderer/PostProcessing/PostProcessingPipeline.cpp
Renderer/PostProcessing/PostProcessingEffectFactory.cpp
Renderer/PostProcessing/OpenGL/BasePostProcessorGL.cpp
Renderer/PostProcessing/OpenGL/ContrastChangerGL.cpp
Renderer/PostProcessing/OpenGL/OriginalCanvasGL.cpp
Renderer/PostProcessing/OpenGL/GaussianBlurGL.cpp
Renderer/Frustum.cpp
Renderer/CubeTexture.cpp
Events/EventQueue.cpp
Components/ComponentManager.cpp
Components/MeshComponent.cpp
Components/RenderMaterialsComponent.cpp
Components/Systems/ObjectRenderingSystem.cpp
Components/Systems/ParticleSystemSystem.cpp
Components/Systems/InfectionSystem.cpp
Components/Systems/BuildingSystem.cpp
Components/Systems/SaveSystem.cpp
Components/Systems/PathAnimationSystem.cpp
Components/Systems/ModelManagerSystem.cpp
Components/Systems/AnimatedModelManagerSystem.cpp
Components/Systems/ModelAnimationSystem.cpp
Components/Systems/SkyboxRenderingSystem.cpp
Components/Systems/MasterRenderingSystem.cpp
Components/Systems/TerrainRenderingSystem.cpp
Components/Systems/CameraControllerSystem.cpp
Components/Systems/PathSegmentRenderingSystem.cpp
Components/Systems/MeshGroupSystem.cpp
Components/Systems/SunRenderingSystem.cpp
Components/Systems/EventHandlingSystem.cpp
Components/Systems/PathSegmentBlueprintSystem.cpp
Components/Systems/RoadNetworkSystem.cpp
Components/Systems/ToolTipSystem.cpp
Components/Systems/AnimatedModelRenderingSystem.cpp
Components/Systems/DayNightCycleSystem.cpp
Components/Systems/TerrainSystem.cpp
Components/Systems/WaterRenderingSystem.cpp
Components/TerrainComponent.cpp
Engine.cpp
Particles/Particle.cpp
Particles/ParticleSystem.cpp
System/Ray.cpp
System/Image.cpp
System/Scene.cpp
System/SystemServices.cpp
System/ModelInstance.cpp
System/Finally.cpp
System/Tools.cpp
System/Camera.cpp
System/Config.cpp
System/Model.cpp
System/StreamTools.cpp
System/PathSegment.cpp
System/AABB.cpp
System/thread_pool.cpp
System/Light.cpp
System/Skybox.cpp
System/RenderMaterial.cpp
System/BaseTexture.cpp
System/Path.cpp
System/File.cpp
System/Math.cpp
System/KdTree.cpp
GUI/Button.cpp
GUI/ImageView.cpp
GUI/ScrollWidget.cpp
GUI/CheckBox.cpp
GUI/TextEdit.cpp
GUI/TextLabel.cpp
GUI/Fonts/TextManager.cpp
GUI/Fonts/Word.cpp
GUI/Fonts/TextStyle.cpp
GUI/StackPanel.cpp
GUI/WidgetView.cpp
GUI/Padding.cpp
GUI/WidgetManager.cpp
GUI/Widget.cpp
GUI/MousePicker.cpp
GUI/ModelPreview.cpp
GUI/DropDown.cpp
GUI/ProgressBar.cpp
GUI/Slider.cpp
GUI/ListBox.cpp
GUI/Constraints/AlignmentConstraint.cpp
GUI/Constraints/UIConstraints.cpp
GUI/Constraints/TwoPointSizeConstraint.cpp
ModelAnimation/JointTransform.cpp
Importers/ModelImporterCollada.cpp
Importers/ModelImporterOBJ.cpp
World/LowPolyTerrain.cpp
World/World.cpp
World/Terrain.cpp
World/WaterTile.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

find_library(SDL2 SDL2)
find_library(GLEW GLEW)
find_library(crossguid crossguid)
find_library(SDL2_image SDL2_image)
find_library(uuid uuid)
find_library(dl dl)
find_library(pthread pthread)
find_library(profiler profiler)

if (NOT SDL2)
	message(FATAL_ERROR "SDL2 not found")
endif()

if (NOT GLEW)
	message(FATAL_ERROR "GLEW not found")
endif()

if (NOT crossguid)
	message(FATAL_ERROR "Crossguid not found")
endif()

if (NOT SDL2_image)
	message(FATAL_ERROR "SDL2_image not found")
endif()

if (NOT uuid)
	message(FATAL_ERROR "uuid not found")
endif()

if (NOT dl)
	message(FATAL_ERROR "dl not found")
endif()

if (NOT pthread)
	message(FATAL_ERROR "pthread not found")
endif()

if (NOT profiler)
	message(FATAL_ERROR "profiler not found")
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g -fPIC -Wno-non-template-friend)
target_link_options(${PROJECT_NAME} PRIVATE -Wl,--no-as-needed)
target_compile_definitions(${PROJECT_NAME} PRIVATE RENDERER_OPEN_GL _DEBUG)
#target_link_options(${PROJECT_NAME} PRIVATE -pg)
#target_link_directories(${PROJECT_NAME} PRIVATE ../external/linux PRIVATE /usr/local/lib PRIVATE /usr/lib)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ../external/inc PUBLIC ../external/inc/SDL2) # PUBLIC . PUBLIC ${CMAKE_SYSTEM_INCLUDE_PATH})
target_include_directories(${PROJECT_NAME} PUBLIC ../headers) # PUBLIC . PUBLIC ${CMAKE_SYSTEM_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME} SDL2 GLEW crossguid SDL2_image uuid dl pthread profiler)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
