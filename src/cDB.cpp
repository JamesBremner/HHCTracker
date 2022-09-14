#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <algorithm>

#include "cDB.h"

int cDB::lastPID = -1;

cDB::cDB()
{
    load();
}

void cDB::addNurse(
    const std::string &name,
    const std::string &licence,
    const std::string &service)
{
    sValue v;
    v.aid = eAttribute::name;
    v.pid = ++lastPID;
    v.value = name;
    myValue.push_back(v);
    v.aid = eAttribute::role;
    v.value = "nurse";
    myValue.push_back(v);
    v.aid = eAttribute::nurselicence;
    v.value = licence;
    myValue.push_back(v);
    v.aid = eAttribute::inService;
    v.value = service;
    myValue.push_back(v);
    save();
}
void cDB::addPatient(
    const std::string &name)
{
    sValue v;
    v.aid = eAttribute::name;
    v.pid = ++lastPID;
    v.value = name;
    myValue.push_back(v);
    v.aid = eAttribute::role;
    v.value = "patient";
    save();
}

void cDB::updateNurse(
    int pid,
    const std::string &name,
    const std::string &licence,
    const std::string &service)
{
    for (auto &v : myValue)
    {
        if (v.pid == pid)
        {
            switch (v.aid)
            {
            case eAttribute::name:
                v.value = name;
                break;
            case eAttribute::nurselicence:
                v.value = licence;
                break;
            case eAttribute::inService:
                v.value = service;
                break;
            default:
                break;
            }
        }
    }
    save();
}

cDB::vperson_t cDB::nursebyDate(int att)
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
    sort(myNurseList, att);

    return myNurseList;
}

void cDB::sort(
    vperson_t &list,
    int att)
{
    using namespace std::chrono;

    // map the list persons keyed by the attribute date
    std::tm tm = {};
    std::map<
        system_clock::time_point,
        person_t>
        mapped;
    auto today = system_clock::now();
    for (auto &p : list)
    {
        std::istringstream in{p.second[att]};
        in >> std::get_time(&tm, "%Y-%m-%d");
        auto tp = system_clock::from_time_t(std::mktime(&tm));
        if (tp < today)
        {
            p.second[3] = "!!!EXPIRED!!!";
        }
        mapped.insert(
            std::make_pair(tp, p));
    }

    // copy persons back into list in keyed order
    list.clear();
    for (auto &x : mapped)
    {
        list.push_back(x.second);
    }
}
std::string cDB::sValue::text_for_file()
{
    std::string sval = value;
    if( aid == eAttribute::name ) {
        int p = sval.find(" ");
        while( p != -1 ) {
            sval[p] = '_';
             p = sval.find(" ");
        }
    }
    std::stringstream ss;
    ss << (int)aid << " " << pid << " " << sval << "\n";
    return ss.str();
}
void cDB::sValue::text_from_file()
{
    if( aid != eAttribute::name )
        return;
    int p = value.find("_");
    while( p != -1)
    {
        value[p] = ' ';
        p = value.find("_");
    }
}
void cDB::save()
{
    std::ofstream f(myfname,std::ofstream::trunc);
    if (!f.is_open())
        throw std::runtime_error(
            "Cannot open database");
    for (auto &v : myValue)
    {
        f << v.text_for_file();
    }
}
void cDB::load()
{
    myfname = "hhct.txt";
    std::ifstream ifs(myfname);
    if (!ifs.is_open())
        return;
    std::string line("x");
    sValue v;
    while( std::getline(ifs,line))
    {
        //std::cout << line << "\n";
        std::istringstream ss( line );
        int i;
        ss >> i;
        v.aid = (eAttribute) i;
        ss >> v.pid;
        ss >> v.value;
        v.text_from_file();
        myValue.push_back( v );
    }
}