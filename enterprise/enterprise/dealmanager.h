#ifndef DEALMANAGER_H
#define DEALMANAGER_H

#include <QObject>
#include "internaldb.h"
#include <QVariant>

class DealManager : public QObject {
    Q_OBJECT
public:
    DealManager(QObject* parent = 0);
    ~DealManager();

public:
    Q_INVOKABLE QVariant getDealData ();

Q_SIGNALS:
    Q_INVOKABLE void dealDataChanged(QByteArray dealData);

private:
    QList<InternalDB::Deal> dealList;
    QList<InternalDB::Owner> ownerList;
    QList<InternalDB::DealOwner> dealOwnerList;
};


#endif // DEALMANAGER_H
