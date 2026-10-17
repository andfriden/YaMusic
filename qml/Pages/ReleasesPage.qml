import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YaMusic 1.0

Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    implicitWidth: width
    height: implicitHeight

    readonly property var personalController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.personalController !== undefined
            ? root.controller.personalController
            : null

    readonly property int cardWidth: 180
    readonly property int cardSpacing: 18

    readonly property var albums:
        root.personalController !== null
            ? root.personalController.newReleasesAlbums()
            : []

    readonly property int columns: Math.max(
        1, Math.floor(root.width / (root.cardWidth + root.cardSpacing)))

    readonly property int rowCount: Math.ceil(
        (root.albums ? root.albums.length : 0) / root.columns)

    readonly property int cardHeight: root.cardWidth + 52

    implicitHeight:
        headerHeight +
        root.rowCount * (root.cardHeight + 22)

    readonly property int headerHeight: 70

    Component.onCompleted: {
        if (root.controller !== null && root.controller !== undefined) {
            root.controller.loadRecommendations()
        }
    }

    Column {
        id: content

        width: parent.width
        spacing: 20

        Label {
            width: parent.width

            text: qsTr("Новые релизы")
            color: AppTheme.textPrimary
            font.pixelSize: 28
            font.bold: true
        }

        Rectangle {
            width: 40
            height: 3
            radius: 1.5

            gradient: Gradient {
                GradientStop { position: 0.0; color: AppTheme.accent }
                GradientStop { position: 1.0; color: AppTheme.mauveAccent }
            }
        }

        Grid {
            width: parent.width

            columns: root.columns
            spacing: root.cardSpacing

            Repeater {
                model: root.albums

                delegate: Item {
                    width: root.cardWidth
                    height: root.cardHeight

                    Rectangle {
                        id: artwork

                        width: root.cardWidth
                        height: root.cardWidth
                        radius: 12
                        color: AppTheme.panelSubtle
                        clip: true

                        Image {
                            id: coverImage

                            anchors.fill: parent

                            source: modelData.coverUri && String(modelData.coverUri).length > 0
                                ? "image://yandex/" + String(modelData.coverUri)
                                : ""

                            sourceSize: Qt.size(artwork.width * 2, artwork.height * 2)
                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            cache: true
                            smooth: true

                            visible: status === Image.Ready
                        }

                        Label {
                            anchors.centerIn: parent

                            text: "♪"
                            color: AppTheme.textSecondary
                            font.pixelSize: 32

                            visible: coverImage.status !== Image.Ready
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                const albumId = String(modelData.albumId || "")

                                if (root.controller && albumId.length > 0) {
                                    root.controller.loadAlbum(albumId)
                                }
                            }
                        }
                    }

                    Label {
                        id: titleLabel

                        anchors.top: artwork.bottom
                        anchors.topMargin: 8
                        anchors.left: parent.left
                        anchors.right: parent.right

                        height: 18

                        text: String(modelData.title || "")
                        color: AppTheme.textPrimary
                        font.pixelSize: 13
                        font.weight: Font.Medium

                        elide: Text.ElideRight
                        maximumLineCount: 1
                        clip: true
                    }

                    Label {
                        anchors.top: titleLabel.bottom
                        anchors.topMargin: 2
                        anchors.left: parent.left
                        anchors.right: parent.right

                        height: 16

                        text: Number(modelData.year || 0) > 0
                            ? String(modelData.year)
                            : ""

                        color: AppTheme.textMuted
                        font.pixelSize: 11

                        elide: Text.ElideRight
                        visible: text.length > 0
                    }
                }
            }
        }
    }
}