import QtQuick 2.0
import Manager 1.0


AbstractScreen {
    id: root
    property var dealOwnerList: []
    property var dealOwnerItems: []

    property var dealItemBorderColor: "#5FBA7D"
    property var dealOwnerComponent: Qt.createComponent("DealItem.qml")

    Flickable {
        id:flickable
        width: parent.width * 0.95
        height: parent.height * 0.95
        contentHeight: content.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        clip:true

        property real spacing: 10
        property real itemWidth: (flickable.width - spacing)/2
        property real itemHeight: (flickable.height - spacing)/2

        Flow {
            id: content
            width: parent.width
            height: 0
            flow: Flow.LeftToRight
            spacing: 5
            property real continuousHeight: 0
        }
    }

    DealManager {
        id: dealManager
    }

    onDealOwnerListChanged: {
        refestDealOwnerList()
    }

    Component.onCompleted: {
        dealOwnerList = dealManager.getDealData()
    }

    function refestDealOwnerList() {
        for (var i in dealOwnerItems)
            dealOwnerItems[i].destroy();

        dealOwnerItems = []

        for (var j in dealOwnerList) {
            var dealOwnerItem = dealOwnerComponent.createObject(content);
            var dealOwnerContent = dealOwnerList[j];

            dealOwnerItem.dealTimeTxt = dealOwnerContent.deal_time
            dealOwnerItem.ownerAddressTxt = dealOwnerContent.owner_address
            dealOwnerItem.ownerDocIdTxt = dealOwnerContent.owner_doc_id
            dealOwnerItem.elasticIdTxt = dealOwnerContent.elastic_id
            dealOwnerItem.statusTxt = dealOwnerContent.status

            dealOwnerItem.width = flickable.itemWidth
            dealOwnerItem.height = flickable.itemHeight
            dealOwnerItem.border.color = dealItemBorderColor
        }

        // caculate height of content
        content.height = (flickable.itemHeight + flickable.spacing) * (j/2)

        if (j%2) {
            content.height += (flickable.itemHeight + flickable.spacing)
        }
    }
}
