
//
// Created by murtazaa on 2021-01-13.
//
#include <OTPParser.h>
#include <mongocxx/cursor.hpp>
#include "MongoTP.h"
#include "MongoDB.h"
#include "DateTimeOperations.h"

Node* MongoTP::queryProfilesAndTDPs(std::string stageIds, std::string tenantIds, std::string itemIds, std::string fromdate) {
//    std::string stageId, tenantIds, itemIds, fromdate
//    tenantIds - LValue
//    tenantIds -
    using namespace bsoncxx::builder::basic;
    MongoDB* db = MongoDB::getInstance();
    mongocxx::pipeline p{};
    struct tm tm;
//    std::string mydate = "2021-01-12 00 00 00";
    strptime(fromdate.c_str(), "%Y-%m-%d %H %M %S", &tm);
    std::time_t tt = std::mktime(&tm);
    auto t = std::chrono::system_clock::from_time_t(tt);
    bsoncxx::types::b_date from_date_val{t};
    p.match(make_document(
            kvp("$and", make_array(
                    make_document(
                            kvp("item.itemID", make_document(
                                    kvp("$in", db->createArray(itemIds))
                            )),
                            kvp("stageID", make_document(
                                    kvp("$in", db->createArray(stageIds))
                            )),
                            kvp("tenantID", make_document(
                                    kvp("$in", db->createArray(tenantIds))
                            )),
                            kvp("timestamp", make_document(
                                    kvp("$gte", from_date_val)
                            ))
                    ))
            )
    ));
    // Ordering all profiles latest first. This will allow when grouping takes place for latest ones to be automatically be first
    p.sort(make_document(
            kvp("timestamp", -1)
    ));
    // Links all the relevant TDPs
    p.lookup(make_document(
            kvp("as", "tdps"),
            kvp("from", "traceabilitydatapackets"),
            kvp("let", make_document(
                    kvp("tdpIds", "$traceabilityDataPacketIDs")
            )),
            kvp("pipeline", make_array(
                    make_document(
                            kvp("$addFields", make_document(
                                    kvp("id", make_document(
                                            kvp("$toString", "$_id")
                                    ))
                            ))),
                    make_document(
                            kvp("$match", make_document(
                                    kvp("$expr", make_document(
                                            kvp("$and",
                                                make_array(make_document(
                                                        kvp("$in", make_array("$id", "$$tdpIds"))
                                                )))
                                    ))
                            ))
                    ),
                    make_document(
                            kvp("$project", make_document(
                                    kvp("id", 1),
                                    kvp("item", 1),
                                    kvp("timestamp", 1),
                                    kvp("traceabilityData", 1),
                                    kvp("userID", 1)
                            ))
                    ),
                    make_document(
                            kvp("$sort", make_document(
                                    kvp("timestamp", -1)
                            ))
                    )
//                        make_document(
//                                kvp("$limit", 1)
//                        )
            ))
    ));
//    p.group(make_document(
//            kvp("_id", "$identifier.identifier"),
//            kvp("grouped", make_document(
//                    kvp("$push", "$$ROOT")
//            ))
//    ));
    mongocxx::cursor c = db->query("traceabilityprofiles", &p);

    Node* r = new Node(80000);
    r->SetCustomString("profileroot");
    r->SetValue("profileroot");
    r->SetLValue("profileroot");
    r->SetRValue("profileroot");
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

