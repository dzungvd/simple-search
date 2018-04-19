#ifndef DEALMANAGER_H
#define DEALMANAGER_H

#include <QObject>
#include "internaldb.h"
#include <QVariant>
#include <QList>
#include <QMap>

class DealManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariant dealOwnerData READ getDealOwnerData NOTIFY dealOwnerDataChanged)
    Q_PROPERTY(QVariant dealData READ getDealData NOTIFY dealDataChanged)
    Q_PROPERTY(QVariant ownerData READ getOwnerData NOTIFY ownerDataChanged)

public:
    DealManager(QObject* parent = 0);
    ~DealManager();

public:
    static DealManager* getInstance();

public Q_SLOTS:
    Q_INVOKABLE QVariant getDealOwnerData (qint64 deal = -1);
    Q_INVOKABLE void updateDealOwnerData();

    Q_INVOKABLE QVariant getDealData();
    Q_INVOKABLE void updateDealData();

    Q_INVOKABLE QVariant getOwnerData();
    Q_INVOKABLE void updateOwnerData();

Q_SIGNALS:
    Q_INVOKABLE void dealOwnerDataChanged(QVariant dealOwnerData);
    Q_INVOKABLE void dealDataChanged(QVariant dealData);
    Q_INVOKABLE void ownerDataChanged(QVariant ownerData);

private:
    QList<InternalDB::Deal> m_dealList;
    QList<InternalDB::Owner> m_ownerList;
    QMap<qint64, QList<InternalDB::DealOwner>> m_dealOwnerMap;

    // json data
    QVariant m_dealOwnerData;
    QVariant m_dealData;
    QVariant m_ownerData;
};


#endif // DEALMANAGER_H
