import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    readonly property int pageHeight: 700

    width: parent ? parent.width : 0
    height: pageHeight

    implicitWidth: width
    implicitHeight: pageHeight


    Component.onCompleted: {

        console.log(
            "PlaylistPage loaded",
            controller
        )

        if (controller) {
            controller.loadRecommendations()
        }
    }


    Rectangle {
        anchors.fill: parent

        color: AppTheme.backgroundPrimary
    }


    Column {

        anchors.fill: parent

        anchors.margins: 20

        spacing: 20



        Label {

            text: "Собираем для вас"

            color: AppTheme.textPrimary

            font.pixelSize: 18

            font.bold: true
        }



        ListView {

            width: parent.width

            height: 180


            orientation:
                ListView.Horizontal


            spacing: 14


            model:
                root.controller
                    ? root.controller.personalPlaylistsModel
                    : null



            delegate: Rectangle {

                width: 150

                height: 150

                radius: 12


                color:
                    AppTheme.panel



                Column {

                    anchors.fill: parent

                    anchors.margins: 10

                    spacing: 8



                    Image {

                        width: 100

                        height: 100


                        anchors.horizontalCenter:
                            parent.horizontalCenter



                        source:
                            coverUri
                                ?
                                "image://yandex/" + coverUri
                                :
                                ""



                        fillMode:
                            Image.PreserveAspectCrop
                    }



                    Label {

                        width:
                            parent.width


                        text:
                            title


                        color:
                            AppTheme.textPrimary


                        elide:
                            Text.ElideRight
                    }
                }



                MouseArea {

                    anchors.fill: parent



                    onClicked: {

                        root.controller
                            .selectPersonalPlaylist(
                            index
                        )
                    }
                }
            }
        }



        Label {

            text:
                "Выбранный плейлист"


            color:
                AppTheme.textPrimary


            font.pixelSize:
                18


            font.bold:
                true
        }



        PlaylistView {

            width:
                parent.width


            height:
                350


            controller:
                root.controller
        }
    }
}