import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    Rectangle {
        anchors.fill: parent

        color: AppTheme.backgroundSecondary

        border.width: 1
        border.color: AppTheme.borderSubtle

        Column {
            anchors.fill: parent
            anchors.margins: 14

            spacing: 8

            Label {
                width: parent.width

                text:
                    qsTr(
                        "Похожие плейлисты"
                    )

                color: AppTheme.textPrimary

                font.pixelSize: 17
                font.bold: true
            }

            Label {
                width: parent.width

                text:
                    qsTr(
                        "Подборки в похожем стиле"
                    )

                color: AppTheme.textMuted

                font.pixelSize: 11

                wrapMode:
                    Text.WordWrap
            }
        }
    }
}