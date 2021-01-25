//
// Created by murtazaa on 2021-01-12.
//
#ifndef NodeMongo_h
#define NodeMongo_h

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include "Node.h"

class MongoDB : public Node {
    static inline MongoDB *instance;
    std::string db_uri;
    std::string db_name;

    // Private constructor so that no objects can be created.
    MongoDB(MULONG ulId);

public:
    static MongoDB *getInstance() {
        if (!MongoDB::instance)
            MongoDB::instance = new MongoDB(999989);
        return MongoDB::instance;
    };

    mongocxx::cursor query(std::string schema, mongocxx::pipeline* p);
    void getAll(std::string schema);
    bsoncxx::builder::basic::array createArray(std::string commaSeperatedArray);
    void commaSeperatedStringToVector(std::string commaSeperatedArray);

};

#endif /* NodeMongo_h */
