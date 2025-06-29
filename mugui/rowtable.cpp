#include "rowtable.h"
#include "callback.h"
#include "support_functions.h"
#include "widgets.h"
#include <FL/fl_draw.H>
using namespace std;
using namespace minion;

//TODO: After changes I might need to force a redraw (redraw())?

RowTable::RowTable()
    : Fl_Table_Row(0, 0, 0, 0)
{
    col_resize(0); // disable manual column resizing
    row_resize(0); // disable manual row resizing
    row_header(0); // disable row headers (along left)
    col_header(0); // disable column headers (along top)
    type(Fl_Table_Row::SELECT_SINGLE);

    callback([](Fl_Widget* w, void* ud) {
        (void) ud;
        w->take_focus();
    });
}

// Need to handle the effect of column changes on data stores.
void RowTable::set_cols(
    int n)
{
    int nr = rows();
    cols(n);
    col_headers.resize(n);
    if (n && nr) {
        for (int i = 0; i < nr; ++i) {
            data.at(i).resize(n);
        }
    }
}

// Need to handle the effect of row changes on data stores.
void RowTable::set_rows(
    int n)
{
    int nc = cols();
    rows(n);
    row_headers.resize(n);
    if (n && nc) {
        data.resize(n, vector<string>(nc));
    }
}

