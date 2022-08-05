#ifndef __csg_fence_hpp__
#define __csg_fence_hpp__

const unsigned g_csg_fence_hpp_size = 11;

const int16_t g_csg_fence_hpp[] =
{
    6, 0, 0, 0, 0, 230, 0, 1, 7, 7, 96,
#if !defined(__x86_64__) && !defined(__i386__)
    0,
#endif
};

#endif