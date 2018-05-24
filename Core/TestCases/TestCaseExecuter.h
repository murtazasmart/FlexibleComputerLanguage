//
//  TestCaseExecuter.h
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/12/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#ifndef __LogAnalyzer__TestCaseExecuter__
#define __LogAnalyzer__TestCaseExecuter__

#include <iostream>
#include "CommonIncludes.h"
#include "TestCaseBase.h"

typedef std::map<MSTRING, TestCaseBase*> MAP_MSTRING_TESTCASEBASE;

class TestCaseExecuter {
public:
    TestCaseExecuter();
    void RegisterTestCase(MSTRING testName, TestCaseBase* testcase);
    void ExecuteAllTestCases(TestCaseArgument *arg);
    void ExecuteTestCase(MSTRING& testName, TestCaseArgument *arg);
    
private:
    MAP_MSTRING_TESTCASEBASE testCaseRegistry;
    
    void PrintTestCaseExecutionResult(MSTRING& testname, TestCaseExecutionResult& res);
    void RegisterAllTestCases();
};

#endif /* defined(__LogAnalyzer__TestCaseExecuter__) */
