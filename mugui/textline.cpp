#include "widgets.h"
#include "callback.h"
#include <FL/Fl_Input.H>
using namespace std;

class TextLine : public Fl_Input
{
    int handle(int event) override
    { 
        auto result = Fl_Input::handle(event);
        if (strcmp(value(), text.c_str()) == 0) {
            if (modified) {
                modified = false;
                // Reset colour
                color(Widget::entry_bg);
                redraw();
            }
        } else {
            if (!modified) {
                modified = true;
                // Set modified/pending colour
                color(Widget::pending_bg);
                redraw();
            }
        }
        return result;
    }

public:
    TextLine()
        : Fl_Input(0, 0, 0, 0)
    {}

    std::string text;
    bool modified;
};

//static
W_TextLine* W_TextLine::make(minion::MMap* parammap)
{
    (void) parammap;
    auto w = new TextLine();
    auto widget = new W_TextLine();
    widget->content_height = Widget::line_height;
    widget->fl_widget = w;
    w->selection_color(Widget::selection_bg);

    w->when(FL_WHEN_RELEASE|FL_WHEN_ENTER_KEY);
    w->callback(
        [](Fl_Widget* w, void* ud) {
            auto widget = static_cast<W_TextLine*>(ud);
            auto ww = static_cast<TextLine*>(w);
            // Don't leave all text selected when return is pressed
            ww->insert_position( ww->insert_position());
            string v = ww->value();
            if (widget->set(v)) {
                Callback1(*widget->widget_name(), v);

                // This would remove keyboard focus from the widget
                //Fl::focus(w->parent());
            }
        });
    return widget;         
}

//TODO
//void TextLine::handle_method(std::string_view method, minion::MList* paramlist){}

bool W_TextLine::set(std::string_view newtext)
{
    auto w = static_cast<TextLine*>(fl_widget);
    //modified = false;
    string v = w->value();
    bool res = false;
    if (newtext != w->text) {
        w->text = newtext;
        res = true;
    }
    if (v != w->text) w->value(w->text.c_str());
    return res;
}
