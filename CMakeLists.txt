cmake_minimum_required(VERSION 3.21)

project(
        YaMusic
        VERSION 0.1
        LANGUAGES CXX
)


# =============================================================
# Project settings
# =============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# =============================================================
# Qt
# =============================================================

find_package(
        Qt6 6.10
        REQUIRED COMPONENTS
        Quick
        Network
        Multimedia
)

qt_standard_project_setup(REQUIRES 6.10)


# =============================================================
# Optional imported content
# =============================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(importedcontent)
endif()


# =============================================================
# Paths
# =============================================================

set(
        YAMUSIC_ROOT
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(
        YAMUSIC_QML_ROOT
        "${YAMUSIC_ROOT}/qml"
)

set(
        YAMUSIC_DATA_ROOT
        "${YAMUSIC_ROOT}/data"
)

set(
        YAMUSIC_RESOURCES_ROOT
        "${YAMUSIC_ROOT}/resources"
)


# =============================================================
# Icons
# =============================================================

set(
        YAMUSIC_ICON_ICNS
        "${YAMUSIC_RESOURCES_ROOT}/YaMusic.icns"
)

set(
        YAMUSIC_ICON_ICO
        "${YAMUSIC_RESOURCES_ROOT}/YaMusic.ico"
)

set(
        YAMUSIC_ICON_SVG
        "${YAMUSIC_RESOURCES_ROOT}/YaMusic.svg"
)

# =============================================================
# Windows icon resource
# =============================================================

if(WIN32 AND EXISTS "${YAMUSIC_ICON_ICO}")

    set(
            YAMUSIC_RC
            "${CMAKE_CURRENT_BINARY_DIR}/YaMusic.rc"
    )

    file(
            WRITE
            "${YAMUSIC_RC}"
            "IDI_ICON1 ICON DISCARDABLE \"${YAMUSIC_ICON_ICO}\"\n"
    )

endif()


# =============================================================
# macOS icon
# =============================================================

if(APPLE AND EXISTS "${YAMUSIC_ICON_ICNS}")

    set_source_files_properties(
            "${YAMUSIC_ICON_ICNS}"
            PROPERTIES
            MACOSX_PACKAGE_LOCATION
            "Resources"
    )

endif()


# =============================================================
# C++ sources
# =============================================================

set(
        YAMUSIC_SOURCES

        main.cpp

        # Core
        src/Core/AppController.cpp
        src/Core/AppController+Catalog.cpp
        src/Core/AppController+Library.cpp
        src/Core/AppController+Personal.cpp
        src/Core/AppController+Playback.cpp
        src/Core/AppController+Search.cpp
        src/Core/AppController+Selection.cpp

        src/Core/AlbumController.cpp
        src/Core/ArtistController.cpp
        src/Core/ChartController.cpp
        src/Core/GenreController.cpp
        src/Core/LibraryController.cpp
        src/Core/PersonalController.cpp
        src/Core/PersonalController+MyWave.cpp
        src/Core/PersonalController+Playback.cpp
        src/Core/PersonalController+RecentlyPlayed.cpp
        src/Core/PersonalController+Recommendations.cpp
        src/Core/PlayerAccentService.cpp
        src/Core/SearchController.cpp
        src/Core/AuthController.cpp

        # Models
        src/Models/Account.cpp
        src/Models/PersonalPlaylist.cpp
        src/Models/Playlist.cpp
        src/Models/Track.cpp

        # Player / playback
        src/Player/PlayerService.cpp
        src/Playback/PlaybackController.cpp

        # Queue
        src/Queue/QueueModel.cpp
        src/Queue/QueueService.cpp

        # Yandex
        src/Yandex/AccountParser.cpp
        src/Yandex/YandexClient.cpp

        # Yandex Account
        src/Yandex/Account/AccountService.cpp

        # Yandex Auth
        src/Yandex/Auth/YandexAuth.cpp
        src/Yandex/Auth/YandexTokenStorage.cpp

        # Yandex Catalog
        src/Yandex/Catalog/AlbumModel.cpp
        src/Yandex/Catalog/AlbumService.cpp
        src/Yandex/Catalog/ArtistAlbumsModel.cpp
        src/Yandex/Catalog/ArtistModel.cpp
        src/Yandex/Catalog/ArtistService.cpp
        src/Yandex/Catalog/ChartModel.cpp
        src/Yandex/Catalog/ChartService.cpp
        src/Yandex/Catalog/CoverImageProvider.cpp
        src/Yandex/Catalog/GenreModel.cpp
        src/Yandex/Catalog/GenreService.cpp
        src/Yandex/Catalog/SearchModel.cpp
        src/Yandex/Catalog/SearchParser.cpp
        src/Yandex/Catalog/SearchResult.cpp
        src/Yandex/Catalog/SearchService.cpp
        src/Yandex/Catalog/SimilarArtistsModel.cpp
        src/Yandex/Catalog/TrackService.cpp

        # Yandex Personal
        src/Yandex/Personal/LikedTracksModel.cpp
        src/Yandex/Personal/LikesService.cpp
        src/Yandex/Personal/LibraryPlaylistsModel.cpp
        src/Yandex/Personal/MyWaveModel.cpp
        src/Yandex/Personal/NewPlaylistsService.cpp
        src/Yandex/Personal/PersonalChartModel.cpp
        src/Yandex/Personal/PersonalLanding.cpp
        src/Yandex/Personal/PersonalPlaylistsModel.cpp
        src/Yandex/Personal/PlaylistModel.cpp
        src/Yandex/Personal/PlaylistService.cpp
        src/Yandex/Personal/RecentListeningModel.cpp
        src/Yandex/Personal/RecentListeningService.cpp
        src/Yandex/Personal/YandexPersonal.cpp
        src/Core/AuthController.cpp
        src/Core/AuthController.h
)


# =============================================================
# Executable
# =============================================================

qt_add_executable(
        appYaMusic
        ${YAMUSIC_SOURCES}
)


# =============================================================
# Platform resources
# =============================================================

if(WIN32 AND EXISTS "${YAMUSIC_RC}")

    target_sources(
            appYaMusic
            PRIVATE
            "${YAMUSIC_RC}"
    )

endif()


if(APPLE AND EXISTS "${YAMUSIC_ICON_ICNS}")

    target_sources(
            appYaMusic
            PRIVATE
            "${YAMUSIC_ICON_ICNS}"
    )

endif()


# =============================================================
# Main QML
#
# Main.qml is located in the project root.
# =============================================================

set_source_files_properties(
        "${YAMUSIC_ROOT}/Main.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Main.qml"
)


# =============================================================
# QML files
# =============================================================

set(
        YAMUSIC_QML_FILES

        # Main
        "${YAMUSIC_ROOT}/Main.qml"

        # Components
        "${YAMUSIC_QML_ROOT}/Components/SearchBar.qml"
        "${YAMUSIC_QML_ROOT}/Components/StatusBar.qml"
        "${YAMUSIC_QML_ROOT}/Components/NowPlayingBar.qml"
        "${YAMUSIC_QML_ROOT}/Components/EntityLink.qml"
        "${YAMUSIC_QML_ROOT}/Components/ExpandedNowPlaying.qml"
        "${YAMUSIC_QML_ROOT}/Components/ChartTrackRow.qml"
        "${YAMUSIC_QML_ROOT}/Components/PersonalChartSection.qml"

        # Pages
        "${YAMUSIC_QML_ROOT}/Pages/HomePage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/SearchPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/MyWavePage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/LibraryPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/AlbumPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/ArtistPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/PlaylistPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/RecentPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/ChartPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/PlaylistsPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/GenresPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/GenrePage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/SportPage.qml"
        "${YAMUSIC_QML_ROOT}/Pages/LoginPage.qml"

        # Home
        "${YAMUSIC_QML_ROOT}/Home/PersonalPlaylistsSection.qml"
        "${YAMUSIC_QML_ROOT}/Home/PlaylistView.qml"
        "${YAMUSIC_QML_ROOT}/Home/RecentListeningSection.qml"

        # My Wave
        "${YAMUSIC_QML_ROOT}/MyWave/MyWaveSection.qml"

        # Search
        "${YAMUSIC_QML_ROOT}/Search/SearchResultsSection.qml"

        # Context
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelMyWave.qml"
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelArtist.qml"
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelAlbum.qml"
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelLibrary.qml"

        # Layout
        "${YAMUSIC_QML_ROOT}/Layout/ContextPanel.qml"
        "${YAMUSIC_QML_ROOT}/Layout/MainLayout.qml"
        "${YAMUSIC_QML_ROOT}/Layout/Sidebar.qml"
        "${YAMUSIC_QML_ROOT}/Layout/TopBar.qml"
        "${YAMUSIC_QML_ROOT}/Layout/NavigationController.qml"

        # Theme
        "${YAMUSIC_QML_ROOT}/Theme/AppTheme.qml"
)


# =============================================================
# QML resource aliases
# =============================================================

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Components/SearchBar.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Components/SearchBar.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Components/StatusBar.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Components/StatusBar.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Components/NowPlayingBar.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Components/NowPlayingBar.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Components/EntityLink.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Components/EntityLink.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Components/ExpandedNowPlaying.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Components/ExpandedNowPlaying.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Components/ChartTrackRow.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Components/ChartTrackRow.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Components/PersonalChartSection.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Components/PersonalChartSection.qml"
)


