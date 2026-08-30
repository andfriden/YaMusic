import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller: null

    // =============================================================
    // Layout
    // =============================================================

    readonly property int sectionSpacing:
        24

    readonly property int titleSpacing:
        10

    readonly property int cardSpacing:
        12

    readonly property int cardHorizontalPadding:
        8

    readonly property int artworkTextSpacing:
        7

    readonly property int titleHeight:
        18

    readonly property int trackCountHeight:
        16


    // =============================================================
    // Size
    // =============================================================

    implicitHeight:
        sectionsColumn.implicitHeight


    // =============================================================
    // Sections
    // =============================================================

    Column {
        id: sectionsColumn

        width:
            parent.width

        spacing:
            root.sectionSpacing


        Repeater {
            model:
                    root.controller !== null &&
                root.controller !== undefined
                ? root.controller.personalPlaylistsModel
                : null


            delegate: Column {
                id: sectionItem

                required property string title
                required property var playlists

                width:
                    sectionsColumn.width

                spacing:
                    root.titleSpacing

                visible:
                    sectionItem.playlists !== null &&
                    sectionItem.playlists !== undefined &&
                    sectionItem.playlists.length > 0


                // =================================================
                // Card geometry
                // =================================================

                readonly property real availableWidth:
                    Math.max(
                        0,
                        sectionItem.width -
                        (
                            Math.max(
                                0,
                                sectionItem.playlists.length - 1
                            ) *
                            root.cardSpacing
                        )
                    )

                readonly property real cardWidth:
                    availableWidth /
                    Math.max(
                        1,
                        sectionItem.playlists.length
                    )

                readonly property real artworkSize:
                    Math.max(
                        1,
                        sectionItem.cardWidth -
                        (
                            root.cardHorizontalPadding * 2
                        )
                    )

                readonly property real cardHeight:
                    root.cardHorizontalPadding +
                    sectionItem.artworkSize +
                    root.artworkTextSpacing +
                    root.titleHeight +
                    root.trackCountHeight +
                    8


                // =================================================
                // Section title
                // =================================================

                Label {
                    width:
                        parent.width

                    height:
                        22

                    text:
                        sectionItem.title

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        19

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
                // Playlist row
                // =================================================

                Row {
                    id: playlistsRow

                    width:
                        parent.width

                    height:
                        sectionItem.cardHeight

                    spacing:
                        root.cardSpacing


                    Repeater {
                        model:
                            sectionItem.playlists


                        delegate: Rectangle {
                            id: playlistCard

                            required property var modelData

                            width:
                                sectionItem.cardWidth

                            height:
                                sectionItem.cardHeight

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
                                    root.cardHorizontalPadding

                                y:
                                    root.cardHorizontalPadding

                                width:
                                    sectionItem.artworkSize

                                height:
                                    sectionItem.artworkSize

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
                                        28

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
                                    root.cardHorizontalPadding

                                width:
                                    parent.width -
                                    root.cardHorizontalPadding * 2

                                anchors.top:
                                    artworkBox.bottom

                                anchors.topMargin:
                                    root.artworkTextSpacing

                                height:
                                    root.titleHeight

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
                                    root.cardHorizontalPadding

                                width:
                                    parent.width -
                                    root.cardHorizontalPadding * 2

                                anchors.top:
                                    titleLabel.bottom

                                anchors.topMargin:
                                    2

                                height:
                                    root.trackCountHeight

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

                                    root.controller.selectPersonalPlaylist(
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