import QtQuick 2.2
import QtQuick.Controls 2.2

Rectangle {
    color: "white"
    TabBar {
        id: tabBar
        anchors.top: parent.top
        width: parent.width
        height: parent.height * 0.1
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Create Deal")
        }
        TabButton {
            text: qsTr("Deal Manager")
        }
        TabButton {
            text: qsTr("Setting")
        }
    }

    SwipeView {
        id: swipeView
        anchors.top: tabBar.bottom
        width: parent.width
        height: parent.height - tabBar.height
        currentIndex: tabBar.currentIndex

        CreateDeal {
            width: swipeView.width
            height: swipeView.height
        }

        DealManger {
            width: swipeView.width
            height: swipeView.height
        }

        Setting {
            width: swipeView.width
            height: swipeView.height
        }
    }
}
