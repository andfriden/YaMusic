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

    // Голубая капсула из темы: хорошо выделяется на тёмном плеере и не
    // сливается со светлым фоном основного окна.
    x: root.alignRight
        ? parent.width - width
        : Math.round((parent.width - width) / 2)
    y: root.below ? parent.height + 6 : -height - 6

    background: Rectangle {
        // Капсула: радиус равен половине высоты, а не «коробка» со скруглением.
        radius: height / 2
        color: AppTheme.accent
        border.width: 1
        border.color: Qt.rgba(1, 1, 1, 0.28)
    }

    contentItem: Text {
        text: root.text
        // На светлом голубом (тёмная тема) нужен контрастный тёмный текст.
        color: AppTheme.dark ? AppTheme.macchiatoCrust : AppTheme.accentOn
        font.pixelSize: 10
        font.weight: Font.Medium

        leftPadding: 7
        rightPadding: 7
        topPadding: 3
        bottomPadding: 3
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
