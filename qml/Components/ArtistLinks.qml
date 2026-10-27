import QtQuick
import YaMusic 1.0

// Список исполнителей трека, каждый — отдельная ссылка на свою страницу.
// Используется в плеере, где у трека может быть несколько артистов.
Row {
    id: root

    property var artists: []          // [{"id": ..., "name": ...}]
    property var controller
    property color baseColor: AppTheme.textSecondary
    property int pixelSize: 12

    spacing: 0

    Repeater {
        model: root.artists

        delegate: Row {
            width: implicitWidth
            height: implicitHeight
            spacing: 0

            EntityLink {
                text: String(modelData.name || "")
                entityId: String(modelData.id || "")
                entityType: "artist"
                controller: root.controller
                baseColor: root.baseColor
                font.pixelSize: root.pixelSize
            }

            Text {
                // Запятая между артистами, после последнего не нужна.
                visible: index < root.artists.length - 1
                text: ", "
                color: root.baseColor
                font.pixelSize: root.pixelSize
            }
        }
    }
}
