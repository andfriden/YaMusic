import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller: null

    readonly property var recentModel:
        controller && controller.recentListeningModel
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
                width: Math.max(0, parent.width - countLabel.implicitWidth - 10)
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

                text: root.recentModel && root.recentModel.count > 0
                    ? qsTr("%1 треков").arg(root.recentModel.count)
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

                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                delegate: TrackRow {
                    width: tracksView.width -
                        (tracksView.ScrollBar.vertical.visible ? 10 : 0)

                    controller: root.controller
                    rowIndex: model.index
                    trackId: model.trackId
                    title: model.title
                    artist: model.artist
                    artistId: model.artistId
                    album: model.album
                    albumId: model.albumId
                    coverUri: model.coverUri
                    durationMs: model.durationMs

                    onActivated: {
                        if (root.controller) {
                            root.controller.selectRecentListening(model.index)
                        }
                    }
                }

                Label {
                    anchors.centerIn: parent

                    text: qsTr("Недавно слушали пока пусто")
                    color: AppTheme.textSecondary
                    font.pixelSize: 12

                    visible: root.recentModel !== null && root.recentModel.count === 0
                }
            }
        }
    }
}