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
#include "String.h"
#include "Entity.h"
#include "OTPParser.h"
#include "ResultGenerator.h"
#include "NamedPipeOperations.h"

using json = nlohmann::json;

int id = 0;

std::string run(Node* root, MSTRING querycode)
{
    DefFileReader dfr;
    MetaData* pMD = dfr.Read("/Users/MurtazaA/99X/Backend/MurtazaQLVersion/FlexibleComputerLanguage/tests/test6/Defs.txt");
    ScriptReader sr;
    ScriptReaderOutput op;
    bool bSucc = sr.ProcessScript(pMD->s_RuleFileName, pMD, op);
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

int main(int argc, const char * argv[])
{
    std::cout << "Starting.." << std::endl;
    int fdin;
    int fdout;
    
    // FIFO file path
    std::string sin = "/Users/MurtazaA/99X/Backend/MurtazaQLVersion/FlexibleComputerLanguage/FlexibleComputerLanguage/queryfifoin";
    std::string sout = "/Users/MurtazaA/99X/Backend/MurtazaQLVersion/FlexibleComputerLanguage/FlexibleComputerLanguage/queryfifoout";
    char *fifosin = (char *)sin.c_str();
    char *fifosout = (char *)sout.c_str();
    
    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(fifosin, 0666);
    mkfifo(fifosout, 0666);
    
    while (1)
    {
        // START
        fdin = open(fifosin, O_RDONLY);
        std::cout << "New start.." << std::endl;
        // First open in read only and read
        
        std::string requestString = NamedPipeOperations::readFromPipe(fdin);
        
        close(fdin);
        // END
        
        // PROCESS START
        json request = json::parse(requestString);
        std::string otps = request["otp"].get<std::string>();
        Node* r = OTPParser::OTPJSONToNodeTree(otps);
        std::string queryResults = "";
        for (auto& data : json::iterator_wrapper(request["queries"]))
        {
            json query = data.value();
            std::string queryString = query.get<std::string>();
            std::cout << queryString << std::endl;
            // HAVE TO WRITE FUNCTION TO RETURN RESULT JSON
            std::string result = run(r, queryString);
            
            if (queryResults.compare("") != 0)
            {
                queryResults = queryResults + ",\"" + result + "\"";
            }
            else
            {
                queryResults = queryResults + "\"" + result + "\"";
            }
        }
        std::string response = "{\"reqId\": " + request["reqId"].get<std::string>() + ", \"queries\": [" + queryResults + "]}";
        std::cout << response << std::endl;
        // PROCESS END
        
        // START
        std::string s = queryResults + " processed in server.";
        // Open FIFO for write only
        fdout = open(fifosout, O_WRONLY);
        
        NamedPipeOperations::writeToPipe(fdout, s);
        
        close(fdout);
        
        requestString = "";
        // END
    }
    return 0;
}


//int main(int argc, const char * argv[])
//{
//    if (argc > 1)
//    {
////        std::cout << argv[1];
////        std::string query = argv[1];
//        json request = json::parse(argv[1]);
//        std::string queryResults = "";
//        for (auto& data : json::iterator_wrapper(request["queries"]))
//        {
//            json query = data.value();
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
//    std::ifstream myfile ("/Users/MurtazaA/99X/Backend/tracified-backend/otp2.txt");
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
//        run(r, "$Y.SetValue($X.GetValue)\n$X.GetAggregatedValue.WriteToFile(D:\\99X\\Backend\\FlexibleComputerLanguage\\FlexibleComputerLanguage/report_Unix)");
////    Tests t1 = Tests();
////    t.RunTest3();
////    TestCaseExecuter texec = TestCaseExecuter();
//    //    texec.RegisterAllTestCases();
////    TestCaseExecuter tce;
////    TestCaseArgument tca;
////    tca.scriptsFolder = "/Users/MurtazaA/99X/Backend/MurtazaQLVersion/FlexibleComputerLanguage/Core/TestCases/files/";
////    tce.ExecuteAllTestCases(&tca);
//
//    return 0;
//}
