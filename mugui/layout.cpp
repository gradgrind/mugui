#include "callback.h"
#include "layout.h"
#include "minion.h"
#include "widget.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Wizard.H>
#include <map>
using namespace std;
using namespace minion;

inline const map<string, Fl_Grid_Align>
    GRID_ALIGN{/** Align the widget in the middle of the cell (default). */
               {"CENTRE", FL_GRID_CENTER},
               /** Align the widget at the top of the cell. */
               {"TOP", FL_GRID_TOP},
               /** Align the widget at the bottom of the cell. */
               {"BOTTOM", FL_GRID_BOTTOM},
               /** Align the widget at the left side of the cell. */
               {"LEFT", FL_GRID_LEFT},
               /** Align the widget at the right side of the cell. */
               {"RIGHT", FL_GRID_RIGHT},
               /** Stretch the widget horizontally to fill the cell. */
               {"HORIZONTAL", FL_GRID_HORIZONTAL},
               /** Stretch the widget vertically to fill the cell. */
               {"VERTICAL", FL_GRID_VERTICAL},
               /** Stretch the widget in both directions to fill the cell. */
               {"FILL", FL_GRID_FILL},
               /** Stretch the widget proportionally. */
               {"PROPORTIONAL", FL_GRID_PROPORTIONAL},
               {"TOP_LEFT", FL_GRID_TOP_LEFT},
               {"TOP_RIGHT", FL_GRID_TOP_RIGHT},
               {"BOTTOM_LEFT", FL_GRID_BOTTOM_LEFT},
               {"BOTTOM_RIGHT", FL_GRID_BOTTOM_RIGHT}};

void callback_close_window(
    Fl_Widget* w, void* ud)
{
    //(void) ud;
    if (Fl::callback_reason() == FL_REASON_CANCELLED) {
        // escape key pressed
        int esc_quit = 0;
        static_cast<Widget*>(ud)->property_int("ESC_CLOSES", esc_quit);
        if (esc_quit == 0)
            return;
    }

    //TODO: message to backend?

    //TODO: If changed data, ask about closing
    //if (!fl_choice("Are you sure you want to quit?", "continue", "quit", NULL)) {
    //    return;
    //}

    w->hide();
}

// *** layout widgets – Fl_Group based

void W_Window::make_window(
    int ww, int wh, W_Window* widget, MMap* props)
{
    props->get_int("WIDTH", ww);
    props->get_int("HEIGHT", wh);
    auto w = new Fl_Double_Window(ww, wh);
    w->callback(callback_close_window);
    widget->container = new Fl_Flex(0, 0, 0, 0);
    //w->box(FL_EMBOSSED_BOX);
    Fl_Group::current(0); // disable "auto-grouping"
    widget->fl_widget = w;
    props->get_int("MIN_WIDTH", ww);
    props->get_int("MIN_HEIGHT", wh);
    w->size_range(ww, wh);

    widget->container->resize(0, 0, w->w(), w->h());
    w->resizable(widget->container);
}

W_Window* W_Window::make(
    MMap* props)
{
    auto widget = new W_Window();
    make_window(800, 600, widget, props);
    return widget;
}

void W_Window::handle_method(
    std::string_view method, minion::MList* paramlist)
{
    if (method == "SHOW") {
        container->layout();
        fl_widget->show();
    } else if (method == "TEXT") {
        string lbl;
        if (paramlist->get_string(1, lbl)) {
            fl_widget->copy_label(lbl.c_str());
        } else
            throw "TEXT value missing for window " + *widget_name();
    } else if (method == "SET_LAYOUT") {
        string w;
        if (paramlist->get_string(1, w)) {
            if (container->children() != 0) {
                throw "Window " + *widget_name() + " already has child: ";
            }
            container->add(get_fltk_widget(w));
        } else
            throw "SET_LAYOUT widget missing for window " + *widget_name();
    } else {
        throw string{"Unknown method on window " + *widget_name() + ": "}.append(method);
    }
}

void W_Grid::newgrid(W_Grid* widget, MMap* props)
{
    auto w = new Fl_Grid(0, 0, 0, 0);
    w->box(FL_NO_BOX);
    Fl_Group::current(0); // disable "auto-grouping"
    widget->fl_widget = w;
    widget->set_box(props);
    // Row/column gaps
    props->get_int("ROW_GAP", widget->vgap);
    props->get_int("COL_GAP", widget->hgap);
    if (props->get_int("GAP", widget->vgap))
        widget->hgap = widget->vgap;
    // Margins
    props->get_int("MARGIN", widget->margin);
}

W_Grid* W_Grid::make(MMap* props)
{
    auto widget = new W_Grid();
    newgrid(widget, props);
    return widget;
}

