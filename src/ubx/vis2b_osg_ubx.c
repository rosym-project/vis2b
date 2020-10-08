// SPDX-License-Identifier: LGPL-3.0
#include "vis2b_osg_ubx.h"
#include <vis2b/functions/osg.h>


struct vis2b_osg_ubx_info
{
    int nr_joints;
    const char *mesh_base;
    char **mesh;
    struct gc_pose *pose;
    struct vis2b_osg_nbx nblock;

    struct vis2b_osg_ubx_port_cache ports;
};


void vis2b_osg_ubx_free_private_data(struct vis2b_osg_ubx_info *inf)
{
    if (!inf) return;

    if (inf->mesh) {
        for (int i = 0; i < inf->nr_joints + 1; i++) {
            if (!inf->mesh[i]) continue;
            free(inf->mesh[i]);
        }
    }

    if (inf->pose) {
        for (int i = 0; i < inf->nr_joints; i++) {
            if (inf->pose[i].translation) free(inf->pose[i].translation);
            if (inf->pose[i].rotation) free(inf->pose[i].rotation);
        }
    }

    free(inf);
}


int vis2b_osg_ubx_init(ubx_block_t *b)
{
    int ret = -1;
    struct vis2b_osg_ubx_info *inf;

    if (!(inf = calloc(1, sizeof(struct vis2b_osg_ubx_info)))) {
        ubx_err(b, "vis2b_osg_ubx: failed to alloc memory for block info");
        ret = EOUTOFMEM;
        goto out;
    }
    b->private_data = inf;
    update_port_cache(b, &inf->ports);

    // Get configuration information
    const int *nr_joints;
    int nr_joints_len = cfg_getptr_int(b, "nr_joints", &nr_joints);
    if (nr_joints_len != 1) {
        ubx_err(b, "vis2b_osg_ubx: nr_joints must be supplied");
        goto out;
    }
    inf->nr_joints = *nr_joints;

    inf->mesh = NULL;
    inf->mesh_base = NULL;
    int mesh_base_len = cfg_getptr_char(b, "mesh_base", &inf->mesh_base);

    const char *mesh_names;
    int mesh_names_len = cfg_getptr_char(b, "mesh_names", &mesh_names);

    inf->mesh = NULL;
    if (mesh_names_len > 0) {
        if (!(inf->mesh = calloc(inf->nr_joints + 1, sizeof(char *)))) {
            ubx_err(b, "vis2b_osg_ubx: failed to alloc memory for mesh names");
            ret = EOUTOFMEM;
            goto out;
        }

        const char *name = mesh_names;
        char *sep;
        for (int i = 0; i < inf->nr_joints + 1; ++i, name = sep + 1) {
            sep = strstr(name, ":");
            unsigned int length = sep
                    ? (sep - name)
                    : ((mesh_names_len - 1) - (name - mesh_names));

            if (!(inf->mesh[i] = calloc(length + 1, sizeof(char)))) {
                ubx_err(b, "vis2b_osg_ubx: failed to alloc memory for mesh names");
                ret = EOUTOFMEM;
                goto out;
            }

            strncpy(inf->mesh[i], name, length);
        }
    }

    // Cache ports and set dimensions
    inf->ports.poses = ubx_port_get(b, "poses");
    inf->ports.poses->in_data_len = inf->nr_joints;

    // Allocate buffers
    if (!(inf->pose = calloc(inf->nr_joints, sizeof(struct gc_pose)))) {
        ubx_err(b, "vis2b_osg_ubx: failed to alloc memory for poses");
        ret = EOUTOFMEM;
        goto out;
    }

    for (int i = 0; i < inf->nr_joints; ++i) {
        if (!(inf->pose[i].translation = calloc(1, sizeof(struct vector3)))) {
            ubx_err(b, "vis2b_osg_ubx: failed to alloc memory for translation");
            ret = EOUTOFMEM;
            goto out;
        }

        if (!(inf->pose[i].rotation = calloc(1, sizeof(struct matrix3x3)))) {
            ubx_err(b, "vis2b_osg_ubx: failed to alloc memory for rotation");
            ret = EOUTOFMEM;
            goto out;
        }

        inf->pose[i].rotation->row[0].data[0] = 1.0;
        inf->pose[i].rotation->row[1].data[1] = 1.0;
        inf->pose[i].rotation->row[2].data[2] = 1.0;
    }

    // Connect to nanoblx ports
    struct vis2b_osg_nbx *nblock = &inf->nblock;
    nblock->size = inf->nr_joints;
    nblock->mesh_base = inf->mesh_base;
    nblock->mesh = (const char **)inf->mesh;
    nblock->pose = inf->pose;

    // Configure visualization
    vis2b_osg_configure(nblock);
    ret = 0;

out:
    if (ret != 0) {
        vis2b_osg_ubx_free_private_data(inf);
    }

    return ret;
}


int vis2b_osg_ubx_start(ubx_block_t *b)
{
    struct vis2b_osg_ubx_info *inf = (struct vis2b_osg_ubx_info *)b->private_data;

    vis2b_osg_start(&inf->nblock);
}


void vis2b_osg_ubx_cleanup(ubx_block_t *b)
{
    struct vis2b_osg_ubx_info *inf = (struct vis2b_osg_ubx_info *)b->private_data;

    vis2b_osg_cleanup(&inf->nblock);
    vis2b_osg_ubx_free_private_data(inf);
}


void vis2b_osg_ubx_step(ubx_block_t *b)
{
    struct vis2b_osg_ubx_info *inf = (struct vis2b_osg_ubx_info *)b->private_data;

    read_gc_pose_array(inf->ports.poses, inf->pose, inf->nr_joints);

    // Render the latest data (reading the poses may not have produced new data)
    vis2b_osg_update(&inf->nblock);
}
