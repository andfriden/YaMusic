import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller
    property int rowIndex: -1
    property string trackId: ""
    property string title: ""
    property string artist: ""
    property string artistId: ""
    property string album: ""
    property string albumId: ""
    property string coverUri: ""
    property int durationMs: 0

    // Подсветка текущего трека: по trackId (обычно) или по паре
    // title+artist (моя волна — в ролях модели нет trackId)
    property string currentMatch: "id"

    readonly property bool highlighted: root.currentMatch === "titleArtist"
        ? root.hasController
            && String(root.controller.currentTrackTitle || "") === root.title
            && String(root.controller.currentTrackArtist || "") === root.artist
        : root.hasController
            && root.trackId.length > 0
            && String(root.controller.currentTrackId || "") === root.trackId

    property bool liked: false
    property bool showLike: false
    property bool showAdd: false
    property bool showRemove: false
    property bool showAlbum: true
    property bool alwaysBorder: false
    property int scrollbarOffset: 0

    // Геометрия (в оригиналах отличалась от места к месту)
    property int rowHeight: 68
    property int coverSize: 52
    property int cornerRadius: 8

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    width: parent ? Math.max(0, parent.width - root.scrollbarOffset) : 0
    height: root.rowHeight

    signal activated()
    signal addRequested(string trackId, string albumId)
    signal removeRequested(int index)

    Rectangle {
        anchors.fill: parent
        radius: root.cornerRadius

        color: root.highlighted
            ? AppTheme.panelActive
            : rowMouseArea.containsMouse
                ? AppTheme.panelHover
                : AppTheme.panelSecondary

        border.width: root.highlighted || root.alwaysBorder ? 1 : 0
        border.color: root.highlighted ? AppTheme.accent : AppTheme.borderSubtle

        Rectangle {
            id: coverContainer

            width: root.coverSize
            height: root.coverSize

            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.verticalCenter: parent.verticalCenter

            radius: 6
            color: AppTheme.artworkPlaceholder
            clip: true

            Image {
                id: cover

                anchors.fill: parent

                source: root.coverUri.length > 0
                    ? "image://yandex/" + root.coverUri
                    : ""

                sourceSize: Qt.size(root.coverSize * 2, root.coverSize * 2)

                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true
                smooth: true

                visible: status === Image.Ready
            }

            Label {
                anchors.centerIn: parent

                text: "♪"
                color: AppTheme.textSecondary
                font.pixelSize: root.coverSize * 0.38

                visible: cover.status !== Image.Ready
            }
        }

        Column {
            id: trackInfo

            anchors.left: coverContainer.right
            anchors.leftMargin: 12
            anchors.right: durationLabel.left
            anchors.rightMargin: 12
            anchors.verticalCenter: parent.verticalCenter

            spacing: 2

            Label {
                width: parent.width
                height: Math.max(18, implicitHeight)

                text: root.title.length > 0 ? root.title : qsTr("Без названия")

                color: AppTheme.textPrimary
                font.pixelSize: 14
                font.bold: true

                elide: Text.ElideRight
                maximumLineCount: 1
            }

            Item {
                id: artistArea

                width: Math.min(artistLabel.implicitWidth, trackInfo.width)
                height: artistLabel.height

                Label {
                    id: artistLabel

                    width: parent.width

                    text: root.artist.length > 0
                        ? root.artist
                        : qsTr("Неизвестный исполнитель")

                    color: artistMouseArea.containsMouse
                        ? AppTheme.accent
                        : AppTheme.textSecondary

                    font.pixelSize: 12
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                MouseArea {
                    id: artistMouseArea

                    anchors.fill: parent

                    hoverEnabled: true
                    enabled: root.artistId.length > 0 && root.hasController
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                    z: 10

                    onClicked: {
                        if (root.hasController && root.artistId.length > 0) {
                            root.controller.loadArtist(root.artistId)
                        }
                    }
                }
            }

            Item {
                id: albumArea

                width: Math.min(albumLabel.implicitWidth, trackInfo.width)
                height: albumLabel.height

                visible: root.showAlbum && root.album.length > 0

                Label {
                    id: albumLabel

                    width: parent.width

                    text: root.album
                    color: albumMouseArea.containsMouse
                        ? AppTheme.accent
                        : AppTheme.textMuted

                    font.pixelSize: 10
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                MouseArea {
                    id: albumMouseArea

                    anchors.fill: parent

                    hoverEnabled: true
                    enabled: root.albumId.length > 0 && root.hasController
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                    z: 10

                    onClicked: {
                        if (root.hasController && root.albumId.length > 0) {
                            root.controller.loadAlbum(root.albumId)
                        }
                    }
                }
            }
        }

        Label {
            id: durationLabel

            width: 44

            anchors.right: rightButtons.left
            anchors.rightMargin: 6
            anchors.verticalCenter: parent.verticalCenter

            text: AppTheme.formatDuration(root.durationMs)

            color: AppTheme.textSecondary
            font.pixelSize: 11
            horizontalAlignment: Text.AlignRight
        }

        Row {
            id: rightButtons

            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.verticalCenter: parent.verticalCenter

            spacing: 2

            Item {
                width: 30
                height: 30

                visible: root.showRemove

                Label {
                    anchors.centerIn: parent

                    text: "×"
                    color: removeMouse.containsMouse ? AppTheme.accent : AppTheme.textMuted
                    font.pixelSize: 18
                    font.bold: true
                }

                MouseArea {
                    id: removeMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.removeRequested(root.rowIndex)
                }
            }

            Item {
                width: 30
                height: 30

                visible: root.showAdd

                Label {
                    anchors.centerIn: parent

                    text: "+"
                    color: addMouse.containsMouse ? AppTheme.accent : AppTheme.textMuted
                    font.pixelSize: 18
                    font.bold: true
                }

                MouseArea {
                    id: addMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.addRequested(root.trackId, root.albumId)
                }
            }

            Item {
                width: 34
                height: 34

                visible: root.showLike

                Label {
                    anchors.centerIn: parent

                    text: root.liked ? "♥" : "♡"
                    color: root.liked ? AppTheme.accent : AppTheme.textSecondary
                    font.pixelSize: 21
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        if (root.hasController && root.trackId.length > 0) {
                            root.controller.toggleLike(root.trackId, root.liked)
                        }
                    }
                }
            }
        }

        MouseArea {
            id: rowMouseArea

            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            z: 0

            onClicked: function(mouse) {
                if (mouse.button === Qt.LeftButton) {
                    root.activated()
                }
            }

            onPressed: function(mouse) {
                if (mouse.button === Qt.RightButton && root.hasController) {
                    root.controller.copyTrack(root.title, root.artist)
                }
            }
        }
    }
}