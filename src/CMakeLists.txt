cmake_minimum_required(VERSION 3.21)

project(iceengine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GAME_DIR_NAME "colonizer")
if (UNIX)
   #	set(CMAKE_LIBRARY_PATH {$CMAKE_LIBRARY_PATH} ../external/linux)
   if (NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE "Debug")
   endif()
elseif(WIN32)
	# Export everything on Windows so we don't need __declspec
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	#set(CMAKE_PREFIX_PATH {$CMAKE_LIBRARY_PATH} ../external/windows/lib) # #IMPORTANT, CMake looks here
endif()
#set(GCC_COMPILER_FLAGS "-O3 -std=c++2b")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COMPILER_FLAGS}")
set(CMAKE_DEBUG_POSTFIX "d")
#set(FORCE_STATIC_VCRT ON)

# https://cmake.org/cmake/help/latest/command/add_library.html#object-libraries
#add_library(glad OBJECT ../external/inc/GL/glad.c)
#set_property(TARGET glad PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(iceengine SHARED
#$<TARGET_OBJECTS:glad>
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
Renderer/OpenGL/VAO.cpp
Renderer/OpenGL/VBO.cpp
Renderer/OpenGL/OctreeDebuggerGL.cpp
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
Components/Systems/BiomeSystem.cpp
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
Components/Systems/BeeSystem.cpp
Components/Systems/CharacterSystem.cpp
Components/Systems/SceneGraphSystem.cpp
Components/TerrainComponent.cpp
Engine.cpp
Particles/Particle.cpp
Particles/ParticleSystem.cpp
../headers/System/Octree.h
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
System/Triangle.cpp
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
Importers/ModelImporterGlTF.cpp
World/LowPolyTerrain.cpp
World/World.cpp
World/Terrain.cpp
World/WaterTile.cpp
Algorithms/AStar.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
	
#set(CMAKE_STATIC_LIBRARY_PREFIX ${PROJECT_SOURCE_DIR}/../external/windows/lib)

get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()

#if(UNIX)
#	find_library(SDL2 SDL2)
#	find_library(crossguid crossguid)
#	find_library(SDL2_image SDL2_image)
#elseif (WIN32)
#	find_library(SDL2 SDL2 HINTS ${PROJECT_SOURCE_DIR}/../external/windows/lib NO_DEFAULT_PATH)
#	#find_library(SDL2dbg SDL2d) # HINTS ${PROJECT_SOURCE_DIR}/../external/windows/lib NO_DEFAULT_PATH)
#	find_library(crossguid crossguid)
#	find_library(crossguiddbg crossguid-dgb)
#	find_library(SDL2_image SDL2_image HINTS  ${PROJECT_SOURCE_DIR}/../external/windows/lib NO_DEFAULT_PATH)
#	#find_library(SDL2_imagedbg SDL2_imaged) # HINTS  ${PROJECT_SOURCE_DIR}/../external/windows/lib NO_DEFAULT_PATH)
#endif()

find_package(SDL2 REQUIRED)
find_package(sdl2-image REQUIRED)
find_package(crossguid REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(glad REQUIRED)
find_package(date REQUIRED)
find_package(glm REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
find_path(RAPIDXML_INCLUDE_DIRS "rapidxml/rapidxml.hpp")

if (UNIX)
	find_library(dl dl)
	find_library(uuid uuid)
	find_library(pthread pthread)
	find_library(profiler profiler)
	if (NOT dl)
		message(FATAL_ERROR "dl not found")
	endif()
	if (NOT uuid)
		message(FATAL_ERROR "uuid not found")
	endif()

	if (NOT pthread)
		message(FATAL_ERROR "pthread not found")
	endif()

	if (NOT profiler)
		message(FATAL_ERROR "profiler not found")
	endif()
endif(UNIX)

if (UNIX)
	target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g -fPIC -Wno-non-template-friend)
	target_link_options(${PROJECT_NAME} PRIVATE -Wl,--no-as-needed)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE RENDERER_OPEN_GL)
target_include_directories(${PROJECT_NAME} PRIVATE ${RAPIDXML_INCLUDE_DIRS} PRIVATE ${TINYGLTF_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ../headers) # PUBLIC . PUBLIC ${CMAKE_SYSTEM_INCLUDE_PATH})

if (UNIX)
   target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static SDL2::SDL2_image crossguid glm::glm nlohmann_json::nlohmann_json glad::glad date::date ${uuid} ${dl} ${pthread} ${profiler})
	set_target_properties(${PROJECT_NAME} 
		PROPERTIES 
			DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
elseif (WIN32)
	target_compile_options(${PROJECT_NAME} PRIVATE /permissive-)
	target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2_image crossguid glm::glm nlohmann_json::nlohmann_json glad::glad date::date)
	set_target_properties(${PROJECT_NAME} 
		PROPERTIES 
      #RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../../${GAME_DIR_NAME}/build/$<CONFIG>"
			DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
endif()
target_precompile_headers(${PROJECT_NAME} PUBLIC ../headers/pch.h)

if (WIN32)
   set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)
elseif (UNIX)
   set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include/iceengine)
endif()

# To make CMAKE_INSTALL_LIBDIR available
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
   CONFIGURATIONS Debug Release RelWithDebInfo
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY "../headers/" # <--- trailing slash to avoid creating a headers folder at the target location
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
		
# We link statically on Linux and dynamically on Windows for now
# Dynamic linking leads to rpath errors on linux, haven't investigated further
if (WIN32)
   # Copy SDL2 and SDL2-image DLLs to game's binary dir
   install(IMPORTED_RUNTIME_ARTIFACTS
      SDL2::SDL2 SDL2::SDL2_image
      CONFIGURATIONS Debug Release RelWithDebInfo
      RUNTIME DESTINATION "${PROJECT_SOURCE_DIR}/../../${GAME_DIR_NAME}/build/$<CONFIG>")
   # Copy the DLLs that SDL2-image depends on to our binary dir
   install(
      FILES
         ${_VCPKG_INSTALLED_DIR}/x64-windows/debug/bin/zlibd1.dll
         ${_VCPKG_INSTALLED_DIR}/x64-windows/debug/bin/libpng16d.dll
      CONFIGURATIONS Debug
      DESTINATION "${PROJECT_SOURCE_DIR}/../../${GAME_DIR_NAME}/build/$<CONFIG>")
   install(
      FILES
         ${_VCPKG_INSTALLED_DIR}/x64-windows/bin/zlib1.dll
         ${_VCPKG_INSTALLED_DIR}/x64-windows/bin/libpng16.dll
      CONFIGURATIONS Release RelWithDebInfo
      DESTINATION "${PROJECT_SOURCE_DIR}/../../${GAME_DIR_NAME}/build/$<CONFIG>")
endif()