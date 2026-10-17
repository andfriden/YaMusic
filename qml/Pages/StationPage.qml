import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YaMusic 1.0

Item {
    id: root

    property var controller
    property string stationType: ""
    property string stationTag: ""
    property string stationTitle: ""

    width: parent ? parent.width : 0
    implicitWidth: width
    height: implicitHeight

    readonly property var stationsController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.stationsController !== undefined
            ? root.controller.stationsController
            : null

    readonly property var trackModel:
        root.stationsController !== null
            ? root.stationsController.tracksModel
            : null

    readonly property int trackHeight: 64

    implicitHeight:
        Math.max(120, headerHeight + trackList.implicitHeight)

    readonly property int headerHeight: 90

    Component.onCompleted: {
        if (root.stationsController !== null && root.stationsController !== undefined) {
            root.stationsController.selectStation(
                root.stationType, root.stationTag, root.stationTitle || "Волна")
        }
    }

    Column {
        id: content

        width: parent.width
        spacing: 20

        Rectangle {
            width: parent.width
            height: root.headerHeight
            radius: 14
            color: AppTheme.panel
            border.width: 1
            border.color: AppTheme.borderSubtle

            Column {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 16
                anchors.bottomMargin: 16

                spacing: 8

                Label {
                    width: parent.width
                    height: 28

                    text: root.stationTitle.length > 0
                        ? root.stationTitle
                        : "Станция"
                    color: AppTheme.textPrimary
                    font.pixelSize: 24
                    font.bold: true

                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }

                Label {
                    width: parent.width
                    height: 20

                    text: qsTr("Радиостанция · умный поток по вашим предпочтениям")
                    color: AppTheme.textMuted
                    font.pixelSize: 12

                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Rectangle {
            width: parent.width
            height: root.trackHeight * 6 + 5 * 6 + 32
            radius: 14
            color: AppTheme.panel
            border.width: 1
            border.color: AppTheme.borderSubtle

            visible: root.trackModel !== null && root.trackModel.count > 0

            ListView {
                id: trackList

                anchors.fill: parent
                anchors.margins: 14

                model: root.trackModel

                clip: true
                spacing: 6

                boundsBehavior: Flickable.StopAtBounds
                interactive: root.trackModel !== null && root.trackModel.count > 6

                ScrollBar.vertical: ScrollBar {
                    policy: root.trackModel !== null && root.trackModel.count > 6
                        ? ScrollBar.AsNeeded
                        : ScrollBar.AlwaysOff
                }

                delegate: TrackRow {
                    width: trackList.width -
                        (trackList.ScrollBar.vertical.visible ? 10 : 0)
                    height: root.trackHeight

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
                    showAlbum: true

                    onActivated: {
                        if (root.stationsController) {
                            root.stationsController.selectStationTrack(model.index)
                        }
                    }
                }
            }
        }
    }
}