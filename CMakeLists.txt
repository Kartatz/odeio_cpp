cmake_minimum_required(VERSION 3.13)

project(
	sexo
	VERSION 2
	DESCRIPTION "nova versão do sexo: o sexo 2"
	HOMEPAGE_URL "https://comiocudecurioso.com/"
	LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")

add_executable(
	pdftxtremover
	src/main.cpp
)

foreach(property BUILD_RPATH INSTALL_RPATH)
	set_target_properties(
		pdftxtremover
		PROPERTIES
		${property} "$ORIGIN/../lib:$ORIGIN"
	)
endforeach()

target_compile_features(pdftxtremover PRIVATE cxx_std_17)

set(USE_IMPLICIT_CRYPTO OFF)
set(ALLOW_CRYPTO_NATIVE ON)
set(REQUIRE_CRYPTO_NATIVE ON)
set(USE_INSECURE_RANDOM ON)
set(SKIP_OS_SECURE_RANDOM ON)

set(HAVE_VERSION_SCRIPT OFF CACHE BOOL "")
#set(WITH_JPEG7 ON CACHE BOOL "")
set(WITH_JPEG8 ON CACHE BOOL "")
set(WITH_SIMD OFF CACHE BOOL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)

set(LIBJPEG_H_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules/jpeg-turbo")
set(LIBJPEG_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}turbojpeg${CMAKE_SHARED_LIBRARY_SUFFIX}")

set(ZLIB_H_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules/zlib")
set(ZLIB_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}z${CMAKE_SHARED_LIBRARY_SUFFIX}")

add_subdirectory(submodules/qpdf EXCLUDE_FROM_ALL)
add_subdirectory(submodules/zlib EXCLUDE_FROM_ALL)
add_subdirectory(submodules/jpeg-turbo EXCLUDE_FROM_ALL)

foreach(target zlib turbojpeg)
	add_custom_command(
		OUTPUT ${target}
		COMMAND ${CMAKE_COMMAND} --build ./ --target ${target}
	)
	
	add_custom_target(ensure_${target} ALL DEPENDS ${target})
	add_dependencies(libqpdf ensure_${target})
endforeach()

foreach(property BUILD_RPATH INSTALL_RPATH)
	set_target_properties(
		libqpdf
		PROPERTIES
		${property} "$ORIGIN"
	)
endforeach()

foreach(property RUNTIME_OUTPUT_DIRECTORY LIBRARY_OUTPUT_DIRECTORY)
	foreach(target zlib turbojpeg)
		set_target_properties(
			${target}
			PROPERTIES
			${property} $<TARGET_FILE_DIR:pdftxtremover>
		)
		
		set_target_properties(
			${target}
			PROPERTIES
			NO_SONAME 1
		)
	endforeach()
endforeach()

target_link_libraries(
	pdftxtremover
	libqpdf
	turbojpeg
)

foreach(target pdftxtremover libqpdf)
	install(
		TARGETS ${target}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib NAMELINK_SKIP
	)
endforeach()

foreach(target pdftxtremover libqpdf zlib turbojpeg)
	install(
		TARGETS ${target}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib NAMELINK_SKIP
	)
endforeach()
