import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    Rectangle {
        anchors.fill: parent

        color: AppTheme.backgroundSecondary

        border.width: 1
        border.color: Qt.rgba(
            AppTheme.border.r,
            AppTheme.border.g,
            AppTheme.border.b,
            0.35
        )

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

                color: AppTheme.textPrimary

                font.pixelSize: 17
                font.bold: true
            }

            Label {
                width: parent.width

                text:
                    qsTr(
                        "Музыка, которая может вам понравиться"
                    )

                color: AppTheme.textMuted

                font.pixelSize: 11

                wrapMode:
                    Text.WordWrap
            }
        }
    }
}