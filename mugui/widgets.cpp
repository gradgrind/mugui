#include "callback.h"
#include "layout.h"
#include "support_functions.h"
#include "widgets.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Grid.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Window.H>
using namespace std;
using namespace minion;

// *** Non-layout widgets ***

void W_Labelled_Widget::handle_method(
    string_view method, MList* paramlist)
{
    string label;
    if (method == "TEXT") {
        string lbl;
        if (paramlist->get_string(1, lbl)) {
            fl_widget->copy_label(lbl.c_str());
            label_width = 0;
            fl_widget->measure_label(label_width, label_height);
            W_Group::child_resized(this);
        } else
            throw "TEXT value missing for widget " + *widget_name();
        W_Group::child_resized(this);
    } else {
        Widget::handle_method(method, paramlist);
    }
}

//static
W_Box* W_Box::make(
    MMap* props)
{
    (void) props;
    auto w = new Fl_Box(0, 0, 0, 0);
    auto widget = new W_Box();
    widget->fl_widget = w;
    widget->set_box(props);
    return widget;
}

//static
W_Hline* W_Hline::make(
    MMap* props)
{
    (void) props;
    auto w = new Fl_Box(FL_BORDER_BOX, 0, 0, 0, 0, "");
    auto widget = new W_Hline();
    widget->fl_widget = w;
    return widget;
}

//static
W_Vline* W_Vline::make(
    MMap* props)
{
    (void) props;
    auto w = new Fl_Box(FL_BORDER_BOX, 0, 0, 0, 0, "");
    auto widget = new W_Vline();
    widget->fl_widget = w;
    return widget;
}

//static
W_Label* W_Label::make(
    MMap* props)
{
    auto w = new Fl_Box(0, 0, 0, 0);
    auto widget = new W_Label();
    widget->minimum_height = Widget::line_height;
    widget->h_content_padding = H_LABEL_PADDING;
    widget->v_content_padding = V_LABEL_PADDING;
    widget->fl_widget = w;
    widget->set_box(props);

    string align;
    if (props->get_string("LABEL_ALIGN", align)) {
        if (align == "LEFT")
            w->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
        else if (align == "RIGHT")
            w->align(FL_ALIGN_INSIDE | FL_ALIGN_RIGHT);
    }

    return widget;
}

//static
W_Choice* W_Choice::make(
    MMap* props)
{
    (void) props;
    auto w = new Fl_Choice(0, 0, 0, 0);
    w->callback([](Fl_Widget* w, void* ud) {
        (void) ud;
        string* dw{Widget::get_widget_name(w)};
        auto ww = static_cast<Fl_Choice*>(w);
        Callback2(*dw, to_string(ww->value()), ww->text());
    });
    auto widget = new W_Choice();
    widget->content_height = Widget::line_height;
    widget->fl_widget = w;
    return widget;
}

void W_Choice::handle_method(
    string_view method, MList* paramlist)
{
    string item;
    if (method == "ADD") {
        int n = paramlist->size();
        for (int i = 1; i < n; ++i) {
            paramlist->get_string(i, item);
            static_cast<Fl_Choice*>(fl_widget)->add(item.c_str());
        }
    } else {
        W_Labelled_Widget::handle_method(method, paramlist);
    }
}

//static
W_Output* W_Output::make(
    MMap* props)
{
    (void) props;
    auto w = new Fl_Output(0, 0, 0, 0);
    auto widget = new W_Output();
    widget->content_height = Widget::line_height;
    widget->fl_widget = w;
    w->selection_color(Widget::selection_bg);
    return widget;
}

void W_Output::handle_method(
    string_view method, MList* paramlist)
{
    string item;
    if (method == "VALUE") {
        string val;
        if (paramlist->get_string(1, val)) {
            static_cast<Fl_Output*>(fl_widget)->value(val.c_str());
        } else
            throw "Output widget VALUE method: value missing";
    } else {
        W_Labelled_Widget::handle_method(method, paramlist);
    }
}

// static
W_PopupEditor* W_PopupEditor::make(
    MMap* props)
{
    (void) props;
    auto w = new Fl_Output(0, 0, 0, 0);
    auto widget = new W_PopupEditor();
    widget->content_height = Widget::line_height;
    widget->fl_widget = w;
    w->color(Widget::entry_bg);
    w->callback([](Fl_Widget* w, void* ud) {
        (void) ud;
        string* dw{Widget::get_widget_name(w)};
        // or string dw{static_cast<Widget*>(ud)->widget_name()};
        Callback1(*dw, static_cast<Fl_Output*>(w)->value());
    });
    return widget;
}

void W_PushButton::set_colour(
    Fl_Color clr)
{
    Fl_Color cx = (fl_lightness(clr) < 50) ? 0xffffff00 : 0;
    auto cp = fl_color_average(cx, clr, contrast);
    fl_widget->color(clr, cp);
}

