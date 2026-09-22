import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Контекстная панель плейлиста: похожие плейлисты.
 */
Item {
    id: root

    property var controller

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    Rectangle {
        anchors.fill: parent

        color: AppTheme.backgroundSecondary
        border.width: 1
        border.color: AppTheme.borderWeak

        Column {
            anchors.fill: parent
            anchors.margins: 14

            spacing: 14

            Label {
                width: parent.width

                text: qsTr("Похожие плейлисты")
                color: AppTheme.textPrimary
                font.pixelSize: 17
                font.bold: true
            }

            ListView {
                id: playlistList

                width: parent.width
                height: parent.height - y

                spacing: 6
                clip: true

                boundsBehavior: Flickable.StopAtBounds

                model: root.hasController ? root.controller.similarPlaylists : []

                delegate: Item {
                    width: playlistList.width
                    height: 60

                    Rectangle {
                        anchors.fill: parent

                        radius: 8
                        color: mouseArea.containsMouse
                            ? AppTheme.panelHover
                            : AppTheme.backgroundSecondary

                        Behavior on color {
                            ColorAnimation { duration: 120 }
                        }

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 2
                            anchors.rightMargin: 2

                            spacing: 10

                            Image {
                                width: 56
                                height: 56

                                anchors.verticalCenter: parent.verticalCenter

                                source: modelData.coverUri && modelData.coverUri !== ""
                                    ? "image://yandex/" + modelData.coverUri
                                    : ""

                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                                cache: true

                                Rectangle {
                                    anchors.fill: parent
                                    radius: 6

                                    color: AppTheme.panelSubtle
                                    visible: parent.status !== Image.Ready
                                }
                            }

                            Label {
                                width: parent.width - 56 - 10

                                anchors.verticalCenter: parent.verticalCenter

                                text: modelData.title || qsTr("Плейлист")
                                color: AppTheme.textPrimary
                                font.pixelSize: 13
                                font.bold: true

                                elide: Text.ElideRight
                                maximumLineCount: 2
                            }
                        }

                        MouseArea {
                            id: mouseArea

                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (!root.hasController ||
                                    modelData.uid === undefined ||
                                    modelData.kind === undefined) {
                                    return
                                }

                                root.controller.selectPersonalPlaylist(
                                    modelData.uid, modelData.kind)
                            }
                        }
                    }
                }
            }
        }
    }
}