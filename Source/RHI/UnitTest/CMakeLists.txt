if(BUILD_SHARED)
	add_definitions(-DBUILD_SHARED_LIB -DBUILD_WITH_PLUGIN)
endif()
################################## Unit Test For RHI #####################################

include_directories(
    ${FREETYPE2_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_unittest(
	RHI-UnitTest-1.Device
	1.Device.cpp
)

add_unittest(
	RHI-UnitTest-2.SwapChainPresent
	2.SwapChainPresent.cpp
)

add_unittest(
	RHI-UnitTest-3.Triangle
	3.Triangle.cpp
)

add_unittest(
	RHI-UnitTest-4.TexturedCube
	4.TexturedCube.cpp
	Base/TextureObject.h
	Base/TextureObject.cpp
)

add_unittest(
	RHI-UnitTest-5.ComputeParticles
	5.ComputeParticles.cpp
)