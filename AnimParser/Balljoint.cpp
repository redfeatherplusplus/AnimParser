// Author: Daren Cheng
// Class:  CS 4392
// Date:   11/15/2015

// Desc: 
// Implements methods given in "Balljoint.h"

//include interface that this file implements
#include "Balljoint.h"
#include "Functions.h"

//global variables
double localMatrix[4 * 4];  //local transformation matrix
double tmpMatrix[4 * 4];    //matrix used to rotate local

//function prototypes
bool isAlphabetic(string token);

//static variable declarations
map<int, vector<float>> Balljoint::joints_lcs;
int Balljoint::numJoints;

///////////////////////
//  private methods  //
///////////////////////

//parse balljoint from skelFile
void Balljoint::parseBalljoint(ifstream &skelFile, int &joint_id) {
    //balljoint token parsed, get id of the joint
    string token;
    skelFile >> token;
    id = token;

    //ignore syntactic left curly brace
    skelFile >> token;

    //initialize joints_lcs
    this->joint_id = joint_id;
    for (int i = 0; i < 16; i++) {
        joints_lcs[joint_id].push_back(0.0);
    }
    joint_id++;

    //read skel file until we reach the right curly brace for this balljoint
    bool rightBraceParsed = false;
    while (!rightBraceParsed && !skelFile.eof()) {
        skelFile >> token;

        //check if token indicates a subjoint
        if (token == "balljoint") {
            //get the subjoint
            Balljoint subjoint;
            subjoint.parseBalljoint(skelFile, joint_id);

            //add the subjoint to this joint
            subjoints.push_back(subjoint);
        }
        //check if token indicates an attribute
        else if (isAlphabetic(token)) {
            //if token is an attribute, set the attribute for this joint
            setAttribute(token, skelFile);
        }
        //check if token is a right curly brace
        else if (token == "}") {
            rightBraceParsed = true;
        }
        else {
            cout << "Error, unexpected token found while parsing skel file." << endl;
        }
    }
}

//set the attribute for a given joint
void Balljoint::setAttribute(string token, ifstream &skelFile) {
    //for each parameter of the attribute, set the parameter
    if (token == "offset") {
        for (float &parameter : offset) {
            skelFile >> token;
            parameter = stof(token);
        }
    }
    else if (token == "boxmin") {
        for (float &parameter : boxmin) {
            skelFile >> token;
            parameter = stof(token);
        }
    }
    else if (token == "boxmax") {
        for (float &parameter : boxmax) {
            skelFile >> token;
            parameter = stof(token);
        }
    }
    else if (token == "pose") {
        for (float &parameter : pose) {
            skelFile >> token;
            parameter = stof(token);
        }
    }
    else if (token == "rotxlimit") {
        for (float &parameter : rotxlimit) {
            skelFile >> token;
            parameter = stof(token);
        }
    }
    else if (token == "rotylimit") {
        for (float &parameter : rotylimit) {
            skelFile >> token;
            parameter = stof(token);
        }
    }
    else if (token == "rotzlimit") {
        for (float &parameter : rotzlimit) {
            skelFile >> token;
            parameter = stof(token);
        }
    }
    else {
        cout << "Error, unexpected attribute found." << endl;
    }
}

