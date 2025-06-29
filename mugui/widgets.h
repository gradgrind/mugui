#ifndef WIDGETS_H
#define WIDGETS_H

#include "minion.h"
#include "widget.h"
#include <FL/Fl_Widget.H>

// *** non-layout widgets

class W_Labelled_Widget : public Widget
{
public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;

    int label_width = 0, label_height = 0;
};

class W_Box : public Widget
{
public:
    static W_Box* make(minion::MMap* props);
};

class W_Hline : public W_Box
{
public:
    static W_Hline* make(minion::MMap* props);
};

class W_Vline : public W_Box
{
public:
    static W_Vline* make(minion::MMap* props);
};

class W_Label : public Widget
{
public:
    //void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_Label* make(minion::MMap* props);
};

class W_PushButton : public W_Label
{
protected:
    float contrast; // contrast (0.0 - 1.0) between on colour and off colour
    void set_colour(Fl_Color clr);

public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_PushButton* make(minion::MMap* props);
};

class W_RadioButton : public W_PushButton
{
public:
    //void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_RadioButton* make(minion::MMap* props);
};

class W_Checkbox : public Widget
{
public:
    static W_Checkbox* make(minion::MMap* props);
};

class W_Choice : public W_Labelled_Widget
{
public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_Choice* make(minion::MMap* props);
};

class W_Output : public W_Labelled_Widget
{
public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_Output* make(minion::MMap* props);
};

class W_PopupEditor : public W_Output
{
public:
    //void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_PopupEditor* make(minion::MMap* props);
};

class W_List : public W_Labelled_Widget
{
public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_List* make(minion::MMap* props);
};

class W_TextLine : public W_Labelled_Widget
{
public:
    //? void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_TextLine* make(minion::MMap* props);

    bool set(std::string_view newtext);
};

class W_RowTable : public W_Labelled_Widget
{
public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_RowTable* make(minion::MMap* props);
};

#endif // WIDGETS_H
