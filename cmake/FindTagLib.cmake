# FindTagLib.cmake — поиск библиотеки TagLib (редактирование тегов/артворка).
#
# Задаёт импортированный таргет TagLib::TagLib (псевдоним локального
# TagLib::tag) и переменные TagLib_VERSION / TagLib_FOUND.
#
# Источники (по приоритету):
#   1. CMake-конфиг от библиотеки (distro/vcpkg/HB) — include(CMakeFindDependencyMacro)
#      не нужен, пробуем штатный find_package без REQUIRED, чтобы не упасть.
#   2. pkg-config (taglib.pc).
#   3. Прямой поиск библиотеки/заголовков по CMAKE_PREFIX_PATH.
#
# Довод: упаковки по-разному называют таргет (TagLib::tag, TagLib::TagLib),
# поэтому здесь мы нормализуем всё к TagLib::TagLib.

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_TAGLIB QUIET taglib)
endif()

# Стандартная опция — путь к конфигу от дистрибутива.
find_path(
    TagLib_CMAKE_DIR
    NAMES taglib-config.cmake
    HINTS
        ${PC_TAGLIB_DIR}/lib/cmake/taglib
        "/usr/local/opt/taglib/lib/cmake/taglib"
        "${CMAKE_PREFIX_PATH}/lib/cmake/taglib"
        "${CMAKE_PREFIX_PATH}/share/taglib"
    PATH_SUFFIXES lib/cmake/taglib share/taglib
)

if(TagLib_CMAKE_DIR AND EXISTS "${TagLib_CMAKE_DIR}/taglib-config.cmake")
    list(APPEND CMAKE_PREFIX_PATH "${TagLib_CMAKE_DIR}/../..")
    find_package(TagLib QUIET NO_MODULE PATHS "${TagLib_CMAKE_DIR}" NO_DEFAULT_PATH)
    if(TAGLIB_FOUND OR TagLib_FOUND)
        set(TagLib_FOUND TRUE)
        if(TARGET TagLib::tag AND NOT TARGET TagLib::TagLib)
            add_library(TagLib::TagLib INTERFACE IMPORTED)
            set_target_properties(TagLib::TagLib PROPERTIES INTERFACE_LINK_LIBRARIES TagLib::tag)
        endif()
        if(NOT TagLib_VERSION AND DEFINED TagLib_VERSION)
            set(TagLib_VERSION "${TagLib_VERSION}")
        endif()
        return()
    endif()
endif()

# pkg-config фолбэк.
if(PC_TAGLIB_FOUND)
    add_library(TagLib::TagLib UNKNOWN IMPORTED)
    set_target_properties(TagLib::TagLib PROPERTIES
        IMPORTED_LOCATION "${PC_TAGLIB_LINK_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${PC_TAGLIB_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${PC_TAGLIB_LIBRARIES}"
    )
    set(TagLib_VERSION "${PC_TAGLIB_VERSION}")
    set(TagLib_FOUND TRUE)
    return()
endif()

# Прямой поиск как последний шанс.
find_path(TagLib_INCLUDE_DIR taglib/tag.h
    HINTS ${PC_TAGLIB_INCLUDE_DIRS} "/usr/local/include" "/usr/include")
find_library(TagLib_LIBRARY NAMES tag
    HINTS ${PC_TAGLIB_LIBRARY_DIRS} "/usr/local/lib" "/usr/lib")

if(TagLib_INCLUDE_DIR AND TagLib_LIBRARY)
    add_library(TagLib::TagLib UNKNOWN IMPORTED)
    set_target_properties(TagLib::TagLib PROPERTIES
        IMPORTED_LOCATION "${TagLib_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${TagLib_INCLUDE_DIR}"
    )
    set(TagLib_FOUND TRUE)
endif()

if(TagLib_FOUND AND NOT TagLib_VERSION)
    set(TagLib_VERSION "unknown")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    TagLib
    REQUIRED_VARS TagLib_INCLUDE_DIR TagLib_LIBRARY
    VERSION_VAR TagLib_VERSION
)