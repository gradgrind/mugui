#include "backend.h"
#include "callback.h"
#include "dialogs.h"
#include "functions.h"
#include "layout.h"
#include "widget.h"
#include "widgets.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <map>
#include <string_view>
using namespace std;
using namespace minion;

const char* (*backend)(const char*);

void SetCallbackFunction(
    const char* (*backend_pointer)(const char*) )
{
    backend = backend_pointer;
}

const map<string, new_function> new_function_map{
    //
    {"Window", W_Window::make},
    {"Grid", W_Grid::make},
    {"Hlayout", W_Layout::make_hlayout},
    {"Vlayout", W_Layout::make_vlayout},
    {"Stack", W_Stack::make},
    {"PushButton", W_PushButton::make},
    {"RadioButton", W_RadioButton::make},
    {"Box", W_Box::make},
    {"Hline", W_Hline::make},
    {"Vline", W_Vline::make},
    {"Label", W_Label::make},
    {"Choice", W_Choice::make},
    {"Output", W_Output::make},
    {"PopupEditor", W_PopupEditor::make},
    {"Checkbox", W_Checkbox::make},
    {"List", W_List::make},
    {"TextLine", W_TextLine::make},
    {"RowTable", W_RowTable::make},
    {"EditForm", W_EditForm::make},
    {"Dialog", W_Dialog::make}
    //
};

string dump_value(
    MValue m)
{
    minion::Writer writer(m, 0);
    return string{writer.dump()};
}

void value_error(
    string msg, MValue m)
{
    throw msg + dump_value(m);
}

void Callback(
    MValue m)
{
    //const char* cbdata;
    const char* cbresult;
    {
        minion::Writer writer(m, -1);
        //printf("callback got '%s'\n", writer.dump_c());
        auto cbdata = writer.dump_c();
        cbresult = backend(cbdata);
    }

    MValue input_value = minion::Reader::read(cbresult);
    //cout << "CALLBACK RETURNED: " << dump_value(input_value) << endl;
    if (auto dolist0 = input_value.m_list())
        do_commands(dolist0->get());
    else if (const char* e = input_value.error_message())
        throw string{e};
    else
        throw string{"Invalid callback result: "}.append(cbresult);
}

void Callback0(
    string& widget)
{
    MList m({widget});
    Callback(m);
}

void Callback1(
    string& widget, MValue data)
{
    MList m({widget, data});
    Callback(m);
}

void Callback2(string& widget, MValue data, MValue data2)
{
    MList m({widget, data, data2});
    Callback(m);
}

//TODO
void Init(
    const char* data0)
{
    fl_font(FL_HELVETICA, FONT_SIZE);
    Widget::init_settings();
    //std::cout << "C says: init '" << data0 << "'" << std::endl;

    //TODO? Where? If?
    //Fl::option(Fl::OPTION_VISIBLE_FOCUS, false);
    Fl::box_border_radius_max(6);

    Widget::set_prefix(""); // start without widget namespace

    auto guidata = minion::Reader::read(data0);
    if (auto e = guidata.error_message()) {
        fl_alert("%s", e);
        return;
    }
    try {
        auto dolist0 = guidata.m_list();
        if (dolist0)
            do_commands(dolist0->get());
        else
            value_error("Input data not a GUI command list: ", guidata);
        auto e = Widget::clear();
        if (!e.empty()) {
            fl_alert("%s", e.c_str());
        }
    } catch (string& e) {
        fl_alert("%s", e.c_str());
        Widget::clear();
    }
}
