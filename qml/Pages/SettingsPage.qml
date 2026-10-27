import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YaMusic 1.0

Item {
    id: root

    property var controller
    property var authController

    signal crashReportsRequested()

    // Страница занимает всю высоту области просмотра, а не свою контентную.
    readonly property bool fillHeight: true

    width: parent ? parent.width : 0
    height: parent ? parent.height : 700
    implicitWidth: width
    implicitHeight: height

    readonly property var updateController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.updateController !== undefined
            ? root.controller.updateController
            : null

    readonly property bool autoUpdateCheck:
        root.updateController !== null && root.updateController.autoCheckEnabled

    readonly property string updateBadge:
        root.updateController !== null && root.updateController.updateAvailable
            ? "Доступна версия " + root.updateController.latestVersion
            : ""

    readonly property var preloadController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.preloadController !== undefined
            ? root.controller.preloadController
            : null

    readonly property bool preloadEnabled:
        root.preloadController !== null && root.preloadController.enabled

    readonly property string cacheUsage:
        root.preloadController !== null
            ? root.preloadController.usedMb.toFixed(0) + " МБ из " +
              root.preloadController.limitMb + " МБ"
            : ""

    readonly property string preloadReady:
        root.preloadController !== null && root.preloadController.windowCount > 0
            ? "Готово " + root.preloadController.readyCount + " из " +
              root.preloadController.windowCount
            : ""

    readonly property var crashReporter:
        root.controller !== null && root.controller !== undefined &&
        root.controller.crashReporter !== undefined
            ? root.controller.crashReporter
            : null

    readonly property string crashReportLabel:
        root.crashReporter !== null && root.crashReporter.reportCount > 0
            ? "Отчёты о падениях: " + root.crashReporter.reportCount
            : "Отчёты о падениях"

    readonly property var playbackSettings:
        root.controller !== null && root.controller !== undefined &&
        root.controller.playbackSettingsController !== undefined
            ? root.controller.playbackSettingsController
            : null

    readonly property int crossfadeSeconds:
        root.playbackSettings !== null ? root.playbackSettings.crossfadeSeconds : 0

    Flickable {
        id: pageFlickable

        anchors.fill: parent

        clip: true
        boundsBehavior: Flickable.StopAtBounds

        contentWidth: width
        contentHeight: settingsColumn.implicitHeight + 40

        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

        Column {
            id: settingsColumn

            width: Math.min(760, pageFlickable.width - 48)
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.top: parent.top
            anchors.topMargin: 8

            spacing: 8

            Text {
                width: parent.width
                text: "Настройки"
                color: AppTheme.textPrimary
                font.pixelSize: 24
                font.bold: true
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Row {
                width: parent.width
                height: 30
                spacing: 8

                Text {
                    text: "Тёмная тема"
                    color: AppTheme.textPrimary
                    font.pixelSize: 13

                    anchors.verticalCenter: parent.verticalCenter
                }

                Item {
                    width: 20
                    height: 20

                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        anchors.fill: parent
                        radius: 4

                        color: AppTheme.dark ? AppTheme.accent : AppTheme.panelSecondary

                        border.width: 1
                        border.color: AppTheme.border

                        Text {
                            anchors.centerIn: parent

                            text: AppTheme.dark ? "✓" : ""
                            color: AppTheme.accentOn
                            font.pixelSize: 12
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (root.controller) {
                                    root.controller.themeController.setDarkTheme(!AppTheme.dark)
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Text {
                width: parent.width
                text: "Качество аудио"
                color: AppTheme.textSecondary
                font.pixelSize: 11
                font.bold: true

                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: 6
            }

            Repeater {
                model: [
                    { key: "low", title: "Низкое" },
                    { key: "normal", title: "Среднее" },
                    { key: "high", title: "Высокое" },
                    { key: "lossless", title: "Lossless" }
                ]

                delegate: Row {
                    id: qualityRow

                    required property var modelData

                    width: parent.width
                    height: 24
                    spacing: 8

                    function isCurrent() {
                        return root.controller &&
                            root.controller.audioQualityController &&
                            root.controller.audioQualityController.quality === modelData.key
                    }

                    Text {
                        width: parent.width - 28
                        height: parent.height

                        text: modelData.title
                        color: AppTheme.textPrimary
                        font.pixelSize: 12

                        verticalAlignment: Text.AlignVCenter
                    }

                    Item {
                        width: 20
                        height: 20

                        anchors.verticalCenter: parent.verticalCenter

                        Rectangle {
                            anchors.fill: parent
                            radius: width / 2

                            color: qualityRow.isCurrent()
                                ? AppTheme.accent
                                : AppTheme.panelSecondary

                            border.width: 1
                            border.color: AppTheme.border

                            Text {
                                anchors.centerIn: parent

                                text: qualityRow.isCurrent() ? "✓" : ""
                                color: AppTheme.accentOn
                                font.pixelSize: 11
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (root.controller &&
                                    root.controller.audioQualityController) {
                                    root.controller.audioQualityController.setQuality(
                                        modelData.key)
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Row {
                width: parent.width
                height: 26
                spacing: 8

                Text {
                    width: parent.width - 28
                    height: parent.height

                    text: "Треки без цензуры"
                    color: AppTheme.textPrimary
                    font.pixelSize: 12

                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Item {
                    width: 20
                    height: 20

                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        anchors.fill: parent
                        radius: 4

                        color: root.controller &&
                            root.controller.audioQualityController &&
                            root.controller.audioQualityController.censorBypass
                            ? AppTheme.accent
                            : AppTheme.panelSecondary

                        border.width: 1
                        border.color: AppTheme.border

                        Text {
                            anchors.centerIn: parent

                            text: root.controller &&
                                root.controller.audioQualityController &&
                                root.controller.audioQualityController.censorBypass
                                ? "✓"
                                : ""
                            color: AppTheme.accentOn
                            font.pixelSize: 11
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller &&
                                root.controller.audioQualityController) {
                                root.controller.audioQualityController.setCensorBypass(
                                    !root.controller.audioQualityController.censorBypass)
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Row {
                width: parent.width
                height: 26
                spacing: 8

                Text {
                    width: parent.width - 28
                    height: parent.height

                    text: "Скрывать AI-треки"
                    color: AppTheme.textPrimary
                    font.pixelSize: 12

                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Item {
                    width: 20
                    height: 20

                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        anchors.fill: parent
                        radius: 4

                        color: root.controller && root.controller.hideAiTracks
                            ? AppTheme.accent
                            : AppTheme.panelSecondary

                        border.width: 1
                        border.color: AppTheme.border

                        Text {
                            anchors.centerIn: parent

                            text: root.controller && root.controller.hideAiTracks
                                ? "✓"
                                : ""
                            color: AppTheme.accentOn
                            font.pixelSize: 11
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller) {
                                root.controller.hideAiTracks = !root.controller.hideAiTracks
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Row {
                width: parent.width
                height: 26
                spacing: 8

                Text {
                    width: parent.width - 28
                    height: parent.height

                    text: "Автодизлайк AI-треков"
                    color: AppTheme.textPrimary
                    font.pixelSize: 12

                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Item {
                    width: 20
                    height: 20

                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        anchors.fill: parent
                        radius: 4

                        color: root.controller && root.controller.autoDislikeAiTracks
                            ? AppTheme.accent
                            : AppTheme.panelSecondary

                        border.width: 1
                        border.color: AppTheme.border

                        Text {
                            anchors.centerIn: parent

                            text: root.controller && root.controller.autoDislikeAiTracks
                                ? "✓"
                                : ""
                            color: AppTheme.accentOn
                            font.pixelSize: 11
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller) {
                                root.controller.autoDislikeAiTracks =
                                    !root.controller.autoDislikeAiTracks
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Row {
                width: parent.width
                height: 26
                spacing: 8

                Text {
                    width: parent.width - 28
                    height: parent.height

                    text: "Пропускать AI-треки"
                    color: AppTheme.textPrimary
                    font.pixelSize: 12

                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Item {
                    width: 20
                    height: 20

                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        anchors.fill: parent
                        radius: 4

                        color: root.controller && root.controller.skipAiTracks
                            ? AppTheme.accent
                            : AppTheme.panelSecondary

                        border.width: 1
                        border.color: AppTheme.border

                        Text {
                            anchors.centerIn: parent

                            text: root.controller && root.controller.skipAiTracks
                                ? "✓"
                                : ""
                            color: AppTheme.accentOn
                            font.pixelSize: 11
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller) {
                                root.controller.skipAiTracks =
                                    !root.controller.skipAiTracks
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Text {
                width: parent.width
                text: "Обновления"
                color: AppTheme.textSecondary
                font.pixelSize: 11
                font.bold: true

                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: 6
            }

            Row {
                width: parent.width
                height: 24
                spacing: 8

                Text {
                    width: parent.width - 28
                    height: parent.height

                    text: "Проверять при запуске"
                    color: AppTheme.textPrimary
                    font.pixelSize: 12

                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Item {
                    width: 20
                    height: 20

                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        anchors.fill: parent
                        radius: 4

                        color: root.autoUpdateCheck ? AppTheme.accent : AppTheme.panelSecondary

                        border.width: 1
                        border.color: AppTheme.border

                        Text {
                            anchors.centerIn: parent

                            text: root.autoUpdateCheck ? "✓" : ""
                            color: AppTheme.accentOn
                            font.pixelSize: 11
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.updateController) {
                                root.updateController.autoCheckEnabled = !root.autoUpdateCheck
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 34
                radius: 8

                color: updateArea.containsMouse ? AppTheme.panelActive : AppTheme.panelSecondary

                border.width: 1
                border.color: AppTheme.borderSubtle

                Text {
                    anchors.centerIn: parent

                    text: root.updateBadge.length > 0
                        ? root.updateBadge
                        : "Проверить обновления"
                    color: updateArea.containsMouse ? AppTheme.accent : AppTheme.textPrimary
                    font.pixelSize: 13
                    font.weight: Font.Medium
                }

                MouseArea {
                    id: updateArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        if (!root.updateController) {
                            return
                        }

                        root.updateController.checkForUpdates()
                        updateDialog.open()
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Text {
                width: parent.width
                text: "Перекрёстное затухание"
                color: AppTheme.textSecondary
                font.pixelSize: 11
                font.bold: true

                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: 6
            }

            Row {
                width: parent.width
                height: 26
                spacing: 8

                Text {
                    width: parent.width - 84
                    height: parent.height

                    text: root.crossfadeSeconds > 0
                        ? root.crossfadeSeconds + " с"
                        : "Выключено"
                    color: AppTheme.textPrimary
                    font.pixelSize: 12

                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Slider {
                    width: 72
                    height: parent.height

                    anchors.verticalCenter: parent.verticalCenter

                    from: 0
                    to: 12
                    stepSize: 1
                    value: root.crossfadeSeconds

                    onMoved: {
                        if (root.playbackSettings) {
                            root.playbackSettings.crossfadeSeconds = Math.round(value)
                        }
                    }
                }
            }

            Text {
                width: parent.width

                text: "Следующий трек заводится до конца текущего: поток запрашивается заранее, играет с диска или из сети"
                color: AppTheme.textMuted
                font.pixelSize: 10
                wrapMode: Text.Wrap
                maximumLineCount: 2
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Text {
                width: parent.width
                text: "Предзагрузка"
                color: AppTheme.textSecondary
                font.pixelSize: 11
                font.bold: true

                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: 6
            }

            Row {
                width: parent.width
                height: 24
                spacing: 8

                Text {
                    width: parent.width - 28
                    height: parent.height

                    text: "Скачивать треки заранее"
                    color: AppTheme.textPrimary
                    font.pixelSize: 12

                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Item {
                    width: 20
                    height: 20

                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        anchors.fill: parent
                        radius: 4

                        color: root.preloadEnabled ? AppTheme.accent : AppTheme.panelSecondary

                        border.width: 1
                        border.color: AppTheme.border

                        Text {
                            anchors.centerIn: parent

                            text: root.preloadEnabled ? "✓" : ""
                            color: AppTheme.accentOn
                            font.pixelSize: 11
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.preloadController) {
                                root.preloadController.enabled = !root.preloadEnabled
                            }
                        }
                    }
                }
            }

            Text {
                width: parent.width

                text: root.preloadEnabled
                    ? [root.preloadReady, root.cacheUsage].filter(function(part) {
                        return part.length > 0
                    }).join(" · ")
                    : "Кэш скачанных треков не пополняется"

                color: AppTheme.textMuted
                font.pixelSize: 11
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                maximumLineCount: 2
            }

            Rectangle {
                width: parent.width
                height: 30
                radius: 8

                visible: root.preloadController !== null && root.preloadController.usedMb > 0

                color: clearCacheArea.containsMouse ? AppTheme.panelActive : AppTheme.panelSecondary

                border.width: 1
                border.color: AppTheme.borderSubtle

                Text {
                    anchors.centerIn: parent

                    text: "Очистить кэш"
                    color: clearCacheArea.containsMouse ? AppTheme.accent : AppTheme.textPrimary
                    font.pixelSize: 12
                }

                MouseArea {
                    id: clearCacheArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        if (root.preloadController) {
                            root.preloadController.clearCache()
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: AppTheme.borderSubtle
            }

            Rectangle {
                width: parent.width
                height: 30
                radius: 8

                color: crashArea.containsMouse ? AppTheme.panelActive : AppTheme.panelSecondary

                border.width: 1
                border.color: AppTheme.borderSubtle

                Text {
                    anchors.centerIn: parent

                    text: root.crashReportLabel
                    color: crashArea.containsMouse ? AppTheme.accent : AppTheme.textPrimary
                    font.pixelSize: 12
                }

                MouseArea {
                    id: crashArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        root.crashReportsRequested()
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 34
                radius: 8

                color: logoutArea.containsMouse ? AppTheme.panelActive : AppTheme.panelSecondary

                border.width: 1
                border.color: AppTheme.borderSubtle

                Text {
                    anchors.centerIn: parent

                    text: "Выйти из аккаунта"
                    color: logoutArea.containsMouse ? AppTheme.error : AppTheme.textPrimary
                    font.pixelSize: 13
                    font.weight: Font.Medium
                }

                MouseArea {
                    id: logoutArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        if (root.authController) {
                            root.authController.logout()
                        }
                    }
                }
            }
        }
    }

    UpdateDialog {
        id: updateDialog

        controller: root.controller

        visible: false
    }
}