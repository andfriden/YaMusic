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

                // Tabs
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

                            Rectangle {
                                required property string modelData

                                width: tabLabel.implicitWidth + 16
                                height: 32
                                radius: 6
                                color: modelData === tabBar.currentTab ? AppTheme.accent : "transparent"

                                Label {
                                    id: tabLabel
                                    anchors.centerIn: parent
                                    text: parent.modelData
                                    color: modelData === tabBar.currentTab ? "#ffffff" : AppTheme.textSecondary
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

                // 0: Tracks
                ListView {
                    id: tracksView

                    visible: tabBar.currentIndex === 0
                    anchors.fill: parent

                    model: root.controller !== null ? root.controller.searchModel : null
                    clip: true
                    spacing: 6

                    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                    delegate: Rectangle {
                        required property int index
                        required property string trackId
                        required property string title
                        required property string artist
                        required property string artistId
                        required property string album
                        required property string albumId
                        required property string coverUri
                        required property int durationMs

                        width: tracksView.width - (tracksView.ScrollBar.vertical.visible ? 10 : 0)
                        height: 68
                        radius: 8

                        color: root.controller &&
                               root.controller.currentTrackId !== "" &&
                               trackId === root.controller.currentTrackId
                               ? AppTheme.panelActive
                               : (rowMouseArea.containsMouse ? AppTheme.panelActive : AppTheme.panelSecondary)

                        border.width: root.controller &&
                                      root.controller.currentTrackId !== "" &&
                                      trackId === root.controller.currentTrackId ? 1 : 0
                        border.color: AppTheme.accent

                        MouseArea {
                            id: rowMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            z: 0
                            onClicked: root.controller.selectSearchResult(index)
                        }

                        Image {
                            id: cover
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
                                visible: cover.status !== Image.Ready
                                Label {
                                    anchors.centerIn: parent
                                    text: "♪"
                                    color: AppTheme.textSecondary
                                    font.pixelSize: 20
                                }
                            }
                        }

                        Column {
                            anchors.left: cover.right
                            anchors.leftMargin: 12
                            anchors.right: durationLabel.left
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

                            Item {
                                width: artistLabel.width
                                height: artistLabel.height

                                Label {
                                    id: artistLabel
                                    width: Math.min(implicitWidth, parent.parent.width)
                                    height: 18
                                    text: artist
                                    color: artistMouseArea.containsMouse ? AppTheme.accent : AppTheme.textSecondary
                                    font.pixelSize: 12
                                    elide: Text.ElideRight
                                }

                                MouseArea {
                                    id: artistMouseArea
                                    width: artistLabel.width
                                    height: artistLabel.height
                                    hoverEnabled: true
                                    enabled: artistId.length > 0
                                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                    z: 10
                                    onClicked: root.controller.loadArtist(artistId)
                                }
                            }

                            Item {
                                width: albumLabel.width
                                height: albumLabel.height

                                Label {
                                    id: albumLabel
                                    width: Math.min(implicitWidth, parent.parent.width)
                                    height: 16
                                    text: album
                                    color: albumMouseArea.containsMouse ? AppTheme.accent : AppTheme.textMuted
                                    font.pixelSize: 10
                                    elide: Text.ElideRight
                                }

                                MouseArea {
                                    id: albumMouseArea
                                    width: albumLabel.width
                                    height: albumLabel.height
                                    hoverEnabled: true
                                    enabled: albumId.length > 0
                                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                    z: 10
                                    onClicked: root.controller.loadAlbum(albumId)
                                }
                            }
                        }

                        Label {
                            id: durationLabel
                            anchors.right: parent.right
                            anchors.rightMargin: 14
                            anchors.verticalCenter: parent.verticalCenter
                            text: formatDuration(durationMs)
                            color: AppTheme.textSecondary
                            font.pixelSize: 11
                        }
                    }

                    Label {
                        anchors.centerIn: parent
                        text: root.controller && root.controller.searching
                              ? qsTr("Поиск...")
                              : qsTr("Ничего не найдено")
                        color: AppTheme.textSecondary
                        visible: root.controller !== null && root.controller.searchModel.count === 0
                    }
                }

                // 1: Albums
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

                        width: albumsView.width - (albumsView.ScrollBar.vertical.visible ? 10 : 0)
                        height: 68
                        radius: 8
                        color: albumMouseArea.containsMouse ? AppTheme.panelActive : AppTheme.panelSecondary
                        border.width: 0

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

                    Label {
                        anchors.centerIn: parent
                        text: root.controller && root.controller.searching
                              ? qsTr("Поиск...")
                              : qsTr("Альбомы не найдены")
                        color: AppTheme.textSecondary
                        visible: root.controller !== null && root.controller.searchAlbumsModel.count === 0
                    }
                }

                // 2: Artists
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

                        width: artistsView.width - (artistsView.ScrollBar.vertical.visible ? 10 : 0)
                        height: 68
                        radius: 8
                        color: artistMouseArea.containsMouse ? AppTheme.panelActive : AppTheme.panelSecondary
                        border.width: 0

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

                    Label {
                        anchors.centerIn: parent
                        text: root.controller && root.controller.searching
                              ? qsTr("Поиск...")
                              : qsTr("Исполнители не найдены")
                        color: AppTheme.textSecondary
                        visible: root.controller !== null && root.controller.searchArtistsModel.count === 0
                    }
                }

                // 3: Playlists
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

                        width: playlistsView.width - (playlistsView.ScrollBar.vertical.visible ? 10 : 0)
                        height: 68
                        radius: 8
                        color: playlistMouseArea.containsMouse ? AppTheme.panelActive : AppTheme.panelSecondary
                        border.width: 0

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
                                text: trackCount > 0 ? qsTr("%1 треков").arg(trackCount) : ""
                                color: AppTheme.textMuted
                                font.pixelSize: 11
                                elide: Text.ElideRight
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent
                        text: root.controller && root.controller.searching
                              ? qsTr("Поиск...")
                              : qsTr("Плейлисты не найдены")
                        color: AppTheme.textSecondary
                        visible: root.controller !== null && root.controller.searchPlaylistsModel.count === 0
                    }
                }
            }
            }
        }
    }

    // Tab bar state
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

    function formatDuration(milliseconds) {
        if (!milliseconds || milliseconds <= 0)
            return "0:00"
        var totalSeconds = Math.floor(milliseconds / 1000)
        var minutes = Math.floor(totalSeconds / 60)
        var seconds = totalSeconds % 60
        return minutes + ":" + (seconds < 10 ? "0" : "") + seconds
    }
}