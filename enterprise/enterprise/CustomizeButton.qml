import QtQuick 2.0

AbstractScreen {
    id: root
    property bool controlDown: false
    property var text
    border.color: controlDown ? "#17a81a" : "#21be2b"
    opacity:  controlDown ? 0.6 : 1
    radius: 2

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: root.height * 0.3
        color: controlDown ?  "#17a81a" : "#21be2b"
        text: root.text
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.controlDown = false
        }

        onPressed: {
            root.controlDown = true
        }

    }
}
