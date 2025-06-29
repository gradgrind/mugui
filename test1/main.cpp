#include "backend.h"
#include "minion.h"
#include <cstdio>
#include <cstdlib>
#include <map>

using namespace std;

string callback_data;

map<string, bool> tabs;

const char* callback0(
    const char* data)
{
    auto m = minion::Reader::read(data);
    printf("callback got '%s'\n", data);
    if (auto mm = m.m_list()) {
        string wname;
        string wtab;
        if ((*mm)->get_string(0, wname)) {
            if (wname == "()buttons") {
                callback_data = string{"[[WIDGET_PREFIX, buttons]"};
                wtab = "(buttons)l_MainWindow";
                if (!tabs[wtab]) {
                    callback_data.append( //
                        ",[MINION_FILE, ../../examples/buttons.minion]");
                    callback_data.append(",[WIDGET, ()viewer, [ADD, ");
                    callback_data.append(wtab);
                    callback_data.append("]]");
                    tabs[wtab] = true;
                }
                callback_data.append( //
                    ",[WIDGET, ()viewer, [SELECT, ");
                callback_data.append(wtab);
                callback_data.append("]]]");

            } else if (wname == "()grid") {
                callback_data = string{"[[WIDGET_PREFIX, grid]"};
                wtab = "(grid)l_MainWindow";
                if (!tabs[wtab]) {
                    callback_data.append( //
                        ",[MINION_FILE, ../../examples/grid.minion]");
                    callback_data.append(",[WIDGET, ()viewer, [ADD, ");
                    callback_data.append(wtab);
                    callback_data.append("]]");
                    tabs[wtab] = true;
                }
                callback_data.append( //
                    ",[WIDGET, ()viewer, [SELECT, ");
                callback_data.append(wtab);
                callback_data.append("]]]");

            } else if (wname == "()studentgroups") {
                callback_data = string{"[[WIDGET_PREFIX, studentgroups]"};
                wtab = "(studentgroups)l_MainWindow";
                if (!tabs[wtab]) {
                    callback_data.append( //
                        ",[MINION_FILE, ../../examples/studentgroups.minion]");
                    callback_data.append(",[WIDGET, ()viewer, [ADD, ");
                    callback_data.append(wtab);
                    callback_data.append("]]");
                    tabs[wtab] = true;
                }
                callback_data.append( //
                    ",[WIDGET, ()viewer, [SELECT, ");
                callback_data.append(wtab);
                callback_data.append("]]]");

            } else if (wname == "()complex") {
                callback_data = string{"[[WIDGET_PREFIX, complex]"};
                wtab = "(complex)l_MainWindow";
                if (!tabs[wtab]) {
                    callback_data.append( //
                        ",[MINION_FILE, ../../examples/complex.minion]");
                    callback_data.append(",[WIDGET, ()viewer, [ADD, ");
                    callback_data.append(wtab);
                    callback_data.append("]]");
                    tabs[wtab] = true;
                }
                callback_data.append( //
                    ",[WIDGET, ()viewer, [SELECT, ");
                callback_data.append(wtab);
                callback_data.append("]]]");

            } else if (wname.starts_with("(buttons)")) {
                callback_data = "[";
                if (wname.ends_with("PB1")) {
                    callback_data.append("[WIDGET, PB1, [SIZE, 300, 80]],");
                }
                callback_data
                    .append("[WIDGET, Output_1, [VALUE, ") //
                    .append(wname)
                    .append("]]]");

            } else if (wname.starts_with("(grid)")) {
                callback_data = string{"[[WIDGET, "} //
                                    .append(wname)
                                    .append(", [TEXT, \"")
                                    .append(wname)
                                    .append(" pushed\"]]]");

            } else if (wname.starts_with("(complex)")) {
                callback_data = string{"[[WIDGET, TableTotals, [VALUE, "} //
                                    .append(minion::Writer::dumpString(data))
                                    .append("]]");
                if (wname == "(complex)EF1" or wname == "(complex)EF4") {
                    callback_data.append(", [WIDGET, popup, [SHOW, ");
                    callback_data.append(wname);
                    callback_data.append("]]");
                }
                callback_data.append("]");

            } else if (wname.starts_with("(studentgroups)")) {
                callback_data = string{"[]"};

            } else {
                throw "Unknown Callback" + minion::Writer::dumpString(data);
            }
            printf("??? %s\n", callback_data.c_str());
            fflush(stdout);
            return callback_data.c_str();
        }
    }
    throw "Invalid callback";
}

