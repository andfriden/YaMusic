import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller


    Column {

        anchors.fill: parent

        spacing: 8



        Label {

            text:
                qsTr("Результаты поиска")


            color:
                AppTheme.textPrimary


            font.pixelSize:
                18


            font.bold:
                true
        }



        ListView {

            id: resultsView


            width:
                parent.width


            height:
                parent.height - 30


            model:
                    root.controller !== null
                ? root.controller.searchModel
                : null


            clip:
                true


            spacing:
                6



            ScrollBar.vertical:

                ScrollBar {
                    policy:
                        ScrollBar.AsNeeded
                }



            delegate:

                Rectangle {

                    id: resultItem



                    required property int index

                    required property string trackId

                    required property string title

                    required property string artist

                    required property string artistId

                    required property string album

                    required property string albumId

                    required property string coverUri

                    required property int durationMs



                    width:

                        resultsView.width -
                        (
                            resultsView.ScrollBar.vertical.visible
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


                        cursorShape:
                            Qt.PointingHandCursor


                        z:
                            0



                        onClicked: {

                            root.controller.selectSearchResult(
                                resultItem.index
                            )
                        }
                    }




                    Image {

                        id: cover


                        anchors.left:
                            parent.left


                        anchors.leftMargin:
                            8


                        anchors.verticalCenter:
                            parent.verticalCenter


                        width:
                            52


                        height:
                            52



                        source:

                                resultItem.coverUri.length > 0
                            ? "image://yandex/" +
                            resultItem.coverUri
                            : ""



                        fillMode:
                            Image.PreserveAspectCrop


                        asynchronous:
                            true


                        cache:
                            true



                        Rectangle {

                            anchors.fill:
                                parent


                            radius:
                                6


                            color:
                                AppTheme.surface


                            visible:
                                cover.status !== Image.Ready



                            Label {

                                anchors.centerIn:
                                    parent


                                text:
                                    "♪"


                                color:
                                    AppTheme.textSecondary


                                font.pixelSize:
                                    20
                            }
                        }
                    }





                    Column {

                        id: trackInfo


                        anchors.left:
                            cover.right


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

                            width:
                                parent.width


                            text:
                                    resultItem.title.length > 0
                                ? resultItem.title
                                : qsTr("Без названия")


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
                                artistLabel.width


                            height:
                                artistLabel.height



                            Label {

                                id: artistLabel


                                width:
                                    Math.min(
                                        implicitWidth,
                                        trackInfo.width
                                    )


                                height:
                                    18


                                text:
                                    resultItem.artist


                                color:

                                    artistMouseArea.containsMouse
                                        ? AppTheme.accent
                                        : AppTheme.textSecondary


                                font.pixelSize:
                                    12


                                elide:
                                    Text.ElideRight
                            }



                            MouseArea {

                                id: artistMouseArea

                                width:
                                    artistLabel.width

                                height:
                                    artistLabel.height

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





                        Item {

                            id: albumArea


                            width:
                                albumLabel.width


                            height:
                                albumLabel.height



                            Label {

                                id: albumLabel


                                width:
                                    Math.min(
                                        implicitWidth,
                                        trackInfo.width
                                    )


                                height:
                                    16


                                text:
                                    resultItem.album


                                color:

                                    albumMouseArea.containsMouse
                                        ? AppTheme.accent
                                        : AppTheme.textMuted


                                font.pixelSize:
                                    10


                                elide:
                                    Text.ElideRight
                            }



                            MouseArea {

                                id: albumMouseArea

                                width:
                                    albumLabel.width

                                height:
                                    albumLabel.height


                                hoverEnabled:
                                    true


                                enabled:

                                    resultItem.albumId.length > 0


                                cursorShape:

                                    enabled
                                        ? Qt.PointingHandCursor
                                        : Qt.ArrowCursor


                                z:
                                    10



                                onClicked: {

                                    root.controller.loadAlbum(
                                        resultItem.albumId
                                    )
                                }
                            }
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
                                resultItem.durationMs
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

                        root.controller &&
                    root.controller.searching
                    ? qsTr("Поиск...")
                    : qsTr("Ничего не найдено")


                color:
                    AppTheme.textSecondary


                visible:

                    root.controller !== null &&
                    root.controller.searchModel.count === 0
            }
        }
    }



    function formatDuration(milliseconds)
    {
        if (
            !milliseconds ||
            milliseconds <= 0
        ) {
            return "0:00"
        }


        var totalSeconds =
            Math.floor(
                milliseconds / 1000
            )


        var minutes =
            Math.floor(
                totalSeconds / 60
            )


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