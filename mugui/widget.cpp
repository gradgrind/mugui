#include "callback.h"
#include "layout.h"
#include "support_functions.h"
#include "widget.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <functional>
#include <iostream>
#include <stdexcept>
using namespace std;
using namespace minion;

// static member
unordered_map<std::string_view, Widget::flagged_widget> Widget::widget_map;

// static member
void Widget::init_foreground(
    Fl_Color colour)
{
    normal_fg = colour;
    Fl::foreground( //
        (colour >> 24) & 0xFF,
        (colour >> 16) & 0xFF,
        (colour >> 8) & 0xFF);
}

// static member
void Widget::init_background(
    Fl_Color colour)
{
    normal_bg = colour;
    Fl::background( //
        (colour >> 24) & 0xFF,
        (colour >> 16) & 0xFF,
        (colour >> 8) & 0xFF);
}

// static member
void Widget::init_background2(
    Fl_Color colour)
{
    entry_bg = colour;
    Fl::background2( //
        (colour >> 24) & 0xFF,
        (colour >> 16) & 0xFF,
        (colour >> 8) & 0xFF);
}

// static member
void Widget::init_selection_background(
    Fl_Color colour)
{
    selection_bg = colour;
}

// static member
void Widget::init_pending_background(
    Fl_Color colour)
{
    pending_bg = colour;
}

// static member
// Note that this can only affect buttons defined after calling
void Widget::init_button_on_contrast(
    int c) // 0 .. 100
{
    if (c < 0)
        c = 0;
    else if (c > 100)
        c = 100;
    button_on_contrast = float(c) / 100;
}

// static member
// Note that this can only affect tables defined after calling
void Widget::init_table_header_contrast(
    int c) // 0 .. 100
{
    if (c < 0)
        c = 0;
    else if (c > 100)
        c = 100;
    table_header_contrast = float(c) / 100;
}

// static member
void Widget::init_settings()
{
    line_height = fl_height() * LINE_HEIGHT_FACTOR;
    init_foreground(NORMAL_FG);
    init_background(NORMAL_BG);
    init_background2(ENTRY_BG);
    init_pending_background(PENDING_BG);
    init_selection_background(SELECTION_BG);
    init_button_on_contrast(BUTTON_ON_CONTRAST);
    init_table_header_contrast(TABLE_HEADER_CONTRAST);
}

// static
// Free all widget memory, reporting any parentless widgets that were not
// declared as "FLOATING:1".
string Widget::clear()
{
    vector<string_view> to_report;
    vector<Fl_Widget*> to_delete;
    for (const auto& [wname, fw] : widget_map) {
        auto w = fw.widget;
        if (!w->fl_widget->parent()) {
            if (!fw.floating)
                to_report.emplace_back(wname);
            to_delete.emplace_back(w->fl_widget);
        }
    }
    string wlist{"Widgets have no parents:"};
    for (const auto& w : to_report) {
        wlist.append("\n  - ").append(w);
    }
    widget_map.clear();
    for (const auto& w : to_delete) {
        delete w;
    }
    if (to_report.empty())
        return "";
    return wlist;
}

// static
void Widget::new_widget(
    MList* m)
{
    string wtype;
    string name;
    {
        string name_;
        // Check new widget name
        if ( //
            m->size() < 4 || !m->get_string(1, wtype) || !m->get_string(2, name_)
            //
        ) {
            MValue m0{*m};
            throw string{"Bad NEW command: "} + dump_value(m0);
        }
        // Check name unique
        if (name_.empty()) {
            throw "A new widget must have a name ...";
        }
        name = "(" + w_prefix + ")";
        name.append(name_);
    }
    if (widget_map.contains(name)) {
        throw string{"Widget name already exists: "}.append(name);
    }

    new_function f;
    try {
        f = new_function_map.at(string{wtype});
    } catch (std::out_of_range& e) {
        throw string{"Unknown widget type: "}.append(wtype);
    }
    auto props = m->get(3).m_map();
    if (!props) {
        MValue m0{*m};
        throw string{"NEW command has no properties: "} + dump_value(m0);
    }

    // Create widget
    Widget* w = f(props->get());
    w->w_name = std::move(name);
    int fwidget = 0;
    (*props)->get_int("FLOATING", fwidget);
    widget_map.emplace(w->w_name, flagged_widget{w, fwidget != 0});
    w->fltk_widget()->user_data(w, true); // auto-free = true
    w->properties = *props;
    // Handle method calls supplied with the widget creation
    w->handle_methods(m, 4);
}

