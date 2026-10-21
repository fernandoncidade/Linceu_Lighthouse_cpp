cmake_minimum_required(VERSION 4.1)
project(Linceu_Lighthouse_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(LINCEU_OUTPUT_NAME "Linceu_Lighthouse" CACHE STRING "Output executable base name")
option(BUILD_CLI "Compilar utilitario de linha de comando linceu-lighthouse-cli" ON)
set(LINCEU_DEFAULT_DEPLOY_DIR "D:/MISCELANEAS/VSCode/LinceuLighthouse/Linceu_Lighthouse")
if(WIN32)
        if(MSVC)
                set(LINCEU_DEFAULT_DEPLOY_DIR "D:/MISCELANEAS/VSCode/LinceuLighthouse/Linceu_Lighthouse_msvc")
        elseif(MINGW)
                set(LINCEU_DEFAULT_DEPLOY_DIR "D:/MISCELANEAS/VSCode/LinceuLighthouse/Linceu_Lighthouse_mingw")
        endif()
endif()
set(LINCEU_DEPLOY_DIR "${LINCEU_DEFAULT_DEPLOY_DIR}" CACHE PATH "Directory that receives the runnable Windows deployment")

set(LINCEU_APP_KIND)
set(LINCEU_WINDOWS_RESOURCES)
set(LINCEU_WINDEPLOYQT_EXECUTABLE "")
if(WIN32)
        enable_language(RC)
        set(LINCEU_APP_KIND WIN32)
        list(APPEND LINCEU_WINDOWS_RESOURCES source/Linceu_Lighthouse_cpp.rc)
        set(LINCEU_SELECTED_QT_CMAKE_DIR "")

        set(LINCEU_QT_CMAKE_CANDIDATE_DIRS "")
        if(DEFINED Qt6_DIR)
                get_filename_component(LINCEU_QT6_CMAKE_DIR_FROM_QT6_DIR "${Qt6_DIR}" DIRECTORY)
                list(APPEND LINCEU_QT_CMAKE_CANDIDATE_DIRS "${LINCEU_QT6_CMAKE_DIR_FROM_QT6_DIR}")
        endif()
        if(DEFINED QT_DIR)
                get_filename_component(LINCEU_QT6_CMAKE_DIR_FROM_QT_DIR "${QT_DIR}" DIRECTORY)
                list(APPEND LINCEU_QT_CMAKE_CANDIDATE_DIRS "${LINCEU_QT6_CMAKE_DIR_FROM_QT_DIR}")
        endif()
        list(REMOVE_DUPLICATES LINCEU_QT_CMAKE_CANDIDATE_DIRS)

        foreach(LINCEU_QT_CMAKE_DIR_CANDIDATE IN LISTS LINCEU_QT_CMAKE_CANDIDATE_DIRS)
                if(EXISTS "${LINCEU_QT_CMAKE_DIR_CANDIDATE}/Qt6/Qt6Config.cmake")
                        set(LINCEU_SELECTED_QT_CMAKE_DIR "${LINCEU_QT_CMAKE_DIR_CANDIDATE}")
                        break()
                endif()
        endforeach()

        if(NOT LINCEU_SELECTED_QT_CMAKE_DIR)
                set(LINCEU_QT_COMPILER_DIR "")
                if(MSVC)
                        set(LINCEU_QT_COMPILER_DIR "msvc2022_64")
                elseif(MINGW)
                        set(LINCEU_QT_COMPILER_DIR "mingw_64")
                endif()

                if(LINCEU_QT_COMPILER_DIR)
                        file(GLOB QT6_CONFIG_CANDIDATES "C:/Qt/*/${LINCEU_QT_COMPILER_DIR}/lib/cmake/Qt6/Qt6Config.cmake")
                        list(LENGTH QT6_CONFIG_CANDIDATES QT6_CANDIDATES_COUNT)
                        if(QT6_CANDIDATES_COUNT GREATER 0)
                                list(SORT QT6_CONFIG_CANDIDATES COMPARE NATURAL ORDER DESCENDING)
                                foreach(QT6_CANDIDATE_PATH IN LISTS QT6_CONFIG_CANDIDATES)
                                        get_filename_component(QT6_CONFIG_DIR "${QT6_CANDIDATE_PATH}" DIRECTORY)
                                        get_filename_component(QT_CMAKE_DIR "${QT6_CONFIG_DIR}" DIRECTORY)
                                        set(LINCEU_SELECTED_QT_CMAKE_DIR "${QT_CMAKE_DIR}")
                                        break()
                                endforeach()
                        endif()
                endif()
        endif()

        if(LINCEU_SELECTED_QT_CMAKE_DIR)
                get_cmake_property(LINCEU_CACHE_VARIABLES CACHE_VARIABLES)
                foreach(LINCEU_CACHE_VAR IN LISTS LINCEU_CACHE_VARIABLES)
                        if(LINCEU_CACHE_VAR MATCHES "^Qt6.*_DIR$" OR LINCEU_CACHE_VAR STREQUAL "Qt6_DIR" OR LINCEU_CACHE_VAR STREQUAL "QT_DIR" OR LINCEU_CACHE_VAR STREQUAL "WINDEPLOYQT_EXECUTABLE")
                                unset(${LINCEU_CACHE_VAR} CACHE)
                        endif()
                endforeach()

                list(PREPEND CMAKE_PREFIX_PATH "${LINCEU_SELECTED_QT_CMAKE_DIR}")
                set(QT_DIR "${LINCEU_SELECTED_QT_CMAKE_DIR}/Qt6" CACHE PATH "Qt6 package directory" FORCE)
                set(Qt6_DIR "${LINCEU_SELECTED_QT_CMAKE_DIR}/Qt6" CACHE PATH "Qt6 package directory" FORCE)

                foreach(LINCEU_QT_COMPONENT Core Gui Widgets Sql Xml Concurrent OpenGL OpenGLWidgets Quick Qml)
                        if(EXISTS "${LINCEU_SELECTED_QT_CMAKE_DIR}/Qt6${LINCEU_QT_COMPONENT}/Qt6${LINCEU_QT_COMPONENT}Config.cmake")
                                set(Qt6${LINCEU_QT_COMPONENT}_DIR
                                        "${LINCEU_SELECTED_QT_CMAKE_DIR}/Qt6${LINCEU_QT_COMPONENT}"
                                        CACHE PATH "Qt6${LINCEU_QT_COMPONENT} package directory" FORCE)
                        endif()
                endforeach()

                get_filename_component(LINCEU_QT_BIN_DIR "${LINCEU_SELECTED_QT_CMAKE_DIR}/../../bin" ABSOLUTE)
                set(LINCEU_WINDEPLOYQT_EXECUTABLE "${LINCEU_QT_BIN_DIR}/windeployqt.exe")
                if(EXISTS "${LINCEU_WINDEPLOYQT_EXECUTABLE}")
                        set(WINDEPLOYQT_EXECUTABLE "${LINCEU_WINDEPLOYQT_EXECUTABLE}" CACHE FILEPATH "Qt windeployqt executable" FORCE)
                endif()
        endif()
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Sql Xml Concurrent OpenGL OpenGLWidgets Quick Qml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Sql Xml Concurrent OpenGL OpenGLWidgets Quick Qml)

set(LINCEU_WINDEPLOYQT_RUNTIME_ARGS)
set(LINCEU_MSVC_RUNTIME_DLLS)
set(LINCEU_QT_RUNTIME_TRANSLATION_FILES)
set(LINCEU_REQUIRED_MSVC_RUNTIME_DLL_NAMES
        msvcp140.dll
        msvcp140_1.dll
        msvcp140_2.dll
        vcruntime140.dll
        vcruntime140_1.dll)
set(LINCEU_UNUSED_MSVC_RUNTIME_DLL_NAMES
        concrt140.dll
        msvcp140_atomic_wait.dll
        msvcp140_codecvt_ids.dll
        vccorlib140.dll
        vcruntime140_threads.dll)

if(WIN32)
        list(APPEND LINCEU_WINDEPLOYQT_RUNTIME_ARGS --no-translations)
endif()

if(MSVC)
        list(APPEND LINCEU_WINDEPLOYQT_RUNTIME_ARGS --no-compiler-runtime)

        set(LINCEU_MSVC_REDIST_ROOTS)
        if(DEFINED ENV{VCToolsRedistDir})
                file(TO_CMAKE_PATH "$ENV{VCToolsRedistDir}" LINCEU_VCTOOLS_REDIST_DIR)
                list(APPEND LINCEU_MSVC_REDIST_ROOTS "${LINCEU_VCTOOLS_REDIST_DIR}")
        endif()
        if(DEFINED ENV{VCINSTALLDIR})
                file(TO_CMAKE_PATH "$ENV{VCINSTALLDIR}/Redist/MSVC" LINCEU_VCINSTALL_REDIST_DIR)
                list(APPEND LINCEU_MSVC_REDIST_ROOTS "${LINCEU_VCINSTALL_REDIST_DIR}")
        endif()
        list(APPEND LINCEU_MSVC_REDIST_ROOTS
                "C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Redist/MSVC")
        list(REMOVE_DUPLICATES LINCEU_MSVC_REDIST_ROOTS)

        foreach(LINCEU_MSVC_REDIST_ROOT IN LISTS LINCEU_MSVC_REDIST_ROOTS)
                if(NOT EXISTS "${LINCEU_MSVC_REDIST_ROOT}")
                        continue()
                endif()

                file(GLOB LINCEU_MSVC_REDIST_DLLS CONFIGURE_DEPENDS
                        "${LINCEU_MSVC_REDIST_ROOT}/x64/Microsoft.VC*.CRT/*.dll"
                        "${LINCEU_MSVC_REDIST_ROOT}/*/x64/Microsoft.VC*.CRT/*.dll")
                list(APPEND LINCEU_MSVC_RUNTIME_DLLS ${LINCEU_MSVC_REDIST_DLLS})
        endforeach()

        list(REMOVE_DUPLICATES LINCEU_MSVC_RUNTIME_DLLS)
        list(SORT LINCEU_MSVC_RUNTIME_DLLS)

        set(LINCEU_FILTERED_MSVC_RUNTIME_DLLS)
        foreach(LINCEU_MSVC_RUNTIME_DLL IN LISTS LINCEU_MSVC_RUNTIME_DLLS)
                get_filename_component(LINCEU_MSVC_RUNTIME_DLL_NAME
                        "${LINCEU_MSVC_RUNTIME_DLL}" NAME)
                string(TOLOWER "${LINCEU_MSVC_RUNTIME_DLL_NAME}" LINCEU_MSVC_RUNTIME_DLL_NAME_LOWER)
                if(LINCEU_MSVC_RUNTIME_DLL_NAME_LOWER IN_LIST LINCEU_REQUIRED_MSVC_RUNTIME_DLL_NAMES)
                        list(APPEND LINCEU_FILTERED_MSVC_RUNTIME_DLLS "${LINCEU_MSVC_RUNTIME_DLL}")
                endif()
        endforeach()
        set(LINCEU_MSVC_RUNTIME_DLLS ${LINCEU_FILTERED_MSVC_RUNTIME_DLLS})

        if(LINCEU_MSVC_RUNTIME_DLLS)
                message(STATUS "Required MSVC runtime DLLs will be deployed next to Linceu_Lighthouse.exe")
        else()
                message(WARNING "MSVC runtime DLLs were not found; target machines may need Microsoft Visual C++ Redistributable installed.")
        endif()
endif()

