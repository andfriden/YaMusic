import QtQuick


Item {
    id: root


    // =============================================================
    // Controller
    // =============================================================

    property var controller


    // =============================================================
    // Navigation state
    // =============================================================

    property string currentSection:
        "home"

    property string currentPageType:
        "section"

    property string currentDetailId:
        ""

    property string currentGenreTitle:
        ""

    property string currentGenreImage:
        ""

    property string currentGenreColor:
        ""

    property var navigationStack:
        []


    // =============================================================
    // Signals
    // =============================================================

    signal pageLoadRequested(
        string source,
        var properties
    )


    // =============================================================
    // Controller navigation
    // =============================================================

    Connections {
        target:
            root.controller

        ignoreUnknownSignals:
            true


        function onArtistPageRequested(
            artistId
        ) {
            root.openArtistPage(
                artistId
            )
        }


        function onAlbumPageRequested(
            albumId
        ) {
            root.openAlbumPage(
                albumId
            )
        }


        function onPlaylistPageRequested() {
            root.openPlaylistPage()
        }


        function onSearchPageRequested(
            query
        ) {
            root.selectSection(
                "search"
            )
        }
    }


    // =============================================================
    // Select section
    // =============================================================

    function selectSection(
        section
    ) {

        root.navigationStack =
            []

        root.currentSection =
            String(
                section || "home"
            )

        root.currentPageType =
            "section"

        root.currentDetailId =
            ""

        root.currentGenreTitle =
            ""

        root.currentGenreImage =
            ""

        root.currentGenreColor =
            ""

        root.requestPageLoad()
    }


    // =============================================================
    // Artist
    // =============================================================

    function openArtistPage(
        artistId
    ) {
        const id =
            String(
                artistId || ""
            ).trim()

        if (
            id.length === 0
        ) {
            return
        }

        root.navigationStack =
            root.navigationStack.concat(
                [
                    {
                        type:
                        root.currentPageType,

                        section:
                        root.currentSection,

                        id:
                        root.currentDetailId
                    }
                ]
            )

        root.currentPageType =
            "artist"

        root.currentDetailId =
            id

        root.requestPageLoad()
    }


    // =============================================================
    // Album
    // =============================================================

    function openAlbumPage(
        albumId
    ) {
        const id =
            String(
                albumId || ""
            ).trim()

        if (
            id.length === 0
        ) {
            return
        }

        root.navigationStack =
            root.navigationStack.concat(
                [
                    {
                        type:
                        root.currentPageType,

                        section:
                        root.currentSection,

                        id:
                        root.currentDetailId
                    }
                ]
            )

        root.currentPageType =
            "album"

        root.currentDetailId =
            id

        root.requestPageLoad()
    }


    // =============================================================
    // Playlist
    // =============================================================

    function openPlaylistPage() {
        root.navigationStack =
            root.navigationStack.concat(
                [
                    {
                        type:
                        root.currentPageType,

                        section:
                        root.currentSection,

                        id:
                        root.currentDetailId
                    }
                ]
            )

        root.currentPageType =
            "playlist"

        root.currentDetailId =
            ""

        root.requestPageLoad()
    }


    // =============================================================
    // Genre
    // =============================================================

    function openGenrePage(
        genreId,
        title,
        image,
        color
    ) {
        const id =
            String(
                genreId || ""
            ).trim()

        if (
            id.length === 0
        ) {
            return
        }

        root.navigationStack =
            root.navigationStack.concat(
                [
                    {
                        type:
                        root.currentPageType,

                        section:
                        root.currentSection,

                        id:
                        root.currentDetailId
                    }
                ]
            )

        root.currentGenreTitle =
            title || ""

        root.currentGenreImage =
            image || ""

        root.currentGenreColor =
            color || ""

        root.currentPageType =
            "genre"

        root.currentDetailId =
            id

        root.requestPageLoad()
    }


    // =============================================================
    // Back
    // =============================================================

    function goBack() {
        if (
            root.navigationStack.length === 0
        ) {
            return
        }

        const stack =
            root.navigationStack.slice()

        const previous =
            stack.pop()

        root.navigationStack =
            stack

        root.currentPageType =
            previous.type || "section"

        root.currentSection =
            previous.section || "home"

        root.currentDetailId =
            previous.id || ""

        root.requestPageLoad()
    }


    // =============================================================
    // Request page load
    // =============================================================

    function requestPageLoad() {
        const source =
            root.pageSourceForCurrentPage()

        let properties = {
            controller:
            root.controller
        }


        if (
            root.currentPageType === "genre"
        ) {
            properties = {
                controller:
                root.controller,

                genreId:
                root.currentDetailId,

                genreTitle:
                root.currentGenreTitle,

                genreImage:
                root.currentGenreImage,

                genreColor:
                root.currentGenreColor
            }
        }


        root.pageLoadRequested(
            source,
            properties
        )
    }


    // =============================================================
    // Page source
    // =============================================================

    function pageSourceForCurrentPage() {
        switch (
            root.currentPageType
            ) {
            case "artist":
                return "../Pages/ArtistPage.qml"

            case "album":
                return "../Pages/AlbumPage.qml"

            case "playlist":
                return "../Pages/PlaylistPage.qml"

            case "genre":
                return "../Pages/GenrePage.qml"

            case "section":
            default:
                return root.pageSourceForSection(
                    root.currentSection
                )
        }
    }


    // =============================================================
    // Section source
    // =============================================================

    function pageSourceForSection(
        section
    ) {
        switch (
            section
            ) {
            case "home":
                return "../Pages/HomePage.qml"

            case "search":
                return "../Pages/SearchPage.qml"

            case "wave":
                return "../Pages/MyWavePage.qml"

            case "library":
                return "../Pages/LibraryPage.qml"

            case "playlists":
                return "../Pages/PlaylistsPage.qml"

            case "recent":
                return "../Pages/RecentPage.qml"

            case "chart":
                return "../Pages/ChartPage.qml"

            case "genres":
                return "../Pages/GenresPage.qml"

            case "sport":
                return "../Pages/SportPage.qml"

            case "liked":
                return "../Pages/LibraryPage.qml"

            case "albums":
                return "../Pages/AlbumPage.qml"

            case "artists":
                return "../Pages/ArtistPage.qml"

            default:
                return "../Pages/HomePage.qml"
        }
    }


    // =============================================================
    // Context type
    // =============================================================

    function contextTypeForCurrentPage() {
        switch (
            root.currentPageType
            ) {
            case "artist":
                return "artist"

            case "album":
                return "album"

            case "playlist":
                return "playlist"

            case "genre":
                return "home"

            case "section":
            default:
                return root.contextTypeForSection(
                    root.currentSection
                )
        }
    }


    function contextTypeForSection(
        section
    ) {
        switch (
            section
            ) {
            case "home":
                return "home"

            case "search":
                return "home"

            case "wave":
                return "mywave"

            case "library":
                return "library"

            case "playlists":
                return "home"

            case "recent":
                return "home"

            case "chart":
                return "home"

            case "genres":
                return "home"

            case "sport":
                return "home"

            case "liked":
                return "home"

            default:
                return "home"
        }
    }


    // =============================================================
    // Controller changed
    // =============================================================

    onControllerChanged: {
        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            root.requestPageLoad()
        }
    }


    // =============================================================
    // Initial state
    // =============================================================

    Component.onCompleted: {
        root.currentSection =
            "home"

        root.currentPageType =
            "section"

        root.currentDetailId =
            ""

        root.currentGenreTitle =
            ""

        root.currentGenreImage =
            ""

        root.currentGenreColor =
            ""

        root.navigationStack =
            []


        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            root.requestPageLoad()
        }
    }
}