//generate local transformation matrix
void Balljoint::generateLocalMatrix() {
    //initialize local matrix
    initializeMatrix(localMatrix);

    //calculate rotation amounts
    double theta_x;
    double theta_y;
    double theta_z;
    double cos_theta;
    double sin_theta;

    //clip 'x' angle
    if (pose[0] < rotxlimit[0]) {
        theta_x = rotxlimit[0];
    }
    else if (pose[0] > rotxlimit[1]) {
        theta_x = rotxlimit[1];
    }
    else {
        theta_x = pose[0];
    }

    //clip 'y' angle
    if (pose[1] < rotylimit[0]) {
        theta_y = rotylimit[0];
    }
    else if (pose[1] > rotylimit[1]) {
        theta_y = rotylimit[1];
    }
    else {
        theta_y = pose[1];
    }

    //clip 'z' angle
    if (pose[2] < rotzlimit[0]) {
        theta_z = rotzlimit[0];
    }
    else if (pose[2] > rotzlimit[1]) {
        theta_z = rotzlimit[1];
    }
    else {
        theta_z = pose[2];
    }

    //rotate around x-axis
    initializeMatrix(tmpMatrix);
    cos_theta = cos(theta_x);
    sin_theta = sin(theta_x);
    tmpMatrix[4 * 1 + 1] = cos_theta;
    tmpMatrix[4 * 1 + 2] = sin_theta;
    tmpMatrix[4 * 2 + 1] = -sin_theta;
    tmpMatrix[4 * 2 + 2] = cos_theta;
    multiplyMatrix(localMatrix, tmpMatrix);

    //rotate around y-axis
    initializeMatrix(tmpMatrix);
    cos_theta = cos(theta_y);
    sin_theta = sin(theta_y);
    tmpMatrix[4 * 0 + 0] = cos_theta;
    tmpMatrix[4 * 0 + 2] = -sin_theta;
    tmpMatrix[4 * 2 + 0] = sin_theta;
    tmpMatrix[4 * 2 + 2] = cos_theta;
    multiplyMatrix(localMatrix, tmpMatrix);

    //rotate around z-axis
    initializeMatrix(tmpMatrix);
    cos_theta = cos(theta_z);
    sin_theta = sin(theta_z);
    tmpMatrix[4 * 0 + 0] = cos_theta;
    tmpMatrix[4 * 0 + 1] = sin_theta;
    tmpMatrix[4 * 1 + 0] = -sin_theta;
    tmpMatrix[4 * 1 + 1] = cos_theta;
    multiplyMatrix(localMatrix, tmpMatrix);

    //offset local matrix
    localMatrix[4 * 3 + 0] = offset[0];
    localMatrix[4 * 3 + 1] = offset[1];
    localMatrix[4 * 3 + 2] = offset[2];
}

//calculate lcs for each joint
void Balljoint::calculate_joints_lcs(double *parentMatrix, int &joint_id) {
    //generate local matrix
    generateLocalMatrix();

    //save parent matrix for this joint
    double current_parent[4 * 4];
    initializeMatrix(current_parent);
    multiplyMatrix(current_parent, parentMatrix);

    //calculate local matrix in terms of wcs
    multiplyMatrix(localMatrix, parentMatrix);

    //save joint's lcs into joints_lcs, increment joint id and size of skeleton
    for (int i = 0; i < 16; i++) {
        joints_lcs[joint_id][i] = localMatrix[i];
    }
    joint_id++;
    numJoints++;

    //update parent matrix with local for subjoints of this joint
    initializeMatrix(parentMatrix);
    multiplyMatrix(parentMatrix, localMatrix);

    //calculate each subjoint of this joint
    for (Balljoint subjoint : subjoints) {
        subjoint.calculate_joints_lcs(parentMatrix, joint_id);
    }

    //restore current parent matrix of this joint
    initializeMatrix(parentMatrix);
    multiplyMatrix(parentMatrix, current_parent);
}

//////////////////////
//  public methods  //
//////////////////////

//Balljoint default constructor
Balljoint::Balljoint() {
    //initialize box position, dimensions, and pose
    offset[0] = 0.0f;
    offset[1] = 0.0f;
    offset[2] = 0.0f;
    boxmin[0] = -0.1f;
    boxmin[1] = -0.1f;
    boxmin[2] = -0.1f;
    boxmax[0] = 0.1f;
    boxmax[1] = 0.1f;
    boxmax[2] = 0.1f;
    pose[0] = 0.0f;
    pose[1] = 0.0f;
    pose[2] = 0.0f;

    //initialize box rotation limits
    rotxlimit[0] = -INFINITY;
    rotxlimit[1] = INFINITY;
    rotylimit[0] = -INFINITY;
    rotylimit[1] = INFINITY;
    rotzlimit[0] = -INFINITY;
    rotzlimit[1] = INFINITY;
}

//calculate and return joints_lcs mapping
map<int, vector<float>> Balljoint::getJointsLcs() {
    //indicate the base's initial transform as identity and id/size as zero
    double identityMatrix[4 * 4];
    initializeMatrix(identityMatrix);
    int base_id = 0;
    numJoints = 0;

    //recursively calculate joints_lcs
    calculate_joints_lcs(identityMatrix, base_id);
    return(joints_lcs);
}

//parse balljoint from skelFile
void Balljoint::getBalljoint(ifstream &skelFile) {
    //indicate the base's id as zero
    int base_id = 0;

    //recursively parse the balljoint from the skelFile
    parseBalljoint(skelFile, base_id);
}

//return matching joint id string based on numerical id
void Balljoint::getName(string &id, int joint_id) {
    if (this->joint_id == joint_id) {
        id = this->id;
    }
    else {
        for (Balljoint &subjoint : subjoints) {
            subjoint.getName(id, joint_id);
        }
    }
}

