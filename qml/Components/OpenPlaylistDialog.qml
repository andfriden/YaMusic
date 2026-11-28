import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

// Ввод ссылки на плейлист Яндекс Музыки. Ссылку разбирает контроллер;
// здесь только поле, подсказка и сообщение об ошибке.
Popup {
    id: root

    property var controller
    property string errorText: ""

    anchors.centerIn: Overlay.overlay

    width: Math.min(460, Overlay.overlay ? Overlay.overlay.width * 0.85 : 460)
    height: contentColumn.implicitHeight + 40

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        color: AppTheme.panel
        radius: 12
        border.width: 1
        border.color: AppTheme.border
    }

    onOpened: {
        root.errorText = ""
        linkField.text = ""
        linkField.forceActiveFocus()
    }

    function submit() {
        if (root.controller === null || root.controller === undefined) {
            return
        }

        if (linkField.text.trim().length === 0) {
            root.errorText = "Вставьте ссылку на плейлист"
            return
        }

        if (root.controller.openPlaylistUrl(linkField.text)) {
            root.close()
        } else {
            root.errorText = "Не удалось разобрать ссылку. Нужна ссылка вида music.yandex.ru/users/…/playlists/…"
        }
    }

    Column {
        id: contentColumn

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20

        spacing: 12

        Label {
            width: parent.width

            text: "Открыть плейлист по ссылке"
            color: AppTheme.textPrimary
            font.pixelSize: 18
            font.bold: true
        }

        Label {
            width: parent.width

            text: "Подойдёт ссылка на свой или чужой плейлист: music.yandex.ru/users/<логин>/playlists/<номер> или music.yandex.ru/playlists/<uuid>."
            color: AppTheme.textSecondary
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        TextField {
            id: linkField

            width: parent.width

            placeholderText: "https://music.yandex.ru/users/…/playlists/…"

            selectByMouse: true

            onTextChanged: root.errorText = ""
            onAccepted: root.submit()
        }

        Label {
            width: parent.width

            text: root.errorText
            color: AppTheme.error
            font.pixelSize: 11
            wrapMode: Text.Wrap

            visible: text.length > 0
        }

        Row {
            width: parent.width
            spacing: 8
            layoutDirection: Qt.RightToLeft

            Rectangle {
                width: openLabel.implicitWidth + 28
                height: 30
                radius: 8

                color: openMouse.containsMouse ? AppTheme.accentHover : AppTheme.accent

                Label {
                    id: openLabel

                    anchors.centerIn: parent

                    text: "Открыть"
                    color: AppTheme.accentOn
                    font.pixelSize: 12
                    font.bold: true
                }

                MouseArea {
                    id: openMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.submit()
                }
            }

            Rectangle {
                width: cancelLabel.implicitWidth + 28
                height: 30
                radius: 8

                color: cancelMouse.containsMouse ? AppTheme.panelHover : AppTheme.panelSecondary

                border.width: 1
                border.color: AppTheme.borderSubtle

                Label {
                    id: cancelLabel

                    anchors.centerIn: parent

                    text: "Отмена"
                    color: AppTheme.textPrimary
                    font.pixelSize: 12
                }

                MouseArea {
                    id: cancelMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.close()
                }
            }
        }
    }
}
