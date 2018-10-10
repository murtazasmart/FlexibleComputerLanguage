//
//  OTPParser.cpp
//  FlexibleComputerLanguage
//
//  Created by Murtaza Anverali on 7/5/18.
//  Copyright © 2018 Dileepa Jayathilaka. All rights reserved.
//

#include "json.hpp"
#include "OTPParser.h"
#include "MemMan.h"
#include "Node.h"
#include "MetaData.h"
#include "Strings.h"
#include "Entity.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

//Testing purposes
#include  <chrono>

// using namespace rapidjson;

// This will iter add oobject to a tree and return tree at the end

void OTPParser::createTDTree(rapidjson::Document& j, Node *parent)
{
    int id = 0;
    for (rapidjson::Value::ConstMemberIterator data = j.MemberBegin(); data != j.MemberEnd(); ++data)
    {
        rapidjson::Document jsonvalue;
        jsonvalue.Parse(data->value.GetString());
        if (jsonvalue.IsObject() || jsonvalue.IsArray())
        {
            Node *datanode = MemoryManager::Inst.CreateNode(++id);
            //            std::cout << (char *)data.key().c_str();
            datanode->SetValue((char *)data->name.GetString());
            parent->AppendNode(datanode);
            createTDTree(jsonvalue, datanode);
        }
        else
        {
            PString pStr = 0;
            MemoryManager::Inst.CreateObject(&pStr);
            pStr->SetValue(jsonvalue.GetString());
            Node *datanode = MemoryManager::Inst.CreateNode(++id);
            std::string val = jsonvalue.GetString();
            //            std::replace(val.begin(), val.end(), '"', '\0');
            val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
            datanode->SetEntityObj((PENTITY)pStr);
            datanode->SetValue((char *)data->name.GetString());
            datanode->SetLValue((char *)val.c_str());
            parent->AppendNode(datanode);
        }
    }
}

Node *OTPParser::OTPJSONToNodeTree(std::string otpsString)
{
    int id = 0;
    //auto start_1 = std::chrono::system_clock::now();
    rapidjson::Document otps;
    otps.Parse(otpsString.c_str());
    //auto end_1 = std::chrono::system_clock::now();
    //std::chrono::duration<double> elapsed_1 = end_1 - start_1;
    //std::cout << "First " << elapsed_1.count() << std::endl;
    Node *root = MemoryManager::Inst.CreateNode(++id);
    int i = 0, j = 0, k = 0;
    //auto start_2 = std::chrono::system_clock::now();
    for (rapidjson::Value::ConstMemberIterator tp = otps[0].MemberBegin(); tp != otps[0].MemberEnd(); ++tp)
    {
        rapidjson::Document tpjson;
        tpjson.Parse(tp->value.GetString());
        Node *tpnode = MemoryManager::Inst.CreateNode(++id);
        tpnode->SetValue((char *)tpjson["stageID"].GetString());
        root->AppendNode(tpnode);
        //auto start_3 = std::chrono::system_clock::now();
        for (rapidjson::Value::ConstValueIterator tdp = tpjson["traceabilityDataPackets"].Begin(); tdp != tpjson["traceabilityDataPackets"].End(); ++tdp)
        {
            rapidjson::Document tdpjson;
            tdpjson.Parse(tdp->GetString());
            Node *tdpnode = MemoryManager::Inst.CreateNode(++id);
            tdpnode->SetValue((char *)tdpjson["userID"].GetString());
            tpnode->AppendNode(tdpnode);
            //auto start_4 = std::chrono::system_clock::now();
            for (rapidjson::Value::ConstValueIterator td = tdpjson["traceabilityData"].Begin(); td != tdpjson["traceabilityData"].End(); ++td)
            {
                rapidjson::Document tdjson;
                tdjson.Parse(td->GetString());
                Node *tdnode = MemoryManager::Inst.CreateNode(++id);
                tdpnode->AppendNode(tdnode);
                tdnode->SetValue((char *)tdjson["key"].GetString());
                //                tdnode->SetValue((char *)"something is better");
                if (tdjson["val"].IsObject() || tdjson["val"].IsArray())
                {
                    //auto start_5 = std::chrono::system_clock::now();
                    rapidjson::Document val;
                    val.Parse(tdjson["val"].GetString());
                    createTDTree(val, tdnode);
                    //auto end_5 = std::chrono::system_clock::now();
                    //std::chrono::duration<double> elapsed_5 = end_5 - start_5;
                    //std::cout << "Creating TD Tree " << elapsed_5.count() << std::endl;
                }
                else
                {
                    PString pStr = 0;
                    MemoryManager::Inst.CreateObject(&pStr);
                    std::string val = tdjson["val"].GetString();
                    //                    std::replace(val.begin(), val.end(), '"', '\0');
                    val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
                    pStr->SetValue(tdjson["val"].GetString());
                    tdnode->SetEntityObj((PENTITY)pStr);
                    tdnode->SetLValue((char *)val.c_str());
                    //                    std::cout << ((PENTITY)tdnode->GetEntityObj())->ul_Type;

                    //                    tdnode->SetValue((char *)tdjson["val"].dump().c_str());
                }
                //                std::cout << (char *)std::to_string(tdjson["type"].get<int>()).c_str();
                tdnode->SetRValue((char *)std::to_string(tdjson["type"].GetInt()).c_str());
            }
            //auto end_4 = std::chrono::system_clock::now();
            //std::chrono::duration<double> elapsed_4 = end_4 - start_4;
            //std::cout << "Inner 2nd for " << elapsed_4.count() << std::endl;
        }
        //auto end_3 = std::chrono::system_clock::now();
        //std::chrono::duration<double> elapsed_3 = end_3 - start_3;
        //std::cout << "Inner 1st for " << elapsed_3.count() << std::endl;
    }
    //auto end_2 = std::chrono::system_clock::now();
    //std::chrono::duration<double> elapsed_2 = end_2 - start_2;
    //std::cout << "Outer for " << elapsed_2.count() << std::endl;
    return root;
}
