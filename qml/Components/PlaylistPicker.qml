import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Popup {
    id: root

    property var controller
    property string trackId: ""
    property string albumId: ""

    anchors.centerIn: Overlay.overlay

    width: Math.min(320, Overlay.overlay ? Overlay.overlay.width * 0.8 : 320)
    height: Math.min(400, Overlay.overlay ? Overlay.overlay.height * 0.6 : 400)

    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        color: AppTheme.panel
        radius: 12
        border.width: 1
        border.color: AppTheme.border
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: "Добавить в плейлист"
            color: AppTheme.textPrimary
            font.pixelSize: 18
            font.bold: true
        }

        ListView {
            width: parent.width
            height: parent.height - 40
            clip: true
            spacing: 6

            model: root.controller !== null
                ? root.controller.libraryPlaylistsModel
                : null

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

            delegate: Rectangle {
                required property int kind
                required property int revision
                required property string title
                required property string coverUri
                required property int trackCount

                width: parent ? parent.width : 0
                height: 56
                radius: 8
                color: mouseArea.containsMouse
                    ? AppTheme.panelHover
                    : "transparent"

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (root.controller && root.trackId.length > 0) {
                            root.controller.addTrackToPlaylist(
                                kind,
                                root.trackId,
                                root.albumId,
                                revision
                            )
                            root.close()
                        }
                    }
                }

                Image {
                    id: plCover
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    width: 40
                    height: 40
                    source: coverUri.length > 0
                        ? "image://yandex/" + coverUri
                        : ""
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    cache: true

                    Rectangle {
                        anchors.fill: parent
                        radius: 6
                        color: AppTheme.surface
                        visible: plCover.status !== Image.Ready
                        Label {
                            anchors.centerIn: parent
                            text: "♪"
                            color: AppTheme.textSecondary
                            font.pixelSize: 16
                        }
                    }
                }

                Column {
                    anchors.left: plCover.right
                    anchors.leftMargin: 12
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2

                    Label {
                        width: parent.width
                        text: title
                        color: AppTheme.textPrimary
                        font.pixelSize: 13
                        font.bold: true
                        elide: Text.ElideRight
                    }

                    Label {
                        width: parent.width
                        text: trackCount > 0 ? trackCount + " треков" : ""
                        color: AppTheme.textMuted
                        font.pixelSize: 11
                        elide: Text.ElideRight
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                text: "Нет плейлистов"
                color: AppTheme.textSecondary
                font.pixelSize: 13
                visible: parent.count === 0
            }
        }
    }
}