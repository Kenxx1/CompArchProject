//
//  openMP.cpp
//  
//
//  Created by Hans von Clemm on 11/27/17.
//TO RUN: gcc-7 -std=c++14 -fopenmp openMP.cpp
//

#include <iostream>
#include <cmath>
#include "omp.h"

using namespace std;

int main(){
    const long size = 16;
    double sinTable[size];
    
    int threadCount = omp_get_num_threads();
    cout << "THREAD COUNT ! " << threadCount << endl;
    
#pragma omp
{
    int id = omp_get_thread_num();
    cout << "Hello(" << id << ") ";
    cout << "World(" << id << ")";
    
    threadCount = omp_get_num_threads();
    cout << "THREAD COUNT ! pos 2 " << threadCount << endl;
}
    
    threadCount = omp_get_num_threads();
    cout << "THREAD COUNT ! pos 3 " << threadCount << endl;
    return 0;

}
