import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    property string contextType: "home"

    readonly property var artistController:
            controller !== null
        ? controller.artistController
        : null

    readonly property string contextTitle:
        titleForContext(contextType)

    Rectangle {
        anchors.fill: parent

        color: "#ededed"

        border.width: 1
        border.color: "#d7d7d7"

        Column {
            anchors.fill: parent

            anchors.margins: 14

            spacing: 12

            Label {
                width: parent.width

                text:
                    root.contextTitle

                color: "#202020"

                font.pixelSize: 17
                font.bold: true

                elide: Text.ElideRight
            }

            Label {
                width: parent.width

                text:
                    descriptionForContext(
                        root.contextType
                    )

                color: "#777777"

                font.pixelSize: 11

                wrapMode: Text.WordWrap

                visible:
                    text.length > 0
            }

            ListView {
                id: artistsView

                width: parent.width

                height:
                    parent.height - 70

                clip: true

                spacing: 8

                model:
                        root.contextType === "artist" &&
                    root.artistController !== null
                    ? root.artistController.similarArtistsModel
                    : null

                boundsBehavior:
                    Flickable.StopAtBounds

                ScrollBar.vertical: ScrollBar {
                    policy:
                            artistsView.contentHeight >
                        artistsView.height
                        ? ScrollBar.AsNeeded
                        : ScrollBar.AlwaysOff
                }

                delegate: Rectangle {
                    required property int index
                    required property string artistId
                    required property string name
                    required property string coverUri

                    width:
                        artistsView.width

                    height: 58

                    radius: 8

                    color:
                        mouseArea.containsMouse
                            ? "#dedede"
                            : "#f4f4f4"

                    border.width: 1

                    border.color:
                        mouseArea.containsMouse
                            ? "#c6c6c6"
                            : "#e1e1e1"

                    Image {
                        id: artistImage

                        anchors.left: parent.left
                        anchors.leftMargin: 7

                        anchors.verticalCenter:
                            parent.verticalCenter

                        width: 44
                        height: 44

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

                            radius: 22

                            color: "#d3d3d3"

                            visible:
                                artistImage.status !==
                                Image.Ready
                        }
                    }

                    Column {
                        anchors.left:
                            artistImage.right

                        anchors.leftMargin: 11

                        anchors.right:
                            parent.right

                        anchors.rightMargin: 10

                        anchors.verticalCenter:
                            parent.verticalCenter

                        spacing: 2

                        Label {
                            width: parent.width

                            text: name

                            color: "#202020"

                            font.pixelSize: 13
                            font.bold: true

                            elide: Text.ElideRight
                        }

                        Label {
                            width: parent.width

                            text: qsTr("Исполнитель")

                            color: "#777777"

                            font.pixelSize: 11

                            elide: Text.ElideRight
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
                                root.artistController === null
                            ) {
                                return
                            }

                            console.log(
                                "ContextPanel: selectSimilarArtist index =",
                                index
                            )

                            root.artistController.selectSimilarArtist(
                                index
                            )
                        }
                    }
                }

                Label {
                    anchors.centerIn: parent

                    width:
                        parent.width - 20

                    text:
                            root.artistController !== null &&
                        root.artistController.loading
                        ? qsTr("Загрузка...")
                        : qsTr(
                            "Нет похожих исполнителей"
                        )

                    color: "#999999"

                    font.pixelSize: 12

                    horizontalAlignment:
                        Text.AlignHCenter

                    visible:
                        root.contextType === "artist" &&
                        artistsView.count === 0
                }
            }
        }
    }

    function titleForContext(type) {
        switch (type) {
            case "artist":
                return qsTr("Похожие исполнители")

            case "album":
                return qsTr("Другие альбомы")

            case "playlist":
                return qsTr("Похожие плейлисты")

            case "library":
                return qsTr("Вам может понравиться")

            default:
                return qsTr("Вам может понравиться")
        }
    }

    function descriptionForContext(type) {
        switch (type) {
            case "artist":
                return qsTr(
                    "Исполнители с похожим звучанием"
                )

            case "album":
                return qsTr(
                    "Другие релизы этого исполнителя"
                )

            case "playlist":
                return qsTr(
                    "Подборки в похожем стиле"
                )

            case "library":
                return qsTr(
                    "Музыка, которая может вам понравиться"
                )

            default:
                return qsTr(
                    "Музыка, которая может вам понравиться"
                )
        }
    }
}