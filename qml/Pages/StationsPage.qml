import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    readonly property var stationsController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.stationsController !== null && root.controller.stationsController !== undefined
            ? root.controller.stationsController
            : null

    readonly property var groups:
        root.stationsController ? root.stationsController.groups : []

    readonly property bool loading:
        root.stationsController ? root.stationsController.loading : false

    readonly property int margin: 24
    readonly property int spacing: 12
    readonly property int cardHeight: 108

    width: parent ? parent.width : 0
    implicitWidth: width
    implicitHeight: contentColumn.implicitHeight + 48

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    Column {
        id: contentColumn

        x: root.margin
        y: root.margin

        width: Math.max(0, root.width - root.margin * 2)
        spacing: 24

        Label {
            width: parent.width

            text: "Станции"
            color: AppTheme.textPrimary
            font.pixelSize: 28
            font.bold: true
            elide: Text.ElideRight
        }

        Label {
            width: parent.width

            visible: !root.loading && root.groups.length === 0

            text: root.stationsController !== null && root.stationsController !== undefined
                ? "Станции недоступны"
                : ""

            color: AppTheme.textSecondary
            font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
        }

        Repeater {
            model: root.groups

            delegate: Column {
                required property var modelData

                width: parent.width
                spacing: 10

                Label {
                    width: parent.width

                    text: modelData.label
                    color: AppTheme.textPrimary
                    font.pixelSize: 20
                    font.bold: true
                }

                ListView {
                    width: parent.width
                    height: root.cardHeight
                    orientation: ListView.Horizontal

                    clip: true
                    spacing: root.spacing

                    ScrollBar.horizontal: ScrollBar {
                        policy: ScrollBar.AsNeeded
                    }

                    model: modelData.items

                    delegate: Rectangle {
                        required property var modelData

                        width: 150
                        height: root.cardHeight

                        radius: 12
                        clip: true

                        color: modelData.backgroundColor && modelData.backgroundColor.length > 0
                            ? modelData.backgroundColor
                            : AppTheme.panelSecondary

                        Image {
                            anchors.fill: parent

                            source: modelData.imageUrl && modelData.imageUrl.length > 0
                                ? modelData.imageUrl
                                : ""

                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            cache: true
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: "#40000000"
                        }

                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom

                            height: 56

                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "transparent" }
                                GradientStop { position: 1.0; color: "#CC000000" }
                            }
                        }

                        Label {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.margins: 10

                            text: modelData.title || ""
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true

                            wrapMode: Text.Wrap
                            maximumLineCount: 2
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (root.stationsController) {
                                    root.stationsController.selectStation(
                                        modelData.type, modelData.tag, modelData.title)
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (root.stationsController !== null && root.stationsController !== undefined) {
            root.stationsController.loadStations()
        }
    }
}