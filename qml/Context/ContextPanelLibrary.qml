import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    Rectangle {
        anchors.fill: parent

        color: "#ededed"

        border.width: 1
        border.color: "#d7d7d7"

        Column {
            anchors.fill: parent
            anchors.margins: 14

            spacing: 8

            Label {
                width: parent.width

                text:
                    qsTr(
                        "Вам может понравиться"
                    )

                color: "#202020"

                font.pixelSize: 17
                font.bold: true
            }

            Label {
                width: parent.width

                text:
                    qsTr(
                        "Музыка, которая может вам понравиться"
                    )

                color: "#777777"

                font.pixelSize: 11

                wrapMode:
                    Text.WordWrap
            }
        }
    }
}