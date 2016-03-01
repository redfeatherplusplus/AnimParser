// Author: Daren Cheng
// Class:  CS 4392
// Date:   10/18/2015

// Desc: 
// Implements methods given in "Balljoint.h"

//include interface that this file implements
#include "Skin.h"
#include "Functions.h"

//global variables
int v1;  //first vertex of triangle
int v2;  //second vertex of triangle
int v3;  //third vertex of triangle

///////////////////////
//  private methods  //
///////////////////////

//get the current skin attribute
void Skin::getAttribute(string token, ifstream &skinFile) {
    //for each attribute of the skin, get the corresponding parameters
    bool rightBraceParsed = false;

    //parse skinFile until a right brace is found for each attribute
    if (token == "positions") {
        //ignore syntatic number of elements and left curly brace
        skinFile >> token;
        skinFile >> token;

        //get position of each point, stop when right curly brace parsed
        while (!rightBraceParsed && !skinFile.eof()) {
            skinFile >> token;
            if (token != "}") {
                vector<float> position;
                position.push_back(stof(token));
                skinFile >> token;
                position.push_back(stof(token));
                skinFile >> token;
                position.push_back(stof(token));

                //add additional '1' to point to fit <x, y, z, 1> format
                position.push_back(1.0f);

                //add this position to the vector of positions
                positions.push_back(position);
            }
            else {
                rightBraceParsed = true;
            }
        }
    }
    else if (token == "normals") {
        //ignore syntatic number of elements and left curly brace
        skinFile >> token;
        skinFile >> token;

        //get normal of each point, stop when right curly brace parsed
        while (!rightBraceParsed && !skinFile.eof()) {
            skinFile >> token;
            if (token != "}") {
                vector<float> normal;
                normal.push_back(stof(token));
                skinFile >> token;
                normal.push_back(stof(token));
                skinFile >> token;
                normal.push_back(stof(token));

                //add additional '0' to vector to fit <x, y, z, 0> format
                normal.push_back(0.0f);

                //add this normal to the vector of normals
                normals.push_back(normal);
            }
            else {
                rightBraceParsed = true;
            }
        }
    }
    else if (token == "skinweights") {
        //ignore syntatic number of elements and left curly brace
        skinFile >> token;
        skinFile >> token;

        //get normal of each point, stop when right curly brace parsed
        int numSkinWeights;
        while (!rightBraceParsed && !skinFile.eof()) {
            skinFile >> token;
            if (token != "}") {
                //first token is number of skin weights for this point
                numSkinWeights = stoi(token);
                vector<pair<int, float>> skinweight;

                //add each joint's weight to this point's skinweight
                pair<int, float> jointWeight;
                for (int i = 0; i < numSkinWeights; i++) {
                    skinFile >> token;
                    jointWeight.first = stoi(token);
                    skinFile >> token;
                    jointWeight.second = stof(token);

                    //add this joint's weight to the skinweight
                    skinweight.push_back(jointWeight);
                }

                //add this point's skinweight to the vector of skin weights
                skinweights.push_back(skinweight);
            }
            else {
                rightBraceParsed = true;
            }
        }
    }
    else if (token == "triangles") {
        //ignore syntatic number of elements and left curly brace
        skinFile >> token;
        skinFile >> token;

        //get normal of each point, stop when right curly brace parsed
        while (!rightBraceParsed && !skinFile.eof()) {
            skinFile >> token;
            if (token != "}") {
                vector<int> triangle;
                triangle.push_back(stoi(token));
                skinFile >> token;
                triangle.push_back(stoi(token));
                skinFile >> token;
                triangle.push_back(stoi(token));

                //add this triangle to the vector of triangles
                triangles.push_back(triangle);
            }
            else {
                rightBraceParsed = true;
            }
        }
    }
    else if (token == "bindings") {
        //ignore syntatic number of elements and left curly brace
        skinFile >> token;
        skinFile >> token;
        while (!rightBraceParsed && !skinFile.eof()) {
            skinFile >> token;
            if (token != "}") {
                //matrix found, ignore syntatic left curly brace and parse
                skinFile >> token;
                vector<float> binding;
                for (int i = 0; i < 12; i++) {
                    //add missing <0 0 0 1> vector
                    if (i % 3 == 0 && i != 0) {
                        binding.push_back(0.0f);
                    }

                    //add contents of binding matrix
                    skinFile >> token;
                    binding.push_back(stof(token));
                }
                //add final missing '1' of the <0 0 0 1> vector
                binding.push_back(1.0f);

                //matrix parsed, ignore syntatic right curly brace
                skinFile >> token;

                //pre-compute the inverse of this binding matrix
                inverse(binding);

                //add this binding to the vector of bindings
                bindings_inverse.push_back(binding);
            }
            else {
                rightBraceParsed = true;
            }
        }
    }
}

//group triangles into joints based on skinweights
void Skin::getJoints() {
    for (vector<int> triangle : triangles) {
        for (int point : triangle) {
            for (pair<int, float> joint : skinweights[point]) {
                //add if this triangle has a significant joint weight
                if (joint.second > 0.5) {
                    joints[joint.first].insert(triangle);
                }
            }
        }
    }
}

