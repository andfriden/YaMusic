import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller
    property bool homeMode: false

    // =============================================================
    // Layout
    // =============================================================

    readonly property int sectionSpacing: 32
    readonly property int titleSpacing: 14
    readonly property int cardSpacing: 18
    readonly property int cardHorizontalPadding: 0
    readonly property int artworkTextSpacing: 10
    readonly property int titleHeight: 20
    readonly property int trackCountHeight: 16

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
                required property string type
                required property var playlists

                // =================================================
                // Home filtering
                // =================================================

                readonly property bool allowedOnHome:
                    !root.homeMode ||
                    sectionItem.type === "personal-playlists" ||
                    sectionItem.type === "new-playlists"

                readonly property var visiblePlaylists:
                    root.homeMode
                        ? sectionItem.playlists.slice(0, 4)
                        : sectionItem.playlists

                width:
                    sectionsColumn.width

                spacing:
                    root.titleSpacing

                visible:
                    sectionItem.allowedOnHome &&
                    sectionItem.visiblePlaylists !== null &&
                    sectionItem.visiblePlaylists !== undefined &&
                    sectionItem.visiblePlaylists.length > 0

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
                                sectionItem.visiblePlaylists.length - 1
                            ) *
                            root.cardSpacing
                        )
                    )

                readonly property real cardWidth:
                        sectionItem.visiblePlaylists.length > 0
                    ? sectionItem.availableWidth /
                    sectionItem.visiblePlaylists.length
                    : 0

                readonly property real artworkSize:
                    Math.max(
                        1,
                        sectionItem.cardWidth
                    )

                readonly property real cardHeight:
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
                        26

                    text:
                        sectionItem.title

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        20

                    font.weight:
                        Font.DemiBold

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
                            sectionItem.visiblePlaylists

                        delegate: Item {
                            id: playlistCard

                            required property var modelData

                            width:
                                sectionItem.cardWidth

                            height:
                                sectionItem.cardHeight

                            // =================================================
                            // Artwork
                            // =================================================

                            Rectangle {
                                id: artworkBox

                                width:
                                    sectionItem.artworkSize

                                height:
                                    sectionItem.artworkSize

                                anchors.left:
                                    parent.left

                                anchors.top:
                                    parent.top

                                radius:
                                    12

                                color:
                                    AppTheme.panelSubtle

                                clip:
                                    true

                                scale:
                                    playlistMouseArea.containsMouse
                                        ? 1.015
                                        : 1.0

                                Behavior on scale {
                                    NumberAnimation {
                                        duration: 140
                                        easing.type:
                                            Easing.OutCubic
                                    }
                                }

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
                                        32

                                    visible:
                                        cover.status !==
                                        Image.Ready
                                }

                                // Subtle hover overlay
                                Rectangle {
                                    anchors.fill:
                                        parent

                                    radius:
                                        12

                                    color:
                                        playlistMouseArea.containsMouse
                                            ? AppTheme.panelHover
                                            : "transparent"

                                    opacity:
                                        playlistMouseArea.containsMouse
                                            ? 0.08
                                            : 0

                                    Behavior on opacity {
                                        NumberAnimation {
                                            duration: 120
                                        }
                                    }
                                }
                            }

                            // =================================================
                            // Title
                            // =================================================

                            Label {
                                id: titleLabel

                                anchors.left:
                                    parent.left

                                anchors.right:
                                    parent.right

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
                                    playlistMouseArea.containsMouse
                                        ? AppTheme.textPrimary
                                        : AppTheme.textPrimary

                                font.pixelSize:
                                    14

                                font.weight:
                                    Font.Medium

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

                                anchors.left:
                                    parent.left

                                anchors.right:
                                    parent.right

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
                                    AppTheme.textMuted

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