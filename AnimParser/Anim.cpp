// Author: Daren Cheng
// Class:  CS 4392
// Date:   11/15/2015

// Desc: 
// Implements methods given in "Anim.h"

//include interface that this file implements
#include "Anim.h"
#include "Functions.h"

//static variable declarations
vector<double> Anim::Channel::Keyframe::hermite_matrix = {
    2, -3, 0, 1,
    -2, 3, 0, 0,
    1, -2, 1, 0,
    1, -1, 0, 0 };

///////////////////////////////////
//  Keyframe :: private methods  //
///////////////////////////////////

//compute tangent based on rule
double Anim::Channel::Keyframe::computeTangent(string tangent_rule, Channel *parent_channel) {
    bool firstKeyframe = false;
    bool lastKeyframe = false;

    //check if this is the first or last keyframe;
    if (id == parent_channel->keyframes.front().id) {
        firstKeyframe = true;
    }
    if (id == parent_channel->keyframes.back().id) {
        lastKeyframe = true;
    }

    //if only one keyframe exists for this channel, use flat as the tangent rule
    if (firstKeyframe && lastKeyframe) {
        tangent_rule = "flat";
    }
    //if this is the first or last keyframe and smoothing is the tangent rule, use linear
    else if ((firstKeyframe || lastKeyframe) && tangent_rule == "smooth") {
        tangent_rule = "linear";
    }

    //compute tangent based on the tangent rule
    if (tangent_rule == "flat") {
        //flat indicates zero velocity, return zero
        return(0);
    }
    else if (tangent_rule == "linear") {
        if (!lastKeyframe) {
            //if this is not the last keyframe, use next keyframe to generate tangent
            double next_value = parent_channel->keyframes[id + 1].value;
            double next_time = parent_channel->keyframes[id + 1].time;

            //compute tangent correction due to scaling time to [0, 1]
            double scaling_correction = next_time - time;
            return(scaling_correction * (next_value - value) / (next_time - time));
        }
        else {
            //else use previous keyframe to generate tangent
            double previous_value = parent_channel->keyframes[id - 1].value;
            double previous_time = parent_channel->keyframes[id - 1].time;

            //compute tangent correction due to scaling time to [0, 1]
            double scaling_correction = time - previous_time;
            return(scaling_correction * (value - previous_value) / (time - previous_time));
        }
    }
    else if (tangent_rule == "smooth") {
        //generate tangent from next and previous keyframes
        double next_value = parent_channel->keyframes[id + 1].value;
        double next_time = parent_channel->keyframes[id + 1].time;
        double previous_value = parent_channel->keyframes[id - 1].value;
        double previous_time = parent_channel->keyframes[id - 1].time;

        //compute tangent correction due to scaling time to [0, 1]
        double scaling_correction = next_time - time;
        return(scaling_correction * (next_value - previous_value) / (next_time - previous_time));
    }
    else {
        //error, invalid tangent rule
        cout << "Error, invalid tangent rule parsed in keyframe: " << id << endl;
        return(0);
    }
}

//////////////////////////////////
//  Keyframe :: public methods  //
//////////////////////////////////

//compute tangents based on tangent rules
void Anim::Channel::Keyframe::computeTangents(Channel *parent_channel) {
    //compute tangents, note that for c1 continuity tangent_in == tangent_out
    tangent_in = computeTangent(tangent_rules.first, parent_channel);
    tangent_out = computeTangent(tangent_rules.second, parent_channel);
}

//compute cubic cofficients based on this keyframe, next keyframe, and tangents
void Anim::Channel::Keyframe::computeCubicCoefficients(Channel *parent_channel) {
    bool firstKeyframe = false;
    bool lastKeyframe = false;

    //check if this is the first or last keyframe;
    if (id == parent_channel->keyframes.front().id) {
        firstKeyframe = true;
    }
    if (id == parent_channel->keyframes.back().id) {
        lastKeyframe = true;
    }

    //load control points into cubic coefficient vector, then multiply by hermite matrix
    if (firstKeyframe && lastKeyframe) {
        //if only one keyframe exists, create a flat curve
        //note that tangent_in == tangent_out == 0
        cubic_coefficients.push_back(value);
        cubic_coefficients.push_back(value);
        cubic_coefficients.push_back(tangent_out);
        cubic_coefficients.push_back(tangent_in);
    }
    else if (!lastKeyframe) {
        //if this is not the last keyframe, use next keyframe to generate curve
        double next_value = parent_channel->keyframes[id + 1].value;
        double next_tangent_in = parent_channel->keyframes[id + 1].tangent_in;
        cubic_coefficients.push_back(value);
        cubic_coefficients.push_back(next_value);
        cubic_coefficients.push_back(tangent_out);
        cubic_coefficients.push_back(next_tangent_in);
    }
    else {
        //else use previous keyframe to generate curve
        double previous_value = parent_channel->keyframes[id - 1].value;
        double previous_tangent_out = parent_channel->keyframes[id - 1].tangent_out;
        cubic_coefficients.push_back(previous_value);
        cubic_coefficients.push_back(value);
        cubic_coefficients.push_back(previous_tangent_out);
        cubic_coefficients.push_back(tangent_in);
    }

    //compute cubic coefficents by multiplying control points with the hermite matrix
    multiplyVector(cubic_coefficients, hermite_matrix);
}

