import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller


    Rectangle {
        anchors.fill: parent

        radius: 10

        color: AppTheme.panel

        border.width: 1
        border.color: AppTheme.borderSubtle



        Column {

            anchors.fill: parent
            anchors.margins: 12

            spacing: 8



            Row {

                width: parent.width

                spacing: 10


                Label {
                    text:
                        root.controller.currentPlaylistTitle ||
                        qsTr("Плейлист")

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        18

                    font.bold:
                        true
                }


                Label {

                    text:
                            root.controller.currentPlaylistTrackCount > 0
                        ? root.controller.currentPlaylistTrackCount +
                        qsTr(" треков")
                        : ""

                    color:
                        AppTheme.textSecondary
                }
            }



            ListView {

                id: tracksView


                width:
                    parent.width


                height:
                    parent.height - 45


                clip:
                    true


                spacing:
                    6



                model:
                        root.controller !== null
                    ? root.controller.playlistModel
                    : null



                ScrollBar.vertical:
                    ScrollBar {
                        policy:
                            ScrollBar.AsNeeded
                    }



                delegate:
                    Rectangle {

                        id: trackDelegate


                        required property int index
                        required property string trackId
                        required property string title
                        required property string artist
                        required property string artistId
                        required property string album
                        required property string coverUri
                        required property int durationMs



                        width:
                            tracksView.width -
                            (
                                tracksView.ScrollBar.vertical.visible
                                    ? 10
                                    : 0
                            )


                        height:
                            68


                        radius:
                            8


                        color:
                            rowMouseArea.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary



                        border.width:
                            1


                        border.color:
                            AppTheme.borderSubtle



                        MouseArea {

                            id: rowMouseArea

                            anchors.fill:
                                parent


                            hoverEnabled:
                                true


                            z:
                                0


                            onClicked: {

                                root.controller.selectPlaylistTrack(
                                    trackDelegate.index
                                )
                            }
                        }




                        Rectangle {

                            id: coverContainer


                            width:
                                52


                            height:
                                52


                            anchors.left:
                                parent.left


                            anchors.leftMargin:
                                8


                            anchors.verticalCenter:
                                parent.verticalCenter


                            radius:
                                6


                            color:
                                AppTheme.artworkPlaceholder


                            clip:
                                true



                            Image {

                                id: cover


                                anchors.fill:
                                    parent


                                source:
                                        trackDelegate.coverUri.length > 0
                                    ? "image://yandex/" +
                                    trackDelegate.coverUri
                                    : ""


                                fillMode:
                                    Image.PreserveAspectCrop


                                asynchronous:
                                    true


                                cache:
                                    true


                                visible:
                                    status === Image.Ready
                            }



                            Label {

                                anchors.centerIn:
                                    parent


                                text:
                                    "♪"


                                color:
                                    AppTheme.textSecondary


                                visible:
                                    cover.status !== Image.Ready
                            }
                        }





                        Column {

                            id: trackInfo


                            z:
                                5


                            anchors.left:
                                coverContainer.right


                            anchors.leftMargin:
                                12


                            anchors.right:
                                durationLabel.left


                            anchors.rightMargin:
                                12


                            anchors.verticalCenter:
                                parent.verticalCenter


                            spacing:
                                2



                            Label {

                                id: titleLabel


                                width:
                                    parent.width


                                text:
                                    trackDelegate.title


                                color:
                                    AppTheme.textPrimary


                                font.pixelSize:
                                    14


                                font.bold:
                                    true


                                elide:
                                    Text.ElideRight
                            }





                            Item {

                                id: artistArea


                                width:
                                    artistLabel.implicitWidth


                                height:
                                    artistLabel.height



                                Label {

                                    id: artistLabel


                                    text:
                                        trackDelegate.artist


                                    color:
                                        artistMouseArea.containsMouse
                                            ? AppTheme.accent
                                            : AppTheme.textSecondary


                                    font.pixelSize:
                                        12


                                    height:
                                        18
                                }



                                MouseArea {

                                    id: artistMouseArea


                                    anchors.fill:
                                        parent


                                    hoverEnabled:
                                        true


                                    enabled:
                                        trackDelegate.artistId.length > 0


                                    cursorShape:
                                        enabled
                                            ? Qt.PointingHandCursor
                                            : Qt.ArrowCursor


                                    z:
                                        10


                                    onClicked: {

                                        root.controller.loadArtist(
                                            trackDelegate.artistId
                                        )
                                    }
                                }
                            }




                            Label {

                                width:
                                    parent.width


                                text:
                                    trackDelegate.album


                                color:
                                    AppTheme.textMuted


                                font.pixelSize:
                                    10


                                elide:
                                    Text.ElideRight
                            }
                        }





                        Label {

                            id: durationLabel


                            anchors.right:
                                parent.right


                            anchors.rightMargin:
                                14


                            anchors.verticalCenter:
                                parent.verticalCenter


                            text:
                                root.formatDuration(
                                    trackDelegate.durationMs
                                )


                            color:
                                AppTheme.textSecondary


                            font.pixelSize:
                                11
                        }
                    }




                Label {

                    anchors.centerIn:
                        parent


                    text:
                        root.controller.loadingPlaylist
                            ? qsTr("Загрузка плейлиста...")
                            : qsTr("В плейлисте нет треков")


                    color:
                        AppTheme.textSecondary


                    visible:
                        root.controller.loadingPlaylist ||
                        root.controller.playlistModel.count === 0
                }
            }
        }
    }



    function formatDuration(milliseconds) {

        if (!milliseconds || milliseconds <= 0) {
            return "0:00"
        }


        var totalSeconds =
            Math.floor(milliseconds / 1000)


        var minutes =
            Math.floor(totalSeconds / 60)


        var seconds =
            totalSeconds % 60


        return minutes +
            ":" +
            (
                    seconds < 10
                    ? "0"
                    : ""
            ) +
            seconds
    }
}