// static
Widget* Widget::get_widget(
    string_view name)
{
    string n;
    if (!name.starts_with('(')) {
        n = "(" + w_prefix + ")";
        n.append(name);
        name = n;
    }
    try {
        return widget_map.at(name).widget;
    } catch (const out_of_range& e) {
        throw string{"Unknown widget: "}.append(name);
    }
}

// The destructor will be called when the associated fltk_widget is destroyed,
// thanks to that widget's user_data setting.
Widget::~Widget() {
    // Delete any associated "user_data"
    if (auto_delete_user_data && user_data) {
        delete static_cast<Fl_Callback_User_Data*>(user_data);
    }
    // Remove from widget map
    widget_map.erase(w_name);
}

void Widget::print_dimensions(
    string indent)
{
    printf("%sWIDGET INFO: %s\n", indent.c_str(), w_name.c_str());
    printf("%sContent: %d x %d\n", indent.c_str(), content_width, content_height);
    printf("%sSet size: %d x %d // %d\n", indent.c_str(), minimum_width, minimum_height, margin);
    printf("%sActual size: %d x %d\n", indent.c_str(), fl_widget->w(), fl_widget->h());
    if (auto g = dynamic_cast<Fl_Grid*>(fl_widget))
        for (int i = 0; i < g->children(); ++i) {
            auto fcw = g->child(i);
            auto cw = static_cast<Widget*>(fcw->user_data());
            if (auto c = g->cell(fcw)) {
                printf("%s** Grid: %d @%d // %d @%d\n",
                       indent.c_str(),
                       c->row(),
                       c->rowspan(),
                       c->col(),
                       c->colspan());
            } else {
                printf("%s** Grid: unplaced\n", indent.c_str());
            }
            cw->print_dimensions(indent + "  ");
        }
    printf("%s----------------------------------\n", indent.c_str());
    fflush(stdout);
}

void Widget::handle_methods(
    MList* dolist, size_t start)
{
    auto len = dolist->size();
    for (size_t i = start; i < len; ++i) {
        auto n = dolist->get(i);
        auto mlist = n.m_list();
        if (mlist) {
            string c;
            if ((*mlist)->get_string(0, c)) {
                handle_method(c, mlist->get());
                continue;
            }
        }
        throw string{"Invalid DO method on widget "} + w_name + ": " + dump_value(n);
    }
}

void Widget::set_box(
    MMap* props)
{
    string btype;
    if (props->get_string("BOXTYPE", btype)) {
        auto bxt = get_boxtype(btype);
        fl_widget->box(bxt);
    }
}

void Widget::handle_method(std::string_view method, minion::MList* paramlist)
{
    if (method == "SIZE") {
        paramlist->get_int(1, minimum_width);
        paramlist->get_int(2, minimum_height);
        W_Group::child_resized(this);

    } else if (method == "HEIGHT") {
        paramlist->get_int(1, minimum_height);
        W_Group::child_resized(this);

    } else if (method == "WIDTH") {
        paramlist->get_int(1, minimum_width);
        W_Group::child_resized(this);

    } else if (method == "COLOUR") {
        string clr;
        if (paramlist->get_string(1, clr)) {
            auto c = get_colour(clr);
            fl_widget->color(c);
            fl_widget->labelcolor(fl_contrast(c, c));
        }
    } else if (method == "TEXT") {
        string lbl;
        if (paramlist->get_string(1, lbl)) {
            fl_widget->copy_label(lbl.c_str());
            content_width = 0;
            fl_widget->measure_label(content_width, content_height);
            W_Group::child_resized(this);
        } else
            throw "TEXT value missing for widget " + w_name;
    } else if (method == "SHOW") {
        fl_widget->show();
    } else if (method == "clear_visible_focus") {
        fl_widget->clear_visible_focus();
    } else if (method == "print_info") {
        print_dimensions();
    } else {
        throw string{"Unknown method on widget " + w_name + ": "}.append(method);
    }
}