//////////////////////////////////
//  Channel :: private methods  //
//////////////////////////////////

//compute cubic at given time in span
double Anim::Channel::evaluateCubic(double time) {
    //check if time is at a discrete keyframe interval
    for (Keyframe keyframe : keyframes) {
        if (time == keyframe.time) {
            return(keyframe.value);
        }
    }

    //keyframe is not at a discrete time interval, find span and interpolate
    for (int i = 0; i < keyframes.size(); i++) {
        //span is defined by the first keyframe whose time is larger than the current 
        //given time, use cubic coefficients of the keyframe previous to this keyframe
        if (time < keyframes[i].time) {
            //scale time to [0, 1] since our hermite curve assumes so
            time = (time - keyframes[i - 1].time) / (keyframes[i].time - keyframes[i - 1].time);

            //multiply each power of time with the appropriate coefficient
            double t_3 = keyframes[i - 1].cubic_coefficients[0] * time * time * time;
            double t_2 = keyframes[i - 1].cubic_coefficients[1] * time * time;
            double t_1 = keyframes[i - 1].cubic_coefficients[2] * time;
            double t_0 = keyframes[i - 1].cubic_coefficients[3] * 1;

            //return value of hermite curve at given time
            return(t_3 + t_2 + t_1 + t_0);
        }
    }
}

//extrapolate based on extrapolation rules
double Anim::Channel::extrapolate(string extrapolation, double time) {
    //note: it would be better to use polymorphism and create multiple channel classes for
    //each extrapolation, however due to the scope of this project we utilize if/else chains

    //extrapolate based on extrapolation rule
    if (extrapolation == "constant") {
        //figure out if we are extrapolating from the left or right
        if (time < min_keyframe_time) {
            //extrapolation is from the left, return min keyframe value
            return(keyframes.front().value);
        }
        else if (time > max_keyframe_time) {
            //extrapolation is from the right, return max keyframe value
            return(keyframes.front().value);
        }
        else {
            //error, extrapolating while inside span of channel
            cout << "Error, extrapolating while inside span of channel." << endl;
            return(0);
        }
    }
    else if (extrapolation == "cycle") {
        //extrapolation is cyclic, mod time into span and return cubic evaluation
        time = floorMod(time - min_keyframe_time, max_keyframe_time - min_keyframe_time) 
            + min_keyframe_time;

        //return cubic evaluation
        return(evaluateCubic(time));
    }
    else if (extrapolation == "cycle_offset") {
        //get number of cycles completed and value of a single cycke
        double cycles_completed = floor(time / (max_keyframe_time - min_keyframe_time));
        double cycle_value = keyframes.back().value - keyframes.front().value;

        //mod time into span and return cubic evaluation
        time = floorMod(time - min_keyframe_time, max_keyframe_time - min_keyframe_time) 
            + min_keyframe_time;

        //return number of completed cycles times value of a single cycle, plus remainder
        return(cycles_completed * cycle_value + evaluateCubic(time));
    }
    else {
        //error, invalid channel extrapolation
        cout << "Error, invalid extrapolation rule parsed." << endl;
        return(0);
    }
}

/////////////////////////////////
//  Channel :: public methods  //
/////////////////////////////////

//evalute the dof for this channel based on the given time
double Anim::Channel::evaluate(double time) {
    //check if extrapolation is required
    if (time < min_keyframe_time) {
        //outside of span from left, extrapolate value with first extrapolation
        return(extrapolate(extrapolations.first, time));
    }
    else if (time > max_keyframe_time) {
        //outside of span from right, extrapolate value with second extrapolation
        return(extrapolate(extrapolations.second, time));
    }
    else {
        //time is inside span of channel, evaluate cubic directly
        return(evaluateCubic(time));
    }
}

//precompute info such extrapolation limits and keyframe cubic coefficients
void Anim::Channel::precompute() {
    //set lower and upper extrapolation limits
    min_keyframe_time = keyframes.front().time;
    max_keyframe_time = keyframes.back().time;

    //compute tangents of each keyframe
    //note that we pass a reference of this channel itself
    //to allow for the keyframe to compute its own tangents
    for (Keyframe &keyframe : keyframes) {
        keyframe.computeTangents(this);
    }

    //compute cubic coefficients of each keyframe
    //note that this computation requires tangents be defined
    //for all keyframes, and thus must be done in a seperate loop
    for (Keyframe &keyframe : keyframes) {
        keyframe.computeCubicCoefficients(this);
    }

}

///////////////////////////////
//  Anim :: private methods  //
///////////////////////////////

