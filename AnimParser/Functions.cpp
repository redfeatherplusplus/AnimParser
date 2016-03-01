// Author: Daren Cheng
// Class:  CS 4392
// Date:   10/18/2015

// Desc: 
// Implements methods given in "Functions.h"

//include interface that this file implements
#include "Functions.h"

/////////////////////////////////////////////////
//  functions that help with basic arithmetic  //
/////////////////////////////////////////////////

//modulo based on floored division
double floorMod(double dividend, double divisor) {
    return(dividend - divisor * floor(dividend / divisor));
}

//////////////////////////////////////////
//  functions that manipulate matrices  //
//////////////////////////////////////////

//multiply two matrices and store the result in the first matrix
void multiplyMatrix(double *firstMatrix, double *homogeneousMatrix) {
    double product[4 * 4] = { 0 };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //multiply column by row for the ij'th position
            for (int ij = 0; ij < 4; ij++) {
                product[4 * i + j] += firstMatrix[4 * i + ij] * homogeneousMatrix[4 * ij + j];
            }
        }
    }

    //copy product matrix over
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            firstMatrix[4 * i + j] = product[4 * i + j];
        }
    }
}

//multiply a vector by a matrix and store the result in the vector
void multiplyVector(double *vector, double *homogeneousMatrix) {
    double product[4] = { 0 };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //multiply the row by col for each element in vector 
            product[i] += vector[j] * homogeneousMatrix[4 * j + i];
        }
    }

    //copy product vector over
    for (int i = 0; i < 4; i++) {
        vector[i] = product[i];
    }
}

//initialize a 4x4 matrix to the identity matrix
void initializeMatrix(double *matrix) {
    for (int i = 0; i < 16; i++) {
        matrix[i] = 0.0;
    }
    matrix[4 * 0 + 0] = 1.0;
    matrix[4 * 1 + 1] = 1.0;
    matrix[4 * 2 + 2] = 1.0;
    matrix[4 * 3 + 3] = 1.0;
}

//prints a matrix
void printMatrix(double *matrix) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f, ", matrix[4 * i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

//replaces a matrix with its inverse
void inverse(double *matrix) {
    //invert translations
    matrix[4 * 3 + 0] = -matrix[4 * 3 + 0];
    matrix[4 * 3 + 1] = -matrix[4 * 3 + 1];
    matrix[4 * 3 + 2] = -matrix[4 * 3 + 2];

    //temp matrix for upper triangle of rotation matrix
    double upper_triangle[3] = { 0 };

    //invert rotations, note that for a rotation matrix R^-1 = R^T
    upper_triangle[0] = matrix[4 * 0 + 1];
    upper_triangle[1] = matrix[4 * 0 + 2];
    upper_triangle[1] = matrix[4 * 1 + 2];

    matrix[4 * 0 + 1] = matrix[4 * 1 + 0];
    matrix[4 * 0 + 2] = matrix[4 * 2 + 0];
    matrix[4 * 1 + 2] = matrix[4 * 2 + 1];

    matrix[4 * 1 + 0] = upper_triangle[0];
    matrix[4 * 2 + 0] = upper_triangle[1];
    matrix[4 * 2 + 1] = upper_triangle[2];
}

////////////////////////////////////////////////////////////
//  functions that manipulate matrices stored in vectors  //
////////////////////////////////////////////////////////////

//multiply two matrices and store the result in the first matrix
void multiplyMatrix(vector<float> &firstMatrix, vector<float> &homogeneousMatrix) {
    float product[4 * 4] = { 0 };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //multiply column by row for the ij'th position
            for (int ij = 0; ij < 4; ij++) {
                product[4 * i + j] += firstMatrix[4 * i + ij] * homogeneousMatrix[4 * ij + j];
            }
        }
    }

    //copy product matrix over
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            firstMatrix[4 * i + j] = product[4 * i + j];
        }
    }
}

//multiply a vector by a matrix and store the result in the vector (double)
void multiplyVector(vector<double> &vec, vector<double> &homogeneousMatrix) {
    double product[4] = { 0 };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //multiply the row by col for each element in vector 
            product[i] += vec[j] * homogeneousMatrix[4 * j + i];
        }
    }

    //copy product vector over
    for (int i = 0; i < 4; i++) {
        vec[i] = product[i];
    }
}

//multiply a vector by a matrix and store the result in the vector (float)
void multiplyVector(vector<float> &vec, vector<float> &homogeneousMatrix) {
    float product[4] = { 0 };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //multiply the row by col for each element in vector 
            product[i] += vec[j] * homogeneousMatrix[4 * j + i];
        }
    }

    //copy product vector over
    for (int i = 0; i < 4; i++) {
        vec[i] = product[i];
    }
}

