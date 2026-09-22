import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Экран текста песни (оверлей).
 *
 * Если у трека есть синхронизированный текст (строки с таймкодами) —
 * показывается ListView с подсветкой текущей строки и автопрокруткой;
 * иначе — простой текст (Flickable).
 */
Item {
    id: root

    property var controller

    signal closed()

    anchors.fill: parent

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    readonly property bool hasTimedLines:
        root.hasController &&
        root.controller.lyricsController !== null && root.controller.lyricsController !== undefined &&
        root.controller.lyricsController.lyricsLineCount > 0

    readonly property int currentLine:
        root.hasTimedLines
            ? root.controller.lyricsController.currentLyricLine
            : -1

    readonly property string plainText:
        root.hasController &&
        root.controller.lyricsController !== null && root.controller.lyricsController !== undefined
            ? String(root.controller.lyricsController.lyricsText || "")
            : ""

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundSecondary

        // ---------------------------------------------------------
        // Шапка
        // ---------------------------------------------------------

        Item {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 24
            anchors.rightMargin: 24
            anchors.topMargin: 12

            height: 60

            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                width: parent.width - 60

                text: root.hasController
                    ? String(root.controller.currentTrackTitle || "")
                    : ""

                color: AppTheme.textPrimary
                font.pixelSize: 20
                font.weight: Font.DemiBold
                elide: Text.ElideRight
            }

            // Кнопка закрытия
            Item {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                width: 42
                height: 42

                Rectangle {
                    anchors.fill: parent
                    radius: width / 2

                    color: closeMouseArea.containsMouse
                        ? Qt.rgba(AppTheme.textPrimary.r, AppTheme.textPrimary.g,
                                  AppTheme.textPrimary.b, 0.12)
                        : "transparent"
                }

                Text {
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: -3

                    text: "⌄"
                    color: AppTheme.textSecondary
                    font.pixelSize: 24
                }

                MouseArea {
                    id: closeMouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.closed()
                }
            }
        }

        // ---------------------------------------------------------
        // Синхронизированный текст
        // ---------------------------------------------------------

        ListView {
            id: linesView

            anchors.top: parent.top
            anchors.topMargin: 90
            anchors.left: parent.left
            anchors.leftMargin: 72
            anchors.right: parent.right
            anchors.rightMargin: 72
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 40

            clip: true
            spacing: 10

            visible: root.hasTimedLines

            model: root.hasTimedLines
                ? root.controller.lyricsController.lyricsLineCount
                : 0

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

            delegate: Text {
                required property int index

                width: linesView.width

                text: root.controller.lyricsController.lyricLineText(index)

                color: index === root.currentLine ? AppTheme.accent : AppTheme.textPrimary
                font.pixelSize: index === root.currentLine ? 20 : 17
                font.weight: index === root.currentLine ? Font.DemiBold : Font.Normal

                lineHeight: 1.6
                wrapMode: Text.WordWrap
            }

            // Сигналы о смене строки и загрузке текста приходят от
            // LyricsController (не от корневого controller)
            Connections {
                target: root.hasController
                    ? root.controller.lyricsController
                    : null

                function onCurrentLyricLineChanged() {
                    if (!root.hasTimedLines || root.currentLine < 0) {
                        return
                    }

                    linesView.positionViewAtIndex(root.currentLine, ListView.Center)
                }

                function onLyricsChanged() {
                    if (root.hasTimedLines) {
                        linesView.positionViewAtIndex(0, ListView.Center)
                    }
                }
            }
        }

        // ---------------------------------------------------------
        // Простой текст
        // ---------------------------------------------------------

        Flickable {
            id: plainFlickable

            anchors.top: parent.top
            anchors.topMargin: 90
            anchors.left: parent.left
            anchors.leftMargin: 72
            anchors.right: parent.right
            anchors.rightMargin: 72
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 40

            contentWidth: parent.width - 144
            contentHeight: plainTextLabel.implicitHeight + 20

            clip: true
            visible: !root.hasTimedLines

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

            Text {
                id: plainTextLabel

                width: parent.width

                text: root.plainText

                color: AppTheme.textPrimary
                font.pixelSize: 17
                lineHeight: 1.8
                wrapMode: Text.WordWrap
            }
        }
    }
}