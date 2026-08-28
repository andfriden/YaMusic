import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller


    Column {
        id: sectionsColumn

        width: parent.width

        spacing: 24


        Repeater {
            id: sectionsRepeater

            model:
                root.controller
                    ? root.controller.personalPlaylistsModel
                    : null


            delegate: Column {
                id: sectionDelegate

                width:
                    sectionsColumn.width

                spacing: 10


                property var sectionPlaylists:
                    model.playlists


                visible:
                    sectionPlaylists !== undefined &&
                    sectionPlaylists !== null &&
                    sectionPlaylists.length > 0


                Label {
                    width:
                        parent.width

                    text:
                        model.title

                    color:
                        AppTheme.textPrimary

                    font.pixelSize: 20

                    font.bold: true
                }


                ListView {
                    id: playlistList

                    width:
                        parent.width

                    height: 150

                    orientation:
                        ListView.Horizontal

                    spacing: 12

                    clip: true

                    model:
                        sectionDelegate.sectionPlaylists


                    delegate: Rectangle {
                        width: 150
                        height: 150

                        radius: 10

                        color:
                            AppTheme.panelSecondary

                        border.width: 1

                        border.color:
                            AppTheme.border


                        Column {
                            anchors.fill: parent

                            anchors.margins: 8

                            spacing: 6


                            Rectangle {
                                width: 134
                                height: 90

                                radius: 8

                                color:
                                    AppTheme.artworkPlaceholder

                                clip: true


                                Image {
                                    anchors.fill: parent

                                    source:
                                            modelData.coverUri &&
                                        modelData.coverUri.length > 0
                                        ? "image://yandex/" +
                                        modelData.coverUri
                                        : ""

                                    fillMode:
                                        Image.PreserveAspectCrop

                                    asynchronous:
                                        true

                                    cache:
                                        true

                                    smooth:
                                        true
                                }
                            }


                            Label {
                                width:
                                    parent.width

                                text:
                                    modelData.title

                                color:
                                    AppTheme.textPrimary

                                font.pixelSize: 13

                                font.bold: true

                                elide:
                                    Text.ElideRight
                            }


                            Label {
                                width:
                                    parent.width

                                text:
                                    modelData.trackCount +
                                    " треков"

                                color:
                                    AppTheme.textSecondary

                                font.pixelSize: 11
                            }
                        }
                    }
                }
            }
        }
    }
}