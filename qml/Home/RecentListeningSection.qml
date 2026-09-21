import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller: null

    readonly property var recentModel:
        controller &&
        controller.recentListeningModel
        ? controller.recentListeningModel
        : null

    readonly property int horizontalMargin: 12
    readonly property int verticalMargin: 12
    readonly property int headerHeight: 30
    readonly property int contentSpacing: 10
    readonly property int trackHeight: 68
    readonly property int trackSpacing: 6
    readonly property int maxVisibleTracks: 7

    readonly property int listHeight:
        maxVisibleTracks * trackHeight +
        (maxVisibleTracks - 1) * trackSpacing

    implicitHeight:
        verticalMargin * 2 +
        headerHeight +
        contentSpacing +
        listHeight

    Rectangle {
        anchors.fill: parent
        radius: 10
        color: AppTheme.panel
        border.width: 1
        border.color: AppTheme.borderSubtle
    }

    Column {
        anchors.fill: parent
        anchors.leftMargin: root.horizontalMargin
        anchors.rightMargin: root.horizontalMargin
        anchors.topMargin: root.verticalMargin
        anchors.bottomMargin: root.verticalMargin

        spacing: root.contentSpacing

        Row {
            width: parent.width
            height: root.headerHeight
            spacing: 10

            Label {
                width: Math.max(
                    0,
                    parent.width -
                    countLabel.implicitWidth -
                    10
                )

                height: parent.height

                text: qsTr("Недавно слушали")
                color: AppTheme.textPrimary

                font.pixelSize: 18
                font.bold: true

                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            Label {
                id: countLabel

                height: parent.height

                text:
                    root.recentModel &&
                    root.recentModel.count > 0
                    ? qsTr("%1 треков")
                        .arg(root.recentModel.count)
                    : ""

                color: AppTheme.textSecondary
                font.pixelSize: 12

                verticalAlignment: Text.AlignVCenter
            }
        }

        Item {
            width: parent.width
            height: root.listHeight

            ListView {
                id: tracksView

                anchors.fill: parent

                model: root.recentModel

                clip: true
                spacing: root.trackSpacing

                boundsBehavior: Flickable.StopAtBounds
                interactive: contentHeight > height

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }

                delegate: Rectangle {
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
                        (tracksView.ScrollBar.vertical.visible
                         ? 10
                         : 0)

                    height: root.trackHeight
                    radius: 8

                    color:
                        root.controller &&
                        root.controller.currentTrackId !== "" &&
                        trackDelegate.trackId ===
                            root.controller.currentTrackId
                        ? AppTheme.panelActive
                        : rowMouseArea.containsMouse
                            ? AppTheme.panelHover
                            : AppTheme.panelSecondary

                    border.width:
                        root.controller &&
                        root.controller.currentTrackId !== "" &&
                        trackDelegate.trackId ===
                            root.controller.currentTrackId
                        ? 1
                        : 0

                    border.color: AppTheme.accent

                    Rectangle {
                        id: coverContainer

                        width: 52
                        height: 52

                        anchors.left: parent.left
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter

                        radius: 6
                        color: AppTheme.artworkPlaceholder

                        clip: true

                        Image {
                            id: coverImage

                            anchors.fill: parent

                            source:
                                trackDelegate.coverUri.length > 0
                                ? "image://yandex/" +
                                  trackDelegate.coverUri
                                : ""

                            sourceSize: Qt.size(104, 104)

                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            cache: true
                            smooth: true

                            visible:
                                status === Image.Ready
                        }

                        Label {
                            anchors.centerIn: parent

                            text: "♪"

                            color: AppTheme.textSecondary
                            font.pixelSize: 20

                            visible:
                                coverImage.status !== Image.Ready
                        }
                    }

                    Column {
                        id: trackInfo

                        anchors.left: coverContainer.right
                        anchors.leftMargin: 12

                        anchors.right: durationLabel.left
                        anchors.rightMargin: 12

                        anchors.verticalCenter: parent.verticalCenter

                        spacing: 2

                        Label {
                            width: parent.width
                            height: 20

                            text:
                                trackDelegate.title.length > 0
                                ? trackDelegate.title
                                : qsTr("Без названия")

                            color: AppTheme.textPrimary

                            font.pixelSize: 14
                            font.bold: true

                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }

                        Item {
                            width: parent.width
                            height: 18

                            Label {
                                id: artistLabel

                                width: parent.width
                                height: parent.height

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

                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }

                            MouseArea {
                                id: artistMouseArea

                                anchors.fill: artistLabel

                                enabled:
                                    root.controller &&
                                    trackDelegate.artistId.length > 0

                                hoverEnabled: true

                                cursorShape:
                                    enabled
                                    ? Qt.PointingHandCursor
                                    : Qt.ArrowCursor

                                z: 10

                                onClicked: {
                                    if (!enabled)
                                        return

                                    root.controller.loadArtist(
                                        trackDelegate.artistId
                                    )
                                }
                            }
                        }

                        Label {
                            width: parent.width
                            height: 14

                            text: trackDelegate.album

                            color: AppTheme.textMuted
                            font.pixelSize: 10

                            elide: Text.ElideRight
                            maximumLineCount: 1

                            visible: text.length > 0
                        }
                    }

                    Label {
                        id: durationLabel

                        width: 44

                        anchors.right: parent.right
                        anchors.rightMargin: 14
                        anchors.verticalCenter: parent.verticalCenter

                        text:
                            root.formatDuration(
                                trackDelegate.durationMs
                            )

                        color: AppTheme.textSecondary

                        font.pixelSize: 11

                        horizontalAlignment:
                            Text.AlignRight
                    }

                    MouseArea {
                        id: rowMouseArea

                        anchors.fill: parent

                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        acceptedButtons:
                            Qt.LeftButton | Qt.RightButton

                        z: 0

                        onClicked: function(mouse) {
                            if (!root.controller)
                                return

                            if (mouse.button !== Qt.LeftButton)
                                return

                            root.controller.selectRecentListening(
                                trackDelegate.index
                            )
                        }

                        onPressed: function(mouse) {
                            if (!root.controller)
                                return

                            if (mouse.button !== Qt.RightButton)
                                return

                            root.controller.copyTrack(
                                trackDelegate.title,
                                trackDelegate.artist
                            )
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent

                text: qsTr("Недавно слушали пока пусто")

                color: AppTheme.textSecondary
                font.pixelSize: 12

                visible:
                    root.recentModel !== null &&
                    root.recentModel.count === 0
            }
        }
    }

    function formatDuration(milliseconds) {
        var value = Number(milliseconds)

        if (!isFinite(value) || value <= 0)
            return "0:00"

        var totalSeconds = Math.floor(value / 1000)
        var minutes = Math.floor(totalSeconds / 60)
        var seconds = totalSeconds % 60

        return minutes +
               ":" +
               (seconds < 10 ? "0" : "") +
               seconds
    }
}
