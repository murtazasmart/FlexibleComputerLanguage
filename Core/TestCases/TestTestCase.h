//
//  TestTestCase.h
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/15/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#ifndef __LogAnalyzer__TestTestCase__
#define __LogAnalyzer__TestTestCase__

#include <iostream>
#include "TestCaseBase.h"

class TestTestCase : public TestCaseBase {
public:
    TestTestCase();
    TestCaseExecutionResult Execute(TestCaseArgument* arg);
};

#endif /* defined(__LogAnalyzer__TestTestCase__) */
