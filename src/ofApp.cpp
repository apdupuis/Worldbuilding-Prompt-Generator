#include "ofApp.h"
#include "boost/algorithm/string/split.hpp"

vector<string> subjects, subject_nums, connectors_single, connectors_plural, antecedents_single, antecedents_plural;
string output_string;
ofxCsv source;
ofTrueTypeFont tt;
string split_test = "Let's split this by /slashes/";

vector<string> previous_subjects, previous_antecedents;

void parseTags(string strng, vector<string>* output_text, vector<string>* tags) {
	bool is_sub = false;
	bool is_tag = false;
	string tag_text;
	string temp_text;

	for (char& c : strng) {
		if (is_tag) {
			if (c != '>') {
				tag_text += c;
			}
			else {
				is_tag = false;
				tags->push_back(tag_text);
				tag_text.clear();
			}
		}
		else if (is_sub) {
			if (c != ']') {
				temp_text += c;
			}
			else {
				is_sub = false;
				temp_text += ']';
				output_text->push_back(temp_text);
				temp_text.clear();
			}
		}
		else {
			if (c == '<') {
				is_tag = true;
			}
			else if (c == '[') {
				if (temp_text != "") output_text->push_back(temp_text);
				temp_text.clear();
				temp_text += '[';
				is_sub = true;
			}
			else {
				temp_text += c;
			}
		}
	}

	if (temp_text != "") {
		output_text->push_back(temp_text);
	}
}

void clearPreviouslySelectedTexts() {
	previous_subjects.clear();
	previous_antecedents.clear();
}

int getUniqueSubjectIndex(vector<string> elements, vector<string>* previously_selected) {
	int num_attempts = 0;
	int randomIndex = ofRandom(elements.size());
	string element = elements[randomIndex];
	while (std::find(previously_selected->begin(), previously_selected->end(), element) != previously_selected->end()) {
		randomIndex = ofRandom(elements.size());
		element = elements[randomIndex];
		num_attempts++;
		if (num_attempts > 20) {
			previously_selected->clear();
			break;
		}
	}
	previously_selected->push_back(element);
	return randomIndex;
}

string getUniqueElement(vector<string> elements, vector<string>* previously_selected) {
	int num_attempts = 0;
	int randomIndex = ofRandom(elements.size());
	string element = elements[randomIndex];
	while (std::find(previously_selected->begin(), previously_selected->end(), element) != previously_selected->end()) {
		randomIndex = ofRandom(elements.size());
		element = elements[randomIndex];
		num_attempts++;
		if (num_attempts > 20) {
			previously_selected->clear();
			break;
		}
	}
	previously_selected->push_back(element);
	return element;
}

string wrapString(string text, int width) {

	string indentation = "\t";
	int indentation_width = tt.stringWidth(indentation);

	string typeWrapped = "";
	string tempString = "";
	vector <string> words = ofSplitString(text, " ");

	for (int i = 0; i < words.size(); i++) {

		string wrd = words[i];

		// if we aren't on the first word, add a space  
		if (i > 0) {
			tempString += " ";
		}
		tempString += wrd;

		int stringwidth = tt.stringWidth(tempString);

		if (stringwidth >= width) {
			typeWrapped += "\n"+indentation;
			tempString = indentation+wrd;		// make sure we're including the extra word on the next line  
		}
		else if (i > 0) {
			// if we aren't on the first word, add a space  
			typeWrapped += " ";
		}

		typeWrapped += wrd;
	}

	return typeWrapped;

}

string generate_line() {
	float two_part_probability = 0.;
	float rand = ofRandom(1);

	int subjectIndex = getUniqueSubjectIndex(subjects, &previous_subjects);
	string subject_str = subjects[subjectIndex];
	string subject_num_str = subject_nums[subjectIndex];
	string connector_str, antecedent_str;
	if (subject_num_str == "s") {
		int connectorIndex = floor(ofRandom(connectors_single.size()));
		connector_str = connectors_single[connectorIndex];

		antecedent_str = getUniqueElement(antecedents_single, &previous_antecedents);

		if (rand < two_part_probability) {
			antecedent_str += " and is " + getUniqueElement(antecedents_single, &previous_antecedents);;
		}
	}
	else {
		int connectorIndex = floor(ofRandom(connectors_plural.size()));
		connector_str = connectors_plural[connectorIndex];

		antecedent_str = getUniqueElement(antecedents_plural, &previous_antecedents);

		if (rand < two_part_probability) {
			antecedent_str += " and are " + getUniqueElement(antecedents_plural, &previous_antecedents);
		}
	}
	return subject_str + " " + connector_str + " " + antecedent_str;
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofTrueTypeFont::setGlobalDpi(72);

	tt.load("C:/Windows/fonts/bkant.ttf", 20, true, true);
	tt.setLineHeight(20);
	tt.setLetterSpacing(1.005);
	
	source.load("C:/Users/alexa/OneDrive/Documents/Mine/WorldbuildingGames/Subjects.csv");
	for (int i = 1; i < source.size(); i++) {
		ofxCsvRow current_row = source.getRow(i);
		int num_cols = current_row.getNumCols();
		if (num_cols > 1) {
			subjects.push_back(current_row.getString(0));
			subject_nums.push_back(current_row.getString(1));
		}
		if (num_cols >= 3) {
			string str = (current_row.getString(2));
			if (str != "") {
				connectors_single.push_back(str);
			}
		}
		if (num_cols >= 4) {
			string str = (current_row.getString(3));
			if (str != "") {
				connectors_plural.push_back(str);
			}
		}
		if (num_cols >= 5) {
			string str = (current_row.getString(4));
			if (str != "") {
				antecedents_single.push_back(str);
				antecedents_plural.push_back(str);
			}
		}
		if (num_cols >= 6) {
			string str = (current_row.getString(5));
			if (str != "") {
				antecedents_single.push_back(str);
			}
		}
		if (num_cols >= 7) {
			string str = (current_row.getString(6));
			if (str != "") {
				antecedents_plural.push_back(str);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(230);
	ofSetColor(50);
	tt.drawString(output_string, ofGetWidth()/2 - tt.stringWidth(output_string)/2, ofGetHeight() / 2 - tt.stringHeight(output_string)/2);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'g') {
		//clearPreviouslySelectedTexts();
		string out_temp = "";
		for (int i = 0; i < 3; i++) {
			string str = generate_line();
			str = wrapString(str, 400);
			out_temp += str + "\n";
		}
		output_string = out_temp;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
