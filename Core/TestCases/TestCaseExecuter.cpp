//
//  TestCaseExecuter.cpp
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/12/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#include "TestCaseExecuter.h"
#include "TestTestCase.h"
#include "TestListInnerCount.h"
#include "TestListInnerCountWithScript.h"
#include "TestListFilter.h"
#include "TestListGroupBy.h"
#include "TestFilterSubtree.h"
#include "TestCustomStrFilterShorthand.h"
#include "TestLoadFromCodeLibrary.h"
#include "TestStringEnclosure.h"
#include "TestListSeek.h"
#include "TestCompound1.h"

TestCaseExecuter::TestCaseExecuter() {
    RegisterAllTestCases();
}

void TestCaseExecuter::RegisterTestCase(MSTRING testName, TestCaseBase *testcase) {
    testCaseRegistry[testName] = testcase;
}

void TestCaseExecuter::RegisterAllTestCases() {
    //RegisterTestCase(_MSTR(TestTestCase), new TestTestCase);
    RegisterTestCase(_MSTR(TestListInnerCount), new TestListInnerCount);
    RegisterTestCase(_MSTR(TestListInnerCountWithScript), new TestListInnerCountWithScript);
    RegisterTestCase(_MSTR(TestListFilter), new TestListFilter);
    RegisterTestCase(_MSTR(TestListGroupBy), new TestListGroupBy);
    RegisterTestCase(_MSTR(TestFilterSubtree), new TestFilterSubtree);
    RegisterTestCase(_MSTR(TestCustomStrFilterShorthand), new TestCustomStrFilterShorthand);
    RegisterTestCase(_MSTR(TestLoadFromCodeLibrary), new TestLoadFromCodeLibrary);
    RegisterTestCase(_MSTR(TestStringEnclosure), new TestStringEnclosure);
    RegisterTestCase(_MSTR(TestListSeek), new TestListSeek);
    RegisterTestCase(_MSTR(TestCompound1), new TestCompound1);
}

void TestCaseExecuter::ExecuteAllTestCases(TestCaseArgument *arg) {
    int passed = 0;
    int failed = 0;
    MAP_MSTRING_TESTCASEBASE::iterator ite = testCaseRegistry.begin();
    MAP_MSTRING_TESTCASEBASE::iterator end = testCaseRegistry.end();
    for ( ; ite != end; ++ite) {
        MSTRING testname = (*ite).first;
        TestCaseBase *testcase = (*ite).second;
        TestCaseExecutionResult res = testcase->Execute(arg);
        PrintTestCaseExecutionResult(testname, res);
        if (res.succ) {
            ++passed;
        } else {
            ++failed;
        }
    }
    std::cout << "\nTest suite execution complete. Passed: " << passed << "; failed: " << failed << "\n";
}

void TestCaseExecuter::ExecuteTestCase(MSTRING &testName, TestCaseArgument *arg) {
    MAP_MSTRING_TESTCASEBASE::iterator ite = testCaseRegistry.find(testName);
    if (ite == testCaseRegistry.end()) {
        std::cout << "\nTest case " << testName << " is not registered.";
    } else {
        TestCaseBase *testcase = (*ite).second;
        TestCaseExecutionResult res = testcase->Execute(arg);
        PrintTestCaseExecutionResult(testName, res);
    }
}

void TestCaseExecuter::PrintTestCaseExecutionResult(MSTRING& testname, TestCaseExecutionResult& res) {
    std::cout << "\nTest case " << testname << (res.succ ? " Passed." : " Failed.");
    if (res.message.length() > 0) {
        std::cout << "\n" << res.message;
    }
}
