//
//  TestListFilter.h
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/24/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#ifndef __LogAnalyzer__TestListFilter__
#define __LogAnalyzer__TestListFilter__

#include <iostream>
#include "TestCaseBase.h"

class TestListFilter : public TestCaseBase {
public:
    virtual TestCaseExecutionResult Execute(TestCaseArgument* arg);
};

#endif /* defined(__LogAnalyzer__TestListFilter__) */
