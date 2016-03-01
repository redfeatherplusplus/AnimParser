// Author: Daren Cheng
// Class:  CS 4392
// Date:   11/15/2015

// Desc: 
// This program animates a posable model
// and given a skel, skin, and anim file

//include dependancies
#include <AntTweakBar.h>
#include <GL/freeglut.h>
#include <windows.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

//include local header files
#include "Balljoint.h"
#include "Skin.h"
#include "Anim.h"
#include "Functions.h"

//function prototypes
void display();
void reshape(int width, int height);
void TW_CALL resetAnim(void *clientData);
void TW_CALL centerCamera(void *clientData);

//variables that determine rendering setings
TwBar *bar;                                                       //pointer to tweakbar
float ambientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };                //ambient light color
float lightPosition0[] = { 2.5f, 2.5f, 2.5f, 1.0f };              //position of sunlight0
float lightColor0[] = { 0.913725f, 0.513725f, 0.0f, 1.0f };       //color of sunlight0
float lightPosition1[] = { -2.5f, 2.5f, -2.5f, 1.0f };            //position of sunlight1
float lightColor1[] = { 0.411764f, 0.745098f, 0.156862f, 1.0f };  //color of sunlight1 
float lineColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };                   //color of object outlines  

//variables defined globally so they aren't redefined each display call
Anim anim;                //anim of the model
Skin skin;                //skin of the model
Balljoint base;           //base joint to hold the tree of joints
double initial_time;      //initial time of program starting
double current_time;      //current time from system clock
double previous_time;     //time of previous update
double update_fps;        //updates per second
double pose[3] = { 0 };   //pose of a given joint  
bool loop_anim = true;    //animation loops if true
bool follow_obj = false;  //camera follows object if true

//lookat and viewport parameters
int width = 640;
int height = 480;
double eye_x = 0.0;
double eye_y = 0.0;
double eye_z = 0.0;
double center_x = 0.0;
double center_y = 0.0;
double center_z = 0.0;

using namespace std;

int main(int argc, char** argv) {
    //print out anim file name and create file stream
    cout << "Filename: " << argv[1] << endl;
    ifstream animFile;
    animFile.open(argv[1]);

    //print out skin file name and create file stream
    cout << "Filename: " << argv[2] << endl;
    ifstream skinFile;
    skinFile.open(argv[2]);

    //print out skel file name and create file stream
    cout << "Filename: " << argv[3] << endl;
    ifstream skelFile;
    skelFile.open(argv[3]);

    //set update fps as specified
    update_fps = stod(argv[4]);

    //set camera position as specified
    eye_x = stod(argv[5]);
    eye_y = stod(argv[6]);
    eye_z = stod(argv[7]);
    center_x = stod(argv[8]);
    center_y = stod(argv[9]);
    center_z = stod(argv[10]);

    //check if anim file opened correctly, then parse file into the anim
    if (animFile.is_open()) {
        //attempt to get the anim
        anim.getAnim(animFile);
    }
    animFile.close();

    //check if skin file opened correctly, then parse file into the skin
    if (skinFile.is_open()) {
        //attempt to get the skin
        skin.getSkin(skinFile);
    }
    skinFile.close();

    //check if skel file opened correctly, then parse file into balljoints
    if (skelFile.is_open()) {
        //check to see if the first token in the skel file is a root joint
        string token;
        skelFile >> token;

        if (token == "balljoint") {
            //get the entire skeleton by getting the base and its children
            base.getBalljoint(skelFile);
        }
        else {
            cout << "Error, .skel file must start with a root balljoint." << endl;
        }
    }
    skelFile.close();

    //test if anim file parsed correctly
    //anim.print();

    //perform smoothing based on initial pose of the joints
    skin.smooth(base.getJointsLcs());

    //initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("AnimParser.cpp");
    glutCreateMenu(NULL);

    //get GLUT callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    //initialize AntTweakBar
    TwInit(TW_OPENGL, NULL);

    //control redirect all mouse and keyboard events to AntTweakBar
    //glutIgnoreKeyRepeat(true);
    glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
    glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
    glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    TwGLUTModifiersFunc(glutGetModifiers);

    //create a tweak bar
    bar = TwNewBar("Translations");

    //add a animation controls
    TwAddVarRO(bar, "Time", TW_TYPE_DOUBLE, &current_time, " precision=3");
    TwAddVarRW(bar, "Update FPS", TW_TYPE_DOUBLE, &update_fps,
        " min=1 max=144 step=1");
    TwAddVarRW(bar, "Loop Anim", TW_TYPE_BOOLCPP, &loop_anim, NULL);
    TwAddButton(bar, "Reset Anim", resetAnim, NULL, NULL);
    TwAddSeparator(bar, NULL, NULL);

    //add camera motions
    TwAddVarRW(bar, "Follow Object", TW_TYPE_BOOLCPP, &follow_obj, NULL);
    TwAddVarRW(bar, "Camera_X", TW_TYPE_DOUBLE, &eye_x,
        " min=-15 max=15 step=0.2 keyIncr=d keyDecr=a ");
    TwAddVarRW(bar, "Camera_Y", TW_TYPE_DOUBLE, &eye_y,
        " min=-15 max=15 step=0.2 keyIncr=e keyDecr=q ");
    TwAddVarRW(bar, "Camera_Z", TW_TYPE_DOUBLE, &eye_z,
        " min=-15 max=15 step=0.2 keyIncr=s keyDecr=w ");
    TwAddButton(bar, "Center Camera", centerCamera, NULL, NULL);
    TwAddSeparator(bar, NULL, NULL);

    //add color controllers
    TwAddVarRW(bar, "Light Color 0", TW_TYPE_COLOR4F, &lightColor0, NULL);
    TwAddVarRW(bar, "Light Color 1", TW_TYPE_COLOR4F, &lightColor1, NULL);

    //initialize time
    initial_time = (double)GetTickCount() / 1000.0;
    current_time = (double)GetTickCount() / 1000.0 - initial_time;
    previous_time = (double)GetTickCount() / 1000.0 - initial_time;

    //call the GLUT main loop
    glutMainLoop();

    cin.get();
    return 0;
}

