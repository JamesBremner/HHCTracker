#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include <inputbox.h>
#include "cStarterGUI.h"

class cDB
{
    enum class eAttribute
    {
        none,
        role,
        name,
        nurselicence,
        inService,
    };
    struct sValue
    {
        eAttribute aid;
        int pid;
        std::string value;
    };

public:
    void addNurse(
        const std::string &name,
        const std::string &licence,
        const std::string &service);

    std::vector<std::string> nurse();

    

private:
    static int lastPID;
    std::vector<sValue> myValue;
};

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
          bnNurseAdd(wex::maker::make<wex::button>(plNurse))
    {
        tabs.move(0, 0, 1000, 500);
        tabs.tabWidth(200);
        tabs.add("Nurses", plNurse);
        tabs.add("Patients", plPatient);

        plNurse.move(10, 10, 900, 400);

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
            });

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
    wex::button &bnNurseAdd;

    cDB theDB;

    void addNurse();
    void listNurse();
    void editNurse();
};

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

std::vector<std::string> cDB::nurse()
{
    std::vector<std::string> ret;
    for (auto &v : myValue)
    {
        if (v.aid == eAttribute::role && v.value == "nurse")
        {
            int pid = v.pid;
            std::string desc;
            for (auto &v2 : myValue)
            {
                if (v2.aid == eAttribute::name && v2.pid == pid)
                    desc += v2.value + "  ";
                if (v2.aid == eAttribute::nurselicence && v2.pid == pid)
                    desc += v2.value + "  ";
                if (v2.aid == eAttribute::inService && v2.pid == pid)
                    desc += v2.value + "  ";
            }
            ret.push_back(desc);
        }
    }
    return ret;
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

void cGUI::listNurse()
{
    lsNurse.clear();
    for (auto &n : theDB.nurse())
    {
        lsNurse.add(n);
    }
}

void cGUI::editNurse()
{
    lsNurse.selectedIndex();
}

main()
{
    cGUI theGUI;
    return 0;
}
