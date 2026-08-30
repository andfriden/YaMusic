import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller: null

    // =============================================================
    // Model
    // =============================================================

    readonly property bool hasController:
        root.controller !== null &&
        root.controller !== undefined

    readonly property var recentModel:
            root.hasController &&
        root.controller.recentListeningModel !== null &&
        root.controller.recentListeningModel !== undefined
        ? root.controller.recentListeningModel
        : null

    // =============================================================
    // Layout
    // =============================================================

    readonly property int horizontalMargin: 12
    readonly property int verticalMargin: 12
    readonly property int headerHeight: 28
    readonly property int contentSpacing: 8
    readonly property int trackHeight: 68
    readonly property int trackSpacing: 6
    readonly property int maxVisibleTracks: 7

    readonly property int listHeight:
        (
            root.maxVisibleTracks *
            root.trackHeight
        ) +
        (
            (
                root.maxVisibleTracks - 1
            ) *
            root.trackSpacing
        )

    implicitHeight:
        root.verticalMargin * 2 +
        root.headerHeight +
        root.contentSpacing +
        root.listHeight

    // =============================================================
    // Background
    // =============================================================

    Rectangle {
        anchors.fill: parent

        radius: 10

        color:
            AppTheme.panel

        border.width: 1

        border.color:
            AppTheme.borderSubtle
    }

    // =============================================================
    // Content
    // =============================================================

    Column {
        id: contentColumn

        x:
            root.horizontalMargin

        y:
            root.verticalMargin

        width:
            Math.max(
                0,
                root.width -
                root.horizontalMargin * 2
            )

        spacing:
            root.contentSpacing

        // =========================================================
        // Header
        // =========================================================

        Row {
            width:
                parent.width

            height:
                root.headerHeight

            spacing: 10

            Label {
                id: titleLabel

                width:
                    Math.max(
                        0,
                        parent.width -
                        countLabel.implicitWidth -
                        10
                    )

                height:
                    parent.height

                text:
                    qsTr("Недавно слушали")

                color:
                    AppTheme.textPrimary

                font.pixelSize: 18
                font.bold: true

                verticalAlignment:
                    Text.AlignVCenter

                elide:
                    Text.ElideRight
            }

            Label {
                id: countLabel

                height:
                    parent.height

                text:
                        root.recentModel !== null &&
                    root.recentModel.count !== undefined &&
                    Number(root.recentModel.count) > 0
                    ? qsTr("%1 треков")
                        .arg(
                        Number(
                            root.recentModel.count
                        )
                    )
                    : ""

                color:
                    AppTheme.textSecondary

                font.pixelSize: 12

                verticalAlignment:
                    Text.AlignVCenter
            }
        }

        // =========================================================
        // Tracks
        // =========================================================

        ListView {
            id: tracksView

            width:
                parent.width

            height:
                root.listHeight

            model:
                root.recentModel

            clip:
                true

            spacing:
                root.trackSpacing

            boundsBehavior:
                Flickable.StopAtBounds

            interactive:
                true

            ScrollBar.vertical:
                ScrollBar {
                    policy:
                        ScrollBar.AsNeeded
                }

            // =====================================================
            // Track delegate
            // =====================================================

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
                    required property string albumId
                    required property int durationMs

                    width:
                        tracksView.width -
                        (
                            tracksView.ScrollBar.vertical.visible
                                ? 10
                                : 0
                        )

                    height:
                        root.trackHeight

                    radius: 8

                    color:
                        rowMouseArea.containsMouse
                            ? AppTheme.panelActive
                            : AppTheme.panelSecondary

                    border.width: 1

                    border.color:
                        rowMouseArea.containsMouse
                            ? AppTheme.border
                            : AppTheme.borderSubtle

                    Behavior on color {
                        ColorAnimation {
                            duration: 120
                            easing.type: Easing.OutCubic
                        }
                    }

                    // =================================================
                    // Artwork
                    // =================================================

                    Rectangle {
                        id: coverContainer

                        width: 52
                        height: 52

                        anchors.left:
                            parent.left

                        anchors.leftMargin:
                            8

                        anchors.verticalCenter:
                            parent.verticalCenter

                        radius: 6

                        color:
                            AppTheme.artworkPlaceholder

                        clip: true

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
                                    104,
                                    104
                                )

                            fillMode:
                                Image.PreserveAspectCrop

                            asynchronous: true
                            cache: true
                            smooth: true

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
                                cover.status !== Image.Ready
                        }
                    }

                    // =================================================
                    // Track information
                    // =================================================

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

                        spacing: 2

                        // ---------------------------------------------
                        // Title
                        // ---------------------------------------------

                        Label {
                            width:
                                parent.width

                            height:
                                20

                            text:
                                    trackDelegate.title.length > 0
                                ? trackDelegate.title
                                : qsTr("Без названия")

                            color:
                                AppTheme.textPrimary

                            font.pixelSize: 14
                            font.bold: true

                            elide:
                                Text.ElideRight

                            maximumLineCount: 1
                        }

                        // ---------------------------------------------
                        // Artist
                        // ---------------------------------------------

                        Item {
                            width:
                                parent.width

                            height: 18

                            Label {
                                id: artistLabel

                                width:
                                    Math.min(
                                        implicitWidth,
                                        parent.width
                                    )

                                height: 18

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

                                font.pixelSize: 12

                                elide:
                                    Text.ElideRight

                                maximumLineCount: 1
                            }

                            MouseArea {
                                id: artistMouseArea

                                anchors.fill:
                                    artistLabel

                                enabled:
                                    root.hasController &&
                                    trackDelegate.artistId.length > 0

                                hoverEnabled: true

                                cursorShape:
                                    enabled
                                        ? Qt.PointingHandCursor
                                        : Qt.ArrowCursor

                                z: 10

                                onClicked: {
                                    if (
                                        !enabled ||
                                        !root.hasController
                                    ) {
                                        return
                                    }

                                    root.controller.loadArtist(
                                        trackDelegate.artistId
                                    )
                                }
                            }
                        }

                        // ---------------------------------------------
                        // Album
                        // ---------------------------------------------

                        Label {
                            width:
                                parent.width

                            height:
                                14

                            text:
                                trackDelegate.album

                            color:
                                AppTheme.textMuted

                            font.pixelSize: 10

                            elide:
                                Text.ElideRight

                            maximumLineCount: 1

                            visible:
                                text.length > 0
                        }
                    }

                    // =================================================
                    // Duration
                    // =================================================

                    Label {
                        id: durationLabel

                        width: 44

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

                        font.pixelSize: 11

                        horizontalAlignment:
                            Text.AlignRight
                    }

                    // =================================================
                    // Whole row click
                    // =================================================

                    MouseArea {
                        id: rowMouseArea

                        anchors.fill:
                            parent

                        hoverEnabled: true

                        cursorShape:
                            Qt.PointingHandCursor

                        z: 0

                        onClicked: {
                            if (
                                !root.hasController
                            ) {
                                return
                            }

                            root.controller.selectRecentListening(
                                trackDelegate.index
                            )
                        }
                    }
                }

            // =====================================================
            // Empty state
            // =====================================================

            Item {
                anchors.fill:
                    parent

                visible:
                    root.recentModel !== null &&
                    root.recentModel.count !== undefined &&
                    Number(root.recentModel.count) === 0

                Label {
                    anchors.centerIn:
                        parent

                    text:
                        qsTr(
                            "Недавно слушали пока пусто"
                        )

                    color:
                        AppTheme.textSecondary

                    font.pixelSize: 12
                }
            }
        }
    }

    // =============================================================
    // Helpers
    // =============================================================

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
}