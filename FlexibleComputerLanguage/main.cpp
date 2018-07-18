//
//  main.cpp
//  FlexibleComputerLanguage
//
//  Created by Dileepa Jayathilaka on 5/8/18.
//  Copyright (c) 2018 Dileepa Jayathilaka. All rights reserved.
//

#include <iostream>
#include "Tests.h"
#include "TestCaseExecuter.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    Tests tt;
    //tt.RunTest3();
    
    TestCaseExecuter tce;
    TestCaseArgument tca;
    tca.scriptsFolder = "/Users/dileepaj/Dileepa/WorkDir/FlexibleComputerLanguage/Core/TestCases/files/";
    tce.ExecuteAllTestCases(&tca);
    
    return 0;
}

