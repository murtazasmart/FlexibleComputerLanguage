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

//Testing purposes
#include  <chrono>

// This will iter add oobject to a tree and return tree at the end

void OTPParser::createTDTree(nlohmann::json j, Node *parent)
{
    int id = 0;
    for (auto &data : nlohmann::json::iterator_wrapper(j))
    {
        nlohmann::json jsonvalue = data.value();
        if (jsonvalue.is_object() || jsonvalue.is_array())
        {
            Node *datanode = MemoryManager::Inst.CreateNode(++id);
            //            std::cout << (char *)data.key().c_str();
            datanode->SetValue((char *)data.key().c_str());
            parent->AppendNode(datanode);
            createTDTree(jsonvalue, datanode);
        }
        else
        {
            PString pStr = 0;
            MemoryManager::Inst.CreateObject(&pStr);
            pStr->SetValue((char *)jsonvalue.dump().c_str());
            Node *datanode = MemoryManager::Inst.CreateNode(++id);
            std::string val = jsonvalue.dump();
            //            std::replace(val.begin(), val.end(), '"', '\0');
            val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
            datanode->SetEntityObj((PENTITY)pStr);
            datanode->SetValue((char *)data.key().c_str());
            datanode->SetLValue((char *)val.c_str());
            parent->AppendNode(datanode);
        }
    }
}

Node *OTPParser::OTPJSONToNodeTree(std::string otpsString)
{
    int id = 0;
    //auto start_1 = std::chrono::system_clock::now();
    nlohmann::json otps = nlohmann::json::parse(otpsString);
    //auto end_1 = std::chrono::system_clock::now();
    //std::chrono::duration<double> elapsed_1 = end_1 - start_1;
    //std::cout << "First " << elapsed_1.count() << std::endl;
    Node *root = MemoryManager::Inst.CreateNode(++id);
    int i = 0, j = 0, k = 0;
    //auto start_2 = std::chrono::system_clock::now();
    for (auto &tp : nlohmann::json::iterator_wrapper(otps[0]))
    {
        nlohmann::json tpjson = tp.value();
        Node *tpnode = MemoryManager::Inst.CreateNode(++id);
        tpnode->SetValue((char *)tpjson["stageID"].get<std::string>().c_str());
        root->AppendNode(tpnode);
        //auto start_3 = std::chrono::system_clock::now();
        for (auto &tdp : nlohmann::json::iterator_wrapper(tpjson["traceabilityDataPackets"]))
        {
            nlohmann::json tdpjson = tdp.value();
            Node *tdpnode = MemoryManager::Inst.CreateNode(++id);
            tdpnode->SetValue((char *)tdpjson["userID"].get<std::string>().c_str());
            tpnode->AppendNode(tdpnode);
            //auto start_4 = std::chrono::system_clock::now();
            for (auto &td : nlohmann::json::iterator_wrapper(tdpjson["traceabilityData"]))
            {
                nlohmann::json tdjson = td.value();
                Node *tdnode = MemoryManager::Inst.CreateNode(++id);
                tdpnode->AppendNode(tdnode);
                tdnode->SetValue((char *)tdjson["key"].get<std::string>().c_str());
                //                tdnode->SetValue((char *)"something is better");
                if (tdjson["val"].is_object() || tdjson["val"].is_array())
                {
                    //auto start_5 = std::chrono::system_clock::now();
                    createTDTree(tdjson["val"], tdnode);
                    //auto end_5 = std::chrono::system_clock::now();
                    //std::chrono::duration<double> elapsed_5 = end_5 - start_5;
                    //std::cout << "Creating TD Tree " << elapsed_5.count() << std::endl;
                }
                else
                {
                    PString pStr = 0;
                    MemoryManager::Inst.CreateObject(&pStr);
                    std::string val = tdjson["val"].dump();
                    //                    std::replace(val.begin(), val.end(), '"', '\0');
                    val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
                    pStr->SetValue((char *)tdjson["val"].dump().c_str());
                    tdnode->SetEntityObj((PENTITY)pStr);
                    tdnode->SetLValue((char *)val.c_str());
                    //                    std::cout << ((PENTITY)tdnode->GetEntityObj())->ul_Type;

                    //                    tdnode->SetValue((char *)tdjson["val"].dump().c_str());
                }
                //                std::cout << (char *)std::to_string(tdjson["type"].get<int>()).c_str();
                tdnode->SetRValue((char *)std::to_string(tdjson["type"].get<int>()).c_str());
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
