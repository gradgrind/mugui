#ifndef DIALOGS_H
#define DIALOGS_H

#include "layout.h"

class W_Dialog : public W_Window
{
public:
    void handle_method(std::string_view method, minion::MList* paramlist) override;
    static W_Dialog* make(minion::MMap* props);
};

#endif // DIALOGS_H
