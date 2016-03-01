// Author: Daren Cheng
// Class:  CS 4392
// Date:   11/15/2015

// Desc: 
// Header file that contains the Animation class

//include guard
#ifndef ANIM_H_INCLUDED
#define ANIM_H_INCLUDED

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

class Anim
{
private:
    class Channel
    {
    private:
        double evaluateCubic(double time);
        double extrapolate(string extrapolation, double time);
    public:
        class Keyframe
        {
        private:
            double computeTangent(string tangent_rule, Channel *parent_channel);
        public:
            static vector<double> hermite_matrix;  //matrix form of hermite curve

            int id;                              //order-based id of the keyframe
            double time;                         //value of this keyframe
            double value;                        //dof value at this time
            double tangent_in;                   //tangent from the left side
            double tangent_out;                  //tangent from the right side
            pair<string, string> tangent_rules;  //tangent rules for hermite curve
            vector<double> cubic_coefficients;   //cubic coefficients for curve

            void computeTangents(Channel *parent_channel);
            void computeCubicCoefficients(Channel *parent_channel);
        };  //end Keyframe inner class of Channel

        double min_keyframe_time;             //lower extrapolation limit
        double max_keyframe_time;             //upper extrapolation limit
        pair<string, string> extrapolations;  //extrapolation rules
        vector<Keyframe> keyframes;           //keyframes inside this channel

        double evaluate(double time);
        void precompute();
    };  //end Channel inner class of Anim

    pair<double, double> range;               //range of the animation 
    vector<Channel> root_translation;         //dofs relating to root translation
    vector<vector<Channel>> joint_rotations;  //dofs relating to joint rotations

    void getChannel(string token, ifstream &animFile, vector<Channel> &joint);
    void getRootTranslation(string token, ifstream &animFile);
    void getJointRotations(string token, ifstream &animFile);
public:
    Anim();
    void getAnim(ifstream &animFile);
    void evaluateRoot(double *pose, double time);
    void evaluateJoint(double *pose, double time, int joint_id);
    void print();
    double getMinRange();
    double getMaxRange();
};

#endif