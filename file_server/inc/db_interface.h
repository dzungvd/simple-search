#ifndef _DB_INTERFACE_
#define _DB_INTERFACE_

#include "elasticsearch/elasticsearch.h"
#include <string>
#include <vector>
#include <map>

namespace bitmile{

  namespace db {
    class Document {
    private:
      std::string owner_address_;
      std::string doc_id_;
      std::string elastic_doc_id_;
      std::string data_;

    public:
      bool ParseJson (Json::Object json_doc);
      std::string GetOwnerAddress();
      std::string GetOwnerDocId();
      std::string GetElasticDocId();
    };

    class DbInterface {
    public:
      DbInterface();
      void QueryDocWithKeywords (const std::vector<std::string>& keywords, std::vector<Document>& result);

      void QueryDocWithId (const std::vector<int> id_list, std::vector<Document>& result);

    private:
      std::string index_;
      std::string type_;
      ElasticSearch* elastic_search_;

    };
  }//namespace db
}//namespace bitmile

#endif
