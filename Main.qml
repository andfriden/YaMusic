import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core

ApplicationWindow {
    id: window

    width: 640
    height: 480
    visible: true
    title: "YaMusic"

    AppController {
        id: appController

        onStatusChanged: function(message) {
            statusLabel.text = message
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 20

        Label {
            text: "YaMusic"
            font.pixelSize: 32
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            text: "Test AppController"
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: appController.testConnection()
        }

        Button {
             text: "Test Yandex API"
             anchors.horizontalCenter: parent.horizontalCenter

             onClicked: appController.testYandexApi()
          }
        

        Label {
            id: statusLabel
            text: "Ready"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
