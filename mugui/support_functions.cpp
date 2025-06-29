#include "support_functions.h"
#include <map>
#include <stdexcept>
using namespace std;

// Read a string with 6 characters as a hex integer, returning the value
// multiplied by 0x100 to make an FLTK colour value.
Fl_Color get_colour(
    const string& colour)
{
    if (colour.size() == 6) {
        unsigned int i;
        try {
            size_t index;
            i = stoul(colour, &index, 16);
            if (index == colour.size()) return i * 0x100;
        } catch (...) {}
    }
    string msg{"Invalid colour: '" + colour + "'"};
    throw msg; 
}

// Not all box types supported by FLTK are included here
inline const map<string_view, Fl_Boxtype> boxtypes{
    {"NO_BOX", FL_NO_BOX},
    {"FLAT_BOX", FL_FLAT_BOX},
    {"UP_BOX", FL_UP_BOX},
    {"DOWN_BOX", FL_DOWN_BOX},
    {"BORDER_BOX", FL_BORDER_BOX},
    {"ROUNDED_BOX", FL_ROUNDED_BOX},
    {"ROUND_UP_BOX", FL_ROUND_UP_BOX},
    {"ROUND_DOWN_BOX", FL_ROUND_DOWN_BOX},
    {"THIN_UP_BOX", FL_THIN_UP_BOX},
    {"THIN_DOWN_BOX", FL_THIN_DOWN_BOX},
    {"ENGRAVED_BOX", FL_ENGRAVED_BOX},
    {"EMBOSSED_BOX", FL_EMBOSSED_BOX},
    {"SHADOW_BOX", FL_SHADOW_BOX},
    {"RSHADOW_BOX", FL_RSHADOW_BOX},
    {"RFLAT_BOX", FL_RFLAT_BOX},
    // frames (no background)
    {"UP_FRAME", FL_UP_FRAME},
    {"DOWN_FRAME", FL_DOWN_FRAME},
    {"BORDER_FRAME", FL_BORDER_FRAME},
    {"ROUNDED_FRAME", FL_ROUNDED_FRAME},
    {"THIN_UP_FRAME", FL_THIN_UP_FRAME},
    {"THIN_DOWN_FRAME", FL_THIN_DOWN_FRAME},
    {"ENGRAVED_FRAME", FL_ENGRAVED_FRAME},
    {"EMBOSSED_FRAME", FL_EMBOSSED_FRAME},
    {"SHADOW_FRAME", FL_SHADOW_FRAME},
};

Fl_Boxtype get_boxtype(
    const string& boxtype)
{
    try {
        return boxtypes.at(boxtype);
    } catch (out_of_range& e) {
        throw "Unknown BOXTYPE: " + boxtype;
    }
}
