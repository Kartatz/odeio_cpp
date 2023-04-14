cmake_minimum_required(VERSION 3.13)

project(
	sexo
	VERSION 2
	DESCRIPTION "nova versão do sexo: o sexo 2"
	HOMEPAGE_URL "https://comiocudecurioso.com/"
	LANGUAGES CXX
)

add_executable(
	pdftxtremover
	src/main.cpp
)

target_compile_features(pdftxtremover PRIVATE cxx_std_17)

add_subdirectory(submodules/qpdf EXCLUDE_FROM_ALL)

target_link_libraries(
	pdftxtremover
	qpdf
)

foreach(target pdftxtremover qpdf)
	install(
		TARGETS ${target}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib NAMELINK_SKIP
	)
endforeach()