set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/HomePage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/HomePage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/SearchPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/SearchPage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/MyWavePage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/MyWavePage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/LibraryPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/LibraryPage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/AlbumPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/AlbumPage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/ArtistPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/ArtistPage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/PlaylistPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/PlaylistPage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/RecentPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/RecentPage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/ChartPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/ChartPage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/PlaylistsPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/PlaylistsPage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/GenresPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/GenresPage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/GenrePage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/GenrePage.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/SportPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/SportPage.qml"
)


set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Home/PersonalPlaylistsSection.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Home/PersonalPlaylistsSection.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Home/PlaylistView.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Home/PlaylistView.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Home/RecentListeningSection.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Home/RecentListeningSection.qml"
)


set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/MyWave/MyWaveSection.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "MyWave/MyWaveSection.qml"
)


set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Search/SearchResultsSection.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Search/SearchResultsSection.qml"
)


set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelHome.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Context/ContextPanelHome.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelMyWave.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Context/ContextPanelMyWave.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelArtist.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Context/ContextPanelArtist.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelAlbum.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Context/ContextPanelAlbum.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelPlaylist.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Context/ContextPanelPlaylist.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Context/ContextPanelLibrary.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Context/ContextPanelLibrary.qml"
)


set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Layout/ContextPanel.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Layout/ContextPanel.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Layout/MainLayout.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Layout/MainLayout.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Layout/Sidebar.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Layout/Sidebar.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Layout/TopBar.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Layout/TopBar.qml"
)

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Layout/NavigationController.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Layout/NavigationController.qml"
)


