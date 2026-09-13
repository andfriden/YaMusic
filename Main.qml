import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

import "Layout"
import "Home"
import "Components"
import "Pages"

ApplicationWindow {
    id: window

    width: 1440
    height: 900
    minimumWidth: 1100
    minimumHeight: 720
    visible: true
    title: "YaMusic"
    color: AppTheme.background

    property bool expandedNowPlayingVisible: false

Connections {
        target: appController

        function onStatusChanged(message) {
            statusBar.message = message
        }

        function onDarkThemeChanged() {
            AppTheme.dark = appController.darkTheme
        }
    }

    Component.onCompleted: {
        AppTheme.dark = appController.darkTheme
    }

    Item {
        id: settingsButton

        width: 38
        height: 38

        anchors.top:
            parent.top

        anchors.right:
            parent.right

        anchors.topMargin:
            15

        anchors.rightMargin:
            20

        visible:
            authController.authenticated

        z: 100

        Rectangle {
            anchors.fill:
                parent

            radius: 10

            color:
                settingsMouseArea.containsMouse
                    ? AppTheme.panelHover
                    : AppTheme.panelSubtle

            border.width:
                1

            border.color:
                settingsMouseArea.containsMouse
                    ? AppTheme.border
                    : AppTheme.borderSubtle

            Behavior on color {
                ColorAnimation {
                    duration: 120
                }
            }

            Behavior on border.color {
                ColorAnimation {
                    duration: 120
                }
            }
        }

        Text {
            anchors.centerIn:
                parent

            text:
                "⚙"

            color:
                settingsMouseArea.containsMouse
                    ? AppTheme.textPrimary
                    : AppTheme.textSecondary

            font.pixelSize:
                16

            Behavior on color {
                ColorAnimation {
                    duration: 120
                }
            }
        }

        MouseArea {
            id: settingsMouseArea

            anchors.fill:
                parent

            hoverEnabled:
                true

            cursorShape:
                Qt.PointingHandCursor

            onClicked: {
                settingsPopup.open()
            }
        }
    }

    Popup {
        id: settingsPopup

        width: 180
        height: 120

        x: settingsButton.x - width + settingsButton.width
        y: settingsButton.y + settingsButton.height + 6

        closePolicy:
            Popup.CloseOnEscape |
            Popup.CloseOnPressOutside

        background:
            Rectangle {
                color:
                    AppTheme.panel

                border.width:
                    1

                border.color:
                    AppTheme.borderSubtle

                radius: 10
            }

        Column {
            anchors.fill:
                parent

            anchors.margins:
                10

            spacing:
                8

            // Theme switcher

            Row {
                width:
                    parent.width

                height:
                    30

                spacing:
                    8

                Text {
                    text:
                        "Тёмная тема"

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        13

                    anchors.verticalCenter:
                        parent.verticalCenter
                }

                Item {
                    width:
                        20

                    height:
                        20

                    anchors.verticalCenter:
                        parent.verticalCenter

                    Rectangle {
                        anchors.fill:
                            parent

                        radius:
                            4

                        color:
                            AppTheme.dark
                                ? AppTheme.accent
                                : AppTheme.panelSecondary

                        border.width:
                            1

                        border.color:
                            AppTheme.border

                        Text {
                            anchors.centerIn:
                                parent

                            text:
                                AppTheme.dark
                                    ? "✓"
                                    : ""

                            color:
                                AppTheme.textPrimary

                            font.pixelSize:
                                12
                        }

                        MouseArea {
                            anchors.fill:
                                parent

                            cursorShape:
                                Qt.PointingHandCursor

                            onClicked: {
                                appController.setDarkTheme(
                                    !AppTheme.dark
                                )
                            }
                        }
                    }
                }
            }

            // Logout

            Rectangle {
                width:
                    parent.width

                height:
                    34

                radius:
                    8

                color:
                    logoutArea.containsMouse
                        ? AppTheme.panelActive
                        : AppTheme.panelSecondary

                border.width:
                    1

                border.color:
                    AppTheme.borderSubtle

                Text {
                    anchors.centerIn:
                        parent

                    text:
                        "Выйти"

                    color:
                        logoutArea.containsMouse
                            ? AppTheme.error
                            : AppTheme.textPrimary

                    font.pixelSize:
                        13

                    font.weight:
                        Font.Medium
                }

                MouseArea {
                    id: logoutArea

                    anchors.fill:
                        parent

                    hoverEnabled:
                        true

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        settingsPopup.close()
                        authController.logout()
                    }
                }
            }
        }
    }

    Loader {
        id: pageLoader

        anchors.fill: parent

        active: !authController.authenticated

        source: "Pages/LoginPage.qml"
    }

    Column {
        id: applicationLayout

        anchors.fill: parent
        spacing: 0

        visible: authController.authenticated &&
            !window.expandedNowPlayingVisible

        MainLayout {
            id: mainLayout

            width: parent.width
            height: parent.height - nowPlayingBar.height

            controller: appController
        }

        NowPlayingBar {
            id: nowPlayingBar

            width: parent.width
            height: 124

            controller: appController

            onExpandedRequested: {
                window.expandedNowPlayingVisible = true
            }
        }
    }

    StatusBar {
        id: statusBar

        visible: false
        message: "Готово"
    }

    ExpandedNowPlaying {
        id: expandedNowPlaying

        anchors.fill: parent

        visible: authController.authenticated &&
            window.expandedNowPlayingVisible

        controller: appController

        onClosed: {
            window.expandedNowPlayingVisible = false
        }
    }
}
