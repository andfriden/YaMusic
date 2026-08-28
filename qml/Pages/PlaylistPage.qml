import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    readonly property int pageHeight: 700


    width:
        parent
            ? parent.width
            : 0

    height:
        pageHeight


    implicitWidth:
        width

    implicitHeight:
        pageHeight


    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    Column {
        anchors.fill:
            parent

        anchors.margins:
            20

        spacing:
            20


        // =========================================================
        // Header
        // =========================================================

        Column {
            width:
                parent.width

            spacing:
                6


            Label {
                width:
                    parent.width

                text:
                        root.controller !== null &&
                    root.controller !== undefined &&
                    root.controller.currentPlaylistTitle.length > 0
                    ? root.controller.currentPlaylistTitle
                    : "Плейлист"

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    24

                font.bold:
                    true

                elide:
                    Text.ElideRight
            }


            Label {
                width:
                    parent.width

                text:
                        root.controller !== null &&
                    root.controller !== undefined &&
                    root.controller.currentPlaylistTrackCount > 0
                    ? qsTr("%1 треков")
                        .arg(
                        root.controller.currentPlaylistTrackCount
                    )
                    : ""

                color:
                    AppTheme.textSecondary

                font.pixelSize:
                    13
            }
        }


        // =========================================================
        // Loading
        // =========================================================

        Label {
            width:
                parent.width

            text:
                "Загрузка плейлиста..."

            color:
                AppTheme.textSecondary

            font.pixelSize:
                14

            visible:
                root.controller !== null &&
                root.controller !== undefined &&
                root.controller.loadingPlaylist
        }


        // =========================================================
        // Playlist content
        // =========================================================

        PlaylistView {
            id: playlistView

            width:
                parent.width

            height:
                Math.max(
                    0,
                    parent.height
                    - 90
                )

            controller:
                root.controller

            visible:
                root.controller !== null &&
                root.controller !== undefined &&
                !root.controller.loadingPlaylist
        }
    }
}