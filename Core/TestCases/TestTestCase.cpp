//
//  TestTestCase.cpp
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/15/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#include "TestTestCase.h"

TestTestCase::TestTestCase() {
    
}

TestCaseExecutionResult TestTestCase::Execute(TestCaseArgument* arg) {
    TestCaseExecutionResult res;
    res.succ = true;
    res.message = _MSTR(Test testcase message);
    return res;
}
