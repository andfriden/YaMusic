import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YaMusic 1.0

// Отчёты о падениях: список сохранённых отчётов, текст выбранного и
// отправка в трекер проекта.
//
// Диалог открывается сам, если прошлый запуск завершился падением: отчёт
// уже лежит на диске, и пользователю остаётся решить, отправлять ли его.
Popup {
    id: root

    property var controller

    readonly property var reporter:
        root.controller !== null && root.controller !== undefined &&
        root.controller.crashReporter !== undefined
            ? root.controller.crashReporter
            : null

    readonly property bool available: root.reporter !== null
    readonly property var titles: root.available ? root.reporter.reportTitles : []
    readonly property string reportText: root.available ? root.reporter.selectedText : ""
    readonly property string startupSummary: root.available ? root.reporter.startupSummary : ""
    readonly property string status: root.available ? root.reporter.status : ""

    anchors.centerIn: Overlay.overlay

    width: Math.min(820, Overlay.overlay ? Overlay.overlay.width * 0.9 : 820)
    height: Math.min(580, Overlay.overlay ? Overlay.overlay.height * 0.85 : 580)

    modal: true
    closePolicy: Popup.CloseOnEscape

    background: Rectangle {
        color: AppTheme.panel
        radius: 12
        border.width: 1
        border.color: AppTheme.border
    }

    onClosed: {
        // Диалог закрыт — отчёты увидены: при следующем запуске о них не
        // напоминаем.
        if (root.available) {
            root.reporter.acknowledgeReports()
        }
    }

    Component.onCompleted: {
        if (root.available && root.reporter.startupCrash) {
            root.open()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18

        spacing: 10

        Label {
            Layout.fillWidth: true

            text: "Отчёты о падениях"
            color: AppTheme.textPrimary
            font.pixelSize: 18
            font.bold: true
        }

        Label {
            Layout.fillWidth: true
            Layout.preferredHeight: root.startupSummary.length > 0 ? implicitHeight : 0

            visible: root.startupSummary.length > 0

            text: root.startupSummary
            wrapMode: Text.WordWrap
            color: AppTheme.error
            font.pixelSize: 12
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: 12

            // Слева — что сохранено: причина и время каждого отчёта.
            Rectangle {
                Layout.preferredWidth: 250
                Layout.fillHeight: true

                visible: root.titles.length > 0

                color: AppTheme.panelSecondary
                radius: 8
                border.width: 1
                border.color: AppTheme.borderSubtle

                ListView {
                    id: reportList

                    anchors.fill: parent
                    anchors.margins: 4

                    clip: true
                    model: root.titles
                    boundsBehavior: Flickable.StopAtBounds

                    currentIndex: root.available ? root.reporter.selectedIndex : -1

                    onCurrentIndexChanged: {
                        if (root.available) {
                            root.reporter.selectReport(currentIndex)
                        }
                    }

                    delegate: Rectangle {
                        id: reportRow

                        required property int index
                        required property string modelData

                        width: reportList.width
                        height: 40
                        radius: 6

                        color: reportList.currentIndex === reportRow.index
                            ? AppTheme.panelActive
                            : reportRowArea.containsMouse
                                ? AppTheme.panelHover
                                : "transparent"

                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8

                            text: reportRow.modelData
                            color: reportList.currentIndex === reportRow.index
                                ? AppTheme.textPrimary
                                : AppTheme.textSecondary
                            font.pixelSize: 11
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }

                        MouseArea {
                            id: reportRowArea

                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor

                            onClicked: reportList.currentIndex = reportRow.index
                        }
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                Layout.fillHeight: true

                visible: root.titles.length === 0

                text: "Отчётов нет — приложение ещё не падало"
                color: AppTheme.textMuted
                font.pixelSize: 13
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                visible: root.titles.length > 0

                clip: true

                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                TextArea {
                    text: root.reportText
                    readOnly: true
                    selectByMouse: true
                    wrapMode: TextArea.Wrap

                    color: AppTheme.textSecondary
                    font.pixelSize: 11

                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }
        }

        Label {
            Layout.fillWidth: true

            text: root.status
            visible: text.length > 0
            wrapMode: Text.WordWrap
            color: AppTheme.textMuted
            font.pixelSize: 11
        }

        RowLayout {
            Layout.fillWidth: true

            spacing: 8

            Button {
                text: "Скопировать"

                enabled: root.titles.length > 0

                onClicked: {
                    if (root.available) {
                        root.reporter.copySelected()
                    }
                }
            }

            Button {
                text: "Отправить в трекер"
                enabled: root.titles.length > 0

                onClicked: {
                    if (root.available) {
                        root.reporter.sendSelectedToTracker()
                    }
                }
            }

            Button {
                text: "Открыть папку"

                onClicked: {
                    if (root.available) {
                        root.reporter.openReportsFolder()
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Удалить"
                enabled: root.titles.length > 0

                onClicked: {
                    if (root.available) {
                        root.reporter.removeSelected()
                    }
                }
            }

            Button {
                text: "Очистить всё"
                enabled: root.titles.length > 0

                onClicked: {
                    if (root.available) {
                        root.reporter.clearReports()
                    }
                }
            }

            Button {
                text: "Закрыть"

                onClicked: root.close()
            }
        }
    }
}
