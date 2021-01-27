//
// Created by murtazaa on 2021-01-13.
//

#ifndef FLEXIBLECOMPUTERLANGUAGE_MONGOTP_H
#define FLEXIBLECOMPUTERLANGUAGE_MONGOTP_H

#include "Node.h"

class MongoTP: public Node {

public:
    MongoTP(int ulId) : Node(ulId)  { }
    Node* queryProfilesAndTDPs(std::string stageIds, std::string tenantIds, std::string itemIds, std::string fromdate);
};

#endif //FLEXIBLECOMPUTERLANGUAGE_MONGOTP_H
