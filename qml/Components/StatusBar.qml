import QtQuick
import QtQuick.Controls.Basic

/*
 * Статус-бар: всплывающее сообщение внизу окна.
 * Показывается через show(), исчезает через hideDelay мс.
 */
Label {
    id: root

    property string message: "Готово"
    property int hideDelay: 3000

    height: 24

    text: message

    color: AppTheme.textSecondary

    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter

    elide: Text.ElideRight

    opacity: 0
    visible: opacity > 0

    Behavior on opacity { NumberAnimation { duration: 200 } }

    function show(text) {
        root.message = text
        root.opacity = 1
        hideTimer.restart()
    }

    function hide() {
        root.opacity = 0
    }

    Timer {
        id: hideTimer
        interval: root.hideDelay
        onTriggered: root.hide()
    }
}