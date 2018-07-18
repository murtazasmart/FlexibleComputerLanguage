//
//  TestListInnerCountWithScript.cpp
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/15/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#include "TestListInnerCountWithScript.h"
#include "EntityList.h"
#include "Null.h"
#include "MemMan.h"
#include "Int.h"
#include "DefFileReader.h"
#include "ScriptReader.h"
#include "MetaData.h"
#include "ExecutionContext.h"

TestCaseExecutionResult TestListInnerCountWithScript::Execute(TestCaseArgument* arg) {
    TestCaseExecutionResult res;
    PNull a[10];
    for (int i = 0; i < 10; ++i) {
        MemoryManager::Inst.CreateObject(&(a[i]));
    }
    // Create the following list
    // { {null, null, null}, {null, null}, {null, null, null, null, null} }
    // Get InnerCount for thos list should return {3, 2, 5}
    
    PENTITYLIST list1 = 0;
    MemoryManager::Inst.CreateObject(&list1);
    list1->push_back(a[0]);
    list1->push_back(a[1]);
    list1->push_back(a[2]);
    
    PENTITYLIST list2 = 0;
    MemoryManager::Inst.CreateObject(&list2);
    list2->push_back(a[3]);
    list2->push_back(a[4]);
    
    PENTITYLIST list3 = 0;
    MemoryManager::Inst.CreateObject(&list3);
    list3->push_back(a[5]);
    list3->push_back(a[6]);
    list3->push_back(a[7]);
    list3->push_back(a[8]);
    list3->push_back(a[9]);
    
    PENTITYLIST upperList = 0;
    MemoryManager::Inst.CreateObject(&upperList);
    upperList->push_back(list1);
    upperList->push_back(list2);
    upperList->push_back(list3);
    
    //PENTITYLIST result = upperList->GetInnerCount();
    DefFileReader dfr;
	MetaData* pMD = dfr.Read(arg->scriptsFolder + _MSTR(TestInnerCountScriptDefs.txt));
	ScriptReader sr;
	ScriptReaderOutput op;
	bool bSucc = sr.ProcessScript(arg->scriptsFolder + _MSTR(TestInnerCountScript.txt), pMD, op);
	if(!bSucc)
	{
        res.message = _MSTR(Failed to read script);
        res.succ = false;
        return res;
	}
	ExecutionContext ec;
	ec.p_mapFunctions = &op.map_Functions;
	ec.p_MD = pMD;
	ec.map_Var["BigList"] = upperList;
	op.p_ETL->Execute(&ec);
    PENTITYLIST result = (PENTITYLIST)(ec.map_Var["Result"]);
    
    if (result->size() != 3) {
        res.succ = false;
        res.message = _MSTR(Result list size is incorrect);
        return res;
    }
    
    result->SeekToBegin();
    PENTITY ent = result->GetCurrElem();
    unsigned long val = ((PInt)ent)->GetValue();
    if (val != 3) {
        res.succ = false;
        res.message = _MSTR(One inner count is wrong);
        return res;
    }
    
    result->Seek(1, false);
    ent = result->GetCurrElem();
    val = ((PInt)ent)->GetValue();
    if (val != 2) {
        res.succ = false;
        res.message = _MSTR(One inner count is wrong);
        return res;
    }
    
    result->Seek(1, false);
    ent = result->GetCurrElem();
    val = ((PInt)ent)->GetValue();
    if (val != 5) {
        res.succ = false;
        res.message = _MSTR(One inner count is wrong);
        return res;
    }
    
    res.succ = true;
    res.message = EMPTY_STRING;
    return res;
}
