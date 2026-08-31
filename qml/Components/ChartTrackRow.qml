import QtQuick

Item {
    id: root

    property var controller
    property string chartType: ""
    property int trackIndex: -1

    width:
        parent
            ? parent.width
            : 0

    height:
        72


    Rectangle {
        anchors.fill:
            parent

        radius:
            8

        color:
            mouseArea.containsMouse
                ? AppTheme.panelHover
                : "transparent"


        Row {
            anchors.fill:
                parent

            anchors.leftMargin:
                8

            anchors.rightMargin:
                8

            spacing:
                12


            Text {
                width:
                    30

                anchors.verticalCenter:
                    parent.verticalCenter

                text:
                    String(position || index + 1)

                color:
                    AppTheme.textSecondary

                font.pixelSize:
                    15

                font.bold:
                    true

                horizontalAlignment:
                    Text.AlignHCenter
            }


            Rectangle {
                width:
                    56

                height:
                    56

                anchors.verticalCenter:
                    parent.verticalCenter

                radius:
                    8

                color:
                    AppTheme.panelHover

                clip:
                    true


                Image {
                    anchors.fill:
                        parent

                    source:
                            coverUri &&
                        String(coverUri).length > 0
                        ? "image://yandex/" +
                        String(coverUri)
                        : ""

                    fillMode:
                        Image.PreserveAspectCrop

                    asynchronous:
                        true

                    cache:
                        true

                    smooth:
                        true
                }
            }


            Column {
                width:
                    parent.width -
                    110

                anchors.verticalCenter:
                    parent.verticalCenter

                spacing:
                    3


                Text {
                    width:
                        parent.width

                    text:
                        title || ""

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        14

                    font.bold:
                        true

                    elide:
                        Text.ElideRight

                    maximumLineCount:
                        1
                }


                Text {
                    width:
                        parent.width

                    text:
                        artist || ""

                    color:
                        AppTheme.textSecondary

                    font.pixelSize:
                        12

                    elide:
                        Text.ElideRight

                    maximumLineCount:
                        1
                }
            }
        }


        MouseArea {
            id: mouseArea

            anchors.fill:
                parent

            hoverEnabled:
                true

            cursorShape:
                Qt.PointingHandCursor

            onClicked: {
                if (
                    root.controller !== null &&
                    root.controller !== undefined &&
                    root.chartType.length > 0 &&
                    root.trackIndex >= 0
                ) {
                    root.controller.selectTrack(
                        root.chartType,
                        root.trackIndex
                    )
                }
            }
        }
    }
}