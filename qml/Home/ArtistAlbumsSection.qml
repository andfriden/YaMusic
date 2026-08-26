import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    readonly property var artistController:
            controller !== null
        ? controller.artistController
        : null

    implicitHeight:
        albumsColumn.implicitHeight

    Column {
        id: albumsColumn

        width: parent.width

        spacing: 12

        Label {
            text:
                qsTr("Популярные альбомы")

            color: AppTheme.textPrimary

            font.pixelSize: 22
            font.bold: true
        }

        ListView {
            id: albumsView

            width: parent.width
            height: 180

            model:
                    root.artistController !== null
                ? root.artistController.albumsModel
                : null

            orientation:
                ListView.Horizontal

            spacing: 12

            clip: true

            boundsBehavior:
                Flickable.StopAtBounds

            delegate: Rectangle {
                required property int index
                required property string albumId
                required property string title
                required property string coverUri
                required property int year

                width: 150
                height: 180

                radius: 9

                color:
                    mouseArea.containsMouse
                        ? "#e2e2e2"
                        : "#eeeeee"

                border.width: 1

                border.color:
                    mouseArea.containsMouse
                        ? "#c9c9c9"
                        : AppTheme.panelHover

                Column {
                    anchors.fill: parent

                    spacing: 6

                    Image {
                        id: albumCover

                        width: 150
                        height: 150

                        source:
                                coverUri.length > 0
                            ? "image://yandex/" +
                            coverUri
                            : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous: true
                        cache: true

                        Rectangle {
                            anchors.fill: parent

                            radius: 9

                            color: AppTheme.surface

                            visible:
                                albumCover.status !== Image.Ready
                        }
                    }

                    Label {
                        width: parent.width

                        text: title

                        color: AppTheme.textPrimary

                        font.pixelSize: 12
                        font.bold: true

                        elide: Text.ElideRight
                    }

                    Label {
                        visible: year > 0

                        text: year

                        color: AppTheme.textMuted

                        font.pixelSize: 10
                    }
                }

                MouseArea {
                    id: mouseArea

                    anchors.fill: parent

                    hoverEnabled: true

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        if (
                            root.controller === null
                        ) {
                            return
                        }

                        root.controller.loadAlbum(
                            albumId
                        )
                    }
                }
            }
        }

        Label {
            width: parent.width

            text:
                qsTr("Альбомов нет")

            color: AppTheme.textMuted

            font.pixelSize: 13

            horizontalAlignment:
                Text.AlignHCenter

            visible:
                root.artistController !== null &&
                root.artistController.albumsModel !== null &&
                albumsView.count === 0
        }
    }
}