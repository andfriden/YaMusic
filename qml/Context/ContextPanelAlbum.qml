import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    readonly property var albumController:
            root.controller !== null &&
        root.controller !== undefined &&
        root.controller.albumController !== undefined &&
        root.controller.albumController !== null
        ? root.controller.albumController
        : null

    readonly property var otherAlbumsModel:
            root.albumController !== null &&
        root.albumController.otherAlbumsModel !== undefined &&
        root.albumController.otherAlbumsModel !== null
        ? root.albumController.otherAlbumsModel
        : null

    Rectangle {
        anchors.fill: parent

        color: AppTheme.backgroundSecondary

        border.width: 1
        border.color: AppTheme.borderSubtle

        Column {
            anchors.fill: parent
            anchors.margins: 14

            spacing: 12

            // =====================================================
            // Header
            // =====================================================

            Label {
                width: parent.width

                text: qsTr("Другие альбомы")

                color: AppTheme.textPrimary

                font.pixelSize: 17
                font.bold: true

                elide: Text.ElideRight
            }

            Label {
                width: parent.width

                text: qsTr(
                    "Другие релизы этого исполнителя"
                )

                color: AppTheme.textMuted

                font.pixelSize: 11

                wrapMode: Text.WordWrap
            }

            // =====================================================
            // Albums
            // =====================================================

            Item {
                width: parent.width

                height:
                    parent.height - 70

                ListView {
                    id: albumsView

                    anchors.fill: parent

                    clip: true

                    spacing: 8

                    model:
                        root.otherAlbumsModel

                    boundsBehavior:
                        Flickable.StopAtBounds

                    ScrollBar.vertical:
                        ScrollBar {
                            policy:
                                    albumsView.contentHeight >
                                albumsView.height
                                ? ScrollBar.AsNeeded
                                : ScrollBar.AlwaysOff
                        }

                    delegate: Rectangle {
                        id: albumRow

                        required property int index
                        required property string albumId
                        required property string title
                        required property string coverUri
                        required property int year

                        width:
                            albumsView.width

                        height: 92

                        radius: 9

                        color:
                            albumMouse.containsMouse
                                ? "#dedede"
                                : "#f4f4f4"

                        border.width: 1

                        border.color:
                            albumMouse.containsMouse
                                ? "#c6c6c6"
                                : "#e1e1e1"

                        // =================================================
                        // Artwork
                        // =================================================

                        Rectangle {
                            id: albumArtwork

                            width: 76
                            height: 76

                            anchors.left:
                                parent.left

                            anchors.leftMargin: 7

                            anchors.verticalCenter:
                                parent.verticalCenter

                            radius: 6

                            color: AppTheme.surface

                            clip: true

                            Image {
                                id: albumImage

                                anchors.fill: parent

                                source:
                                        albumRow.coverUri.length > 0
                                    ? "image://yandex/" +
                                    albumRow.coverUri
                                    : ""

                                sourceSize:
                                    Qt.size(76, 76)

                                fillMode:
                                    Image.PreserveAspectCrop

                                asynchronous: true
                                cache: true

                                visible:
                                    status === Image.Ready
                            }

                            Label {
                                anchors.centerIn: parent

                                text: "♪"

                                color: AppTheme.textMuted

                                font.pixelSize: 22

                                visible:
                                    albumImage.status !==
                                    Image.Ready
                            }
                        }

                        // =================================================
                        // Album information
                        // =================================================

                        Column {
                            anchors.left:
                                albumArtwork.right

                            anchors.leftMargin: 11

                            anchors.right:
                                parent.right

                            anchors.rightMargin: 8

                            anchors.verticalCenter:
                                parent.verticalCenter

                            spacing: 3

                            Label {
                                width: parent.width

                                text:
                                        albumRow.title.length > 0
                                    ? albumRow.title
                                    : qsTr("Без названия")

                                color: AppTheme.textPrimary

                                font.pixelSize: 13
                                font.bold: true

                                elide:
                                    Text.ElideRight
                            }

                            Label {
                                width: parent.width

                                text:
                                        albumRow.year > 0
                                    ? String(albumRow.year)
                                    : ""

                                color: AppTheme.textMuted

                                font.pixelSize: 11

                                visible:
                                    text.length > 0
                            }
                        }

                        // =================================================
                        // Click
                        // =================================================

                        MouseArea {
                            id: albumMouse

                            anchors.fill: parent

                            hoverEnabled: true

                            cursorShape:
                                Qt.PointingHandCursor

                            onClicked: {

                                if (
                                    root.controller === null ||
                                    root.controller === undefined
                                ) {
                                    return
                                }

                                if (
                                    albumRow.albumId.length === 0
                                ) {
                                    return
                                }

                                root.controller.loadAlbum(
                                    albumRow.albumId
                                )
                            }
                        }
                    }
                }

                // =====================================================
                // Empty / loading state
                // =====================================================

                Label {
                    anchors.centerIn: parent

                    width:
                        parent.width - 20

                    text:
                            root.albumController !== null &&
                        root.albumController.loading
                        ? qsTr("Загрузка...")
                        : qsTr("Нет других альбомов")

                    color: AppTheme.textDisabled

                    font.pixelSize: 12

                    horizontalAlignment:
                        Text.AlignHCenter

                    visible:
                        albumsView.count === 0
                }
            }
        }
    }
}