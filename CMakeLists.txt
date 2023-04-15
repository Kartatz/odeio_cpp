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


add_custom_command(
	OUTPUT "a"
	COMMAND ${CMAKE_COMMAND} --build ./ --target zlib
	COMMENT "-- Building FFmpeg"
)
endif()

add_custom_target(ensure_zlib ALL DEPENDS "a")

add_subdirectory(submodules/qpdf EXCLUDE_FROM_ALL)
add_subdirectory(submodules/zlib EXCLUDE_FROM_ALL)
add_subdirectory(submodules/turbo-jpeg EXCLUDE_FROM_ALL)

foreach(property BUILD_RPATH INSTALL_RPATH)
	set_target_properties(
		libqpdf
		PROPERTIES
		${property} "$ORIGIN"
	)
endforeach()

foreach(property RUNTIME_OUTPUT_DIRECTORY LIBRARY_OUTPUT_DIRECTORY)
	foreach(target turbojpeg zlib)
		set_target_properties(
			${target}
			PROPERTIES
			${property} $<TARGET_FILE_DIR:sparklec>
		)
	endforeach()
endforeach()

target_link_libraries(
	pdftxtremover
	libqpdf
)

foreach(target pdftxtremover libqpdf)
	install(
		TARGETS ${target}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib NAMELINK_SKIP
	)
endforeach()