set(LINCEU_COPY_MSVC_RUNTIME_TO_TARGET_COMMANDS)
set(LINCEU_COPY_MSVC_RUNTIME_TO_DEPLOY_COMMANDS)
set(LINCEU_REMOVE_UNUSED_MSVC_RUNTIME_FROM_TARGET_COMMANDS)
set(LINCEU_REMOVE_UNUSED_MSVC_RUNTIME_FROM_DEPLOY_COMMANDS)
if(MSVC)
        foreach(LINCEU_UNUSED_MSVC_RUNTIME_DLL IN LISTS LINCEU_UNUSED_MSVC_RUNTIME_DLL_NAMES)
                list(APPEND LINCEU_REMOVE_UNUSED_MSVC_RUNTIME_FROM_TARGET_COMMANDS
                        COMMAND "${CMAKE_COMMAND}" -E rm -f
                                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/${LINCEU_UNUSED_MSVC_RUNTIME_DLL}")
                list(APPEND LINCEU_REMOVE_UNUSED_MSVC_RUNTIME_FROM_DEPLOY_COMMANDS
                        COMMAND "${CMAKE_COMMAND}" -E rm -f
                                "${LINCEU_DEPLOY_DIR}/${LINCEU_UNUSED_MSVC_RUNTIME_DLL}")
        endforeach()
endif()

if(MSVC AND LINCEU_MSVC_RUNTIME_DLLS)
        list(APPEND LINCEU_COPY_MSVC_RUNTIME_TO_TARGET_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        ${LINCEU_MSVC_RUNTIME_DLLS}
                        "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>")
        list(APPEND LINCEU_COPY_MSVC_RUNTIME_TO_DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        ${LINCEU_MSVC_RUNTIME_DLLS}
                        "${LINCEU_DEPLOY_DIR}")
endif()

if(WIN32)
        set(LINCEU_QT_TRANSLATIONS_DIR "")
        if(LINCEU_SELECTED_QT_CMAKE_DIR)
                get_filename_component(LINCEU_QT_INSTALL_ROOT "${LINCEU_SELECTED_QT_CMAKE_DIR}/../.." ABSOLUTE)
                set(LINCEU_QT_TRANSLATIONS_DIR "${LINCEU_QT_INSTALL_ROOT}/translations")
        elseif(DEFINED Qt${QT_VERSION_MAJOR}_DIR)
                get_filename_component(LINCEU_QT_INSTALL_ROOT "${Qt${QT_VERSION_MAJOR}_DIR}/../../.." ABSOLUTE)
                set(LINCEU_QT_TRANSLATIONS_DIR "${LINCEU_QT_INSTALL_ROOT}/translations")
        endif()

        foreach(LINCEU_QT_TRANSLATION_LANG IN ITEMS de en es fr it pt_BR)
                set(LINCEU_QT_TRANSLATION_FILE
                        "${LINCEU_QT_TRANSLATIONS_DIR}/qt_${LINCEU_QT_TRANSLATION_LANG}.qm")
                if(EXISTS "${LINCEU_QT_TRANSLATION_FILE}")
                        list(APPEND LINCEU_QT_RUNTIME_TRANSLATION_FILES "${LINCEU_QT_TRANSLATION_FILE}")
                endif()
        endforeach()

        if(LINCEU_QT_RUNTIME_TRANSLATION_FILES)
                message(STATUS "Qt runtime translations will be limited to: de, en, es, fr, it, pt_BR")
        else()
                message(WARNING "Supported Qt runtime translation files were not found in ${LINCEU_QT_TRANSLATIONS_DIR}.")
        endif()
endif()

set(LINCEU_PREPARE_QT_TRANSLATIONS_TO_TARGET_COMMANDS)
set(LINCEU_PREPARE_QT_TRANSLATIONS_TO_DEPLOY_COMMANDS)
if(WIN32)
        list(APPEND LINCEU_PREPARE_QT_TRANSLATIONS_TO_TARGET_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E rm -rf
                        "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/locale/qt"
                COMMAND "${CMAKE_COMMAND}" -E make_directory
                        "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/locale/qt")
        list(APPEND LINCEU_PREPARE_QT_TRANSLATIONS_TO_DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E rm -rf
                        "${LINCEU_DEPLOY_DIR}/locale/qt"
                COMMAND "${CMAKE_COMMAND}" -E make_directory
                        "${LINCEU_DEPLOY_DIR}/locale/qt")

        if(LINCEU_QT_RUNTIME_TRANSLATION_FILES)
                list(APPEND LINCEU_PREPARE_QT_TRANSLATIONS_TO_TARGET_COMMANDS
                        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                                ${LINCEU_QT_RUNTIME_TRANSLATION_FILES}
                                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/locale/qt")
                list(APPEND LINCEU_PREPARE_QT_TRANSLATIONS_TO_DEPLOY_COMMANDS
                        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                                ${LINCEU_QT_RUNTIME_TRANSLATION_FILES}
                                "${LINCEU_DEPLOY_DIR}/locale/qt")
        endif()

        list(APPEND LINCEU_PREPARE_QT_TRANSLATIONS_TO_TARGET_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E rm -rf
                        "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/translations")
        list(APPEND LINCEU_PREPARE_QT_TRANSLATIONS_TO_DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E rm -rf
                        "${LINCEU_DEPLOY_DIR}/translations")
endif()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(source)
include_directories(source/data/Charts)
include_directories(source/data)
include_directories(source/data/GeradorEstatisticas)
include_directories(source/gui)
include_directories(source/locale)
include_directories(source/services)
include_directories(source/services/GerenciamentoBaseEvento)
include_directories(source/services/GerenciamentoDiretorioScanner)
include_directories(source/services/GerenciamentoEventoModificado)
include_directories(source/services/GerenciamentoEventoMovido)
include_directories(source/services/GerenciamentoMetadados)
include_directories(source/services/GerenciamentoObservador)
include_directories(source/services/GerenciamentoTabela)
include_directories(source/security)
include_directories(source/tools)
include_directories(source/tools/GerenciamentoAdministradorFiltros)
include_directories(source/tools/GerenciamentoFiltros)
include_directories(source/ui)
include_directories(source/ui/GerenciamentoBotoes)
include_directories(source/ui/GerenciamentoDialogoCores)
include_directories(source/ui/GerenciamentoEstatisticasUI)
include_directories(source/ui/GerenciamentoEventosUI)
include_directories(source/ui/GerenciamentoLocalizadorQt)
include_directories(source/ui/GerenciamentoMenusUI)
include_directories(source/ui/GerenciamentoTradutorMetadadosQt)
include_directories(source/ui/Manual)
include_directories(source/utils)

file(GLOB_RECURSE LINCEU_SECURITY_SOURCES
        source/security/*.cpp
        source/security/*.h)

file(GLOB LINCEU_APP_TRANSLATION_FILES
        "${CMAKE_SOURCE_DIR}/source/locale/*.qm")

file(GLOB_RECURSE LINCEU_CHARTS_LGPL_SOURCES
        source/data/Charts/*.cpp
        source/data/Charts/*.h)

add_executable(Linceu_Lighthouse_cpp ${LINCEU_APP_KIND}
        ${LINCEU_WINDOWS_RESOURCES}
        ${LINCEU_SECURITY_SOURCES}
        ${LINCEU_CHARTS_LGPL_SOURCES}
        source/assets/ABOUT/ABOUT_de_DE.txt
        source/assets/ABOUT/ABOUT_en_US.txt
        source/assets/ABOUT/ABOUT_es_ES.txt
        source/assets/ABOUT/ABOUT_fr_FR.txt
        source/assets/ABOUT/ABOUT_it_IT.txt
        source/assets/ABOUT/ABOUT_pt_BR.txt
        source/assets/ABOUT/History_APP_de_DE.txt
        source/assets/ABOUT/History_APP_en_US.txt
        source/assets/ABOUT/History_APP_es_ES.txt
        source/assets/ABOUT/History_APP_fr_FR.txt
        source/assets/ABOUT/History_APP_it_IT.txt
        source/assets/ABOUT/History_APP_pt_BR.txt
        "source/assets/CLC/CLC_de_DE - Linceu Lighthouse.txt"
        "source/assets/CLC/CLC_en_US - Linceu Lighthouse.txt"
        "source/assets/CLC/CLC_es_ES - Linceu Lighthouse.txt"
        "source/assets/CLC/CLC_fr_FR - Linceu Lighthouse.txt"
        "source/assets/CLC/CLC_it_IT - Linceu Lighthouse.txt"
        "source/assets/CLC/CLC_pt_BR - Linceu Lighthouse.txt"
        "source/assets/COPYRIGHT/AVIS DE COPYRIGHT ET DE MARQUE DÉPOSÉE_fr_FR.txt"
        "source/assets/COPYRIGHT/AVISO DE COPYRIGHT E MARCA REGISTRA_pt_BR.txt"
        "source/assets/COPYRIGHT/AVISO DE DERECHOS DE AUTOR Y MARCA REGISTRADA_es_ES.txt"
        "source/assets/COPYRIGHT/AVVISO SU COPYRIGHT E MARCHIO REGISTRATO_it_IT.txt"
        "source/assets/COPYRIGHT/COPYRIGHT AND TRADEMARK NOTICE_en_US.txt"
        "source/assets/COPYRIGHT/HINWEIS ZU URHEBERRECHT UND MARKENZEICHEN_de_DE.txt"
        "source/assets/EULA/EULA_de_DE - Linceu Lighthouse.txt"
        "source/assets/EULA/EULA_en_US - Linceu Lighthouse.txt"
        "source/assets/EULA/EULA_es_ES - Linceu Lighthouse.txt"
        "source/assets/EULA/EULA_fr_FR - Linceu Lighthouse.txt"
        "source/assets/EULA/EULA_it_IT - Linceu Lighthouse.txt"
        "source/assets/EULA/EULA_pt_BR - Linceu Lighthouse.txt"
        source/assets/images/icons/analyze.ico
        source/assets/images/icons/atualizar.ico
        source/assets/images/icons/blocos-de-cores.ico
        source/assets/images/icons/calendar.ico
        source/assets/images/icons/clear.ico
        source/assets/images/icons/clear_button3.ico
        source/assets/images/icons/filtro.ico
        source/assets/images/icons/linceu_sem_fundo.ico
        source/assets/images/icons/linceu_sem_fundo_71_71.ico
        source/assets/images/icons/linceu_sem_fundo_150_150.ico
        source/assets/images/icons/linceu_sem_fundo_300_300.ico
        source/assets/images/icons/linceu_sem_fundo_584_800.ico
        source/assets/images/icons/linceu_sem_fundo_1080_1080.ico
        source/assets/images/icons/linceu_sem_fundo_1440_2160.ico
        source/assets/images/icons/linceu_sem_fundo_1920_1080.ico
        source/assets/images/icons/linceu_sem_fundo_2160_2160.ico
        source/assets/images/icons/linceu_sem_fundo_3840_2160.ico
        source/assets/images/icons/paleta-de-cores.ico
        source/assets/images/icons/pause1.ico
        source/assets/images/icons/pause2.ico
        source/assets/images/icons/pause3.ico
        source/assets/images/icons/pause4.ico
        source/assets/images/icons/salvar.ico
        source/assets/images/icons/save_as.ico
        source/assets/images/icons/scan.ico
        source/assets/images/icons/selecione.ico
        source/assets/images/icons/start.ico
        source/assets/images/icons/statistics.ico
        source/assets/images/icons/stop.ico
        source/assets/images/imgs/analyze.png
        source/assets/images/imgs/atualizar.png
        source/assets/images/imgs/blocos-de-cores.png
        source/assets/images/imgs/calendar.png
        source/assets/images/imgs/clear.png
        source/assets/images/imgs/clear_button3.png
        source/assets/images/imgs/filtro.png
        source/assets/images/imgs/linceu_sem_fundo.png
        source/assets/images/imgs/linceu_sem_fundo_71_71.png
        source/assets/images/imgs/linceu_sem_fundo_150_150.png
        source/assets/images/imgs/linceu_sem_fundo_300_300.png
        source/assets/images/imgs/linceu_sem_fundo_584_800.png
        source/assets/images/imgs/linceu_sem_fundo_1080_1080.png
        source/assets/images/imgs/linceu_sem_fundo_1440_2160.png
        source/assets/images/imgs/linceu_sem_fundo_1920_1080.png
        source/assets/images/imgs/linceu_sem_fundo_2160_2160.png
        source/assets/images/imgs/linceu_sem_fundo_3840_2160.png
        source/assets/images/imgs/paleta-de-cores.png
        source/assets/images/imgs/pause1.png
        source/assets/images/imgs/pause2.png
        source/assets/images/imgs/pause3.png
        source/assets/images/imgs/pause4.png
        source/assets/images/imgs/salvar.png
        source/assets/images/imgs/save_as.png
        source/assets/images/imgs/scan.png
        source/assets/images/imgs/selecione.png
        source/assets/images/imgs/start.png
        source/assets/images/imgs/statistics.png
        source/assets/images/imgs/stop.png
        "source/assets/LICENSES/Apache License Version 2.0.txt"
        "source/assets/LICENSES/BSD License Clause-2.txt"
        "source/assets/LICENSES/BSD License Clause-3.txt"
        "source/assets/LICENSES/GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.txt"
        "source/assets/LICENSES/GNU LESSER GENERAL PUBLIC LICENSE Version 3.txt"
        "source/assets/LICENSES/ISC LICENSE.txt"
        "source/assets/LICENSES/MIT License.txt"
        source/assets/LICENSES/MS-Store-ADA-v8.9-FINAL-EN.pdf
        "source/assets/LICENSES/PYTHON SOFTWARE FOUNDATION LICENSE.txt"
        "source/assets/LICENSES/SQLite Is Public Domain.txt"
        source/assets/NOTICES/NOTICE_de_DE.txt
        source/assets/NOTICES/NOTICE_en_US.txt
        source/assets/NOTICES/NOTICE_es_ES.txt
        source/assets/NOTICES/NOTICE_fr_FR.txt
        source/assets/NOTICES/NOTICE_it_IT.txt
        source/assets/NOTICES/NOTICE_pt_BR.txt
        source/assets/PRIVACY_POLICY/Privacy_Policy_de_DE.txt
        source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt
        source/assets/PRIVACY_POLICY/Privacy_Policy_es_ES.txt
        source/assets/PRIVACY_POLICY/Privacy_Policy_fr_FR.txt
        source/assets/PRIVACY_POLICY/Privacy_Policy_it_IT.txt
        source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt
        "source/assets/RELEASE/RELEASE NOTES_de_DE.txt"
        "source/assets/RELEASE/RELEASE NOTES_en_US.txt"
        "source/assets/RELEASE/RELEASE NOTES_es_ES.txt"
        "source/assets/RELEASE/RELEASE NOTES_fr_FR.txt"
        "source/assets/RELEASE/RELEASE NOTES_it_IT.txt"
        "source/assets/RELEASE/RELEASE NOTES_pt_BR.txt"
        source/data/GeradorEstatisticas/gst_01_BaseGerador.cpp
        source/data/GeradorEstatisticas/gst_01_BaseGerador.h
        source/data/GeradorEstatisticas/gst_02_GraficoPizza.cpp
        source/data/GeradorEstatisticas/gst_02_GraficoPizza.h
        source/data/GeradorEstatisticas/gst_03_GraficoBarras.cpp
        source/data/GeradorEstatisticas/gst_03_GraficoBarras.h
        source/data/GeradorEstatisticas/gst_04_GraficoTimeline.cpp
        source/data/GeradorEstatisticas/gst_04_GraficoTimeline.h
        source/data/GeradorEstatisticas/gst_05_GraficoTreemap.cpp
        source/data/GeradorEstatisticas/gst_05_GraficoTreemap.h
        source/data/GeradorEstatisticas/gst_06_GraficoHistograma.cpp
        source/data/GeradorEstatisticas/gst_06_GraficoHistograma.h
        source/data/GeradorEstatisticas/gst_07_GraficoPareto.cpp
        source/data/GeradorEstatisticas/gst_07_GraficoPareto.h
        source/data/GeradorEstatisticas/gst_08_GraficoLinha.cpp
        source/data/GeradorEstatisticas/gst_08_GraficoLinha.h
        source/data/GeradorEstatisticas/gst_09_GraficoBoxplot.cpp
        source/data/GeradorEstatisticas/gst_09_GraficoBoxplot.h
        source/data/GeradorEstatisticas/gst_10_GraficoRadarEventos.cpp
        source/data/GeradorEstatisticas/gst_10_GraficoRadarEventos.h
        source/data/GeradorEstatisticas/gst_11_GraficoHeatmap.cpp
        source/data/GeradorEstatisticas/gst_11_GraficoHeatmap.h
        source/data/GeradorEstatisticas/gst_12_GraficoScatter.cpp
        source/data/GeradorEstatisticas/gst_12_GraficoScatter.h
        source/data/GeradorEstatisticas/gst_13_GraficoSankey.cpp
        source/data/GeradorEstatisticas/gst_13_GraficoSankey.h
        source/data/GeradorEstatisticas/gst_14_GraficoRadar.cpp
        source/data/GeradorEstatisticas/gst_14_GraficoRadar.h
        source/data/GeradorEstatisticas/gst_15_GraficoDotplot.cpp
        source/data/GeradorEstatisticas/gst_15_GraficoDotplot.h
        source/data/GeradorEstatisticas/gst_16_GraficoSankeyEventoCaminho.cpp
        source/data/GeradorEstatisticas/gst_16_GraficoSankeyEventoCaminho.h
        source/data/GeradorEstatisticas/gst_17_GraficoSankeyTipoCaminho.cpp
        source/data/GeradorEstatisticas/gst_17_GraficoSankeyTipoCaminho.h
        source/data/GeradorEstatisticas/gst_18_GraficoArvoreDiretorios.cpp
        source/data/GeradorEstatisticas/gst_18_GraficoArvoreDiretorios.h
        source/data/st_01_GeradorEstatisticas.cpp
        source/data/st_01_GeradorEstatisticas.h
        source/gui/ic_01_InterfaceMonitor.cpp
        source/gui/ic_01_InterfaceMonitor.h
        source/gui/ic_02_Inicializador.cpp
        source/gui/ic_02_Inicializador.h
        source/gui/ic_03_Configurador.cpp
        source/gui/ic_03_Configurador.h
        source/gui/ic_04_Atualizador.cpp
        source/gui/ic_04_Atualizador.h
        source/gui/ic_05_GerenciadorProgresso.cpp
        source/gui/ic_05_GerenciadorProgresso.h
        source/gui/ic_06_GerenciadorMensagens.cpp
        source/gui/ic_06_GerenciadorMensagens.h
        source/gui/ic_07_ManipuladorTabela.cpp
        source/gui/ic_07_ManipuladorTabela.h
        source/gui/ic_08_Internacionalizador.cpp
        source/gui/ic_08_Internacionalizador.h
        source/services/GerenciamentoBaseEvento/gbank_01_set_callback.cpp
        source/services/GerenciamentoBaseEvento/gbank_01_set_callback.h
        source/services/GerenciamentoBaseEvento/gbank_02_criar_banco_de_dados.cpp
        source/services/GerenciamentoBaseEvento/gbank_02_criar_banco_de_dados.h
        source/services/GerenciamentoBaseEvento/gbank_03_processar_exclusao.cpp
        source/services/GerenciamentoBaseEvento/gbank_03_processar_exclusao.h
        source/services/GerenciamentoBaseEvento/gbank_04_registrar_evento_especifico.cpp
        source/services/GerenciamentoBaseEvento/gbank_04_registrar_evento_especifico.h
        source/services/GerenciamentoBaseEvento/gbank_05_obter_metadados_arquivo_excluido.cpp
        source/services/GerenciamentoBaseEvento/gbank_05_obter_metadados_arquivo_excluido.h
        source/services/GerenciamentoBaseEvento/gbank_06_registrar_evento_no_banco.cpp
        source/services/GerenciamentoBaseEvento/gbank_06_registrar_evento_no_banco.h
        source/services/GerenciamentoBaseEvento/gbank_07_atualizar_interface_apos_evento.cpp
        source/services/GerenciamentoBaseEvento/gbank_07_atualizar_interface_apos_evento.h
        source/services/GerenciamentoBaseEvento/gbank_08_scan_directory.cpp
        source/services/GerenciamentoBaseEvento/gbank_08_scan_directory.h
        source/services/GerenciamentoBaseEvento/gbank_09_get_tipo_from_snapshot.cpp
        source/services/GerenciamentoBaseEvento/gbank_09_get_tipo_from_snapshot.h
        source/services/GerenciamentoBaseEvento/gbank_10_is_directory.cpp
        source/services/GerenciamentoBaseEvento/gbank_10_is_directory.h
        source/services/GerenciamentoBaseEvento/gbank_11_limpar_registros.cpp
        source/services/GerenciamentoBaseEvento/gbank_11_limpar_registros.h
        source/services/GerenciamentoBaseEvento/gbank_12_obter_tipo_anterior.cpp
        source/services/GerenciamentoBaseEvento/gbank_12_obter_tipo_anterior.h
        source/services/GerenciamentoBaseEvento/gbank_13_notificar_evento.cpp
        source/services/GerenciamentoBaseEvento/gbank_13_notificar_evento.h
        source/services/GerenciamentoBaseEvento/gbank_14_remover_exclusao_temporaria.cpp
        source/services/GerenciamentoBaseEvento/gbank_14_remover_exclusao_temporaria.h
        source/services/GerenciamentoBaseEvento/gbank_15_criar_evento_exclusao.cpp
        source/services/GerenciamentoBaseEvento/gbank_15_criar_evento_exclusao.h
        source/services/GerenciamentoBaseEvento/gbank_16_criar_evento_padrao.cpp
        source/services/GerenciamentoBaseEvento/gbank_16_criar_evento_padrao.h
        source/services/GerenciamentoBaseEvento/gbank_17_atualizar_interface_apos_exclusao.cpp
        source/services/GerenciamentoBaseEvento/gbank_17_atualizar_interface_apos_exclusao.h
        source/services/GerenciamentoBaseEvento/gbank_18_processar_eventos_movimentacao.cpp
        source/services/GerenciamentoBaseEvento/gbank_18_processar_eventos_movimentacao.h
        source/services/GerenciamentoBaseEvento/gbank_19_inserir_evento_movido.cpp
        source/services/GerenciamentoBaseEvento/gbank_19_inserir_evento_movido.h
        source/services/GerenciamentoBaseEvento/gbank_20_db_writer.cpp
        source/services/GerenciamentoBaseEvento/gbank_20_db_writer.h
        source/services/GerenciamentoDiretorioScanner/gscanner_01_process_batch.cpp
        source/services/GerenciamentoDiretorioScanner/gscanner_01_process_batch.h
        source/services/GerenciamentoDiretorioScanner/gscanner_02_scan_directory.cpp
        source/services/GerenciamentoDiretorioScanner/gscanner_02_scan_directory.h
        source/services/GerenciamentoDiretorioScanner/gscanner_03_processar_fila.cpp
        source/services/GerenciamentoDiretorioScanner/gscanner_03_processar_fila.h
        source/services/GerenciamentoDiretorioScanner/gscanner_04_get_file_type.cpp
        source/services/GerenciamentoDiretorioScanner/gscanner_04_get_file_type.h
        source/services/GerenciamentoDiretorioScanner/gscanner_05_processar_item.cpp
        source/services/GerenciamentoDiretorioScanner/gscanner_05_processar_item.h
        source/services/GerenciamentoDiretorioScanner/gscanner_06_atualizar_progresso.cpp
        source/services/GerenciamentoDiretorioScanner/gscanner_06_atualizar_progresso.h
        source/services/GerenciamentoDiretorioScanner/gscanner_07_atualizar_interface.cpp
        source/services/GerenciamentoDiretorioScanner/gscanner_07_atualizar_interface.h
        source/services/GerenciamentoDiretorioScanner/gscanner_08_finalizar_scan.cpp
        source/services/GerenciamentoDiretorioScanner/gscanner_08_finalizar_scan.h
        source/services/GerenciamentoDiretorioScanner/gscanner_09_scan_worker_run.cpp
        source/services/GerenciamentoDiretorioScanner/gscanner_09_scan_worker_run.h
        source/services/GerenciamentoEventoModificado/gevmod_01_calcular_intervalo.cpp
        source/services/GerenciamentoEventoModificado/gevmod_01_calcular_intervalo.h
        source/services/GerenciamentoEventoModificado/gevmod_02_calcular_intervalo_original.cpp
        source/services/GerenciamentoEventoModificado/gevmod_02_calcular_intervalo_original.h
        source/services/GerenciamentoEventoModificado/gevmod_03_is_arquivo_codigo_grande.cpp
        source/services/GerenciamentoEventoModificado/gevmod_03_is_arquivo_codigo_grande.h
        source/services/GerenciamentoEventoModificado/gevmod_04_limpar_cache_metadados.cpp
        source/services/GerenciamentoEventoModificado/gevmod_04_limpar_cache_metadados.h
        source/services/GerenciamentoEventoModificado/gevmod_05_processar.cpp
        source/services/GerenciamentoEventoModificado/gevmod_05_processar.h
        source/services/GerenciamentoEventoModificado/gevmod_06_processar_massivo.cpp
        source/services/GerenciamentoEventoModificado/gevmod_06_processar_massivo.h
        source/services/GerenciamentoEventoModificado/gevmod_07_processar_normal.cpp
        source/services/GerenciamentoEventoModificado/gevmod_07_processar_normal.h
        source/services/GerenciamentoEventoMovido/gevmov_01_MovimentacaoWorker.cpp
        source/services/GerenciamentoEventoMovido/gevmov_01_MovimentacaoWorker.h
        source/services/GerenciamentoEventoMovido/gevmov_02_verificar_movimentacao.cpp
        source/services/GerenciamentoEventoMovido/gevmov_02_verificar_movimentacao.h
        source/services/GerenciamentoEventoMovido/gevmov_03_remover_exclusao.cpp
        source/services/GerenciamentoEventoMovido/gevmov_03_remover_exclusao.h
        source/services/GerenciamentoEventoMovido/gevmov_04_atualizar_linha_recente.cpp
        source/services/GerenciamentoEventoMovido/gevmov_04_atualizar_linha_recente.h
        source/services/GerenciamentoEventoMovido/gevmov_05_atualizar_tabela_completa.cpp
        source/services/GerenciamentoEventoMovido/gevmov_05_atualizar_tabela_completa.h
        source/services/GerenciamentoEventoMovido/gevmov_06_inicializar_sistema_evento.cpp
        source/services/GerenciamentoEventoMovido/gevmov_06_inicializar_sistema_evento.h
        source/services/GerenciamentoEventoMovido/gevmov_07_processar_exclusoes_pendentes.cpp
        source/services/GerenciamentoEventoMovido/gevmov_07_processar_exclusoes_pendentes.h
        source/services/GerenciamentoEventoMovido/gevmov_08_adicionar_item_tabela.cpp
        source/services/GerenciamentoEventoMovido/gevmov_08_adicionar_item_tabela.h
        source/services/GerenciamentoEventoMovido/gevmov_09_adicionar_evento.cpp
        source/services/GerenciamentoEventoMovido/gevmov_09_adicionar_evento.h
        source/services/GerenciamentoMetadados/gmet_01_ExtrairMetadados.cpp
        source/services/GerenciamentoMetadados/gmet_01_ExtrairMetadados.h
        source/services/GerenciamentoMetadados/gmet_02_ExtrairMetadadosCodigoFonte.cpp
        source/services/GerenciamentoMetadados/gmet_02_ExtrairMetadadosCodigoFonte.h
        source/services/GerenciamentoMetadados/gmet_03_ExtrairMetadadosImagem.cpp
        source/services/GerenciamentoMetadados/gmet_03_ExtrairMetadadosImagem.h
        source/services/GerenciamentoMetadados/gmet_04_ExtrairMetadadosAudio.cpp
        source/services/GerenciamentoMetadados/gmet_04_ExtrairMetadadosAudio.h
        source/services/GerenciamentoMetadados/gmet_05_ExtrairMetadadosVideo.cpp
        source/services/GerenciamentoMetadados/gmet_05_ExtrairMetadadosVideo.h
        source/services/GerenciamentoMetadados/gmet_06_ExtrairMetadadosDocumento.cpp
        source/services/GerenciamentoMetadados/gmet_06_ExtrairMetadadosDocumento.h
        source/services/GerenciamentoMetadados/gmet_07_ExtrairMetadadosPlanilha.cpp
        source/services/GerenciamentoMetadados/gmet_07_ExtrairMetadadosPlanilha.h
        source/services/GerenciamentoMetadados/gmet_08_ExtrairMetadadosApresentacao.cpp
        source/services/GerenciamentoMetadados/gmet_08_ExtrairMetadadosApresentacao.h
        source/services/GerenciamentoMetadados/gmet_09_ExtrairMetadadosBancoDados.cpp
        source/services/GerenciamentoMetadados/gmet_09_ExtrairMetadadosBancoDados.h
        source/services/GerenciamentoMetadados/gmet_10_ExtrairMetadadosExecutavel.cpp
        source/services/GerenciamentoMetadados/gmet_10_ExtrairMetadadosExecutavel.h
        source/services/GerenciamentoMetadados/gmet_11_ExtrairMetadadosTemporario.cpp
        source/services/GerenciamentoMetadados/gmet_11_ExtrairMetadadosTemporario.h
        source/services/GerenciamentoMetadados/gmet_12_ExtrairMetadadosCompactados.cpp
        source/services/GerenciamentoMetadados/gmet_12_ExtrairMetadadosCompactados.h
        source/services/GerenciamentoMetadados/gmet_13_ExtrairMetadadosBackup.cpp
        source/services/GerenciamentoMetadados/gmet_13_ExtrairMetadadosBackup.h
        source/services/GerenciamentoMetadados/gmet_14_ExtrairMetadadosLog.cpp
        source/services/GerenciamentoMetadados/gmet_14_ExtrairMetadadosLog.h
        source/services/GerenciamentoMetadados/gmet_15_ExtrairMetadadosConfig.cpp
        source/services/GerenciamentoMetadados/gmet_15_ExtrairMetadadosConfig.h
        source/services/GerenciamentoMetadados/gmet_16_ExtrairMetadadosOlefile.cpp
        source/services/GerenciamentoMetadados/gmet_16_ExtrairMetadadosOlefile.h
        source/services/GerenciamentoMetadados/gmet_17_ExtrairMetadadosDadosEstruturados.cpp
        source/services/GerenciamentoMetadados/gmet_17_ExtrairMetadadosDadosEstruturados.h
        source/services/GerenciamentoMetadados/gmet_18_ExtrairMetadadosCompletos.cpp
        source/services/GerenciamentoMetadados/gmet_18_ExtrairMetadadosCompletos.h
        source/services/GerenciamentoMetadados/gmet_19_GetTipoArquivo.cpp
        source/services/GerenciamentoMetadados/gmet_19_GetTipoArquivo.h
        source/services/GerenciamentoMetadados/gmet_20_GetTamanhoDiretorioArquivo.cpp
        source/services/GerenciamentoMetadados/gmet_20_GetTamanhoDiretorioArquivo.h
        source/services/GerenciamentoMetadados/gmet_21_GetFormataTamanho.cpp
        source/services/GerenciamentoMetadados/gmet_21_GetFormataTamanho.h
        source/services/GerenciamentoMetadados/gmet_22_GetAtributosArquivo.cpp
        source/services/GerenciamentoMetadados/gmet_22_GetAtributosArquivo.h
        source/services/GerenciamentoMetadados/gmet_23_GetAutorArquivo.cpp
        source/services/GerenciamentoMetadados/gmet_23_GetAutorArquivo.h
        source/services/GerenciamentoMetadados/gmet_24_GetDimensoesArquivo.cpp
        source/services/GerenciamentoMetadados/gmet_24_GetDimensoesArquivo.h
        source/services/GerenciamentoMetadados/gmet_25_GetDuracaoArquivo.cpp
        source/services/GerenciamentoMetadados/gmet_25_GetDuracaoArquivo.h
        source/services/GerenciamentoMetadados/gmet_26_GetTaxaBitsArquivo.cpp
        source/services/GerenciamentoMetadados/gmet_26_GetTaxaBitsArquivo.h
        source/services/GerenciamentoMetadados/gmet_27_GetProtecaoArquivo.cpp
        source/services/GerenciamentoMetadados/gmet_27_GetProtecaoArquivo.h
        source/services/GerenciamentoMetadados/gmet_28_AtualizarInterface.cpp
        source/services/GerenciamentoMetadados/gmet_28_AtualizarInterface.h
        source/services/GerenciamentoMetadados/gmet_29_SalvarEstadoTabela.cpp
        source/services/GerenciamentoMetadados/gmet_29_SalvarEstadoTabela.h
        source/services/GerenciamentoMetadados/gmet_30_RestaurarEstadoTabela.cpp
        source/services/GerenciamentoMetadados/gmet_30_RestaurarEstadoTabela.h
        source/services/GerenciamentoMetadados/gmet_31_CarregarConfiguracoes.cpp
        source/services/GerenciamentoMetadados/gmet_31_CarregarConfiguracoes.h
        source/services/GerenciamentoMetadados/gmet_32_SalvarConfiguracoes.cpp
        source/services/GerenciamentoMetadados/gmet_32_SalvarConfiguracoes.h
        source/services/GerenciamentoMetadados/gmet_33_ProcessarFilaMetadados.cpp
        source/services/GerenciamentoMetadados/gmet_33_ProcessarFilaMetadados.h
        source/services/GerenciamentoMetadados/gmet_34_CallbackMetadados.cpp
        source/services/GerenciamentoMetadados/gmet_34_CallbackMetadados.h
        source/services/GerenciamentoMetadados/gmet_35_ConfigurarTabela.cpp
        source/services/GerenciamentoMetadados/gmet_35_ConfigurarTabela.h
        source/services/GerenciamentoMetadados/gmet_36_MostrarDialogoConfiguracao.cpp
        source/services/GerenciamentoMetadados/gmet_36_MostrarDialogoConfiguracao.h
        source/services/GerenciamentoMetadados/gmet_37_InvalidarCacheDiretoriosRelacionados.cpp
        source/services/GerenciamentoMetadados/gmet_37_InvalidarCacheDiretoriosRelacionados.h
        source/services/GerenciamentoMetadados/gmet_38_GetMetadados.cpp
        source/services/GerenciamentoMetadados/gmet_38_GetMetadados.h
        source/services/GerenciamentoMetadados/gmet_39_IdentificarTipoArquivo.cpp
        source/services/GerenciamentoMetadados/gmet_39_IdentificarTipoArquivo.h
        source/services/GerenciamentoMetadados/gmet_40_AdicionarMetadadosAoItem.cpp
        source/services/GerenciamentoMetadados/gmet_40_AdicionarMetadadosAoItem.h
        source/services/GerenciamentoMetadados/gmet_41_ProcessarColunaThread.cpp
        source/services/GerenciamentoMetadados/gmet_41_ProcessarColunaThread.h
        source/services/GerenciamentoMetadados/gmet_42_AdicionarItemParaColuna.cpp
        source/services/GerenciamentoMetadados/gmet_42_AdicionarItemParaColuna.h
        source/services/GerenciamentoMetadados/gmet_43_AtualizarColunaInterface.cpp
        source/services/GerenciamentoMetadados/gmet_43_AtualizarColunaInterface.h
        source/services/GerenciamentoMetadados/gmet_44_ExtrairMetadadosEmLote.cpp
        source/services/GerenciamentoMetadados/gmet_44_ExtrairMetadadosEmLote.h
        source/services/GerenciamentoMetadados/gmet_45_MetadadosExtraidosCallback.cpp
        source/services/GerenciamentoMetadados/gmet_45_MetadadosExtraidosCallback.h
        source/services/GerenciamentoMetadados/gmet_46_GetSizeUnit.cpp
        source/services/GerenciamentoMetadados/gmet_46_GetSizeUnit.h
        source/services/GerenciamentoMetadados/gmet_47_GetTamanhoBytes.cpp
        source/services/GerenciamentoMetadados/gmet_47_GetTamanhoBytes.h
        source/services/GerenciamentoObservador/gob_01_DetectarOperacaoMassiva.cpp
        source/services/GerenciamentoObservador/gob_01_DetectarOperacaoMassiva.h
        source/services/GerenciamentoObservador/gob_02_Iniciar.cpp
        source/services/GerenciamentoObservador/gob_02_Iniciar.h
        source/services/GerenciamentoObservador/gob_03_HandleScanError.cpp
        source/services/GerenciamentoObservador/gob_03_HandleScanError.h
        source/services/GerenciamentoObservador/gob_04_IniciarMonitoramento.cpp
        source/services/GerenciamentoObservador/gob_04_IniciarMonitoramento.h
        source/services/GerenciamentoObservador/gob_05_ProcessarBufferEventos.cpp
        source/services/GerenciamentoObservador/gob_05_ProcessarBufferEventos.h
        source/services/GerenciamentoObservador/gob_06_PararScan.cpp
        source/services/GerenciamentoObservador/gob_06_PararScan.h
        source/services/GerenciamentoObservador/gob_07_Parar.cpp
        source/services/GerenciamentoObservador/gob_07_Parar.h
        source/services/GerenciamentoObservador/gob_08_LimparEstado.cpp
        source/services/GerenciamentoObservador/gob_08_LimparEstado.h
        source/services/GerenciamentoObservador/gob_09_Monitorar.cpp
        source/services/GerenciamentoObservador/gob_09_Monitorar.h
        source/services/GerenciamentoObservador/gob_10_ProcessarEventoInterno.cpp
        source/services/GerenciamentoObservador/gob_10_ProcessarEventoInterno.h
        source/services/GerenciamentoObservador/gob_11_ProcessarEvento.cpp
        source/services/GerenciamentoObservador/gob_11_ProcessarEvento.h
        source/services/GerenciamentoObservador/gob_12_ReiniciarMonitoramento.cpp
        source/services/GerenciamentoObservador/gob_12_ReiniciarMonitoramento.h
        source/services/GerenciamentoObservador/gob_13_PausarMonitoramentoEscaneamento.cpp
        source/services/GerenciamentoObservador/gob_13_PausarMonitoramentoEscaneamento.h
        source/services/GerenciamentoTabela/gtab_01_DetectarTemaWindows.cpp
        source/services/GerenciamentoTabela/gtab_01_DetectarTemaWindows.h
        source/services/GerenciamentoTabela/gtab_02_CalcularCorTextoIdeal.cpp
        source/services/GerenciamentoTabela/gtab_02_CalcularCorTextoIdeal.h
        source/services/GerenciamentoTabela/gtab_03_ConfigurarTabela.cpp
        source/services/GerenciamentoTabela/gtab_03_ConfigurarTabela.h
        source/services/GerenciamentoTabela/gtab_04_AjustarLargurasColunas.cpp
        source/services/GerenciamentoTabela/gtab_04_AjustarLargurasColunas.h
        source/services/GerenciamentoTabela/gtab_05_AplicarQuebraLinhaCabecalho.cpp
        source/services/GerenciamentoTabela/gtab_05_AplicarQuebraLinhaCabecalho.h
        source/services/GerenciamentoTabela/gtab_06_AplicarQuebraLinhaTodosCabecalhos.cpp
        source/services/GerenciamentoTabela/gtab_06_AplicarQuebraLinhaTodosCabecalhos.h
        source/services/GerenciamentoTabela/gtab_07_AjustarAlturaCabecalho.cpp
        source/services/GerenciamentoTabela/gtab_07_AjustarAlturaCabecalho.h
        source/services/GerenciamentoTabela/gtab_08_RedimensionarCabecalho.cpp
        source/services/GerenciamentoTabela/gtab_08_RedimensionarCabecalho.h
        source/services/GerenciamentoTabela/gtab_09_AtualizarCabecalhos.cpp
        source/services/GerenciamentoTabela/gtab_09_AtualizarCabecalhos.h
        source/services/GerenciamentoTabela/gtab_10_AtualizarDadosTabela.cpp
        source/services/GerenciamentoTabela/gtab_10_AtualizarDadosTabela.h
        source/services/GerenciamentoTabela/gtab_11_AtualizarLinhaMaisRecente.cpp
        source/services/GerenciamentoTabela/gtab_11_AtualizarLinhaMaisRecente.h
        source/services/GerenciamentoTabela/gtab_12_AtualizarVisualizacaoTabela.cpp
        source/services/GerenciamentoTabela/gtab_12_AtualizarVisualizacaoTabela.h
        source/services/GerenciamentoTabela/gtab_13_AtualizarVisibilidadeColunas.cpp
        source/services/GerenciamentoTabela/gtab_13_AtualizarVisibilidadeColunas.h
        source/services/GerenciamentoTabela/gtab_14_InvalidarCacheCores.cpp
        source/services/GerenciamentoTabela/gtab_14_InvalidarCacheCores.h
        source/services/GerenciamentoTabela/gtab_15_ObterCoresOperacao.cpp
        source/services/GerenciamentoTabela/gtab_15_ObterCoresOperacao.h
        source/services/GerenciamentoTabela/gtab_16_ObterIndicesColunas.cpp
        source/services/GerenciamentoTabela/gtab_16_ObterIndicesColunas.h
        source/services/GerenciamentoTabela/gtab_17_AtivarCores.cpp
        source/services/GerenciamentoTabela/gtab_17_AtivarCores.h
        source/services/GerenciamentoTabela/gtab_18_OcultarCores.cpp
        source/services/GerenciamentoTabela/gtab_18_OcultarCores.h
        source/services/GerenciamentoTabela/gtab_19_AplicarCoresLinhaEspecifica.cpp
        source/services/GerenciamentoTabela/gtab_19_AplicarCoresLinhaEspecifica.h
        source/services/GerenciamentoTabela/gtab_20_AplicarCoresTodasColunas.cpp
        source/services/GerenciamentoTabela/gtab_20_AplicarCoresTodasColunas.h
        source/services/GerenciamentoTabela/gtab_21_RedefinirCoresTodasColunas.cpp
        source/services/GerenciamentoTabela/gtab_21_RedefinirCoresTodasColunas.h
        source/services/GerenciamentoTabela/gtab_22_RemoverCorColuna.cpp
        source/services/GerenciamentoTabela/gtab_22_RemoverCorColuna.h
        source/services/GerenciamentoTabela/gtab_23_AtualizarCoresColunas.cpp
        source/services/GerenciamentoTabela/gtab_23_AtualizarCoresColunas.h
        source/services/GerenciamentoTabela/gtab_24_SetColunaColorir.cpp
        source/services/GerenciamentoTabela/gtab_24_SetColunaColorir.h
        source/services/GerenciamentoTabela/gtab_25_SetColunasColorirEmMassa.cpp
        source/services/GerenciamentoTabela/gtab_25_SetColunasColorirEmMassa.h
        source/services/GerenciamentoTabela/gtab_26_RemoverTodasCoresColunas.cpp
        source/services/GerenciamentoTabela/gtab_26_RemoverTodasCoresColunas.h
        source/services/GerenciamentoTabela/gtab_27_SalvarConfiguracoesCores.cpp
        source/services/GerenciamentoTabela/gtab_27_SalvarConfiguracoesCores.h
        source/services/GerenciamentoTabela/gtab_28_CarregarConfiguracoesCores.cpp
        source/services/GerenciamentoTabela/gtab_28_CarregarConfiguracoesCores.h
        source/services/GerenciamentoTabela/gtab_29_EhColunaPersonalizadaColorida.cpp
        source/services/GerenciamentoTabela/gtab_29_EhColunaPersonalizadaColorida.h
        source/services/GerenciamentoTabela/gtab_30_MostrarDialogoConfiguracao.cpp
        source/services/GerenciamentoTabela/gtab_30_MostrarDialogoConfiguracao.h
        source/services/GerenciamentoTabela/gtab_31_AjustarCorSelecao.cpp
        source/services/GerenciamentoTabela/gtab_31_AjustarCorSelecao.h
        source/services/GerenciamentoTabela/gtab_32_WorkerThread.cpp
        source/services/GerenciamentoTabela/gtab_32_WorkerThread.h
        source/services/GerenciamentoTabela/gtab_33_AtualizarEstiloTema.cpp
        source/services/GerenciamentoTabela/gtab_33_AtualizarEstiloTema.h
        source/services/GerenciamentoTabela/gtab_34_MonitorTemaWindows.cpp
        source/services/GerenciamentoTabela/gtab_34_MonitorTemaWindows.h
        source/services/GerenciamentoTabela/gtab_35_IniciarProcessamentoPesado.cpp
        source/services/GerenciamentoTabela/gtab_35_IniciarProcessamentoPesado.h
        source/services/GerenciamentoTabela/gtab_36_AplicarCoresTodasColunasProcessamento.cpp
        source/services/GerenciamentoTabela/gtab_36_AplicarCoresTodasColunasProcessamento.h
        source/services/GerenciamentoTabela/gtab_37_ProcessarCoresEmBackground.cpp
        source/services/GerenciamentoTabela/gtab_37_ProcessarCoresEmBackground.h
        source/services/GerenciamentoTabela/gtab_38_OnCoresProcessadas.cpp
        source/services/GerenciamentoTabela/gtab_38_OnCoresProcessadas.h
        source/services/GerenciamentoTabela/gtab_39_AtualizarCoresNaInterface.cpp
        source/services/GerenciamentoTabela/gtab_39_AtualizarCoresNaInterface.h
        source/services/GerenciamentoTabela/gtab_40_AtualizarInterfacePosProcessamento.cpp
        source/services/GerenciamentoTabela/gtab_40_AtualizarInterfacePosProcessamento.h
        source/services/GerenciamentoTabela/gtab_41_ProcessarSelecaoBackground.cpp
        source/services/GerenciamentoTabela/gtab_41_ProcessarSelecaoBackground.h
        source/services/GerenciamentoTabela/gtab_42_ShutdownExecutors.cpp
        source/services/GerenciamentoTabela/gtab_42_ShutdownExecutors.h
        source/services/GerenciamentoTabela/gtab_43_AtualizarCabecalhosBridge.cpp
        source/services/GerenciamentoTabela/gtab_43_AtualizarCabecalhosBridge.h
        source/services/GerenciamentoTabela/gtab_44_OnIdiomaAlterado.cpp
        source/services/GerenciamentoTabela/gtab_44_OnIdiomaAlterado.h
        source/services/GerenciamentoTabela/gtab_45_RetraduzirDadosExistentes.cpp
        source/services/GerenciamentoTabela/gtab_45_RetraduzirDadosExistentes.h
        source/services/GerenciamentoTabela/gtab_46_ExecutarRetraducaoAgendada.cpp
        source/services/GerenciamentoTabela/gtab_46_ExecutarRetraducaoAgendada.h
        source/services/GerenciamentoTabela/gtab_47_EventTableModel.cpp
        source/services/GerenciamentoTabela/gtab_47_EventTableModel.h
        source/services/ob_01_Observador.cpp
        source/services/ob_01_Observador.h
        source/services/ob_02_BaseEvento.cpp
        source/services/ob_02_BaseEvento.h
        source/services/ob_03_DiretorioScanner.cpp
        source/services/ob_03_DiretorioScanner.h
        source/services/ob_04_EventoAdicionado.cpp
        source/services/ob_04_EventoAdicionado.h
        source/services/ob_05_EventoExcluido.cpp
        source/services/ob_05_EventoExcluido.h
        source/services/ob_06_EventoModificado.cpp
        source/services/ob_06_EventoModificado.h
        source/services/ob_07_EventoRenomeado.cpp
        source/services/ob_07_EventoRenomeado.h
        source/services/ob_08_EventoMovido.cpp
        source/services/ob_08_EventoMovido.h
        source/services/ob_09_GerenciadorColunas.cpp
        source/services/ob_09_GerenciadorColunas.h
        source/services/ob_10_GerenciadorTabela.cpp
        source/services/ob_10_GerenciadorTabela.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_01_desconectar_sinais.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_01_desconectar_sinais.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_02_aplicar_filtros.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_02_aplicar_filtros.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_03_limpar_filtros.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_03_limpar_filtros.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_04_sincronizar_menu_principal_com_filtros.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_04_sincronizar_menu_principal_com_filtros.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_05_atualizar_contagem.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_05_atualizar_contagem.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_06_atualizar_contagem_eventos_monitorados.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_06_atualizar_contagem_eventos_monitorados.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_07_verificar_filtro_operacao.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_07_verificar_filtro_operacao.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_08_atualizar_contagem_apos_idioma.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_08_atualizar_contagem_apos_idioma.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_09_notificar_alteracao_idioma.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_09_notificar_alteracao_idioma.h
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_10_salvar_estado_checkboxes.cpp
        source/tools/GerenciamentoAdministradorFiltros/gadmfil_10_salvar_estado_checkboxes.h
        source/tools/GerenciamentoFiltros/gfil_01_on_idioma_alterado.cpp
        source/tools/GerenciamentoFiltros/gfil_01_on_idioma_alterado.h
        source/tools/GerenciamentoFiltros/gfil_02_desconectar_sinais.cpp
        source/tools/GerenciamentoFiltros/gfil_02_desconectar_sinais.h
        source/tools/GerenciamentoFiltros/gfil_03_atualizar_status.cpp
        source/tools/GerenciamentoFiltros/gfil_03_atualizar_status.h
        source/tools/GerenciamentoFiltros/gfil_04_setup_ui.cpp
        source/tools/GerenciamentoFiltros/gfil_04_setup_ui.h
        source/tools/GerenciamentoFiltros/gfil_05_on_filtro_alterado.cpp
        source/tools/GerenciamentoFiltros/gfil_05_on_filtro_alterado.h
        source/tools/GerenciamentoFiltros/gfil_06_sincronizar_com_menu_principal.cpp
        source/tools/GerenciamentoFiltros/gfil_06_sincronizar_com_menu_principal.h
        source/tools/GerenciamentoFiltros/gfil_07_verificar_filtro_operacao.cpp
        source/tools/GerenciamentoFiltros/gfil_07_verificar_filtro_operacao.h
        source/tools/GerenciamentoFiltros/gfil_08_atualizar_contagem.cpp
        source/tools/GerenciamentoFiltros/gfil_08_atualizar_contagem.h
        source/tools/GerenciamentoFiltros/gfil_09_atualizar_interface.cpp
        source/tools/GerenciamentoFiltros/gfil_09_atualizar_interface.h
        source/tools/GerenciamentoFiltros/gfil_10_limpar_filtros.cpp
        source/tools/GerenciamentoFiltros/gfil_10_limpar_filtros.h
        source/tools/fil_01_Filtros.cpp
        source/tools/fil_01_Filtros.h
        source/tools/fil_02_AdministradorCalendario.cpp
        source/tools/fil_02_AdministradorCalendario.h
        source/tools/fil_03_AdministradorFiltros.cpp
        source/tools/fil_03_AdministradorFiltros.h
        source/ui/GerenciamentoBotoes/geb_01_selecionar_diretorio.cpp
        source/ui/GerenciamentoBotoes/geb_01_selecionar_diretorio.h
        source/ui/GerenciamentoBotoes/geb_02_alternar_analise_diretorio.cpp
        source/ui/GerenciamentoBotoes/geb_02_alternar_analise_diretorio.h
        source/ui/GerenciamentoBotoes/geb_03_exportar_dados.cpp
        source/ui/GerenciamentoBotoes/geb_03_exportar_dados.h
        source/ui/GerenciamentoBotoes/geb_04_exportar_para_sqlite.cpp
        source/ui/GerenciamentoBotoes/geb_04_exportar_para_sqlite.h
        source/ui/GerenciamentoBotoes/geb_05_limpar_dados.cpp
        source/ui/GerenciamentoBotoes/geb_05_limpar_dados.h
        source/ui/GerenciamentoBotoes/geb_06_pausar_monitoramento_ou_escaneamento.cpp
        source/ui/GerenciamentoBotoes/geb_06_pausar_monitoramento_ou_escaneamento.h
        source/ui/GerenciamentoDialogoCores/gdc_01_setup_ui.cpp
        source/ui/GerenciamentoDialogoCores/gdc_01_setup_ui.h
        source/ui/GerenciamentoDialogoCores/gdc_02_adicionar_grid_cores.cpp
        source/ui/GerenciamentoDialogoCores/gdc_02_adicionar_grid_cores.h
        source/ui/GerenciamentoDialogoCores/gdc_03_selecionar_cor.cpp
        source/ui/GerenciamentoDialogoCores/gdc_03_selecionar_cor.h
        source/ui/GerenciamentoDialogoCores/gdc_04_abrir_seletor_avancado.cpp
        source/ui/GerenciamentoDialogoCores/gdc_04_abrir_seletor_avancado.h
        source/ui/GerenciamentoDialogoCores/gdc_05_accept.cpp
        source/ui/GerenciamentoDialogoCores/gdc_05_accept.h
        source/ui/GerenciamentoDialogoCores/gdc_06_traduzir_dialogo_cores.cpp
        source/ui/GerenciamentoDialogoCores/gdc_06_traduzir_dialogo_cores.h
        source/ui/GerenciamentoDialogoCores/gdc_07_obter_cor.cpp
        source/ui/GerenciamentoDialogoCores/gdc_07_obter_cor.h
        source/ui/GerenciamentoDialogoCores/gdc_08_atualizar_traducoes.cpp
        source/ui/GerenciamentoDialogoCores/gdc_08_atualizar_traducoes.h
        source/ui/GerenciamentoEstatisticasUI/geui_01_mostrar_estatisticas.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_01_mostrar_estatisticas.h
        source/ui/GerenciamentoEstatisticasUI/geui_02_salvar_todos_graficos.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_02_salvar_todos_graficos.h
        source/ui/GerenciamentoEstatisticasUI/geui_03_limpar_referencia_dialog.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_03_limpar_referencia_dialog.h
        source/ui/GerenciamentoEstatisticasUI/geui_04_atualizar_graficos_apos_mudanca_idioma.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_04_atualizar_graficos_apos_mudanca_idioma.h
        source/ui/GerenciamentoEstatisticasUI/geui_05_criar_painel_selecao.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_05_criar_painel_selecao.h
        source/ui/GerenciamentoEstatisticasUI/geui_06_criar_painel_graficos.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_06_criar_painel_graficos.h
        source/ui/GerenciamentoEstatisticasUI/geui_07_criar_botao_toggle_painel.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_07_criar_botao_toggle_painel.h
        source/ui/GerenciamentoEstatisticasUI/geui_08_criar_lista_graficos.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_08_criar_lista_graficos.h
        source/ui/GerenciamentoEstatisticasUI/geui_09_criar_mapeamento_funcoes.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_09_criar_mapeamento_funcoes.h
        source/ui/GerenciamentoEstatisticasUI/geui_10_popular_checkboxes.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_10_popular_checkboxes.h
        source/ui/GerenciamentoEstatisticasUI/geui_11_verificar_estado_checkbox_todos.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_11_verificar_estado_checkbox_todos.h
        source/ui/GerenciamentoEstatisticasUI/geui_12_alternar_todos_checkboxes.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_12_alternar_todos_checkboxes.h
        source/ui/GerenciamentoEstatisticasUI/geui_13_obter_estados_checkboxes.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_13_obter_estados_checkboxes.h
        source/ui/GerenciamentoEstatisticasUI/geui_14_gerar_todos_graficos.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_14_gerar_todos_graficos.h
        source/ui/GerenciamentoEstatisticasUI/geui_15_atualizar_graficos.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_15_atualizar_graficos.h
        source/ui/GerenciamentoEstatisticasUI/geui_16_atualizar_graficos_sem_fechar.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_16_atualizar_graficos_sem_fechar.h
        source/ui/GerenciamentoEstatisticasUI/geui_17_regenerar_graficos_existentes.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_17_regenerar_graficos_existentes.h
        source/ui/GerenciamentoEstatisticasUI/geui_18_salvar_graficos_selecionados.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_18_salvar_graficos_selecionados.h
        source/ui/GerenciamentoEstatisticasUI/geui_19_toggle_painel_selecao.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_19_toggle_painel_selecao.h
        source/ui/GerenciamentoEstatisticasUI/geui_20_atualizar_textos_painel_selecao.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_20_atualizar_textos_painel_selecao.h
        source/ui/GerenciamentoEstatisticasUI/geui_21_atualizar_textos_checkboxes.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_21_atualizar_textos_checkboxes.h
        source/ui/GerenciamentoEstatisticasUI/geui_22_atualizar_layout_apos_mudanca_botao.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_22_atualizar_layout_apos_mudanca_botao.h
        source/ui/GerenciamentoEstatisticasUI/geui_23_atualizar_checkboxes_graficos.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_23_atualizar_checkboxes_graficos.h
        source/ui/GerenciamentoEstatisticasUI/geui_24_atualizar_abas_graficos.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_24_atualizar_abas_graficos.h
        source/ui/GerenciamentoEstatisticasUI/geui_25_atualizar_dados_graficos_com_novos_titulos.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_25_atualizar_dados_graficos_com_novos_titulos.h
        source/ui/GerenciamentoEstatisticasUI/geui_26_calcular_largura_ideal.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_26_calcular_largura_ideal.h
        source/ui/GerenciamentoEstatisticasUI/geui_27_ajustar_largura_painel_selecao.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_27_ajustar_largura_painel_selecao.h
        source/ui/GerenciamentoEstatisticasUI/geui_28_traduzir_botoes_detalhes.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_28_traduzir_botoes_detalhes.h
        source/ui/GerenciamentoEstatisticasUI/geui_29_abrir_diretorio.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_29_abrir_diretorio.h
        source/ui/GerenciamentoEstatisticasUI/geui_30_botao_rotacionado.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_30_botao_rotacionado.h
        source/ui/GerenciamentoEstatisticasUI/geui_31_worker_grafico.cpp
        source/ui/GerenciamentoEstatisticasUI/geui_31_worker_grafico.h
        source/ui/GerenciamentoEventosUI/geve_01_alternar_filtro.cpp
        source/ui/GerenciamentoEventosUI/geve_01_alternar_filtro.h
        source/ui/GerenciamentoEventosUI/geve_02_alternar_visibilidade_coluna.cpp
        source/ui/GerenciamentoEventosUI/geve_02_alternar_visibilidade_coluna.h
        source/ui/GerenciamentoEventosUI/geve_03_resetar_colunas.cpp
        source/ui/GerenciamentoEventosUI/geve_03_resetar_colunas.h
        source/ui/GerenciamentoEventosUI/geve_04_resetar_cores.cpp
        source/ui/GerenciamentoEventosUI/geve_04_resetar_cores.h
        source/ui/GerenciamentoEventosUI/geve_05_redefinir_todas_colunas_cores.cpp
        source/ui/GerenciamentoEventosUI/geve_05_redefinir_todas_colunas_cores.h
        source/ui/GerenciamentoEventosUI/geve_06_alterar_idioma.cpp
        source/ui/GerenciamentoEventosUI/geve_06_alterar_idioma.h
        source/ui/GerenciamentoEventosUI/geve_07_confirmar_mudanca_idioma.cpp
        source/ui/GerenciamentoEventosUI/geve_07_confirmar_mudanca_idioma.h
        source/ui/GerenciamentoEventosUI/geve_08_limpar_dados_monitorados.cpp
        source/ui/GerenciamentoEventosUI/geve_08_limpar_dados_monitorados.h
        source/ui/GerenciamentoEventosUI/geve_09_limpar_dados_interface.cpp
        source/ui/GerenciamentoEventosUI/geve_09_limpar_dados_interface.h
        source/ui/GerenciamentoEventosUI/geve_10_reiniciar_aplicativo.cpp
        source/ui/GerenciamentoEventosUI/geve_10_reiniciar_aplicativo.h
        source/ui/GerenciamentoEventosUI/geve_11_atualizar_interface.cpp
        source/ui/GerenciamentoEventosUI/geve_11_atualizar_interface.h
        source/ui/GerenciamentoLocalizadorQt/glqt_01_inicializar_tradutor_metadados.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_01_inicializar_tradutor_metadados.h
        source/ui/GerenciamentoLocalizadorQt/glqt_02_criar_mapa_compatibilidade.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_02_criar_mapa_compatibilidade.h
        source/ui/GerenciamentoLocalizadorQt/glqt_03_carregar_tradutor.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_03_carregar_tradutor.h
        source/ui/GerenciamentoLocalizadorQt/glqt_04_set_idioma.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_04_set_idioma.h
        source/ui/GerenciamentoLocalizadorQt/glqt_05_get_text.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_05_get_text.h
        source/ui/GerenciamentoLocalizadorQt/glqt_06_get_fallback_text.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_06_get_fallback_text.h
        source/ui/GerenciamentoLocalizadorQt/glqt_07_tr.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_07_tr.h
        source/ui/GerenciamentoLocalizadorQt/glqt_08_salvar_preferencia_idioma.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_08_salvar_preferencia_idioma.h
        source/ui/GerenciamentoLocalizadorQt/glqt_09_carregar_preferencia_idioma.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_09_carregar_preferencia_idioma.h
        source/ui/GerenciamentoLocalizadorQt/glqt_10_get_idiomas_disponiveis.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_10_get_idiomas_disponiveis.h
        source/ui/GerenciamentoLocalizadorQt/glqt_11_traduzir_tipo_operacao.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_11_traduzir_tipo_operacao.h
        source/ui/GerenciamentoLocalizadorQt/glqt_12_traduzir_metadados.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_12_traduzir_metadados.h
        source/ui/GerenciamentoLocalizadorQt/glqt_13_traduzir_tipo_operacao_fallback.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_13_traduzir_tipo_operacao_fallback.h
        source/ui/GerenciamentoLocalizadorQt/glqt_14_criar_arquivos_traducao.cpp
        source/ui/GerenciamentoLocalizadorQt/glqt_14_criar_arquivos_traducao.h
        source/ui/GerenciamentoMenusUI/gmui_01_MenuPersistente.cpp
        source/ui/GerenciamentoMenusUI/gmui_01_MenuPersistente.h
        source/ui/GerenciamentoMenusUI/gmui_02_GerenciadorCores.cpp
        source/ui/GerenciamentoMenusUI/gmui_02_GerenciadorCores.h
        source/ui/GerenciamentoMenusUI/gmui_03_SobreDialog.cpp
        source/ui/GerenciamentoMenusUI/gmui_03_SobreDialog.h
        source/ui/GerenciamentoMenusUI/gmui_04_criar_menu_principal.cpp
        source/ui/GerenciamentoMenusUI/gmui_04_criar_menu_principal.h
        source/ui/GerenciamentoMenusUI/gmui_05_configurar_menu_arquivo.cpp
        source/ui/GerenciamentoMenusUI/gmui_05_configurar_menu_arquivo.h
        source/ui/GerenciamentoMenusUI/gmui_06_configurar_menu_configuracoes.cpp
        source/ui/GerenciamentoMenusUI/gmui_06_configurar_menu_configuracoes.h
        source/ui/GerenciamentoMenusUI/gmui_07_configurar_menu_opcoes.cpp
        source/ui/GerenciamentoMenusUI/gmui_07_configurar_menu_opcoes.h
        source/ui/GerenciamentoMenusUI/gmui_08_criar_submenu_cores.cpp
        source/ui/GerenciamentoMenusUI/gmui_08_criar_submenu_cores.h
        source/ui/GerenciamentoMenusUI/gmui_09_criar_icone_cor.cpp
        source/ui/GerenciamentoMenusUI/gmui_09_criar_icone_cor.h
        source/ui/GerenciamentoMenusUI/gmui_10_abrir_dialogo_cor.cpp
        source/ui/GerenciamentoMenusUI/gmui_10_abrir_dialogo_cor.h
        source/ui/GerenciamentoMenusUI/gmui_11_criar_submenu_colunas.cpp
        source/ui/GerenciamentoMenusUI/gmui_11_criar_submenu_colunas.h
        source/ui/GerenciamentoMenusUI/gmui_12_alternar_ordenacao_linhas.cpp
        source/ui/GerenciamentoMenusUI/gmui_12_alternar_ordenacao_linhas.h
        source/ui/GerenciamentoMenusUI/gmui_13_criar_submenu_colunas_coloridas.cpp
        source/ui/GerenciamentoMenusUI/gmui_13_criar_submenu_colunas_coloridas.h
        source/ui/GerenciamentoMenusUI/gmui_14_selecionar_todas_colunas.cpp
        source/ui/GerenciamentoMenusUI/gmui_14_selecionar_todas_colunas.h
        source/ui/GerenciamentoMenusUI/gmui_15_selecionar_todas_cores_colunas.cpp
        source/ui/GerenciamentoMenusUI/gmui_15_selecionar_todas_cores_colunas.h
        source/ui/GerenciamentoMenusUI/gmui_16_criar_submenu_exportacao.cpp
        source/ui/GerenciamentoMenusUI/gmui_16_criar_submenu_exportacao.h
        source/ui/GerenciamentoMenusUI/gmui_17_resetar_opcoes_exportacao.cpp
        source/ui/GerenciamentoMenusUI/gmui_17_resetar_opcoes_exportacao.h
        source/ui/GerenciamentoMenusUI/gmui_18_criar_submenu_idiomas.cpp
        source/ui/GerenciamentoMenusUI/gmui_18_criar_submenu_idiomas.h
        source/ui/GerenciamentoMenusUI/gmui_19_get_texto_traduzido_para_idioma.cpp
        source/ui/GerenciamentoMenusUI/gmui_19_get_texto_traduzido_para_idioma.h
        source/ui/GerenciamentoMenusUI/gmui_20_confirmar_alteracao_idioma.cpp
        source/ui/GerenciamentoMenusUI/gmui_20_confirmar_alteracao_idioma.h
        source/ui/GerenciamentoMenusUI/gmui_21_on_traducoes_carregadas.cpp
        source/ui/GerenciamentoMenusUI/gmui_21_on_traducoes_carregadas.h
        source/ui/GerenciamentoMenusUI/gmui_22_exibir_sobre.cpp
        source/ui/GerenciamentoMenusUI/gmui_22_exibir_sobre.h
        source/ui/GerenciamentoMenusUI/gmui_24_exibir_manual.cpp
        source/ui/GerenciamentoMenusUI/gmui_24_exibir_manual.h
        source/ui/GerenciamentoMenusUI/gmui_25_exibir_ajuda.cpp
        source/ui/GerenciamentoMenusUI/gmui_25_exibir_ajuda.h
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_01_verificar_idioma_cache.cpp
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_01_verificar_idioma_cache.h
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_02_traduzir_tipo_operacao.cpp
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_02_traduzir_tipo_operacao.h
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_03_obter_chave_traducao_reversa.cpp
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_03_obter_chave_traducao_reversa.h
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_04_traduzir_metadados.cpp
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_04_traduzir_metadados.h
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_05_traduzir_tipo_arquivo.cpp
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_05_traduzir_tipo_arquivo.h
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_06_traduzir_atributos.cpp
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_06_traduzir_atributos.h
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_07_traduzir_autor.cpp
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_07_traduzir_autor.h
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_08_traduzir_protegido.cpp
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_08_traduzir_protegido.h
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_09_traduzir_dimensoes.cpp
        source/ui/GerenciamentoTradutorMetadadosQt/gtmqt_09_traduzir_dimensoes.h
        source/ui/Manual/common_Manual.cpp
        source/ui/Manual/common_Manual.h
        source/ui/Manual/DOC_DE_DE.cpp
        source/ui/Manual/DOC_DE_DE.h
        source/ui/Manual/DOC_EN_US.cpp
        source/ui/Manual/DOC_EN_US.h
        source/ui/Manual/DOC_ES_ES.cpp
        source/ui/Manual/DOC_ES_ES.h
        source/ui/Manual/DOC_FR_FR.cpp
        source/ui/Manual/DOC_FR_FR.h
        source/ui/Manual/DOC_IT_IT.cpp
        source/ui/Manual/DOC_IT_IT.h
        source/ui/Manual/DOC_PT_BR.cpp
        source/ui/Manual/DOC_PT_BR.h
        source/ui/AppChooserDialog.cpp
        source/ui/AppChooserDialog.h
        source/ui/ui_01_GerenciadorBotoes.cpp
        source/ui/ui_01_GerenciadorBotoes.h
        source/ui/ui_02_GerenciadorBotoesUI.cpp
        source/ui/ui_02_GerenciadorBotoesUI.h
        source/ui/ui_03_GerenciadorMenusUI.cpp
        source/ui/ui_03_GerenciadorMenusUI.h
        source/ui/ui_04_GerenciadorEventosUI.cpp
        source/ui/ui_04_GerenciadorEventosUI.h
        source/ui/ui_05_GerenciadorProgressoUI.cpp
        source/ui/ui_05_GerenciadorProgressoUI.h
        source/ui/ui_06_GerenciadorEstatisticasUI.cpp
        source/ui/ui_06_GerenciadorEstatisticasUI.h
        source/ui/ui_07_GerenciadorDados.cpp
        source/ui/ui_07_GerenciadorDados.h
        source/ui/ui_08_GerenciadorEventosArquivo.cpp
        source/ui/ui_08_GerenciadorEventosArquivo.h
        source/ui/ui_09_GerenciadorMonitoramento.cpp
        source/ui/ui_09_GerenciadorMonitoramento.h
        source/ui/ui_10_GerenciadorLimpeza.cpp
        source/ui/ui_10_GerenciadorLimpeza.h
        source/ui/ui_11_DialogoCores.cpp
        source/ui/ui_11_DialogoCores.h
        source/ui/ui_12_LocalizadorQt.cpp
        source/ui/ui_12_LocalizadorQt.h
        source/ui/ui_13_TradutorMetadadosQt.cpp
        source/ui/ui_13_TradutorMetadadosQt.h
        source/ui/ui_14_OpcoesSobre.cpp
        source/ui/ui_14_OpcoesSobre.h
        source/ui/ui_15_Manual.cpp
        source/ui/ui_15_Manual.h
        source/utils/ApplicationPathUtils.cpp
        source/utils/ApplicationPathUtils.h
        source/utils/AssociatedApps.cpp
        source/utils/AssociatedApps.h
        source/utils/CaminhoPersistenteUtils.cpp
        source/utils/CaminhoPersistenteUtils.h
        source/utils/IconUtils.cpp
        source/utils/IconUtils.h
        source/utils/LogManager.cpp
        source/utils/LogManager.h
        source/src_01_InicializadorMain.cpp
        source/src_01_InicializadorMain.h
        source/src_02_SplashAppStarter.cpp
        source/src_02_SplashAppStarter.h
        main.cpp
        MANUAL.md
        OBSERVACAO.md
        README.md)

target_link_libraries(Linceu_Lighthouse_cpp PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::OpenGL
        Qt${QT_VERSION_MAJOR}::OpenGLWidgets
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Qml)

if(WIN32)
        target_link_libraries(Linceu_Lighthouse_cpp PRIVATE Shlwapi Shell32)
endif()

set_target_properties(Linceu_Lighthouse_cpp PROPERTIES
        OUTPUT_NAME "${LINCEU_OUTPUT_NAME}")

add_custom_command(TARGET Linceu_Lighthouse_cpp POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_SOURCE_DIR}/source/assets"
                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/assets"
        COMMAND "${CMAKE_COMMAND}" -E make_directory
                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/locale"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                ${LINCEU_APP_TRANSLATION_FILES}
                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/locale"
        ${LINCEU_PREPARE_QT_TRANSLATIONS_TO_TARGET_COMMANDS}
        COMMENT "Copying Linceu runtime assets and translations next to $<TARGET_FILE_NAME:Linceu_Lighthouse_cpp>"
        COMMAND_EXPAND_LISTS
        VERBATIM)

if(MSVC)
    target_compile_options(Linceu_Lighthouse_cpp PRIVATE
        $<$<CONFIG:Release>:/Ox>
        $<$<CONFIG:Release>:/fp:fast>
        $<$<CONFIG:Release>:/Gw>
        $<$<CONFIG:Release>:/Zc:inline>
        $<$<CONFIG:RelWithDebInfo>:/Ox>
        $<$<CONFIG:RelWithDebInfo>:/fp:fast>
        $<$<CONFIG:RelWithDebInfo>:/Gw>
        $<$<CONFIG:RelWithDebInfo>:/Zc:inline>
    )
    
    set_property(TARGET Linceu_Lighthouse_cpp PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    set_property(TARGET Linceu_Lighthouse_cpp PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)

    add_custom_command(TARGET Linceu_Lighthouse_cpp POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E rm -f
                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/vc_redist.x64.exe"
        ${LINCEU_REMOVE_UNUSED_MSVC_RUNTIME_FROM_TARGET_COMMANDS}
        ${LINCEU_COPY_MSVC_RUNTIME_TO_TARGET_COMMANDS}
        COMMENT "Preparing MSVC runtime next to $<TARGET_FILE_NAME:Linceu_Lighthouse_cpp>"
        COMMAND_EXPAND_LISTS
        VERBATIM)
endif()

if(MINGW)
    target_compile_options(Linceu_Lighthouse_cpp PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-ffunction-sections>
        $<$<CONFIG:Release>:-fdata-sections>
        $<$<CONFIG:MinSizeRel>:-Os>
        $<$<CONFIG:MinSizeRel>:-ffunction-sections>
        $<$<CONFIG:MinSizeRel>:-fdata-sections>)

    target_link_options(Linceu_Lighthouse_cpp PRIVATE
        $<$<CONFIG:Release>:-Wl,--gc-sections>
        $<$<CONFIG:Release>:-Wl,--strip-all>
        $<$<CONFIG:MinSizeRel>:-Wl,--gc-sections>
        $<$<CONFIG:MinSizeRel>:-Wl,--strip-all>)

    if(CMAKE_STRIP AND (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
        add_custom_command(TARGET Linceu_Lighthouse_cpp POST_BUILD
                COMMAND "${CMAKE_STRIP}" --strip-unneeded "$<TARGET_FILE:Linceu_Lighthouse_cpp>"
                COMMENT "Stripping MinGW executable symbols from $<TARGET_FILE_NAME:Linceu_Lighthouse_cpp>"
                VERBATIM)
    endif()
endif()

set(LINCEU_CLI_DEPLOY_COMMAND)
if(BUILD_CLI)
        set(LINCEU_CLI_SOURCES
                source/cli/main_cli.cpp
                ${LINCEU_WINDOWS_RESOURCES}
                source/security/secore_01_postgres_utils.cpp
                source/security/secore_01_postgres_utils.h
                source/security/secore_02_SecurityManager.cpp
                source/security/secore_02_SecurityManager.h
                source/security/secore_03_evento_campos.cpp
                source/security/secore_03_evento_campos.h
                source/security/secore_05_SecurityDbWriter.cpp
                source/security/secore_05_SecurityDbWriter.h
                source/security/base_01_baseline_criptografico.cpp
                source/security/base_01_baseline_criptografico.h
                source/security/agre_01_motor_regras.cpp
                source/security/agre_01_motor_regras.h
                source/security/risk_01_score_risco.cpp
                source/security/risk_01_score_risco.h
                source/security/quar_01_quarentena.cpp
                source/security/quar_01_quarentena.h
                source/security/evid_01_exportacao_evidencia.cpp
                source/security/evid_01_exportacao_evidencia.h
                source/security/exec_01_inteligencia_executaveis.cpp
                source/security/exec_01_inteligencia_executaveis.h
                source/security/mdir_01_multi_diretorio.cpp
                source/security/mdir_01_multi_diretorio.h
                source/security/sdiff_01_diff_conteudo.cpp
                source/security/sdiff_01_diff_conteudo.h
                source/security/sess_01_correlacao_sessoes.cpp
                source/security/sess_01_correlacao_sessoes.h
                source/security/attr_01_atribuicao_forense.cpp
                source/security/attr_01_atribuicao_forense.h
                source/services/GerenciamentoMetadados/gmet_10_ExtrairMetadadosExecutavel.cpp
                source/services/GerenciamentoMetadados/gmet_10_ExtrairMetadadosExecutavel.h
                source/utils/ApplicationPathUtils.cpp
                source/utils/ApplicationPathUtils.h
                source/utils/CaminhoPersistenteUtils.cpp
                source/utils/CaminhoPersistenteUtils.h
                source/utils/LogManager.cpp
                source/utils/LogManager.h
        )

        add_executable(linceu-lighthouse-cli
                ${LINCEU_CLI_SOURCES}
        )

        target_compile_definitions(linceu-lighthouse-cli PRIVATE LINCEU_CLI_HEADLESS)

        target_include_directories(linceu-lighthouse-cli PRIVATE
                ${CMAKE_SOURCE_DIR}
                source
                source/security
                source/services
                source/utils
        )

        target_link_libraries(linceu-lighthouse-cli PRIVATE
                Qt${QT_VERSION_MAJOR}::Core
                Qt${QT_VERSION_MAJOR}::Sql
                Qt${QT_VERSION_MAJOR}::Xml
        )

        if(WIN32)
                target_link_libraries(linceu-lighthouse-cli PRIVATE
                        Shlwapi
                        Shell32
                        Advapi32
                        wintrust
                        version
                        crypt32
                )
        endif()

        set_target_properties(linceu-lighthouse-cli PROPERTIES
                OUTPUT_NAME "linceu-lighthouse-cli"
        )

        if(MSVC)
                target_compile_options(linceu-lighthouse-cli PRIVATE
                        $<$<CONFIG:Release>:/Ox /fp:fast /Gw /Zc:inline>
                        $<$<CONFIG:RelWithDebInfo>:/Ox /fp:fast /Gw /Zc:inline>
                )
        endif()

        if(MINGW)
                target_compile_options(linceu-lighthouse-cli PRIVATE
                        $<$<CONFIG:Release>:-O3 -ffunction-sections -fdata-sections>
                        $<$<CONFIG:MinSizeRel>:-Os -ffunction-sections -fdata-sections>
                )
                target_link_options(linceu-lighthouse-cli PRIVATE
                        $<$<CONFIG:Release>:-Wl,--gc-sections -Wl,--strip-all>
                        $<$<CONFIG:MinSizeRel>:-Wl,--gc-sections -Wl,--strip-all>
                )
        endif()

        set(LINCEU_CLI_DEPLOY_COMMAND
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "$<TARGET_FILE:linceu-lighthouse-cli>"
                        "${LINCEU_DEPLOY_DIR}/$<TARGET_FILE_NAME:linceu-lighthouse-cli>"
        )
endif()

if (WIN32)
        target_link_libraries(Linceu_Lighthouse_cpp PRIVATE wintrust version crypt32)

        set(LINCEU_WINDEPLOYQT_COMMAND "")
        if(WINDEPLOYQT_EXECUTABLE AND EXISTS "${WINDEPLOYQT_EXECUTABLE}")
                set(LINCEU_WINDEPLOYQT_COMMAND "${WINDEPLOYQT_EXECUTABLE}")
        elseif(TARGET Qt${QT_VERSION_MAJOR}::windeployqt)
                set(LINCEU_WINDEPLOYQT_COMMAND "$<TARGET_FILE:Qt${QT_VERSION_MAJOR}::windeployqt>")
        endif()

        if (LINCEU_WINDEPLOYQT_COMMAND)
                add_custom_target(Linceu_Lighthouse_deploy
                        COMMAND "${CMAKE_COMMAND}" -E rm -f
                                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/Linceu_Lighthouse_cpp.exe"
                        COMMAND "${CMAKE_COMMAND}" -E rm -f
                                "${LINCEU_DEPLOY_DIR}/Linceu_Lighthouse_cpp.exe"
                        COMMAND "${CMAKE_COMMAND}" -E make_directory "${LINCEU_DEPLOY_DIR}"
                        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                                "$<TARGET_FILE:Linceu_Lighthouse_cpp>"
                                "${LINCEU_DEPLOY_DIR}/$<TARGET_FILE_NAME:Linceu_Lighthouse_cpp>"
                        ${LINCEU_CLI_DEPLOY_COMMAND}
                        COMMAND "${LINCEU_WINDEPLOYQT_COMMAND}"
                                ${LINCEU_WINDEPLOYQT_RUNTIME_ARGS}
                                --dir "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>"
                                "$<TARGET_FILE:Linceu_Lighthouse_cpp>"
                        COMMAND "${CMAKE_COMMAND}" -E rm -f
                                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/vc_redist.x64.exe"
                        ${LINCEU_REMOVE_UNUSED_MSVC_RUNTIME_FROM_TARGET_COMMANDS}
                        ${LINCEU_COPY_MSVC_RUNTIME_TO_TARGET_COMMANDS}
                        COMMAND "${CMAKE_COMMAND}" -E copy_directory
                                "${CMAKE_SOURCE_DIR}/source/assets"
                                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/assets"
                        COMMAND "${CMAKE_COMMAND}" -E make_directory
                                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/locale"
                        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                                ${LINCEU_APP_TRANSLATION_FILES}
                                "$<TARGET_FILE_DIR:Linceu_Lighthouse_cpp>/locale"
                        ${LINCEU_PREPARE_QT_TRANSLATIONS_TO_TARGET_COMMANDS}
                        COMMAND "${LINCEU_WINDEPLOYQT_COMMAND}"
                                ${LINCEU_WINDEPLOYQT_RUNTIME_ARGS}
                                --dir "${LINCEU_DEPLOY_DIR}"
                                "${LINCEU_DEPLOY_DIR}/$<TARGET_FILE_NAME:Linceu_Lighthouse_cpp>"
                        COMMAND "${CMAKE_COMMAND}" -E rm -f
                                "${LINCEU_DEPLOY_DIR}/vc_redist.x64.exe"
                        ${LINCEU_REMOVE_UNUSED_MSVC_RUNTIME_FROM_DEPLOY_COMMANDS}
                        ${LINCEU_COPY_MSVC_RUNTIME_TO_DEPLOY_COMMANDS}
                        COMMAND "${CMAKE_COMMAND}" -E copy_directory
                                "${CMAKE_SOURCE_DIR}/source/assets"
                                "${LINCEU_DEPLOY_DIR}/assets"
                        COMMAND "${CMAKE_COMMAND}" -E make_directory "${LINCEU_DEPLOY_DIR}/locale"
                        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                                ${LINCEU_APP_TRANSLATION_FILES}
                                "${LINCEU_DEPLOY_DIR}/locale"
                        ${LINCEU_PREPARE_QT_TRANSLATIONS_TO_DEPLOY_COMMANDS}
                        COMMENT "Deploying Qt runtime dependencies and packaging runnable copy for ${LINCEU_OUTPUT_NAME}"
                        COMMAND_EXPAND_LISTS
                        VERBATIM)
                add_dependencies(Linceu_Lighthouse_deploy Linceu_Lighthouse_cpp)
                if(BUILD_CLI)
                        add_dependencies(Linceu_Lighthouse_deploy linceu-lighthouse-cli)
                endif()
        else()
                message(WARNING "Qt windeployqt target not available; Qt runtime dependencies will not be copied automatically.")
        endif()
endif()
