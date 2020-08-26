// SPDX-License-Identifier: LGPL-3.0
#ifndef VIS2B_FUNCTIONS_OSG_H
#define VIS2B_FUNCTIONS_OSG_H

#include <vis2b/types/osg.h>


#ifdef __cplusplus
extern "C" {
#endif

void vis2b_osg_configure(struct vis2b_osg_nbx *b);

void vis2b_osg_start(struct vis2b_osg_nbx *b);

void vis2b_osg_update(struct vis2b_osg_nbx *b);

void vis2b_osg_cleanup(struct vis2b_osg_nbx *b);

#ifdef __cplusplus
}
#endif

#endif
