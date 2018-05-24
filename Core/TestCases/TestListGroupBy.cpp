//
//  TestListGroupBy.cpp
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/25/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#include "TestListGroupBy.h"
#include "TestCaseIncludes.h"

TestCaseExecutionResult TestListGroupBy::Execute(TestCaseArgument* arg) {
    PENTITYLIST list = new EntityList();
    list->push_back(new String("abc"));
    list->push_back(new String("abcde"));
    list->push_back(new String("def"));
    list->push_back(new String("abcd"));
    list->push_back(new String("dabc"));
    
    DefFileReader dfr;
	MetaData* pMD = dfr.Read(arg->scriptsFolder + _MSTR(TestListGroupByScriptDefs.txt));
	ScriptReader sr;
	ScriptReaderOutput op;
	bool bSucc = sr.ProcessScript(arg->scriptsFolder + _MSTR(TestListGroupByScript.txt), pMD, op);
	if(!bSucc)
	{
        return TestCaseExecutionResult(false, _MSTR(Failed to read script));
	}
	ExecutionContext ec;
	ec.p_mapFunctions = &op.map_Functions;
	ec.p_MD = pMD;
	ec.map_Var["BigList"] = list;
	op.p_ETL->Execute(&ec);
    PENTITYLIST result = (PENTITYLIST)(ec.map_Var["Result"]);
    
    if (result->size() != 3) {
        return TestCaseExecutionResult(false, _MSTR(Result list size is wrong));
    }
    
    EntityList::iterator ite = result->begin();
    PENTITYLIST ll = (PENTITYLIST)(*ite);
    if (ll->size() != 2) {
        return TestCaseExecutionResult(false, _MSTR(First constituent list size is wrong));
    }
    
    ++ite;
    ll = (PENTITYLIST)(*ite);
    if (ll->size() != 2) {
        return TestCaseExecutionResult(false, _MSTR(Second constituent list size is wrong));
    }
    
    ++ite;
    ll = (PENTITYLIST)(*ite);
    if (ll->size() != 1) {
        return TestCaseExecutionResult(false, _MSTR(Third constituent list size is wrong));
    }
    
    return TestCaseExecutionResult();
}
