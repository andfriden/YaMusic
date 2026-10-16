import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YaMusic 1.0

Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    height: parent ? parent.height : 700
    implicitWidth: width
    implicitHeight: height

    readonly property var stats:
        root.controller !== null && root.controller !== undefined &&
        root.controller.statsController !== undefined
            ? root.controller.statsController
            : null

    Flickable {
        id: pageFlickable

        anchors.fill: parent

        clip: true
        boundsBehavior: Flickable.StopAtBounds

        contentWidth: width
        contentHeight: contentColumn.implicitHeight + 40

        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

        Column {
            id: contentColumn

            width: pageFlickable.width - 40
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20

            spacing: 28

            Row {
                width: parent.width
                spacing: 12

                Label {
                    width: parent.width - resetButton.width - parent.spacing

                    text: "Статистика прослушивания"
                    color: AppTheme.textPrimary
                    font.pixelSize: 28
                    font.bold: true

                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.Wrap
                }

                Rectangle {
                    id: resetButton

                    width: resetLabel.implicitWidth + 24
                    height: 32

                    anchors.verticalCenter: parent.verticalCenter

                    radius: 8
                    color: resetMouse.containsMouse ? AppTheme.panelActive : AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    Label {
                        id: resetLabel

                        anchors.centerIn: parent

                        text: "Сбросить"
                        color: resetMouse.containsMouse ? AppTheme.error : AppTheme.textSecondary
                        font.pixelSize: 12
                    }

                    MouseArea {
                        id: resetMouse

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.stats) {
                                root.stats.reset()
                            }
                        }
                    }
                }
            }

            // Сводка.
            Rectangle {
                width: parent.width
                height: 100

                radius: 12
                color: AppTheme.panel
                border.width: 1
                border.color: AppTheme.borderSubtle

                Row {
                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 8

                    Item {
                        width: parent.width / 4 - parent.spacing / 2
                        height: parent.height

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: root.stats ? root.stats.totalPlays : 0
                                color: AppTheme.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "Прослушиваний"
                                color: AppTheme.textSecondary
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.Wrap
                            }
                        }
                    }

                    Item {
                        width: parent.width / 4 - parent.spacing / 2
                        height: parent.height

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: root.stats ? AppTheme.formatTime(root.stats.totalPlayTimeMs) : "0:00"
                                color: AppTheme.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "Времени в музыке"
                                color: AppTheme.textSecondary
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.Wrap
                            }
                        }
                    }

                    Item {
                        width: parent.width / 4 - parent.spacing / 2
                        height: parent.height

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: root.stats ? root.stats.trackCount : 0
                                color: AppTheme.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "Треков"
                                color: AppTheme.textSecondary
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.Wrap
                            }
                        }
                    }

                    Item {
                        width: parent.width / 4 - parent.spacing / 2
                        height: parent.height

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: root.stats ? root.stats.artistCount : 0
                                color: AppTheme.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "Исполнителей"
                                color: AppTheme.textSecondary
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.Wrap
                            }
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 16

                Column {
                    width: parent.width / 2 - parent.spacing / 2
                    spacing: 12

                    Label {
                        width: parent.width

                        text: "Топ треков"
                        color: AppTheme.textPrimary
                        font.pixelSize: 20
                        font.bold: true
                    }

                    Rectangle {
                        width: parent.width

                    height: root.stats && root.stats.topTracks(20).length > 0
                        ? Math.min(root.stats.topTracks(20).length * 56 + 20, 10 * 56 + 20)
                        : 100

                    radius: 12
                    color: AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    ListView {
                        id: topTracksView

                        anchors.fill: parent
                        anchors.margins: 10

                        spacing: 6
                        clip: true

                        interactive: topTracksView.count > 8
                        boundsBehavior: Flickable.StopAtBounds

                        model: {
                            root.stats
                            AppTheme.dark
                            return root.stats ? root.stats.topTracks(20) : []
                        }

                        delegate: Item {
                            required property int index
                            required property string id
                            required property string title
                            required property string artist
                            required property string coverUri
                            required property int plays

                            width: topTracksView.width
                            height: 50

                            Rectangle {
                                anchors.fill: parent
                                radius: 8
                                color: rowArea.containsMouse
                                    ? AppTheme.panelActive
                                    : "transparent"

                                Behavior on color {
                                    ColorAnimation { duration: 120 }
                                }
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 9
                                anchors.rightMargin: 9

                                spacing: 11

                                Label {
                                    Layout.preferredWidth: 20
                                    Layout.alignment: Qt.AlignVCenter

                                    text: index + 1
                                    color: index < 3 ? AppTheme.accent : AppTheme.textMuted
                                    font.pixelSize: 12
                                    font.bold: index < 3
                                                                        horizontalAlignment: Text.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Rectangle {
                                    Layout.preferredWidth: 36
                                    Layout.preferredHeight: 36
                                    Layout.alignment: Qt.AlignVCenter

                                    radius: 18
                                    color: AppTheme.artworkPlaceholder
                                    clip: true

                                    Image {
                                        id: rowCover
                                        anchors.fill: parent
                                        source: coverUri.length > 0
                                            ? "image://yandex/circle/" + coverUri
                                            : ""
                                        sourceSize: Qt.size(72, 72)
                                        fillMode: Image.PreserveAspectCrop
                                        asynchronous: true
                                        cache: true
                                        smooth: true
                                        visible: status === Image.Ready
                                    }

                                    Icon {
                                        anchors.centerIn: parent
                                        glyph: "note"
                                        color: AppTheme.textMuted
                                        size: 16
                                        strokeWidth: 1.6
                                        visible: rowCover.status !== Image.Ready
                                    }
                                }

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter

                                    spacing: 3

                                    Label {
                                        Layout.fillWidth: true
                                        text: title
                                        color: AppTheme.textPrimary
                                        font.pixelSize: 13
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                    }

                                    Label {
                                        Layout.fillWidth: true
                                        text: artist
                                        color: AppTheme.textSecondary
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                    }
                                }

                                Label {
                                    Layout.alignment: Qt.AlignVCenter

                                    text: plays >= 100 ? "100+" : plays + "×"
                                    color: AppTheme.textMuted
                                    font.pixelSize: 11
                                                                        verticalAlignment: Text.AlignVCenter
                                }
                            }

                            MouseArea {
                                id: rowArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "Нет данных — послушайте несколько треков"
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        visible: root.stats === null ||
                            root.stats.topTracks(20).length === 0
                    }
                }
            }

            Column {
                    width: parent.width / 2 - parent.spacing / 2
                    spacing: 12

                    Label {
                        width: parent.width

                        text: "Топ исполнителей"
                        color: AppTheme.textPrimary
                        font.pixelSize: 20
                        font.bold: true
                    }

                    Rectangle {
                    width: parent.width

                    height: root.stats && root.stats.topArtists(20).length > 0
                        ? Math.min(root.stats.topArtists(20).length * 56 + 20, 10 * 56 + 20)
                        : 100

                    radius: 12
                    color: AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    ListView {
                        id: topArtistsView

                        anchors.fill: parent
                        anchors.margins: 10

                        spacing: 6
                        clip: true

                        interactive: topArtistsView.count > 8
                        boundsBehavior: Flickable.StopAtBounds

                        model: {
                            root.stats
                            AppTheme.dark
                            return root.stats ? root.stats.topArtists(20) : []
                        }

                        delegate: Item {
                            required property int index
                            required property string id
                            required property string name
                            required property string coverUri
                            required property int plays

                            width: topArtistsView.width
                            height: 50

                            Rectangle {
                                anchors.fill: parent
                                radius: 8
                                color: artistRowArea.containsMouse
                                    ? AppTheme.panelActive
                                    : "transparent"

                                Behavior on color {
                                    ColorAnimation { duration: 120 }
                                }
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 9
                                anchors.rightMargin: 9

                                spacing: 11

                                Label {
                                    Layout.preferredWidth: 20
                                    Layout.alignment: Qt.AlignVCenter

                                    text: index + 1
                                    color: index < 3 ? AppTheme.accent : AppTheme.textMuted
                                    font.pixelSize: 12
                                    font.bold: index < 3
                                                                        horizontalAlignment: Text.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Rectangle {
                                    Layout.preferredWidth: 36
                                    Layout.preferredHeight: 36
                                    Layout.alignment: Qt.AlignVCenter

                                    radius: 18
                                    color: AppTheme.artworkPlaceholder
                                    clip: true

                                    Image {
                                        id: artistAvatar
                                        anchors.fill: parent
                                        source: coverUri.length > 0
                                            ? "image://yandex/circle/" + coverUri
                                            : ""
                                        sourceSize: Qt.size(72, 72)
                                        fillMode: Image.PreserveAspectCrop
                                        asynchronous: true
                                        cache: true
                                        smooth: true
                                        visible: status === Image.Ready
                                    }

                                    Icon {
                                        anchors.centerIn: parent
                                        glyph: "artist"
                                        color: AppTheme.textMuted
                                        size: 16
                                        strokeWidth: 1.6
                                        visible: artistAvatar.status !== Image.Ready
                                    }
                                }

                                Label {
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter

                                    text: name
                                    color: AppTheme.textPrimary
                                    font.pixelSize: 13
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                }

                                Label {
                                    Layout.alignment: Qt.AlignVCenter

                                    text: plays >= 100 ? "100+" : plays + "×"
                                    color: AppTheme.textMuted
                                    font.pixelSize: 11
                                                                        verticalAlignment: Text.AlignVCenter
                                }
                            }

                            MouseArea {
                                id: artistRowArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "Нет данных — послушайте несколько треков"
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        visible: root.stats === null ||
                            root.stats.topArtists(20).length === 0
                    }
                }
            }
            }

            Item {
                width: 1
                height: 20
            }
        }
    }
}