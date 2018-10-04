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
    nlohmann::json otps = nlohmann::json::parse(otpsString);
    Node *root = MemoryManager::Inst.CreateNode(++id);
    int i = 0, j = 0, k = 0;
    for (auto &tp : nlohmann::json::iterator_wrapper(otps[0]))
    {
        nlohmann::json tpjson = tp.value();
        Node *tpnode = MemoryManager::Inst.CreateNode(++id);
        tpnode->SetValue((char *)tpjson["stageID"].get<std::string>().c_str());
        root->AppendNode(tpnode);
        for (auto &tdp : nlohmann::json::iterator_wrapper(tpjson["traceabilityDataPackets"]))
        {
            nlohmann::json tdpjson = tdp.value();
            Node *tdpnode = MemoryManager::Inst.CreateNode(++id);
            tdpnode->SetValue((char *)tdpjson["userID"].get<std::string>().c_str());
            tpnode->AppendNode(tdpnode);
            for (auto &td : nlohmann::json::iterator_wrapper(tdpjson["traceabilityData"]))
            {
                nlohmann::json tdjson = td.value();
                Node *tdnode = MemoryManager::Inst.CreateNode(++id);
                tdpnode->AppendNode(tdnode);
                tdnode->SetValue((char *)tdjson["key"].get<std::string>().c_str());
                //                tdnode->SetValue((char *)"something is better");
                if (tdjson["val"].is_object() || tdjson["val"].is_array())
                {
                    createTDTree(tdjson["val"], tdnode);
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
        }
    }
    return root;
}
