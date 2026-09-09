import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.centerIn: parent

        width: Math.min(parent.width * 0.8, 500)

        spacing: 20

        Label {
            Layout.alignment: Qt.AlignHCenter

            text: "Вход в YaMusic"

            font.pixelSize: 28
            font.bold: true

            color: AppTheme.textPrimary
        }

        Label {
            Layout.fillWidth: true

            text: "Войдите через Яндекс и скопируйте адрес страницы после авторизации."

            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap

            color: AppTheme.textSecondary
        }

        Button {
            Layout.alignment: Qt.AlignHCenter

            text: "Войти через Яндекс"

            onClicked: {
                Qt.openUrlExternally(
                    "https://oauth.yandex.ru/authorize" +
                    "?response_type=token" +
                    "&client_id=23cabbbdc6cd418abb4b39c32c41195d"
                )
            }
        }

        Label {
            Layout.fillWidth: true

            text: "После входа скопируйте всю строку из адресной строки браузера и вставьте её сюда."

            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap

            color: AppTheme.textSecondary
        }

        TextField {
            id: tokenField

            Layout.fillWidth: true

            placeholderText: "Вставьте токен или URL целиком"

            onTextChanged: {
                errorLabel.text = ""
            }
        }

        Button {
            Layout.alignment: Qt.AlignHCenter

            text: "Продолжить"

            enabled: tokenField.text.trim().length > 0

            onClicked: {
                authController.loginWithToken(
                    tokenField.text
                )
            }
        }

        Label {
            id: errorLabel

            Layout.fillWidth: true

            visible: text.length > 0

            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap

            color: AppTheme.textSecondary
        }
    }

    Connections {
        target: authController

        function onErrorOccurred(message) {
            errorLabel.text = message
        }
    }
}