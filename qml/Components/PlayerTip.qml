import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

// Всплывающая подсказка для кнопок плеера. Появляется над кнопкой через
// небольшую задержку, пока курсор в её MouseArea, и не перехватывает клики.
Popup {
    id: root

    // MouseArea кнопки: подсказка видна, пока в неё наведён курсор.
    property var target: null
    property string text: ""

    // Для кнопок у правого края: подсказка прижимается вправо, чтобы
    // не выйти за окно.
    property bool alignRight: false

    // Для кнопок у верхнего края окна: подсказка показывается под ними.
    property bool below: false

    readonly property bool hovered:
        root.text.length > 0 &&
        root.target !== null && root.target !== undefined && root.target.containsMouse

    modal: false
    focus: false
    closePolicy: Popup.NoAutoClose
    visible: false

    // Тёмная подсказка читается и на светлой, и на тёмной теме — и
    // совпадает с мини-плеером, который всегда в палитре Macchiato.
    x: root.alignRight
        ? parent.width - width
        : Math.round((parent.width - width) / 2)
    y: root.below ? parent.height + 8 : -height - 8

    background: Rectangle {
        radius: 6
        color: AppTheme.macchiatoSurface0
        border.width: 1
        border.color: AppTheme.macchiatoSurface2
    }

    contentItem: Text {
        text: root.text
        color: AppTheme.macchiatoText
        font.pixelSize: 12

        leftPadding: 9
        rightPadding: 9
        topPadding: 6
        bottomPadding: 6
    }

    Timer {
        id: showTimer

        interval: 350
        repeat: false

        onTriggered: root.visible = true
    }

    onHoveredChanged: {
        if (root.hovered) {
            showTimer.restart()
        } else {
            showTimer.stop()
            root.visible = false
        }
    }
}