//draw a single triangle
void Skin::drawTriangle(vector<int> triangle) {
    //get the verticies specified by this triangle
    v1 = triangle[0];
    v2 = triangle[1];
    v3 = triangle[2];

    //first vertex
    glNormal3f(ssa_normals[v1][0], ssa_normals[v1][1], ssa_normals[v1][2]);
    glVertex3f(ssa_positions[v1][0], ssa_positions[v1][1], ssa_positions[v1][2]);

    //second vertex
    glNormal3f(ssa_normals[v2][0], ssa_normals[v2][1], ssa_normals[v2][2]);
    glVertex3f(ssa_positions[v2][0], ssa_positions[v2][1], ssa_positions[v2][2]);

    //third vertex
    glNormal3f(ssa_normals[v3][0], ssa_normals[v3][1], ssa_normals[v3][2]);
    glVertex3f(ssa_positions[v3][0], ssa_positions[v3][1], ssa_positions[v3][2]);

    /* //draw normals
    glBegin(GL_LINES);
    glVertex3f(ssa_positions[v1][0], ssa_positions[v1][1], ssa_positions[v1][2]);
    glVertex3f(ssa_positions[v1][0] + ssa_normals[v1][0] * 0.1,
        ssa_positions[v1][1] + ssa_normals[v1][1] * 0.1,
        ssa_positions[v1][2] + ssa_normals[v1][2] * 0.1);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(ssa_positions[v2][0], ssa_positions[v2][1], ssa_positions[v2][2]);
    glVertex3f(ssa_positions[v2][0] + ssa_normals[v2][0] * 0.1,
        ssa_positions[v2][1] + ssa_normals[v2][1] * 0.1,
        ssa_positions[v2][2] + ssa_normals[v2][2] * 0.1);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(ssa_positions[v3][0], ssa_positions[v3][1], ssa_positions[v3][2]);
    glVertex3f(ssa_positions[v3][0] + ssa_normals[v3][0] * 0.1,
        ssa_positions[v3][1] + ssa_normals[v3][1] * 0.1,
        ssa_positions[v3][2] + ssa_normals[v3][2] * 0.1);
    glEnd(); */
}

//////////////////////
//  public methods  //
//////////////////////

//Skin default constructor
Skin::Skin() {
    //initialize stuff
}

//get the skin from the skinFile by getting each of the skin's attributes
void Skin::getSkin(ifstream &skinFile) {
    string token;
    while (!skinFile.eof()) {
        skinFile >> token;
        getAttribute(token, skinFile);
    }

    //initialize ssa_positions and ssa_normals
    for (int i = 0; i < positions.size(); i++) {
        ssa_positions.push_back(positions[i]);
        ssa_normals.push_back(normals[i]);
    }

    //group triangles into joints based on skinweights
    getJoints();
}

//perform SSA on each vertex, and store the result in ssa_positions
void Skin::smooth(map<int, vector<float>> &joints_lcs) {

    //operate on a copy of each position
    for (int i = 0; i < positions.size(); i++) {

        //store position of a single joint,normal, and initialize ssa variables
        vector<float> joint_position;
        vector<float> joint_normal;
        for (int j = 0; j < 4; j++) {
            joint_position.push_back(0.0);
            joint_normal.push_back(0.0);
            ssa_positions[i][j] = 0.0f;
            ssa_normals[i][j] = 0.0f;
        }

        //calculate and weigh each joint's influence on the point
        for (pair<int, float> jointWeight : skinweights[i]) {
            //maintain each weighted position in a tmp vector
            for (int j = 0; j < 4; j++) {
                joint_position[j] = positions[i][j];
                joint_normal[j] = normals[i][j];
            }

            //move vector into the joint's lcs by undoing binding
            multiplyVector(joint_position, bindings_inverse[jointWeight.first]);
            multiplyVector(joint_normal, bindings_inverse[jointWeight.first]);

            //multiply by joint's current wcs transformation
            multiplyVector(joint_position, joints_lcs[jointWeight.first]);
            multiplyVector(joint_normal, joints_lcs[jointWeight.first]);

            //scale this joint's influence on the point as desired
            multiplyScalar(joint_position, jointWeight.second);
            multiplyScalar(joint_normal, jointWeight.second);

            //ssa_position and normal is the sum of all weighted joint values
            for (int j = 0; j < 4; j++) {
                ssa_positions[i][j] += joint_position[j];
                ssa_normals[i][j] += joint_normal[j];
            }
        }
    }
}

//outline joint of interest 
void Skin::outlineJoint(int joint_id, float *lineColor) {
    //save current rendering settings
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushAttrib(GL_LIGHTING_BIT);

    //rendering settings for outline
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-5.0f, -5.0f);
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor4f(lineColor[0], lineColor[1], lineColor[2], lineColor[3]);
    glLineWidth(2.5f);

    //draw outline
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    for (vector<int> triangle : joints[joint_id]) {
        drawTriangle(triangle);
    }
    glEnd();
    glPopMatrix();

    //rendering settings for object
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_SMOOTH);
    
    //restore lighting and rendering mode
    glPopAttrib();

    //re-draw object with current depth
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    for (vector<int> triangle : joints[joint_id]) {
        drawTriangle(triangle);
    }
    glEnd();
    glPopMatrix();

    //restore rendering settings
    glPopAttrib();

}

//draw each joint in a unique color
void Skin::drawJoints() {
    //save current rendering settings
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    //draw each triangle in the skin
    glEnable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    for (auto &joint : joints) {
        for (vector<int> triangle : joint.second) {
            glColor3ub(joint.first + 1, joint.first + 1, joint.first + 1);
            drawTriangle(triangle);
        }
    }
    glEnd();
    glPopMatrix();

    //restore rendering settings
    glPopAttrib();
}

//draw entire skin
void Skin::draw() {
    //draw each triangle in the skin
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    for (vector<int> triangle : triangles) {
        drawTriangle(triangle);
    }
    glEnd();
    glPopMatrix();
}

//print out attribute for testing purposes
void Skin::print() {
    int i = 0;
    for (vector<float> binding : bindings_inverse) {
        //print out elements of current position
        for (float element : binding) {
            cout << element << " ";
            i++;
            if (i == 4) {
                i = 0;
                cout << endl;
            }
        }
        cout << endl;
    }

    //print out size
    cout << "size: " << skinweights.size() << endl;
}