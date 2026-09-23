import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    ListModel {
        id: randomTracksModel
    }

    Rectangle {
        anchors.fill: parent

        radius: 10
        color: AppTheme.backgroundPrimary
        border.width: 1
        border.color: AppTheme.panelHover

        Column {
            anchors.fill: parent
            anchors.margins: 14

            spacing: 12

            Label {
                width: parent.width

                text: qsTr("Недавно слушали")
                color: AppTheme.textPrimary
                font.pixelSize: 18
                font.bold: true
            }

            Column {
                id: emptyState

                width: parent.width
                height: parent.height - 46

                spacing: 8
                anchors.horizontalCenter: parent.horizontalCenter

                visible: randomTracksModel.count === 0

                BusyIndicator {
                    width: 28
                    height: 28

                    anchors.horizontalCenter: parent.horizontalCenter

                    running: root.hasController && root.controller.loadingMyWave
                }

                Label {
                    width: parent.width

                    text: !root.hasController ||
                        root.controller.recentListeningModel === undefined ||
                        root.controller.recentListeningModel === null
                        ? qsTr("История прослушивания недоступна")
                        : qsTr("Загрузка истории...")

                    color: AppTheme.textMuted
                    font.pixelSize: 12

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                }
            }

            ListView {
                id: tracksView

                width: parent.width
                height: parent.height - 46

                visible: randomTracksModel.count > 0

                clip: true
                spacing: 6

                model: randomTracksModel

                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.vertical: ScrollBar {
                    policy: tracksView.contentHeight > tracksView.height
                        ? ScrollBar.AsNeeded
                        : ScrollBar.AlwaysOff
                }

                delegate: Rectangle {
                    id: trackDelegate

                    required property int sourceIndex
                    required property string title
                    required property string artist
                    required property string coverUri

                    width: tracksView.width
                    height: 64

                    radius: 8
                    color: trackMouse.containsMouse
                        ? AppTheme.panelHover
                        : AppTheme.panelSecondary

                    border.width: 1
                    border.color: trackMouse.containsMouse
                        ? AppTheme.border
                        : AppTheme.borderSubtle

                    Rectangle {
                        id: artwork

                        width: 52
                        height: 52

                        anchors.left: parent.left
                        anchors.leftMargin: 6
                        anchors.verticalCenter: parent.verticalCenter

                        radius: 6
                        color: AppTheme.artworkPlaceholder
                        clip: true

                        Image {
                            id: coverImage

                            anchors.fill: parent

                            source: trackDelegate.coverUri.length > 0
                                ? "image://yandex/" + trackDelegate.coverUri
                                : ""

                            sourceSize: Qt.size(52, 52)

                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            cache: true
                            smooth: true

                            visible: status === Image.Ready
                        }

                        Label {
                            anchors.centerIn: parent

                            text: "♪"
                            color: AppTheme.textDisabled
                            font.pixelSize: 20

                            visible: coverImage.status !== Image.Ready
                        }
                    }

                    Column {
                        anchors.left: artwork.right
                        anchors.leftMargin: 10
                        anchors.right: parent.right
                        anchors.rightMargin: 8
                        anchors.verticalCenter: parent.verticalCenter

                        spacing: 2

                        Label {
                            width: parent.width

                            text: trackDelegate.title.length > 0
                                ? trackDelegate.title
                                : qsTr("Без названия")

                            color: AppTheme.textPrimary
                            font.pixelSize: 13
                            font.bold: true

                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }

                        Label {
                            width: parent.width

                            text: trackDelegate.artist.length > 0
                                ? trackDelegate.artist
                                : qsTr("Неизвестный исполнитель")

                            color: trackMouse.containsMouse
                                ? AppTheme.accent
                                : AppTheme.textSecondary

                            font.pixelSize: 11

                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }
                    }

                    MouseArea {
                        id: trackMouse

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.hasController) {
                                root.controller.selectRecentListening(trackDelegate.sourceIndex)
                            }
                        }
                    }
                }
            }
        }
    }

    function rebuildRandomTracks() {
        randomTracksModel.clear()

        if (!root.hasController) {
            return
        }

        const model = root.controller.recentListeningModel

        if (model === null || model === undefined) {
            return
        }

        const items = model.randomTrackData(10)

        if (items === null || items === undefined) {
            return
        }

        for (let i = 0; i < items.length; ++i) {
            const item = items[i]

            if (item === null || item === undefined) {
                continue
            }

            randomTracksModel.append({
                sourceIndex: Number(item.sourceIndex),
                trackId: String(item.trackId || ""),
                title: String(item.title || ""),
                artist: String(item.artist || ""),
                artistId: String(item.artistId || ""),
                album: String(item.album || ""),
                albumId: String(item.albumId || ""),
                coverUri: String(item.coverUri || ""),
                durationMs: Number(item.durationMs || 0)
            })
        }
    }

    Connections {
        target: root.hasController ? root.controller.recentListeningModel : null

        function onModelReset() {
            root.rebuildRandomTracks()
        }

        function onRowsInserted() {
            root.rebuildRandomTracks()
        }

        function onRowsRemoved() {
            root.rebuildRandomTracks()
        }
    }

    onControllerChanged: {
        root.rebuildRandomTracks()
    }

    Component.onCompleted: {
        root.rebuildRandomTracks()
    }
}