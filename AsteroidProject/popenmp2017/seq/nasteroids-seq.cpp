//
//  nasteroids-seq.cpp
//  Created by Hans von Clemm on 11/27/17.
// Group# 11
// Hans von Clemm - 100377951, John Fitzgerald - 100377174, Kenneth Maher - 100377812, Cvetanka Jovanovska - 100326911
//TO RUN in Terminal (mac) g++ -std=c++14 nasteroids-par.cpp  -o seq
//


#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace std;

//Global Variables
float dt = 0.1; //step
int dmin = 2;
int width = 200;
int height = 200;
int ray_width = 4;
double gravC = 6.674 * pow(10, -5);
int m = 1000;
int sdm = 50;
double inf = std::numeric_limits<double>::infinity();

//Create/Design Asteroid Struct
struct Asteroids{
    
    double xpos;
    double ypos;
    double mass;
    
    double xvel;
    double yvel;
    
    vector <double> fxVect;
    vector <double> fyVect;
};

//Create/Design Planet Struct
struct Planets{
    double xpos;
    double ypos;
    double mass;
};

void astForceCalc(int i, int o, Asteroids* astArray){
    double angle;
    double slope;
    
    double dist = sqrt(pow((astArray[i].xpos - astArray[o].xpos), 2) + pow((astArray[i].ypos - astArray[o].ypos), 2));
    
    slope = (astArray[i].ypos - astArray[o].ypos) / (astArray[i].xpos - astArray[o].xpos);
    
    
    if (slope > 1 || slope < -1 || !isinf(trunc(slope))){
        slope = slope - trunc(slope);
    }
    if (isnan(slope)){
        slope = (astArray[i].ypos - astArray[o].ypos) / (astArray[i].xpos - astArray[o].xpos);
    }
    
    angle = atan(slope);
    double fx = ((gravC * astArray[o].mass * astArray[i].mass) / (dist*dist)) * cos(angle);
    double fy = ((gravC * astArray[o].mass * astArray[i].mass) / (dist*dist)) * sin(angle);
    
    if (fx > 200)
        fx = 200;
    if (fy > 200)
        fy = 200;
    
    if (dist < 2)
        fx, fy = 0;
    
    astArray[i].fxVect.push_back(fx);
    astArray[i].fyVect.push_back(fy);
    
    astArray[o].fxVect.push_back(-1*fx);
    astArray[o].fyVect.push_back(-1*fy);
}


void planetForceCalc(int i, int p, Asteroids *astArray, Planets *planetArray){
    
    double angle;
    double slope;
    
    double dist = sqrt(pow((astArray[i].xpos - planetArray[p].xpos), 2) + pow((astArray[i].ypos - planetArray[p].ypos), 2));
    
    slope = (astArray[i].ypos - planetArray[p].ypos) / (astArray[i].xpos - planetArray[p].xpos);
    if (slope > 1 || slope < -1 || !isinf(trunc(slope))){
        slope = slope - trunc(slope);
    }
    if (isnan(slope)){
        slope = (astArray[i].ypos - planetArray[p].ypos) / (astArray[i].xpos - planetArray[p].xpos);
    }
    
    angle = atan(slope);
    double fx = ((gravC * planetArray[p].mass * astArray[i].mass) / (dist*dist)) * cos(angle);
    double fy = ((gravC * planetArray[p].mass * astArray[i].mass) / (dist*dist)) * sin(angle);
    //calculate accel due to x and y forces ---------------------- TO efficiently parallelize this, you want to do all of it at the end of each time cycle in a large loop
    
    if (fx > 200)
        fx = 200;
    if (fy > 200)
        fy = 200;
    
    astArray[i].fxVect.push_back(fx);
    astArray[i].fyVect.push_back(fy);
}

