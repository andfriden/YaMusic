import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    signal chartRequested()

    width:
        parent
            ? parent.width
            : 0

    height:
        42


    Button {
        anchors.left:
            parent.left

        anchors.verticalCenter:
            parent.verticalCenter

        text:
            qsTr("Чарты")

        onClicked:
            root.chartRequested()
    }
}