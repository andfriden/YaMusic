import QtQuick
import QtQuick.Controls.Basic
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

            Column {
                width: parent.width
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

                        model: root.stats ? root.stats.topTracks(20) : []

                        delegate: Item {
                            required property string id
                            required property string title
                            required property string artist
                            required property int plays

                            width: topTracksView.width
                            height: 50

                            Rectangle {
                                anchors.fill: parent
                                radius: 8
                                color: index % 2 === 0 ? AppTheme.panelSecondary : "transparent"
                            }

                            Row {
                                anchors.fill: parent
                                anchors.margins: 8
                                spacing: 10

                                Label {
                                    width: 26
                                    height: parent.height

                                    text: index + 1
                                    color: AppTheme.textMuted
                                    font.pixelSize: 14
                                    font.bold: true
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Column {
                                    width: parent.width - playsLabel.width - 36
                                    height: parent.height

                                    anchors.verticalCenter: parent.verticalCenter

                                    spacing: 2

                                    Label {
                                        width: parent.width
                                        text: title
                                        color: AppTheme.textPrimary
                                        font.pixelSize: 13
                                        font.bold: true
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                    }

                                    Label {
                                        width: parent.width
                                        text: artist
                                        color: AppTheme.textSecondary
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                    }
                                }

                                Label {
                                    id: playsLabel

                                    height: parent.height

                                    text: plays >= 100 ? "100+" : plays + "×"
                                    color: AppTheme.textMuted
                                    font.pixelSize: 12
                                    verticalAlignment: Text.AlignVCenter
                                }
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
                width: parent.width
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

                        model: root.stats ? root.stats.topArtists(20) : []

                        delegate: Item {
                            required property string id
                            required property string name
                            required property int plays

                            width: topArtistsView.width
                            height: 50

                            Rectangle {
                                anchors.fill: parent
                                radius: 8
                                color: index % 2 === 0 ? AppTheme.panelSecondary : "transparent"
                            }

                            Row {
                                anchors.fill: parent
                                anchors.margins: 8
                                spacing: 10

                                Label {
                                    width: 26
                                    height: parent.height

                                    text: index + 1
                                    color: AppTheme.textMuted
                                    font.pixelSize: 14
                                    font.bold: true
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Label {
                                    width: parent.width - playsLabel2.width - 36
                                    height: parent.height

                                    text: name
                                    color: AppTheme.textPrimary
                                    font.pixelSize: 13
                                    font.bold: true
                                    elide: Text.ElideRight
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Label {
                                    id: playsLabel2

                                    height: parent.height

                                    text: plays >= 100 ? "100+" : plays + "×"
                                    color: AppTheme.textMuted
                                    font.pixelSize: 12
                                    verticalAlignment: Text.AlignVCenter
                                }
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

            Item {
                width: 1
                height: 20
            }
        }
    }
}