import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root


    // =============================================================
    // Navigation requests
    // =============================================================

    signal chartRequested()
    signal genresRequested()
    signal playlistsRequested()
    signal sportRequested()
    signal myPlaylistsRequested()


    // =============================================================
    // Size
    // =============================================================

    width:
        parent
            ? parent.width
            : 0

    height:
        42


    // =============================================================
    // Buttons
    // =============================================================

    Row {
        id: buttonsRow

        anchors.fill:
            parent

        spacing:
            12


        // =========================================================
        // Charts
        // =========================================================

        Button {
            width:
                (buttonsRow.width -
                    buttonsRow.spacing * 4) / 5

            height:
                buttonsRow.height

            text:
                qsTr("Чарты")

            background:
                Rectangle {
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

            contentItem:
                Text {
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

                    elide:
                        Text.ElideRight
                }

            onClicked:
                root.chartRequested()
        }


        // =========================================================
        // Genres
        // =========================================================

        Button {
            width:
                (buttonsRow.width -
                    buttonsRow.spacing * 4) / 5

            height:
                buttonsRow.height

            text:
                qsTr("Жанры")

            background:
                Rectangle {
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

            contentItem:
                Text {
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

                    elide:
                        Text.ElideRight
                }

            onClicked:
                root.genresRequested()
        }


        // =========================================================
        // Playlists
        // =========================================================

        Button {
            width:
                (buttonsRow.width -
                    buttonsRow.spacing * 4) / 5

            height:
                buttonsRow.height

            text:
                qsTr("Плейлисты")

            background:
                Rectangle {
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

            contentItem:
                Text {
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

                    elide:
                        Text.ElideRight
                }

            onClicked:
                root.playlistsRequested()
        }


        // =========================================================
        // Sport
        // =========================================================

        Button {
            width:
                (buttonsRow.width -
                    buttonsRow.spacing * 4) / 5

            height:
                buttonsRow.height

            text:
                qsTr("Спорт")

            background:
                Rectangle {
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

            contentItem:
                Text {
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

                    elide:
                        Text.ElideRight
                }

            onClicked:
                root.sportRequested()
        }


        // =========================================================
        // My Playlists
        // =========================================================

        Button {
            width:
                (buttonsRow.width -
                    buttonsRow.spacing * 4) / 5

            height:
                buttonsRow.height

            text:
                qsTr("Мои плейлисты")

            background:
                Rectangle {
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

            contentItem:
                Text {
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

                    elide:
                        Text.ElideRight
                }

            onClicked:
                root.myPlaylistsRequested()
        }
    }
}