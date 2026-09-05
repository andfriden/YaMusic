import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root


    // =============================================================
    // Controller
    // =============================================================

    property var controller


    // =============================================================
    // Subgenre
    // =============================================================

    property string subGenreId:
        ""

    property string subGenreTitle:
        ""

    property string subGenreColor:
        ""


    // =============================================================
    // Genre controller
    // =============================================================

    readonly property var genreController:
            controller && controller.genreController
        ? controller.genreController
        : null


    // =============================================================
    // Navigation
    // =============================================================

    signal backRequested()


    // =============================================================
    // Layout
    // =============================================================

    implicitHeight:
        contentColumn.y +
        contentColumn.height +
        48


    // =============================================================
    // Helpers
    // =============================================================

    function coverSource(uri) {
        if (!uri)
            return ""

        var value =
            String(uri)

        if (
            value.indexOf(
                "image://yandex/"
            ) === 0
        ) {
            return value
        }

        return "image://yandex/" + value
    }


    function formatDuration(ms) {
        if (
            !ms ||
            ms <= 0
        ) {
            return ""
        }

        var totalSeconds =
            Math.floor(
                ms / 1000
            )

        var minutes =
            Math.floor(
                totalSeconds / 60
            )

        var seconds =
            totalSeconds % 60

        return minutes + ":" +
            (
                    seconds < 10
                    ? "0"
                    : ""
            ) +
            seconds
    }


    function loadContent() {
        if (
            !root.genreController
        ) {
            return
        }

        if (
            root.subGenreId.length === 0
        ) {
            return
        }

        root.genreController.loadSubGenre(
            root.subGenreId
        )
    }


    // =============================================================
    // Background
    // =============================================================

    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    // =============================================================
    // Content
    // =============================================================

    Flickable {
        id: flickable

        anchors.fill:
            parent

        clip:
            true

        interactive:
            true

        flickableDirection:
            Flickable.VerticalFlick

        boundsBehavior:
            Flickable.StopAtBounds

        contentWidth:
            width

        contentHeight:
            contentColumn.y +
            contentColumn.height +
            48


        // =========================================================
        // Main column
        // =========================================================

        Column {
            id: contentColumn

            x:
                30

            y:
                28

            width:
                Math.max(
                    0,
                    flickable.width - 60
                )

            spacing:
                32


            // =====================================================
            // Header
            // =====================================================

            RowLayout {
                width:
                    parent.width

                height:
                    48

                spacing:
                    16


                Button {
                    Layout.alignment:
                        Qt.AlignVCenter

                    text:
                        "‹"

                    implicitWidth:
                        42

                    implicitHeight:
                        42

                    onClicked:
                        root.backRequested()
                }


                Rectangle {
                    Layout.alignment:
                        Qt.AlignVCenter

                    width:
                        6

                    height:
                        42

                    radius:
                        3

                    color:
                        root.subGenreColor ||
                        AppTheme.accent
                }


                Text {
                    Layout.fillWidth:
                        true

                    Layout.alignment:
                        Qt.AlignVCenter

                    text:
                        root.subGenreTitle

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        32

                    font.bold:
                        true

                    elide:
                        Text.ElideRight

                    maximumLineCount:
                        1

                    clip:
                        true
                }
            }


            // =====================================================
            // Playlists
            // =====================================================

            Column {
                width:
                    parent.width

                spacing:
                    16


                Text {
                    width:
                        parent.width

                    text:
                        "Плейлисты"

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        24

                    font.bold:
                        true
                }


                Flow {
                    width:
                        parent.width

                    spacing:
                        20


                    Repeater {
                        model:
                            root.genreController
                                ? root.genreController.subGenrePlaylists
                                : []


                        delegate:
                            Item {
                                width:
                                    180

                                height:
                                    238


                                Rectangle {
                                    id: playlistCard

                                    width:
                                        180

                                    height:
                                        180

                                    radius:
                                        10

                                    color:
                                        AppTheme.panel

                                    clip:
                                        true


                                    Image {
                                        anchors.fill:
                                            parent

                                        source:
                                            root.coverSource(
                                                modelData.coverUri
                                            )

                                        fillMode:
                                            Image.PreserveAspectCrop

                                        asynchronous:
                                            true

                                        cache:
                                            true

                                        visible:
                                            source !== ""
                                    }


                                    Rectangle {
                                        anchors.fill:
                                            parent

                                        color:
                                            AppTheme.accent

                                        opacity:
                                            playlistMouse.containsMouse
                                                ? 0.12
                                                : 0

                                        Behavior on opacity {
                                            NumberAnimation {
                                                duration:
                                                    120
                                            }
                                        }
                                    }
                                }


                                Column {
                                    anchors.top:
                                        playlistCard.bottom

                                    anchors.topMargin:
                                        8

                                    width:
                                        parent.width

                                    spacing:
                                        3


                                    Text {
                                        width:
                                            parent.width

                                        text:
                                            modelData.title || ""

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


                                    Text {
                                        width:
                                            parent.width

                                        text:
                                            (
                                                modelData.trackCount ||
                                                0
                                            ) +
                                            " треков"

                                        color:
                                            AppTheme.textSecondary

                                        font.pixelSize:
                                            13

                                        elide:
                                            Text.ElideRight

                                        maximumLineCount:
                                            1
                                    }
                                }


                                MouseArea {
                                    id: playlistMouse

                                    anchors.fill:
                                        parent

                                    hoverEnabled:
                                        true

                                    cursorShape:
                                        Qt.PointingHandCursor

                                    onClicked: {
                                        if (
                                            root.controller
                                        ) {
                                            root.controller.selectPersonalPlaylist(
                                                modelData.uid,
                                                modelData.kind
                                            )
                                        }
                                    }
                                }
                            }
                    }
                }


                Text {
                    visible:
                        root.genreController &&
                        root.genreController.subGenreLoading &&
                        (
                            !root.genreController.subGenrePlaylists ||
                            root.genreController.subGenrePlaylists.length === 0
                        )

                    text:
                        "Загрузка..."

                    color:
                        AppTheme.textSecondary

                    font.pixelSize:
                        14
                }
            }


            // =====================================================
            // Popular tracks
            // =====================================================

            Column {
                width:
                    parent.width

                spacing:
                    16


                Text {
                    width:
                        parent.width

                    text:
                        "Популярные треки"

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        24

                    font.bold:
                        true
                }


                Column {
                    width:
                        parent.width

                    spacing:
                        2


                    Repeater {
                        model:
                            root.genreController
                                ? root.genreController.subGenreTracks
                                : []


                        delegate:
                            Rectangle {
                                width:
                                    parent.width

                                height:
                                    68

                                radius:
                                    8

                                color:
                                    trackMouse.containsMouse
                                        ? AppTheme.panelHover
                                        : "transparent"


                                RowLayout {
                                    anchors.fill:
                                        parent

                                    anchors.leftMargin:
                                        8

                                    anchors.rightMargin:
                                        12

                                    spacing:
                                        12


                                    Rectangle {
                                        Layout.alignment:
                                            Qt.AlignVCenter

                                        width:
                                            52

                                        height:
                                            52

                                        radius:
                                            6

                                        color:
                                            AppTheme.panel

                                        clip:
                                            true


                                        Image {
                                            anchors.fill:
                                                parent

                                            source:
                                                root.coverSource(
                                                    modelData.coverUri
                                                )

                                            fillMode:
                                                Image.PreserveAspectCrop

                                            asynchronous:
                                                true

                                            cache:
                                                true

                                            visible:
                                                source !== ""
                                        }
                                    }


                                    ColumnLayout {
                                        Layout.fillWidth:
                                            true

                                        spacing:
                                            2


                                        Text {
                                            Layout.fillWidth:
                                                true

                                            text:
                                                modelData.title || ""

                                            color:
                                                AppTheme.textPrimary

                                            font.pixelSize:
                                                15

                                            elide:
                                                Text.ElideRight

                                            maximumLineCount:
                                                1
                                        }


                                        Text {
                                            Layout.fillWidth:
                                                true

                                            text:
                                                modelData.artist || ""

                                            color:
                                                AppTheme.textSecondary

                                            font.pixelSize:
                                                13

                                            elide:
                                                Text.ElideRight

                                            maximumLineCount:
                                                1
                                        }
                                    }


                                    Text {
                                        Layout.alignment:
                                            Qt.AlignVCenter

                                        text:
                                            root.formatDuration(
                                                modelData.durationMs
                                            )

                                        color:
                                            AppTheme.textSecondary

                                        font.pixelSize:
                                            13
                                    }
                                }


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
                                            root.genreController
                                        ) {
                                            root.genreController.playSubGenreTrack(
                                                index
                                            )
                                        }
                                    }
                                }
                            }
                    }
                }
            }


            // =====================================================
            // Bottom spacing
            // =====================================================

            Item {
                width:
                    1

                height:
                    1
            }
        }


        // =========================================================
        // Scroll bar
        // =========================================================

        ScrollBar.vertical:
            ScrollBar {
                policy:
                    ScrollBar.AsNeeded
            }
    }


    // =============================================================
    // Load content
    // =============================================================

    onSubGenreIdChanged: {
        root.loadContent()
    }


    onGenreControllerChanged: {
        root.loadContent()
    }


    Component.onCompleted: {
        root.loadContent()
    }
}