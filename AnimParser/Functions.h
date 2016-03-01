// Author: Daren Cheng
// Class:  CS 4392
// Date:   9/20/2015

// Desc: 
// Header file that contrains method's used in Cube.cpp

//include guard
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

//include dependancies
#include <AntTweakBar.h>
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <vector>

using namespace std;

//functions that help with basic arithmetic
double floorMod(double dividend, double divisor);

//functions that manipulate matrices
void multiplyMatrix(double *firstMatrix, double *homogeneousMatrix);
void multiplyVector(double *vector, double *homogeneousMatrix);
void initializeMatrix(double *matrix);
void printMatrix(double *matrix);
void inverse(double *matrix);

//functions that manipulate matrices stored in a vector
void multiplyMatrix(vector<float> &firstMatrix, vector<float> &homogeneousMatrix);
void multiplyVector(vector<double> &vec, vector<double> &homogeneousMatrix);
void multiplyVector(vector<float> &vec, vector<float> &homogeneousMatrix);
void multiplyScalar(vector<float> &vec, float &scalar);
void initializeMatrix(vector<float> &matrix);
void printMatrix(vector<float> &matrix);
void inverse(vector<float> &matrix);

//functions that help set up the rendering environment
void drawWCSAxis();
void drawOCSAxis();
void drawCube(int subdivisions);
#endif