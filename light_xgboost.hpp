#include<iostream>
#include<stdio.h>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<algorithm>

 


namespace light_xgboost{
struct Feature{
     Feature(int a, float b):idx(a),value(b){}
     int idx;
     float value;
};

class Node{
public:
    int nid;
    int fidx;
    bool (*cond)(float value, float split_weight);
    float value;
    int yes;
    int no;
    int missing;
    bool is_leaf;
    Node():nid(-1),fidx(-1),cond(NULL),value(-1),yes(-1),no(-1),missing(-1),is_leaf(false){
    }
    static bool cond_less(float value, float split_weight){
     return value < split_weight;
    }
    bool parse(const std::string& line){
         if(leaf_line(line)){
           //7:leaf=29.2258
            if(-1 == sscanf(line.c_str(),"%d:leaf=%f", &nid, &value)){
               std::cout << "leaf error" << std::endl;
               return false;
            }
            is_leaf = true;
         }else{
		    //3:[f2<10000] yes=7,no=8,missing=7
            char cond_char = '<';
            if(-1 == sscanf(line.c_str(),"%d:[f%d%c%f] yes=%d,no=%d,missing=%d", &nid, &fidx, &cond_char, &value, &yes, &no, &missing))
            {
               std::cout << "cond error" << std::endl;
               return false;
            }
            if(cond_char == '<'){
                cond = cond_less;
            }else{
                std::cout << "parse failed, cond value:"<< value << std::endl;
                return false;
            }
            is_leaf = false;
        }
       return true;
    }

    bool leaf_line(const std::string& line){
         size_t found = line.find("leaf");
         return found != std::string::npos;
    }
    bool parse_leaf(const std::string& line){
    
    }
    bool cond_line(const std::string& line){
         //TODO: trick
         size_t found = line.find("[");
         return found != std::string::npos;
    }
    void print(){
       std::cout << "nid" << nid << std::endl;
       std::cout << "value" << value<< std::endl;
    }
};





class BoostTree{
    private:
        std::vector<Node> nodes;
        bool predict_impl(const std::map<int, float>& fmap,int i, float& value){
            if(nodes[i].is_leaf){
                value = nodes[i].value;
                return true;
            }else{
                std::map<int, float>::const_iterator it = fmap.find(nodes[i].fidx);
                if(it == fmap.end()){
                     return  predict_impl(fmap, nodes[i].missing, value);
                }else{
                      if((nodes[i].cond)(it->second, nodes[i].value)){
                        return predict_impl(fmap, nodes[i].yes, value);
                      }
                      else return predict_impl(fmap, nodes[i].no, value);
                }
            }
        }
    public:
        bool predict(std::map<int,float>& fmap, float& value){
             return predict_impl(fmap, 0, value);
        }
        void push_node(const Node& node){
            nodes.push_back(node);
        }
        void print(){
             std::cout << "boost tree:" << std::endl;
             for(size_t i = 0; i < nodes.size(); i ++) {
                 std::cout << nodes[i].nid << "|" ;
             }
             std::cout << std::endl;
        }
        static bool comp(const Node& a,const Node& b){
            return a.nid < b.nid;
        } 
        bool nice(){
            sort(nodes.begin(),nodes.end(),BoostTree::comp);
        }
};

class Trees{
     private:
         std::vector<BoostTree> trees;

        bool head_line(const std::string& line){
           size_t found = line.find("booster");
           return found != std::string::npos;
        }
     public:
        void print(){
             for(size_t i = 0; i < trees.size(); i++){
                  trees[i].print();
             }
        }

         bool predict(const std::vector<Feature>& features, float& value){
             //init
             value = 0.0;
             //build map
             std::map<int,float> fmap;
             for(size_t i =  0; i < features.size(); i++){
                  fmap[features[i].idx] = features[i].value;
             }
             //predict
             for(size_t i = 0; i < trees.size(); i ++){
                 float tree_res = 0.0;
                 if(trees[i].predict(fmap, tree_res)){
                    value += tree_res;
                 }else{
                     return false;
                 }
             }
             return true;
        }

    
        std::string& trim(std::string &line)   
        {  
            if (line.empty())   
            {  
                return line;  
            }  
            line.erase(0,line.find_first_not_of("	 "));  
            line.erase(line.find_last_not_of("	 ") + 1);  
            return line; 
        }
        bool load_nice_file(const std::string& file_name){
                std::ifstream in_file(file_name.c_str());
                std::string line;
                while(getline(in_file, line))
                {
                     trim(line);
                     if(head_line(line)){
                         if(trees.size() != 0) {
                              trees[trees.size()-1].nice();
                         }
                         trees.push_back(BoostTree());
                     }
                     else{
                          Node nd;
                          nd.parse(line);
                          trees[trees.size()-1].push_node(nd);
                     }
                }
                trees[trees.size() - 1].nice();
        }
};
}
