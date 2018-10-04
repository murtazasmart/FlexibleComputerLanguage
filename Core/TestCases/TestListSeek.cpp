//
//  TestListSeek.cpp
//  FlexibleComputerLanguage
//
//  Created by Dileepa Jayathilaka on 7/18/18.
//  Copyright © 2018 Dileepa Jayathilaka. All rights reserved.
//

#include "TestListSeek.h"
#include "../Strings.h"
#include "EntityList.h"
#include "MemMan.h"
#include "TestCaseIncludes.h"

TestCaseExecutionResult TestListSeek::Execute(TestCaseArgument* arg) {
    TestCaseExecutionResult res;
    
//    PString str1 = 0;
//    MemoryManager::Inst.CreateObject(&str1);
//    str1->SetValue("1");
//
//    PString str2 = 0;
//    MemoryManager::Inst.CreateObject(&str2);
//    str2->SetValue("2");
//
//    PString str3 = 0;
//    MemoryManager::Inst.CreateObject(&str3);
//    str3->SetValue("3");
//
//    PString str4 = 0;
//    MemoryManager::Inst.CreateObject(&str4);
//    str4->SetValue("4");
//
//    PENTITYLIST list = 0;
//    MemoryManager::Inst.CreateObject(&list);
//    list->push_back(str1);
//    list->push_back(str2);
//    list->push_back(str3);
//    list->push_back(str4);
//
//    list->SeekToBegin();
//    PString str = (PString)list->GetCurrElem();
//    if (str->ToString() != "1") {
//        res.succ = false;
//        res.message = "reading str1 failed";
//        return res;
//    }
//
//    list->Seek(1, false);
//    str = (PString)list->GetCurrElem();
//    if (str->ToString() != "2") {
//        res.succ = false;
//        res.message = "reading str2 failed";
//        return res;
//    }
//
//    list->Seek(1, false);
//    str = (PString)list->GetCurrElem();
//    if (str->ToString() != "3") {
//        res.succ = false;
//        res.message = "reading str3 failed";
//        return res;
//    }
//
//    list->Seek(1, false);
//    str = (PString)list->GetCurrElem();
//    if (str->ToString() != "4") {
//        res.succ = false;
//        res.message = "reading str4 failed";
//        return res;
//    }
//
//    list->Seek(1, false);
//    if (list->GetCurrElem() != 0) {
//        res.succ = false;
//        res.message = "list end not properly detected.";
//        return res;
//    }
//
//    // Now check with the script
//    DefFileReader dfr;
//    MetaData* pMD = dfr.Read(arg->scriptsFolder + _MSTR(TestListSeekScriptDefs.txt));
//    ScriptReader sr;
//    ScriptReaderOutput op;
//    bool bSucc = sr.ProcessScript(arg->scriptsFolder + _MSTR(TestListSeekScript.txt), pMD, op);
//    if(!bSucc)
//    {
//        return TestCaseExecutionResult(false, _MSTR(Failed to read script));
//    }
//    ExecutionContext ec;
//    ec.p_mapFunctions = &op.map_Functions;
//    ec.p_MD = pMD;
//    ec.map_Var["List"] = list;
//    Node* root = new Node(1);
//    ec.map_Var["Result"] = root;
//    op.p_ETL->Execute(&ec);
//
//    if (root->GetChildCount() != 4) {
//        res.succ = false;
//        char msg[50];
//        snprintf(msg, 49, "Incorrect child count (%lu) after executing script", root->GetChildCount());
//        res.message = msg;
//        return res;
//    }
//
//    Node* node = root->GetFirstChild();
//    if (MSTRING(node->GetValue()) != "1") {
//        res.succ = false;
//        res.message = "1 not found in result";
//        return res;
//    }
//
//    node = node->GetRightSibling();
//    if (MSTRING(node->GetValue()) != "2") {
//        res.succ = false;
//        res.message = "2 not found in result";
//        return res;
//    }
//
//    node = node->GetRightSibling();
//    if (MSTRING(node->GetValue()) != "3") {
//        res.succ = false;
//        res.message = "3 not found in result";
//        return res;
//    }
//
//    node = node->GetRightSibling();
//    if (MSTRING(node->GetValue()) != "4") {
//        res.succ = false;
//        res.message = "4 not found in result";
//        return res;
//    }
//
//    res.succ = true;
    return res;
}
