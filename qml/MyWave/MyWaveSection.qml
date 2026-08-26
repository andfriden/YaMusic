import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller
    property bool compactMode: false

    readonly property bool hasController:
        root.controller !== null &&
        root.controller !== undefined

    readonly property bool hasTracks:
        root.hasController &&
        root.controller.myWaveModel !== null &&
        root.controller.myWaveModel !== undefined &&
        root.controller.myWaveModel.count > 0

    readonly property int margin:
        root.compactMode ? 12 : 24


    Rectangle {
        anchors.fill: parent

        visible:
            root.hasTracks || root.compactMode

        radius:
            root.compactMode ? 10 : 14

        color: AppTheme.panel

        border.width: 1
        border.color: "#d4d4d4"
    }


    Column {
        anchors.fill: parent
        anchors.margins: root.margin

        spacing:
            root.compactMode ? 10 : 16


        Label {
            text: "Моя волна"

            color: AppTheme.textPrimary

            font.pixelSize:
                root.compactMode ? 18 : 30

            font.bold: true

            height:
                root.compactMode ? 28 : 40
        }


        ListView {
            id: trackList

            width: parent.width

            height:
                parent.height -
                (root.compactMode ? 38 : 56)

            model:
                root.hasController
                    ? root.controller.myWaveModel
                    : null

            clip: true

            spacing: 6


            ScrollBar.vertical:
                ScrollBar {
                    policy:
                        ScrollBar.AsNeeded
                }


            delegate: Rectangle {

                required property int index
                required property string title
                required property string artist
                required property string album
                required property string coverUri
                required property int durationMs


                width:
                    trackList.width -
                    (
                        trackList.ScrollBar.vertical.visible
                            ? 10
                            : 0
                    )

                height:
                    root.compactMode
                        ? 58
                        : 68


                radius: 9


                color:
                    root.isCurrentTrack(
                        title,
                        artist)
                        ? "#d9d9d9"
                        : (
                            mouse.containsMouse
                                ? "#e1e1e1"
                                : "#f2f2f2"
                        )


                border.width: 1

                border.color:
                    root.isCurrentTrack(
                        title,
                        artist)
                        ? "#bdbdbd"
                        : "#e0e0e0"



                Image {
                    id: cover

                    anchors.left:
                        parent.left

                    anchors.leftMargin:
                        root.compactMode ? 7 : 9

                    anchors.verticalCenter:
                        parent.verticalCenter


                    width:
                        root.compactMode ? 44 : 52

                    height:
                        root.compactMode ? 44 : 52


                    source:
                            coverUri.length > 0
                        ? "image://yandex/" + coverUri
                        : ""


                    fillMode:
                        Image.PreserveAspectCrop

                    asynchronous: true
                    cache: true


                    Rectangle {
                        anchors.fill: parent

                        radius: 6

                        color: AppTheme.surface

                        visible:
                            cover.status !== Image.Ready


                        Label {
                            anchors.centerIn: parent

                            text: "♪"

                            color:
                                AppTheme.textSecondary

                            font.pixelSize:
                                root.compactMode ? 18 : 20
                        }
                    }
                }



                Column {

                    anchors.left:
                        cover.right

                    anchors.leftMargin: 12

                    anchors.right:
                        duration.left

                    anchors.rightMargin: 10

                    anchors.verticalCenter:
                        parent.verticalCenter


                    spacing: 2


                    Label {

                        width: parent.width

                        text:
                                title.length > 0
                            ? title
                            : "Без названия"


                        color:
                            AppTheme.textPrimary


                        font.pixelSize:
                            root.compactMode ? 13 : 14


                        font.bold: true

                        elide:
                            Text.ElideRight
                    }


                    Label {

                        width: parent.width


                        text:
                            artist


                        color:
                            AppTheme.textSecondary


                        font.pixelSize:
                            root.compactMode ? 11 : 12


                        elide:
                            Text.ElideRight
                    }


                    Label {

                        visible:
                            !root.compactMode &&
                            album.length > 0


                        width: parent.width


                        text:
                            album


                        color:
                            AppTheme.textMuted


                        font.pixelSize: 10


                        elide:
                            Text.ElideRight
                    }
                }



                Label {

                    id: duration


                    anchors.right:
                        parent.right

                    anchors.rightMargin:
                        root.compactMode ? 10 : 14


                    anchors.verticalCenter:
                        parent.verticalCenter


                    text:
                        root.formatDuration(
                            durationMs)


                    color:
                        AppTheme.textSecondary


                    font.pixelSize: 11
                }



                MouseArea {

                    id: mouse


                    anchors.fill:
                        parent


                    hoverEnabled: true


                    cursorShape:
                        Qt.PointingHandCursor


                    onClicked: {

                        root.controller.selectMyWaveTrack(
                            index)

                    }
                }
            }
        }
    }



    function isCurrentTrack(title, artist)
    {
        if (!root.hasController)
            return false


        return (
            root.controller.currentTrackTitle === title &&
            root.controller.currentTrackArtist === artist
        )
    }



    function formatDuration(milliseconds)
    {
        if (!milliseconds || milliseconds <= 0)
            return "0:00"


        var totalSeconds =
            Math.floor(milliseconds / 1000)


        var minutes =
            Math.floor(totalSeconds / 60)


        var seconds =
            totalSeconds % 60


        return minutes +
            ":" +
            (seconds < 10 ? "0" : "") +
            seconds
    }
}