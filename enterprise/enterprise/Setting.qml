import QtQuick 2.0

AbstractScreen {
    CustomizeButton {
        id: logout
        width: parent.width * 0.1
        height: parent.height * 0.1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        text: "Logout"

        onControlDownChanged: {
            if (controlDown == true)
                account.onLogout()

            appWindow.currentScreenIndex = appWindow.loginIndex
        }
    }
}
