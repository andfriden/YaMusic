import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YaMusic 1.0

// Предложение обновиться: что за версия, что в ней изменилось и кнопки
// «скачать и установить», «позже» и «пропустить версию». Установка
// доступна не всегда — из каталога сборки предлагается страница релиза.
Popup {
    id: root

    property var controller

    readonly property var updates:
        root.controller !== null && root.controller !== undefined
            ? root.controller.updateController
            : null

    readonly property bool hasUpdates:
        root.updates !== null && root.updates !== undefined

    readonly property real progressValue:
        root.hasUpdates ? Math.max(0, Math.min(1, root.updates.downloadProgress)) : 0

    anchors.centerIn: Overlay.overlay

    width: Math.min(470, Overlay.overlay ? Overlay.overlay.width * 0.86 : 470)
    height: Math.min(540, Overlay.overlay ? Overlay.overlay.height * 0.82 : 540)

    modal: true
    closePolicy: Popup.CloseOnEscape

    background: Rectangle {
        color: AppTheme.panel
        radius: 12
        border.width: 1
        border.color: AppTheme.border
    }

    onClosed: {
        // Закрытие диалога — это «позже»: версия остаётся доступной,
        // но окно больше не открывается само.
        if (root.hasUpdates && !root.updates.readyToRestart) {
            root.updates.dismiss()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18

        spacing: 12

        Label {
            Layout.fillWidth: true

            text: "Обновление YaMusic"
            color: AppTheme.textPrimary
            font.pixelSize: 18
            font.bold: true
        }

        Label {
            Layout.fillWidth: true

            text: root.hasUpdates
                ? "Установлена " + root.updates.currentVersion +
                  " · доступна " + root.updates.latestVersion
                : ""

            color: AppTheme.textSecondary
            font.pixelSize: 13
            elide: Text.ElideRight
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 6

            radius: 3
            color: AppTheme.panelSecondary

            visible: root.hasUpdates && root.updates.installing

            Rectangle {
                width: parent.width * root.progressValue
                height: parent.height

                radius: 3
                color: AppTheme.accent

                Behavior on width {
                    NumberAnimation { duration: 120 }
                }
            }
        }

        Flickable {
            id: notesView

            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true
            contentWidth: width
            contentHeight: notesLabel.implicitHeight
            boundsBehavior: Flickable.StopAtBounds

            Label {
                id: notesLabel

                width: notesView.width

                text: root.hasUpdates ? root.updates.releaseNotes : ""
                textFormat: Text.MarkdownText
                wrapMode: Text.WordWrap

                color: AppTheme.textSecondary
                font.pixelSize: 12
            }
        }

        Label {
            Layout.fillWidth: true

            // Установка невозможна (сборка из каталога разработчика,
            // нет прав) — говорим почему и ведём на страницу релиза.
            text: root.hasUpdates && !root.updates.canInstall ? root.updates.installHint : ""
            visible: text.length > 0
            wrapMode: Text.WordWrap
            color: AppTheme.textMuted
            font.pixelSize: 11
        }

        Label {
            Layout.fillWidth: true

            text: root.hasUpdates ? root.updates.status : ""
            visible: text.length > 0
            wrapMode: Text.WordWrap
            color: AppTheme.textSecondary
            font.pixelSize: 12
        }

        RowLayout {
            Layout.fillWidth: true

            spacing: 8

            Button {
                text: "Скачать и установить"
                visible: root.hasUpdates && root.updates.canInstall &&
                    !root.updates.installing && !root.updates.readyToRestart

                onClicked: {
                    if (root.hasUpdates) {
                        root.updates.downloadAndInstall()
                    }
                }
            }

            Button {
                text: "Перезапустить"
                visible: root.hasUpdates && root.updates.readyToRestart

                onClicked: {
                    if (root.hasUpdates) {
                        root.updates.restartApplication()
                    }
                }
            }

            Button {
                text: "Страница релиза"

                onClicked: {
                    if (root.hasUpdates) {
                        root.updates.openReleasePage()
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Пропустить версию"
                visible: root.hasUpdates && !root.updates.installing

                onClicked: {
                    if (root.hasUpdates) {
                        root.updates.skipVersion()
                    }

                    root.close()
                }
            }

            Button {
                text: "Позже"
                visible: root.hasUpdates && !root.updates.installing &&
                    !root.updates.readyToRestart

                onClicked: root.close()
            }
        }
    }
}
