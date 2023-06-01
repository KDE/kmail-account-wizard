/*
    SPDX-FileCopyrightText: 2010 Omat Holding B.V. <info@omat.nl>
    SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include <KIO/Job>
#include <KMime/HeaderParsing>
#include <QObject>
#include <QUrl>

class QDomElement;
class QDomDocument;

struct Server;
struct Identity;

/// This class will search in Mozilla's database for an xml file
/// describing the isp data belonging to that address.
///
/// This class searches and wraps the result for further usage.
///
/// References:
/// * https://wiki.mozilla.org/Thunderbird:Autoconfiguration
/// * https://developer.mozilla.org/en/Thunderbird/Autoconfiguration
/// * https://ispdb.mozillamessaging.com/
class Ispdb : public QObject
{
    Q_OBJECT

    /// This property holds the email of the user.
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)

    /// This property holds the password of the user.
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

public:
    enum SocketType {
        None = 0,
        SSL,
        StartTLS,
    };
    Q_ENUM(SocketType);

    /// Ispdb uses custom authtyps, hence the enum here.
    /// @see https://wiki.mozilla.org/Thunderbird:Autoconfiguration:ConfigFileFormat
    /// In particular, note that Ispdb's Plain represents both Cleartext and AUTH Plain
    /// We will always treat it as Cleartext
    enum AuthType {
        Plain = 0,
        CramMD5,
        NTLM,
        GSSAPI,
        ClientIP,
        NoAuth,
        Basic,
        OAuth2,
    };
    Q_ENUM(AuthType);

    enum Length {
        Long = 0,
        Short,
    };
    Q_ENUM(Length);

    explicit Ispdb(QObject *parent = nullptr);
    ~Ispdb() override;

    Q_REQUIRED_RESULT QString email() const;
    void setEmail(const QString &email);

    Q_REQUIRED_RESULT QString password() const;
    void setPassword(const QString &password);

    /// After finished() has been emitted you can
    /// retrieve the domains that are covered by these
    /// settings
    Q_INVOKABLE QStringList relevantDomains() const;

    /** After finished() has been emitted you can
        get the name of the provider, you can get a long
        name and a short one */
    Q_INVOKABLE QString name(Ispdb::Length) const;

    /** After finished() has been emitted you can
        get a list of imap servers available for this provider */
    Q_INVOKABLE QList<Server> imapServers() const;

    /** After finished() has been emitted you can
        get a list of pop3 servers available for this provider */
    Q_INVOKABLE QList<Server> pop3Servers() const;

    /** After finished() has been emitted you can
        get a list of smtp servers available for this provider */
    Q_INVOKABLE QList<Server> smtpServers() const;

    Q_INVOKABLE QList<Identity> identities() const;

    Q_INVOKABLE int defaultIdentity() const;

    /// Starts looking up the servers which belong to the e-mailaddress
    Q_INVOKABLE void start();

private:
    void slotResult(KJob *);

Q_SIGNALS:
    /// emitted when done
    void finished(bool ok);
    void searchType(const QString &type);
    void emailChanged();
    void passwordChanged();

protected:
    /** search types, where to search for autoconfig
        @see lookupUrl to generate a url base on this type
     */
    enum SearchServerType {
        IspAutoConfig = 0, /**< http://autoconfig.example.com/mail/config-v1.1.xml */
        IspWellKnow, /**< http://example.com/.well-known/autoconfig/mail/config-v1.1.xml */
        DataBase /**< https://autoconfig.thunderbird.net/v1.1/example.com */
    };

    /// Start request to server
    void startJob(const QUrl &url);

    /** generate url and start job afterwards */
    void lookupInDb(bool auth, bool crypt);

    /** an valid xml document is available, parse it and create all the objects
        should run createServer, createIdentity, ...
     */
    void parseResult(const QDomDocument &document);

    /** create a server object out of an element */
    Server createServer(const QDomElement &n);

    /** create a identity object out of an element */
    Identity createIdentity(const QDomElement &n);

    /** get standard urls for autoconfig
        @return the standard url for autoconfig depends on serverType
        @param type of request (ex. "mail")
        @param version of the file (example for mail "1.1")
        @param auth use authentication with username & password to access autoconfig file
                    (username is the emailaddress)
        @param crypt use https
     */
    Q_REQUIRED_RESULT QUrl lookupUrl(const QString &type, const QString &version, bool auth, bool crypt);

    /** setter for serverType */
    void setServerType(Ispdb::SearchServerType type);

    /** getter for serverType */
    Q_REQUIRED_RESULT Ispdb::SearchServerType serverType() const;

    /** replaces %EMAILLOCALPART%, %EMAILADDRESS% and %EMAILDOMAIN% with the
        parts of the emailaddress */
    Q_REQUIRED_RESULT QString replacePlaceholders(const QString &);

    QByteArray mData; /** storage of incoming data from kio */
protected Q_SLOTS:

    /** slot for TransferJob to dump data */
    void dataArrived(KIO::Job *, const QByteArray &data);

private:
    KMime::Types::AddrSpec mAddr; // emailaddress
    QString mPassword;
    QString mEmail;

    // storage of the results
    QStringList mDomains;
    QString mDisplayName;
    QString mDisplayShortName;
    QList<Server> mImapServers;
    QList<Server> mPop3Servers;
    QList<Server> mSmtpServers;
    QList<Identity> mIdentities;

    int mDefaultIdentity = -1;
    Ispdb::SearchServerType mServerType = DataBase;
    bool mStart = true;
};

struct Server {
    Q_GADGET
    Q_PROPERTY(QString hostname MEMBER hostname)
    Q_PROPERTY(QString username MEMBER username)
public:
    Server() = default;

    Q_REQUIRED_RESULT bool isValid() const
    {
        return port != -1;
    }

    Ispdb::AuthType authentication = Ispdb::Plain;
    Ispdb::SocketType socketType = Ispdb::None;
    QString hostname;
    QString username;
    int port = -1;
};
QDebug operator<<(QDebug d, const Server &t);

struct Identity {
    Identity() = default;

    Q_REQUIRED_RESULT bool isValid() const
    {
        return !name.isEmpty();
    }

    Q_REQUIRED_RESULT bool isDefault() const
    {
        return mDefault;
    }

    QString email;
    QString name;
    QString organization;
    QString signature;
    bool mDefault = false;
};
QDebug operator<<(QDebug d, const Identity &t);
