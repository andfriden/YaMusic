import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    // Текст поля доступен странице: история и живой поиск работают
    // с тем, что пользователь уже ввёл.
    property alias text: searchField.text

    property int activeSuggestion: -1

    height: 42

    // Подсказки рисуются поверх соседних элементов страницы.
    z: 10

    readonly property var searchController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.searchController !== undefined
            ? root.controller.searchController
            : null

    readonly property var suggestions:
        root.searchController !== null &&
        root.searchController.suggestions !== undefined
            ? root.searchController.suggestions
            : []

    signal queryEdited(string text)
    signal searchSubmitted()

    function focusSearch() {
        searchField.forceActiveFocus()
    }

    function setText(value) {
        searchField.text = value
    }

    function performSearch() {
        const query = searchField.text.trim()

        root.searchSubmitted()

        // Живой поиск идёт напрямую в контроллер: AppController::search()
        // заодно переключает раздел и перезагружает страницу — при вводе
        // это сбрасывало бы фокус и результаты.
        if (root.searchController !== null) {
            root.searchController.search(query)
            return
        }

        if (query.length === 0) {
            return
        }

        if (root.controller !== null && root.controller !== undefined) {
            root.controller.search(query)
        }
    }

    function activateSuggestion(index) {
        if (index < 0 || index >= root.suggestions.length) {
            return
        }

        searchField.text = root.suggestions[index]
        root.activeSuggestion = -1

        root.performSearch()
        searchField.forceActiveFocus()
    }

    TextField {
        id: searchField

        anchors.fill: parent

        leftPadding: 42
        rightPadding: 42

        placeholderText: "Поиск музыки"

        color: AppTheme.textPrimary
        placeholderTextColor: AppTheme.textMuted

        selectionColor: AppTheme.accent
        selectedTextColor: AppTheme.textPrimary

        font.pixelSize: 14

        // Поле не блокируем на время живого поиска: иначе фокус
        // сбрасывался бы после первой же паузы в наборе.
        enabled: root.controller !== null && root.controller !== undefined

        background: Rectangle {
            radius: 11

            color: searchField.activeFocus ? AppTheme.panelActive : AppTheme.panelSubtle

            border.width: searchField.activeFocus ? 1 : 0
            border.color: AppTheme.accent

            Behavior on color {
                ColorAnimation { duration: 120 }
            }
        }

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.verticalCenter: parent.verticalCenter

            text: "⌕"
            color: searchField.activeFocus ? AppTheme.textPrimary : AppTheme.textMuted
            font.pixelSize: 23
            font.weight: Font.Light
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.verticalCenter: parent.verticalCenter

            visible: root.controller !== null && root.controller !== undefined &&
                root.controller.searching

            text: "…"
            color: AppTheme.accent
            font.pixelSize: 18
            font.bold: true
        }

        onTextEdited: {
            root.activeSuggestion = -1
            root.queryEdited(searchField.text)
        }

        onAccepted: {
            if (root.activeSuggestion >= 0) {
                root.activateSuggestion(root.activeSuggestion)
                return
            }

            root.performSearch()
        }

        onActiveFocusChanged: {
            if (!searchField.activeFocus) {
                root.activeSuggestion = -1
            }
        }

        Keys.onDownPressed: function(event) {
            if (root.suggestions.length === 0) {
                return
            }

            root.activeSuggestion = Math.min(
                root.activeSuggestion + 1,
                root.suggestions.length - 1)
            event.accepted = true
        }

        Keys.onUpPressed: function(event) {
            if (root.suggestions.length === 0) {
                return
            }

            root.activeSuggestion = Math.max(root.activeSuggestion - 1, -1)
            event.accepted = true
        }

        Keys.onEscapePressed: function(event) {
            root.activeSuggestion = -1
            event.accepted = true
        }
    }

    Rectangle {
        id: suggestPopup

        anchors.top: parent.bottom
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.right: parent.right

        height: Math.min(root.suggestions.length, 7) * 32 + 8

        radius: 10
        color: AppTheme.panel
        border.width: 1
        border.color: AppTheme.borderSubtle

        visible: searchField.activeFocus && root.suggestions.length > 0 &&
            searchField.text.trim().length >= 2

        z: 20

        ListView {
            anchors.fill: parent
            anchors.margins: 4

            clip: true
            model: root.suggestions

            delegate: Rectangle {
                required property int index
                required property string modelData

                width: ListView.view.width
                height: 32
                radius: 6

                color: index === root.activeSuggestion
                    ? AppTheme.panelActive
                    : suggestMouse.containsMouse ? AppTheme.panelHover : "transparent"

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter

                    text: modelData
                    color: AppTheme.textPrimary
                    font.pixelSize: 13
                    elide: Text.ElideRight
                }

                MouseArea {
                    id: suggestMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.activateSuggestion(index)
                }
            }
        }
    }
}
