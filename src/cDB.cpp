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
    v.aid = eAttribute::service;
    v.value = service;
    myValue.push_back(v);
    save();
}
void cDB::addPatient(
    const std::vector<std::string> &vals)
{
    sValue v;
    v.aid = eAttribute::name;
    v.pid = ++lastPID;
    v.value = vals[0];
    myValue.push_back(v);
    v.aid = eAttribute::role;
    v.value = "patient";
    myValue.push_back(v);
    v.aid = eAttribute::expire;
    v.value = vals[1];
    myValue.push_back(v);
    v.aid = eAttribute::certification;
    v.value = vals[2];
    myValue.push_back(v);
    v.aid = eAttribute::authorization;
    v.value = vals[3];
    myValue.push_back(v);
    v.aid = eAttribute::supplies;
    v.value = vals[4];
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
            case eAttribute::service:
                v.value = service;
                break;
            default:
                break;
            }
        }
    }
    save();
}

void cDB::updatePatient(
    int pid,
    const std::vector<std::string> &vals)
{
        for (auto &v : myValue)
    {
        if (v.pid == pid)
        {
            switch (v.aid)
            {
            case eAttribute::name:
                v.value = vals[0];
                break;
            case eAttribute::expire:
                v.value = vals[1];
                break;
            case eAttribute::certification:
                v.value = vals[2];
                break;
            case eAttribute::authorization:
                v.value = vals[3];
                break;
            case eAttribute::supplies:
                v.value = vals[4];
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
                nursePID(v.pid));
        }
    }
    // sort into date order
    sort(myNurseList, att);

    return myNurseList;
}

cDB::vperson_t cDB::patientbyDate(int att)
{
    myPatientList.clear();
    for (auto &v : myValue)
    {
        if (v.aid == eAttribute::role && v.value == "patient")
        {
            myPatientList.push_back(
                patientPID(v.pid));
        }
    }

    sort(myPatientList, att);

    return myPatientList;
}

cDB::person_t cDB::nurselist(int listIndex)
{
    return nursePID(
        myNurseList[listIndex].first);
}
cDB::person_t cDB::nursePID(int pid)
{
    person_t ret;
    ret.first = pid;
    ret.second.resize(4);
    for (auto &v : myValue)
    {
        if (v.aid == eAttribute::name && v.pid == pid)
            ret.second[0] = v.value;
        if (v.aid == eAttribute::nurselicence && v.pid == pid)
            ret.second[1] = v.value;
        if (v.aid == eAttribute::service && v.pid == pid)
            ret.second[2] = v.value;
    }
    return ret;
}
cDB::person_t cDB::patientlist(int listIndex)
{
    return patientPID(
        myPatientList[listIndex].first);
}
cDB::person_t cDB::patientPID(int pid)
{
    person_t ret;
    ret.first = pid;
    ret.second.resize(5);
    for (auto &v : myValue)
    {
        if (v.aid == eAttribute::name && v.pid == pid)
            ret.second[0] = v.value;
        if (v.aid == eAttribute::expire && v.pid == pid)
            ret.second[1] = v.value;
        if (v.aid == eAttribute::certification && v.pid == pid)
            ret.second[2] = v.value;
        if (v.aid == eAttribute::authorization && v.pid == pid)
            ret.second[3] = v.value;
        if (v.aid == eAttribute::supplies && v.pid == pid)
            ret.second[4] = v.value;
    }
    return ret;
}

long long cDB::secs(
    const cDB::person_t &p,
    int index)
{
    using namespace std::chrono;
    std::tm tm = {};
    std::istringstream ina{p.second[index]};
    ina >> std::get_time(&tm, "%Y-%m-%d");
    return system_clock::from_time_t(std::mktime(&tm)).time_since_epoch().count();
}

void cDB::sort(
    vperson_t &list,
    int att)
{
    using namespace std::chrono;

    std::sort(
        list.begin(), list.end(),
        [&](person_t &a, person_t &b) -> bool
        {
            return (secs(a, att) < secs(b, att));
        });

    // // map the list persons keyed by the attribute date
    // std::tm tm = {};
    // std::map<
    //     system_clock::time_point,
    //     person_t>
    //     mapped;
    // auto today = system_clock::now();
    // for (auto &p : list)
    // {
    //     std::istringstream in{p.second[att]};
    //     in >> std::get_time(&tm, "%Y-%m-%d");
    //     auto tp = system_clock::from_time_t(std::mktime(&tm));
    //     if (tp < today)
    //     {
    //         p.second.push_back("!!!EXPIRED!!!");
    //     }
    //     mapped.insert(
    //         std::make_pair(tp, p));
    // }

    // // copy persons back into list in keyed order
    // list.clear();
    // for (auto &x : mapped)
    // {
    //     list.push_back(x.second);
    // }
}
std::string cDB::sValue::text_for_file()
{
    std::string sval = value;
    if (aid == eAttribute::name)
    {
        int p = sval.find(" ");
        while (p != -1)
        {
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
    if (aid != eAttribute::name)
        return;
    int p = value.find("_");
    while (p != -1)
    {
        value[p] = ' ';
        p = value.find("_");
    }
}
void cDB::save()
{
    std::ofstream f(myfname, std::ofstream::trunc);
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
    while (std::getline(ifs, line))
    {
        // std::cout << line << "\n";
        std::istringstream ss(line);
        int i;
        ss >> i;
        v.aid = (eAttribute)i;
        ss >> v.pid;
        if (v.pid > lastPID)
            lastPID = v.pid;
        ss >> v.value;
        v.text_from_file();
        myValue.push_back(v);
    }
}