void RowTable::draw_cell(
    TableContext context, int ROW, int COL, int X, int Y, int W, int H)
{
    switch (context) {
    case CONTEXT_STARTPAGE: // before page is drawn..
        //fl_font(FL_HELVETICA, 16); // set the font for the drawing operations

        // Adjust column widths
        size_columns();

        // Handle change of selected row
        if (Fl_Table::select_row != _current_row) {
            select_row(Fl_Table::select_row);
            _current_row = Fl_Table::select_row;
            Fl::add_timeout(0.0, _row_cb, this);
        }
        return;

    case CONTEXT_COL_HEADER: // Draw column headers
        fl_push_clip(X, Y, W, H);
        {
            // Draw cell bg
            Fl_Color bg = col_header_color();
            fl_color(bg);
            Fl_Color textclr = fl_contrast(Widget::normal_fg, bg);
            fl_rectf(X, Y, W, H);
            // Draw cell data
            fl_color(textclr);
            fl_draw(col_headers[COL].c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            // Draw box border
            fl_rect(X, Y, W, H);
        }
        fl_pop_clip();
        return;

    case CONTEXT_ROW_HEADER: // Draw row headers
        fl_push_clip(X, Y, W, H);
        {
            // Draw cell bg
            Fl_Color bg = row_header_color();
            fl_color(bg);
            Fl_Color textclr = fl_contrast(Widget::normal_fg, bg);
            fl_rectf(X, Y, W, H);
            // Draw cell data
            fl_color(textclr);
            fl_draw(row_headers[ROW].c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            // Draw box border
            fl_rect(X, Y, W, H);
        }
        fl_pop_clip();
        return;

    case CONTEXT_CELL: // Draw data in cells
        fl_push_clip(X, Y, W, H);
        {
            // Draw cell bg
            Fl_Color textclr;
            if (row_selected(ROW)) {
                //if (ROW == _current_row) {
                fl_color(Widget::selection_bg);
                textclr = fl_contrast(Widget::normal_fg, Widget::selection_bg);
            } else {
                fl_color(Widget::normal_bg);
                textclr = Widget::normal_fg;
            }
            fl_rectf(X, Y, W, H);
            // Draw cell data
            fl_color(textclr);
            fl_draw(data[ROW][COL].c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            // Draw box border
            fl_color(textclr);
            fl_rect(X, Y, W, H);
        }
        fl_pop_clip();
        return;

    default:
        return;
    }
}

//TODO: Do I need to set font (fl_font()) before using fl_measure()?
void RowTable::size_columns()
{
    int ncols = cols();
    int nrows = rows();
    int cw, ch, wmax;

    // Deal with row headers
    wmax = 0;
    if (row_header()) {
        for (int r = 0; r < nrows; ++r) {
            cw = 0;
            fl_measure(row_headers[r].c_str(), cw, ch, 0);
            if (cw > wmax)
                wmax = cw;
        }
    }
    int rhw = wmax + 10;
    row_header_width(rhw);

    // Get widest column entries
    struct colwidth
    {
        int col, width;
    };
    std::vector<colwidth> colwidths;

    for (int c = 0; c < ncols; ++c) {
        cw = 0;
        fl_measure(col_headers[c].c_str(), cw, ch, 0);
        wmax = cw;
        for (int r = 0; r < nrows; ++r) {
            cw = 0;
            fl_measure(data[r][c].c_str(), cw, ch, 0);
            if (cw > wmax)
                wmax = cw;
        }
        colwidths.emplace_back(colwidth{c, wmax});
    }
    std::sort(colwidths.begin(), colwidths.end(), [](colwidth a, colwidth b) {
        return a.width > b.width;
    });

    //for (const auto &i : colwidths)
    //    cout << "$ " << i.col << ": " << i.width << endl;

    int restwid = tiw;
    int icols = ncols;
    const int padwidth = 4;
    for (colwidth cw : colwidths) {
        if (icols == 1) {
            if (cw.width + padwidth < restwid) {
                col_width(cw.col, restwid);
            } else {
                col_width(cw.col, cw.width + padwidth);
            }
        } else {
            int defwid = restwid / icols;
            --icols;
            if (cw.width + padwidth < defwid) {
                col_width(cw.col, defwid);
                restwid -= defwid;
            } else {
                col_width(cw.col, cw.width + padwidth);
                restwid -= cw.width + padwidth;
            }
        }
    }
}

// This is the callback mechanism for row-selection change
void RowTable::_row_cb(
    void *table)
{
    auto ww = static_cast<RowTable *>(table);
    string* dw{Widget::get_widget_name(ww)};
    int i = ww->_current_row;
    if (i >= 0) {
        //TODO: Do I want the row data?
        auto rowheader = ww->row_headers.at(i);
        auto row = ww->data.at(i);
        MList ml{};
        ml.emplace_back(rowheader);
        for (const auto& s : row) {
            ml.emplace_back(s);
        }
        Callback2(*dw, to_string(i), ml);
    } else {
        Callback1(*dw, to_string(i));
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

W_RowTable* W_RowTable::make(minion::MMap* parammap)
{
    (void) parammap;
    auto w = new RowTable();
    auto widget = new W_RowTable();
    widget->fl_widget = w;

    Fl_Color cx = (fl_lightness(normal_bg) < 50) ? 0xffffff00 : 0;
    auto cp = fl_color_average(cx, normal_bg, table_header_contrast);
    w->header_bg = cp;
    w->col_header_color(cp);
    w->row_header_color(cp);
    return widget;
}

void W_RowTable::handle_method(std::string_view method, minion::MList* paramlist)
{
    auto t = static_cast<RowTable*>(fl_widget);

    //TODO: It would probably be better to use an enum and switch!
    if (method == "rows") {
        int n;
        if (paramlist->get_int(1, n))
            t->set_rows(n);
    } else if (method == "cols") {
        int n;
        if (paramlist->get_int(1, n))
            t->set_cols(n);
    } else if (method == "row_header_width") {
        int rhw;
        if (paramlist->get_int(1, rhw) && rhw > 0) {
            t->row_header(1);
            t->row_header_width(rhw);
        } else {
            t->row_header(0);
        }
    } else if (method == "col_header_height") {
        int chh;
        if (paramlist->get_int(1, chh) && chh > 0) {
            t->col_header(1);
            t->col_header_height(chh);
        } else {
            t->col_header(0);
        }
    } else if (method == "col_header_colour") {
        string clr;
        if (paramlist->get_string(1, clr)) {
            t->col_header_color(get_colour(clr));
        } else {
            t->col_header_color(t->header_bg);
        }
    } else if (method == "row_header_colour") {
        string clr;
        if (paramlist->get_string(1, clr)) {
            t->row_header_color(get_colour(clr));
        } else {
            t->row_header_color(t->header_bg);
        }
    } else if (method == "row_height_all") {
        int h;
        if (paramlist->get_int(1, h))
            t->row_height_all(h);
    } else if (method == "col_width_all") {
        int w;
        if (paramlist->get_int(1, w))
            t->col_width_all(w);
    } else if (method == "col_headers") {
        t->col_headers.clear();
        auto n = paramlist->size() - 1;
        t->set_cols(n);
        string hdr;
        for (size_t i = 0; i < n; ++i) {
            paramlist->get_string(i + 1, hdr);
            t->col_headers[i] = hdr;
        }
    } else if (method == "row_headers") {
        t->row_headers.clear();
        auto n = paramlist->size() - 1;
        t->set_rows(n);
        string hdr;
        for (size_t i = 0; i < n; ++i) {
            paramlist->get_string(i + 1, hdr);
            t->row_headers[i] = hdr;
        }
    } else if (method == "add_row") {
        auto n = paramlist->size() - 2;
        if (n != static_cast<size_t>(t->cols())) {
            throw "RowTable: add_row with wrong length";
        }
        string hdr;
        paramlist->get_string(1, hdr);
        t->row_headers.emplace_back(hdr);
        vector<string> r(n);
        for (size_t i = 0; i < n; ++i) {
            paramlist->get_string(i + 2, hdr);
            r[i] = hdr;
        }
        t->data.emplace_back(r);
        t->rows(t->rows() + 1);
    } else {
        W_Labelled_Widget::handle_method(method, paramlist);
    }
}
