//
//  main.cpp
//  FlexibleComputerLanguage
//
//  Created by Dileepa Jayathilaka on 5/8/18.
//  Copyright (c) 2018 Dileepa Jayathilaka. All rights reserved.
//
// VERSION 2

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
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
#include <pthread.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "dotenv.h"

//Testing purposes
#include <chrono>
#include <MongoDB.h>
#include <MongoTP.h>
#include <MongoReview.h>

#define JSON_PARSE_ERROR 1
#define JSON_TO_NODE_TREE_ERROR 2
#define QUERY_LANGUAGE_ERROR 3

#define THREADS 3

//using namespace rapidjson;

INITIALIZE_EASYLOGGINGPP

// shared data
int id = 0;

int readFlag = 0;
int writeFlag = 0;

pthread_mutex_t mutex_read;
pthread_mutex_t mutex_write;

std::string requestString;
std::string response;

//Testing purposes
std::chrono::duration<double> elapsed_time;

#include "EntityList.h"

Node* loadParams(std::string paramsString)
{
    int id = 0;
    rapidjson::Document params;
    params.Parse(paramsString.c_str());
    Node *pParams = MemoryManager::Inst.CreateNode(++id);
    pParams->SetValue("params");
    pParams->SetLValue("params");
    pParams->SetRValue("params");
    pParams->SetCustomString("params");
    for (rapidjson::Value::ConstMemberIterator data = params.MemberBegin(); data != params.MemberEnd(); ++data) {
        rapidjson::Value &jsonvalue = params[data->name.GetString()];
        PString pStr = 0;
        MemoryManager::Inst.CreateObject(&pStr);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        jsonvalue.Accept(writer);
        pStr->SetValue(buffer.GetString());

        Node *datanode = MemoryManager::Inst.CreateNode(++id);
        std::string val = buffer.GetString();
        //            std::replace(val.begin(), val.end(), '"', '\0');
        val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
        datanode->SetEntityObj((PENTITY) pStr);
        datanode->SetValue((char *) data->name.GetString());
        datanode->SetLValue((char *) val.c_str());
        pParams->AppendNode(datanode);
    }
    return pParams;
}

std::string run(Node *root, MSTRING querycode, std::string paramString = "{ \"testParam\": \"Placeholder\"}")
{
    // PENTITYLIST list = new EntityList();
    // list->push_back(new String("abc"));
    // list->push_back(new String("abcde"));
    // list->push_back(new String("def"));
    // list->push_back(new String("abcd"));
    // list->push_back(new String("dabc"));
    DefFileReader dfr;
    // CAUTION: This file path is hardcoded and can cause crashes. You have been warned!
    MetaData *pMD = dfr.Read("Defs.txt");
    ScriptReader sr;
    ScriptReaderOutput op;
    //    bool bSucc = sr.ProcessScript(pMD->s_RuleFileName, pMD, op);
    bool bSucc = sr.ProcessScript(pMD, op, querycode);
    if (!bSucc)
    {
        std::wcout << "\nFailed to read script\n";
    }
    ExecutionContext ec;
    ec.p_mapFunctions = &op.map_Functions;
    ec.p_MD = pMD;
    Node *pY = MemoryManager::Inst.CreateNode(++id);
    Node *pRESULT = MemoryManager::Inst.CreateNode(++id);
    std::string s = "52";
    root->SetValue((char *)s.c_str());
    ec.map_Var["X"] = root;
    ec.map_Var["Y"] = pY;
    ec.map_Var["RESULT"] = pRESULT;
    ec.map_Var["PARAMS"] = loadParams(paramString);
    op.p_ETL->Execute(&ec);
    return ResultGenerator::CreateResult(pRESULT);
}

// TODO add func. to add params to context dynamically

// TODO add function for processTDPQuery

std::string processOTPQuery(std::string requestString, rapidjson::Document& request)
{
    // START
    std::string reqId = "0";
    try
    {
        //LOG(ERROR) << "AA";
        reqId = request["reqId"].GetString();
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        request["obj"].Accept(writer);
        std::string otps = buffer.GetString();

        Node *r;

        try
        {
            //LOG(ERROR) << "AAA";
            r = OTPParser::OTPJSONToNodeTree(otps);
        }
        catch (int ex)
        {
            //request.Accept(writer);
            //LOG(ERROR) << "Request:" << buffer.GetString();
            throw JSON_TO_NODE_TREE_ERROR;
        }

        std::string queryResults = "";

        //LOG(ERROR) << "AAAAA";
        rapidjson::Value& queries = request["queries"];
        //for (auto &data : nlohmann::json::iterator_wrapper(request["queries"]))
        for (rapidjson::Value::ConstValueIterator data = queries.Begin(); data != queries.End(); ++data)
        {
            rapidjson::Value& query = (rapidjson::Value&)(*data);
            std::string queryString = query.GetString();
            //             HAVE TO WRITE FUNCTION TO RETURN RESULT JSON
            std::string result;

            try
            {
                if (request.HasMember("params"))
                {
                    rapidjson::StringBuffer buffer;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                    request["params"].Accept(writer);
                    result = run(r, queryString, buffer.GetString());
                } else {
                result = run(r, queryString);
            }
            }
            catch (int ex)
            {
                //LOG(ERROR) << "OTPS:" << otps;
                //LOG(ERROR) << "QueryString:" << queryString;
                throw QUERY_LANGUAGE_ERROR;
            }

            if (queryResults.compare("") != 0)
            {
                queryResults = queryResults + "," + result;
            }
            else
            {
                queryResults = queryResults + result;
            }
        }
        std::string resReqId = request["reqId"].GetString();
        std::string response =
            "{\"reqId\": \"" + resReqId + "\", \"queries\": [" + queryResults + "]}";
        // PROCESS END
        return response;
    }
    catch (int ex)
    {
        return "{\"reqId\":" + reqId + "\", \"error\": {\"id\":\"" + std::to_string(ex) + "\", \"message\":\"Query language has failed.\"}}";
    }
}

