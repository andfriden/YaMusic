cmake_minimum_required(VERSION 3.16)

project(YaMusic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
        Qt6
        REQUIRED
        COMPONENTS
        Quick
        Network
        Multimedia
)

qt_standard_project_setup(REQUIRES 6.10)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appYaMusic

        main.cpp

        # Core
        src/Core/AppController.h
        src/Core/AppController.cpp

        # Player
        src/Player/PlayerService.h
        src/Player/PlayerService.cpp

        # Models
        src/Models/Track.h
        src/Models/Track.cpp

        src/Models/Account.h
        src/Models/Account.cpp

        # Yandex
        src/Yandex/YandexClient.h
        src/Yandex/YandexClient.cpp

        src/Yandex/AccountParser.h
        src/Yandex/AccountParser.cpp

        # Auth
        src/Yandex/Auth/YandexAuth.h
        src/Yandex/Auth/YandexAuth.cpp

        src/Yandex/Auth/YandexTokenStorage.h
        src/Yandex/Auth/YandexTokenStorage.cpp

        # Account
        src/Yandex/Account/AccountService.h
        src/Yandex/Account/AccountService.cpp

        # Catalog
        src/Yandex/Catalog/SearchResult.h
        src/Yandex/Catalog/SearchResult.cpp

        src/Yandex/Catalog/SearchParser.h
        src/Yandex/Catalog/SearchParser.cpp

        src/Yandex/Catalog/SearchService.h
        src/Yandex/Catalog/SearchService.cpp

        src/Yandex/Catalog/SearchModel.h
        src/Yandex/Catalog/SearchModel.cpp

        src/Yandex/Catalog/CoverImageProvider.h
        src/Yandex/Catalog/CoverImageProvider.cpp

        src/Yandex/Catalog/TrackService.h
        src/Yandex/Catalog/TrackService.cpp
        src/Playback/PlaybackController.cpp
        src/Playback/PlaybackController.h
)

qt_add_qml_module(appYaMusic
        URI YaMusic

        QML_FILES
        Main.qml
)

# macOS application bundle
set_target_properties(appYaMusic PROPERTIES

        MACOSX_BUNDLE TRUE

        MACOSX_BUNDLE_BUNDLE_VERSION
        ${PROJECT_VERSION}

        MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}

        WIN32_EXECUTABLE TRUE
)

target_link_libraries(appYaMusic
        PRIVATE

        Qt6::Quick
        Qt6::Network
        Qt6::Multimedia
)

# macOS Security framework
if (APPLE)

    find_library(
            SECURITY_FRAMEWORK
            Security
    )

    target_link_libraries(
            appYaMusic
            PRIVATE
            ${SECURITY_FRAMEWORK}
    )

endif()

install(
        TARGETS appYaMusic
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
        TARGET appYaMusic
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)

install(
        SCRIPT ${deploy_script}
)