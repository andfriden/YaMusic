import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property string currentSection: "home"
    property bool canGoBack: false

    signal sectionSelected(string section)
    signal backRequested()

    readonly property int barHeight: 68

    implicitHeight:
        root.barHeight

    Rectangle {
        anchors.fill: parent

        color:
            AppTheme.backgroundPrimary
    }

    Row {
        anchors.fill: parent

        anchors.leftMargin: 28
        anchors.rightMargin: 28

        spacing: 8

        // =========================================================
        // Back
        // =========================================================

        Item {
            id: backArea

            width:
                root.canGoBack
                    ? 40
                    : 0

            height:
                parent.height

            visible:
                root.canGoBack

            Item {
                width: 36
                height: 36

                anchors.verticalCenter:
                    parent.verticalCenter

                Rectangle {
                    anchors.fill: parent

                    radius: 10

                    color:
                        backMouseArea.containsMouse
                            ? AppTheme.panelHover
                            : "transparent"
                }

                Text {
                    anchors.centerIn: parent

                    text: "‹"

                    color:
                        backMouseArea.containsMouse
                            ? AppTheme.textPrimary
                            : AppTheme.textSecondary

                    font.pixelSize: 30
                    font.weight: Font.Light

                    verticalAlignment:
                        Text.AlignVCenter
                }

                MouseArea {
                    id: backMouseArea

                    anchors.fill: parent

                    hoverEnabled: true

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        root.backRequested()
                    }
                }
            }
        }

        // =========================================================
        // Brand
        // =========================================================

        Item {
            id: brand

            width: 150
            height: parent.height

            Text {
                id: brandText

                anchors.left:
                    parent.left

                anchors.verticalCenter:
                    parent.verticalCenter

                text: "YaMusic"

                color:
                    brandMouseArea.containsMouse
                        ? AppTheme.accent
                        : AppTheme.textPrimary

                font.pixelSize: 25
                font.weight: Font.Bold

                Behavior on color {
                    ColorAnimation {
                        duration: 120
                    }
                }
            }

            MouseArea {
                id: brandMouseArea

                anchors.fill:
                    parent

                hoverEnabled: true

                cursorShape:
                    Qt.PointingHandCursor

                onClicked: {
                    root.sectionSelected("home")
                }
            }
        }

        // =========================================================
        // Navigation
        // =========================================================

        Repeater {
            model: [
                {
                    title: "Моя волна",
                    section: "wave"
                },
                {
                    title: "Чарты",
                    section: "chart"
                },
                {
                    title: "Жанры",
                    section: "genres"
                },
                {
                    title: "Плейлисты",
                    section: "playlists"
                },
                {
                    title: "Спорт",
                    section: "sport"
                },
                {
                    title: "Мне нравится",
                    section: "liked"
                }
            ]

            delegate: Item {
                id: navigationItem

                required property var modelData

                width:
                    navigationText.implicitWidth + 28

                height:
                    parent.height

                readonly property bool active:
                    root.currentSection ===
                    modelData.section

                // -------------------------------------------------
                // Navigation background
                // -------------------------------------------------

                Rectangle {
                    anchors.left:
                        parent.left

                    anchors.right:
                        parent.right

                    anchors.verticalCenter:
                        parent.verticalCenter

                    height: 38

                    radius: 10

                    color:
                        navigationItem.active
                            ? AppTheme.panelActive
                            : (
                                navigationMouseArea.containsMouse
                                    ? AppTheme.panelHover
                                    : "transparent"
                            )

                    Behavior on color {
                        ColorAnimation {
                            duration: 100
                        }
                    }
                }

                // -------------------------------------------------
                // Active indicator
                // -------------------------------------------------

                Rectangle {
                    anchors.left:
                        parent.left

                    anchors.right:
                        parent.right

                    anchors.bottom:
                        parent.bottom

                    height: 2

                    radius: 1

                    color:
                        navigationItem.active
                            ? AppTheme.accent
                            : "transparent"

                    Behavior on color {
                        ColorAnimation {
                            duration: 120
                        }
                    }
                }

                // -------------------------------------------------
                // Text
                // -------------------------------------------------

                Text {
                    id: navigationText

                    anchors.centerIn:
                        parent

                    text:
                        navigationItem.modelData.title

                    color:
                        navigationItem.active
                            ? AppTheme.textPrimary
                            : AppTheme.textSecondary

                    font.pixelSize: 14

                    font.weight:
                        navigationItem.active
                            ? Font.DemiBold
                            : Font.Normal

                    Behavior on color {
                        ColorAnimation {
                            duration: 100
                        }
                    }
                }

                MouseArea {
                    id: navigationMouseArea

                    anchors.fill:
                        parent

                    hoverEnabled: true

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        root.sectionSelected(
                            navigationItem.modelData.section
                        )
                    }
                }
            }
        }
    }
}