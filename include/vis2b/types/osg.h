// SPDX-License-Identifier: LGPL-3.0
#ifndef VIS2B_TYPES_OSG_H
#define VIS2B_TYPES_OSG_H

#include <dyn2b/types/geometry.h>


#ifdef __cplusplus
extern "C" {
#endif

struct vis2b_osg;

struct vis2b_osg_nbx
{
    // Configuration
    int size;
    const char *mesh_base;
    const char **mesh;          // [size + 1] or NULL
    // Ports
    const struct gc_pose *pose; // [size]
    // Internal state
    struct vis2b_osg *osg;
};

#ifdef __cplusplus
}
#endif

#endif
