//
//  TestCaseBase.cpp
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/12/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#include "TestCaseBase.h"

TestCaseExecutionResult::TestCaseExecutionResult() {
    succ = true;
    message = EMPTY_STRING;
}

TestCaseExecutionResult::TestCaseExecutionResult(bool success, MSTRING msg) :
succ(success), message(msg) {
    
}

TestCaseBase::TestCaseBase() {
    
}

TestCaseExecutionResult TestCaseBase::Execute(TestCaseArgument *arg) {
    TestCaseExecutionResult res;
    res.succ = false;
    res.message = _MSTR('This test case evaluation is not implemented yet!!');
    return res;
}
