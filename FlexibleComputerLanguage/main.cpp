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

#define     JSON_PARSE_ERROR            1
#define     JSON_TO_NODE_TREE_ERROR     2
#define     QUERY_LANGUAGE_ERROR        3

#define THREADS 3

INITIALIZE_EASYLOGGINGPP

// shared data
int id = 0;
pthread_mutex_t mutex_read;
pthread_mutex_t mutex_write;
pthread_cond_t ready_read;
pthread_cond_t ready_write;
std::string requestString;
std::string response;

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
    MetaData* pMD = dfr.Read("/home/murtaza/99X/C++/FlexibleComputerLanguage/tests/Defs.txt");
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
        return response;
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

void * readSlave(void *fifosin)
{
    int fdin;

    fdin = open((char *) fifosin, O_RDONLY);
    LOG(INFO) << "New start...";
    
    pthread_mutex_lock(&mutex_read);

    requestString = NamedPipeOperations::readFromPipe(fdin);
    
    pthread_cond_signal(&ready_read);
    pthread_mutex_unlock(&mutex_read);

    close(fdin);

    LOG(INFO) << requestString;

    return;
}

void * intermediateSlave(void *)
{
    pthread_mutex_lock(&mutex_read);
    pthread_cond_wait(&ready_read, &mutex_read);
    
    std::string intermediateRequest = requestString;
    requestString = "";

    pthread_mutex_unlock(&mutex_read);

    nlohmann::json request;
    try
    {
        request = nlohmann::json::parse(intermediateRequest);
    } catch (int ex)
    {
        LOG(ERROR) << "Request:" << request;
        throw JSON_PARSE_ERROR;
    }
    std::string type = request["type"].get<std::string>();

    std::string intermediateResponse = "";

    if ( type == "query")
    {
        intermediateResponse = processQuery(intermediateRequest, request);
    }
    else if ( type == "test")
    {
        intermediateResponse = processTest(intermediateRequest);
    }
    LOG(INFO) << intermediateResponse;

    pthread_mutex_lock(&mutex_write);
    
    response = intermediateResponse;

    pthread_cond_signal(&ready_write);
    pthread_mutex_unlock(&mutex_write);
}

void * writeSlave(void *fifosout)
{
    int fdout;

    fdout = open((char *)fifosout, O_WRONLY);

    pthread_mutex_lock(&mutex_write);
    pthread_cond_wait(&ready_write, &mutex_write);
    
    NamedPipeOperations::writeToPipe(fdout, response);
    
    pthread_mutex_unlock(&mutex_write);

    close(fdout);

    LOG(INFO) << "request wrapped up";

    return;
}

int main(int argc, const char * argv[])
{
    int i;
    pthread_t tid[THREADS];
    pthread_mutex_init(&mutex_read, NULL);
    pthread_mutex_init(&mutex_write, NULL);
    pthread_cond_init(&ready_read, NULL);
    pthread_cond_init(&ready_write, NULL);

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

    while (1)
    {
        std::string reqId = "0";

        // START
        try
        {
            pthread_create(&tid[0], NULL, readSlave, (void *)fifosin);
            pthread_create(&tid[1], NULL, intermediateSlave, NULL);
            pthread_create(&tid[2], NULL, writeSlave, (void *)fifosout);
        }
        catch (int ex)
        {
            int fdout = open(fifosout, O_WRONLY);
            NamedPipeOperations::writeToPipe(fdout, "{\"reqId\":" + reqId + "\", \"error\": {\"id\":\"" + std::to_string(ex) + "\", \"message\":\"Query language has failed.\"}}");
            mkfifo(fifosin, 0666);
            mkfifo(fifosout, 0666);
        }
    }
    for (i=0; i<THREADS; i++){
        pthread_join(tid[i], NULL);
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
