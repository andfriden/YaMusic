import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root


    // =============================================================
    // Properties
    // =============================================================

    property var controller

    property string genreId:
        ""

    property string genreTitle:
        ""

    property string genreImage:
        ""

    property string genreColor:
        ""

    property var genreSubGenres:
        []


    // =============================================================
    // Layout
    // =============================================================

    readonly property int margin:
        24

    readonly property int cardWidth:
        220

    readonly property int cardHeight:
        150

    readonly property int spacing:
        16

    readonly property int columns:
        Math.max(
            1,
            Math.floor(
                (
                    width -
                    root.margin * 2 +
                    root.spacing
                ) /
                (
                    root.cardWidth +
                    root.spacing
                )
            )
        )


    height:
        Math.max(
            parent
                ? parent.height
                : 0,

            contentColumn.height +
            root.margin * 2
        )


    // =============================================================
    // Navigation
    // =============================================================

    signal subGenreRequested(
        string subGenreId,
        string title,
        string color
    )


    // =============================================================
    // Background
    // =============================================================

    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    // =============================================================
    // Content
    // =============================================================

    Column {
        id: contentColumn

        x:
            root.margin

        y:
            root.margin

        width:
            Math.max(
                0,
                root.width -
                root.margin * 2
            )

        spacing:
            24


        // =========================================================
        // Genre header
        // =========================================================

        Rectangle {
            width:
                parent.width

            height:
                220

            radius:
                16

            clip:
                true

            color:
                    root.genreColor.length > 0
                ? root.genreColor
                : AppTheme.panelSecondary


            Image {
                anchors.fill:
                    parent

                source:
                        root.genreImage.length > 0
                    ? root.genreImage
                    : ""

                fillMode:
                    Image.PreserveAspectCrop

                asynchronous:
                    true

                cache:
                    true
            }


            Rectangle {
                anchors.fill:
                    parent

                color:
                    "#50000000"
            }


            Rectangle {
                anchors.left:
                    parent.left

                anchors.right:
                    parent.right

                anchors.bottom:
                    parent.bottom

                height:
                    120

                gradient:
                    Gradient {
                        GradientStop {
                            position:
                                0.0

                            color:
                                "transparent"
                        }

                        GradientStop {
                            position:
                                1.0

                            color:
                                "#D9000000"
                        }
                    }
            }


            Label {
                anchors.left:
                    parent.left

                anchors.right:
                    parent.right

                anchors.bottom:
                    parent.bottom

                anchors.leftMargin:
                    20

                anchors.rightMargin:
                    20

                anchors.bottomMargin:
                    18

                text:
                    root.genreTitle

                color:
                    "#FFFFFF"

                font.pixelSize:
                    30

                font.bold:
                    true

                elide:
                    Text.ElideRight
            }
        }


        // =========================================================
        // Subgenres title
        // =========================================================

        Label {
            width:
                parent.width

            text:
                "Поджанры"

            color:
                AppTheme.textPrimary

            font.pixelSize:
                24

            font.bold:
                true

            visible:
                subGenreRepeater.count > 0
        }


        // =========================================================
        // Subgenres
        // =========================================================

        Grid {
            id: subGenreGrid

            width:
                parent.width

            columns:
                root.columns

            rowSpacing:
                root.spacing

            columnSpacing:
                root.spacing

            visible:
                subGenreRepeater.count > 0


            Repeater {
                id: subGenreRepeater

                model:
                    root.genreSubGenres


                delegate:
                    Rectangle {
                        width:
                            root.cardWidth

                        height:
                            root.cardHeight

                        radius:
                            14

                        clip:
                            true

                        color:
                                modelData.color &&
                            modelData.color.length > 0
                            ? modelData.color
                            : AppTheme.panelSecondary


                        // =================================================
                        // Artwork
                        // =================================================

                        Image {
                            anchors.fill:
                                parent

                            source:
                                    modelData.image300 &&
                                modelData.image300.length > 0
                                ? modelData.image300
                                : ""

                            fillMode:
                                Image.PreserveAspectCrop

                            asynchronous:
                                true

                            cache:
                                true
                        }


                        // =================================================
                        // Overlay
                        // =================================================

                        Rectangle {
                            anchors.fill:
                                parent

                            color:
                                "#45000000"
                        }


                        Rectangle {
                            anchors.left:
                                parent.left

                            anchors.right:
                                parent.right

                            anchors.bottom:
                                parent.bottom

                            height:
                                80

                            gradient:
                                Gradient {
                                    GradientStop {
                                        position:
                                            0.0

                                        color:
                                            "transparent"
                                    }

                                    GradientStop {
                                        position:
                                            1.0

                                        color:
                                            "#CC000000"
                                    }
                                }
                        }


                        // =================================================
                        // Title
                        // =================================================

                        Label {
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
                                12

                            text:
                                modelData.title || ""

                            color:
                                "#FFFFFF"

                            font.pixelSize:
                                17

                            font.bold:
                                true

                            elide:
                                Text.ElideRight
                        }


                        // =================================================
                        // Click
                        // =================================================

                        MouseArea {
                            anchors.fill:
                                parent

                            hoverEnabled:
                                true

                            cursorShape:
                                Qt.PointingHandCursor


                            onEntered: {
                                parent.opacity =
                                    0.85
                            }


                            onExited: {
                                parent.opacity =
                                    1.0
                            }


                            onClicked: {
                                const id =
                                    String(
                                        modelData.id || ""
                                    ).trim()

                                if (
                                    id.length === 0
                                ) {
                                    return
                                }


                                root.subGenreRequested(
                                    id,
                                    modelData.title || "",
                                    modelData.color || ""
                                )
                            }
                        }
                    }
            }
        }


        // =========================================================
        // Empty state
        // =========================================================

        Label {
            width:
                parent.width

            visible:
                subGenreRepeater.count === 0

            text:
                "У этого жанра нет поджанров"

            color:
                AppTheme.textSecondary

            font.pixelSize:
                16
        }
    }


    // =============================================================
    // Debug
    // =============================================================

    Component.onCompleted: {
        console.log(
            "GenrePage:",
            root.genreId,
            root.genreTitle,
            "subGenres:",
            root.genreSubGenres.length
        )
    }
}