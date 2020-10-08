return block
{
    name = "vis2b/osg",
    license = "LGPL3",
    meta_data = "Simple robot visualizer based on Open Scene Graph (OSG)",

    port_cache = true,

    types = {
        { name = "gc_pose", class="struct" },
    },

    configurations = {
        { name = "nr_joints", type_name = "int", min = 1, max = 1, doc = "Number of joints" },
        { name = "mesh_base", type_name = "char", min = 0, doc = "Directory from which meshes should be loaded" },
        { name = "mesh_names", type_name = "char", min = 0, doc = "Colon-separated list of mesh file names" },
    },

    ports = {
        { name = "poses", in_type_name = "struct gc_pose", doc = "Array of poses for each link in the kinematic chain" },
    },

    operations = { start=true, step=true }
}
