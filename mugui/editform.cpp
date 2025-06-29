#include "layout.h"
#include <FL/Enumerations.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Grid.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/fl_draw.H>
#include <string>
using namespace std;
using namespace minion;

// static
W_EditForm* W_EditForm::make(
    minion::MMap* props)
{
    (void) props;
    auto flgrid = new Fl_Grid(0, 0, 0, 0);
    Fl_Group::current(0); // disable "auto-grouping"
    flgrid->box(FL_BORDER_FRAME);
    //col_width(0, 0);
    //col_weight(0, 0);
    //gap(10, 0);
    //margin(5, 5, 5, 5);
    auto widget = new W_EditForm();
    widget->fl_widget = flgrid;
    //flgrid->color(Widget::entry_bg);
    { // Label alignment
        string align_;
        props->get_string("LABEL_POS", align_);
        if (align_ == "CENTRE") {
            widget->label_pos = 1;
        } else if (align_ == "RIGHT") {
            widget->label_pos = 2;
        }
    }
    // Row/column gaps
    props->get_int("ROW_GAP", widget->vgap);
    props->get_int("LABEL_GAP", widget->hgap);
    props->get_int("V_LABEL_GAP", widget->v_labelgap);
    // Margins
    props->get_int("MARGIN", widget->margin);
    widget->ncols = 2;
    return widget;
}

void W_EditForm::handle_child_resized()
{
    auto flgrid = static_cast<Fl_Grid*>(fl_widget);
    // Reset margin
    flgrid->margin(margin, margin, margin, margin);
    int lwidth = 0;
    for (const auto& lfe : labelled_elements) {
        if (!lfe.span) {
            auto lw = lfe.element->label_width;
            if (lw > lwidth)
                lwidth = lw;
        }
    }
    flgrid->gap(vgap, lwidth + hgap);
    for (const auto& lfe : labelled_elements) {
        if (lfe.span) {
            if (auto lh = lfe.element->label_height) {
                lfe.element->fltk_widget()->vertical_label_margin(v_labelgap);
                auto dh = v_labelgap + lh;
                if (lfe.child_index == 0) {
                    // stretched top margin
                    flgrid->margin(-1, dh + margin);
                } else {
                    // stretched gap
                    flgrid->row_gap(lfe.child_index - 1, dh + vgap);
                }
            }
        } else if (auto lw = lfe.element->label_width) {
            auto dw = hgap;
            if (label_pos == 0) {
                dw += lwidth - lw;
            } else if (label_pos == 1) {
                dw += (lwidth - lw) / 2;
            }
            lfe.element->fltk_widget()->horizontal_label_margin(dw);
        }
    }
    grid_layout();
}

void W_EditForm::handle_method(
    std::string_view method, minion::MList* paramlist)
{
    if (method == "ADD") {
        auto flgrid = static_cast<Fl_Grid*>(fl_widget);

        // Add new children to list
        auto n = paramlist->size();
        if (n < 2)
            throw "No widget(s) to ADD to " + *widget_name();
        for (size_t i = 1; i < n; ++i) {
            string wname;
            if (paramlist->get_string(i, wname)) {
                auto wc = get_widget(wname);
                auto fwc = wc->fltk_widget();
                // Check that it is new to the layout
                for (int i = 0; i < nrows; ++i) {
                    if (fwc == flgrid->child(i))
                        throw "Widget " + wname + " already in layout " + *widget_name();
                }
                flgrid->add(fwc);
                flgrid->layout(nrows + 1, 2);
                int span = 0;
                wc->property_int("SPAN", span);
                auto flwc = wc->fltk_widget();
                if (span == 0) {
                    flgrid->widget(flwc, nrows, 1);
                    flgrid->row_weight(nrows, 0);
                    if (auto lwc = dynamic_cast<W_Labelled_Widget*>(wc)) {
                        flwc->align(FL_ALIGN_LEFT);
                        labelled_elements.emplace_back(labelled_form_element{lwc, nrows, false});
                    }
                } else {
                    flgrid->widget(flwc, nrows, 0, 1, 2);
                    if (span == 1)
                        flgrid->row_weight(nrows, 0);
                    else
                        flgrid->row_weight(nrows, 1);
                    if (auto lwc = dynamic_cast<W_Labelled_Widget*>(wc)) {
                        flwc->align(FL_ALIGN_TOP_LEFT);
                        labelled_elements.emplace_back(labelled_form_element{lwc, nrows, true});
                    }
                }
                ++nrows;
            }
        }
        flgrid->col_weight(0, 0);
        handle_child_resized();
        return;
    }

    W_Grid::handle_method(method, paramlist);
}