std::string processTDPQuery(std::string requestString, rapidjson::Document& request)
{
    // START
    std::string reqId = "0";
    try
    {
        //LOG(ERROR) << "AA";
        reqId = request["reqId"].GetString();
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        request["obj"].Accept(writer);
        std::string objString = buffer.GetString();

        Node *r;

        try
        {
            //LOG(ERROR) << "AAA";
            r = OTPParser::TDPJSONToNodeTree(objString);
        }
        catch (int ex)
        {
            //request.Accept(writer);
            //LOG(ERROR) << "Request:" << buffer.GetString();
            throw JSON_TO_NODE_TREE_ERROR;
        }

        std::string queryResults = "";

        //LOG(ERROR) << "AAAAA";
        rapidjson::Value& queries = request["queries"];
        //for (auto &data : nlohmann::json::iterator_wrapper(request["queries"]))
        for (rapidjson::Value::ConstValueIterator data = queries.Begin(); data != queries.End(); ++data)
        {
            rapidjson::Value& query = (rapidjson::Value&)(*data);
            std::string queryString = query.GetString();
            //             HAVE TO WRITE FUNCTION TO RETURN RESULT JSON
            std::string result;

            try
            {
                if (request.HasMember("params"))
                {
                    rapidjson::StringBuffer buffer;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                    request["params"].Accept(writer);
                    result = run(r, queryString, buffer.GetString());
                } else {
                result = run(r, queryString);
            }
            }
            catch (int ex)
            {
                //LOG(ERROR) << "OTPS:" << otps;
                //LOG(ERROR) << "QueryString:" << queryString;
                throw QUERY_LANGUAGE_ERROR;
            }

            if (queryResults.compare("") != 0)
            {
                queryResults = queryResults + "," + result;
            }
            else
            {
                queryResults = queryResults + result;
            }
        }
        std::string resReqId = request["reqId"].GetString();
        std::string response =
                "{\"reqId\": \"" + resReqId + "\", \"queries\": [" + queryResults + "]}";
        // PROCESS END
        return response;
    }
    catch (int ex)
    {
        return "{\"reqId\":" + reqId + "\", \"error\": {\"id\":\"" + std::to_string(ex) + "\", \"message\":\"Query language has failed.\"}}";
    }
}

std::string processTest(std::string requestString)
{
    return requestString + " processed";
}

void *readSlave(void *fifosin)
{
    LOG(INFO) << "readSlave started";
    int fdIn = open((char *)fifosin, O_RDONLY);
    FILE *readStream = NamedPipeOperations::openPipeToRead(fdIn);
    while (1)
    {
        if (readFlag == 0)
        {
            pthread_mutex_lock(&mutex_read);
            //LOG(INFO) << "New start...0";

            requestString = NamedPipeOperations::readFromPipe((FILE *)readStream);

            if (requestString.length() != 0)
            {
                // LOG(INFO) << "Something was read";
                //LOG(INFO) << "requestString " << requestString;
                readFlag = 1;
            }
            else
            {
                std::this_thread::sleep_for (std::chrono::milliseconds(10));
            }
            pthread_mutex_unlock(&mutex_read);
        }
        else
        {
            std::this_thread::sleep_for (std::chrono::milliseconds(10));
        }
    }

    NamedPipeOperations::closeReadPipe(readStream, fdIn);
    close(fdIn);
}

