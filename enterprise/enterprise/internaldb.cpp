#include "internaldb.h"
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlError>
#include <QDebug>

InternalDB::InternalDB(QObject* parent = 0) : QObject(parent) {
}

InternalDB::~InternalDB() {
    db.close();
}

InternalDB* InternalDB::getInstance() {
    static InternalDB* instance = NULL;
    if (instance == NULL)
        instance = new InternalDB();
    return instance;
}

bool InternalDB::establiseConnection() {
    if (sqlDbName.isEmpty())
            sqlDbName.append(DEFAULT_DB_NAME);

    disconnect();

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(sqlDbName);

    Q_ASSERT(db.open());

    // create db if not exists
    sqlQuery = QSqlQuery(db);
    createDB();
}

void InternalDB::disconnect() {
    if (db.isOpen())
        db.close();
}

void InternalDB::setSqlDbName(const QString& _sqlDbName) {
    if (_sqlDbName.isEmpty())
        return;

    sqlDbName = _sqlDbName;
}

QString InternalDB::getSqlDbName() const {
    return sqlDbName;
}


QSqlQuery* InternalDB::getSqlQuery() {
    return &sqlQuery;
}


bool InternalDB::createDB() {
    QString q;

    // create Owner table
    q = QString("CREATE TABLE IF NOT EXISTS %1 (%2 BLOB PRIMARY KEY)").arg(OWNER_TABLE, OWNER_ADDRESS);

    if (!query(q)) {
        qDebug() << "Have error when create OWNER_TABLE";
        return false;
    }

    // create Deal table
    q = QString ("CREATE TABLE IF NOT EXISTS %1 (%2 DOUBLE, "
                 "                 %3 BLOB, %4 BLOB, %5 BLOB, %6 UNSIGNED BIG INT PRIMARY KEY)"
                 ).arg(DEAL_TABLE, DEAL_PRICE,
                       DEAL_PUBLICKEY, DEAL_PRIVATEKEY,
                       DEAL_KEYWORDS, DEAL_TIME);

    if (!query(q)) {
        qDebug() << "Have error when create DEAL_TABLE";
        return false;
    }

    // create DealOwner Table
    q = QString ("CREATE TABLE IF NOT EXISTS %1 (%2 UNSIGNED BIG INT, %3 BLOB, "
                 "                 %4 INT, %5 BLOB, %6 BLOB, %7 BLOB, %8 BLOB, %9 BLOB)"
                 ).arg(DEALOWNER_TABLE, DEALOWNER_DEAL_TIME, DEALOWNER_OWNER_ADDRESS,
                       DEALOWNER_STATUS, DEALOWNER_ENCRYPT_DATA, DEALOWNER_DECRYPT_DATA,
                       DEALOWNER_SECRETKEY, DEALOWNER_DOC_ID, DEALOWNER_ELASTICID);

    if (!query(q)) {
        qDebug() << "Have error when create DEAL_TABLE";
        return false;
    }

    return true;

}

// interact with Deal table
bool InternalDB::insertDealData(const Deal& _data) {
    QString q = "INSERT INTO %1 VALUES (:d2, ':d3', ':d4', ':d5', :d6)";
    q = q.arg(DEAL_TABLE);

    q.replace(":d2", convertToString(_data.price));
    q.replace(":d3", convertToString(_data.public_key));
    q.replace(":d4", convertToString(_data.private_key));
    q.replace(":d5", convertToString(_data.keywords));
    q.replace(":d6", convertToString(_data.time));

    return query(q);
}

bool InternalDB::updateDealData(const Deal& _data) {
    QString q = "UPDATE %1 SET %2=:d2, %3=':d3', %4=':d4', %5=':d5' WHERE %6=:d6";
    q = q.arg(DEAL_TABLE, DEAL_PRICE, DEAL_PUBLICKEY, DEAL_PRIVATEKEY, DEAL_KEYWORDS, DEAL_TIME);

    q.replace(":d2",  convertToString(_data.price));
    q.replace(":d3",  convertToString(_data.public_key));
    q.replace(":d4",  convertToString(_data.private_key));
    q.replace(":d5",  convertToString(_data.keywords));
    q.replace(":d6",  convertToString(_data.time));

   return query(q);
}

