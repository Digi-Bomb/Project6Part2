// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/generators.hpp>
#include <boost/uuid/uuid_io.hpp>

//use of boost library to generate UUIDs for client IDs, which ensures uniqueness
/*
int main() {
    boost::uuids::random_generator gen;
    boost::uuids::uuid id = gen();

    std::cout << "UUID: " << id << '\n';
    return 0;
}
*/

