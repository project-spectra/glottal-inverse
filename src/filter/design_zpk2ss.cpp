#include "filter_design.h"


void convert_zpk2ss(const filter_zpk& src, filter_ss& dst)
{
    filter_tf inter;

    convert_zpk2tf(src, inter);
    convert_tf2ss(inter, dst);
}