//get the current channel of the animation
void Anim::getChannel(string token, ifstream &animFile, vector<Channel> &joint) {
    Channel joint_channel;
    int numKeyFrames;

    //ignore syntatic left curly brace and channel marker
    animFile >> token;
    animFile >> token;

    //ignore extrapolation marker and get extrapolation
    animFile >> token;
    animFile >> token;
    joint_channel.extrapolations.first = token;
    animFile >> token;
    joint_channel.extrapolations.second = token;

    //ignore syntatical keyframe marker and get keyframe count
    animFile >> token;
    animFile >> token;
    numKeyFrames = stoi(token);

    //ignore syntatic left curly brace and get every keyframe
    animFile >> token;
    for (int j = 0; j < numKeyFrames; j++) {
        Channel::Keyframe keyframe;
        keyframe.id = j;
        animFile >> token;
        keyframe.time = stod(token);
        animFile >> token;
        keyframe.value = stod(token);
        animFile >> token;
        keyframe.tangent_rules.first = token;
        animFile >> token;
        keyframe.tangent_rules.second = token;

        //add this keyframe to the channel
        joint_channel.keyframes.push_back(keyframe);
    }

    //precompute information for this joint's channel as needed
    joint_channel.precompute();

    //ignore syntatic right curly braces and add channel to joint
    animFile >> token;
    animFile >> token;
    joint.push_back(joint_channel);
}

//get the root translations described in the animation
void Anim::getRootTranslation(string token, ifstream &animFile) {
    //get xyz root translations
    getChannel(token, animFile, root_translation);
    getChannel(token, animFile, root_translation);
    getChannel(token, animFile, root_translation);
}

//get the joint rotations described in the animation
void Anim::getJointRotations(string token, ifstream &animFile) {
    vector<Channel> joint_rotation;

    //get xyz rotation joint
    getChannel(token, animFile, joint_rotation);
    getChannel(token, animFile, joint_rotation);
    getChannel(token, animFile, joint_rotation);

    //add this joint rotation to the list of joint roations
    joint_rotations.push_back(joint_rotation);
}

//////////////////////////////
//  Anim :: public methods  //
//////////////////////////////

//Skin default constructor
Anim::Anim() {
    //initialize stuff
}

//get the animation from the animFile by getting each of the anim's attributes
void Anim::getAnim(ifstream &animFile) {
    string token;
    int numJointRotations;

    //ignore syntatic animation marker and left curly brace
    animFile >> token;
    animFile >> token;

    //ignore syntatic range marker and get range for the animation
    animFile >> token;
    animFile >> token;
    range.first = stof(token);
    animFile >> token;
    range.second = stof(token);

    //ignore syntatic numchannels marker and get numJointRotations from numchannels value
    animFile >> token;
    animFile >> token;
    numJointRotations = (stoi(token) - 3) / 3;

    //get xyz position channels for the root joint
    getRootTranslation(token, animFile);

    //ger xyz rotation channels for every joint
    for (int i = 0; i < numJointRotations; i++) {
        getJointRotations(token, animFile);
    }
}

//evalute the translation pose at the given time for the root
void Anim::evaluateRoot(double *pose, double time) {
    //evaluate translation pose for each dof
    pose[0] = root_translation[0].evaluate(time);
    pose[1] = root_translation[1].evaluate(time);
    pose[2] = root_translation[2].evaluate(time);
}

//evalute the pose at the given time for a joint
void Anim::evaluateJoint(double *pose, double time, int joint_id) {
    //evaluate pose for each dof
    pose[0] = joint_rotations[joint_id][0].evaluate(time);
    pose[1] = joint_rotations[joint_id][1].evaluate(time);
    pose[2] = joint_rotations[joint_id][2].evaluate(time);
}

//print animation for testing
void Anim::print() {
    //print range of the animation
    cout << "range: " << range.first << ", " << range.second << endl << endl;

    //print out root translations
    for (Channel channel : root_translation) {
        //print out this channel's extrapolation
        cout << "extrapolation: " << channel.extrapolations.first
            << ", " << channel.extrapolations.second << endl;

        //print out this channel's keyframes
        for (Channel::Keyframe keyframe : channel.keyframes) {
            /* cout << "time: " << keyframe.time << "; value: " << keyframe.value
            << "; tangent rules: " << keyframe.tangent_rules.first
            << ", " << keyframe.tangent_rules.second << endl; */

            //print cubic coefficients
            for (int i = 0; i < 4; i++) {
                cout << keyframe.cubic_coefficients[i] << " ";
            }
            cout << endl;
        }

        //spacing
        cout << endl;
    }

    //print out every other channel
    for (vector<Channel> joint_rotation : joint_rotations) {
        for (Channel channel : joint_rotation) {
            //print out this channel's extrapolation
            cout << "extrapolation: " << channel.extrapolations.first
                << ", " << channel.extrapolations.second << endl;

            //print out this channel's keyframes
            for (Channel::Keyframe keyframe : channel.keyframes) {
                /* cout << "time: " << keyframe.time << "; value: " << keyframe.value
                    << "; tangent rules: " << keyframe.tangent_rules.first
                    << ", " << keyframe.tangent_rules.second << endl; */ 

                //print cubic coefficients
                for (int i = 0; i < 4; i++) {
                    cout << keyframe.cubic_coefficients[i] << " ";
                }
                cout << endl;
            }

            //spacing
            cout << endl;
        }
    }
}

//returns min range
double Anim::getMinRange() { return(range.first); }

//returns max range
double Anim::getMaxRange() { return(range.second); }
