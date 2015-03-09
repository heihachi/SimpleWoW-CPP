#include <cstdio>
#include <iostream>
#include <fstream>
using namespace std;

#include <Config.h>

int main() {
    Section *conf = parseConfig("test.txt");

    cout << "----------------------"<<endl;
    if (conf == NULL) {
        cerr << "configuration is empty" << endl;
        return 1;
    }
    std::ofstream Out ("result.cfg",std::ios::out | std::ios::binary);
    printSection(Out, conf); //print configuration into result.cfg file

    long a = 0;
    bool xxx = false;
    std::string str = "";
    std::string rules_name;
    cout << "--- Initialize variables ---" << endl;

    //This is the safe way to initialize some of our variables
    SetupVars set_vars;
    set_vars.Add("str1", 			//config variable
            new SetupBaseType_string(str),  //init functor
            true,                           //if undefined config var is it fatality or no
            "test");			//default value if there is no the variable in config file
    set_vars.Add("a", new SetupBaseType_long(a), false, "-1");
    set_vars.Add("xxx", new SetupBaseType_yesno(xxx), false, "yes");
    set_vars.Add("name", new SetupBaseType_string(rules_name), false, "ZZZZ");
    try {
        set_vars.Set(conf);
        //conf is a section pointer from which we should init our variables.
    } catch (errGetVal &er_val) {
        cerr << "variable '" << er_val.key
                << "' wasn't define in configuration" << endl;
    }

    cout << "--- The result of initialization ---" << endl;
    cout << "int a = " << a << endl
            << "bool xxx = " << xxx << endl
            << "string str = " << str << endl
            << "string rules_name = " << rules_name << endl;

    return 0;
}