void rebound(int i, Asteroids *astArray){
    if (astArray[i].xpos <= 0){ // flip x velocity and set xpos = 2
        astArray[i].xpos = 2;
        astArray[i].xvel = astArray[i].xvel * -1;
    }
    
    if (astArray[i].xpos >= width){ // flip x velocity and set xpos = width - 2
        astArray[i].xpos = width - 2;
        astArray[i].xvel = astArray[i].xvel * -1;
    }
    
    if (astArray[i].ypos <= 0){ // flip y velocity and set ypos = 2
        astArray[i].ypos = 2;
        astArray[i].yvel = astArray[i].yvel * -1;
    }
    
    if (astArray[i].ypos >= height){
        astArray[i].ypos = height - 2;
        astArray[i].yvel = astArray[i].yvel * -1;
    }
}

void movement(int i, Asteroids *astArray){
    
    double sigfx = 0;
    double sigfy = 0;
    
    for (int v = 0; v < astArray[i].fxVect.size(); v++){
        sigfx += astArray[i].fxVect[v];
    }
    
    for (int v = 0; v < astArray[i].fyVect.size(); v++){
        sigfy += astArray[i].fyVect[v];
    }
    
    
    double accelX = sigfx / astArray[i].mass;
    double accelY = sigfy / astArray[i].mass;
    
    astArray[i].xvel = astArray[i].xvel + (accelX * dt);
    astArray[i].yvel = astArray[i].yvel + (accelY * dt);
    
    astArray[i].xpos = astArray[i].xpos + (astArray[i].xvel * dt);
    astArray[i].ypos = astArray[i].ypos + (astArray[i].yvel * dt);
}

