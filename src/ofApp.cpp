
//
//  Starter file for Project 3 - Skeleton Builder
//
//  This file includes functionality that supports selection and translate/rotation
//  of scene objects using the mouse.
//
//  Modifer keys for rotatation are x, y and z keys (for each axis of rotation)
//
//  (c) Kevin M. Smith  - 24 September 2018
//
//Sair Abbas - CS116

#include "ofApp.h"

//--------------------------------------------------------------
//
void ofApp::setup() {

	ofSetBackgroundColor(ofColor::black);
	ofEnableDepthTest();
	mainCam.setDistance(15);
	mainCam.setNearClip(.1);
	
	sideCam.setPosition(40, 0, 0);
	sideCam.lookAt(glm::vec3(0, 0, 0));
	topCam.setNearClip(.1);
	topCam.setPosition(0, 16, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));
	ofSetSmoothLighting(true);


	// setup one point light
	//
	light1.enable();
	light1.setPosition(5, 5, 0);
	light1.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
	light1.setSpecularColor(ofColor(255.f, 255.f, 255.f));

	theCam = &mainCam;

	//  create a scene consisting of a ground plane with 2x2 blocks
	//  arranged in semi-random positions, scales and rotations
	//
	// ground plane
	//
	scene.push_back(new Plane(glm::vec3(0, -2, 0), glm::vec3(0, 1, 0)));   

}

 
//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	theCam->begin();
	ofNoFill();
	drawAxis();
	ofEnableLighting();

	//  draw the objects in scene
	//
	material.begin();
	ofFill();
	for (int i = 0; i < scene.size(); i++) {
		if (objSelected() && scene[i] == selected[0])
			ofSetColor(ofColor::white);
		else ofSetColor(scene[i]->diffuseColor);
		scene[i]->draw();
	}

	material.end();
	ofDisableLighting();
	theCam->end();
}

// 
// Draw an XYZ axis in RGB at transform
//
void ofApp::drawAxis(glm::mat4 m, float len) {

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(glm::vec3(m*glm::vec4(0, 0, 0, 1)), glm::vec3(m*glm::vec4(len, 0, 0, 1)));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(glm::vec3(m*glm::vec4(0, 0, 0, 1)), glm::vec3(m*glm::vec4(0, len, 0, 1)));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(glm::vec3(m*glm::vec4(0, 0, 0, 1)), glm::vec3(m*glm::vec4(0, 0, len, 1)));
}

// print C++ code for obj tranformation channels. (for debugging);
//
void ofApp::printChannels(SceneObject *obj) {
	cout << "position = glm::vec3(" << obj->position.x << "," << obj->position.y << "," << obj->position.z << ");" << endl;
	cout << "rotation = glm::vec3(" << obj->rotation.x << "," << obj->rotation.y << "," << obj->rotation.z << ");" << endl;
	cout << "scale = glm::vec3(" << obj->scale.x << "," << obj->scale.y << "," << obj->scale.z << ");" << endl;
	cout << "name: " << obj->name << endl;
}

//Create joint
void ofApp::createJoint()
{
	//Declare return point variable
	glm::vec3 pointRtn = glm::vec3(0, 0, 0);
	//Project mouse point onto 3D point normal to the view axis 
	if (mouseToDragPlane(ofGetMouseX(), ofGetMouseY(), pointRtn) == true) {	
		//If object was selected
		if (objSelected()) {
			//Pass values from coordinate conversion
			string name = "joint" + to_string(updateName);
			glm::vec4 inverseRtn = glm::vec4(pointRtn, 1);
			inverseRtn = glm::inverse(selected[0]->getMatrix()) * inverseRtn;
			pointRtn.x = inverseRtn.x;
			pointRtn.y = inverseRtn.y;
			pointRtn.z = inverseRtn.z;
			//Add new joint to lists 
			Joint *temp = new Joint(pointRtn, name);
			scene.push_back(temp);
			joints.push_back(temp);
			selected[0]->addChild(temp);
		}
		//If object was not selected
		else {
			//Add new joint to lists
			string newJoint = "joint" + to_string(updateName);
			Joint *temp = new Joint(pointRtn, newJoint);
			scene.push_back(temp);
			joints.push_back(temp);
		}
		//Update name counter
		updateName++;
	}
}

