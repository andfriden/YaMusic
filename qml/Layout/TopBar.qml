import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YaMusic 1.0

Item {
    id: root

    property var controller
    property var authController
    property string currentSection: "home"
    property bool canGoBack: false

    signal sectionSelected(string section)
    signal backRequested()

    readonly property int barHeight: 68

    implicitHeight: root.barHeight

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 28
        anchors.rightMargin: 28

        spacing: 8

        Item {
            id: backArea

            Layout.preferredWidth: root.canGoBack ? 40 : 0
            Layout.preferredHeight: 36

            visible: root.canGoBack

            Rectangle {
                anchors.fill: parent
                radius: 10

                color: backMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
            }

            Text {
                anchors.centerIn: parent

                text: "‹"
                color: backMouseArea.containsMouse ? AppTheme.textPrimary : AppTheme.textSecondary
                font.pixelSize: 30
                font.weight: Font.Light
            }

            MouseArea {
                id: backMouseArea

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: root.backRequested()
            }
        }

        Item {
            Layout.preferredWidth: 150
            Layout.fillHeight: true

            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                text: "YaMusic"
                color: brandMouseArea.containsMouse ? AppTheme.accent : AppTheme.textPrimary
                font.pixelSize: 25
                font.weight: Font.Bold

                Behavior on color {
                    ColorAnimation { duration: 120 }
                }
            }

            MouseArea {
                id: brandMouseArea

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: root.sectionSelected("home")
            }
        }

        Repeater {
            model: [
                { title: "Моя волна", section: "wave" },
                { title: "Чарты", section: "chart" },
                { title: "Жанры", section: "genres" },
                { title: "Плейлисты", section: "playlists" },
                { title: "Спорт", section: "sport" },
                { title: "Мне нравится", section: "liked" }
            ]

            delegate: Item {
                id: navigationItem

                required property var modelData

                readonly property bool active:
                    root.currentSection === modelData.section

                Layout.preferredWidth: navigationText.implicitWidth + 28
                Layout.preferredHeight: 38

                Rectangle {
                    anchors.fill: parent
                    radius: 10

                    color: navigationItem.active
                        ? AppTheme.panelActive
                        : navigationMouseArea.containsMouse
                            ? AppTheme.panelHover
                            : "transparent"

                    Behavior on color {
                        ColorAnimation { duration: 100 }
                    }
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 2
                    radius: 1

                    color: navigationItem.active ? AppTheme.accent : "transparent"

                    Behavior on color {
                        ColorAnimation { duration: 120 }
                    }
                }

                Text {
                    id: navigationText

                    anchors.centerIn: parent

                    text: navigationItem.modelData.title
                    color: navigationItem.active ? AppTheme.textPrimary : AppTheme.textSecondary
                    font.pixelSize: 14
                    font.weight: navigationItem.active ? Font.DemiBold : Font.Normal

                    Behavior on color {
                        ColorAnimation { duration: 100 }
                    }
                }

                MouseArea {
                    id: navigationMouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.sectionSelected(navigationItem.modelData.section)
                }
            }
        }

        // Распорка: прижимает настройки к правому краю
        Item {
            Layout.fillWidth: true
        }

        Item {
            id: settingsButton

            Layout.preferredWidth: 38
            Layout.preferredHeight: 38

            Rectangle {
                anchors.fill: parent
                radius: 10

                color: settingsMouseArea.containsMouse ? AppTheme.panelHover : AppTheme.panelSubtle

                border.width: 1
                border.color: settingsMouseArea.containsMouse ? AppTheme.border : AppTheme.borderSubtle

                Behavior on color {
                    ColorAnimation { duration: 120 }
                }

                Behavior on border.color {
                    ColorAnimation { duration: 120 }
                }
            }

            Text {
                anchors.centerIn: parent

                text: "⚙"
                color: settingsMouseArea.containsMouse ? AppTheme.textPrimary : AppTheme.textSecondary
                font.pixelSize: 16

                Behavior on color {
                    ColorAnimation { duration: 120 }
                }
            }

            MouseArea {
                id: settingsMouseArea

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: settingsPopup.open()
            }
        }
    }

    Popup {
        id: settingsPopup

        width: 180
        height: 120

        x: settingsButton.x + settingsButton.width - width
        y: settingsButton.y + settingsButton.height + 6

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            color: AppTheme.panel
            border.width: 1
            border.color: AppTheme.borderSubtle
            radius: 10
        }

        Column {
            anchors.fill: parent
            anchors.margins: 10

            spacing: 8

            Row {
                width: parent.width
                height: 30
                spacing: 8

                Text {
                    text: "Тёмная тема"
                    color: AppTheme.textPrimary
                    font.pixelSize: 13

                    anchors.verticalCenter: parent.verticalCenter
                }

                Item {
                    width: 20
                    height: 20

                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        anchors.fill: parent
                        radius: 4

                        color: AppTheme.dark ? AppTheme.accent : AppTheme.panelSecondary

                        border.width: 1
                        border.color: AppTheme.border

                        Text {
                            anchors.centerIn: parent

                            text: AppTheme.dark ? "✓" : ""
                            color: AppTheme.accentOn
                            font.pixelSize: 12
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (root.controller) {
                                    root.controller.themeController.setDarkTheme(!AppTheme.dark)
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 34
                radius: 8

                color: logoutArea.containsMouse ? AppTheme.panelActive : AppTheme.panelSecondary

                border.width: 1
                border.color: AppTheme.borderSubtle

                Text {
                    anchors.centerIn: parent

                    text: "Выйти"
                    color: logoutArea.containsMouse ? AppTheme.error : AppTheme.textPrimary
                    font.pixelSize: 13
                    font.weight: Font.Medium
                }

                MouseArea {
                    id: logoutArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        settingsPopup.close()

                        if (root.authController) {
                            root.authController.logout()
                        }
                    }
                }
            }
        }
    }
}