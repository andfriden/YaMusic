import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    // =============================================================
    // 1. ОСНОВНЫЕ СВОЙСТВА
    // =============================================================

    property var controller
    property string currentSection: "home"
    property string contextType: contextTypeForSection(currentSection)

    // Панель видна только для альбомов, исполнителей и плейлистов
    readonly property bool showContextPanel:
        root.currentSection === "albums" ||
        root.currentSection === "artists" ||
        root.currentSection === "playlists"

    signal sectionSelected(string section)


    // =============================================================
    // 2. ФОН
    // =============================================================
    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }


    // =============================================================
    // 3. РАСКЛАДКА
    // =============================================================
    Row {
        anchors.fill: parent


        // ---------- Левая панель ----------
        Sidebar {
            id: sidebar
            width: 205
            height: parent.height
            currentSection: root.currentSection
            onSectionSelected: function(section) {
                root.currentSection = section
                root.sectionSelected(section)
            }
        }


        // Разделитель
        Rectangle {
            width: 1
            height: parent.height
            color: "#22ffffff"
        }


        // ---------- Центральная область ----------
        Item {
            id: mainArea
            width: parent.width - sidebar.width - 1 - (
                root.showContextPanel ? contextPanel.width : 0
            )
            height: parent.height
            clip: true


            // =========================================================
            // ScrollView – с добавленным отступом внизу (126px)
            // =========================================================
            ScrollView {
                id: pageScroll

                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 10
                anchors.bottomMargin: 10

                clip: true

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }


                // Контейнер содержимого – содержит Loader и пустой элемент для отступа
                Item {
                    id: contentHost

                    width: pageScroll.availableWidth
                    implicitWidth: width

                    // Высота: если загружен элемент, берём его implicitHeight + 126,
                    // иначе просто 126 (чтобы сохранить минимальный отступ)
                    height: pageLoader.item !== null && pageLoader.item !== undefined
                        ? pageLoader.item.implicitHeight + 126
                        : 126
                    implicitHeight: height


                    // Загрузчик страниц
                    Loader {
                        id: pageLoader

                        width: parent.width
                        height: item !== null && item !== undefined
                            ? item.implicitHeight
                            : 0

                        onLoaded: {
                            if (item !== null && item !== undefined) {
                                item.width = pageLoader.width
                            }
                        }
                    }


                    // Пустой элемент, создающий дополнительное пространство внизу (126px)
                    Item {
                        width: parent.width
                        height: 126
                    }
                }
            }
        }


        // ---------- Правая контекстная панель ----------
        Item {
            id: contextArea
            width: root.showContextPanel ? contextPanel.width : 0
            height: parent.height
            visible: root.showContextPanel


            Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                color: "#22ffffff"
            }


            ContextPanel {
                id: contextPanel
                width: 260
                height: parent.height
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 14
                visible: root.showContextPanel
                contextType: root.contextType
                controller: root.controller
            }
        }
    }


    // =============================================================
    // 4. ОБРАБОТКА СОБЫТИЙ HOME -> CHART
    // =============================================================
    Connections {
        target: root.currentSection === "home" ? pageLoader.item : null
        function onChartRequested() {
            root.currentSection = "chart"
        }
    }


    // =============================================================
    // 5. ЗАГРУЗКА СТРАНИЦ
    // =============================================================
    function loadCurrentPage() {
        pageLoader.setSource(
            pageSourceForSection(root.currentSection),
            { controller: root.controller }
        )
    }

    onCurrentSectionChanged: {
        root.contextType = contextTypeForSection(root.currentSection)
        loadCurrentPage()
    }

    onControllerChanged: {
        if (root.controller !== null && root.controller !== undefined) {
            loadCurrentPage()
        }
    }

    Component.onCompleted: {
        loadCurrentPage()
    }


    // =============================================================
    // 6. МАРШРУТИЗАЦИЯ СТРАНИЦ
    // =============================================================
    function pageSourceForSection(section) {
        switch (section) {
            case "home":      return "../Pages/HomePage.qml"
            case "search":    return "../Pages/SearchPage.qml"
            case "wave":      return "../Pages/MyWavePage.qml"
            case "library":   return "../Pages/LibraryPage.qml"
            case "albums":    return "../Pages/AlbumPage.qml"
            case "artists":   return "../Pages/ArtistPage.qml"
            case "playlists": return "../Pages/PlaylistPage.qml"
            case "recent":    return "../Pages/RecentPage.qml"
            case "chart":     return "../Pages/ChartPage.qml"
            default:          return "../Pages/HomePage.qml"
        }
    }


    // =============================================================
    // 7. ТИП КОНТЕКСТА
    // =============================================================
    function contextTypeForSection(section) {
        switch (section) {
            case "artists":   return "artist"
            case "albums":    return "album"
            case "playlists": return "playlist"
            default:          return "home"
        }
    }
}