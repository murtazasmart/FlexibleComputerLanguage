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
#include <pthread.h>

//Testing purposes
#include  <chrono>

#define JSON_PARSE_ERROR 1
#define JSON_TO_NODE_TREE_ERROR 2
#define QUERY_LANGUAGE_ERROR 3

#define THREADS 3

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

std::string run(Node *root, MSTRING querycode)
{
    // PENTITYLIST list = new EntityList();
    // list->push_back(new String("abc"));
    // list->push_back(new String("abcde"));
    // list->push_back(new String("def"));
    // list->push_back(new String("abcd"));
    // list->push_back(new String("dabc"));
    DefFileReader dfr;
    MetaData *pMD = dfr.Read("/home/sam/FlexibleComputerLanguage/FlexibleComputerLanguage/tests/Defs.txt");
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

        try
        {
            auto pt15 = std::chrono::system_clock::now(); // testing purposes
            r = OTPParser::OTPJSONToNodeTree(otps);
            auto pt16 = std::chrono::system_clock::now(); // testing purposes
            elapsed_time = pt16-pt15;
            LOG(INFO) << "Time elapsed processQuery 1st part: " << elapsed_time.count();
        }
        catch (int ex)
        {
            LOG(ERROR) << "Request:" << request;
            throw JSON_TO_NODE_TREE_ERROR;
        }

        std::string queryResults = "";
        auto pt17 = std::chrono::system_clock::now(); // testing purposes
        for (auto &data : nlohmann::json::iterator_wrapper(request["queries"]))
        {
            nlohmann::json query = data.value();
            std::string queryString = query.get<std::string>();
            //             HAVE TO WRITE FUNCTION TO RETURN RESULT JSON
            std::string result;

            try
            {
                auto pt19 = std::chrono::system_clock::now(); // testing purposes
                result = run(r, queryString);
                auto pt20 = std::chrono::system_clock::now(); // testing purposes
                elapsed_time = pt20-pt19;
                LOG(INFO) << "Time elapsed processQuery 2nd part: " << elapsed_time.count();
            }
            catch (int ex)
            {
                LOG(ERROR) << "OTPS:" << otps;
                LOG(ERROR) << "QueryString:" << queryString;
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
        auto pt18 = std::chrono::system_clock::now(); // testing purposes
        elapsed_time = pt18-pt17;
        LOG(INFO) << "Time elapsed processQuery 3rd part: " << elapsed_time.count();
        std::string response =
            "{\"reqId\": \"" + request["reqId"].get<std::string>() + "\", \"queries\": [" + queryResults + "]}";
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
    auto pt1 = std::chrono::system_clock::now(); // testing purposes
    LOG(INFO) << "readSlave started";
    int fdIn = open((char *)fifosin, O_RDONLY);
    FILE *readStream = NamedPipeOperations::openPipeToRead(fdIn);
    auto pt2 = std::chrono::system_clock::now(); // testing purposes
    elapsed_time = pt2-pt1;
    LOG(INFO) << "Time elapsed readslave header: " << elapsed_time.count();
    while (1)
    {
        if (readFlag == 0)
        {
            auto pt3 = std::chrono::system_clock::now(); // testing purposes
            pthread_mutex_lock(&mutex_read);
            //LOG(INFO) << "New start...0";

            requestString = NamedPipeOperations::readFromPipe((FILE *)readStream);

            if (requestString.length() != 0)
            {
                // LOG(INFO) << "requestString " << requestString;
                readFlag = 1;
            }
            pthread_mutex_unlock(&mutex_read);
            auto pt4 = std::chrono::system_clock::now(); // testing purposes
            elapsed_time = pt4-pt3; // testing purposes
            LOG(INFO) << "Time elapsed readslave task: " << elapsed_time.count(); // testing purposes
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
            auto pt5 = std::chrono::system_clock::now(); // testing purposes
            pthread_mutex_lock(&mutex_read);
            //LOG(INFO) << "New start...1";

            intermediateRequest = requestString;
            requestString.clear();

            readFlag = 0;
            pthread_mutex_unlock(&mutex_read);
            auto pt6 = std::chrono::system_clock::now(); // testing purposes
            elapsed_time = pt6-pt5; // testing purposes
            LOG(INFO) << "Time elapsed processslave 1st task: " << elapsed_time.count(); // testing purposes
        }

        if (intermediateRequest.length() != 0)
        {
            nlohmann::json request;
            try
            {
                //LOG(INFO) << "intermediateRequest " << intermediateRequest;
                auto pt7 = std::chrono::system_clock::now(); // testing purposes
                request = nlohmann::json::parse(intermediateRequest);
                auto pt8 = std::chrono::system_clock::now(); // testing purposes
                elapsed_time = pt8-pt7; // testing purposes
                LOG(INFO) << "Time elapsed processslave 2nd task: " << elapsed_time.count(); // testing purposes
            }
            catch (int ex)
            {
                LOG(ERROR) << "Request:" << request;
                throw JSON_PARSE_ERROR;
            }
            std::string type = request["type"].get<std::string>();
            auto pt9 = std::chrono::system_clock::now(); // testing purposes

            if (type == "query")
            {
                intermediateResponse = processQuery(intermediateRequest, request);
            }
            else if (type == "test")
            {
                intermediateResponse = processTest(intermediateRequest);
            }
            auto pt10 = std::chrono::system_clock::now(); // testing purposes
            elapsed_time = pt10-pt9; // testing purposes
            LOG(INFO) << "Time elapsed processslave 3rd task: " << elapsed_time.count(); // testing purposes
            intermediateRequest.clear();
        }

        if ((writeFlag == 0) && (intermediateResponse.length() != 0))
        {
            auto pt11 = std::chrono::system_clock::now(); // testing purposes
            pthread_mutex_lock(&mutex_write);

            response = intermediateResponse;
            //LOG(INFO) << "intermediateResponse " << intermediateResponse;
            intermediateResponse.clear();
            writeFlag = 1;

            pthread_mutex_unlock(&mutex_write);
            auto pt12 = std::chrono::system_clock::now(); // testing purposes
            elapsed_time = pt12-pt11; // testing purposes
            LOG(INFO) << "Time elapsed processslave 4th task: " << elapsed_time.count(); // testing purposes
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
            auto pt13 = std::chrono::system_clock::now(); // testing purposes
            int fdOut = open((char *)fifosout, O_WRONLY);
            FILE *writeStream = NamedPipeOperations::openPipeToWrite(fdOut);
            
            NamedPipeOperations::writeToPipe((FILE *)writeStream, response);

            //LOG(INFO) << "response " << response;
            response.clear();
            writeFlag = 0;
            pthread_mutex_unlock(&mutex_write);

            NamedPipeOperations::closeWritePipe(writeStream, fdOut);
            close(fdOut);
            LOG(INFO) << "request wrapped up";
            auto pt14 = std::chrono::system_clock::now(); // testing purposes
            elapsed_time = pt14-pt13; // testing purposes
            LOG(INFO) << "Time elapsed writeslave task: " << elapsed_time.count(); // testing purposes
        }
    }
}

int main(int argc, const char *argv[])
{
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
    std::string sin = "/tmp/queryfifoin";
    std::string sout = "/tmp/queryfifoout";
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
