import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    // Временный экспериментальный выбор раскладки:
    // "shelves" (полки) / "tabs" (табы). После выбора один убрать.
    property string layoutMode: "shelves"

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
    readonly property int columns: 6
    readonly property int spacing: 12
    readonly property real cardWidth:
        Math.floor((root.width - root.margin * 2 - spacing * (columns - 1)) / columns)
    readonly property int cardHeight: 96

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
        spacing: 20

        Label {
            width: parent.width

            text: "Станции"
            color: AppTheme.textPrimary
            font.pixelSize: 28
            font.bold: true
            elide: Text.ElideRight
        }

        // Временный переключатель раскладки для сравнения
        Row {
            spacing: 8

            Repeater {
                model: [
                    { id: "shelves", title: "Полки" },
                    { id: "tabs", title: "Табы" }
                ]

                delegate: Rectangle {
                    required property var modelData

                    width: layoutPillLabel.implicitWidth + 24
                    height: 30

                    radius: 15
                    color: root.layoutMode === modelData.id
                        ? AppTheme.accent
                        : AppTheme.panel

                    Label {
                        id: layoutPillLabel

                        anchors.centerIn: parent

                        text: modelData.title
                        color: root.layoutMode === modelData.id
                            ? AppTheme.accentOn
                            : AppTheme.textSecondary
                        font.pixelSize: 12
                        font.bold: root.layoutMode === modelData.id
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: root.layoutMode = modelData.id
                    }
                }
            }
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

        // ============ РАСКЛАДКА: ПОЛКИ (горизонтальные ряды по секциям) ============
        Column {
            visible: root.layoutMode === "shelves"
            width: parent.width
            spacing: 24

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
                                anchors.margins: 10

                                text: modelData.title || ""
                                color: "white"
                                font.pixelSize: 13
                                font.bold: true

                                elide: Text.ElideRight
                                maximumLineCount: 1
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

        // ============ РАСКЛАДКА: ТАБЫ (чипы по типу + сетка активного) ============
        Column {
            id: tabsColumn

            visible: root.layoutMode === "tabs"
            width: parent.width
            spacing: 20

            property int currentGroup: root.groups.length > 0 ? 0 : -1

            Flow {
                width: parent.width
                spacing: 8

                Repeater {
                    model: root.groups

                    delegate: Rectangle {
                        required property var modelData

                        width: tabLabel.implicitWidth + 28
                        height: 32

                        radius: 16
                        color: tabsColumn.currentGroup === index
                            ? AppTheme.accent
                            : AppTheme.panel

                        border.width: 1
                        border.color: AppTheme.borderSubtle

                        Label {
                            id: tabLabel

                            anchors.centerIn: parent

                            text: modelData.label
                            color: tabsColumn.currentGroup === index
                                ? AppTheme.accentOn
                                : AppTheme.textSecondary
                            font.pixelSize: 13
                            font.bold: tabsColumn.currentGroup === index

                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor

                            onClicked: tabsColumn.currentGroup = index
                        }
                    }
                }
            }

            Flow {
                width: parent.width
                spacing: root.spacing

                Repeater {
                    model: root.groups.length > 0 ? root.groups[tabsColumn.currentGroup].items : []

                    delegate: Rectangle {
                        required property var modelData

                        width: root.cardWidth
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
                            anchors.margins: 10

                            text: modelData.title || ""
                            color: "white"
                            font.pixelSize: 13
                            font.bold: true

                            elide: Text.ElideRight
                            maximumLineCount: 1
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