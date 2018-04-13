import QtQuick 2.0
import QtQuick.Controls 2.2

Rectangle {
    id: root
    color: "#009688"
    property var text: ""
    property bool destroyWhenDoubleClick: false

    signal selfRemove(var item);
    signal leftClick(var item);

    onTextChanged: {
       lb.text = text

       if (text == "") {
            width = 0
            radius = 0
       }
       else {
            width = lb.width  + 10
            radius = 5
       }
    }

    Text {
        id: lb
        height: parent.height
        font.pixelSize: 16
        text: text
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        anchors.fill: parent

        onDoubleClicked: {
            if (!destroyWhenDoubleClick)
                return;

            selfRemove(root)
        }

        onClicked: {
            leftClick(root)
        }
    }
}