set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Pages/LoginPage.qml"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "Pages/LoginPage.qml"
)

# =============================================================
# AppTheme singleton
# =============================================================

set_source_files_properties(
        "${YAMUSIC_QML_ROOT}/Theme/AppTheme.qml"
        PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
        QT_RESOURCE_ALIAS
        "Theme/AppTheme.qml"
)


# =============================================================
# CSV resource
# =============================================================

set(
        YAMUSIC_GENRE_PLAYLISTS_CSV
        "${YAMUSIC_DATA_ROOT}/genre_playlists.csv"
)

set_source_files_properties(
        "${YAMUSIC_GENRE_PLAYLISTS_CSV}"
        PROPERTIES
        QT_RESOURCE_ALIAS
        "genre_playlists.csv"
)


# =============================================================
# QML module
# =============================================================

qt_add_qml_module(
        appYaMusic

        URI
        YaMusic

        VERSION
        1.0

        RESOURCE_PREFIX
        "/qt/qml"

        RESOURCES
        "${YAMUSIC_GENRE_PLAYLISTS_CSV}"

        QML_FILES
        ${YAMUSIC_QML_FILES}
)


# =============================================================
# Output name
# =============================================================

if(UNIX AND NOT APPLE)

    set(
            YAMUSIC_OUTPUT_NAME
            "yamusic"
    )

