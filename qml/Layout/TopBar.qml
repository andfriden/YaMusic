import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property string currentSection: "home"
    property bool canGoBack: false

    signal sectionSelected(string section)
    signal backRequested()

    readonly property int barHeight: 82
    implicitHeight: root.barHeight

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    Row {
        anchors.fill: parent
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        spacing: 10

        Item {
            id: backArea

            width: root.canGoBack ? 42 : 0
            height: parent.height
            visible: root.canGoBack

            ToolButton {
                id: backButton

                width: 38
                height: 38

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                text: "‹"

                contentItem: Text {
                    text: backButton.text

                    color:
                        backButton.hovered
                            ? AppTheme.accent
                            : AppTheme.textPrimary

                    font.pixelSize: 30

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    radius: 8

                    color:
                        backButton.hovered
                            ? AppTheme.panelHover
                            : "transparent"
                }

                onClicked: {
                    root.backRequested()
                }
            }
        }

        Item {
            id: brand

            width: 150
            height: parent.height

            Rectangle {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                width: brandText.implicitWidth + 20
                height: 42
                radius: 12

                color:
                    brandMouseArea.containsMouse
                        ? AppTheme.panelHover
                        : "transparent"
            }

            Text {
                id: brandText

                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter

                text: "YaMusic"

                color:
                    brandMouseArea.containsMouse
                        ? AppTheme.accent
                        : AppTheme.textPrimary

                font.pixelSize: 24
                font.bold: true
            }

            MouseArea {
                id: brandMouseArea

                anchors.fill: parent

                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    root.sectionSelected("home")
                }
            }
        }

        Repeater {
            model: [
                { title: "Чарты", section: "chart" },
                { title: "Жанры", section: "genres" },
                { title: "Плейлисты", section: "playlists" },
                { title: "Спорт", section: "sport" },
                { title: "Мне нравится", section: "liked" }
            ]

            delegate: Item {
                id: navigationItem

                required property var modelData

                width: navigationText.implicitWidth + 32
                height: parent.height

                readonly property bool active:
                    root.currentSection === modelData.section

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    height: 42
                    radius: 12

                    color:
                        navigationItem.active
                            ? AppTheme.panelActive
                            : (
                                navigationMouseArea.containsMouse
                                    ? AppTheme.panelHover
                                    : "transparent"
                            )
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 3
                    radius: 2

                    color:
                        navigationItem.active
                            ? AppTheme.accent
                            : "transparent"
                }

                Text {
                    id: navigationText

                    anchors.centerIn: parent

                    text: navigationItem.modelData.title

                    color:
                        navigationItem.active
                            ? AppTheme.textPrimary
                            : AppTheme.textSecondary

                    font.pixelSize: 15

                    font.weight:
                        navigationItem.active
                            ? Font.DemiBold
                            : Font.Normal
                }

                MouseArea {
                    id: navigationMouseArea

                    anchors.fill: parent

                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        root.sectionSelected(
                            navigationItem.modelData.section
                        )
                    }
                }
            }
        }
    }
}