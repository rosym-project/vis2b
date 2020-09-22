// SPDX-License-Identifier: LGPL-3.0
#include <vis2b/functions/osg.h>

#include <math.h>

#define NR_JOINTS 2

int main(void)
{
    struct gc_pose pose[NR_JOINTS] = {
        {
            .rotation = (struct matrix3x3 [1]) { {
                    .row_x = { 1.0, 0.0        ,  0.0 },
                    .row_y = { 0.0, sin(M_PI_4), -cos(M_PI_4) },
                    .row_z = { 0.0, cos(M_PI_4),  sin(M_PI_4) } } },
            .translation = (struct vector3 [1]) { { 0.0, 0.0, 0.15 } }
        },
        {
            .rotation = (struct matrix3x3 [1]) { {
                    .row_x = { 1.0, 0.0,  0.0 },
                    .row_y = { 0.0, 0.0, -1.0 },
                    .row_z = { 0.0, 1.0,  0.0 } } },
            .translation = (struct vector3 [1]) { {
                    0.0, cos(M_PI_4) * 0.15, sin(M_PI_4) * 0.15 + 0.15 } }
        }
    };

    struct vis2b_osg_nbx vis = {
        // Configuration
        .size = NR_JOINTS,
        .mesh = (const char *[]) {
            "cylinder.stl",
            "cylinder.stl",
            "cylinder.stl"
        },
        // Connections
        .pose = pose
    };

    vis2b_osg_configure(&vis);
    vis2b_osg_start(&vis);

    for (int i = 0; i < 1000; i++) {
        vis2b_osg_update(&vis);
    }

    vis2b_osg_cleanup(&vis);

    return 0;
}
