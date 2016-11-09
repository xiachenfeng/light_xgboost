#include<iostream>
#include<stdio.h>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<algorithm>
#include<light_xgboost.hpp>
int main(){
    std::cout << "light xgboost" << std::endl;
    light_xgboost::Trees boost_trees; 
    boost_trees.load_nice_file(std::string("./dump.nice.txt"));
    std::vector<light_xgboost::Feature> fv;
    fv.push_back(light_xgboost::Feature(1,20000000));
    float r = 0.0;
    boost_trees.predict(fv,r);
    std::cout << "r:" << r << std::endl;
}
