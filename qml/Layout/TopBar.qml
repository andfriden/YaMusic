import QtQuick
import QtQuick.Controls.Basic


Item {
    id: root


    // =============================================================
    // Properties
    // =============================================================

    property string currentSection:
        "home"


    // =============================================================
    // Signals
    // =============================================================

    signal sectionSelected(
        string section
    )


    // =============================================================
    // Layout
    // =============================================================

    readonly property int barHeight:
        82

    implicitHeight:
        root.barHeight


    // =============================================================
    // Background
    // =============================================================

    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    // =============================================================
    // Content
    // =============================================================

    Row {
        anchors.fill:
            parent

        anchors.leftMargin:
            24

        anchors.rightMargin:
            24

        spacing:
            10


        // =========================================================
        // Brand
        // =========================================================

        Item {
            id: brand

            width:
                150

            height:
                parent.height


            Rectangle {
                anchors.left:
                    parent.left

                anchors.verticalCenter:
                    parent.verticalCenter

                width:
                    brandText.implicitWidth + 20

                height:
                    42

                radius:
                    12

                color:
                    brandMouseArea.containsMouse
                        ? AppTheme.panelHover
                        : "transparent"
            }


            Text {
                id: brandText

                anchors.left:
                    parent.left

                anchors.leftMargin:
                    10

                anchors.verticalCenter:
                    parent.verticalCenter

                text:
                    "YaMusic"

                color:
                    brandMouseArea.containsMouse
                        ? AppTheme.accent
                        : AppTheme.textPrimary

                font.pixelSize:
                    24

                font.bold:
                    true
            }


            MouseArea {
                id: brandMouseArea

                anchors.fill:
                    parent

                hoverEnabled:
                    true

                cursorShape:
                    Qt.PointingHandCursor

                onClicked: {
                    root.sectionSelected(
                        "home"
                    )
                }
            }
        }

        // =========================================================
        // Navigation
        // =========================================================

        Repeater {
            model: [
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
                    navigationText.implicitWidth +
                    32

                height:
                    parent.height


                readonly property bool active:
                    root.currentSection ===
                    modelData.section


                Rectangle {
                    anchors.left:
                        parent.left

                    anchors.right:
                        parent.right

                    anchors.verticalCenter:
                        parent.verticalCenter

                    height:
                        42

                    radius:
                        12

                    color:
                        navigationItem.active
                            ? AppTheme.panelActive
                            : (
                                navigationMouseArea.containsMouse
                                    ? AppTheme.panelHover
                                    : "transparent"
                            )
                }


                Rectangle {
                    anchors.left:
                        parent.left

                    anchors.right:
                        parent.right

                    anchors.bottom:
                        parent.bottom

                    height:
                        3

                    radius:
                        2

                    color:
                        navigationItem.active
                            ? AppTheme.accent
                            : "transparent"
                }


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

                    font.pixelSize:
                        15

                    font.weight:
                        navigationItem.active
                            ? Font.DemiBold
                            : Font.Normal
                }


                MouseArea {
                    id: navigationMouseArea

                    anchors.fill:
                        parent

                    hoverEnabled:
                        true

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