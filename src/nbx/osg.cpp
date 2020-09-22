// SPDX-License-Identifier: LGPL-3.0
#include <vis2b/functions/osg.h>

#include <vector>
#include <cassert>

#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/Version>

#include <osgGA/OrbitManipulator>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#if OSG_MIN_VERSION_REQUIRED(3,4,0)
#include <osgViewer/config/SingleWindow>
#endif

#include <osgDB/ReadFile>


struct vis2b_osg
{
    osgViewer::Viewer *viewer;
    std::vector<osg::MatrixTransform *> tf;
};


void create_frame(osg::Transform *root)
{
    double height = 0.15;
    double radius = 0.005;
    osg::Vec3 pos(0.0, 0.0, height / 2.0);

    osg::MatrixTransform *x_tf = new osg::MatrixTransform();
    osg::Matrix x_matrix = osg::Matrix::rotate(-osg::PI_2, 0.0, 1.0, 0.0);
    x_tf->setMatrix(x_matrix);
    osg::ShapeDrawable *x_shape = new osg::ShapeDrawable(new osg::Cylinder(pos, radius, height));
    x_shape->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));
    osg::Geode *x_geode = new osg::Geode();
    x_geode->addDrawable(x_shape);
    x_tf->addChild(x_geode);
    root->addChild(x_tf);

    osg::MatrixTransform *y_tf = new osg::MatrixTransform();
    osg::Matrix y_matrix = osg::Matrix::rotate(osg::PI_2, 1.0, 0.0, 0.0);
    y_tf->setMatrix(y_matrix);
    osg::ShapeDrawable *y_shape = new osg::ShapeDrawable(new osg::Cylinder(pos, radius, height));
    y_shape->setColor(osg::Vec4(0.0, 1.0, 0.0, 1.0));
    osg::Geode *y_geode = new osg::Geode();
    y_geode->addDrawable(y_shape);
    y_tf->addChild(y_geode);
    root->addChild(y_tf);

    osg::MatrixTransform *z_tf = new osg::MatrixTransform();
    osg::ShapeDrawable *z_shape = new osg::ShapeDrawable(new osg::Cylinder(pos, radius, height));
    z_shape->setColor(osg::Vec4(0.0, 0.0, 1.0, 1.0));
    osg::Geode *z_geode = new osg::Geode();
    z_geode->addDrawable(z_shape);
    z_tf->addChild(z_geode);
    root->addChild(z_tf);
}


void vis2b_osg_configure(struct vis2b_osg_nbx *b)
{
    assert(b);

    b->osg = new vis2b_osg;
    b->osg->viewer = new osgViewer::Viewer;
    b->osg->tf.resize(b->size);

    osg::Group *root = new osg::Group();

    // Initialize material to color the robot's links
    osg::Material *material = new osg::Material;
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1.0));
    osg::StateSet *material_state_set = new osg::StateSet;
    material_state_set->setAttribute(material);

    // Create the robot's base link
    osg::MatrixTransform *world = new osg::MatrixTransform();
    create_frame(world);
    if ((b->size > 0) && (b->mesh)) {
        std::string file = b->mesh_base ? std::string(b->mesh_base) + "/" : "";
        file += b->mesh[0];

        osg::Node *mesh = osgDB::readNodeFile(file);
        mesh->asGroup()->getChild(0)->asGeode()->setStateSet(material_state_set);
        world->addChild(mesh);
    }

    root->addChild(world);

    // Create the robot's further links
    for (int i = 0; i < b->size; i++) {
        b->osg->tf[i] = new osg::MatrixTransform();
        create_frame(b->osg->tf[i]);

        if (b->mesh) {
            std::string file = b->mesh_base ? std::string(b->mesh_base) + "/" : "";
            file += b->mesh[i + 1];

            osg::Node *mesh = osgDB::readNodeFile(file);
            mesh->asGroup()->getChild(0)->asGeode()->setStateSet(material_state_set);
            b->osg->tf[i]->addChild(mesh);
        }

        root->addChild(b->osg->tf[i]);
    }

    b->osg->viewer->setSceneData(root);


    // Initialize the camera
    osg::Matrix look_at;
    osg::Vec3 eye(0.0, -4.0, 0.0);
    osg::Vec3 centre(0.0, 0.0, 0.0);
    osg::Vec3 up(0.0, 0.0, 1.0);
    look_at.makeLookAt(eye, centre, up);
    b->osg->viewer->getCamera()->setViewMatrix(look_at);

    // Initialize a camera controller
    osgGA::OrbitManipulator *manipulator = new osgGA::OrbitManipulator;
    manipulator->setWheelZoomFactor(-1.0 * manipulator->getWheelZoomFactor());
    b->osg->viewer->setCameraManipulator(manipulator);


    // Initialize the application window
    unsigned int width;
    unsigned int height;
    osg::GraphicsContext::WindowingSystemInterface *wsi = osg::GraphicsContext::getWindowingSystemInterface();
    osg::GraphicsContext::ScreenIdentifier main_screen_id;
    main_screen_id.readDISPLAY();
    main_screen_id.setUndefinedScreenDetailsToDefaultScreen();
    wsi->getScreenResolution(main_screen_id, width, height);
#if OSG_MIN_VERSION_REQUIRED(3,4,0)
    b->osg->viewer->apply(new osgViewer::SingleWindow(width / 4, height / 4, width / 2, height / 2));
#else
    b->osg->viewer->setUpViewInWindow(width / 4, height / 4, width / 2, height / 2);
#endif

    // Run in single-threaded mode
    b->osg->viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
}


void vis2b_osg_start(struct vis2b_osg_nbx *b)
{
    assert(b);
    assert(b->osg);
    assert(b->osg->viewer);

    b->osg->viewer->realize();
}


void vis2b_osg_update(struct vis2b_osg_nbx *b)
{
    assert(b);
    assert(b->osg);
    assert(b->osg->viewer);
    assert(b->pose);

    for (int i = 0; i < b->size; i++) {
        matrix3x3 *rot = &b->pose[i].rotation[0];
        vector3 *trans = &b->pose[i].translation[0];

        osg::Matrix m;
        m.set(
            rot->row[0].x, rot->row[0].y, rot->row[0].z, 0.0,
            rot->row[1].x, rot->row[1].y, rot->row[1].z, 0.0,
            rot->row[2].x, rot->row[2].y, rot->row[2].z, 0.0,
            trans->x     , trans->y     , trans->z     , 1.0);
        b->osg->tf[i]->setMatrix(m);
    }

    b->osg->viewer->frame();
}


void vis2b_osg_cleanup(struct vis2b_osg_nbx *b)
{
    assert(b);
    assert(b->osg);

    delete b->osg->viewer;
    delete b->osg;
}
