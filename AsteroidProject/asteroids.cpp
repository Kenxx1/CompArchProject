//
//  asteroids.cpp
//  Created by Hans von Clemm on 10/26/17.


#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace std;

//global variables
float dt = 0.1; //step
int dmin = 2;
int width = 200;
int height = 200;
int ray_width = 4;
double gravC = pow(6.67408, -11);
int m = 1000;
int sdm = 50;

struct Asteroids{
    
    double xpos;
    double ypos;
    double mass;
    
    double xvel;
    double yvel;
    
    double fx;
    double fy;
    
    Asteroids(){};
    
    Asteroids(double xpos, double ypos, double mass){
        xpos = xpos;
        ypos = ypos;
        mass = mass;
        xvel = 0;
        yvel = 0;
    }
    
};

struct Planets{
    double xpos;
    double ypos;
    double mass;
    
    Planets(){};
    
    Planets(double xpos, double ypos, double mass){
        xpos = xpos;
        ypos = ypos;
        mass = mass;
        
    }
    
};

void forceCalc(int temp, int oppose, Asteroids *astArray, Planets *planetArray){
    //calculate attractive force between the two and how much it'll cause it to move --> using formulas
    //distance between
    
    //Grab values
    double tempX = astArray[temp].xpos;
    double tempY = astArray[temp].ypos;
    double opposeX = astArray[oppose].xpos;
    double opposeY = astArray[oppose].ypos;
    double tempM = astArray[temp].mass;
    double opposeM = astArray[oppose].mass;
    
    double dist = sqrt(pow((tempX - opposeX), 2) + pow((tempY - opposeY), 2));
    double angle;
    
    cout << "Dist: " << dist << endl;
    
    if (dist > 2 ){
        double slope = (tempY - opposeY) / (tempX - opposeX);
        cout << "slope: " << slope << endl;
        
        if (slope > 1 || slope < -1){
            slope = slope - trunc(slope);
            cout << "truncated -> new slope: " << slope << endl;
        }
        angle = atan(slope);
        cout << "angle calculated is " << angle << " radians." << endl;
    }
    
    //double fx =
    double fx = ((gravC * tempM * opposeM) / dist) * cos(angle);
    cout << fixed << setprecision(12) << fx << endl;
    
    double fy = ((gravC * tempM * opposeM) / dist) * sin(angle);
    
    astArray[temp].fx = fx;
    astArray[temp].fy = fy;
    
    
}


int main(int argc, char *argv[]){
    
    if (argc != 6){ //error argument for nasteroids-seq
        cout << "nasteroids-seq: Wrong arguments." << endl;
        cout << "Correct use:" << endl;
        cout << "nasteroids-seq num_asteroids num_iterations num_planets pos_ray seed" << endl;
        exit(-1);
    }
    
    else {
        cout << "Inputs Correct." << endl;
    }
    
    
    int num_asteroids = stoi(argv[1]);
    int num_iterations = stoi(argv[2]);
    int num_planets = stoi(argv[3]);
    double pos_ray = stod(argv[4]);
    int seed = stoi(argv[5]);
    cout << num_asteroids << " " << num_iterations << " " << num_planets << " " << fixed << setprecision(1) << pos_ray << " " << seed << endl;
    
    //cout initial conditions inputed by the user
    string initStr = to_string(num_asteroids) + " " + to_string(num_iterations) + " " + to_string(num_planets) + " " + to_string(pos_ray) + " " + to_string(seed);
    //cout << initStr << endl;
    
    ofstream initFile;
    initFile.open ("init_conf.txt");
    initFile << initStr << endl;
    initFile.close();
    
    Asteroids * astArray = new Asteroids[num_asteroids];
    Planets * planetArray = new Planets[num_planets];

    
    //random distributions
    default_random_engine re(seed);
    uniform_real_distribution<double> xdist(0.0, std::nextafter(width, std::numeric_limits<double>::max()));
    uniform_real_distribution<double> ydist(0.0, std::nextafter(height, std::numeric_limits<double>::max()));
    normal_distribution<double> mdist(m, sdm);
    cout << fixed << setprecision(4);
    
    //cout << xdist(re) << " " << ydist(re) << endl;
    //cout << "Seed is " << seed << endl;
    
    
    //assign values for pos, mass etc for all asteroids and planets
    for (int i = 0; i < num_asteroids; i++){
        astArray[i].xpos = xdist(re);
        astArray[i].ypos = ydist(re);
        astArray[i].mass = mdist(re);
    }
    
    for (int i = 0; i < num_planets; i++){
        if (i == 0 || i == 4){
            //left axis, x = 0
            planetArray[i].ypos = ydist(re);
            planetArray[i].xpos = 0;
        }
        if (i == 1 || i == 5){
            //top border, y = 0
            planetArray[i].xpos = xdist(re);
            planetArray[i].ypos = 0;
            
        }
        if (i == 2 || i == 6){
            //right border, x = width
            planetArray[i].ypos = ydist(re);
            planetArray[i].xpos = width;
        }
        if (i == 3 || i == 7){
            //bottom border, y = height
            planetArray[i].xpos = xdist(re);
            planetArray[i].ypos = height;
        }
        //double planetMass =
        planetArray[i].mass = mdist(re) * 10;
    }
    
    
    //print statements for asteroids and planets
    for (int i = 0; i < num_asteroids; i++){
        cout << fixed << setprecision(3) << astArray[i].xpos << " " << astArray[i].ypos << " " << astArray[i].mass << endl;
    }
    
    for (int i = 0; i < num_planets; i++){
        cout << fixed << setprecision(3) << planetArray[i].xpos << " " << planetArray[i].ypos << " " << planetArray[i].mass << endl;
    }
    //print laser beam location
    cout << "0.000 " << pos_ray << endl;
    
    
    /*
    
    for (int i = 0; i < width; i++){
        
        for (int v = 0; v < height; v++){
            
            cout << "O";
        }
        cout << endl;
    }*/
    
    /*
    cout << fixed << setprecision(3);
    
    default_random_engine re(seed);
    uniform_real_distribution<double> horiz(0, width);
    uniform_real_distribution<double> vertic(0, height);
    
    double rando = horiz(re);
    
    cout << "RANDoM NUMBER IS: " << rando << endl;
     */
    
    vector <double> xForces;
    vector <double> yForces;
    
    for (int i = 0; i < num_asteroids; i++){
        cout << endl << "Asteroid at " << i << endl;
        
        
        for (int j = 0; j < num_asteroids; j++){
            
            if (i != j){
                forceCalc(i, j, astArray, planetArray);
                
                cout << fixed << setprecision(12) << astArray[i].fx << endl;
                xForces.push_back(astArray[i].fx);
                yForces.push_back(astArray[i].fy);
            }
        }
        
        for (int k = 0; k < num_planets; k++){
            //calaculate attractive force between asteroid [i] and planet in question --> using formulas
        }
        
        //sum all forces together and divide by the mass
        //for int
        
    }
    
    //rebound effect
    
}






















