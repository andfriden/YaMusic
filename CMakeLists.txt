cmake_minimum_required(VERSION 3.16)

project(
        YaMusic
        VERSION 0.1
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
        Qt6
        REQUIRED
        COMPONENTS
        Quick
        Network
        Multimedia
)

qt_standard_project_setup(
        REQUIRES 6.10
)

# =============================================================
# Imported Figma / design content
# =============================================================

if (
        EXISTS
        "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt"
)
    add_subdirectory(
            ${CMAKE_CURRENT_LIST_DIR}/importedcontent
    )
endif()


# =============================================================
# Application
# =============================================================

qt_add_executable(
        appYaMusic

        # =========================================================
        # Entry point
        # =========================================================

        main.cpp

        # =========================================================
        # Core
        # =========================================================

        src/Core/AppController.h
        src/Core/AppController.cpp

        src/Core/LibraryController.h
        src/Core/LibraryController.cpp

        src/Core/AlbumController.h
        src/Core/AlbumController.cpp

        src/Core/ArtistController.h
        src/Core/ArtistController.cpp

        src/Core/PlaybackController.h
        src/Core/PlaybackController.cpp

        src/Core/PersonalController.h
        src/Core/PersonalController.cpp

        src/Core/PersonalController+MyWave.cpp
        src/Core/PersonalController+Playback.cpp
        src/Core/PersonalController+RecentlyPlayed.cpp
        src/Core/PersonalController+Recommendations.cpp

        src/Core/SearchController.h
        src/Core/SearchController.cpp


        # =========================================================
        # Player
        # =========================================================

        src/Player/PlayerService.h
        src/Player/PlayerService.cpp


        # =========================================================
        # Playback
        # =========================================================

        src/Playback/PlaybackController.h
        src/Playback/PlaybackController.cpp


        # =========================================================
        # Queue
        # =========================================================

        src/Queue/QueueService.h
        src/Queue/QueueService.cpp


        # =========================================================
        # Models
        # =========================================================

        src/Models/Track.h
        src/Models/Track.cpp

        src/Models/Account.h
        src/Models/Account.cpp

        src/Models/Playlist.h
        src/Models/Playlist.cpp

        src/Models/PersonalPlaylist.h
        src/Models/PersonalPlaylist.cpp


        # =========================================================
        # Recent Listening
        # =========================================================

        src/Yandex/Personal/RecentListeningModel.h
        src/Yandex/Personal/RecentListeningModel.cpp

        src/Yandex/Personal/RecentListeningService.h
        src/Yandex/Personal/RecentListeningService.cpp


        # =========================================================
        # Yandex
        # =========================================================

        src/Yandex/YandexClient.h
        src/Yandex/YandexClient.cpp

        src/Yandex/AccountParser.h
        src/Yandex/AccountParser.cpp


        # =========================================================
        # Auth
        # =========================================================

        src/Yandex/Auth/YandexAuth.h
        src/Yandex/Auth/YandexAuth.cpp

        src/Yandex/Auth/YandexTokenStorage.h
        src/Yandex/Auth/YandexTokenStorage.cpp


        # =========================================================
        # Account
        # =========================================================

        src/Yandex/Account/AccountService.h
        src/Yandex/Account/AccountService.cpp


        # =========================================================
        # Catalog
        # =========================================================

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

        src/Yandex/Catalog/AlbumService.h
        src/Yandex/Catalog/AlbumService.cpp

        src/Yandex/Catalog/AlbumModel.h
        src/Yandex/Catalog/AlbumModel.cpp

        src/Yandex/Catalog/ArtistService.h
        src/Yandex/Catalog/ArtistService.cpp

        src/Yandex/Catalog/ArtistModel.h
        src/Yandex/Catalog/ArtistModel.cpp

        src/Yandex/Catalog/ArtistAlbumsModel.h
        src/Yandex/Catalog/ArtistAlbumsModel.cpp

        src/Yandex/Catalog/SimilarArtistsModel.h
        src/Yandex/Catalog/SimilarArtistsModel.cpp


        # =========================================================
        # Personal
        # =========================================================

        src/Yandex/Personal/YandexPersonal.h
        src/Yandex/Personal/YandexPersonal.cpp

        src/Yandex/Personal/MyWaveModel.h
        src/Yandex/Personal/MyWaveModel.cpp

        src/Yandex/Personal/PersonalLanding.h
        src/Yandex/Personal/PersonalLanding.cpp

        src/Yandex/Personal/PlaylistService.h
        src/Yandex/Personal/PlaylistService.cpp

        src/Yandex/Personal/PersonalPlaylistsModel.h
        src/Yandex/Personal/PersonalPlaylistsModel.cpp

        src/Yandex/Personal/PlaylistModel.h
        src/Yandex/Personal/PlaylistModel.cpp
        src/Core/AppController+Catalog.cpp
        src/Core/AppController+Personal.cpp
        src/Core/AppController+Selection.cpp
        src/Core/AppController+Playback.cpp
        src/Core/AppController+Search.cpp
        src/Yandex/Personal/LibraryPlaylistsModel.cpp
        src/Yandex/Personal/LibraryPlaylistsModel.h
        src/Core/AppController+Library.cpp
        src/Yandex/Personal/LikesService.cpp
        src/Yandex/Personal/LikesService.h
        src/Yandex/Personal/LikedTracksModel.cpp
        src/Yandex/Personal/LikedTracksModel.h
        src/Queue/QueueModel.cpp
        src/Queue/QueueModel.h
        src/Core/PlayerAccentService.cpp
        src/Core/PlayerAccentService.h
        src/Core/ChartController.cpp
        src/Core/ChartController.h
        src/Yandex/Catalog/ChartService.cpp
        src/Yandex/Catalog/ChartService.h
        src/Yandex/Catalog/ChartModel.cpp
        src/Yandex/Catalog/ChartModel.h
        src/Yandex/Personal/PersonalChartModel.cpp
        src/Yandex/Personal/PersonalChartModel.h
        src/Yandex/Personal/NewPlaylistsService.cpp
        src/Yandex/Personal/NewPlaylistsService.h
        src/Models/Genre.h
        src/Yandex/Catalog/GenreService.cpp
        src/Yandex/Catalog/GenreService.h
        src/Yandex/Catalog/GenreModel.cpp
        src/Yandex/Catalog/GenreModel.h
        src/Core/GenreController.cpp
        src/Core/GenreController.h
)


