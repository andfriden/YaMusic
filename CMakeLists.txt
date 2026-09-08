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
# Icons
# =============================================================

set(
        YAMUSIC_ICON_ICNS
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/YaMusic.icns"
)

set(
        YAMUSIC_ICON_ICO
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/YaMusic.ico"
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
            MACOSX_PACKAGE_LOCATION "Resources"
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
)


# =============================================================
# Executable
# =============================================================

qt_add_executable(
        appYaMusic

        ${YAMUSIC_SOURCES}

        $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${YAMUSIC_RC}>>:${YAMUSIC_RC}>

        $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${YAMUSIC_ICON_ICNS}>>:${YAMUSIC_ICON_ICNS}>
)


# =============================================================
# QML singleton
# =============================================================

set_source_files_properties(
        qml/Theme/AppTheme.qml
        PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
)


# =============================================================
# QML module
# =============================================================

qt_add_qml_module(
        appYaMusic

        URI YaMusic
        VERSION 1.0

        RESOURCES
        data/genre_playlists.csv

        QML_FILES

        qml/Main.qml

        qml/Components/SearchBar.qml
        qml/Components/StatusBar.qml
        qml/Components/NowPlayingBar.qml
        qml/Components/EntityLink.qml
        qml/Components/ExpandedNowPlaying.qml
        qml/Components/ChartTrackRow.qml
        qml/Components/PersonalChartSection.qml

        qml/Pages/HomePage.qml
        qml/Pages/SearchPage.qml
        qml/Pages/MyWavePage.qml
        qml/Pages/LibraryPage.qml
        qml/Pages/AlbumPage.qml
        qml/Pages/ArtistPage.qml
        qml/Pages/PlaylistPage.qml
        qml/Pages/RecentPage.qml
        qml/Pages/ChartPage.qml
        qml/Pages/PlaylistsPage.qml
        qml/Pages/GenresPage.qml
        qml/Pages/GenrePage.qml
        qml/Pages/SubGenrePage.qml
        qml/Pages/SportPage.qml

        qml/Home/PersonalPlaylistsSection.qml
        qml/Home/PlaylistView.qml
        qml/Home/RecentListeningSection.qml

        qml/MyWave/MyWaveSection.qml

        qml/Search/SearchResultsSection.qml

        qml/Context/ContextPanelHome.qml
        qml/Context/ContextPanelMyWave.qml
        qml/Context/ContextPanelArtist.qml
        qml/Context/ContextPanelAlbum.qml
        qml/Context/ContextPanelPlaylist.qml
        qml/Context/ContextPanelLibrary.qml

        qml/Layout/ContextPanel.qml
        qml/Layout/MainLayout.qml
        qml/Layout/Sidebar.qml
        qml/Layout/TopBar.qml
        qml/Layout/NavigationController.qml

        qml/Theme/AppTheme.qml
)


# =============================================================
# Output name
#
# Linux:
#   build/YaMusic/   = QML module directory
#   build/yamusic    = executable
#
# macOS / Windows:
#   YaMusic
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
# macOS icon
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
        TARGETS appYaMusic

        BUNDLE
        DESTINATION .

        RUNTIME
        DESTINATION bin
)


# =============================================================
# Linux desktop entry
# =============================================================

if(UNIX AND NOT APPLE)

    set(
            YAMUSIC_DESKTOP_FILE
            "${CMAKE_CURRENT_BINARY_DIR}/YaMusic.desktop"
    )

    file(
            WRITE
            "${YAMUSIC_DESKTOP_FILE}"

            "[Desktop Entry]\n"
            "Type=Application\n"
            "Name=YaMusic\n"
            "Comment=Yandex Music desktop client\n"
            "Exec=yamusic\n"
            "Terminal=false\n"
            "Categories=AudioVideo;Audio;Music;\n"
    )

    install(
            FILES
            "${YAMUSIC_DESKTOP_FILE}"

            DESTINATION
            share/applications
    )

endif()


# =============================================================
# Qt deployment
#
# Qt libraries, plugins and QML modules are copied into
# the installation tree so the user does not need Qt installed.
# =============================================================

qt_generate_deploy_qml_app_script(
        TARGET appYaMusic

        OUTPUT_SCRIPT
        deploy_script

        NO_UNSUPPORTED_PLATFORM_ERROR
)

install(
        SCRIPT
        "${deploy_script}"
)