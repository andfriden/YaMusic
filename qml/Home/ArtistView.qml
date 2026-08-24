import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item {
    id: root

    property var controller

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            Layout.fillWidth: true

            text: controller !== null
                ? controller.artistName
                : ""

            color: "white"
            font.pixelSize: 28
            font.bold: true

            elide: Text.ElideRight
        }

        RowLayout {
            Layout.fillWidth: true
            visible: controller !== null && controller.loading

            spacing: 8

            BusyIndicator {
                implicitWidth: 20
                implicitHeight: 20
                running: controller !== null &&
                    controller.loading
            }

            Label {
                text: qsTr("Loading tracks…")
                color: "#888888"
                font.pixelSize: 13
            }
        }

        ListView {
            id: tracksView

            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true
            spacing: 2

            model: controller !== null
                ? controller.artistModel
                : null

            delegate: Rectangle {
                id: trackItem

                required property int index
                required property string trackId
                required property string title
                required property string artist
                required property string album
                required property string coverUri
                required property int durationMs

                width: tracksView.width
                height: 64

                radius: 8

                color: mouseArea.containsMouse
                    ? "#292929"
                    : "transparent"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 12
                    spacing: 12

                    Rectangle {
                        Layout.preferredWidth: 48
                        Layout.preferredHeight: 48

                        radius: 6
                        color: "#252525"

                        clip: true

                        Image {
                            id: cover

                            anchors.fill: parent

                            source: trackItem.coverUri

                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            cache: true
                        }

                        Label {
                            anchors.centerIn: parent

                            text: "♪"

                            color: "#666666"
                            font.pixelSize: 20

                            visible: cover.status !== Image.Ready
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            Layout.fillWidth: true

                            text: trackItem.title

                            color: "white"
                            font.pixelSize: 14
                            font.bold: true

                            elide: Text.ElideRight
                        }

                        Label {
                            Layout.fillWidth: true

                            text: {
                                if (trackItem.artist.length > 0 &&
                                    trackItem.album.length > 0) {
                                    return trackItem.artist +
                                        " • " +
                                        trackItem.album
                                }

                                if (trackItem.artist.length > 0)
                                    return trackItem.artist

                                return trackItem.album
                            }

                            color: "#888888"
                            font.pixelSize: 12

                            elide: Text.ElideRight
                        }
                    }

                    Label {
                        text: {
                            if (trackItem.durationMs <= 0)
                                return ""

                            const totalSeconds =
                                Math.floor(
                                    trackItem.durationMs / 1000)

                            const minutes =
                                Math.floor(totalSeconds / 60)

                            const seconds =
                                totalSeconds % 60

                            return minutes +
                                ":" +
                                (seconds < 10 ? "0" : "") +
                                seconds
                        }

                        color: "#777777"
                        font.pixelSize: 12
                    }
                }

                MouseArea {
                    id: mouseArea

                    anchors.fill: parent

                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        if (root.controller !== null) {
                            root.controller.selectTrack(
                                trackItem.index)
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }

        Label {
            Layout.fillWidth: true
            Layout.fillHeight: true

            visible: controller !== null &&
                !controller.loading &&
                controller.artistModel !== null &&
                controller.artistModel.count === 0

            text: qsTr("No tracks")

            color: "#777777"
            font.pixelSize: 15

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}