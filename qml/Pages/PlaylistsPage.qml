import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller: null


    // =============================================================
    // Page size
    // =============================================================

    width:
        parent
            ? parent.width
            : 0

    implicitWidth:
        width

    implicitHeight:
        contentColumn.implicitHeight + 40

    height:
        implicitHeight


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

    Column {
        id: contentColumn

        width:
            parent.width

        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.top:
            parent.top

        anchors.margins:
            20

        spacing:
            28


        // =========================================================
        // Header
        // =========================================================

        Label {
            width:
                parent.width

            text:
                "Плейлисты"

            color:
                AppTheme.textPrimary

            font.pixelSize:
                28

            font.bold:
                true

            elide:
                Text.ElideRight
        }


        // =========================================================
        // Sections
        // =========================================================

        Repeater {
            model:
                    root.controller !== null &&
                root.controller !== undefined
                ? root.controller.personalPlaylistsModel
                : null


            delegate: Column {
                id: sectionItem

                required property string title
                required property string type
                required property var playlists


                width:
                    contentColumn.width

                spacing:
                    12


                visible:
                    sectionItem.playlists !== null &&
                    sectionItem.playlists !== undefined &&
                    sectionItem.playlists.length > 0


                // =================================================
                // Section title
                // =================================================

                Label {
                    width:
                        parent.width

                    height:
                        24

                    text:
                        sectionItem.title

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        20

                    font.bold:
                        true

                    verticalAlignment:
                        Text.AlignVCenter

                    elide:
                        Text.ElideRight

                    maximumLineCount:
                        1
                }


                // =================================================
                // Playlist grid
                // =================================================

                Grid {
                    width:
                        parent.width

                    columns:
                        Math.max(
                            1,
                            Math.floor(
                                width / 180
                            )
                        )

                    rowSpacing:
                        16

                    columnSpacing:
                        12


                    Repeater {
                        model:
                            sectionItem.playlists


                        delegate: Rectangle {
                            id: playlistCard

                            required property var modelData


                            width:
                                180

                            height:
                                238

                            radius:
                                10

                            color:
                                playlistMouseArea.containsMouse
                                    ? AppTheme.panelActive
                                    : AppTheme.panelSecondary

                            border.width:
                                1

                            border.color:
                                playlistMouseArea.containsMouse
                                    ? AppTheme.border
                                    : AppTheme.borderSubtle


                            Behavior on color {
                                ColorAnimation {
                                    duration:
                                        140

                                    easing.type:
                                        Easing.OutCubic
                                }
                            }


                            // =================================================
                            // Artwork
                            // =================================================

                            Rectangle {
                                id: artworkBox

                                x:
                                    8

                                y:
                                    8

                                width:
                                    parent.width - 16

                                height:
                                    width

                                radius:
                                    8

                                color:
                                    AppTheme.panelHover

                                clip:
                                    true


                                Image {
                                    id: cover

                                    anchors.fill:
                                        parent

                                    source:
                                            playlistCard.modelData.coverUri &&
                                        String(
                                            playlistCard.modelData.coverUri
                                        ).length > 0
                                        ? "image://yandex/" +
                                        String(
                                            playlistCard.modelData.coverUri
                                        )
                                        : ""

                                    sourceSize:
                                        Qt.size(
                                            Math.max(
                                                1,
                                                artworkBox.width * 2
                                            ),
                                            Math.max(
                                                1,
                                                artworkBox.height * 2
                                            )
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
                                        42

                                    visible:
                                        cover.status !==
                                        Image.Ready
                                }


                                Rectangle {
                                    anchors.fill:
                                        parent

                                    radius:
                                        8

                                    color:
                                        "transparent"

                                    border.width:
                                        1

                                    border.color:
                                        playlistMouseArea.containsMouse
                                            ? AppTheme.border
                                            : AppTheme.borderSubtle
                                }
                            }


                            // =================================================
                            // Title
                            // =================================================

                            Label {
                                id: titleLabel

                                x:
                                    8

                                width:
                                    parent.width - 16

                                anchors.top:
                                    artworkBox.bottom

                                anchors.topMargin:
                                    8

                                height:
                                    18

                                text:
                                    String(
                                        playlistCard.modelData.title ||
                                        ""
                                    )

                                color:
                                    AppTheme.textPrimary

                                font.pixelSize:
                                    13

                                font.bold:
                                    true

                                verticalAlignment:
                                    Text.AlignVCenter

                                elide:
                                    Text.ElideRight

                                maximumLineCount:
                                    1
                            }


                            // =================================================
                            // Track count
                            // =================================================

                            Label {
                                id: trackCountLabel

                                x:
                                    8

                                width:
                                    parent.width - 16

                                anchors.top:
                                    titleLabel.bottom

                                anchors.topMargin:
                                    3

                                height:
                                    16

                                text:
                                        Number(
                                            playlistCard.modelData.trackCount ||
                                            0
                                        ) > 0
                                    ? qsTr("%1 треков")
                                        .arg(
                                        Number(
                                            playlistCard.modelData.trackCount
                                        )
                                    )
                                    : ""

                                color:
                                    AppTheme.textSecondary

                                font.pixelSize:
                                    11

                                verticalAlignment:
                                    Text.AlignVCenter

                                elide:
                                    Text.ElideRight

                                maximumLineCount:
                                    1

                                visible:
                                    text.length > 0
                            }


                            // =================================================
                            // Click
                            // =================================================

                            MouseArea {
                                id: playlistMouseArea

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


                                    const uid =
                                        String(
                                            playlistCard.modelData.uid ||
                                            ""
                                        )


                                    const kind =
                                        Number(
                                            playlistCard.modelData.kind ||
                                            0
                                        )


                                    if (
                                        uid.length === 0 ||
                                        kind <= 0
                                    ) {
                                        return
                                    }


                                    root.controller
                                        .selectPersonalPlaylist(
                                        uid,
                                        kind
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