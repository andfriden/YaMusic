import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    signal chartRequested()
    signal genresRequested()
    signal playlistsRequested()


    width:
        parent
            ? parent.width
            : 0

    height:
        42


    Row {
        anchors.fill:
            parent

        spacing:
            12


        // =========================================================
        // Charts
        // =========================================================

        Button {
            width:
                (parent.width - 2 * parent.spacing) / 3

            height:
                parent.height

            text:
                qsTr("Чарты")


            background: Rectangle {
                radius:
                    height / 2

                color:
                    parent.hovered
                        ? AppTheme.panelHover
                        : AppTheme.panelSecondary

                border.width:
                    1

                border.color:
                    AppTheme.borderSubtle
            }


            contentItem: Text {
                text:
                    parent.text

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    13

                font.bold:
                    true

                horizontalAlignment:
                    Text.AlignHCenter

                verticalAlignment:
                    Text.AlignVCenter
            }


            onClicked:
                root.chartRequested()
        }


        // =========================================================
        // Genres
        // =========================================================

        Button {
            width:
                (parent.width - 2 * parent.spacing) / 3

            height:
                parent.height

            text:
                qsTr("Жанры")


            background: Rectangle {
                radius:
                    height / 2

                color:
                    parent.hovered
                        ? AppTheme.panelHover
                        : AppTheme.panelSecondary

                border.width:
                    1

                border.color:
                    AppTheme.borderSubtle
            }


            contentItem: Text {
                text:
                    parent.text

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    13

                font.bold:
                    true

                horizontalAlignment:
                    Text.AlignHCenter

                verticalAlignment:
                    Text.AlignVCenter
            }


            onClicked:
                root.genresRequested()
        }


        // =========================================================
        // Playlists
        // =========================================================

        Button {
            width:
                (parent.width - 2 * parent.spacing) / 3

            height:
                parent.height

            text:
                qsTr("Плейлисты")


            background: Rectangle {
                radius:
                    height / 2

                color:
                    parent.hovered
                        ? AppTheme.panelHover
                        : AppTheme.panelSecondary

                border.width:
                    1

                border.color:
                    AppTheme.borderSubtle
            }


            contentItem: Text {
                text:
                    parent.text

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    13

                font.bold:
                    true

                horizontalAlignment:
                    Text.AlignHCenter

                verticalAlignment:
                    Text.AlignVCenter
            }


            onClicked:
                root.playlistsRequested()
        }
    }
}