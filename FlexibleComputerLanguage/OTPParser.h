//
//  OTPParser.h
//  FlexibleComputerLanguage
//
//  Created by Murtaza Anverali on 7/5/18.
//  Copyright © 2018 Dileepa Jayathilaka. All rights reserved.
//

#include "json.hpp"
#include "CommonIncludes.h"

class OTPParser
{
  public:
    static void createTDTree(Document j, Node *parent);
    static Node *OTPJSONToNodeTree(std::string otpsString);
};