void *processSlave(void *)
{
    std::string intermediateRequest = "";
    std::string intermediateResponse = "";
    LOG(INFO) << "processSlave started";
    while (1)
    {
        if (readFlag == 1)
        {
            pthread_mutex_lock(&mutex_read);
            // LOG(INFO) << "New start...";

            intermediateRequest = requestString;
            requestString.clear();

            readFlag = 0;
            pthread_mutex_unlock(&mutex_read);
        }

        if (intermediateRequest.length() != 0)
        {
            //nlohmann::json request;
            rapidjson::Document request;
            try
            {
                //LOG(INFO) << "intermediateRequest " << intermediateRequest;
                request.Parse(intermediateRequest.c_str());
                //rapidjson::StringBuffer buffer;
                //rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                //request.Accept(writer);
                //LOG(INFO)  << "Request:" << buffer.GetString();
            }
            catch (int ex)
            {
                LOG(ERROR) << "Request:" << (std::string)request.GetString();
                throw JSON_PARSE_ERROR;
            }
            std::string type = request["type"].GetString();
            if (type == "otpquery" || type == "otptemplatequery")
            {
                intermediateResponse = processOTPQuery(intermediateRequest, request);
            }
            else if (type == "tdpquery")
            {
                intermediateResponse = processTDPQuery(intermediateRequest, request);
            }
            else if (type == "test")
            {
                intermediateResponse = processTest(intermediateRequest);
            }
            intermediateRequest.clear();
        }

        if ((writeFlag == 0) && (intermediateResponse.length() != 0))
        {
            pthread_mutex_lock(&mutex_write);

            response = intermediateResponse;
            // LOG(INFO) << "intermediateResponse " << intermediateResponse;
            intermediateResponse.clear();
            writeFlag = 1;

            pthread_mutex_unlock(&mutex_write);
        }
        else
        {
            std::this_thread::sleep_for (std::chrono::milliseconds(10));
        }
    }
}

void *writeSlave(void *fifosout)
{
    LOG(INFO) << "writeSlave started";

    while (1)
    {
        if (writeFlag == 1)
        {
            pthread_mutex_lock(&mutex_write);
            //LOG(INFO) << "New start...2";
            int fdOut = open((char *)fifosout, O_WRONLY);
            FILE *writeStream = NamedPipeOperations::openPipeToWrite(fdOut);

            NamedPipeOperations::writeToPipe((FILE *)writeStream, response);

            LOG(INFO) << "response " << response;
            response.clear();
            writeFlag = 0;
            pthread_mutex_unlock(&mutex_write);

            NamedPipeOperations::closeWritePipe(writeStream, fdOut);
            close(fdOut);
            LOG(INFO) << "request wrapped up";
        }
        else
        {
            std::this_thread::sleep_for (std::chrono::milliseconds(10));
        }
    }
}

int main(int argc, const char *argv[])
{
  auto& dotenv = dotenv::env;  // Reference re-naming
//   MongoDB* m = MongoDB::getInstance();
//   MongoTP* tp = new MongoTP(80005);
//   tp->queryProfilesAndTDPs("104", "360d88f0-047a-11eb-af2a-f9e4ff9a11e7", "5fd9f23fb4f953a6f26e3bc1,5fd9f220b4f953c5d36e3bc0","");
//    MongoReview* mr = new MongoReview(80006);
//    mr->queryReviews("5d41258804613f0001be6bc2|5d08bf42302ab000013ad0d8");
   int i;
   pthread_t tid[THREADS];
   pthread_mutex_init(&mutex_read, NULL);
   pthread_mutex_init(&mutex_write, NULL);

    //    Tests t = Tests();
    //    t.RunTest1();
    //    t.RunTest3();

    Logger::ConfigureLogger();

    LOG(INFO) << "Starting..";

    // FIFO file path
    std::string pathin = getenv("QL_PIPE_FIFO_IN") == 0 ? dotenv["QL_PIPE_FIFO_IN"] : getenv("QL_PIPE_FIFO_IN");
    std::string pathout = getenv("QL_PIPE_FIFO_OUT") == 0 ? dotenv["QL_PIPE_FIFO_OUT"] : getenv("QL_PIPE_FIFO_OUT");
    std::string sin = "/tmp/" + pathin;
    std::string sout = "/tmp/" + pathout;
    char *fifosin = (char *)sin.c_str();
    char *fifosout = (char *)sout.c_str();

    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(fifosin, 0666);
    mkfifo(fifosout, 0666);

    std::string reqId = "0";

    // START
    try
    {
        pthread_create(&tid[0], NULL, readSlave, (void *)fifosin);
        pthread_create(&tid[1], NULL, processSlave, NULL);
        pthread_create(&tid[2], NULL, writeSlave, (void *)fifosout);
        for (i = 0; i < THREADS; i++)
        {
            pthread_join(tid[i], NULL);
        }
    }
    catch (int ex)
    {
        int fdout = open(fifosout, O_WRONLY);
        FILE *stream = NamedPipeOperations::openPipeToWrite(fdout);
        NamedPipeOperations::writeToPipe(stream, "{\"reqId\":" + reqId + "\", \"error\": {\"id\":\"" + std::to_string(ex) + "\", \"message\":\"Query language has failed.\"}}");
        NamedPipeOperations::closeWritePipe(stream, fdout);
        close(fdout);
        mkfifo(fifosin, 0666);
        mkfifo(fifosout, 0666);
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
