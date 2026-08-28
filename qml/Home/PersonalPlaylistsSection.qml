import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    implicitHeight:
        sectionsColumn.implicitHeight


    Column {
        id: sectionsColumn

        width:
            parent.width

        spacing:
            24


        Repeater {
            model:
                    root.controller !== null &&
                root.controller !== undefined
                ? root.controller.personalPlaylistsModel
                : null


            delegate: Column {
                width:
                    sectionsColumn.width

                spacing:
                    10

                visible:
                    playlists !== null &&
                    playlists !== undefined &&
                    playlists.length > 0


                Label {
                    width:
                        parent.width

                    text:
                        title

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        18

                    font.bold:
                        true
                }


                Row {
                    spacing:
                        12


                    Repeater {
                        model:
                            playlists


                        delegate: Rectangle {
                            width:
                                150

                            height:
                                150

                            radius:
                                10

                            color:
                                AppTheme.panelSecondary

                            border.width:
                                1

                            border.color:
                                AppTheme.border


                            Column {
                                anchors.fill:
                                    parent

                                anchors.margins:
                                    8

                                spacing:
                                    6


                                Rectangle {
                                    width:
                                        134

                                    height:
                                        90

                                    radius:
                                        8

                                    color:
                                        AppTheme.panelHover


                                    Image {
                                        anchors.fill:
                                            parent

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
                                    }
                                }


                                Label {
                                    width:
                                        parent.width

                                    text:
                                        modelData.title

                                    color:
                                        AppTheme.textPrimary

                                    font.pixelSize:
                                        13

                                    font.bold:
                                        true

                                    elide:
                                        Text.ElideRight
                                }


                                Label {
                                    text:
                                            modelData.trackCount > 0
                                        ? qsTr("%1 треков")
                                            .arg(
                                            modelData.trackCount
                                        )
                                        : ""

                                    color:
                                        AppTheme.textSecondary

                                    font.pixelSize:
                                        11
                                }
                            }


                            MouseArea {
                                anchors.fill:
                                    parent

                                cursorShape:
                                    Qt.PointingHandCursor


                                onClicked: {
                                    if (
                                        root.controller === null ||
                                        root.controller === undefined
                                    ) {
                                        return
                                    }


                                    root.controller
                                        .selectPersonalPlaylist(
                                        String(modelData.uid),
                                        Number(modelData.kind)
                                    )
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}