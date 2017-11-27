//
//  openMP.cpp
//
//
//  Created by Hans von Clemm on 11/27/17.
//

#include <iostream>
#include <cmath>
#include "omp.h"

using namespace std;

int main(){
    const long size = 100000;
    double sinTable[size];
    
    long random;
//#pragma omp
    for (int i = 0; i < size; ++i){
        random = i*i*i;
        for (int v = 0; v < 500; v++){
            random = v*i;
        }
    }
    
    
    cout << random << endl;
}