void W_Grid::handle_child_resized()
{
    if (nrows == 0 || ncols == 0)
        return;
    auto flgrid = static_cast<Fl_Grid*>(fl_widget);
    // Reset margin and gaps
    flgrid->margin(margin, margin, margin, margin);
    flgrid->gap(vgap, hgap);
    grid_layout();
}

void W_Grid::grid_layout()
{
    auto flgrid = static_cast<Fl_Grid*>(fl_widget);
    vector<int> rsize(nrows, 0);
    vector<int> csize(ncols, 0);
    for (int r = 0; r < nrows; ++r) {
        for (int c = 0; c < ncols; ++c) {
            auto cel = flgrid->cell(r, c);
            if (cel) {
                // Use the larger of the content size + margins + border size
                // and the explicit minimum size
                auto fcw = cel->widget();
                auto cw = static_cast<Widget*>(fcw->user_data());
                auto box = fcw->box();
                int w = Fl::box_dx(box) * 2 //
                        + cw->margin * 2    //
                        + cw->content_width //
                        + cw->h_content_padding * 2;
                if (w < cw->minimum_width)
                    w = cw->minimum_width;
                int h = Fl::box_dy(box) * 2  //
                        + cw->margin * 2     //
                        + cw->content_height //
                        + cw->v_content_padding * 2;
                if (h < cw->minimum_height)
                    h = cw->minimum_height;
                cel->minimum_size(w, h);

                printf("MIN SIZE: %s %d %d\n", cw->widget_name()->c_str(), w, h);

                // Take spans into consideration when determining running
                // col/row sizes (span != 1 not counted)
                if (cel->colspan() == 1 && w > csize.at(c))
                    csize[c] = w;
                if (cel->rowspan() == 1 && h > rsize.at(r))
                    rsize[r] = h;
            }
        }
    }
    int wh = rsize.at(nrows - 1);
    for (int r = 0; r < nrows - 1; ++r)
        wh += flgrid->row_gap(r) + rsize.at(r);
    int ww = csize.at(ncols - 1);
    for (int c = 0; c < ncols - 1; ++c)
        ww += flgrid->col_gap(c) + csize.at(c);
    content_width = ww;
    content_height = wh;
    printf("GRID SIZE (%s): %d %d => %d %d\n", widget_name()->c_str(), nrows, ncols, ww, wh);
    fflush(stdout);

    static_cast<Fl_Grid*>(fl_widget)->layout();
    W_Group::child_resized(this);
}

void W_Grid::handle_method(
    string_view method, MList* paramlist)
{
    if (method == "ADD") {
        // Add new children to grid
        auto flgrid = static_cast<Fl_Grid*>(fl_widget);
        auto n = paramlist->size();
        if (n < 2)
            throw "No widget(s) to ADD to " + *widget_name();
        for (size_t i = 1; i < n; ++i) {
            auto w_i = paramlist->get(i);
            auto wlistp = w_i.m_list();
            if (wlistp) {
                auto wlist = wlistp->get();
                string wname;
                if (wlist->get_string(0, wname)) {
                    auto wchild = get_widget(wname);
                    auto flchild = wchild->fltk_widget();
                    if (flgrid->cell(flchild))
                        throw "Widget " + wname + " already in layout " + *widget_name();
                    int row, col, rspan = 1, cspan = 1;
                    if (wlist->get_int(1, row) && wlist->get_int(2, col)) {
                        wlist->get_int(3, rspan);
                        wlist->get_int(4, cspan);
                        flgrid->add(flchild);

                        Fl_Grid_Align align = FL_GRID_CENTER;
                        string fill;
                        if (wchild->property_string("GRID_ALIGN", fill)) {
                            try {
                                align = GRID_ALIGN.at(fill);
                            } catch (out_of_range& e) {
                                throw string{"Invalid GRID_ALIGN: "} + fill;
                            }
                        }

                        // Place the child widget ...
                        //  ... first check rows and cols (with spans)
                        if (row >= 0 && col >= 0 && rspan > 0 && cspan > 0) {
                            if (col + cspan > ncols) {
                                ncols = col + cspan;
                            }
                            if (row + rspan > nrows) {
                                nrows = row + rspan;
                            }
                            flgrid->layout(nrows, ncols);
                            flgrid->widget(flchild, row, col, rspan, cspan, align);
                        } else {
                            throw "Widget " + *wchild->widget_name()
                                + ", Grid placement invalid: \n " + to_string(row) + "+"
                                + to_string(rspan) + " / " + to_string(col) + "+"
                                + to_string(cspan);
                        }
                        continue;
                    }
                    throw "Grid ADD command needs two values (row and column), grid: "
                        + *widget_name();
                }
            }
            throw "Invalid grid ADD command on '" + *widget_name() + "':\n  " + dump_value(w_i);
        }
        handle_child_resized();
        return;
    }

    // Set row weights
    if (method == "ROW_WEIGHTS") {
        auto fw = static_cast<Fl_Grid*>(fl_widget);
        auto n = paramlist->size();
        for (size_t i = 1; i < n; ++i) {
            auto m = paramlist->get(i);
            if (auto wlp = m.m_list()) {
                auto wl = wlp->get();
                int row, wt;
                if (wl->get_int(0, row) && wl->get_int(1, wt)) {
                    fw->row_weight(row, wt);
                    continue;
                }
            }
            throw "Invalid ROW_WEIGHTS value: " + dump_value(m);
        }

        //fw->layout();
        return;
    }

    // Set column weights
    if (method == "COL_WEIGHTS") {
        auto fw = static_cast<Fl_Grid*>(fl_widget);
        auto n = paramlist->size();
        for (size_t i = 1; i < n; ++i) {
            auto m = paramlist->get(i);
            if (auto wlp = m.m_list()) {
                auto wl = wlp->get();
                int col, wt;
                if (wl->get_int(0, col) && wl->get_int(1, wt)) {
                    fw->col_weight(col, wt);
                    continue;
                }
            }
            throw "Invalid COL_WEIGHTS value: " + dump_value(m);
        }

        //fw->layout();
        return;
    }

    if (method == "SHOW_GRID") {
        int show_grid = 0;
        paramlist->get_int(1, show_grid);
        static_cast<Fl_Grid*>(fl_widget)->show_grid(show_grid);
        return;
    }

    Widget::handle_method(method, paramlist);
    return;
}