bool InternalDB::removeDealData(const Deal& _data) {
    QString q = "DELETE FROM %1 WHERE %2=:d2)";
    q.arg(DEAL_TABLE, DEAL_TIME);
    q.replace(":d2", convertToString(_data.time));
    return query(q);
}

// interact with Owner table
bool InternalDB::insertOwnerData(const Owner& _data) {
    QString q =  "INSERT INTO %1 VALUES (':d2')";
    q = q.arg(OWNER_TABLE);
    q.replace(":d2", convertToString(_data.address));
    return query(q);
}

bool InternalDB::updateOwnerData(const Owner& _data) {
    Q_UNUSED(_data);
    //TODO
}

bool InternalDB::removeOwnerData(const Owner& _data) {
    QString q =  "DELETE FROM %1 WHERE %2=':d2')";
    q = q.arg(OWNER_TABLE, OWNER_ADDRESS);
    q.replace(":d2", convertToString(_data.address));
    return query(q);
}

// interact with DealOwner table
bool InternalDB::insertDealOwnerData(const DealOwner& _data) {
    QString q = "INSERT INTO %1 VALUES (:d2,':d3', :d4, ':d5', ':d6', ':d7', ':d8', ':d9')";
    q = q.arg(DEALOWNER_TABLE);

    q.replace(":d2", convertToString(_data.deal_time));
    q.replace(":d3", convertToString(_data.owner_address));
    q.replace(":d4", convertToString(_data.status));
    q.replace(":d5", convertToString(_data.encrypt_data));
    q.replace(":d6", convertToString(_data.decrypt_data));
    q.replace(":d7", convertToString(_data.owner_secret_key));
    q.replace(":d8", convertToString(_data.owner_doc_id));
    q.replace(":d9", convertToString(_data.elastic_id));

    return query(q);
}

bool InternalDB::updateDealOwnerData(const DealOwner& _data) {
    QString q = "UPDATE %1 SET %2=:d2, %3=':d3', %4=':d4', %5=':d5', %6=':d6', %7=':d7' "
                "WHERE %8=':d8' AND %9=':d9'";

    q = q.arg(DEALOWNER_TABLE, DEALOWNER_STATUS, DEALOWNER_ENCRYPT_DATA
          ,DEALOWNER_DECRYPT_DATA, DEALOWNER_SECRETKEY,
          DEALOWNER_DOC_ID, DEALOWNER_ELASTICID,
          DEALOWNER_DEAL_TIME, DEALOWNER_OWNER_ADDRESS);

    q.replace(":d2", convertToString(_data.status));
    q.replace(":d3", convertToString(_data.encrypt_data));
    q.replace(":d4", convertToString(_data.decrypt_data));
    q.replace(":d5", convertToString(_data.owner_secret_key));
    q.replace(":d6", convertToString(_data.owner_doc_id));
    q.replace(":d7", convertToString(_data.elastic_id));
    q.replace(":d8", convertToString(_data.deal_time));
    q.replace(":d9", convertToString(_data.owner_address));

    return query(q);
}

bool InternalDB::removeDealOwnerData(const DealOwner& _data) {
    QString q = "DELETE FROM %1 WHERE %2=:d2 AND %3=':d3'";
    q = q.arg(DEALOWNER_TABLE, DEALOWNER_DEAL_TIME, DEALOWNER_OWNER_ADDRESS);

    q.replace(":d2", convertToString(_data.deal_time));
    q.replace(":d3", convertToString(_data.owner_address));

    return query(q);
}


bool InternalDB::query(QString _query) {
    qDebug() << "InternalDB::query " << _query;

    sqlQuery.prepare(_query);

    if (!sqlQuery.exec()) {
        qDebug() << sqlQuery.lastError().text();
        return false;
    }

    return true;
}
