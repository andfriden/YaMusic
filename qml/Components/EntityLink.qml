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

    // Не называем свойство "enabled" — оно перекрыло бы базовое
    // Item.enabled и Qt выдал бы warning о переопределении
    readonly property bool linkEnabled:
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
            enabled: root.linkEnabled
            cursorShape: root.linkEnabled ? Qt.PointingHandCursor : Qt.ArrowCursor

            onClicked: {
                if (!root.linkEnabled) {
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