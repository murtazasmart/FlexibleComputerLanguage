//
// Created by murtazaa on 2021-01-18.
//

#include "Node.h"
#include <OTPParser.h>
#include <mongocxx/cursor.hpp>
#include "MongoReview.h"
#include "MongoDB.h"

Node* MongoReview::queryReviews(std::string profileIds) {
//    std::string stageId, tenantIds, itemIds, fromdate
//    tenantIds - LValue
//    tenantIds -
    using namespace bsoncxx::builder::basic;
    MongoDB* db = MongoDB::getInstance();
    mongocxx::pipeline p{};
    p.match(make_document(
            kvp("$and", make_array(
                    make_document(
                            kvp("profileId", make_document(
                                    kvp("$in", db->createArray(profileIds))
                            ))
            ))
            )
    ));
    std::string j = db->query("reviews", &p);

    Node* r = new Node(70000);
    r->SetCustomString("reviewsroot");
    r->SetValue("reviewsroot");
    r->SetLValue("reviewsroot");
    r->SetRValue("reviewsroot");
//    std::string j = "[";
//    for (auto doc: c) {
//        std::cout << bsoncxx::to_json(doc) << "\n";
//        if (j.length() > 1)
//            j += ",";
//        j += bsoncxx::to_json(doc);
//    }
//    j += "]";
    rapidjson::Document otps;
    OTPParser::createTDTree(otps.Parse(j.c_str()), r);
    return r;
}

