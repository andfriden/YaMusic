import QtQuick

Item {
    id: root

    property string text: ""
    property string entityId: ""
    property string entityType: ""
    property var controller

    width: linkText.implicitWidth
    height: linkText.implicitHeight


    Text {
        id: linkText

        text:
            root.text

        color:
            mouseArea.containsMouse
                ? AppTheme.accent
                : AppTheme.textSecondary

        font.underline:
            mouseArea.containsMouse


        MouseArea {
            id: mouseArea

            anchors.fill:
                parent

            hoverEnabled:
                true

            enabled:
                root.entityId.length > 0 &&
                root.controller !== null &&
                root.controller !== undefined

            cursorShape:
                enabled
                    ? Qt.PointingHandCursor
                    : Qt.ArrowCursor


            onClicked: {

                if (
                    root.entityType === "artist"
                ) {
                    root.controller.loadArtist(
                        root.entityId
                    )
                }
                else if (
                    root.entityType === "album"
                ) {
                    root.controller.loadAlbum(
                        root.entityId
                    )
                }
            }
        }
    }
}