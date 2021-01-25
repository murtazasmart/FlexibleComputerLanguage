//
// Created by murtazaa on 2021-01-18.
//

#include "Node.h"
#include <OTPParser.h>
#include <mongocxx/cursor.hpp>
#include "MongoReview.h"
#include "MongoDB.h"

Node* MongoReview::queryReviews(std::string identifier, std::string tenantIds, std::string itemIds, std::string fromdate) {
//    std::string stageId, tenantIds, itemIds, fromdate
//    tenantIds - LValue
//    tenantIds -
    using namespace bsoncxx::builder::basic;
    MongoDB* db = MongoDB::getInstance();
    mongocxx::pipeline p{};
    p.match(make_document(
            kvp("$and", make_array(
                    make_document(
                            kvp("item.itemID", make_document(
                                    kvp("$in", db->createArray(itemIds))
                            )),
                            kvp("identifier.identifier", make_document(
                                    kvp("$in", db->createArray(identifier))
                            )),
                            kvp("tenantId", make_document(
                                    kvp("$in", db->createArray(tenantIds))
                            ))
            ))
            )
    ));
    mongocxx::cursor c = db->query("reviews", &p);

    Node* r = new Node(80000);
    r->SetCustomString("reviewsroot");
    r->SetValue("reviewsroot");
    r->SetLValue("reviewsroot");
    r->SetRValue("reviewsroot");
    std::string j = "[";
    for (auto doc: c) {
        std::cout << bsoncxx::to_json(doc) << "\n";
        if (j.length() > 1)
            j += ",";
        j += bsoncxx::to_json(doc);
    }
    j += "]";
    rapidjson::Document otps;
    OTPParser::createTDTree(otps.Parse(j.c_str()), r);
    return r;
}

