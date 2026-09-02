import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller


    // =============================================================
    // Genre controller
    // =============================================================

    readonly property var genreController:
            controller !== null &&
        controller !== undefined
        ? controller.genreController
        : null


    // =============================================================
    // Genre model
    // =============================================================

    readonly property var genreModel:
            genreController !== null &&
        genreController !== undefined
        ? genreController.model
        : null


    // =============================================================
    // Page size
    // =============================================================

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


    // =============================================================
    // Initial data loading
    // =============================================================

    Component.onCompleted: {
        if (
            root.genreController !== null &&
            root.genreController !== undefined
        ) {
            root.genreController.loadGenres()
        }
    }


    // =============================================================
    // Content
    // =============================================================

    Column {
        id: content

        width:
            parent.width

        spacing:
            24


        // =========================================================
        // Header
        // =========================================================

        Text {
            width:
                parent.width

            text:
                qsTr("Жанры")

            color:
                AppTheme.textPrimary

            font.pixelSize:
                28

            font.bold:
                true
        }


        // =========================================================
        // Genres
        // =========================================================

        GridView {
            id: genreGrid

            width:
                parent.width

            height:
                contentHeight

            cellWidth:
                220

            cellHeight:
                180

            interactive:
                false

            clip:
                false

            model:
                root.genreModel


            delegate: Item {
                width:
                    genreGrid.cellWidth

                height:
                    genreGrid.cellHeight


                Rectangle {
                    id: card

                    anchors.fill:
                        parent

                    anchors.margins:
                        6

                    radius:
                        14

                    color:
                        cardColor

                    clip:
                        true


                    // =================================================
                    // Artwork
                    // =================================================

                    Image {
                        id: genreImage

                        anchors.fill:
                            parent

                        source:
                                image300 !== undefined &&
                            image300 !== null &&
                            image300.length > 0
                            ? "image://yandex/" +
                            image300
                            : ""

                        sourceSize:
                            Qt.size(
                                300,
                                300
                            )

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous:
                            true

                        cache:
                            true

                        smooth:
                            true

                        visible:
                            status === Image.Ready
                    }


                    // =================================================
                    // Artwork overlay
                    // =================================================

                    Rectangle {
                        anchors.fill:
                            parent

                        radius:
                            card.radius

                        color:
                            "#000000"

                        opacity:
                                genreImage.status === Image.Ready
                            ? 0.30
                            : 0.0
                    }


                    // =================================================
                    // Title
                    // =================================================

                    Text {
                        anchors.left:
                            parent.left

                        anchors.right:
                            parent.right

                        anchors.bottom:
                            parent.bottom

                        anchors.leftMargin:
                            14

                        anchors.rightMargin:
                            14

                        anchors.bottomMargin:
                            14

                        text:
                            title

                        color:
                            "#ffffff"

                        font.pixelSize:
                            18

                        font.bold:
                            true

                        elide:
                            Text.ElideRight
                    }


                    // =================================================
                    // Hover
                    // =================================================

                    Rectangle {
                        anchors.fill:
                            parent

                        radius:
                            card.radius

                        color:
                            "#ffffff"

                        opacity:
                            mouseArea.containsMouse
                                ? 0.08
                                : 0.0


                        Behavior on opacity {
                            NumberAnimation {
                                duration:
                                    120
                            }
                        }
                    }


                    // =================================================
                    // Click
                    // =================================================

                    MouseArea {
                        id: mouseArea

                        anchors.fill:
                            parent

                        hoverEnabled:
                            true

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: {
                            // Открытие жанра добавим следующим этапом.
                        }
                    }
                }


                // =================================================
                // Card fallback color
                // =================================================

                property color cardColor:
                        typeof color === "string" &&
                    color.length > 0
                    ? color
                    : genreId === "all"
                        ? "#6A5ACD"
                        : genreId === "audiobooks"
                            ? "#B86B35"
                            : genreId === "fiction"
                                ? "#76509A"
                                : AppTheme.panelSecondary
            }
        }
    }
}