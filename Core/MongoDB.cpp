//
// Created by murtazaa on 2021-01-12.
//

#include "MongoDB.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <OTPParser.h>
#include "Logger.h"

MongoDB::MongoDB(MULONG ulId) : Node(ulId) {
    try {
        //testing Mongo DB connection
        mongocxx::instance inst{}; //DONT REMOVE THIS
        this->db_uri = "mongodb+srv://testuser:test@ap-cluster-0.maddq.mongodb.net/tracified-backend-test-db";
        this->db_name = "tracified-backend-test-db";
    } catch (const std::exception &e) {
        LOG(INFO) << "Could not Establish Connection to Database";
        std::cout << e.what();
    }
}

void MongoDB::getAll(std::string schema) {
    try {
        mongocxx::client conn{
                mongocxx::uri(this->db_uri)};
        auto collection = conn[this->db_name][schema];
        mongocxx::cursor c = collection.find({});
        // for (auto doc: c) {
        //     std::cout << bsoncxx::to_json(doc) << "\n";
        // }
        LOG(INFO) << "Connection to Database Successful";
    } catch (const std::exception &e) {
        LOG(INFO) << "Could not Establish Connection to Database";
        std::cout << e.what();
    }
}

std::string MongoDB::query(std::string schema, mongocxx::pipeline* p) {
    using namespace bsoncxx::builder::basic;
    try {
        mongocxx::client conn{
                mongocxx::uri(this->db_uri)};
        auto collection = conn[this->db_name][schema];
        mongocxx::cursor c = collection.aggregate(*p, mongocxx::options::aggregate{});
//        std::cout << bsoncxx::to_json(c);
        std::string j = "[";
        for (auto doc: c) {
            std::cout << bsoncxx::to_json(doc) << "\n";
            if (j.length() > 1)
                j += ",";
            j += bsoncxx::to_json(doc);
        }
        j += "]";
        LOG(INFO) << "Connection to Database Successful";
        return j;
    } catch (const std::exception &e) {
        LOG(INFO) << "Could not Establish Connection to Database";
        std::string err = e.what();
        std::cout << e.what();
    }
}


bsoncxx::builder::basic::array MongoDB::createArray(std::string commaSeperatedArray)
{
    bsoncxx::builder::basic::array arr = bsoncxx::builder::basic::array{};
    std::vector<std::string> result;

    std::stringstream s_stream(commaSeperatedArray); //create string stream from the string

    while(s_stream.good()){
        std::string substr;
        getline(s_stream, substr, '|'); //get first string delimited by comma
        result.push_back(substr);
    }

    for(int i = 0; i<result.size(); i++){ //print all splitted strings
        arr.append(result.at(i));
//        std::cout << result.at(i) << std::endl;
    }

    return arr;
}

void MongoDB::commaSeperatedStringToVector(std::string commaSeperatedArray)
{}