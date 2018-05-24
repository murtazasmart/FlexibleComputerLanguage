//
//  TestLoadFromCodeLibrary.cpp
//  FlexibleComputerLanguage
//
//  Created by Dileepa Jayathilaka on 5/24/18.
//  Copyright (c) 2018 Dileepa Jayathilaka. All rights reserved.
//

#include "TestLoadFromCodeLibrary.h"
#include "TestCaseIncludes.h"

TestCaseExecutionResult TestLoadFromCodeLibrary::Execute(TestCaseArgument* arg) {
    PNODE nodeX = new Node(1);
    PNODE nodeY = new Node(2);
    PNODE nodeZ = new Node(3);
    
    DefFileReader dfr;
	MetaData* pMD = dfr.Read(arg->scriptsFolder + _MSTR(TestLoadFromCodeLibraryScriptDefs.txt));
	ScriptReader sr;
	ScriptReaderOutput op;
	bool bSucc = sr.ProcessScript(arg->scriptsFolder + _MSTR(TestLoadFromCodeLibraryScript.txt), pMD, op);
	if(!bSucc)
	{
        return TestCaseExecutionResult(false, _MSTR(Failed to read script));
	}
	ExecutionContext ec;
	ec.p_mapFunctions = &op.map_Functions;
	ec.p_MD = pMD;
	ec.map_Var["X"] = nodeX;
    ec.map_Var["Y"] = nodeY;
    ec.map_Var["Z"] = nodeZ;
    ec.map_Var["RET"] = new Node(4);
	op.p_ETL->Execute(&ec);
    
    MULONG zChildCount = nodeZ->GetChildCount();
    if (zChildCount != 2) {
        return TestCaseExecutionResult(false, _MSTR(Node Z child count is wrong));
    }
    
    return TestCaseExecutionResult();
}
