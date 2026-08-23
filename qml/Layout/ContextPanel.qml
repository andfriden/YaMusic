import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property string contextType: "home"

    property string contextTitle:
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

                elide:
                    Text.ElideRight
            }

            Label {
                width: parent.width

                text:
                    descriptionForContext(
                        root.contextType)

                color: "#777777"

                font.pixelSize: 11

                wrapMode:
                    Text.WordWrap

                visible:
                    text.length > 0
            }

            ListView {
                id: recommendationList

                width: parent.width

                height:
                    parent.height -
                    70

                spacing: 8

                clip: true

                model:
                    itemsForContext(
                        root.contextType)

                delegate: Rectangle {
                    width:
                        recommendationList.width

                    height: 58

                    radius: 8

                    color:
                        rowMouseArea.containsMouse
                            ? "#dedede"
                            : "#f4f4f4"

                    border.width: 1

                    border.color:
                        rowMouseArea.containsMouse
                            ? "#c6c6c6"
                            : "#e1e1e1"

                    MouseArea {
                        id: rowMouseArea

                        anchors.fill: parent

                        hoverEnabled: true

                        cursorShape:
                            Qt.PointingHandCursor
                    }

                    Rectangle {
                        anchors.left: parent.left

                        anchors.leftMargin: 7

                        anchors.verticalCenter:
                            parent.verticalCenter

                        width: 44

                        height: 44

                        radius:
                                modelData.kind === "artist"
                            ? 22
                            : 6

                        color: "#d3d3d3"

                        Label {
                            anchors.centerIn: parent

                            text:
                                    modelData.kind === "artist"
                                ? "♪"
                                : "▣"

                            color: "#737373"

                            font.pixelSize: 18
                        }
                    }

                    Column {
                        anchors.left:
                            parent.left

                        anchors.leftMargin: 62

                        anchors.right:
                            parent.right

                        anchors.rightMargin: 10

                        anchors.verticalCenter:
                            parent.verticalCenter

                        spacing: 2

                        Label {
                            width: parent.width

                            text:
                                modelData.title

                            color: "#202020"

                            font.pixelSize: 13

                            font.bold: true

                            elide:
                                Text.ElideRight
                        }

                        Label {
                            width: parent.width

                            text:
                                modelData.subtitle

                            color: "#777777"

                            font.pixelSize: 11

                            elide:
                                Text.ElideRight
                        }
                    }
                }

                Label {
                    anchors.centerIn:
                        parent

                    text:
                        "Здесь будут рекомендации"

                    color: "#999999"

                    font.pixelSize: 12

                    visible:
                        recommendationList.count === 0
                }
            }
        }
    }

    function titleForContext(type) {
        switch (type) {
            case "artist":
                return "Похожие исполнители"

            case "album":
                return "Другие альбомы"

            case "playlist":
                return "Похожие плейлисты"

            case "library":
                return "Вам может понравиться"

            default:
                return "Вам может понравиться"
        }
    }

    function descriptionForContext(type) {
        switch (type) {
            case "artist":
                return "Исполнители с похожим звучанием"

            case "album":
                return "Другие релизы этого исполнителя"

            case "playlist":
                return "Подборки в похожем стиле"

            default:
                return "Музыка, которая может вам понравиться"
        }
    }

    function itemsForContext(type) {
        switch (type) {
            case "artist":
                return [
                    {
                        title: "Sapphyre",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    },
                    {
                        title: "Phantaszz",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    },
                    {
                        title: "asuro",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    },
                    {
                        title: "NXVXRMXSS",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    },
                    {
                        title: "Barsbeat",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    }
                ]

            case "album":
                return [
                    {
                        title: "Другой альбом",
                        subtitle: "Альбом",
                        kind: "album"
                    },
                    {
                        title: "Новый релиз",
                        subtitle: "Альбом",
                        kind: "album"
                    }
                ]

            case "playlist":
                return [
                    {
                        title: "Похожая подборка",
                        subtitle: "Плейлист",
                        kind: "playlist"
                    },
                    {
                        title: "Ещё музыка",
                        subtitle: "Плейлист",
                        kind: "playlist"
                    }
                ]

            default:
                return [
                    {
                        title: "Sapphyre",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    },
                    {
                        title: "Phantaszz",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    },
                    {
                        title: "asuro",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    },
                    {
                        title: "NXVXRMXSS",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    },
                    {
                        title: "Barsbeat",
                        subtitle: "Исполнитель",
                        kind: "artist"
                    }
                ]
        }
    }
}