//callback function passed to glutDisplayFunc
void display() {
    //clear frame buffer to avoid rendering issues
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //rendering settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //set camera angle and position, along with viewport
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (double)width / height, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_x, eye_y, eye_z, center_x, center_y, center_z, 0, 1, 0);

    //draw WCS axis
    drawWCSAxis();

    //Enable lighting and create a light
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    //Add ambient light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    //Add two 3/4 spotlights
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);

    //get current time
    current_time = (double)GetTickCount() / 1000.0 - initial_time;

    //check if we should update in this draw call to maintain update fps
    if (current_time - previous_time >= 1.0 / update_fps) {
        //an update is necessary, check if the current time is in the range of the animation
        if ((current_time <= anim.getMaxRange() && current_time >= anim.getMinRange()) || loop_anim) {
            //get current root translation
            anim.evaluateRoot(pose, current_time);

            //update root transation
            base.setRootOffset(pose);

            //follow object if flag is raised to do so
            if (follow_obj) {
                center_x = pose[0];
                center_y = pose[1];
                center_z = pose[2];
            }

            //update every joints rotation
            for (int i = 0; i < base.numJoints; i++) {
                //get current pose based on time from the animation
                anim.evaluateJoint(pose, current_time, i);

                //update pose of indicated joint
                base.setPose(pose, i);
            }

            //update smoothing based on current pose of the joints
            skin.smooth(base.getJointsLcs());

            //indicate current time as time of the previous update
            previous_time = current_time;
        }
    }

    //draw model by drawing the skin
    skin.draw();

    //Draw and refresh AntTweakBar
    TwDraw();
    TwRefreshBar(bar);

    //Swap Buffers for double buffering
    glutSwapBuffers();

    //Indicate need for new display() call
    glutPostRedisplay();

}

//callback function called by GLUT when window size changes
void reshape(int width_, int height_)
{
    width = width_;
    height = height_;
    // Set OpenGL viewport and camera
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (double)width / height, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_x, eye_y, eye_z, center_x, center_y, center_z, 0, 1, 0);

    //send the new window size to AntTweakBar
    TwWindowSize(width, height);
}

//callback function that resets animation
void TW_CALL resetAnim(void *clientData) {
    initial_time = (double)GetTickCount() / 1000.0;
    current_time = (double)GetTickCount() / 1000.0 - initial_time;
    previous_time = (double)GetTickCount() / 1000.0 - initial_time;
}

//callback function that centers camera to center of WCS
void TW_CALL centerCamera(void *clientData) {
    center_x = 0.0;
    center_y = 0.0;
    center_z = 0.0;
}