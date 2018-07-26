//
//  TestListFilter.cpp
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/24/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#include "TestListFilter.h"
#include "../Strings.h"
#include "EntityList.h"
#include "MemMan.h"
#include "Value.h"
#include "DefFileReader.h"
#include "ScriptReader.h"
#include "ExecutionContext.h"

TestCaseExecutionResult TestListFilter::Execute(TestCaseArgument* arg) {
    TestCaseExecutionResult res;
    
    PString str1 = 0;
    MemoryManager::Inst.CreateObject(&str1);
    str1->SetValue("abcd");
    
    PString str2 = 0;
    MemoryManager::Inst.CreateObject(&str2);
    str2->SetValue("adbc");
    
    PString str3 = 0;
    MemoryManager::Inst.CreateObject(&str3);
    str3->SetValue("deabcef");
    
    PString str4 = 0;
    MemoryManager::Inst.CreateObject(&str4);
    str4->SetValue("dabd");
    
    PENTITYLIST list = 0;
    MemoryManager::Inst.CreateObject(&list);
    list->push_back(str1);
    list->push_back(str2);
    list->push_back(str3);
    list->push_back(str4);
    
    DefFileReader dfr;
	MetaData* pMD = dfr.Read(arg->scriptsFolder + _MSTR(TestListFilterScriptDefs.txt));
	ScriptReader sr;
	ScriptReaderOutput op;
	bool bSucc = sr.ProcessScript(arg->scriptsFolder + _MSTR(TestListFilterScript.txt), pMD, op);
	if(!bSucc)
	{
        res.message = _MSTR(Failed to read script);
        res.succ = false;
        return res;
	}
	ExecutionContext ec;
	ec.p_mapFunctions = &op.map_Functions;
	ec.p_MD = pMD;
	ec.map_Var["BigList"] = list;
	op.p_ETL->Execute(&ec);
    PENTITYLIST result = (PENTITYLIST)(ec.map_Var["Result"]);
    
    if (result->size() != 2) {
        res.succ = false;
        res.message = _MSTR(Resulting list size is incorrect);
        return res;
    }
    
    PString str = (PString)result->front();
    if (str->GetValue() != MSTRING("abcd")) {
        res.succ = false;
        res.message = _MSTR(First element is wrong);
        return res;
    }
    
    str = (PString)result->back();
    if (str->GetValue() != MSTRING("deabcef")) {
        res.succ = false;
        res.message = _MSTR(Second element is wrong);
        return res;
    }
    
    res.succ = true;
    res.message = EMPTY_STRING;
    return res;
}