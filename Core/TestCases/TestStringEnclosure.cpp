//
//  TestStringEnclosure.cpp
//  FlexibleComputerLanguage
//
//  Created by Dileepa Jayathilaka on 5/26/18.
//  Copyright (c) 2018 Dileepa Jayathilaka. All rights reserved.
//

#include "TestStringEnclosure.h"
#include "TestCaseIncludes.h"

TestCaseExecutionResult TestStringEnclosure::Execute(TestCaseArgument* arg) {
    PNODE node1 = new Node(1);
    PNODE node2 = new Node(2);
    PNODE node3 = new Node(3);
    PNODE node4 = new Node(4);
    PNODE node5 = new Node(5);
    PNODE node6 = new Node(6);
    PNODE node7 = new Node(7);
    PNODE node8 = new Node(8);
    
    node1->AppendNode(node2);
    node1->AppendNode(node3);
    
    node2->AppendNode(node4);
    node2->AppendNode(node5);
    node2->AppendNode(node6);
    
    node5->AppendNode(node7);
    node5->AppendNode(node8);
    
    node1->SetValue("adc");
    node2->SetValue("abc");
    node3->SetValue("deabcde");
    node4->SetValue("abcd");
    node5->SetValue("abd");
    node6->SetValue("abba");
    node7->SetValue("acb");
    node8->SetValue("dabc");
    
    DefFileReader dfr;
	MetaData* pMD = dfr.Read(arg->scriptsFolder + _MSTR(TestStringEnclosureScriptDefs.txt));
	ScriptReader sr;
	ScriptReaderOutput op;
	bool bSucc = sr.ProcessScript(arg->scriptsFolder + _MSTR(TestStringEnclosureScript.txt), pMD, op);
	if(!bSucc)
	{
        return TestCaseExecutionResult(false, _MSTR(Failed to read script));
	}
	ExecutionContext ec;
	ec.p_mapFunctions = &op.map_Functions;
	ec.p_MD = pMD;
	ec.map_Var["RootNode"] = node1;
	op.p_ETL->Execute(&ec);
    
    MSTRING str = node1->GetLVal();
    if (MSTRING(node1->GetLVal()) != "pqrs") {
        return TestCaseExecutionResult(false, _MSTR(Node 1 LValue is wrong));
    }
    if (MSTRING(node2->GetLVal()) != "xyz") {
        return TestCaseExecutionResult(false, _MSTR(Node 2 LValue is wrong));
    }
    if (MSTRING(node4->GetLVal()) != "xyz") {
        return TestCaseExecutionResult(false, _MSTR(Node 4 LValue is wrong));
    }
    if (MSTRING(node8->GetLVal()) != "xyz") {
        return TestCaseExecutionResult(false, _MSTR(Node 8 LValue is wrong));
    }
    
    return TestCaseExecutionResult();
}