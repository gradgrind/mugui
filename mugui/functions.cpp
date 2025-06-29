#include "functions.h"
#include "callback.h"
#include "iofile.h"
#include "support_functions.h"
#include "widget.h"
#include <FL/Fl.H>
#include <FL/fl_ask.H>
using namespace std;
using namespace minion;

void GUI(
    MList* cmd)
{
    string s;
    if (!cmd->get_string(0, s)) {
        value_error("Invalid GUI command: ", *cmd);
    }
    if (s == "WIDGET") {
        // Handle widget methods
        string w;
        if (cmd->get_string(1, w)) {
            Widget::get_widget(w)->handle_methods(cmd, 2);
            return;
        }
        value_error("Invalid WIDGET command: ", *cmd);
    }
    if (s == "NEW") {
        // Make a new widget
        Widget::new_widget(cmd);
        return;
    }
    auto f = function_map.at(s);
    f(cmd);
}

void do_commands(
    MList* dolist)
{
    auto len = dolist->size();
    for (size_t i = 0; i < len; ++i) {
        if (auto command = dolist->get(i).m_list())
            GUI(command->get());
        else {
            value_error("Invalid GUI command: ", **command);
        }
    }
}

void f_MINION_FILE(
    MList* m)
{
    string f;
    if (m->get_string(1, f)) {
        string data0 = readfile(f);
        if (data0.empty()) {
            throw "Error opening file:\n " + f;
        }
        auto guidata = Reader::read(data0);
        if (auto e = guidata.error_message()) {
            throw string(e);
        }
        auto dolist0 = guidata.m_list();
        if (dolist0) {
            do_commands(dolist0->get());
            return;
        } else
            value_error("Input data not a GUI command list:\n ", guidata);
    }
    value_error("Invalid command:\n ", *m);
}

void f_RUN(
    MList* m)
{
    (void) m;
    //auto cc = Fl::run();
    //std::cout << "Main Loop ended: " << cc << std::endl;
    Fl::run();
}

void setup_method(
    string_view method, MList* paramlist)
{
    if (method == "BACKGROUND") {
        std::string colour;
        if (paramlist->get_string(1, colour)) {
            Widget::init_background(get_colour(colour));
            return;
        }
        throw string{"Invalid SETUP method, colour expected: "} + dump_value(*paramlist);
    }
    if (method == "BACKGROUND2") {
        std::string colour;
        if (paramlist->get_string(1, colour)) {
            Widget::init_background2(get_colour(colour));
            return;
        }
        throw string{"Invalid SETUP method, colour expected: "} + dump_value(*paramlist);
    }
    if (method == "FOREGROUND") {
        std::string colour;
        if (paramlist->get_string(1, colour)) {
            Widget::init_foreground(get_colour(colour));
            return;
        }
        throw string{"Invalid SETUP method, colour expected: "} + dump_value(*paramlist);
    }
    if (method == "SELECTION_BACKGROUND") {
        std::string colour;
        if (paramlist->get_string(1, colour)) {
            Widget::init_selection_background(get_colour(colour));
            return;
        }
        throw string{"Invalid SETUP method, colour expected: "} + dump_value(*paramlist);
    }
    if (method == "PENDING_BACKGROUND") {
        std::string colour;
        if (paramlist->get_string(1, colour)) {
            Widget::init_pending_background(get_colour(colour));
            return;
        }
        throw string{"Invalid SETUP method, colour expected: "} + dump_value(*paramlist);
    }
    if (method == "BUTTON_ON_CONTRAST") {
        int c;
        if (paramlist->get_int(1, c)) {
            Widget::init_button_on_contrast(c);
            return;
        }
        throw string{"Invalid SETUP method, contrast (0 .. 100) expected: "}
            + dump_value(*paramlist);
    }
    if (method == "TABLE_HEADER_CONTRAST") {
        int c;
        if (paramlist->get_int(1, c)) {
            Widget::init_table_header_contrast(c);
            return;
        }
        throw string{"Invalid SETUP method, contrast (0 .. 100) expected: "}
            + dump_value(*paramlist);
    }
    throw string{"Invalid SETUP method: "} + dump_value(*paramlist);
    //TODO?
    //Fl::box_border_radius_max 	( 	int 	R	);
    //Fl::menu_linespacing 	( 	int 	H	);
    //LINE_HEIGHT?
    //PENDING_COLOUR?
}

void f_SETUP(
    MList* m)
{
    auto len = m->size();
    for (size_t i = 1; i < len; ++i) {
        auto n = m->get(i);
        auto mlist = n.m_list();
        if (mlist) {
            string c;
            if ((*mlist)->get_string(0, c)) {
                setup_method(c, mlist->get());
                continue;
            }
        }
        throw string{"SETUP function, expected method (list): "} + dump_value(n);
    }
}

void f_WIDGET_PREFIX(
    MList* m)
{
    //(void) m;
    string p;
    if (m->get_string(1, p)) {
        Widget::set_prefix(p);
        return;
    }
    throw string{"INVALID WIDGET_PREFIX function: "} + dump_value(*m);
}

std::unordered_map<std::string, function_handler> function_map{
    //
    {"MINION_FILE", f_MINION_FILE},
    {"RUN", f_RUN},
    {"SETUP", f_SETUP},
    {"WIDGET_PREFIX", f_WIDGET_PREFIX}
    //
};
