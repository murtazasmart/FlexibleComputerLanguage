//
//  main.cpp
//  FlexibleComputerLanguage
//
//  Created by Dileepa Jayathilaka on 5/8/18.
//  Copyright (c) 2018 Dileepa Jayathilaka. All rights reserved.
//
// VERSION 2

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "Tests.h"
#include "json.hpp"
#include "MemMan.h"
#include "Node.h"
#include "DefFileReader.h"
#include "ScriptReader.h"
#include "MetaData.h"
#include "ExecutionTemplateList.h"
#include "ExecutionContext.h"
#include "TestCaseExecuter.h"
#include "Int.h"
#include "DateTime.h"
#include "Strings.h"
#include "Entity.h"
#include "OTPParser.h"
#include "ResultGenerator.h"
#include "NamedPipeOperations.h"
#include "easylogging++.h"
#include "Logger.h"
#include <iostream>
#include <memory>

#define     JSON_PARSE_ERROR            1
#define     JSON_TO_NODE_TREE_ERROR     2
#define     QUERY_LANGUAGE_ERROR        3

INITIALIZE_EASYLOGGINGPP

int id = 0;


#include "EntityList.h"

std::string run(Node* root, MSTRING querycode)
{
    // PENTITYLIST list = new EntityList();
    // list->push_back(new String("abc"));
    // list->push_back(new String("abcde"));
    // list->push_back(new String("def"));
    // list->push_back(new String("abcd"));
    // list->push_back(new String("dabc"));
    DefFileReader dfr;
    // CAUTION: This file path is hardcoded and can cause crashes. You have been warned!
    MetaData* pMD = dfr.Read("Defs.txt");
    ScriptReader sr;
    ScriptReaderOutput op;
//    bool bSucc = sr.ProcessScript(pMD->s_RuleFileName, pMD, op);
    bool bSucc = sr.ProcessScript(pMD, op, querycode);
    if(!bSucc)
    {
        std::wcout<<"\nFailed to read script\n";
    }
    ExecutionContext ec;
    ec.p_mapFunctions = &op.map_Functions;
    ec.p_MD = pMD;
    Node* pY = MemoryManager::Inst.CreateNode(++id);
    Node* pRESULT = MemoryManager::Inst.CreateNode(++id);
    std::string s = "52";
    root->SetValue((char *)s.c_str());
    ec.map_Var["X"] = root;
    ec.map_Var["Y"] = pY;
    ec.map_Var["RESULT"] = pRESULT;
    op.p_ETL->Execute(&ec);
    return ResultGenerator::CreateResult(pRESULT);
}

std::string processQuery(std::string requestString, nlohmann::json request)
{
    // START
    std::string reqId = "0";
    try
    {
        reqId = request["reqId"].get<std::string>();
        std::string otps = request["otp"].dump();
        Node *r;

        try {
            r = OTPParser::OTPJSONToNodeTree(otps);
        } catch (int ex)
        {
            LOG(ERROR) << "Request:" << request;
            throw JSON_TO_NODE_TREE_ERROR;
        }

        std::string queryResults = "";
        for (auto &data : nlohmann::json::iterator_wrapper(request["queries"]))
        {
            nlohmann::json query = data.value();
            std::string queryString = query.get<std::string>();
            //             HAVE TO WRITE FUNCTION TO RETURN RESULT JSON
            std::string result;

            try
            {
                result = run(r, queryString);
            } catch (int ex)
            {
                LOG(ERROR) << "OTPS:" <<  otps;
                LOG(ERROR) << "QueryString:" << queryString;
                throw QUERY_LANGUAGE_ERROR;
            }

            if (queryResults.compare("") != 0)
            {
                queryResults = queryResults + "," + result;
            } else
            {
                queryResults = queryResults + result;
            }
        }
        std::string response =
                "{\"reqId\": \"" + request["reqId"].get<std::string>() + "\", \"queries\": [" + queryResults + "]}";
        // PROCESS END
        // return response;
        return "{\"reqId\": \"1\", \"queries\": []}";
    }
    catch (int ex)
    {
        return "{\"reqId\":" + reqId + "\", \"error\": {\"id\":\"" + std::to_string(ex) + "\", \"message\":\"Query language has failed.\"}}";
    }
}


std::string processTest(std::string requestString)
{
    return requestString + "processed";
}