//get pose of given joint
void Balljoint::getPose(double *pose, int joint_id) {
    if (this->joint_id == joint_id) {
        for (int i = 0; i < 3; i++) {
            pose[i] = this->pose[i];
        }
    }
    else {
        for (Balljoint &subjoint : subjoints) {
            subjoint.getPose(pose, joint_id);
        }
    }
}

//set pose of given joint
void Balljoint::setPose(double *pose, int joint_id) {
    if (this->joint_id == joint_id) {
        for (int i = 0; i < 3; i++) {
            this->pose[i] = pose[i];
        }
    }
    else {
        for (Balljoint &subjoint : subjoints) {
            subjoint.setPose(pose, joint_id);
        }
    }
}

//set offset of the root
void Balljoint::setRootOffset(double *offset) {
    for (int i = 0; i < 3; i++) {
        this->offset[i] = offset[i];
    }
}

//draw the Balljoint
void Balljoint::draw(double *parentMatrix) {
    //draw a box for this Balljoint
    glPushMatrix();

    //generate local matrix
    generateLocalMatrix();

    //calculate and offset by local matrix in terms of wcs
    multiplyMatrix(localMatrix, parentMatrix);
    glMultMatrixd(localMatrix);

    //save parent matrix for this joint
    double current_parent[4 * 4];
    initializeMatrix(current_parent);
    multiplyMatrix(current_parent, parentMatrix);

    //back and front x-face
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(boxmin[0], boxmin[1], boxmin[2]);
    glVertex3f(boxmin[0], boxmin[1], boxmax[2]);
    glVertex3f(boxmin[0], boxmax[1], boxmax[2]);
    glVertex3f(boxmin[0], boxmax[1], boxmin[2]);
    glEnd();
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(boxmax[0], boxmax[1], boxmax[2]);
    glVertex3f(boxmax[0], boxmin[1], boxmax[2]);
    glVertex3f(boxmax[0], boxmin[1], boxmin[2]);
    glVertex3f(boxmax[0], boxmax[1], boxmin[2]);
    glEnd();

    //back and front y-face
    glBegin(GL_QUADS);
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(boxmin[0], boxmin[1], boxmin[2]);
    glVertex3f(boxmax[0], boxmin[1], boxmin[2]);
    glVertex3f(boxmax[0], boxmin[1], boxmax[2]);
    glVertex3f(boxmin[0], boxmin[1], boxmax[2]);
    glEnd();
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(boxmax[0], boxmax[1], boxmax[2]);
    glVertex3f(boxmax[0], boxmax[1], boxmin[2]);
    glVertex3f(boxmin[0], boxmax[1], boxmin[2]);
    glVertex3f(boxmin[0], boxmax[1], boxmax[2]);
    glEnd();

    //back and front z-face
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(boxmin[0], boxmin[1], boxmin[2]);
    glVertex3f(boxmin[0], boxmax[1], boxmin[2]);
    glVertex3f(boxmax[0], boxmax[1], boxmin[2]);
    glVertex3f(boxmax[0], boxmin[1], boxmin[2]);
    glEnd();
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(boxmax[0], boxmax[1], boxmax[2]);
    glVertex3f(boxmin[0], boxmax[1], boxmax[2]);
    glVertex3f(boxmin[0], boxmin[1], boxmax[2]);
    glVertex3f(boxmax[0], boxmin[1], boxmax[2]);
    glEnd();

    glPopMatrix();

    //update parent matrix with local for subjoints of this joint
    initializeMatrix(parentMatrix);
    multiplyMatrix(parentMatrix, localMatrix);

    //draw each subjoint of this joint
    for (Balljoint subjoint : subjoints) {
        subjoint.draw(parentMatrix);
    }
    
    //restore current parent matrix of this joint
    initializeMatrix(parentMatrix);
    multiplyMatrix(parentMatrix, current_parent);
}

//print the Balljoint
void Balljoint::print(int &depth) {
    //print id of this joint
    cout << id << endl;

    //maintain current depth value
    int tmp = depth;

    //increase depth value for subnodes
    depth++;
    for (Balljoint subjoint : subjoints) {
        for (int i = 0; i < depth; i++) {
            cout << "-";
        }
        subjoint.print(depth);
    }

    //re-initialize depth for this joint
    depth = tmp;
}

//check character by character if a string is alphabetic
bool isAlphabetic(string token) {
    for (int i = 0; i < token.length(); i++) {
        if (!isalpha(token[i])) {
            return(false);
        }
    }
    return(true);
}

