/****************************************************
 * oneR.cc
 * Authors: Patrick Durgin, Andrew Rocha, and John Tabone
 * Date: 4/1/14
 * Class: CSCI-399
 * Purpose: Implement OneR machine learning algorithm in C++
 * Input: User-selected .arff files for classifer training
 *        and testing
 * Output: - Rule generated by our OneR algorithm
 *         - Number of correct and incorrect instances classified
 *          by our OneR algorithm
 * Notes: - Works best with two classes. WEKA beat us
 *         when using more than 2 classes
 *        - In your arff, attributes must be declared in the 
 *         following fashion:
 *            @attribute astigmatism {no, yes}
 *        - In your arff, instances must be declared in the
 *         following fashion:
 *            young,myope,no,reduced,none 
 *        - Does NOT handle missing values 
 *        - Does NOT handle numeric values
 ****************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "strtk.hpp" //string toolkit --> External library 
                     //to easily parse comma separated instances


using namespace std;

//class of each attrib value
struct Class {
   string name;
   int count;

   Class(){
      count = 0;
   }
};

//attrib values
struct value {
   string name;
   int count;
   string majorityClass;
   int majorityClassNum;
   float errorRate;
   vector<Class> classes;

   value(){
      count = 0;
      majorityClassNum = 0;
      errorRate = 0;
   }
};

//attrib
struct attribute {
   string name;
   vector<value> values;
   float totalError;

   attribute(){
      totalError = 0;
   }
};

//rule
struct rule {
   string attrib_name;
   vector<value> values;
};

//vector of attribute structs
vector<attribute> attributes;

//holds our rule
rule the_rule;  

//total attribs
int attrib_count = 0;

//total instances
int instance_count = 0;

//total correct
int correct = 0;

//total incorrect
int incorrect = 0;

//prototypes
void readARFF(string);
void getInstances(string);
void getAttribValueCount(vector<string>);
void assignClass();
void classCount(vector<string>);
void getMajorityClass();
void getErrorValues();
void getRule();
void testClassifier(string);

int main (void){

   string arff; //name of arff file
   string test; //name of file to test

   cout << "Please enter name and extension of file to apply OneR algorithm to: " << endl;
   cin >> arff;

   cout <<"What file would you like to classify? " <<endl;
   cin >> test;
   
   readARFF(arff);
   getInstances(arff);
   getRule();
   testClassifier(test);

   cout << "Correctly classified instances: " << correct << endl;
   cout << "Incorrectly classified instances: " << incorrect << endl;
   cout << "Total number of instances: " << correct+incorrect << endl;
  

}

/* readARFF(string)
 *
 * Reads in arff file for classifer training.
 *
 */
void readARFF (string arff){
   
   string line;
   ifstream myFile;
     
   myFile.open(arff.c_str());

   if (myFile.is_open()){
      while (getline (myFile, line)){
	 if (line.find("@attribute") != std::string::npos){
	    //Declare dummy structs
	    attribute default_attrib;
	    value default_value;

	    //find attribute name
	    unsigned first = line.find("e ");
	    unsigned last = line.find ("{");
	    string attrib_name = line.substr(first+2, last-first-2);
	    default_attrib.name = attrib_name;

	    //find attrib values
	    first = line.find("{");
	    last = line.find("}");
	    string attrib_values = line.substr(first+1, last-first-1);
	    vector<string> temp;
	    strtk::parse(attrib_values, ", ", temp);
	    for (int i = 0; i< temp.size(); i++){
	       default_value.name = temp[i];
	       default_attrib.values.push_back(default_value);
	    }
	    attrib_count++;	    
	    attributes.push_back(default_attrib);

	 }	 
      }
   }

   myFile.close();
   	    
}

/* getInstances(string)
 *
 * Reads in arff file to get instance info.
 *
 */
void getInstances(string arff){
   ifstream myFile;
   string line;

   myFile.open(arff.c_str());

   //Create classifier based on training split
   if (myFile.is_open()){
      while (getline (myFile, line)){
	 if (line.find("@data") != std::string::npos){
	    while (getline (myFile, line)){
	       if (line.find("%") != std::string::npos) {
		     getline (myFile, line);
	       }
	       else {
		  vector<string> temp;
		  strtk::parse(line, ",", temp);
		  getAttribValueCount(temp);
		  assignClass();
		  classCount(temp);
		  getMajorityClass();
		  getErrorValues();
		  instance_count++;
	       }//else
   

	    }//while getline(myFile, line)
	 }//line.find("@data")
      }//while getline(myFile, line)
   }//if myFile.open()
   
   myFile.close();
}
     
/* getAttribValueCount(vector<string>)
 *
 * Get count of each attrib value in instances.
 *
 */
void getAttribValueCount(vector<string> instance_values){
   //Get count of each possible attribute value 
   for (int i = 0; i < attrib_count-1; i++){
      for(int j = 0; j < attributes[i].values.size(); j++){
	 if(instance_values[i] == attributes[i].values[j].name)
	    attributes[i].values[j].count += 1;
      }
   }
}

/* assignClass()
 *
 * Assign names from last attrib i.e. class to Class struct
 *
 */
