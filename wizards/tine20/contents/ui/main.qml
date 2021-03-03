// SPDX-FileCopyrightText: 2010 Casey Link <unnamedrambler@gmail.com>
// SPDX-FileCopyrightText: 2012 Volker Krause <vkrause@kde.org>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.pim.accountwizard 1.0

Kirigami.ScrollablePage {
    id: root
    
    // stage 1, setup identity and run imap server test
    // stage 2, smtp setup (no server test) and Dav resources
    property int stage: 1;
    property var identity; // global so it can be accesed in setup and testOk
    property bool userChangedServerAddress: false
    property bool valid: serverAddressField.text.trim().length !== 0 && emailField.text.trim().length !== 0; 
    
    function guessServerName() {
        if (userChangedServerAddress) {
            return;
        }

        const email = emailField.text;
        const pos = email.indexOf("@");
        if (pos >= 0 && (pos + 1) < email.length) {
            const server = email.slice(pos + 1, email.length);
            serverAddressField.text = server;
        }
    }

    function setup() {
        print("setup() "  + stage);
        var serverAddress = serverAddressField.text.trim();
        if (stage === 1) {
            SetupManager.openWallet();

            identity = SetupManager.createIdentity();
            identity.setEmail(emailField.text);
            identity.setRealName(nameField.text);

            ServerTest.test(serverAddress, "imap");
        } else { // stage 2
            var smtp = SetupManager.createTransport("smtp");
            smtp.name = serverAddress;
            smtp.host = serverAddress;
            smtp.port = 25;
            smtp.encryption = "TLS";
            smtp.authenticationType = "plain";
            smtp.username = emailField.text;
            smtp.password = passwordField.password;
            identity.setTransport(smtp);

            var dav = SetupManager.createResource("akonadi_davgroupware_resource");
            print("dav: " + dav );
            try {
                dav.setName("Tine 2.0");
                dav.setOption("DisplayName", "Tine 2.0");
                dav.setOption("RefreshInterval", 60);
                dav.setOption("RemoteUrls", ["$default$|CalDav|https://" + serverAddressField.text.trim() + "/calendars", 
                                "$default$|CardDav|https://" + serverAddressField.text.trim() + "/addressbooks"]);
                dav.setOption("DefaultUsername", emailField.text.trim());
            } catch (e) { print(e); }

            print("pre-exec");
            SetupManager.execute();
            print("post-exec");
        }
    }

    function testResultFail() {
        testOk(-1);
    }

    function testOk(arg) {
        print("testOk arg =", arg);
        const imapRes = SetupManager.createResource("akonadi_imap_resource");
        imapRes.setName(serverAddressField.text.trim());
        imapRes.setOption("ImapServer", serverAddressField.text.trim());
        imapRes.setOption("UserName", emailEditField.text.trim());
        imapRes.setOption("Password", passwordEditField.password.trim());
        imapRes.setOption("UseDefaultIdentity", false);
        imapRes.setOption("AccountIdentity", identity.uoid());
        imapRes.setOption("DisconnectedModeEnabled", true);
        imapRes.setOption("IntervalCheckTime", 60);
        imapRes.setOption("SubscriptionEnabled", true);
        imapRes.setOption("SieveSupport", false);
        imapRes.setOption("Authentication", 7); // ClearText
        if (arg == "ssl") { 
            // The ENUM used for authentication (in the imap resource only)
            imapRes.setOption("Safety", "SSL"); // SSL/TLS
            imapRes.setOption("ImapPort", 993);
        } else if ( arg == "tls" ) { // tls is really STARTTLS
            imapRes.setOption("Safety", "STARTTLS");  // STARTTLS
            imapRes.setOption("ImapPort", 143);
        } else if ( arg == "none" ) {
            imapRes.setOption("Safety", "NONE");  // No encryption
            imapRes.setOption("ImapPort", 143);
        } else {
            // safe default fallback in case server test failed
            imapRes.setOption("Safety", "STARTTLS");
            imapRes.setOption("ImapPort", 143);
        }
        stage = 2;
        setup();
    }


    //validateInput();
    
    Component.onCompleted: {
        try {
            ServerTest.testFail.connect(testResultFail);
            ServerTest.testResult.connect(testOk);
            //page.pageLeftNext.connect(setup);
        } catch (e) {
            print(e);
        }
    }
    
    Kirigami.FormLayout {
        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Personal Settings")
        }
        QQC2.TextField {
            id: nameField
            Kirigami.FormData.label: i18n("Name:")
            text: SetupManager.name
        }
        QQC2.TextField {
            id: emailField
            Kirigami.FormData.label: i18n("Email:")
            onTextChanged: guessServerName()
            text: SetupManager.email
        }
        Kirigami.PasswordField {
            id: passwordField
            Kirigami.FormData.label: i18n("Password:")
            text: SetupManager.password
            onAccepted: userChangedServerAddress = true;
        }
        QQC2.TextField {
            id: serverAddressField
            Kirigami.FormData.label: i18n("Server Address:")
        }
    }
    
    footer: QQC2.ToolBar {
        contentItem: RowLayout {
            QQC2.Button {
                alignment: Layout.AlignRight
                text: i18n("Next")
                enabled: root.valid
                onClicked: setup()
            }
        }
    }
}
