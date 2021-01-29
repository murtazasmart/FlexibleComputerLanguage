//
// Created by murtazaa on 2021-01-18.
//

#ifndef FLEXIBLECOMPUTERLANGUAGE_MONGOREVIEW_H
#define FLEXIBLECOMPUTERLANGUAGE_MONGOREVIEW_H

class MongoReview: public Node {

public:
    MongoReview(int ulId) : Node(ulId)  { }
    Node* queryReviews(std::string profileIds);
    Node* queryIdentifiers(std::string identifiers);
};

#endif //FLEXIBLECOMPUTERLANGUAGE_MONGOREVIEW_H
