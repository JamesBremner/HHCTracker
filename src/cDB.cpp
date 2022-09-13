#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>


#include "cDB.h"

cDB::vperson_t cDB::nursebyDate( int att )
{
    myNurseList.clear();
    for (auto &v : myValue)
    {
        if (v.aid == eAttribute::role && v.value == "nurse")
        {
            // add nurse to list
            myNurseList.push_back(
                nurse(v.pid));
        }
    }
    // sort into date order
    sort( myNurseList,att);
    
    return myNurseList;
}

void cDB::sort(
    vperson_t& list,
    int att)
{
    using namespace std::chrono;

    // map the list persons keyed by the attribute date
    std::tm tm = {};
    std::map< 
        system_clock::time_point,
        person_t > mapped;
    auto today = system_clock::now();
    for( auto& p : list )
    {
        std::istringstream in{p.second[att]};
        in >> std::get_time(&tm, "%Y-%m-%d");
        auto tp = system_clock::from_time_t(std::mktime(&tm));
        if( tp < today ) {
            p.second[3] = "!!!EXPIRED!!!";
        }
        mapped.insert( 
            std::make_pair( tp, p ));
    }

    // copy persons back into list in keyed order
    list.clear();
    for( auto& x : mapped )
    {
        list.push_back( x.second );
    }
    
}