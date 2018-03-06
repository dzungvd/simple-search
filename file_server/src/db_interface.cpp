#include "db_interface.h"
#include <iostream>

namespace bitmile{
  namespace db{
    std::string Document::GetOwnerAddress () {
      return owner_address_;
    }
    void Document::SetOwnerAddress (std::string& addr) {
      owner_address_ = addr;
    }

    std::string Document::GetOwnerDocId() {
      return doc_id_;
    }

    void Document::SetOwnerDocId (std::string& id) {
      doc_id_ = id;
    }

    std::string Document::GetElasticDocId() {
      return elastic_doc_id_;
    }

    void Document::SetElasticDocId (std::string& id) {
      elastic_doc_id_ = id;
    }
    bool Document::ParseJson (Json::Object json_doc) {
      /*     {
       *        "_index": "user_data",
       *        "_type": "_doc",
       *        "_id": "1",
       *        "_score": 0.5753642,
       *        "_source": {
       *          "owner": "fake ethereum address here",
       *          "keywords": "hung thinh is born on october 1993",
       *          "data": "this is a test data",
       *          "docId": "1"
       *        }
       *      }
       */
      //check json doc validity

      //json data must contain _id field which is document identification of elastic and _source which is actual data begin stored

      bool valid = json_doc.member ("_id") &&
        json_doc.member ("_source");

      if (!valid)  {
        return false;
      }

      Json::Object json_source = json_doc["_source"].getObject();

      /* data must contain
       *   1. owner : owner's blockchain address
       *   2. keywords: keywords as search reference
       *   3. data: link to real data
       *   4. docId: document id for owner
       */
      valid = valid && json_source.member ("owner") &&
        json_source.member ("keywords") &&
        json_source.member ("data") &&
        json_source.member ("docId");

      if (!valid) {
        return false;
      }


      //get content data
      elastic_doc_id_ = json_doc["_id"].getString();
      owner_address_ = json_source["owner"].getString();
      doc_id_ = json_source["docId"].getString();

      return true;
    }

    DbInterface::DbInterface() {

      //TODO: make port parameter here
      elastic_search_ = new ElasticSearch ("localhost:9200");
      index_ = "user_data";
      type_ = "_doc";
    }

    void DbInterface::QueryDocWithKeywords (const std::vector<std::string>& keywords, std::vector<Document>& result) {

      //create json query from keywords
      /*
        query have form:
        {
        "query" : {
        "bool" : {
        "must" : [
        { "match" : { "keywords" : "input_keyword1" } },
        { "match" : { "keywords" : "input_keyword2" } },
        ]
        }
        }
        }
      */

      Json::Array must_level_arr;
      for (int i = 0; i < keywords.size(); i++){
        //create query
        Json::Object keyword_level;
        keyword_level.addMemberByKey("keywords", keywords[i]);

        Json::Object match_level;
        match_level.addMemberByKey("match", keyword_level);

        must_level_arr.addElement(match_level);

      }

      Json::Object must_level;
      must_level.addMemberByKey("must", must_level_arr);

      Json::Object bool_level;
      bool_level.addMemberByKey("bool", must_level);

      Json::Object query_level;
      query_level.addMemberByKey("query", bool_level);


      /* send query to elastic and get json result
       *
       *example result format:
       *{
       *  "took": 2,
       *  "timed_out": false,
       *  "_shards": {
       *    "total": 5,
       *    "successful": 5,
       *    "skipped": 0,
       *    "failed": 0
       *  },
       *  "hits": {
       *    "total": 1,
       *    "max_score": 0.5753642,
       *    "hits": [
       *      {
       *        "_index": "user_data",
       *        "_type": "_doc",
       *        "_id": "1",
       *        "_score": 0.5753642,
       *        "_source": {
       *          "owner": "fake ethereum address here",
       *          "keywords": "hung thinh is born on october 1993",
       *          "data": "this is a test data",
       *          "docId": "1"
       *        }
       *      }
       *    ]
       *  }
       *}
       */
      Json::Object j_result;

      try{
        if (!elastic_search_->search(index_, type_, query_level.str(), j_result)) {
          std::cout << "can't find any doc matched query" << std::endl;
          return;
        }
      }catch (Exception& e) {
        std::cerr << "failed in DbInterface::Querydocwithkeywords. Exception caught: " << e.what() << std::endl;
        return;
      }

      Json::Array hit_list;
      hit_list = j_result.getValue("hits").getObject().getValue("hits").getArray();

      Json::Array::const_iterator hit_list_iter = hit_list.begin();

      while (hit_list_iter != hit_list.end()) {
        Document doc;
        if (doc.ParseJson ((*hit_list_iter).getObject())) {
          result.push_back (doc);
        }
        ++hit_list_iter;
      }
    }

    void DbInterface::QueryDocWithId (const std::vector<int> id_list, std::vector<Document>& result) {

    }
  }//namespace db
}//namespace bitmile
