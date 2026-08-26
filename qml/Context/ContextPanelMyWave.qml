import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller


    /*
     * ============================================================
     * Random tracks
     * ============================================================
     */

    ListModel {
        id: randomTracksModel
    }


    /*
     * ============================================================
     * Panel
     * ============================================================
     */

    Rectangle {
        anchors.fill:
            parent

        radius:
            10

        color:
            AppTheme.backgroundPrimary

        border.width:
            1

        border.color:
            AppTheme.panelHover


        Column {
            anchors.fill:
                parent

            anchors.margins:
                14

            spacing:
                12


            /*
             * ====================================================
             * Header
             * ====================================================
             */

            Label {
                width:
                    parent.width

                text:
                    qsTr("Недавно слушали")

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    18

                font.bold:
                    true
            }


            /*
             * ====================================================
             * Empty state
             * ====================================================
             */

            Column {
                id: emptyState

                width:
                    parent.width

                height:
                    parent.height - 46

                spacing:
                    8

                anchors.horizontalCenter:
                    parent.horizontalCenter

                visible:
                    randomTracksModel.count === 0


                BusyIndicator {
                    width:
                        28

                    height:
                        28

                    anchors.horizontalCenter:
                        parent.horizontalCenter

                    running:
                        root.controller !== null &&
                        root.controller !== undefined &&
                        root.controller.loadingMyWave
                }


                Label {
                    width:
                        parent.width

                    text:
                            root.controller === null ||
                        root.controller === undefined ||
                        root.controller.recentListeningModel === undefined ||
                        root.controller.recentListeningModel === null
                        ? qsTr(
                            "История прослушивания недоступна"
                        )
                        : qsTr(
                            "Загрузка истории..."
                        )

                    color:
                        AppTheme.textMuted

                    font.pixelSize:
                        12

                    horizontalAlignment:
                        Text.AlignHCenter

                    verticalAlignment:
                        Text.AlignVCenter

                    wrapMode:
                        Text.WordWrap
                }
            }


            /*
             * ====================================================
             * Random tracks
             * ====================================================
             */

            ListView {
                id: tracksView

                width:
                    parent.width

                height:
                    parent.height - 46

                visible:
                    randomTracksModel.count > 0

                clip:
                    true

                spacing:
                    6

                model:
                    randomTracksModel

                boundsBehavior:
                    Flickable.StopAtBounds


                ScrollBar.vertical:
                    ScrollBar {
                        policy:
                                tracksView.contentHeight >
                            tracksView.height
                            ? ScrollBar.AsNeeded
                            : ScrollBar.AlwaysOff
                    }


                delegate:
                    Rectangle {
                        id: trackDelegate

                        required property int sourceIndex
                        required property string trackId
                        required property string title
                        required property string artist
                        required property string artistId
                        required property string album
                        required property string albumId
                        required property string coverUri
                        required property int durationMs

                        width:
                            tracksView.width

                        height:
                            64

                        radius:
                            8

                        color:
                            trackMouse.containsMouse
                                ? AppTheme.panelHover
                                : AppTheme.panelSecondary

                        border.width:
                            1

                        border.color:
                            trackMouse.containsMouse
                                ? AppTheme.border
                                : AppTheme.borderSubtle


                        /*
                         * ------------------------------------------------
                         * Artwork
                         * ------------------------------------------------
                         */

                        Rectangle {
                            id: artwork

                            width:
                                52

                            height:
                                52

                            anchors.left:
                                parent.left

                            anchors.leftMargin:
                                6

                            anchors.verticalCenter:
                                parent.verticalCenter

                            radius:
                                6

                            color:
                                AppTheme.artworkPlaceholder

                            clip:
                                true


                            Image {
                                id: coverImage

                                anchors.fill:
                                    parent

                                source:
                                        trackDelegate.coverUri.length > 0
                                    ? "image://yandex/" +
                                    trackDelegate.coverUri
                                    : ""

                                sourceSize:
                                    Qt.size(
                                        52,
                                        52
                                    )

                                fillMode:
                                    Image.PreserveAspectCrop

                                asynchronous:
                                    true

                                cache:
                                    true

                                smooth:
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
                                    AppTheme.textDisabled

                                font.pixelSize:
                                    20

                                visible:
                                    coverImage.status !==
                                    Image.Ready
                            }
                        }


                        /*
                         * ------------------------------------------------
                         * Track information
                         * ------------------------------------------------
                         */

                        Column {
                            anchors.left:
                                artwork.right

                            anchors.leftMargin:
                                10

                            anchors.right:
                                parent.right

                            anchors.rightMargin:
                                8

                            anchors.verticalCenter:
                                parent.verticalCenter

                            spacing:
                                2


                            Label {
                                width:
                                    parent.width

                                text:
                                        trackDelegate.title.length > 0
                                    ? trackDelegate.title
                                    : qsTr(
                                        "Без названия"
                                    )

                                color:
                                    AppTheme.textPrimary

                                font.pixelSize:
                                    13

                                font.bold:
                                    true

                                elide:
                                    Text.ElideRight

                                maximumLineCount:
                                    1
                            }


                            Label {
                                width:
                                    parent.width

                                text:
                                        trackDelegate.artist.length > 0
                                    ? trackDelegate.artist
                                    : qsTr(
                                        "Неизвестный исполнитель"
                                    )

                                color:
                                    trackMouse.containsMouse
                                        ? AppTheme.accent
                                        : AppTheme.textSecondary

                                font.pixelSize:
                                    11

                                elide:
                                    Text.ElideRight

                                maximumLineCount:
                                    1
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

                                maximumLineCount:
                                    1

                                visible:
                                    text.length > 0
                            }
                        }


                        /*
                         * ------------------------------------------------
                         * Click
                         * ------------------------------------------------
                         */

                        MouseArea {
                            id: trackMouse

                            anchors.fill:
                                parent

                            hoverEnabled:
                                true

                            cursorShape:
                                Qt.PointingHandCursor

                            onClicked: {

                                if (
                                    root.controller === null ||
                                    root.controller === undefined
                                ) {
                                    return
                                }

                                console.log(
                                    "ContextPanelMyWave:"
                                    + " recent track selected",
                                    "| sourceIndex:",
                                    trackDelegate.sourceIndex,
                                    "| title:",
                                    trackDelegate.title,
                                    "| artist:",
                                    trackDelegate.artist
                                )

                                root.controller
                                    .selectRecentListening(
                                    trackDelegate.sourceIndex
                                )
                            }
                        }
                    }
            }
        }
    }


    /*
     * ============================================================
     * Build random tracks
     * ============================================================
     */

    function rebuildRandomTracks() {

        randomTracksModel.clear()


        console.log(
            "========================================"
        )

        console.log(
            "ContextPanelMyWave::rebuildRandomTracks"
        )

        console.log(
            "controller:",
            root.controller
        )


        if (
            root.controller === null ||
            root.controller === undefined
        ) {
            console.log(
                "controller is null"
            )

            console.log(
                "========================================"
            )

            return
        }


        var model =
            root.controller.recentListeningModel


        console.log(
            "controller.recentListeningModel:",
            model
        )


        if (
            model === null ||
            model === undefined
        ) {
            console.log(
                "recentListeningModel is null"
            )

            console.log(
                "========================================"
            )

            return
        }


        /*
         * The model itself does not expose a QML count property.
         *
         * We ask C++ for the random selection directly.
         */

        var items =
            model.randomTrackData(
                10
            )


        console.log(
            "randomTrackData result:",
            items
        )


        if (
            items === null ||
            items === undefined
        ) {
            console.log(
                "randomTrackData returned null"
            )

            console.log(
                "========================================"
            )

            return
        }


        console.log(
            "random item count:",
            items.length
        )


        for (
            var i = 0;
            i < items.length;
            ++i
        ) {

            var item =
                items[i]


            if (
                item === null ||
                item === undefined
            ) {
                continue
            }


            randomTracksModel.append(
                {
                    sourceIndex:
                        Number(
                            item.sourceIndex
                        ),

                    trackId:
                        String(
                            item.trackId ||
                            ""
                        ),

                    title:
                        String(
                            item.title ||
                            ""
                        ),

                    artist:
                        String(
                            item.artist ||
                            ""
                        ),

                    artistId:
                        String(
                            item.artistId ||
                            ""
                        ),

                    album:
                        String(
                            item.album ||
                            ""
                        ),

                    albumId:
                        String(
                            item.albumId ||
                            ""
                        ),

                    coverUri:
                        String(
                            item.coverUri ||
                            ""
                        ),

                    durationMs:
                        Number(
                            item.durationMs ||
                            0
                        )
                }
            )
        }


        console.log(
            "final randomTracksModel.count:",
            randomTracksModel.count
        )

        console.log(
            "========================================"
        )
    }


    /*
     * ============================================================
     * Model signals
     * ============================================================
     */

    Connections {
        target:
                root.controller !== null &&
            root.controller !== undefined
            ? root.controller.recentListeningModel
            : null


        function onModelReset() {

            console.log(
                "ContextPanelMyWave:"
                + " recent modelReset"
            )

            rebuildRandomTracks()
        }


        function onRowsInserted() {

            console.log(
                "ContextPanelMyWave:"
                + " recent rowsInserted"
            )

            rebuildRandomTracks()
        }


        function onRowsRemoved() {

            console.log(
                "ContextPanelMyWave:"
                + " recent rowsRemoved"
            )

            rebuildRandomTracks()
        }
    }


    /*
     * ============================================================
     * Controller changes
     * ============================================================
     */

    onControllerChanged: {

        console.log(
            "ContextPanelMyWave:"
            + " controllerChanged"
        )

        rebuildRandomTracks()
    }


    /*
     * ============================================================
     * Startup
     * ============================================================
     */

    Component.onCompleted: {

        console.log(
            "========================================"
        )

        console.log(
            "ContextPanelMyWave CREATED"
        )

        console.log(
            "controller:",
            root.controller
        )

        rebuildRandomTracks()

        console.log(
            "========================================"
        )
    }
}