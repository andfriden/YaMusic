import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Страница жанров: сетка жанровых карточек с цветной обложкой.
 * Клик по жанру испускает genreRequested — навигацией занимается
 * MainLayout (страница не знает про NavigationController).
 */
Item {
    id: root

    property var controller

    // -------------------------------------------------------------
    // Состояние
    // -------------------------------------------------------------

    readonly property var genreController:
        root.controller && root.controller.genreController
            ? root.controller.genreController
            : null

    readonly property var genreModel:
        root.genreController ? root.genreController.model : null

    // -------------------------------------------------------------
    // Геометрия сетки
    // -------------------------------------------------------------

    readonly property int margin: 24
    readonly property int spacing: 16
    readonly property int columns: 5
    readonly property int cardHeight: 180

    readonly property int cardWidth:
        Math.floor(
            (root.width - root.margin * 2 - root.spacing * (root.columns - 1)) /
            root.columns
        )

    width: parent ? parent.width : 0
    implicitWidth: width
    implicitHeight: contentColumn.implicitHeight + root.margin * 2
    height: implicitHeight

    signal genreRequested(string genreId, string title, string image, string color, var subGenres)

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    Column {
        id: contentColumn

        x: root.margin
        y: root.margin

        width: Math.max(0, root.width - root.margin * 2)
        spacing: 20

        // Заголовок
        Label {
            width: parent.width

            text: "Жанры"
            color: AppTheme.textPrimary
            font.pixelSize: 28
            font.bold: true
            elide: Text.ElideRight
        }

        // Пустое состояние / загрузка
        Label {
            width: parent.width

            visible: genreRepeater.count === 0

            text: root.genreController !== null && root.genreController !== undefined &&
                root.genreController.loading
                ? "Загрузка жанров..."
                : "Жанры недоступны"

            color: AppTheme.textSecondary
            font.pixelSize: 16

            horizontalAlignment: Text.AlignHCenter
        }

        // Сетка жанров
        Grid {
            id: genreGrid

            visible: genreRepeater.count > 0

            width: parent.width
            columns: root.columns

            rowSpacing: root.spacing
            columnSpacing: root.spacing

            Repeater {
                id: genreRepeater

                model: root.genreModel

                delegate: Rectangle {
                    width: root.cardWidth
                    height: root.cardHeight

                    radius: 14
                    clip: true

                    color: model.color && model.color.length > 0
                        ? model.color
                        : AppTheme.panelSecondary

                    // Обложка
                    Image {
                        anchors.fill: parent

                        source: model.image300 && model.image300.length > 0
                            ? "image://yandex/" + model.image300
                            : ""

                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true
                        cache: true
                    }

                    // Затемнение для читаемости текста
                    Rectangle {
                        anchors.fill: parent
                        color: "#40000000"
                    }

                    // Градиент снизу
                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom

                        height: 90

                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "transparent" }
                            GradientStop { position: 1.0; color: "#CC000000" }
                        }
                    }

                    // Название жанра
                    Label {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: 14

                        text: model.title || ""
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true

                        elide: Text.ElideRight
                        maximumLineCount: 2
                        wrapMode: Text.Wrap
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            const id = String(model.genreId || "").trim()

                            if (id.length === 0) {
                                return
                            }

                            root.genreRequested(
                                id,
                                model.title || "",
                                model.image300 || "",
                                model.color || "",
                                model.subGenres || []
                            )
                        }

                        onEntered: parent.opacity = 0.85
                        onExited: parent.opacity = 1.0
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (root.genreController !== null && root.genreController !== undefined) {
            root.genreController.loadGenres()
        }
    }
}