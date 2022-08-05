#ifndef __csg_bridge_hpp__
#define __csg_bridge_hpp__

const unsigned g_csg_bridge_hpp_size = 33;

const int16_t g_csg_bridge_hpp[] =
{
    6, 0, 970, -3250, 0, 970, 3250, 2, 800, 140, 112, 6, 520, 1015, -3250,
    520, 1015, 3250, 2, 240, 90, 112, 6, -520, 1015, -3250, -520, 1015, 3250,
    2, 240, 90, 112,
#if !defined(__x86_64__) && !defined(__i386__)
    0,
#endif
};

#endif