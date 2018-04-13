#include "dealmanager.h"
#include <QSqlQuery>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

DealManager::DealManager(QObject* parent) : QObject(parent) {

}

DealManager::~DealManager() {

}

QVariant DealManager::getDealData () {
    dealOwnerList.clear();

    InternalDB* db = InternalDB::getInstance();
    QString q = "SELECT * FROM DealOwner";

    if (!db->query(q))
        return QByteArray();

    QSqlQuery* queryObj = db->getSqlQuery();

    InternalDB::DealOwner dealOwner;
    QJsonObject jsonObj;
    QJsonArray dealOwnerArr;

    while (queryObj->next()) {
        dealOwner.deal_time = queryObj->value(InternalDB::DEALOWNER_DEALTIME_INDEX).toUInt();
        dealOwner.decrypt_data = queryObj->value(InternalDB::DEALOWNER_DECRYPT_DATA_INDEX).toString();
        dealOwner.encrypt_data = queryObj->value(InternalDB::DEALOWNER_ENCRYPT_DATA_INDEX).toString();
        dealOwner.owner_address = queryObj->value(InternalDB::DEALOWNER_OWNER_ADDRESS_INDEX).toString();
        dealOwner.owner_doc_id = queryObj->value(InternalDB::DEALOWNER_DOC_ID_INDEX).toString();
        dealOwner.owner_secret_key = queryObj->value(InternalDB::DEALOWNER_SECRET_KEY_INDEX).toString();
        dealOwner.status = queryObj->value(InternalDB::DEALOWNER_STATUS_INDEX).toInt();
        dealOwner.elastic_id =  queryObj->value(InternalDB::DEALOWNER_ELASTIC_ID_INDEX).toInt();

        jsonObj[DEALOWNER_DEAL_TIME] = QString("%1").arg(dealOwner.deal_time);
        jsonObj[DEALOWNER_DECRYPT_DATA] = dealOwner.decrypt_data;
        jsonObj[DEALOWNER_ENCRYPT_DATA] = dealOwner.encrypt_data;
        jsonObj[DEALOWNER_OWNER_ADDRESS] = dealOwner.owner_address;
        jsonObj[DEALOWNER_DOC_ID] = dealOwner.owner_doc_id;
        jsonObj[DEALOWNER_ELASTICID] = dealOwner.elastic_id;
        jsonObj[DEALOWNER_STATUS] = QString("%1").arg(dealOwner.status);

        dealOwnerArr.append(jsonObj);
    }

    return QJsonDocument(dealOwnerArr).toVariant();
}
