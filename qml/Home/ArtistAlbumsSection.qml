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

        width:
            parent.width

        spacing:
            12

        Label {
            text:
                qsTr("Популярные альбомы")

            color:
                "#202020"

            font.pixelSize:
                22

            font.bold:
                true
        }

        ListView {
            id: albumsView

            width:
                parent.width

            height:
                180

            clip:
                true

            orientation:
                ListView.Horizontal

            spacing:
                12

            boundsBehavior:
                Flickable.StopAtBounds

            model:
                    root.artistController !== null
                ? root.artistController.albumsModel
                : null

            delegate: Item {
                required property string albumId
                required property string title
                required property string coverUri
                required property int year

                width:
                    150

                height:
                    180

                Column {
                    anchors.fill:
                        parent

                    spacing:
                        6

                    Image {
                        id: albumCover

                        width:
                            150

                        height:
                            150

                        source:
                                coverUri.length > 0
                            ? "image://yandex/" +
                            coverUri
                            : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous:
                            true

                        cache:
                            true

                        Rectangle {
                            anchors.fill:
                                parent

                            radius:
                                9

                            color:
                                "#d0d0d0"

                            visible:
                                albumCover.status !==
                                Image.Ready
                        }
                    }

                    Label {
                        width:
                            parent.width

                        text:
                            title

                        color:
                            "#202020"

                        font.pixelSize:
                            12

                        font.bold:
                            true

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        visible:
                            year > 0

                        text:
                            year

                        color:
                            "#888888"

                        font.pixelSize:
                            10
                    }
                }
            }
        }
    }
}