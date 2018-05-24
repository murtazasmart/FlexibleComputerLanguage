//
//  TestCaseBase.h
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/12/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#ifndef __LogAnalyzer__TestCaseBase__
#define __LogAnalyzer__TestCaseBase__

#include <iostream>
#include "CommonIncludes.h"

class TestCaseArgument {
public:
    MSTRING scriptsFolder;
};

class TestCaseExecutionResult {
public:
    bool succ;
    MSTRING message;
    
    TestCaseExecutionResult();
    TestCaseExecutionResult(bool success, MSTRING msg);
};

class TestCaseBase {
public:
    TestCaseBase();
    
    virtual TestCaseExecutionResult Execute(TestCaseArgument* arg);
};

#endif /* defined(__LogAnalyzer__TestCaseBase__) */
