#include "dialogs.h"
#include <FL/Fl_Double_Window.H>
using namespace std;
using namespace minion;

W_Dialog* W_Dialog::make(
    MMap* props)
{
    auto widget = new W_Dialog();
    make_window(300, 200, widget, props);
    static_cast<Fl_Window*>(widget->fl_widget)->set_modal();
    return widget;
}

void W_Dialog::handle_method(
    string_view method, MList* paramlist)
{
    if (method == "SHOW") {
        string p;
        if (paramlist->get_string(1, p)) {
            //TODO checks ...
            auto w = get_fltk_widget(p);

            fl_widget->position(w->x(), w->y());
        }
        container->layout();
        fl_widget->show();
    } else {
        W_Window::handle_method(method, paramlist);
    }
}
