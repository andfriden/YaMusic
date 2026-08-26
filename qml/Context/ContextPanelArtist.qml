import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    readonly property var artistController:
            root.controller !== null &&
        root.controller !== undefined &&
        root.controller.artistController !== undefined &&
        root.controller.artistController !== null
        ? root.controller.artistController
        : null

    readonly property var similarArtistsModel:
            root.artistController !== null &&
        root.artistController.similarArtistsModel !== undefined &&
        root.artistController.similarArtistsModel !== null
        ? root.artistController.similarArtistsModel
        : null

    Rectangle {
        id: panel

        anchors.fill: parent

        color: AppTheme.backgroundSecondary

        border.width: 1
        border.color: AppTheme.borderSubtle

        radius: 12

        Column {
            anchors.fill: parent
            anchors.margins: 14

            spacing: 12

            Label {
                width: parent.width

                text: qsTr("Похожие исполнители")

                color: AppTheme.textPrimary

                font.pixelSize: 17
                font.bold: true

                elide: Text.ElideRight
            }

            Label {
                width: parent.width

                text: qsTr("Исполнители с похожим звучанием")

                color: AppTheme.textMuted

                font.pixelSize: 11

                wrapMode: Text.WordWrap
            }

            Item {
                width: parent.width
                height: parent.height - 70

                ListView {
                    id: artistsView

                    anchors.fill: parent

                    clip: true

                    spacing: 8

                    model: root.similarArtistsModel

                    boundsBehavior: Flickable.StopAtBounds

                    ScrollBar.vertical: ScrollBar {
                        policy:
                                artistsView.contentHeight > artistsView.height
                            ? ScrollBar.AsNeeded
                            : ScrollBar.AlwaysOff
                    }

                    delegate: Rectangle {

                        required property int index
                        required property string artistId
                        required property string name
                        required property string coverUri

                        width: artistsView.width
                        height: 58

                        radius: 8

                        color:
                            mouseArea.containsMouse
                                ? "#dedede"
                                : "#f4f4f4"

                        border.width: 1

                        border.color:
                            mouseArea.containsMouse
                                ? "#c6c6c6"
                                : "#e1e1e1"

                        Rectangle {
                            id: artwork

                            width: 44
                            height: 44

                            anchors.left: parent.left
                            anchors.leftMargin: 7
                            anchors.verticalCenter: parent.verticalCenter

                            radius: width / 2

                            color: "#d3d3d3"

                            clip: true

                            Image {
                                id: image

                                anchors.fill: parent

                                source:
                                        coverUri.length > 0
                                    ? "image://yandex/" + coverUri
                                    : ""

                                sourceSize: Qt.size(44, 44)

                                fillMode: Image.PreserveAspectCrop

                                asynchronous: true
                                cache: true

                                visible:
                                    status === Image.Ready
                            }

                            Label {
                                anchors.centerIn: parent

                                text: "♪"

                                color: AppTheme.textMuted

                                font.pixelSize: 18

                                visible:
                                    image.status !== Image.Ready
                            }
                        }

                        Column {
                            anchors.left: artwork.right
                            anchors.leftMargin: 11
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            anchors.verticalCenter: parent.verticalCenter

                            spacing: 2

                            Label {
                                width: parent.width

                                text: name

                                color: AppTheme.textPrimary

                                font.pixelSize: 13
                                font.bold: true

                                elide: Text.ElideRight
                            }

                            Label {
                                width: parent.width

                                text: qsTr("Исполнитель")

                                color: AppTheme.textMuted

                                font.pixelSize: 11
                            }
                        }

                        MouseArea {
                            id: mouseArea

                            anchors.fill: parent

                            hoverEnabled: true

                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (root.artistController === null) {
                                    return
                                }

                                root.artistController.selectSimilarArtist(
                                    index
                                )
                            }
                        }
                    }
                }

                Label {
                    anchors.centerIn: parent

                    width: parent.width - 20

                    text:
                            root.artistController !== null &&
                        root.artistController.loading
                        ? qsTr("Загрузка...")
                        : qsTr("Нет похожих исполнителей")

                    color: AppTheme.textDisabled

                    font.pixelSize: 12

                    horizontalAlignment: Text.AlignHCenter

                    visible:
                        artistsView.count === 0
                }
            }
        }
    }

    /*
     * ============================================================
     * Theme diagnostics
     * ============================================================
     */

    Component.onCompleted: {
        console.log("========================================")
        console.log("ContextPanelArtist")
        console.log("AppTheme.border =", AppTheme.border)
        console.log("AppTheme.borderSubtle =", AppTheme.borderSubtle)
        console.log("AppTheme.borderStrong =", AppTheme.borderStrong)
        console.log("AppTheme.panel =", AppTheme.panel)
        console.log("AppTheme.backgroundSecondary =", AppTheme.backgroundSecondary)
        console.log("========================================")
    }
}