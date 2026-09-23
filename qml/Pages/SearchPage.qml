import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    height: parent ? parent.height : 700
    implicitWidth: width
    implicitHeight: height

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    PlaylistPicker {
        id: addToPlaylistPopup
        controller: root.controller
    }

    Column {
        anchors.fill: parent
        spacing: 16

        SearchBar {
            width: parent.width
            controller: root.controller
        }

        Rectangle {
            width: parent.width
            height: parent.height - 80
            color: AppTheme.backgroundPrimary

            Column {
                anchors.fill: parent
                spacing: 8

                Rectangle {
                    width: parent.width
                    height: 36
                    color: AppTheme.backgroundPrimary

                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 24

                        Repeater {
                            model: ["Треки", "Альбомы", "Исполнители", "Плейлисты"]

                            delegate: Rectangle {
                                required property string modelData

                                width: tabLabel.implicitWidth + 16
                                height: 32
                                radius: 6

                                color: modelData === tabBar.currentTab
                                    ? AppTheme.accent
                                    : "transparent"

                                Label {
                                    id: tabLabel

                                    anchors.centerIn: parent

                                    text: parent.modelData
                                    color: modelData === tabBar.currentTab
                                        ? AppTheme.accentOn
                                        : AppTheme.textSecondary
                                    font.pixelSize: 13
                                    font.bold: modelData === tabBar.currentTab
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor

                                    onClicked: tabBar.currentTab = parent.modelData
                                }
                            }
                        }
                    }
                }

                Item {
                    width: parent.width
                    height: parent.height - 44

                    ListView {
                        id: tracksView

                        visible: tabBar.currentIndex === 0
                        anchors.fill: parent

                        model: root.controller !== null ? root.controller.searchModel : null
                        clip: true
                        spacing: 6

                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                        onAtYEndChanged: {
                            if (atYEnd && root.controller) {
                                root.controller.loadMoreSearchResults()
                            }
                        }

                        delegate: TrackRow {
                            width: tracksView.width -
                                (tracksView.ScrollBar.vertical.visible ? 10 : 0)

                            controller: root.controller
                            rowIndex: model.index
                            trackId: model.trackId
                            title: model.title
                            artist: model.artist
                            artistId: model.artistId
                            album: model.album
                            albumId: model.albumId
                            coverUri: model.coverUri
                            durationMs: model.durationMs
                            showAdd: true

                            onActivated: root.controller.selectSearchResult(model.index)
                            onAddRequested: function(trackId, albumId) {
                                addToPlaylistPopup.trackId = trackId
                                addToPlaylistPopup.albumId = albumId
                                addToPlaylistPopup.open()
                            }
                        }

                        Label {
                            anchors.centerIn: parent

                            text: root.controller && root.controller.searching
                                ? qsTr("Поиск...")
                                : qsTr("Ничего не найдено")

                            color: AppTheme.textSecondary

                            visible: root.controller !== null &&
                                root.controller.searchModel.count === 0
                        }
                    }

                    ListView {
                        id: albumsView

                        visible: tabBar.currentIndex === 1
                        anchors.fill: parent

                        model: root.controller !== null ? root.controller.searchAlbumsModel : null
                        clip: true
                        spacing: 6

                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                        delegate: Rectangle {
                            required property int index
                            required property string albumId
                            required property string title
                            required property string coverUri
                            required property int year

                            width: albumsView.width -
                                (albumsView.ScrollBar.vertical.visible ? 10 : 0)
                            height: 68
                            radius: 8

                            color: albumMouseArea.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary

                            MouseArea {
                                id: albumMouseArea

                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                z: 0

                                onClicked: root.controller.selectSearchAlbum(index)
                            }

                            Image {
                                id: albumCover

                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                width: 52
                                height: 52

                                source: coverUri.length > 0 ? "image://yandex/" + coverUri : ""
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                                cache: true

                                Rectangle {
                                    anchors.fill: parent
                                    radius: 6
                                    color: AppTheme.surface

                                    visible: albumCover.status !== Image.Ready

                                    Label {
                                        anchors.centerIn: parent
                                        text: "♪"
                                        color: AppTheme.textSecondary
                                        font.pixelSize: 20
                                    }
                                }
                            }

                            Column {
                                anchors.left: albumCover.right
                                anchors.leftMargin: 12
                                anchors.right: yearLabel.left
                                anchors.rightMargin: 12
                                anchors.verticalCenter: parent.verticalCenter

                                spacing: 2

                                Label {
                                    width: parent.width

                                    text: title.length > 0 ? title : qsTr("Без названия")
                                    color: AppTheme.textPrimary
                                    font.pixelSize: 14
                                    font.bold: true
                                    elide: Text.ElideRight
                                }

                                Label {
                                    width: parent.width

                                    text: year > 0 ? String(year) : ""
                                    color: AppTheme.textMuted
                                    font.pixelSize: 11
                                    elide: Text.ElideRight
                                }
                            }

                            Label {
                                id: yearLabel

                                anchors.right: parent.right
                                anchors.rightMargin: 14
                                anchors.verticalCenter: parent.verticalCenter

                                text: year > 0 ? String(year) : ""
                                color: AppTheme.textSecondary
                                font.pixelSize: 11
                            }
                        }

                        onAtYEndChanged: {
                            if (atYEnd && root.controller) {
                                root.controller.loadMoreSearchResults()
                            }
                        }

                        Label {
                            anchors.centerIn: parent

                            text: root.controller && root.controller.searching
                                ? qsTr("Поиск...")
                                : qsTr("Альбомы не найдены")

                            color: AppTheme.textSecondary

                            visible: root.controller !== null &&
                                root.controller.searchAlbumsModel.count === 0
                        }
                    }

                    ListView {
                        id: artistsView

                        visible: tabBar.currentIndex === 2
                        anchors.fill: parent

                        model: root.controller !== null ? root.controller.searchArtistsModel : null
                        clip: true
                        spacing: 6

                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                        delegate: Rectangle {
                            required property int index
                            required property string artistId
                            required property string name
                            required property string coverUri

                            width: artistsView.width -
                                (artistsView.ScrollBar.vertical.visible ? 10 : 0)
                            height: 68
                            radius: 8

                            color: artistMouseArea.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary

                            MouseArea {
                                id: artistMouseArea

                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                z: 0

                                onClicked: root.controller.selectSearchArtist(index)
                            }

                            Rectangle {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                width: 52
                                height: 52
                                radius: 26
                                color: AppTheme.surface

                                Image {
                                    id: artistCover

                                    anchors.fill: parent

                                    source: coverUri.length > 0 ? "image://yandex/" + coverUri : ""
                                    fillMode: Image.PreserveAspectCrop
                                    asynchronous: true
                                    cache: true
                                }

                                Label {
                                    anchors.centerIn: parent

                                    text: "♪"
                                    color: AppTheme.textSecondary
                                    font.pixelSize: 20

                                    visible: artistCover.status !== Image.Ready
                                }
                            }

                            Label {
                                anchors.left: parent.left
                                anchors.leftMargin: 72
                                anchors.right: parent.right
                                anchors.rightMargin: 14
                                anchors.verticalCenter: parent.verticalCenter

                                text: name.length > 0 ? name : qsTr("Без имени")
                                color: AppTheme.textPrimary
                                font.pixelSize: 14
                                font.bold: true
                                elide: Text.ElideRight
                            }
                        }

                        onAtYEndChanged: {
                            if (atYEnd && root.controller) {
                                root.controller.loadMoreSearchResults()
                            }
                        }

                        Label {
                            anchors.centerIn: parent

                            text: root.controller && root.controller.searching
                                ? qsTr("Поиск...")
                                : qsTr("Исполнители не найдены")

                            color: AppTheme.textSecondary

                            visible: root.controller !== null &&
                                root.controller.searchArtistsModel.count === 0
                        }
                    }

                    ListView {
                        id: playlistsView

                        visible: tabBar.currentIndex === 3
                        anchors.fill: parent

                        model: root.controller !== null ? root.controller.searchPlaylistsModel : null
                        clip: true
                        spacing: 6

                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                        delegate: Rectangle {
                            required property int index
                            required property string uid
                            required property int kind
                            required property string title
                            required property string coverUri
                            required property int trackCount

                            width: playlistsView.width -
                                (playlistsView.ScrollBar.vertical.visible ? 10 : 0)
                            height: 68
                            radius: 8

                            color: playlistMouseArea.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary

                            MouseArea {
                                id: playlistMouseArea

                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                z: 0

                                onClicked: root.controller.selectSearchPlaylist(index)
                            }

                            Image {
                                id: playlistCover

                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                width: 52
                                height: 52

                                source: coverUri.length > 0 ? "image://yandex/" + coverUri : ""
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                                cache: true

                                Rectangle {
                                    anchors.fill: parent
                                    radius: 6
                                    color: AppTheme.surface

                                    visible: playlistCover.status !== Image.Ready

                                    Label {
                                        anchors.centerIn: parent
                                        text: "♪"
                                        color: AppTheme.textSecondary
                                        font.pixelSize: 20
                                    }
                                }
                            }

                            Column {
                                anchors.left: playlistCover.right
                                anchors.leftMargin: 12
                                anchors.right: parent.right
                                anchors.rightMargin: 14
                                anchors.verticalCenter: parent.verticalCenter

                                spacing: 2

                                Label {
                                    width: parent.width

                                    text: title.length > 0 ? title : qsTr("Без названия")
                                    color: AppTheme.textPrimary
                                    font.pixelSize: 14
                                    font.bold: true
                                    elide: Text.ElideRight
                                }

                                Label {
                                    width: parent.width

                                    text: trackCount > 0
                                        ? qsTr("%1 треков").arg(trackCount)
                                        : ""
                                    color: AppTheme.textMuted
                                    font.pixelSize: 11
                                    elide: Text.ElideRight
                                }
                            }
                        }

                        onAtYEndChanged: {
                            if (atYEnd && root.controller) {
                                root.controller.loadMoreSearchResults()
                            }
                        }

                        Label {
                            anchors.centerIn: parent

                            text: root.controller && root.controller.searching
                                ? qsTr("Поиск...")
                                : qsTr("Плейлисты не найдены")

                            color: AppTheme.textSecondary

                            visible: root.controller !== null &&
                                root.controller.searchPlaylistsModel.count === 0
                        }
                    }
                }
            }
        }
    }

    QtObject {
        id: tabBar

        property string currentTab: "Треки"
        property int currentIndex: {
            switch (currentTab) {
                case "Треки": return 0
                case "Альбомы": return 1
                case "Исполнители": return 2
                case "Плейлисты": return 3
                default: return 0
            }
        }
    }
}