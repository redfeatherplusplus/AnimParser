// Author: Daren Cheng
// Class:  CS 4392
// Date:   11/15/2015

// Desc: 
// Header file that contains the balljoint class

//include guard
#ifndef BALLJOINT_H_INCLUDED
#define BALLJOINT_H_INCLUDED

//include dependancies
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

class Balljoint
{
private:
    static map<int, vector<float>> joints_lcs;  //joint lcs to joint id map

    string id;                    //id of this joint
    vector<Balljoint> subjoints;  //subjoints of this joint
    float offset[3];              //offset from parent joint
    float boxmin[3];              //bottom left corner of box
    float boxmax[3];              //top right corner of box
    float pose[3];                //current pose of the box
    float rotxlimit[2];           //upper and lower x rotation limits
    float rotylimit[2];           //upper and lower y rotation limits
    float rotzlimit[2];           //upper and lower z rotation limits
    int joint_id;                 //numerical id of this joint

    void parseBalljoint(ifstream &skelFile, int &joint_id);
    void setAttribute(string token, ifstream &skelFile);
    void generateLocalMatrix();
    void calculate_joints_lcs(double *parentMatrix, int &joint_id);
public:
    static int numJoints;  //number of joints in this skeleton

    Balljoint();
    map<int, vector<float>> getJointsLcs();
    void getBalljoint(ifstream &skelFile);
    void getName(string &id, int joint_id);
    void getPose(double *pose, int joint_id);
    void setPose(double *pose, int joint_id);
    void setRootOffset(double *offset);
    void draw(double *parentMatrix);
    void print(int &depth);
};

#endif