const char* callback1(
    const char* data)
{
    auto m = minion::Reader::read(data);
    printf("callback got '%s'\n", data);
    if (auto mm = m.m_list()) {
        string wname;
        if ((*mm)->get_string(0, wname)) {
            callback_data = "[";
            if (wname.ends_with("PB1")) {
                callback_data.append("[WIDGET, PB1, [SIZE, 300, 80]],");
            }
            callback_data
                .append("[WIDGET, Output_1, [VALUE, ") //
                .append(wname)
                .append("]]]");
            printf("??? %s\n", callback_data.c_str());
            fflush(stdout);
            return callback_data.c_str();
        }
    }
    throw "Invalid callback";
}

const char* callback2(
    const char* data)
{
    auto m = minion::Reader::read(data);
    printf("callback got '%s'\n", data);
    if (auto mm = m.m_list()) {
        string wname;
        if ((*mm)->get_string(0, wname)) {
            callback_data = string{"[[WIDGET, "} //
                                .append(wname)
                                .append(", [TEXT, \"")
                                .append(wname)
                                .append(" pushed\"]]]");
            printf("??? %s\n", callback_data.c_str());
            fflush(stdout);
            return callback_data.c_str();
        }
    }
    throw "Invalid callback";
}

const char* callback2a(
    const char* data)
{
    auto m = minion::Reader::read(data);
    printf("callback got '%s'\n", data);
    if (auto mm = m.m_list()) {
        string wname;
        if ((*mm)->get_string(0, wname)) {
            callback_data = string{"[[WIDGET, "} //
                                .append(wname)
                                .append(", [TEXT, \"")
                                .append(wname)
                                .append("\"]]]");
            printf("??? %s\n", callback_data.c_str());
            fflush(stdout);
            return callback_data.c_str();
        }
    }
    throw "Invalid callback";
}

const char* callback3(
    const char* data)
{
    auto m = minion::Reader::read(data);
    printf("callback got '%s'\n", data);
    if (auto mm = m.m_list()) {
        string wname;
        if ((*mm)->get_string(0, wname)) {
            callback_data = string{"[[WIDGET, TableTotals, [VALUE, "} //
                                .append(minion::Writer::dumpString(data))
                                .append("]]");
            if (wname == "()EF1" or wname == "()EF4") {
                callback_data.append(", [WIDGET, popup, [SHOW, ");
                callback_data.append(wname);
                callback_data.append("]]");
            }
            callback_data.append("]");
            printf("??? %s\n", callback_data.c_str());
            fflush(stdout);
            return callback_data.c_str();
        }
    }
    throw "Invalid callback";
}

int main()
{
    auto fplist = {
        // These paths are relative to the directory
        // in which the binary is built.
        "../../examples/demo.minion",
        "../../examples/buttons1.minion",
        "../../examples/grid1.minion",
        "../../examples/grid2.minion",
        "../../examples/complex1.minion"
        //
    };

    auto flist = {
        //
        callback0,
        callback1,
        callback2,
        callback2a,
        callback3
        //
    };

    string guidata;

    for (int count = 0; count < 1; ++count) {
        int i = 0;
        for (const auto& fp : fplist) {
            tabs = {};

            SetCallbackFunction(flist.begin()[i]);
            ++i;

            string fpm = string{"[[MINION_FILE,"}.append(fp).append(
                "],[WIDGET,MainWindow,[SHOW]],[WIDGET, l_MainWindow, [print_info]],[RUN]]");
            Init(fpm.c_str());
        }
    }

    return 0;
}
