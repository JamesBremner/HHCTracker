#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include <inputbox.h>
#include "cStarterGUI.h"

#include "cDB.h"

int cDB::lastPID = -1;

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "HHC Tracker",
              {50, 50, 1000, 500}),
          tabs(wex::maker::make<wex::tabbed>(fm)),
          plNurse(wex::maker::make<wex::panel>(tabs)),
          plPatient(wex::maker::make<wex::panel>(tabs)),
          lbNursePanel(wex::maker::make<wex::label>(plNurse)),
          lbNurseOrder(wex::maker::make<wex::label>(plNurse)),
          chNurseOrder(wex::maker::make<wex::choice>(plNurse)),
          lsNurse(wex::maker::make<wex::list>(plNurse)),
          bnNurseAdd(wex::maker::make<wex::button>(plNurse)),

          bnPatientAdd(wex::maker::make<wex::button>(plPatient)),
          lsPatient(wex::maker::make<wex::list>(plPatient))
    {
        tabs.move(0, 0, 1000, 500);
        tabs.tabWidth(200);
        tabs.add("Nurses", plNurse);
        tabs.add("Patients", plPatient);

        ConstructNursesPanel();
        ConstructPatientsPanel();

        show();
        tabs.select(0);
        run();
    }

private:
    wex::tabbed &tabs;
    wex::panel &plNurse;
    wex::panel &plPatient;
    wex::label &lbNursePanel;
    wex::label &lbNurseOrder;
    wex::choice &chNurseOrder;
    wex::list &lsNurse;
    wex::list &lsPatient;
    wex::button &bnNurseAdd;

    wex::button &bnPatientAdd;

    cDB theDB;

    void ConstructNursesPanel();
    void ConstructPatientsPanel();

    void addNurse();
    void listNurse();
    void editNurse();

    void addPatient();
    void listPatient();
};

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
}

cDB::vperson_t cDB::patient()
{
    myPatientList.clear();
    for (auto &v : myValue)
    {
        if (v.aid == eAttribute::role && v.value == "patient")
        {
            myPatientList.push_back(
                patient(v.pid));
        }
    }
    return myPatientList;
}
cDB::person_t cDB::nurse(int pid)
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
        if (v.aid == eAttribute::inService && v.pid == pid)
            ret.second[2] = v.value;
    }
    return ret;
}
cDB::person_t cDB::patient(int pid)
{
    person_t ret;
    ret.first = pid;
    ret.second.resize(1);
    for (auto &v : myValue)
    {
        if (v.aid == eAttribute::name && v.pid == pid)
            ret.second[0] = v.value;
    }
    return ret;
}

void cGUI::ConstructNursesPanel()
{
    lbNursePanel.move(20, 40, 100, 30);
    lbNursePanel.text("Nurses");

    bnNurseAdd.move(200, 40, 100, 30);
    bnNurseAdd.text("ADD");
    bnNurseAdd.events().click(
        [&]
        {
            addNurse();
            listNurse();
        });

    lbNurseOrder.move(20, 70, 40, 30);
    lbNurseOrder.text("Order");
    chNurseOrder.move(80, 70, 100, 100);
    chNurseOrder.add("Licence");
    chNurseOrder.add("InService");
    chNurseOrder.select(0);
    chNurseOrder.events().select(
        chNurseOrder.id(),
        [&]
        {
            listNurse();
        });

    lsNurse.move(10, 120, 900, 400);
    lsNurse.events().select(
        lsNurse.id(),
        [&]
        {
            editNurse();
            listNurse();
        });
}

void cGUI::ConstructPatientsPanel()
{
    bnPatientAdd.move(200, 40, 100, 30);
    bnPatientAdd.text("ADD");
    bnPatientAdd.events().click(
        [&]
        {
            addPatient();
            listPatient();
        });
}

void cGUI::addNurse()
{
    wex::inputbox ib;
    ib.text("Nurse");
    ib.add("Name", "");
    ib.add("Licence", "");
    ib.add("Service", "");

    ib.showModal();

    theDB.addNurse(
        ib.value("Name"),
        ib.value("Licence"),
        ib.value("Service"));
}
void cGUI::editNurse()
{
    auto nurse = theDB.nurse(
        lsNurse.selectedIndex());

    wex::inputbox ib;
    ib.text("Nurse");
    ib.add("Name", nurse.second[0]);
    ib.add("Licence", nurse.second[1]);
    ib.add("Service", nurse.second[2]);

    ib.showModal();

    theDB.updateNurse(
        nurse.first,
        ib.value("Name"),
        ib.value("Licence"),
        ib.value("Service"));
}
void cGUI::addPatient()
{
    wex::inputbox ib;
    ib.text("Patient");
    ib.add("Name", "");

    ib.showModal();

    theDB.addPatient(
        ib.value("Name"));
}
void cGUI::listNurse()
{
    // clear the old list
    lsNurse.clear();

    // calculate date attribute index
    int dateIndex;
    switch (chNurseOrder.selectedIndex())
    {
    case 0:
        dateIndex = 1;
        break;
    case 1:
        dateIndex = 2;
        break;
    default:
        return;
    }
    
    // populate list
    for (auto &n : theDB.nursebyDate(dateIndex))
    {
        std::stringstream ss;
        for (auto &s : n.second)
        {
            ss << std::setw(16) << s;
        }
        lsNurse.add(ss.str());
    }
}
void cGUI::listPatient()
{
    lsPatient.clear();
    for (auto &n : theDB.patient())
    {
        std::stringstream ss;
        for (auto &s : n.second)
        {
            ss << std::setw(12) << s;
        }
        lsPatient.add(ss.str());
    }
}

main()
{
    cGUI theGUI;
    return 0;
}