# =============================================================
# Theme singleton
#
# IMPORTANT:
# QT_QML_SINGLETON_TYPE must be set BEFORE qt_add_qml_module().
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

        QML_FILES

        # =========================================================
        # Root
        # =========================================================

        Main.qml


        # =========================================================
        # Components
        # =========================================================

        qml/Components/SearchBar.qml
        qml/Components/StatusBar.qml
        qml/Components/NowPlayingBar.qml
        qml/Components/EntityLink.qml
        qml/Components/ExpandedNowPlaying.qml
        qml/Components/ChartTrackRow.qml
        qml/Components/PersonalChartSection.qml


        # =========================================================
        # Pages
        # =========================================================

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


        # =========================================================
        # Home
        # =========================================================

        qml/Home/PersonalPlaylistsSection.qml
        qml/Home/PlaylistView.qml
        qml/Home/RecentListeningSection.qml
        qml/Home/AlbumView.qml
        qml/Home/ArtistView.qml
        qml/Home/ArtistAlbumsSection.qml


        # =========================================================
        # My Wave
        # =========================================================

        qml/MyWave/MyWaveSection.qml


        # =========================================================
        # Search
        # =========================================================

        qml/Search/SearchResultsSection.qml


        # =========================================================
        # Context
        # =========================================================

        qml/Context/ContextPanelHome.qml
        qml/Context/ContextPanelMyWave.qml
        qml/Context/ContextPanelArtist.qml
        qml/Context/ContextPanelAlbum.qml
        qml/Context/ContextPanelPlaylist.qml
        qml/Context/ContextPanelLibrary.qml


        # =========================================================
        # Layout
        # =========================================================

        qml/Layout/ContextPanel.qml
        qml/Layout/MainLayout.qml
        qml/Layout/Sidebar.qml


        # =========================================================
        # Theme
        # =========================================================

        qml/Theme/AppTheme.qml
)


# =============================================================
# Target properties
# =============================================================

set_target_properties(
        appYaMusic
        PROPERTIES

        MACOSX_BUNDLE
        TRUE

        MACOSX_BUNDLE_BUNDLE_VERSION
        ${PROJECT_VERSION}

        MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}

        WIN32_EXECUTABLE
        TRUE
)


# =============================================================
# Qt libraries
# =============================================================

target_link_libraries(
        appYaMusic
        PRIVATE

        Qt6::Quick
        Qt6::Network
        Qt6::Multimedia
)


# =============================================================
# macOS Security framework
# =============================================================

if (APPLE)

    find_library(
            SECURITY_FRAMEWORK
            Security
    )

    if (SECURITY_FRAMEWORK)

        target_link_libraries(
                appYaMusic
                PRIVATE

                ${SECURITY_FRAMEWORK}
        )

    endif()

endif()


# =============================================================
# Install
# =============================================================

include(GNUInstallDirs)

install(
        TARGETS
        appYaMusic

        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


# =============================================================
# QML deployment
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
        ${deploy_script}
)