//Delete joint
void ofApp::deleteJoint()
{
	if (objSelected()) {
		//Delete child of selected parent
		if (selected[0]->parent) {
			selected[0]->parent->deleteChild(selected[0]);
		}
		//Add child to previous parent
		for (int i = 0; i < selected[0]->childList.size(); i++) {
			selected[0]->parent->addChild(selected[0]->childList[i]);
		}
		//Delete joint from scene vector
		for (int i = 0; i < scene.size(); i++) {
			if (scene[i] == selected[0])
				scene.erase(scene.begin() + i);
		}
		//Delete joint from joints vector
		for (int i = 0; i < joints.size(); i++) {
			if (joints.at(i) == selected[0])
				joints.erase(joints.begin() + i);
		}
	}
}

//Delete all joints 
void ofApp::deleteAllJoints()
{
	//Clear vectors
	joints.clear();
	scene.clear();
	//Restore scene
	scene.push_back(new Plane(glm::vec3(0, -2, 0), glm::vec3(0, 1, 0)));
}

//Save to file
void ofApp::saveToFile()
{
	//Open output file
	outputFile.open("C:/Users/sair_/Documents/CS116/openFrameworks/apps/myApps/Project3/bin/data/outputFile.txt");
	//Declare output variable
	string outputString = "";
	//Add values to output string
	for (int i = 0; i < joints.size(); i++)
	{
		//Concatenate output string
		outputString = "create ";
		outputString += "-joint " + joints[i]->name + " ";
		outputString += "-rotate " + to_string(joints[i]->rotation.x) + "," + to_string(joints[i]->rotation.y) + "," + to_string(joints[i]->rotation.z) + " ";
		outputString += "-translate " + to_string(joints[i]->position.x) + "," + to_string(joints[i]->position.y) + "," + to_string(joints[i]->position.z) + " ";
		outputString += "-parent " + joints[i]->parentName() + "\n";
		outputFile << outputString;
	}
	//Close output file
	outputFile.close();
}

