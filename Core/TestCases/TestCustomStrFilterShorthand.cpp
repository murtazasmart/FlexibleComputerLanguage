//
//  TestCustomStrFilterShorthand.cpp
//  LogAnalyzer
//
//  Created by Dileepa Jayathilaka on 1/26/15.
//  Copyright (c) 2015 99x Eurocenter. All rights reserved.
//

#include "TestCustomStrFilterShorthand.h"
#include "TestCaseIncludes.h"

TestCaseExecutionResult TestCustomStrFilterShorthand::Execute(TestCaseArgument* arg) {
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
    
    node1->SetCustomString("abc");
    node2->SetCustomString("abc");
    node3->SetCustomString("bc");
    node4->SetCustomString("aab");
    node5->SetCustomString("");
    node6->SetCustomString("abc");
    node7->SetCustomString("abc");
    node8->SetCustomString("bc");
    
    DefFileReader dfr;
	MetaData* pMD = dfr.Read(arg->scriptsFolder + _MSTR(TestCustomStrFilterShorthandScriptDefs.txt));
	ScriptReader sr;
	ScriptReaderOutput op;
	bool bSucc = sr.ProcessScript(arg->scriptsFolder + _MSTR(TestCustomStrFilterShorthandScript.txt), pMD, op);
	if(!bSucc)
	{
        return TestCaseExecutionResult(false, _MSTR(Failed to read script));
	}
	ExecutionContext ec;
	ec.p_mapFunctions = &op.map_Functions;
	ec.p_MD = pMD;
	ec.map_Var["RootNode"] = node1;
	op.p_ETL->Execute(&ec);
    PENTITYLIST result = (PENTITYLIST)(ec.map_Var["Result"]);
    
    if (result->size() != 4) {
        return TestCaseExecutionResult(false, _MSTR(Result list size is wrong));
    }
    
    EntityList::iterator ite = result->begin();
    PNODE node = (PNODE)(*ite);
    if (node->GetID() != 1) {
        return TestCaseExecutionResult(false, _MSTR(First node is wrong));
    }
    
    ++ite;
    node = (PNODE)(*ite);
    if (node->GetID() != 2) {
        return TestCaseExecutionResult(false, _MSTR(Second node is wrong));
    }
    
    ++ite;
    node = (PNODE)(*ite);
    if (node->GetID() != 7) {
        return TestCaseExecutionResult(false, _MSTR(Third node is wrong));
    }
    
    ++ite;
    node = (PNODE)(*ite);
    if (node->GetID() != 6) {
        return TestCaseExecutionResult(false, _MSTR(Forth node is wrong));
    }
    
    return TestCaseExecutionResult();
}