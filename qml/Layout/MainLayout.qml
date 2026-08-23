import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    property string currentSection: "home"

    property string contextType:
        contextTypeForSection(
            currentSection)

    signal sectionSelected(
        string section)

    default property alias content:
        contentHost.data

    Rectangle {
        anchors.fill: parent

        color: "#f5f5f5"
    }

    Row {
        anchors.fill: parent

        spacing: 0

        Sidebar {
            id: sidebar

            width: 205

            height: parent.height

            currentSection:
                root.currentSection

            onSectionSelected: function(section) {

                root.currentSection =
                    section

                root.sectionSelected(
                    section)
            }
        }

        Rectangle {
            width: 1

            height: parent.height

            color: "#dddddd"
        }

        Item {
            id: mainArea

            width:
                parent.width -
                sidebar.width -
                contextPanel.width -
                1

            height: parent.height

            clip: true

            ScrollView {
                id: contentScrollView

                anchors.fill: parent

                anchors.topMargin: 18

                anchors.leftMargin: 20

                anchors.rightMargin: 20

                anchors.bottomMargin: 158

                clip: true

                ScrollBar.vertical:
                    ScrollBar {
                        policy:
                            ScrollBar.AsNeeded
                    }

                Item {
                    id: contentHost

                    width:
                        contentScrollView.availableWidth

                    implicitHeight:
                        childrenRect.height

                    height:
                        Math.max(
                            implicitHeight,
                            contentScrollView.height)

                    clip: false
                }
            }
        }

        Rectangle {
            width: 1

            height: parent.height

            color: "#dddddd"
        }

        ContextPanel {
            id: contextPanel

            width: 260

            height: parent.height

            anchors.top:
                parent.top

            anchors.bottom:
                parent.bottom

            anchors.topMargin: 14

            anchors.bottomMargin: 158

            contextType:
                root.contextType
        }
    }

    function contextTypeForSection(section) {
        switch (section) {
            case "artists":
                return "artist"

            case "albums":
                return "album"

            case "playlists":
                return "playlist"

            case "library":
                return "library"

            default:
                return "home"
        }
    }
}