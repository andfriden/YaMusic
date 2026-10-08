import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    implicitWidth: width
    implicitHeight: content.implicitHeight
    height: implicitHeight

    Component.onCompleted: {
        if (root.controller !== null && root.controller !== undefined) {
            root.controller.loadMyWave()
            root.controller.loadRecommendations()
            root.controller.loadWheel()
        }
    }

    Column {
        id: content

        width: parent.width
        spacing: 24

        SearchBar {
            width: content.width
            controller: root.controller
        }

        MyWaveSection {
            width: content.width
            compactMode: true
            controller: root.controller
        }

        Column {
            width: content.width
            visible: root.controller &&
                root.controller.personalController &&
                root.controller.personalController.wheelItems &&
                root.controller.personalController.wheelItems.length > 0

            spacing: 10

            Label {
                width: parent.width

                text: "Колесо"
                color: AppTheme.textPrimary
                font.pixelSize: 20
                font.bold: true
            }

            ListView {
                width: parent.width
                height: 140
                orientation: ListView.Horizontal
                spacing: 12
                clip: true

                ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }

                model: root.controller.personalController.wheelItems

                delegate: Rectangle {
                    required property var modelData

                    width: 140
                    height: 140
                    radius: 12
                    clip: true

                    color: AppTheme.panelSecondary

                    Image {
                        anchors.fill: parent

                        source: modelData.coverUri && modelData.coverUri.length > 0
                            ? "image://yandex/" + modelData.coverUri
                            : ""
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true
                        cache: true
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        height: 44

                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "transparent" }
                            GradientStop { position: 1.0; color: "#CC000000" }
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: 8

                        text: modelData.title || ""
                        color: "white"
                        font.pixelSize: 12
                        font.bold: true
                        elide: Text.ElideRight
                        maximumLineCount: 1
                    }
                }
            }
        }

        PersonalPlaylistsSection {
            width: content.width
            controller: root.controller
            homeMode: true
        }

        RecentListeningSection {
            width: content.width
            controller: root.controller
        }
    }
}