import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    readonly property var myWaveModel:
            controller !== null &&
        controller !== undefined &&
        controller.myWaveModel !== undefined &&
        controller.myWaveModel !== null
        ? controller.myWaveModel
        : null

    Rectangle {
        anchors.fill: parent

        radius: 10
        color: "#f5f5f5"

        border.width: 1
        border.color: "#dddddd"

        Column {
            anchors.fill: parent
            anchors.margins: 14

            spacing: 12

            Label {
                width: parent.width

                text:
                        root.myWaveModel !== null
                    ? "Моя волна  •  " + root.myWaveModel.count
                    : "Моя волна"

                color: "#202020"

                font.pixelSize: 18
                font.bold: true
            }

            Label {
                visible:
                    root.myWaveModel === null ||
                    root.myWaveModel.count === 0

                width: parent.width

                text:
                        root.myWaveModel === null
                    ? "Модель My Wave недоступна"
                    : "Моя волна пока пуста"

                color: "#888888"

                font.pixelSize: 13

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                wrapMode: Text.WordWrap
            }

            ListView {
                id: tracksView

                visible:
                    root.myWaveModel !== null &&
                    root.myWaveModel.count > 0

                width: parent.width
                height: parent.height - 46

                clip: true

                spacing: 6

                model: root.myWaveModel

                delegate: Rectangle {
                    id: trackDelegate

                    required property int index
                    required property string title
                    required property string artist
                    required property string album
                    required property string coverUri
                    required property string trackId

                    width: tracksView.width
                    height: 64

                    radius: 8

                    color:
                        mouseArea.containsMouse
                            ? "#e5e5e5"
                            : "#ededed"

                    border.width: 1
                    border.color: "#dddddd"

                    Row {
                        anchors.fill: parent

                        anchors.leftMargin: 6
                        anchors.rightMargin: 8

                        spacing: 10

                        Rectangle {
                            width: 52
                            height: 52

                            anchors.verticalCenter: parent.verticalCenter

                            radius: 6

                            color: "#d8d8d8"

                            clip: true

                            Image {
                                anchors.fill: parent

                                source:
                                        trackDelegate.coverUri !== ""
                                    ? "image://yandex/" +
                                    trackDelegate.coverUri
                                    : ""

                                fillMode: Image.PreserveAspectCrop

                                asynchronous: true
                                cache: true
                            }

                            Label {
                                anchors.centerIn: parent

                                text: "♪"

                                color: "#999999"

                                font.pixelSize: 20

                                visible:
                                    trackDelegate.coverUri === ""
                            }
                        }

                        Column {
                            width: parent.width - 68

                            anchors.verticalCenter: parent.verticalCenter

                            spacing: 3

                            Label {
                                width: parent.width

                                text:
                                        trackDelegate.title !== ""
                                    ? trackDelegate.title
                                    : "Без названия"

                                color: "#202020"

                                font.pixelSize: 13
                                font.bold: true

                                elide: Text.ElideRight
                            }

                            Label {
                                width: parent.width

                                text:
                                        trackDelegate.artist !== ""
                                    ? trackDelegate.artist
                                    : "Неизвестный исполнитель"

                                color: "#555555"

                                font.pixelSize: 11

                                elide: Text.ElideRight
                            }

                            Label {
                                width: parent.width

                                text:
                                        trackDelegate.album !== ""
                                    ? trackDelegate.album
                                    : "Без альбома"

                                color: "#888888"

                                font.pixelSize: 10

                                elide: Text.ElideRight
                            }
                        }
                    }

                    MouseArea {
                        id: mouseArea

                        anchors.fill: parent

                        hoverEnabled: true

                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            console.log(
                                "My Wave selected:",
                                trackDelegate.index,
                                trackDelegate.title,
                                "trackId:",
                                trackDelegate.trackId
                            )

                            if (
                                root.controller !== null &&
                                root.controller !== undefined &&
                                root.controller.selectMyWaveTrack !== undefined
                            ) {
                                root.controller.selectMyWaveTrack(
                                    trackDelegate.index
                                )
                            } else {
                                console.warn(
                                    "My Wave controller/selectMyWaveTrack unavailable"
                                )
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("========================================")
        console.log("ContextPanelMyWave CREATED")
        console.log("controller:", root.controller)
        console.log("myWaveModel:", root.myWaveModel)

        console.log(
            "myWaveModel.count:",
                root.myWaveModel !== null
                ? root.myWaveModel.count
                : -1
        )

        console.log("========================================")
    }

    Connections {
        target: root.myWaveModel

        function onCountChanged() {
            console.log(
                "ContextPanelMyWave count:",
                    root.myWaveModel !== null
                    ? root.myWaveModel.count
                    : -1
            )
        }
    }
}