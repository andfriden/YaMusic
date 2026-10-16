import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YaMusic 1.0

Item {
    id: root

    property var controller
    property var authController
    property string currentSection: "home"
    property bool canGoBack: false

    signal sectionSelected(string section)
    signal backRequested()

    readonly property int barHeight: 68

    // Состояние апдейтера для пункта настроек: контроллер может быть не
    // создан, поэтому все обращения к нему идут через эту проверку.
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

    // Состояние предзагрузки: сколько треков уже готово и сколько места
    // занял кэш.
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

    implicitHeight: root.barHeight

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 28
        anchors.rightMargin: 28

        spacing: 8

        Item {
            id: backArea

            Layout.preferredWidth: root.canGoBack ? 40 : 0
            Layout.preferredHeight: 36

            visible: root.canGoBack

            Rectangle {
                anchors.fill: parent
                radius: 10

                color: backMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
            }

            Text {
                anchors.centerIn: parent

                text: "‹"
                color: backMouseArea.containsMouse ? AppTheme.textPrimary : AppTheme.textSecondary
                font.pixelSize: 30
                font.weight: Font.Light
            }

            MouseArea {
                id: backMouseArea

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: root.backRequested()
            }
        }

        Item {
            Layout.preferredWidth: 170
            Layout.fillHeight: true

            Text {
                id: logoText
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                text: "YaMusic"
                font.pixelSize: 25
                font.weight: Font.Bold

                color: brandMouseArea.containsMouse ? AppTheme.accent : AppTheme.textPrimary

                Behavior on color {
                    ColorAnimation { duration: 120 }
                }
            }

            // Градиентный акцентный штрих под логотипом — появляется
            // с плавной анимацией при наведении.
            Rectangle {
                id: brandUnderline

                anchors.left: logoText.left
                anchors.right: logoText.right
                anchors.top: logoText.bottom
                anchors.topMargin: 2

                height: 3
                radius: 1.5

                gradient: Gradient {
                    GradientStop { position: 0.0; color: AppTheme.accent }
                    GradientStop { position: 1.0; color: AppTheme.mauveAccent }
                }

                opacity: 0.0

                Behavior on opacity {
                    NumberAnimation { duration: 140 }
                }

                states: [
                    State {
                        when: brandMouseArea.containsMouse
                        PropertyChanges { target: brandUnderline; opacity: 1.0 }
                    }
                ]
            }

            MouseArea {
                id: brandMouseArea

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: root.sectionSelected("home")
            }
        }

        Repeater {
            model: [
                { title: "Моя волна", section: "wave" },
                { title: "Чарты", section: "chart" },
                { title: "Жанры", section: "genres" },
                { title: "Станции", section: "stations" },
                { title: "Подкасты", section: "podcasts" },
                { title: "Плейлисты", section: "playlists" },
                { title: "Спорт", section: "sport" },
                { title: "Мне нравится", section: "liked" }
            ]

            delegate: Item {
                id: navigationItem

                required property var modelData

                readonly property bool active:
                    root.currentSection === modelData.section

                Layout.preferredWidth: navigationText.implicitWidth + 28
                Layout.preferredHeight: 38

                Rectangle {
                    anchors.fill: parent
                    radius: 10

                    color: navigationItem.active
                        ? AppTheme.panelActive
                        : navigationMouseArea.containsMouse
                            ? AppTheme.panelHover
                            : "transparent"

                    Behavior on color {
                        ColorAnimation { duration: 100 }
                    }
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: 2
                    radius: 1

                    color: navigationItem.active ? AppTheme.accent : "transparent"

                    Behavior on color {
                        ColorAnimation { duration: 120 }
                    }
                }

                Text {
                    id: navigationText

                    anchors.centerIn: parent

                    text: navigationItem.modelData.title
                    color: navigationItem.active ? AppTheme.textPrimary : AppTheme.textSecondary
                    font.pixelSize: 14
                    font.weight: navigationItem.active ? Font.DemiBold : Font.Normal

                    Behavior on color {
                        ColorAnimation { duration: 100 }
                    }
                }

                MouseArea {
                    id: navigationMouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.sectionSelected(navigationItem.modelData.section)
                }
            }
        }

        // Распорка: прижимает настройки к правому краю
        Item {
            Layout.fillWidth: true
        }

        Item {
            id: settingsButton

            Layout.preferredWidth: 38
            Layout.preferredHeight: 38

            Rectangle {
                anchors.fill: parent
                radius: 10

                color: settingsMouseArea.containsMouse ? AppTheme.panelHover : AppTheme.panelSubtle

                border.width: 1
                border.color: settingsMouseArea.containsMouse ? AppTheme.border : AppTheme.borderSubtle

                Behavior on color {
                    ColorAnimation { duration: 120 }
                }

                Behavior on border.color {
                    ColorAnimation { duration: 120 }
                }
            }

            Text {
                anchors.centerIn: parent

                text: "⚙"
                color: settingsMouseArea.containsMouse ? AppTheme.textPrimary : AppTheme.textSecondary
                font.pixelSize: 16

                Behavior on color {
                    ColorAnimation { duration: 120 }
                }
            }

            MouseArea {
                id: settingsMouseArea

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: settingsPopup.open()
            }
        }
    }

    Popup {
        id: settingsPopup

        width: 260
        height: 600

        x: settingsButton.x + settingsButton.width - width
        y: settingsButton.y + settingsButton.height + 6

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            color: AppTheme.panel
            border.width: 1
            border.color: AppTheme.borderSubtle
            radius: 10
        }

        Column {
            anchors.fill: parent
            anchors.margins: 10

            spacing: 8

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
                anchors.topMargin: 2
            }

            Repeater {
                    model: [
                        { key: "low", title: "Низкое" },
                        { key: "normal", title: "Среднее" },
                        { key: "high", title: "Высокое" },
                        { key: "lossless", title: "Lossless" }
                    ]

                    delegate: Row {
                        required property var modelData

                        width: parent.width
                        height: 24
                        spacing: 8

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

                                color: root.controller &&
                                    root.controller.audioQualityController &&
                                    root.controller.audioQualityController.quality === modelData.key
                                    ? AppTheme.accent
                                    : AppTheme.panelSecondary

                                border.width: 1
                                border.color: AppTheme.border

                                Text {
                                    anchors.centerIn: parent

                                    text: root.controller &&
                                        root.controller.audioQualityController &&
                                        root.controller.audioQualityController.quality === modelData.key
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

            Text {
                width: parent.width
                text: "Обновления"
                color: AppTheme.textSecondary
                font.pixelSize: 11
                font.bold: true

                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: 2
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
                        settingsPopup.close()

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
                text: "Предзагрузка"
                color: AppTheme.textSecondary
                font.pixelSize: 11
                font.bold: true

                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: 2
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

                // Состояние окна и занятое место: пустая строка, когда
                // предзагрузка выключена или очередь кончилась.
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
                        settingsPopup.close()

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
    }
}