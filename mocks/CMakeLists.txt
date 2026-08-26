cmake_minimum_required(VERSION 3.24)
project(linceu_mocks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(LINCEU_MOCK_QT_ROOT
    "C:/Qt/6.11.0/mingw_64"
    CACHE PATH "Qt 6 MinGW root used to build the VersionEditor mock")

if(DEFINED ENV{QTDIR} AND EXISTS "$ENV{QTDIR}/lib/cmake/Qt6/Qt6Config.cmake")
    set(LINCEU_MOCK_QT_ROOT
        "$ENV{QTDIR}"
        CACHE PATH "Qt 6 MinGW root used to build the VersionEditor mock" FORCE)
elseif(NOT EXISTS "${LINCEU_MOCK_QT_ROOT}/lib/cmake/Qt6/Qt6Config.cmake")
    file(GLOB LINCEU_QT6_CONFIGS "C:/Qt/*/mingw_64/lib/cmake/Qt6/Qt6Config.cmake")
    list(LENGTH LINCEU_QT6_CONFIGS LINCEU_QT6_CONFIGS_COUNT)
    if(LINCEU_QT6_CONFIGS_COUNT GREATER 0)
        list(SORT LINCEU_QT6_CONFIGS COMPARE NATURAL ORDER DESCENDING)
        list(GET LINCEU_QT6_CONFIGS 0 LINCEU_QT6_CONFIG)
        get_filename_component(LINCEU_QT6_CONFIG_DIR "${LINCEU_QT6_CONFIG}" DIRECTORY)
        get_filename_component(LINCEU_QT6_CMAKE_DIR "${LINCEU_QT6_CONFIG_DIR}" DIRECTORY)
        get_filename_component(LINCEU_MOCK_QT_ROOT "${LINCEU_QT6_CMAKE_DIR}/../.." ABSOLUTE)
        set(LINCEU_MOCK_QT_ROOT
            "${LINCEU_MOCK_QT_ROOT}"
            CACHE PATH "Qt 6 MinGW root used to build the VersionEditor mock" FORCE)
    endif()
endif()

if(NOT EXISTS "${LINCEU_MOCK_QT_ROOT}/lib/cmake/Qt6/Qt6Config.cmake")
    message(FATAL_ERROR
        "Qt6Config.cmake was not found. Set LINCEU_MOCK_QT_ROOT or QTDIR to your Qt 6 MinGW installation.")
endif()

list(PREPEND CMAKE_PREFIX_PATH "${LINCEU_MOCK_QT_ROOT}/lib/cmake")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(LINCEU_MOCK_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/output")
file(MAKE_DIRECTORY "${LINCEU_MOCK_OUTPUT_DIR}")

function(linceu_configure_mock_target target_name)
    target_include_directories(${target_name} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}")

    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${LINCEU_MOCK_OUTPUT_DIR}")

    foreach(LINCEU_CFG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${LINCEU_CFG}" LINCEU_CFG_UPPER)
        set_target_properties(${target_name} PROPERTIES
            "RUNTIME_OUTPUT_DIRECTORY_${LINCEU_CFG_UPPER}" "${LINCEU_MOCK_OUTPUT_DIR}")
    endforeach()
endfunction()

function(linceu_enable_qt_deploy target_name)
    if(NOT WIN32)
        return()
    endif()

    set(LINCEU_MOCK_WINDEPLOYQT "${LINCEU_MOCK_QT_ROOT}/bin/windeployqt.exe")
    if(NOT EXISTS "${LINCEU_MOCK_WINDEPLOYQT}")
        message(WARNING "windeployqt.exe nao encontrado em ${LINCEU_MOCK_WINDEPLOYQT}; o executavel pode nao rodar fora do ambiente configurado.")
        return()
    endif()

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND "${LINCEU_MOCK_WINDEPLOYQT}"
            --dir "$<TARGET_FILE_DIR:${target_name}>"
            --compiler-runtime
            --no-translations
            "$<TARGET_FILE:${target_name}>"
        COMMENT "Deploying Qt runtime for ${target_name}"
        VERBATIM)
endfunction()

if(WIN32)
    add_executable(find_VersionEditor WIN32
        find_VersionEditor.cpp
        find_VersionEditor.h)
else()
    add_executable(find_VersionEditor
        find_VersionEditor.cpp
        find_VersionEditor.h)
endif()

target_link_libraries(find_VersionEditor PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets)

linceu_configure_mock_target(find_VersionEditor)
linceu_enable_qt_deploy(find_VersionEditor)

if(WIN32)
    add_executable(compile_translations WIN32
        compile_translations.cpp
        compile_translations.h)
else()
    add_executable(compile_translations
        compile_translations.cpp
        compile_translations.h)
endif()

target_link_libraries(compile_translations PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets)

linceu_configure_mock_target(compile_translations)
linceu_enable_qt_deploy(compile_translations)
