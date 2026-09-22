import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Карточка плейлиста/альбома для сеток.
 *
 * Заменяет три почти одинаковые копии (PlaylistsPage, GenrePage,
 * SportPage). Отличия:
 *   - coverUri/title/trackCount и подпись под обложкой
 *   - activated(): клик по карточке — родитель сам выбирает
 *     selectPersonalPlaylist() или loadAlbum()
 */
Item {
    id: root

    property var controller
    property string coverUri: ""
    property string title: ""
    property int trackCount: 0
    property int year: 0
    property string subtitle: ""

    property int cardWidth: 180
    property int cardHeight: 0
    property int cornerRadius: 10

    signal activated()

    width: root.cardWidth

    // Высота по умолчанию считается от ширины: квадратная обложка
    // (ширина - 16) + 8+8 отступы + 18 название + 3 + 16 подпись.
    // Передаётся cardHeight > 0 — используется он.
    height: root.cardHeight > 0
        ? root.cardHeight
        : Math.round(root.cardWidth + 45)

    Rectangle {
        anchors.fill: parent
        radius: root.cornerRadius

        color: mouseArea.containsMouse
            ? AppTheme.panelActive
            : AppTheme.panelSecondary

        border.width: 1
        border.color: mouseArea.containsMouse
            ? AppTheme.border
            : AppTheme.borderSubtle

        Behavior on color {
            ColorAnimation { duration: 140; easing.type: Easing.OutCubic }
        }

        // ---------------------------------------------------------
        // Обложка
        // ---------------------------------------------------------

        Rectangle {
            id: artworkBox

            x: 8
            y: 8
            width: parent.width - 16
            height: width
            radius: 8

            color: AppTheme.panelHover
            clip: true

            Image {
                id: cover

                anchors.fill: parent

                source: root.coverUri.length > 0
                    ? "image://yandex/" + root.coverUri
                    : ""

                sourceSize: Qt.size(artworkBox.width * 2, artworkBox.height * 2)

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
                font.pixelSize: 42

                visible: cover.status !== Image.Ready
            }

            // Обводка поверх обложки, чтобы скругление не "съедалось"
            // краем изображения
            Rectangle {
                anchors.fill: parent
                radius: 8

                color: "transparent"
                border.width: 1
                border.color: mouseArea.containsMouse
                    ? AppTheme.border
                    : AppTheme.borderSubtle
            }
        }

        // ---------------------------------------------------------
        // Название
        // ---------------------------------------------------------

        Label {
            id: titleLabel

            x: 8
            width: parent.width - 16

            anchors.top: artworkBox.bottom
            anchors.topMargin: 8

            height: 18

            text: root.title.length > 0 ? root.title : qsTr("Плейлист")

            color: AppTheme.textPrimary
            font.pixelSize: 13
            font.bold: true

            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            maximumLineCount: 1
        }

        // ---------------------------------------------------------
        // Подпись (кол-во треков или год)
        // ---------------------------------------------------------

        Label {
            id: countLabel

            x: 8
            width: parent.width - 16

            anchors.top: titleLabel.bottom
            anchors.topMargin: 3

            height: 16

            text: root.subtitle.length > 0
                ? root.subtitle
                : root.trackCount > 0
                    ? qsTr("%1 треков").arg(root.trackCount)
                    : ""

            color: AppTheme.textSecondary
            font.pixelSize: 11

            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            maximumLineCount: 1

            visible: text.length > 0
        }

        // ---------------------------------------------------------
        // Клик
        // ---------------------------------------------------------

        MouseArea {
            id: mouseArea

            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onClicked: root.activated()
        }
    }
}