//static
W_PushButton* W_PushButton::make(
    MMap* props)
{
    auto w = new Fl_Button(0, 0, 0, 0);
    auto widget = new W_PushButton();
    widget->minimum_height = Widget::line_height;
    widget->h_content_padding = H_LABEL_PADDING;
    widget->v_content_padding = V_LABEL_PADDING;
    widget->fl_widget = w;
    widget->set_box(props);
    string btype;
    if (props->get_string("DOWNBOXTYPE", btype)) {
        auto bxt = get_boxtype(btype);
        w->down_box(bxt);
    }
    // "selection" (pressed) colour
    widget->contrast = Widget::button_on_contrast;
    widget->set_colour(Widget::entry_bg);
    w->callback([](Fl_Widget* w, void* ud) {
        (void) ud;
        string* dw{Widget::get_widget_name(w)};
        // or string dw{static_cast<Widget*>(ud)->widget_name()};
        //auto ww = static_cast<Fl_Button*>(w);
        Callback0(*dw);
    });
    return widget;
}

void W_PushButton::handle_method(
    string_view method, MList* paramlist)
{
    if (method == "COLOUR") {
        string clr;
        if (paramlist->get_string(1, clr)) {
            set_colour(get_colour(clr));
        }
    } else if (method == "ON_CONTRAST") {
        int c;
        if (paramlist->get_int(1, c)) {
            if (c < 0)
                c = 0;
            else if (c > 100)
                c = 100;
            contrast = float(c) / 100;
            set_colour(fl_widget->color());
        }
    } else {
        W_Label::handle_method(method, paramlist);
    }
}

//static
W_RadioButton* W_RadioButton::make(
    MMap* props)
{
    (void) props;
    auto w = new Fl_Radio_Button(0, 0, 0, 0);
    auto widget = new W_RadioButton();
    widget->minimum_height = Widget::line_height;
    widget->h_content_padding = H_LABEL_PADDING;
    widget->v_content_padding = V_LABEL_PADDING;
    widget->fl_widget = w;
    widget->set_box(props);
    string btype;
    if (props->get_string("DOWNBOXTYPE", btype)) {
        auto bxt = get_boxtype(btype);
        w->down_box(bxt);
    }
    // "selection" (pressed) colour
    widget->contrast = Widget::button_on_contrast;
    widget->set_colour(Widget::entry_bg);
    w->callback([](Fl_Widget* w, void* ud) {
        (void) ud;
        string* dw{Widget::get_widget_name(w)};
        // or string dw{static_cast<Widget*>(ud)->widget_name()};
        //auto ww = static_cast<Fl_Button*>(w);
        Callback0(*dw);
    });
    return widget;
}

//static
W_Checkbox* W_Checkbox::make(
    MMap* props)
{
    (void) props;
    auto w = new Fl_Round_Button(0, 0, 0, Widget::line_height);
    auto widget = new W_Checkbox();
    widget->fl_widget = w;
    widget->set_box(props);
    w->callback([](Fl_Widget* w, void* ud) {
        (void) ud;
        string* dw{Widget::get_widget_name(w)};
        // or string dw{static_cast<Widget*>(ud)->widget_name()};
        auto ww = static_cast<Fl_Round_Button*>(w);
        string val{};
        if (ww->value() != 0)
            val = "1";
        Callback1(*dw, val);
    });
    return widget;
}

//static
W_List* W_List::make(
    MMap* props)
{
    (void) props;
    auto w = new Fl_Hold_Browser(0, 0, 0, 0);
    auto widget = new W_List();
    widget->fl_widget = w;
    //w->color(Widget::entry_bg);
    w->selection_color(Widget::selection_bg);

    w->callback([](Fl_Widget* w, void* ud) {
        (void) ud;
        //printf("§CB: %b\n", Fl::callback_reason());
        //fflush(stdout);
        string* dw{Widget::get_widget_name(w)};
        // or string dw{static_cast<Widget*>(ud)->widget_name()};
        auto ww = static_cast<Fl_Hold_Browser*>(w);
        auto i = ww->value();
        // Callback only for actual items (1-based indexing)
        if (i > 0) {
            Callback2(*dw, to_string(i - 1), ww->text(i));
        }
    });
    return widget;
}

void W_List::handle_method(
    string_view method, MList* paramlist)
{
    string item;
    if (method == "SET") {
        auto e1 = static_cast<Fl_Hold_Browser*>(fl_widget);
        e1->clear();
        int n = paramlist->size();
        for (int i = 1; i < n; ++i) {
            //TODO? add can have a second argument (void *), which can
            // refer to data ...
            paramlist->get_string(i, item);
            e1->add(item.c_str());
        }
    } else {
        W_Labelled_Widget::handle_method(method, paramlist);
    }
}
