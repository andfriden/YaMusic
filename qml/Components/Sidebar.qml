import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property string currentSection: "home"
    property int sidebarWidth: 220

    signal sectionSelected(string section)

    readonly property int itemHeight: 40

    implicitWidth: root.sidebarWidth
    implicitHeight: column.height

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundSecondary
    }

    Rectangle {
        anchors.top: parent.top
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: AppTheme.divider
    }

    Column {
        id: column

        width: parent.width

        // ---- Бренд ----
        Item {
            width: parent.width
            height: 64

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter

                text: qsTr("YaMusic")
                color: AppTheme.textPrimary
                font.pixelSize: 20
                font.weight: Font.Bold
            }
        }

        // ---- Навигация ----
        Repeater {
            model: [
                { title: qsTr("Главная"), section: "home", glyph: "note" },
                { title: qsTr("Поиск"), section: "search", glyph: "search" },
                { title: qsTr("Моя волна"), section: "wave", glyph: "wave" },
                { title: qsTr("Новые релизы"), section: "releases", glyph: "chart" },
                { title: qsTr("Жанры"), section: "genres", glyph: "genre" },
                { title: qsTr("Станции"), section: "stations", glyph: "station" },
                { title: qsTr("Подкасты"), section: "podcasts", glyph: "podcast" },
                { title: qsTr("Плейлисты"), section: "playlists", glyph: "playlist" },
                { title: qsTr("Спорт"), section: "sport", glyph: "sport" },
                { title: qsTr("Мне нравится"), section: "liked", glyph: "heartFilled" },
                { title: qsTr("Статистика"), section: "stats", glyph: "stats" },
                { title: qsTr("Настройки"), section: "settings", glyph: "settings" }
            ]

            delegate: Item {
                id: navItem

                required property var modelData

                readonly property bool active:
                    root.currentSection === modelData.section

                width: parent.width
                height: root.itemHeight

                Rectangle {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12

                    radius: 10

                    color: navItem.active
                        ? AppTheme.panelActive
                        : navMouse.containsMouse ? AppTheme.panelHover : "transparent"

                    Behavior on color {
                        ColorAnimation { duration: 100 }
                    }
                }

                Row {
                    anchors.left: parent.left
                    anchors.leftMargin: 24
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter

                    spacing: 12

                    Icon {
                        anchors.verticalCenter: parent.verticalCenter
                        glyph: navItem.modelData.glyph
                        color: navItem.active ? AppTheme.textPrimary : AppTheme.textSecondary
                        size: 17
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: navItem.modelData.title
                        color: navItem.active ? AppTheme.textPrimary : AppTheme.textSecondary
                        font.pixelSize: 14
                        font.weight: navItem.active ? Font.DemiBold : Font.Normal

                        elide: Text.ElideRight
                    }
                }

                MouseArea {
                    id: navMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.sectionSelected(navItem.modelData.section)
                }
            }
        }
    }
}