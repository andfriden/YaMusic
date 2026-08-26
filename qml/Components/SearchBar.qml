import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    height: 40

    Row {
        anchors.fill: parent

        spacing: 10

        TextField {
            id: searchField

            width:
                parent.width -
                searchButton.width -
                10

            height: parent.height

            placeholderText:
                "Поиск музыки..."

            color: AppTheme.textPrimary

            placeholderTextColor:
                AppTheme.textMuted

            selectionColor:
                "#8ab4f8"

            selectedTextColor:
                "#101010"

            background: Rectangle {
                radius: 6

                color: "#ffffff"

                border.width: 1

                border.color:
                    searchField.activeFocus
                        ? AppTheme.textMuted
                        : AppTheme.borderStrong
            }

            enabled:
                !root.controller.searching

            onAccepted: {
                root.performSearch()
            }
        }

        Button {
            id: searchButton

            width: 100

            height: parent.height

            text:
                root.controller.searching
                    ? "Поиск..."
                    : "Найти"

            enabled:
                !root.controller.searching &&
                searchField.text.trim().length > 0

            onClicked: {
                root.performSearch()
            }
        }
    }

    function performSearch() {
        var query =
            searchField.text.trim()

        if (query.length === 0) {
            return
        }

        root.controller.testSearch(
            query)
    }
}