import QtQuick
import QtQuick.Controls.Basic

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
}

    Item {
        id: logoutButton

        width: 92
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
                logoutMouseArea.containsMouse
                    ? AppTheme.panelHover
                    : AppTheme.panelSubtle

            border.width:
                1

            border.color:
                logoutMouseArea.containsMouse
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
                "Выйти"

            color:
                logoutMouseArea.containsMouse
                    ? AppTheme.textPrimary
                    : AppTheme.textSecondary

            font.pixelSize:
                13

            font.weight:
                Font.Medium

            Behavior on color {
                ColorAnimation {
                    duration: 120
                }
            }
        }

        MouseArea {
            id: logoutMouseArea

            anchors.fill:
                parent

            hoverEnabled:
                true

            cursorShape:
                Qt.PointingHandCursor

            onClicked: {
                authController.logout()
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