//static
W_Layout* W_Layout::new_hvgrid(
    MMap* props, bool horizontal)
{
    auto widget = new W_Layout();
    newgrid(widget, props);
    widget->horizontal = horizontal;
    if (horizontal)
        widget->nrows = 1;
    else
        widget->ncols = 1;
    return widget;
}

void W_Layout::handle_method(
    std::string_view method, minion::MList* paramlist)
{
    if (method == "ADD") {
        auto flgrid = static_cast<Fl_Grid*>(fl_widget);
        auto n = paramlist->size();
        if (n < 2)
            throw "No widget(s) to ADD to " + *widget_name();
        // Add new children to list
        for (size_t i = 1; i < n; ++i) {
            string wname;
            if (paramlist->get_string(i, wname)) {
                auto wc = get_widget(wname);
                auto flchild = wc->fltk_widget();
                if (flgrid->cell(flchild))
                    throw "Widget " + wname + " already in layout " + *widget_name();

                flgrid->add(flchild);
                Fl_Grid_Align align = FL_GRID_CENTER;
                string fill;
                if (wc->property_string("GRID_ALIGN", fill)) {
                    try {
                        align = GRID_ALIGN.at(fill);
                    } catch (out_of_range& e) {
                        throw string{"Invalid GRID_ALIGN: "} + fill;
                    }
                }
                int weight = 0;
                wc->property_int("GRID_GROW", weight);
                if (horizontal) {
                    ++ncols;
                    flgrid->layout(1, ncols);
                    flgrid->widget(flchild, 0, ncols - 1, align);
                    flgrid->col_weight(ncols - 1, weight);
                } else {
                    ++nrows;
                    flgrid->layout(nrows, 1);
                    flgrid->widget(flchild, nrows - 1, 0, align);
                    flgrid->row_weight(nrows - 1, weight);
                }
            } else {
                MValue m0{*paramlist};
                throw "Invalid ADD to layout " + *widget_name() + ": " + dump_value(m0);
            }
        }
        handle_child_resized();
        return;
    }

    W_Grid::handle_method(method, paramlist);
}

void W_Stack::handle_method(
    std::string_view method, minion::MList* paramlist)
{
    if (method == "ADD") {
        string wname;
        if (paramlist->get_string(1, wname)) {
            fl_widget->as_group()->add(get_fltk_widget(wname));
        }
    } else if (method == "SELECT") {
        string wname;
        if (paramlist->get_string(1, wname)) {
            auto subw = Widget::get_fltk_widget(wname);
            if (current)
                current->hide();
            subw->show();
            subw->take_focus();
            current = subw;
            static_cast<Fl_Flex*>(fl_widget)->layout();
        } else {
            throw "Method SELECT without widget";
        }
    } else {
        Widget::handle_method(method, paramlist);
    }
}

//static method
W_Stack* W_Stack::make(
    minion::MMap* props)
{
    (void) props;
    auto w = new Fl_Flex(0, 0, 0, 0);
    w->box(FL_EMBOSSED_BOX);
    Fl_Group::current(0); // disable "auto-grouping"
    auto widget = new W_Stack();
    widget->fl_widget = w;
    widget->set_box(props);

    return widget;
}
