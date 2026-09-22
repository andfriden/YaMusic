import QtQuick
import YaMusic 1.0

/*
 * Навигация приложения: единственный источник правды для текущего
 * раздела/страницы. Управляет стеком истории для детальных страниц
 * (исполнитель, альбом, плейлист, жанр) и запрашивает загрузку
 * страницы через сигнал pageLoadRequested.
 */
Item {
    id: root

    // =============================================================
    // Контроллер
    // =============================================================

    property var controller

    // =============================================================
    // Состояние навигации
    // =============================================================

    property string currentSection: "home"
    property string currentPageType: "section"
    property string currentDetailId: ""

    property string currentGenreTitle: ""
    property string currentGenreImage: ""
    property string currentGenreColor: ""

    property var navigationStack: []

    signal pageLoadRequested(string source, var properties)

    // =============================================================
    // Команды контроллера (навигация из C++/моделей)
    // =============================================================

    Connections {
        target: root.controller
        ignoreUnknownSignals: true

        function onArtistPageRequested(artistId) {
            root.openArtistPage(artistId)
        }

        function onAlbumPageRequested(albumId) {
            root.openAlbumPage(albumId)
        }

        function onPlaylistPageRequested() {
            root.openPlaylistPage()
        }

        function onSearchPageRequested(query) {
            root.selectSection("search")
        }
    }

    // =============================================================
    // Выбор раздела
    // =============================================================

    function selectSection(section) {
        root.navigationStack = []

        root.currentSection = String(section || "home")
        root.currentPageType = "section"
        root.currentDetailId = ""
        root.currentGenreTitle = ""
        root.currentGenreImage = ""
        root.currentGenreColor = ""

        root.requestPageLoad()
    }

    // =============================================================
    // Открытие детальной страницы
    // =============================================================

    // Общая часть для всех детальных страниц: запоминаем текущее
    // состояние в стеке и переключаемся на новый тип страницы
    function pushPage(pageType, id, extra) {
        const normalizedId = String(id || "").trim()

        if (normalizedId.length === 0) {
            return
        }

        root.navigationStack = root.navigationStack.concat([{
            type: root.currentPageType,
            section: root.currentSection,
            id: root.currentDetailId
        }])

        root.currentPageType = pageType
        root.currentDetailId = normalizedId

        if (extra) {
            root.currentGenreTitle = extra.title || ""
            root.currentGenreImage = extra.image || ""
            root.currentGenreColor = extra.color || ""
        }

        root.requestPageLoad()
    }

    function openArtistPage(artistId) {
        root.pushPage("artist", artistId)
    }

    function openAlbumPage(albumId) {
        root.pushPage("album", albumId)
    }

    function openPlaylistPage() {
        // У плейлиста нет id — тип страницы "playlist" (kind достаёт
        // сам контроллер)
        root.navigationStack = root.navigationStack.concat([{
            type: root.currentPageType,
            section: root.currentSection,
            id: root.currentDetailId
        }])

        root.currentPageType = "playlist"
        root.currentDetailId = ""

        root.requestPageLoad()
    }

    function openGenrePage(genreId, title, image, color) {
        root.pushPage("genre", genreId, {
            title: title || "",
            image: image || "",
            color: color || ""
        })
    }

    // =============================================================
    // Назад
    // =============================================================

    function goBack() {
        if (root.navigationStack.length === 0) {
            return
        }

        const stack = root.navigationStack.slice()
        const previous = stack.pop()

        root.navigationStack = stack

        root.currentPageType = previous.type || "section"
        root.currentSection = previous.section || "home"
        root.currentDetailId = previous.id || ""

        root.requestPageLoad()
    }

    // =============================================================
    // Загрузка страницы
    // =============================================================

    function requestPageLoad() {
        let properties = { controller: root.controller }

        if (root.currentPageType === "genre") {
            properties = {
                controller: root.controller,
                genreId: root.currentDetailId,
                genreTitle: root.currentGenreTitle,
                genreImage: root.currentGenreImage,
                genreColor: root.currentGenreColor
            }
        }

        root.pageLoadRequested(root.pageSourceForCurrentPage(), properties)
    }

    function pageSourceForCurrentPage() {
        switch (root.currentPageType) {
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
                return root.pageSourceForSection(root.currentSection)
        }
    }

    function pageSourceForSection(section) {
        switch (section) {
            case "home":
                return "../Pages/HomePage.qml"
            case "search":
                return "../Pages/SearchPage.qml"
            case "wave":
                return "../Pages/MyWavePage.qml"
            case "library":
            case "liked":
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
            default:
                return "../Pages/HomePage.qml"
        }
    }

    // =============================================================
    // Тип контекстной панели
    // =============================================================

    function contextTypeForCurrentPage() {
        switch (root.currentPageType) {
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
                return root.contextTypeForSection(root.currentSection)
        }
    }

    function contextTypeForSection(section) {
        switch (section) {
            case "wave":
                return "mywave"
            case "library":
                return "library"
            case "home":
            case "search":
            case "playlists":
            case "recent":
            case "chart":
            case "genres":
            case "sport":
            case "liked":
            default:
                return "home"
        }
    }

    // =============================================================
    // Инициализация
    // =============================================================

    onControllerChanged: {
        if (root.controller !== null && root.controller !== undefined) {
            root.requestPageLoad()
        }
    }

    Component.onCompleted: {
        root.currentSection = "home"
        root.currentPageType = "section"
        root.currentDetailId = ""
        root.currentGenreTitle = ""
        root.currentGenreImage = ""
        root.currentGenreColor = ""
        root.navigationStack = []

        if (root.controller !== null && root.controller !== undefined) {
            root.requestPageLoad()
        }
    }
}