//multiply a vector by a scalar
void multiplyScalar(vector<float> &vec, float &scalar) {
    for (float &element : vec) {
        element *= scalar;
    }
}

//initialize a 4x4 matrix to the identity matrix
void initializeMatrix(vector<float> &matrix) {
    for (int i = 0; i < 16; i++) {
        matrix[i] = 0.0;
    }
    matrix[4 * 0 + 0] = 1.0;
    matrix[4 * 1 + 1] = 1.0;
    matrix[4 * 2 + 2] = 1.0;
    matrix[4 * 3 + 3] = 1.0;
}

//prints a matrix
void printMatrix(vector<float> &matrix) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f, ", matrix[4 * i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

//replaces a matrix with its inverse
void inverse(vector<float> &matrix) {
    //invert translations
    matrix[4 * 3 + 0] = -matrix[4 * 3 + 0];
    matrix[4 * 3 + 1] = -matrix[4 * 3 + 1];
    matrix[4 * 3 + 2] = -matrix[4 * 3 + 2];

    //temp matrix for upper triangle of rotation matrix
    float upper_triangle[3] = { 0 };

    //invert rotations, note that for a rotation matrix R^-1 = R^T
    upper_triangle[0] = matrix[4 * 0 + 1];
    upper_triangle[1] = matrix[4 * 0 + 2];
    upper_triangle[2] = matrix[4 * 1 + 2];

    matrix[4 * 0 + 1] = matrix[4 * 1 + 0];
    matrix[4 * 0 + 2] = matrix[4 * 2 + 0];
    matrix[4 * 1 + 2] = matrix[4 * 2 + 1];

    matrix[4 * 1 + 0] = upper_triangle[0];
    matrix[4 * 2 + 0] = upper_triangle[1];
    matrix[4 * 2 + 1] = upper_triangle[2];
}

////////////////////////////////////////////////////////////
//  functions that help set up the rendering environment  //
////////////////////////////////////////////////////////////

//draw WCS axis
void drawWCSAxis() {
    //save current rendering settings
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    //set line parameters
    glDisable(GL_LIGHTING);
    glLineWidth(2.5);

    //draw x-axis
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(10.0f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();

    //draw y-axis
    glPushMatrix();
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 10.0f, 0.0f);
    glEnd();
    glPopMatrix();

    //draw z-axis
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 10.0f);
    glEnd();
    glPopMatrix();

    //draw zx-plane
    glLineWidth(0.5);
    glPushMatrix();
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    for (int i = 0; i < 10; i++) {
        //z vector lines
        glBegin(GL_LINES);
        glVertex3f(i*1.0f, 0.0f, -10.0f);
        glVertex3f(i*1.0f, 0.0f, 10.0f);
        glVertex3f(-i*1.0f, 0.0f, -10.0f);
        glVertex3f(-i*1.0f, 0.0f, 10.0f);
        glEnd();

        //x vector lines
        glBegin(GL_LINES);
        glVertex3f(-10.0f, 0.0f, 1.0f*i);
        glVertex3f(10.0f, 0.0f, 1.0f*i);
        glVertex3f(-10.0f, 0.0f, -1.0f*i);
        glVertex3f(10.0f, 0.0f, -1.0f*i);
        glEnd();
    }
    glPopMatrix();

    //restore previous rendering settings
    glPopAttrib();
}

//draw OCS axis
void drawOCSAxis() {
    //save current rendering settings
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    //set line parameters
    glDisable(GL_LIGHTING);

    //draw x-axis
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(-1.5f, 0.0f, 0.0f);
    glVertex3f(1.5f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();

    //draw y-axis
    glPushMatrix();
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, -1.5f, 0.0f);
    glVertex3f(0.0f, 1.5f, 0.0f);
    glEnd();
    glPopMatrix();

    //draw z-axis
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, -1.5f);
    glVertex3f(0.0f, 0.0f, 1.5f);
    glEnd();
    glPopMatrix();

    //re-enable lighting
    glEnable(GL_LIGHTING);

    //restore previous rendering settings
    glPopAttrib();
}

