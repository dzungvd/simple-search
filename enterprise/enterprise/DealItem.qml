import QtQuick 2.0
import QtQuick.Controls 2.2

AbstractScreen {
    border.color: "grey"
    radius: 2

    property string timeTxt: ""
    property string priceTxt: ""
    property string keywordList: ""
    property int acceptValue: 0
    property int ignoreValue: 0
    property int totalValue: 0
    property var dealOwnerData: undefined

    property bool controlDown: false
    property string controlDownPressColor: "#F4F3F2"
    property string controlDownClickColor: "white"

    color: controlDownClickColor

    onDealOwnerDataChanged: {
        if (dealOwnerData === undefined)
            return;

        console.log ("onDealOwnerDataChanged " + dealOwnerData);
        // get external data
        if (dealOwnerData["accept"]) // const string ???
            acceptValue = dealOwnerData["accept"]
            ignoreValue = dealOwnerData["ignore"]
            totalValue  = dealOwnerData["total"]

        dealOwnerFlow.refeshList()
    }

    Item {
        id: dealBlock
        width: parent.width * 0.97
        height: parent.height * 0.97
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        property real spacing: 5
        property real itemHeight: (height - (children.length-1)*spacing)/children.length

        Item {
            id: dealTimeBlock
            width: parent.width
            height: dealBlock.itemHeight

            Text {
                id: dealTimelb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Deal Time: ")
            }

            Text {
                id: dealTime
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(dealTimelb)
                text: timeTxt
            }
        }

        Item {
            id: priceBlock
            width: parent.width
            height: dealBlock.itemHeight
            y: getNextBottomPosition(dealTimeBlock,dealBlock.spacing)

            Text {
                id: pricelb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Price: ")
            }

            Text {
                id: price
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(pricelb)
                text: priceTxt
            }
        }

        Item {
            id: keywordBlock
            width: parent.width
            height: dealBlock.itemHeight
            y: getNextBottomPosition(priceBlock,dealBlock.spacing)

            Text {
                id: keywordlb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Keywords: ")
            }

            Text {
                id: keywordTxt
                width: getRemainWidth(keywordBlock, keywordlb, 5)
                height: dealBlock.itemHeight
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(keywordlb)
                text: keywordList
            }
        }

        Item {
            id: acceptNumBlock
            width: parent.width
            height: dealBlock.itemHeight
            y: getNextBottomPosition(keywordBlock,dealBlock.spacing)

            Text {
                id: acceptNumlb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Accept: ")
            }

            Text {
                id: acceptNumTxt
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(acceptNumlb)
                text: qsTr("" + acceptValue)
            }
        }

        Item {
            id: ignoreNumBlock
            width: parent.width
            height: dealBlock.itemHeight
            y: getNextBottomPosition(acceptNumBlock,dealBlock.spacing)

            Text {
                id: ignoreNumLb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Ignore: ")
            }

            Text {
                id: ignoreNumTxt
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(ignoreNumLb)
                text: qsTr("" + ignoreValue)
            }
        }

        Item {
            id: totalBlock
            width: parent.width
            height: dealBlock.itemHeight
            y: getNextBottomPosition(ignoreNumBlock,dealBlock.spacing)

            Text {
                id: totalLb
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Total: ")
            }

            Text {
                id: total
                anchors.verticalCenter: parent.verticalCenter
                x: getNextRightPosition(totalLb)
                text: qsTr("" + totalValue)
            }
        }
    }


    Item {
        id: dealOwnerBlock
        width: parent.width * 0.97
        height: parent.height * 0.97
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: false

        Flickable {
            width: parent.width
            height: parent.height
            contentHeight: dealOwnerFlow.height

            Item {
                id: dealOwnerFlow
                width: parent.width
                height: continuousHeight

                property real continuousHeight: 0
                property real commonheight: 30
                property int spacing: 5
                property var component: Qt.createComponent("qrc:/CustomizeLabel.qml")

                function refeshList () {
                    // remove old list element
                    while (children.length) {
                        children[0].destroy();
                    }

                    var list = dealOwnerData["data"]

                    for (var i in list) {
                        // add new keyword to List
                        var label = component.createObject(dealOwnerFlow);
                        label.text = list[i].owner_address
                        label.height = commonheight

                        if (children.length > 1) {
                            label.y = getNextBottomPosition(children[children.length-1], spacing)
                        }
                    }

                    continuousHeight = children.length * commonheight  + spacing * (children.length -1)
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            controlDown = true
            parent.color = controlDownClickColor

            dealBlock.visible  = !dealBlock.visible;
            dealOwnerBlock.visible = !dealOwnerBlock.visible;
        }

        onPressed: {
            controlDown = false
            parent.color = controlDownPressColor
        }
    }
}
