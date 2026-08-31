import QtQuick

Item {
    id: root

    property var controller

    readonly property var chartController:
        controller !== null &&
        controller !== undefined
            ? controller.chartController
            : null

    readonly property var russiaModel:
        chartController !== null &&
        chartController !== undefined
            ? chartController.russiaModel
            : null

    readonly property var worldModel:
        chartController !== null &&
        chartController !== undefined
            ? chartController.worldModel
            : null


    width:
        parent
            ? parent.width
            : 0

    implicitWidth:
        width

    implicitHeight:
        content.implicitHeight

    height:
        implicitHeight


    Component.onCompleted: {
        if (
            root.chartController !== null &&
            root.chartController !== undefined
        ) {
            root.chartController.loadCharts()
        }
    }


    Column {
        id: content

        width:
            parent.width

        spacing:
            28


        // =========================================================
        // Заголовок
        // =========================================================

        Text {
            width:
                parent.width

            text:
                qsTr("Чарты")

            color:
                AppTheme.textPrimary

            font.pixelSize:
                28

            font.bold:
                true
        }


        // =========================================================
        // Россия + Мир
        // =========================================================

        Row {
            width:
                parent.width

            spacing:
                20


            // =====================================================
            // Россия
            // =====================================================

            Column {
                width:
                    (parent.width - parent.spacing) / 2

                spacing:
                    10


                Text {
                    width:
                        parent.width

                    text:
                        qsTr("Россия")

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        20

                    font.bold:
                        true
                }


                ListView {
                    id: russiaList

                    width:
                        parent.width

                    height:
                        5 * 72 + 4 * 6

                    spacing:
                        6

                    clip:
                        true

                    interactive:
                        true

                    boundsBehavior:
                        Flickable.StopAtBounds

                    model:
                        root.russiaModel


                    delegate: ChartTrackRow {
                        width:
                            russiaList.width

                        controller:
                            root.chartController

                        chartType:
                            "russia"

                        trackIndex:
                            index
                    }
                }
            }


            // =====================================================
            // Мир
            // =====================================================

            Column {
                width:
                    (parent.width - parent.spacing) / 2

                spacing:
                    10


                Text {
                    width:
                        parent.width

                    text:
                        qsTr("Мир")

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        20

                    font.bold:
                        true
                }


                ListView {
                    id: worldList

                    width:
                        parent.width

                    height:
                        5 * 72 + 4 * 6

                    spacing:
                        6

                    clip:
                        true

                    interactive:
                        true

                    boundsBehavior:
                        Flickable.StopAtBounds

                    model:
                        root.worldModel


                    delegate: ChartTrackRow {
                        width:
                            worldList.width

                        controller:
                            root.chartController

                        chartType:
                            "world"

                        trackIndex:
                            index
                    }
                }
            }
        }
    }
}