//draw cube with subdivisions
void drawCube(int subdivisions) {
    double sub_length = 1.0 / (double)subdivisions;

    //define triangles according to the right-hand rule
    glPushMatrix();
    for (int i = 0; i < subdivisions; i++) {
        for (int j = 0; j < subdivisions; j++) {
            //front and back x-face
            glBegin(GL_TRIANGLES);
            glNormal3f(1.0, 0.0, 0.0);
            glVertex3f(0.5, (i + 0)*sub_length - 0.5, (j + 0)*sub_length - 0.5);
            glVertex3f(0.5, (i + 1)*sub_length - 0.5, (j + 0)*sub_length - 0.5);
            glVertex3f(0.5, (i + 1)*sub_length - 0.5, (j + 1)*sub_length - 0.5);
            glEnd();
            glBegin(GL_TRIANGLES);
            glNormal3f(1.0, 0.0, 0.0);
            glVertex3f(0.5, (i + 1)*sub_length - 0.5, (j + 1)*sub_length - 0.5);
            glVertex3f(0.5, (i + 0)*sub_length - 0.5, (j + 1)*sub_length - 0.5);
            glVertex3f(0.5, (i + 0)*sub_length - 0.5, (j + 0)*sub_length - 0.5);
            glEnd();
            glBegin(GL_TRIANGLES);
            glNormal3f(-1.0, 0.0, 0.0);
            glVertex3f(-0.5, (i + 1)*sub_length - 0.5, (j + 1)*sub_length - 0.5);
            glVertex3f(-0.5, (i + 1)*sub_length - 0.5, (j + 0)*sub_length - 0.5);
            glVertex3f(-0.5, (i + 0)*sub_length - 0.5, (j + 0)*sub_length - 0.5);
            glEnd();
            glBegin(GL_TRIANGLES);
            glNormal3f(-1.0, 0.0, 0.0);
            glVertex3f(-0.5, (i + 0)*sub_length - 0.5, (j + 0)*sub_length - 0.5);
            glVertex3f(-0.5, (i + 0)*sub_length - 0.5, (j + 1)*sub_length - 0.5);
            glVertex3f(-0.5, (i + 1)*sub_length - 0.5, (j + 1)*sub_length - 0.5);
            glEnd(); 

            //front and back y-face
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f((i + 1)*sub_length - 0.5, 0.5, (j + 1)*sub_length - 0.5);
            glVertex3f((i + 1)*sub_length - 0.5, 0.5, (j + 0)*sub_length - 0.5);
            glVertex3f((i + 0)*sub_length - 0.5, 0.5, (j + 0)*sub_length - 0.5);
            glEnd();
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f((i + 0)*sub_length - 0.5, 0.5, (j + 0)*sub_length - 0.5);
            glVertex3f((i + 0)*sub_length - 0.5, 0.5, (j + 1)*sub_length - 0.5);
            glVertex3f((i + 1)*sub_length - 0.5, 0.5, (j + 1)*sub_length - 0.5);
            glEnd();
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0, -1.0, 0.0);
            glVertex3f((i + 0)*sub_length - 0.5, -0.5, (j + 0)*sub_length - 0.5);
            glVertex3f((i + 1)*sub_length - 0.5, -0.5, (j + 0)*sub_length - 0.5);
            glVertex3f((i + 1)*sub_length - 0.5, -0.5, (j + 1)*sub_length - 0.5);
            glEnd();
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0, -1.0, 0.0);
            glVertex3f((i + 1)*sub_length - 0.5, -0.5, (j + 1)*sub_length - 0.5);
            glVertex3f((i + 0)*sub_length - 0.5, -0.5, (j + 1)*sub_length - 0.5);
            glVertex3f((i + 0)*sub_length - 0.5, -0.5, (j + 0)*sub_length - 0.5);
            glEnd();

            //front and back z-face
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3f((i + 0)*sub_length - 0.5, (j + 0)*sub_length - 0.5, 0.5);
            glVertex3f((i + 1)*sub_length - 0.5, (j + 0)*sub_length - 0.5, 0.5);
            glVertex3f((i + 1)*sub_length - 0.5, (j + 1)*sub_length - 0.5, 0.5);
            glEnd();
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3f((i + 1)*sub_length - 0.5, (j + 1)*sub_length - 0.5, 0.5);
            glVertex3f((i + 0)*sub_length - 0.5, (j + 1)*sub_length - 0.5, 0.5);
            glVertex3f((i + 0)*sub_length - 0.5, (j + 0)*sub_length - 0.5, 0.5);
            glEnd();
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0, 0.0, -1.0);
            glVertex3f((i + 1)*sub_length - 0.5, (j + 1)*sub_length - 0.5, -0.5);
            glVertex3f((i + 1)*sub_length - 0.5, (j + 0)*sub_length - 0.5, -0.5);
            glVertex3f((i + 0)*sub_length - 0.5, (j + 0)*sub_length - 0.5, -0.5);
            glEnd();
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0, 0.0, -1.0);
            glVertex3f((i + 0)*sub_length - 0.5, (j + 0)*sub_length - 0.5, -0.5);
            glVertex3f((i + 0)*sub_length - 0.5, (j + 1)*sub_length - 0.5, -0.5);
            glVertex3f((i + 1)*sub_length - 0.5, (j + 1)*sub_length - 0.5, -0.5);
            glEnd();
        }
    }
    glPopMatrix();
}
