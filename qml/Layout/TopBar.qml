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
    signal crashReportsRequested()
    signal settingsRequested()

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

    // Отчёты о падениях: сколько их накопилось, видно сразу.
    readonly property var crashReporter:
        root.controller !== null && root.controller !== undefined &&
        root.controller.crashReporter !== undefined
            ? root.controller.crashReporter
            : null

    readonly property string crashReportLabel:
        root.crashReporter !== null && root.crashReporter.reportCount > 0
            ? "Отчёты о падениях: " + root.crashReporter.reportCount
            : "Отчёты о падениях"

    // Настройки воспроизведения: сейчас это перекрёстное затухание.
    readonly property var playbackSettings:
        root.controller !== null && root.controller !== undefined &&
        root.controller.playbackSettingsController !== undefined
            ? root.controller.playbackSettingsController
            : null

    readonly property int crossfadeSeconds:
        root.playbackSettings !== null ? root.playbackSettings.crossfadeSeconds : 0

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

onClicked: root.settingsRequested()
            }
        }
    }
}