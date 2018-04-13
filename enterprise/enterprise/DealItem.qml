import QtQuick 2.0
import QtQuick.Controls 2.2

AbstractScreen {
    border.color: "grey"
    radius: 2

    property var dealTimeTxt: ""
    property var ownerAddressTxt: ""
    property var ownerDocIdTxt: ""
    property var elasticIdTxt: ""
    property var statusTxt: ""


    Item {
        id: rootBlock
        width: parent.width * 0.97
        height: parent.height * 0.97
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        property real spacing: 5
        property real itemHeight: (height - spacing*5)/6

        Item {
            id: dealTimeBlock
            width: parent.width
            height: rootBlock.itemHeight

            Text {
                id: dealTimelb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Deal Time: ")
            }

            Text {
                id: dealTime
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(dealTimelb)
                text: dealTimeTxt
            }
        }

        Item {
            id: ownerAddressBlock
            width: parent.width
            height: rootBlock.itemHeight
            y: getNextBottomPosition(dealTimeBlock,rootBlock.spacing)

            Text {
                id: ownerAddresslb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Owner Address: ")
            }

            Text {
                id: ownerAddress
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(ownerAddresslb)
                text: ownerAddressTxt
            }
        }

        Item {
            id: ownerDocIdBlock
            width: parent.width
            height: rootBlock.itemHeight
            y: getNextBottomPosition(ownerAddressBlock,rootBlock.spacing)

            Text {
                id: ownerdocIdlb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Doc ID: ")
            }

            Text {
                id: ownerdocId
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(ownerdocIdlb)
                text: ownerDocIdTxt
            }
        }

        Item {
            id: elasticIdBlock
            width: parent.width
            height: rootBlock.itemHeight
            y: getNextBottomPosition(ownerDocIdBlock,rootBlock.spacing)

            Text {
                id: elasticIdlb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Elastic ID: ")
            }

            Text {
                id: elasticId
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(elasticIdlb)
                text: elasticIdTxt
            }
        }

        Item {
            id: statusBlock
            width: parent.width
            height: rootBlock.itemHeight
            y: getNextBottomPosition(elasticIdBlock,rootBlock.spacing)

            Text {
                id: statuslb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Status: ")
            }

            Text {
                id: status
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(statuslb)
                text: statusTxt
            }
        }
    }
}
