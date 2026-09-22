import QtQuick
import YaMusic 1.0

/*
 * Кликабельная ссылка на сущность (исполнитель/альбом).
 * При наличии entityId и controller открывает страницу.
 */
Item {
    id: root

    property string text: ""
    property string entityId: ""
    property string entityType: ""
    property var controller

    width: linkText.implicitWidth
    height: linkText.implicitHeight

    readonly property bool enabled:
        root.entityId.length > 0 &&
        root.controller !== null && root.controller !== undefined

    Text {
        id: linkText

        text: root.text

        color: mouseArea.containsMouse ? AppTheme.accent : AppTheme.textSecondary
        font.underline: mouseArea.containsMouse

        MouseArea {
            id: mouseArea

            anchors.fill: parent

            hoverEnabled: true
            enabled: root.enabled
            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

            onClicked: {
                if (!root.enabled) {
                    return
                }

                if (root.entityType === "artist") {
                    root.controller.loadArtist(root.entityId)
                } else if (root.entityType === "album") {
                    root.controller.loadAlbum(root.entityId)
                }
            }
        }
    }
}