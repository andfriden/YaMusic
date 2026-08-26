import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    signal sectionSelected(string section)

    property string currentSection: "home"

    Rectangle {
        anchors.fill: parent

        color: AppTheme.backgroundSecondary

        border.width: 1
        border.color: AppTheme.borderSubtle

        // Header + navigation
        Column {
            id: navigation

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            anchors.margins: 12

            spacing: 6

            Label {
                width: parent.width

                leftPadding: 10

                text: "YaMusic"

                color: AppTheme.textPrimary

                font.pixelSize: 22
                font.bold: true

                bottomPadding: 12
            }

            Repeater {
                model: [
                    {
                        id: "home",
                        title: "Главная"
                    },
                    {
                        id: "search",
                        title: "Поиск"
                    },
                    {
                        id: "wave",
                        title: "Моя волна"
                    },
                    {
                        id: "library",
                        title: "Медиатека"
                    },
                    {
                        id: "albums",
                        title: "Альбомы"
                    },
                    {
                        id: "artists",
                        title: "Исполнители"
                    },
                    {
                        id: "playlists",
                        title: "Плейлисты"
                    },
                    {
                        id: "liked",
                        title: "Мне нравится"
                    },
                    {
                        id: "recent",
                        title: "Недавно слушали"
                    }
                ]

                delegate: Rectangle {
                    width: navigation.width
                    height: 40

                    radius: 8

                    color:
                            root.currentSection === modelData.id
                        ? AppTheme.panelActive
                        : mouseArea.containsMouse
                            ? AppTheme.panelHover
                            : "transparent"

                    border.width:
                            root.currentSection === modelData.id
                        ? 1
                        : 0

                    border.color: AppTheme.borderSubtle

                    MouseArea {
                        id: mouseArea

                        anchors.fill: parent

                        hoverEnabled: true

                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            root.currentSection = modelData.id
                            root.sectionSelected(modelData.id)
                        }
                    }

                    Label {
                        anchors.fill: parent

                        leftPadding: 12
                        rightPadding: 12

                        text: modelData.title

                        color:
                                root.currentSection === modelData.id
                            ? AppTheme.textPrimary
                            : AppTheme.textSecondary

                        font.pixelSize: 13

                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        // Footer
        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            anchors.leftMargin: 12
            anchors.rightMargin: 12
            anchors.bottomMargin: 12

            spacing: 0

            Rectangle {
                width: parent.width
                height: 1

                color: AppTheme.divider
            }

            Label {
                width: parent.width

                leftPadding: 10
                topPadding: 8

                text: "YaMusic"

                color: AppTheme.textMuted

                font.pixelSize: 10
            }
        }
    }
}