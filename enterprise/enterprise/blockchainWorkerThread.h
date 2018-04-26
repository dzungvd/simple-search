#ifndef BLOCKCHAINWORKERTHREAD_H
#define BLOCKCHAINWORKERTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include "blockchain/blockchain_interface.h"
#include "nlohmann/json.hpp"

class BlockchainWorkerThread : public QThread
{
    Q_OBJECT
    void run() override;

public:
    BlockchainWorkerThread(std::vector<int64_t> deal_ids, std::vector<int> answer_numbs, std::vector<int> key_numbs, std::string deal_address);
    void addDeal (int64_t deal_id, int answer_numb, int key_numb);
Q_SIGNALS:
    void newAnswers (std::vector<int64_t> deal_ids, std::vector<int> answer_numbs);
    void newKeys (std::vector<int64_t> deal_ids, std::vector<int> key_numbs);
private:
    QMutex mutex;

    bitmile::blockchain::BlockchainInterface blockchain_;
    std::vector<int64_t> deal_ids_;
    std::vector<int> answer_numbs_;
    std::vector<int> key_numbs_;
    std::string deal_address_;
};

#endif // BLOCKCHAINWORKER_H
