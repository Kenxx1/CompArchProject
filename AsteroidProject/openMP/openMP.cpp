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
    const long size = 16;
    double sinTable[size];
    
    #pragma omp parallel for
    for (int i = 0; i < size; ++i){
        cout << "Hi" << " Lets  " << 1 << endl;
    }

}
