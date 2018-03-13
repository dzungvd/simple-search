#ifndef _DB_INTERFACE_
#define _DB_INTERFACE_

#include "elasticsearch/elasticsearch.h"
#include <string>
#include <vector>
#include <map>

#include "nlohmann/json.hpp"

extern "C" {
#include "sodium.h"
}

namespace bitmile{

  namespace db {
    class Document {
    private:
      std::string owner_address_;
      std::string doc_id_;
      std::string elastic_doc_id_;
      std::vector<char> data_;
      std::vector<std::string> keywords_;

    public:
      nlohmann::json ToJson () const;
      bool FromJson (nlohmann::json& doc);

      bool ParseJson (Json::Object json_doc);
      std::string GetOwnerAddress() const;
      void SetOwnerAddress(const std::string& addr);

      std::string GetOwnerDocId() const;
      void SetOwnerDocId (const std::string& docId);

      std::string GetElasticDocId() const;
      void SetElasticDocId(const std::string& id);

      void SetData (const char* dat, const size_t size);
      const std::vector<char>& GetData () const;

      void SetKeywords (const std::vector<std::string>& keywords);
      const std::vector<std::string>& GetKeywords() const;

    };

    class DbInterface {
    public:
      DbInterface();

      void QueryDocWithKeywords (const std::vector<std::string>& keywords, std::vector<Document>& result);

      void QueryDocWithId (const std::vector<int> id_list, std::vector<Document>& result);

      std::string InsertDoc (const Document& doc);
    private:
      std::string index_;
      std::string type_;
      ElasticSearch* elastic_search_;

    };
  }//namespace db
}//namespace bitmile

#endif