//Read from file
void ofApp::readFile()
{
	//Open input file
	inputFile.open("C:/Users/sair_/Documents/CS116/openFrameworks/apps/myApps/Project3/bin/data/outputFile.txt");
	//Declare input variables
	string inputString = "";
	string jointString = "";
	string parentString = "";
	string rotValue = "";
	string posValue = "";
	glm::vec3 tempPos;
	glm::vec3 tempRot;
	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	//Loop through input file
	while (!inputFile.eof())
	{
		//Retrieve joint name
		getline(inputFile, inputString, ' ');
		if (inputString == "-joint") {
			getline(inputFile, jointString, ' ');
		}
		//Retrieve rotation values
		if (inputString == "-rotate") {
			getline(inputFile, rotValue, ',');
			rotX = stof(rotValue);
			getline(inputFile, rotValue, ',');
			rotY = stof(rotValue);
			getline(inputFile, rotValue, ' ');
			rotZ = stof(rotValue);
			tempRot = glm::vec3(rotX, rotY, rotZ);
		}
		//Retrieve translation values
		if (inputString == "-translate") {
			getline(inputFile, posValue, ',');
			posX = stof(posValue);
			getline(inputFile, posValue, ',');
			posY = stof(posValue);
			getline(inputFile, posValue, ' ');
			posZ = stof(posValue);
			tempPos = glm::vec3(posX, posY, posZ);
		}
		//Retrieve parent name
		if (inputString == "-parent") {
			getline(inputFile, parentString, '\n');
			//If parent is not root 
			if (parentString != "NULL") {
				Joint *temp = new Joint(tempPos, jointString);
				temp->rotation = tempRot;
				scene.push_back(temp);
				joints.push_back(temp);
				//Add children to parent
				for (int i = 0; i < joints.size(); i++) {
					if (joints[i]->name == parentString) {
						joints[i]->addChild(temp);
					}
				}
			}
			//If parent is root
			else {
				Joint *temp = new Joint(tempPos, jointString);
				temp->rotation = tempRot;
				scene.push_back(temp);
				joints.push_back(temp);
			}
		}
	}
	//Close input file
	inputFile.close();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

	switch (key) {
	case OF_KEY_ALT:
		bAltKeyDown = false;
		mainCam.disableMouseInput();
		break;
	case 'x':
		bRotateX = false;
		break;
	case 'y':
		bRotateY = false;
		break;
	case 'z':
		bRotateZ = false;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 'A':
	case 'a':
		deleteAllJoints();
		break;
	case 'C':
	case 'c':
		if (mainCam.getMouseInputEnabled()) mainCam.disableMouseInput();
		else mainCam.enableMouseInput();
		break;
	case 'D':
	case 'd':
		deleteJoint();
		break;
	case 'F':
	case 'b':
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'h':
		bHide = !bHide;
		break;
	case 'i':
		break;
	case 'J':
	case 'j':
		createJoint();
		break;
	case 'n':
		break;
	case 'p':
		if (objSelected()) printChannels(selected[0]);
		break;
	case 'R':
	case 'r':
		readFile();
		break;
	case 'S':
	case 's':
		saveToFile();
		break;
	case 'x':
		bRotateX = true;
		break;
	case 'y':
		bRotateY = true;
		break;
	case 'z':
		bRotateZ = true;
		break;
	case OF_KEY_F1: 
		theCam = &mainCam;
		break;
	case OF_KEY_F2:
		theCam = &sideCam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_ALT:
		bAltKeyDown = true;
		if (!mainCam.getMouseInputEnabled()) mainCam.enableMouseInput();
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (objSelected() && bDrag) {
		glm::vec3 point; 
		mouseToDragPlane(x, y, point);
		if (bRotateX) {
			selected[0]->rotation += glm::vec3((point.x - lastPoint.x) * 20.0, 0, 0);
		}
		else if (bRotateY) {
			selected[0]->rotation += glm::vec3(0, (point.x - lastPoint.x) * 20.0, 0);
		}
		else if (bRotateZ) {
			selected[0]->rotation += glm::vec3(0, 0, (point.x - lastPoint.x) * 20.0);
		}
		else {
			selected[0]->position += (point - lastPoint);
		}
		lastPoint = point;
	}

}

//  This projects the mouse point in screen space (x, y) to a 3D point on a plane
//  normal to the view axis of the camera passing through the point of the selected object.
//  If no object selected, the plane passing through the world origin is used.
//
bool ofApp::mouseToDragPlane(int x, int y, glm::vec3 &point) {
	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	float dist;
	glm::vec3 pos;
	if (objSelected()) {
		pos = selected[0]->position;
	}
	else pos = glm::vec3(0, 0, 0);
	if (glm::intersectRayPlane(p, dn, pos, glm::normalize(theCam->getZAxis()), dist)) {
		point = p + dn * dist;
		return true;
	}
	return false;
}

//--------------------------------------------------------------
//
// Provides functionality of single selection and if something is already selected,
// sets up state for translation/rotation of object using mouse.
//
void ofApp::mousePressed(int x, int y, int button){

	// if we are moving the camera around, don't allow selection
	//
	if (mainCam.getMouseInputEnabled()) return;

	// clear selection list
	//
	selected.clear();

	//
	// test if something selected
	//
	vector<SceneObject *> hits;

	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	// check for selection of scene objects
	//
	for (int i = 0; i < scene.size(); i++) {
		
		glm::vec3 point, norm;
		
		//  We hit an object
		//
		if (scene[i]->isSelectable && scene[i]->intersect(Ray(p, dn), point, norm)) {
			hits.push_back(scene[i]);
		}
	}


	// if we selected more than one, pick nearest
	//
	SceneObject *selectedObj = NULL;
	if (hits.size() > 0) {
		selectedObj = hits[0];
		float nearestDist = std::numeric_limits<float>::infinity();
		for (int n = 0; n < hits.size(); n++) {
			float dist = glm::length(hits[n]->position - theCam->getPosition());
			if (dist < nearestDist) {
				nearestDist = dist;
				selectedObj = hits[n];
			}	
		}
	}
	if (selectedObj) {
		selected.push_back(selectedObj);
		bDrag = true;
		mouseToDragPlane(x, y, lastPoint);
	}
	else {
		selected.clear();
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bDrag = false;

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

