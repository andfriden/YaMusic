import QtQuick
import QtQuick.Controls.Basic

Label {
    id: root

    property string message: "Готово"

    height: 24

    text: message

    color: "#404040"

    horizontalAlignment:
        Text.AlignHCenter

    verticalAlignment:
        Text.AlignVCenter

    elide:
        Text.ElideRight
}