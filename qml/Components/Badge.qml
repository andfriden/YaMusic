import QtQuick
import YaMusic 1.0

// Маленький бейдж-пилюля для пометок трека: «AI» (контент сгенерирован
// ИИ) и «Без цензуры» (у трека есть нецензурная версия).
Rectangle {
    id: root

    property string text: ""
    property color badgeColor: AppTheme.textMuted
    property color textColor: AppTheme.backgroundPrimary

    implicitWidth: badgeText.implicitWidth + 12
    implicitHeight: 16

    radius: height / 2
    color: root.badgeColor

    Text {
        id: badgeText

        anchors.centerIn: parent

        text: root.text
        color: root.textColor
        font.pixelSize: 9
        font.weight: Font.DemiBold
        font.letterSpacing: 0.4

        horizontalAlignment: Text.AlignHCenter
    }
}