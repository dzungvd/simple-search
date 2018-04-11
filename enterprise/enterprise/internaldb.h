#ifndef INTERNALDB_H
#define INTERNALDB_H

#include <QSqlDatabase>
#include <QObject>
#include <QSqlQuery>


#define DEFAULT_DB_NAME "internal.db"

/* table structure */
#define OWNER_TABLE "Owner"
#define     OWNER_ADDRESS "address"                     // BLOB             <- TEXT

#define DEAL_TABLE "Deal"
#define     DEAL_PRICE "price"                          // DOUBLE PRECISION
#define     DEAL_PUBLICKEY "public_key"                 // BLOB             <- BASE64
#define     DEAL_PRIVATEKEY "private_key"               // BLOB             <- BASE64
#define     DEAL_KEYWORDS "keywords"                    // BLOB             <- JSON format {keywords:[...]}
#define     DEAL_TIME "time"                            // UNSIGNED BIG INT <- timestame

#define DEALOWNER_TABLE "DealOwner"
#define     DEALOWNER_DEAL_TIME "deal_time"             // UNSIGNED BIG INT
#define     DEALOWNER_OWNER_ADDRESS  "owner_address"    // BLOB             <- BASE64
#define     DEALOWNER_STATUS    "status"                // INT              <- ENUM {dont_accept:0, accept:1, waiting: 2}
#define         DEALOWNER_STATUS_DONTACCEPT  0
#define         DEALOWNER_STATUS_ACCEPT      1
#define         DEALOWNER_STATUS_WAITING     2
#define     DEALOWNER_ENCRYPT_DATA "encrypt_data"       // BLOB             <- BASE64
#define     DEALOWNER_DECRYPT_DATA "decrypt_data"       // BLOB             <- BASE64
#define     DEALOWNER_SECRETKEY "owner_secret_key"      // BLOB             <- BASE64
#define     DEALOWNER_DOC_ID "owner_doc_id"             // UNSIGNED BIG INT
#define     DEALOWNER_ELASTICID  "elastic_id"           // UNSIGNED BIG INT

class InternalDB : public QObject {
    Q_OBJECT
public:
    InternalDB(QObject*);
    ~InternalDB();

    struct Deal {
       qreal    price;
       QString  public_key;
       QString  private_key;
       QString  keywords;
       quint64  time;
    };

    struct Owner {
        QString address;
    };

    struct DealOwner {
        quint64 deal_time;
        QString owner_address;
        qint8   status;
        QString encrypt_data;
        QString decrypt_data;
        QString owner_secret_key;
        quint64 owner_doc_id;
        quint64 elastic_id;
    };

public:
    static InternalDB* getInstance();

    // interact with Deal table
    bool insertDealData(const Deal&);
    bool updateDealData(const Deal&);
    bool removeDealData(const Deal&);

    // interact with Owner table
    bool insertOwnerData(const Owner&);
    bool updateOwnerData(const Owner&);
    bool removeOwnerData(const Owner&);

    // interact with DealOwner table
    bool insertDealOwnerData(const DealOwner&);
    bool updateDealOwnerData(const DealOwner&);
    bool removeDealOwnerData(const DealOwner&);

    // utils
    bool establiseConnection();
    void disconnect();
    bool createDB();
    bool isExitsDB();
    bool query(QString _query);

    // get set properties
    void setSqlDbName(const QString& _sqlPathName);
    QString getSqlDbName() const;
    QSqlQuery* getSqlQuery() const;

private:
    QString convertToString(const quint64 arg) {return QString("%1").arg(arg);}
    QString convertToString(const qreal arg) {return QString("%1").arg(arg);}
    QString convertToString(const qint8 arg) {return QString("%1").arg(arg);}
    QString convertToString(const QString arg) {return arg;}

private:
    QString sqlDbName;
    QSqlDatabase db;
    QSqlQuery sqlQuery;
};

#endif // INTERNALDB_H
