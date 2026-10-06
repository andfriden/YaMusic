import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller: null

    readonly property var podcasts:
        root.controller !== null && root.controller !== undefined
            ? root.controller.podcasts
            : []

    readonly property int columns:
        Math.max(1, Math.floor(contentColumn.width / 180))

    width: parent ? parent.width : 0
    implicitWidth: width
    implicitHeight: contentColumn.implicitHeight + 40
    height: implicitHeight

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    Column {
        id: contentColumn

        width: parent.width
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20

        spacing: 28

        Label {
            width: parent.width

            text: "Подкасты"
            color: AppTheme.textPrimary
            font.pixelSize: 28
            font.bold: true
            elide: Text.ElideRight
        }

        Label {
            width: parent.width

            visible: root.podcasts.length === 0

            text: "Подкасты пока не загрузились"
            color: AppTheme.textSecondary
            font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
        }

        Grid {
            width: parent.width

            columns: root.columns
            rowSpacing: 16
            columnSpacing: 12

            Repeater {
                model: root.podcasts

                delegate: Item {
                    required property var modelData

                    width: Math.floor((contentColumn.width - (root.columns - 1) * 12) / root.columns)
                    height: 232

                    Rectangle {
                        id: artworkBox

                        width: parent.width - 16
                        height: width
                        anchors.horizontalCenter: parent.horizontalCenter

                        radius: 8
                        color: AppTheme.artworkPlaceholder
                        clip: true

                        Image {
                            id: artworkImage

                            anchors.fill: parent

                            source: String(modelData.coverUri || "").length > 0
                                ? "image://yandex/" + modelData.coverUri
                                : ""

                            sourceSize: Qt.size(width * 2, width * 2)

                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            cache: true
                            smooth: true

                            visible: status === Image.Ready
                        }

                        Label {
                            anchors.centerIn: parent

                            text: "♪"
                            color: AppTheme.textMuted
                            font.pixelSize: 28

                            visible: artworkImage.status !== Image.Ready
                        }
                    }

                    Label {
                        id: titleLabel

                        width: parent.width - 16
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: artworkBox.bottom
                        anchors.topMargin: 8

                        text: String(modelData.title || "")

                        color: AppTheme.textPrimary
                        font.pixelSize: 14
                        font.bold: true

                        elide: Text.ElideRight
                        maximumLineCount: 1
                    }

                    Label {
                        width: parent.width - 16
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: titleLabel.bottom
                        anchors.topMargin: 2

                        text: String(modelData.description || "")

                        color: AppTheme.textSecondary
                        font.pixelSize: 12

                        wrapMode: Text.Wrap
                        maximumLineCount: 2
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }
}