void assignClass(){
   //Assign names from last attrib i.e. class to Class struct
   for (int i = 0; i < attributes[attrib_count-1].values.size(); i++){
      Class default_class;
      for (int j = 0; j < attributes.size()-1; j++){
	 for (int k = 0; k < attributes[j].values.size(); k++){
	    default_class.name = attributes[attrib_count-1].values[i].name;
	    attributes[j].values[k].classes.push_back(default_class);
	 }
      }
   }
}

/* classCount(vector<string>)
 *
 * Get class count for each attribute value
 *
 */
void classCount(vector<string> instance_values){
   
   //Get class count for each attribute value
   for (int i = 0; i < instance_values.size()-1; i++){
      for (int j = 0; j < attributes.size()-1; j++){
	 for (int k = 0; k < attributes[j].values.size(); k++){
	    for (int l = 0; l < attributes[j].values[k].classes.size(); l++){
	       if (instance_values[i] == attributes[j].values[k].name){
		  if (instance_values[attrib_count-1] == attributes[j].values[k].classes[l].name){
		     attributes[j].values[k].classes[l].count+=1;
		  }
	       }//if
	    }//for l
	 }//for k
      }//for j  
   }//for i


}

/* getMajorityClass()
 *
 * Get number value of majority class for every attribute value and
 * get name of majority class for every attribute value
 * 
 */
void getMajorityClass(){

   //Get number value of majority class for every attribute value
   for (int i = 0; i < attributes.size()-1; i++){
      for (int j = 0; j < attributes[i].values.size(); j++){
	 int max = 0;
	 for (int k = 0; k < attributes[i].values[j].classes.size(); k++){
	    if(attributes[i].values[j].classes[k].count > max){
	       max = attributes[i].values[j].classes[k].count;
	       attributes[i].values[j].majorityClassNum = max;
	    }
	 }//k
      }//j
   }//i

   //Get name of majority class for every attribute value
   for (int i = 0; i < attributes.size()-1; i++){
      for (int j = 0; j < attributes[i].values.size(); j++){
	 for (int k = 0; k < attributes[i].values[j].classes.size(); k++){
	    if(attributes[i].values[j].classes[k].count == attributes[i].values[j].majorityClassNum){
	       attributes[i].values[j].majorityClass = attributes[i].values[j].classes[k].name;
	    }
	 }//k
      }//j
   }//i
  
}

/* getErrorValues()
 *
 * Gets error rates for each attribute's rules and total error rate
 * for each attribute.
 *
 */
void getErrorValues() {
   //Gets error rates for each attribute's rules and total error rate
   //for each attribute
   for (int i = 0; i < attributes.size()-1; i++){
      int total_e = 0;
      for(int j = 0; j < attributes[i].values.size(); j++){
	 int count = attributes[i].values[j].count;
	 int maj = attributes[i].values[j].majorityClassNum;
	 attributes[i].values[j].errorRate = (float)(count-maj)/count;
	 total_e+=(count-maj);
      }//j
      attributes[i].totalError = (float)(total_e)/instance_count;
   }//i

}

/* getRule()
 *
 * Calculates total error rate and gets our rule!
 *
 */

void getRule() {
   //Find minimum total error rate for each attribute
   float min = attributes[0].totalError;
   for (int i = 0; i < attributes.size()-1; i++){
      if (attributes[i].totalError < min){
	 min = attributes[i].totalError;
      }
   }
      
   for (int i = 0; i < attributes.size()-1; i++){
      if(min == attributes[i].totalError){

	 //Stores rule
	 value the_value;
	 the_rule.attrib_name = attributes[i].name;
	 for (int j = 0; j < attributes[i].values.size(); j++){
	    the_value.name = attributes[i].values[j].name;
	    the_value.majorityClass = attributes[i].values[j].majorityClass;
	    the_rule.values.push_back(the_value);
	 }

	 //Outputs rule
	 cout << "The rule generated by OneR is as follows: " << endl << endl;
	 cout << attributes[i].name << ":" << endl;
	 for (int k = 0; k < attributes[i].values.size(); k++){
	    cout << "    " <<  attributes[i].values[k].name << " ==> " <<  attributes[i].values[k].majorityClass << endl;   
	 }//j
	 break; //in case of tie
      }//if
   }//i
   
}

/* testClassifier(string)
 *
 * Tests our classifier with the user-inputted test data.
 *
 */
void testClassifier(string test){
   ifstream myFile;
   string line;
   

   myFile.open(test.c_str());
      
   //Test classifier 
   if (myFile.is_open()){
      while (getline (myFile, line)){
	 if (line.find("@data") != std::string::npos){
	    while (getline (myFile, line)){
	       if (line.find("%") != std::string::npos) {
		  getline (myFile, line);
	       }
	       else {
		  vector<string> temp;
		  strtk::parse(line, ",", temp);
		  int total = temp.size()-1;
		  for (int i = 0; i < temp.size(); i++){
		     for(int j = 0; j < the_rule.values.size(); j++){
			if (temp[i] == the_rule.values[j].name){
			   if(temp[total] == the_rule.values[j].majorityClass){
			      correct++; 
			   }
			   else{
			      incorrect++;
			   }
			}
		     }
		  }

	       }//else

	    }//while getline(myFile, line)
	 }//line.find("@data")
      }//while getline(myFile, line)
   }//if myFile.open()
   
   myFile.close();


}