int main(int argc, const char * argv[])
{

//    Tests t = Tests();
//    t.RunTest1();
//    t.RunTest3();

    Logger::ConfigureLogger();

    LOG(INFO) << "Starting..";
    int fdin;
    int fdout;

    // FIFO file path
    std::string sin = "/tmp/queryfifoin";
    std::string sout = "/tmp/queryfifoout";
    char *fifosin = (char *)sin.c_str();
    char *fifosout = (char *)sout.c_str();

    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(fifosin, 0666);
    mkfifo(fifosout, 0666);

    while (1)
    {
        std::string reqId = "0";

        // START
        try
        {
            fdin = open(fifosin, O_RDONLY);
            LOG(INFO) << "New start..";
            // First open in read only and read

            std::string requestString = NamedPipeOperations::readFromPipe(fdin);

            close(fdin);
            // END

            LOG(INFO) << requestString;

            // PROCESS START
            nlohmann::json request;
            try
            {
                request = nlohmann::json::parse(requestString);
            } catch (int ex)
            {
                LOG(ERROR) << "Request:" << request;
                throw JSON_PARSE_ERROR;
            }
            std::string type = request["type"].get<std::string>();
            std::string response = "";
            if ( type == "query")
            {
                response = processQuery(requestString, request);
            }
            else if ( type == "test")
            {
                response = processTest(requestString);
            }
            LOG(INFO) << response;
            // PROCESS END

            // START
            // Open FIFO for write only
            fdout = open(fifosout, O_WRONLY);

            NamedPipeOperations::writeToPipe(fdout, response);

            close(fdout);

            LOG(INFO) << "request wrapped up";

            requestString = "";
            // END
        }
        catch (int ex)
        {
            NamedPipeOperations::writeToPipe(fdout, "{\"reqId\":" + reqId + "\", \"error\": {\"id\":\"" + std::to_string(ex) + "\", \"message\":\"Query language has failed.\"}}");
            mkfifo(fifosin, 0666);
            mkfifo(fifosout, 0666);
        }
    }
    return 0;
}


//int main(int argc, const char * argv[])
//{
//    if (argc > 1)
//    {
////        std::cout << argv[1];
////        std::string query = argv[1];
//        nlohmann::json request = nlohmann::json::parse(argv[1]);
//        std::string queryResults = "";
//        for (auto& data : nlohmann::json::iterator_wrapper(request["queries"]))
//        {
//            nlohmann::json query = data.value();
//            std::string queryString = query.get<std::string>();
//            std::cout << queryString << std::endl;
//            std::string result = "abcqwe";
//            if (queryResults.compare("") != 0)
//            {
//                queryResults = queryResults + ",\"" + result + "\"";
//            }
//            else
//            {
//                queryResults = queryResults + "\"" + result + "\"";
//            }
//        }
//        std::string response = "{\"reqId\": " + request["reqId"].get<std::string>() + ", \"queries\": [" + queryResults + "]}";
//        std::cout << response << std::endl;
//    }
//    std::string line;
//    std::string j;
//    std::ifstream myfile ("/home/murtaza/99X/tracified-backend/otp.txt");
//    if (myfile.is_open())
//    {
//        getline (myfile,line);
//        j = line;
//        myfile.close();
//    }
//    Tests t = Tests();
////    t.RunTest1();
//    Node* r = OTPParser::OTPJSONToNodeTree(j);
////    OTPParser.OTP
//    // HAVE TO WRITE FUNCTION TO RETURN RESULT JSON
//        std::string res = run(r, "$Y.SetValue($X.GetValue)\n$X.GetAggregatedValue.WriteToFile(D:\\99X\\Backend\\FlexibleComputerLanguage\\FlexibleComputerLanguage/report_Unix)");
//        std::cout << res;
////    Tests t1 = Tests();
////    t.RunTest3();
////    TestCaseExecuter texec = TestCaseExecuter();
//    //    texec.RegisterAllTestCases();
////    TestCaseExecuter tce;
////    TestCaseArgument tca;
////    tca.scriptsFolder = "/Users/MurtazaA/99X/Backend/MurtazaQLVersion/FlexibleComputerLanguage/Core/TestCases/files/";
////    tce.ExecuteAllTestCases(&tca);
//
//
////    PENTITYLIST list = new EntityList();
////    list->push_back(new String("abc"));
////    list->push_back(new String("abcde"));
////    list->push_back(new String("def"));
////    list->push_back(new String("abcd"));
////    list->push_back(new String("dabc"));
////
////    DefFileReader dfr;
////    MetaData* pMD = dfr.Read("/home/murtaza/99X/C++/FlexibleComputerLanguage/Core/TestCases/files/TestListGroupByScript.txt");
////    ScriptReader sr;
////    ScriptReaderOutput op;
////    bool bSucc = sr.ProcessScript(pMD->s_RuleFileName, pMD, op);
////    ExecutionContext ec;
////    ec.p_mapFunctions = &op.map_Functions;
////    ec.p_MD = pMD;
////
////    ec.map_Var["BigList"] = list;
////    op.p_ETL->Execute(&ec);
////    PENTITYLIST result = (PENTITYLIST)(ec.map_Var["Result"]);
//
//    return 0;
//}