else()

    set(
            YAMUSIC_OUTPUT_NAME
            "YaMusic"
    )

endif()


# =============================================================
# Target properties
# =============================================================

set_target_properties(
        appYaMusic
        PROPERTIES

        OUTPUT_NAME
        "${YAMUSIC_OUTPUT_NAME}"

        MACOSX_BUNDLE
        TRUE

        MACOSX_BUNDLE_BUNDLE_NAME
        "YaMusic"

        MACOSX_BUNDLE_BUNDLE_VERSION
        "${PROJECT_VERSION}"

        MACOSX_BUNDLE_SHORT_VERSION_STRING
        "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"

        MACOSX_BUNDLE_GUI_IDENTIFIER
        "my.appyamusic"

        WIN32_EXECUTABLE
        TRUE
)


# =============================================================
# macOS bundle icon
# =============================================================

if(APPLE AND EXISTS "${YAMUSIC_ICON_ICNS}")

    set_target_properties(
            appYaMusic
            PROPERTIES
            MACOSX_BUNDLE_ICON_FILE
            "YaMusic.icns"
    )

endif()


# =============================================================
# Libraries
# =============================================================

target_link_libraries(
        appYaMusic
        PRIVATE

        Qt6::Quick
        Qt6::Network
        Qt6::Multimedia
)


# =============================================================
# macOS frameworks
# =============================================================

if(APPLE)

    find_library(
            SECURITY_FRAMEWORK
            Security
    )

    if(SECURITY_FRAMEWORK)

        target_link_libraries(
                appYaMusic
                PRIVATE
                "${SECURITY_FRAMEWORK}"
        )

    endif()

endif()


# =============================================================
# Install target
# =============================================================

install(
        TARGETS
        appYaMusic

        BUNDLE
        DESTINATION
        .

        RUNTIME
        DESTINATION
        bin
)


# =============================================================
## =============================================================
## Linux desktop entry and icon
## =============================================================
#
#if(UNIX AND NOT APPLE)
#
#    set(
#            YAMUSIC_DESKTOP_FILE
#            "${CMAKE_CURRENT_BINARY_DIR}/YaMusic.desktop"
#    )
#
#    file(
#            WRITE
#            "${YAMUSIC_DESKTOP_FILE}"
#
#            "[Desktop Entry]\n"
#            "Type=Application\n"
#            "Name=YaMusic\n"
#            "Comment=Yandex Music desktop client\n"
#            "Exec=yamusic\n"
#            "Icon=yamusic\n"
#            "Terminal=false\n"
#            "Categories=AudioVideo;Audio;Music;\n"
#    )
#
#    install(
#            FILES
#            "${YAMUSIC_DESKTOP_FILE}"
#
#            DESTINATION
#            share/applications
#    )
#
#    if(EXISTS "${YAMUSIC_ICON_SVG}")
#
#        install(
#                FILES
#                "${YAMUSIC_ICON_SVG}"
#
#                DESTINATION
#                share/icons/hicolor/scalable/apps
#
#                RENAME
#                yamusic.svg
#        )
#
#    endif()
#
#endif()

# =============================================================
# Qt deployment
# =============================================================

qt_generate_deploy_qml_app_script(
        TARGET
        appYaMusic

        OUTPUT_SCRIPT
        deploy_script

        NO_UNSUPPORTED_PLATFORM_ERROR
)

install(
        SCRIPT
        "${deploy_script}"
)