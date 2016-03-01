// Author: Daren Cheng
// Class:  CS 4392
// Date:   10/18/2015

// Desc: 
// Header file that contains the Skin class

//include guard
#ifndef SKIN_H_INCLUDED
#define SKIN_H_INCLUDED

//include dependancies
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

class Skin
{
private:
    vector<vector<float>> positions;
    vector<vector<float>> normals;
    vector<vector<float>> ssa_positions;
    vector<vector<float>> ssa_normals;
    vector<vector<pair<int, float>>> skinweights;
    vector<vector<int>> triangles;
    vector<vector<float>> bindings_inverse;
    map<int, set<vector<int>>> joints;

    void getAttribute(string token, ifstream &skinFile);
    void getJoints();
    void drawTriangle(vector<int> triangle);
public:
    Skin();
    void getSkin(ifstream &skinFile);
    void smooth(map<int, vector<float>> &joints_lcs);
    void outlineJoint(int joint_id, float *lineColor);
    void drawJoints();
    void draw();
    void print();
};

#endif