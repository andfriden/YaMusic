import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    height: 42

    // =============================================================
    // Search field
    // =============================================================

    TextField {
        id: searchField

        anchors.fill:
            parent

        leftPadding:
            42

        rightPadding:
            42

        placeholderText:
            "Поиск музыки"

        color:
            AppTheme.textPrimary

        placeholderTextColor:
            AppTheme.textMuted

        selectionColor:
            AppTheme.accent

        selectedTextColor:
            AppTheme.textPrimary

        font.pixelSize:
            14

        enabled:
            root.controller !== null &&
            root.controller !== undefined &&
            !root.controller.searching

        background:
            Rectangle {
                radius: 11

                color:
                    searchField.activeFocus
                        ? AppTheme.panelActive
                        : AppTheme.panelSubtle

                border.width:
                    searchField.activeFocus
                        ? 1
                        : 0

                border.color:
                    AppTheme.accent

                Behavior on color {
                    ColorAnimation {
                        duration: 120
                    }
                }
            }

        // ---------------------------------------------------------
        // Search icon
        // ---------------------------------------------------------

        Text {
            anchors.left:
                parent.left

            anchors.leftMargin:
                14

            anchors.verticalCenter:
                parent.verticalCenter

            text:
                "⌕"

            color:
                searchField.activeFocus
                    ? AppTheme.textPrimary
                    : AppTheme.textMuted

            font.pixelSize:
                23

            font.weight:
                Font.Light
        }

        // ---------------------------------------------------------
        // Loading indicator
        // ---------------------------------------------------------

        Text {
            anchors.right:
                parent.right

            anchors.rightMargin:
                14

            anchors.verticalCenter:
                parent.verticalCenter

            visible:
                root.controller !== null &&
                root.controller !== undefined &&
                root.controller.searching

            text:
                "…"

            color:
                AppTheme.accent

            font.pixelSize:
                18

            font.bold:
                true
        }

        // ---------------------------------------------------------
        // Search
        // ---------------------------------------------------------

        onAccepted: {
            root.performSearch()
        }
    }

    // =============================================================
    // Search
    // =============================================================

    function performSearch()
    {
        const query =
            searchField.text.trim()

        if (
            query.length === 0
        ) {
            return
        }

        if (
            root.controller === null ||
            root.controller === undefined
        ) {
            return
        }

        root.controller.search(
            query
        )
    }
}