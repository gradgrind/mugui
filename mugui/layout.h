#ifndef LAYOUT_H
#define LAYOUT_H

#include "minion.h"
#include "widget.h"
#include "widgets.h"
#include <FL/Fl_Flex.H>
#include <FL/Fl_Grid.H>
#include <FL/Fl_Widget.H>
#include <string_view>

class W_Group : public Widget
{
protected:
    virtual void handle_child_resized() {};

public:
    static void child_resized(Widget* w)
    {
        if (auto fpw = w->fltk_widget()->parent()) {
            if (auto pw = static_cast<W_Group*>(fpw->user_data()))
                pw->handle_child_resized();
        }
    }

    /*
    static void set_widget_label(W_Labelled_Widget* w)
    {
        if (auto wp = w->fltk_widget()->parent()) {
            auto p = static_cast<W_Group*>(wp->user_data());
            p->handle_widget_label(w);
        }
    }

    static void set_child_size(
        Fl_Widget* wc, int ww, int wh)
    {
        if (auto wp = wc->parent()) {
            auto p = static_cast<W_Group*>(wp->user_data());
            p->handle_child_size(wc, ww, wh);
        } else {
            wc->size(ww, wh);
        }
    }
    
    static void child_size_modified(
        Widget* wc)
    {
        if (auto p = wc->fltk_widget()->parent()) {
            auto wp{static_cast<W_Group*>(p->user_data())};
            if (wp)
                wp->handle_child_modified(wc);
        }
    }
    */
};

class W_Window : public Widget
{
protected:
    Fl_Flex* container;
    static void make_window(int ww, int wh, W_Window* widget, minion::MMap* props);

public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_Window* make(minion::MMap* props);
};

class W_Grid : public W_Group
{
protected:
    static void newgrid(W_Grid* widget, minion::MMap* props);
    void handle_child_resized() override;
    void grid_layout();
    int nrows = 0;
    int ncols = 0;
    int vgap = 0;
    int hgap = 0;

public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_Grid* make(minion::MMap* props);

    void grid_sizes();
};

class W_Layout : public W_Grid
{
    static W_Layout* new_hvgrid(minion::MMap* parammap, bool horizontal);

    bool horizontal = false;

public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_Layout* make_hlayout(
        minion::MMap* props)
    {
        return new_hvgrid(props, true);
    }
    static W_Layout* make_vlayout(
        minion::MMap* props)
    {
        return new_hvgrid(props, false);
    }
};

class W_Stack : public W_Group
{
    Fl_Widget* current = nullptr;

    //void handle_child_size(Fl_Widget* wc, int ww, int wh) override;
    //void handle_child_modified(Widget* wc) override;

public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_Stack* make(minion::MMap* props);
};

class W_EditForm : public W_Grid
{
    struct labelled_form_element
    {
        W_Labelled_Widget* element;
        int child_index;
        bool span;
    };

    std::vector<labelled_form_element> labelled_elements;
    int label_pos = 0;  // 0=>left, 1=>centre, 2=>right
    int v_labelgap = 5; // vertical space between label and spanning widget

    void handle_child_resized() override;

public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_EditForm* make(minion::MMap* props);
};

#endif // LAYOUT_H