int main(int argc, char *argv[]){
    
    //omp_set_num_threads(16);
    
    //Error arguments for nasteroids-seq
    if (argc != 6){
        cout << "nasteroids-seq: Wrong arguments." << endl;
        cout << "Correct use:" << endl;
        cout << "nasteroids-seq num_asteroids num_iterations num_planets pos_ray seed" << endl;
        exit(-1);
    }
    
    //Take in command line arguments
    int num_asteroids = stoi(argv[1]);
    int num_iterations = stoi(argv[2]);
    int num_planets = stoi(argv[3]);
    double pos_ray = stod(argv[4]);
    int seed = stoi(argv[5]);
    //cout << num_asteroids << " " << num_iterations << " " << num_planets << " " << fixed << setprecision(1) << pos_ray << " " << seed << endl;
    
    //Execution Setup
    cout << "Execution setup" << endl << endl;
    cout << "Number of bodies: " << num_asteroids << endl;
    cout << "Initial file: init_conf.txt" << endl;
    cout << "Output file: out.txt" << endl;
    cout << "Number of planets: " << num_planets << endl << endl;
    cout << "Number of bodies: " << num_asteroids << endl;
    cout << "Gravity: " << gravC << endl;
    cout << "Delta time: " << fixed << setprecision(1) << dt << endl;
    cout << "Number of steps: " << num_iterations << endl;
    cout << "Min. distance: " << dmin << endl;
    cout << "Width: " << width << endl;
    cout << "Height: " << height << endl << endl;
    
    //Create random distribution specifications
    default_random_engine re(seed);
    uniform_real_distribution<double> xdist(0.0, std::nextafter(width, std::numeric_limits<double>::max()));
    uniform_real_distribution<double> ydist(0.0, std::nextafter(height, std::numeric_limits<double>::max()));
    normal_distribution<double> mdist(m, sdm);
    
    //Initialize asteroid and planet struct arrays
    Asteroids * astArray = new Asteroids[num_asteroids];
    Planets * planetArray = new Planets[num_planets];
    
    //Assign random values to all asteroids
    for (int i = 0; i < num_asteroids; i++){
        astArray[i].xpos = xdist(re);
        astArray[i].ypos = ydist(re);
        astArray[i].mass = mdist(re);
        astArray[i].xvel = 0;
        astArray[i].yvel = 0;
    }
    
    //Assign random values for the planets
    for (int i = 0; i < num_planets; i++){
        if (i % 4 == 0){
            //left axis, x = 0
            planetArray[i].ypos = ydist(re);
            planetArray[i].xpos = 0;
        }
        if (i % 4  == 1){
            //top border, y = 0
            planetArray[i].xpos = xdist(re);
            planetArray[i].ypos = 0;
            
        }
        if (i % 4 == 2){
            //right border, x = width
            planetArray[i].ypos = ydist(re);
            planetArray[i].xpos = width;
        }
        if (i % 4 == 3){
            //bottom border, y = height
            planetArray[i].xpos = xdist(re);
            planetArray[i].ypos = height;
        }
        //double planetMass =
        planetArray[i].mass = mdist(re) * 10;
    }
    
    //Create initFile and write initial conditions
    ofstream initFile;
    initFile.open ("init_conf.txt");
    initFile << num_asteroids << " " << num_iterations << " " << num_planets << " " << fixed << setprecision(3) << pos_ray << " " << seed << endl;
    
    //Write asteroid info to initFile  -------can parallelize
    //#pragma omp parallel for
    for (int i = 0; i < num_asteroids; i++){
        //cout << fixed << setprecision(3) << astArray[i].xpos << " " << astArray[i].ypos << " " << astArray[i].mass << endl;
        initFile << fixed << setprecision(3) << astArray[i].xpos << " " << astArray[i].ypos << " " << astArray[i].mass << endl;
    }
    
    //Write planet info to initFile -----------can parallelize
    //#pragma omp parallel for
    for (int i = 0; i < num_planets; i++){
        //cout << fixed << setprecision(3) << planetArray[i].xpos << " " << planetArray[i].ypos << " " << planetArray[i].mass << endl;
        initFile << fixed << setprecision(3) << planetArray[i].xpos << " " << planetArray[i].ypos << " " << planetArray[i].mass << endl;
    }
    
    //Write laser beam loc to initFile
    //cout << "0.000 " << pos_ray << endl;
    initFile << "0.000 " << pos_ray << endl;
    
    //Close initFile
    initFile.close();
    
    //Calculation section
    for (int t = 0; t < num_iterations; t++){
        
        //Clear fx values for past iteration
        for (int c = 0; c < num_asteroids; c++){
            astArray[c].fxVect.clear();
            astArray[c].fyVect.clear();
        }
        
        
        //Parse through asteroids
        
        for (int i = 0; i < num_asteroids; i++){
            
            //Compare asteroids (i) with all subsequent asteroids (o)
            for (int o = i+1; o < num_asteroids; o++){
                astForceCalc(i, o, astArray);
            }
            
            
            //Compare asteroids with all planets
            for (int p = 0; p < num_planets; p++){
                planetForceCalc(i, p, astArray, planetArray);
            }
        }
        
        
        //Calculate movement
        for (int m = 0; m < num_asteroids; m++){
            movement(m, astArray);
        }
        
        for (int s = 0; s < num_asteroids; s++){
            rebound(s, astArray);
        }
        
        //Check if asteroid is in the field of the ray
        for (int r = 0; r < num_asteroids; r++){
            if (astArray[r].ypos > (pos_ray - 2) && astArray[r].ypos < (pos_ray + 2)){
                int newCount = num_asteroids - 1;
                Asteroids *tempArray = astArray;
                //cout << "asteroid at " << r << " exploded" << endl;
                for (int z = r; z < newCount; z++){
                    tempArray[z] = astArray[z+1];
                }
                num_asteroids = newCount;
                astArray = tempArray;
            }
        }
    }
    
    //Write data to outFile
    ofstream outFile;
    outFile.open ("out.txt");
    
    for (int v = 0; v < num_asteroids; v++){
        outFile << fixed << setprecision(3) << astArray[v].xpos << " " << astArray[v].ypos << " " << astArray[v].xvel << " " << astArray[v].yvel << " " << astArray[v].mass << endl;
        //cout << fixed << setprecision(3) << astArray[v].xpos << " " << astArray[v].ypos << " " << astArray[v].xvel << " " << astArray[v].yvel << " " << astArray[v].mass << endl;
    }
    //cout << fixed << setprecision(3) << astArray[num_asteroids-1].xpos << " " << astArray[num_asteroids-1].ypos << " " << astArray[num_asteroids-1].xvel << " " << astArray[num_asteroids-1].yvel << " " << astArray[num_asteroids-1].mass << endl;
    //cout << "NUm asteroids " << num_asteroids << endl;
    outFile.close();
}


