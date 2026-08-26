import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller


    /*
     * ============================================================
     * Main panel
     * ============================================================
     */

    Rectangle {
        anchors.fill: parent

        radius: 10

        color:
            AppTheme.panel

        border.width:
            1

        border.color:
            AppTheme.borderSubtle


        /*
         * ========================================================
         * Content
         * ========================================================
         */

        Column {
            anchors.fill: parent

            anchors.margins:
                12

            spacing:
                8


            /*
             * ====================================================
             * Header
             * ====================================================
             */

            Row {
                width:
                    parent.width

                spacing:
                    10


                Label {
                    text:
                        qsTr("Недавно слушали")

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        18

                    font.bold:
                        true

                    anchors.verticalCenter:
                        parent.verticalCenter
                }


                Label {
                    text:
                            root.controller !== null &&
                        root.controller.recentListeningModel !== null &&
                        root.controller.recentListeningModel !== undefined &&
                        root.controller.recentListeningModel.count > 0
                        ? qsTr("%1 треков")
                            .arg(
                            root.controller
                                .recentListeningModel
                                .count
                        )
                        : ""

                    color:
                        AppTheme.textSecondary

                    anchors.verticalCenter:
                        parent.verticalCenter
                }
            }


            /*
             * ====================================================
             * Tracks
             * ====================================================
             */

            ListView {
                id: tracksView

                width:
                    parent.width

                height:
                    parent.height - 40

                model:
                        root.controller !== null &&
                    root.controller.recentListeningModel !== null &&
                    root.controller.recentListeningModel !== undefined
                    ? root.controller.recentListeningModel
                    : null

                clip:
                    true

                spacing:
                    6

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


                /*
                 * =================================================
                 * Track delegate
                 * =================================================
                 */

                delegate:
                    Rectangle {
                        id: trackDelegate

                        required property int index
                        required property string trackId
                        required property string title
                        required property string artist
                        required property string artistId
                        required property string coverUri
                        required property string album
                        required property int durationMs


                        width:
                            tracksView.width -
                            (
                                tracksView
                                    .ScrollBar
                                    .vertical
                                    .visible
                                    ? 10
                                    : 0
                            )

                        height:
                            68

                        radius:
                            8


                        /*
                         * -------------------------------------------------
                         * Row background
                         * -------------------------------------------------
                         */

                        color:
                            rowMouseArea.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary

                        border.width:
                            1

                        border.color:
                            rowMouseArea.containsMouse
                                ? AppTheme.border
                                : AppTheme.borderSubtle


                        /*
                         * -------------------------------------------------
                         * Cover
                         * -------------------------------------------------
                         */

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
                                    AppTheme.textSecondary

                                font.pixelSize:
                                    20

                                visible:
                                    cover.status !==
                                    Image.Ready
                            }
                        }


                        /*
                         * -------------------------------------------------
                         * Track information
                         * -------------------------------------------------
                         */

                        Column {
                            id: trackInfo

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


                            /*
                             * Track title
                             */

                            Label {
                                id: titleLabel

                                width:
                                    parent.width

                                text:
                                        trackDelegate.title.length > 0
                                    ? trackDelegate.title
                                    : qsTr("Без названия")

                                color:
                                    AppTheme.textPrimary

                                font.pixelSize:
                                    14

                                font.bold:
                                    true

                                elide:
                                    Text.ElideRight

                                maximumLineCount:
                                    1
                            }


                            /*
                             * Artist
                             *
                             * Invisible layout item is intentionally kept
                             * so album remains on the same vertical position.
                             */

                            Item {
                                id: artistArea

                                width:
                                    parent.width

                                height:
                                    artistLabel.height


                                Label {
                                    id: artistLabel

                                    width:
                                        Math.min(
                                            implicitWidth,
                                            parent.width
                                        )

                                    height:
                                        18

                                    text:
                                            trackDelegate.artist.length > 0
                                        ? trackDelegate.artist
                                        : qsTr(
                                            "Неизвестный исполнитель"
                                        )

                                    color:
                                        artistMouseArea.containsMouse
                                            ? AppTheme.accent
                                            : AppTheme.textSecondary

                                    font.pixelSize:
                                        12

                                    elide:
                                        Text.ElideRight

                                    maximumLineCount:
                                        1
                                }
                            }


                            /*
                             * Album
                             */

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
                         * -------------------------------------------------
                         * Duration
                         * -------------------------------------------------
                         */

                        Label {
                            id: durationLabel

                            width:
                                44

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

                            horizontalAlignment:
                                Text.AlignRight
                        }


                        /*
                         * -------------------------------------------------
                         * Whole row click
                         * -------------------------------------------------
                         */

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
                                if (
                                    root.controller === null ||
                                    root.controller === undefined
                                ) {
                                    return
                                }

                                root.controller.selectRecentListening(
                                    trackDelegate.index
                                )
                            }
                        }


                        /*
                         * -------------------------------------------------
                         * Artist click
                         *
                         * Only the actual artist text is clickable.
                         * -------------------------------------------------
                         */

                        MouseArea {
                            id: artistMouseArea

                            x:
                                trackInfo.x

                            y:
                                trackInfo.y +
                                titleLabel.height +
                                trackInfo.spacing

                            width:
                                artistLabel.width

                            height:
                                artistLabel.height

                            z:
                                10

                            enabled:
                                trackDelegate.artistId.length > 0

                            hoverEnabled:
                                true

                            cursorShape:
                                enabled
                                    ? Qt.PointingHandCursor
                                    : Qt.ArrowCursor

                            onClicked: {
                                if (
                                    root.controller === null ||
                                    root.controller === undefined
                                ) {
                                    return
                                }

                                if (
                                    trackDelegate.artistId.length === 0
                                ) {
                                    return
                                }

                                root.controller.loadArtist(
                                    trackDelegate.artistId
                                )
                            }
                        }
                    }


                /*
                 * =================================================
                 * Empty state
                 * =================================================
                 */

                Item {
                    anchors.fill:
                        parent

                    visible:
                        root.controller !== null &&
                        root.controller.recentListeningModel !== null &&
                        root.controller.recentListeningModel !== undefined &&
                        root.controller.recentListeningModel.count === 0

                    Label {
                        anchors.centerIn:
                            parent

                        text:
                            qsTr(
                                "Недавно слушали пока пусто"
                            )

                        color:
                            AppTheme.textSecondary

                        font.pixelSize:
                            12
                    }
                }
            }
        }
    }


    /*
     * ============================================================
     * Helpers
     * ============================================================
     */

    function formatDuration(milliseconds) {
        var value =
            Number(milliseconds)

        if (
            !isFinite(value) ||
            value <= 0
        ) {
            return "0:00"
        }

        var totalSeconds =
            Math.floor(
                value / 1000
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


    /*
     * ============================================================
     * Diagnostics
     * ============================================================
     */

    Component.onCompleted: {
        console.log(
            "========================================"
        )

        console.log(
            "RecentListeningSection CREATED"
        )

        console.log(
            "controller:",
            root.controller
        )

        console.log(
            "recentListeningModel:",
                root.controller !== null &&
                root.controller.recentListeningModel !== undefined
                ? root.controller.recentListeningModel
                : null
        )

        console.log(
            "track count:",
                root.controller !== null &&
                root.controller.recentListeningModel !== undefined &&
                root.controller.recentListeningModel !== null
                ? root.controller.recentListeningModel.count
                : 0
        )

        console.log(
            "========================================"
        )
    }
}