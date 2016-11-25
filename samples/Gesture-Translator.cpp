/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include <cstring>
#include "Leap.h"
#include <unistd.h>
#include <iomanip>

using namespace Leap;

class SampleListener : public Listener {
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);

  private:
};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void SampleListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}


//Prints out "Sideways" and returns 1 if the hand is facing left. 
int isSideways(float *array) {

	if (array[2] < -40.0) {

std::cout << "Sideways" << std::endl;
		return 1;

	} else {
	
		return 0;

	}

}

//Prints out / returns 1 if hand is open.
int openOrClosed(Vector vector) {

	std::cout << vector.z << std::endl;

	if (vector.z >= 4.5) {	

std::cout << "HAND IS OPEN" << std::endl;
		return 1;

	} else {

		if (vector.z <= -2.5) {

std::cout << "HAND IS CLOSED" << std::endl;
			return 2;

		}

	}
	
	return 0;	

}

void printBoneInformation(float information[5][4][3]) {

	for (int finger = 0; finger < 5; finger = finger + 1) {

		std::cout << "Finger: ";

		switch (finger) {

			case 0:
			std::cout << "Thumb";
			break;

			case 1:
			std::cout << "Index";			
			break;

			case 2:
			std::cout << "Middle";
			break;

			case 3:
			std::cout << "Ring";
			break;

			case 4:
			std::cout << "Pinky";
			break;

		}

		std::cout << std::endl;

		for (int bone = 0; bone < 4; bone = bone + 1) {

			std::cout << "     ";

	
			switch (bone) {

				case 0:
				std::cout << "Metacarpal";
				break;

				case 1:
				std::cout << "Proximal";			
				break;

				case 2:
				std::cout << "Middle";
				break;

				case 3:
				std::cout << "Distal";
				break;

			}	

			

			for (int axis = 0; axis < 3; axis = axis + 1) {

				std::cout << "     ";

				switch (axis) {
	
					case 0:
					std::cout << "X = " << std::setprecision(2) << std::fixed << information[finger][bone][axis];
					break;

					case 1:
					std::cout << "Y = " << information[finger][bone][axis];
					break;
					
					case 2:
					std::cout << "Z = " << information[finger][bone][axis] << std::endl;
					break;

				}		
			
			}
		
		}

		std::cout << std::endl;

	}
	
	std::cout << std::endl;

}

//Returns the float average of the desired inputs. Bone_start and bone_finish allow for some flexibility.
float averageDirection(float information[5][4][3], int finger, int bone_start, int bone_finish, int axis) {

	float sum = 0.0;
		
	for (int temp = bone_start; temp <= bone_finish; temp = temp + 1) {

		sum = sum + information[finger][temp][axis];

	}

	return sum/(bone_finish - bone_start + 1);

}

//Calculates the distance between the distal phalanges for any 2 fingers
float distanceBetweenFingers(float information[5][4][3], int finger1, int finger2){
	float x1 = information[finger1][3][0];
	float y1 = information[finger1][3][2];
	float x2 = information[finger2][3][0];
	float y2 = information[finger2][3][2];

	float distancex = pow(x2-x1, 2);
	float distancey = pow(y2-y1, 2);

	return sqrt(distancex - distancey);
}


bool in_bounds(float information[5][4][3], int finger, int bone, int axis, float target) {
 
    float min, max;
    float range = 0.55;
    
    if ((target+range) > 1.0) {
        
        max = 1.0;
        
    } else {
     
        max = target + range;
        
    }
    
    if ((target - range) < -1.0) {
        
        min = -1.0;
        
    } else {
     
        min = target - range;
        
    }
    
    return (information[finger][bone][axis] > min && information[finger][bone][axis] < max);
    
}

bool check_bone(float information[5][4][3], int finger, int bone, float bonex, float boney, float bonez) {

	bool check1 = false;
	bool check2 = false;
	bool check3 = false;

	if (in_bounds(information, finger, bone, 0, bonex)) {

		check1 = true;

	}

	if (in_bounds(information, finger, bone, 1, boney)) {

		check2 = true;

	}

	if (in_bounds(information, finger, bone, 2, bonez)) {

		check3 = true;

	}

	return (check1 && check2 && check3);

} 

bool check_finger(float information[5][4][3], int finger, float bone1x, float bone2x, float bone3x, float bone1y, float bone2y, float bone3y, float bone1z, float bone2z, float bone3z) {

	bool check1 = false;
	bool check2 = false;
	bool check3 = false;

	if (check_bone(information, finger, 1, bone1x, bone1y, bone1z)) {

		check1 = true;
	
	}

	if (check_bone(information, finger, 2, bone2x, bone2y, bone2z)) {

		check2 = true;

	}

	if (check_bone(information, finger, 3, bone3x, bone3y, bone3z)) {

		check3 = true;

	}

	return (check1 && check2 && check3);

}

void presentation(float information[5][4][3]) {

	//Open and palm facing up
	if (check_finger(information, 0, 0.60, 0.12, 0.13, -0.73, -0.78, -0.78, 0.33, 0.61, 0.61) &&
		check_finger(information, 1, -0.01, -0.01, -0.02, -0.92, -0.90, -0.89, 0.39, 0.43, 0.46) &&
		check_finger(information, 2, 0.00, 0.00, 0.00, -0.89, -0.88, -0.86, 0.45, 0.48, 0.51) &&
		check_finger(information, 3, 0.05, 0.05, 0.06, -0.89, -0.87, -0.85, 0.46, 0.49, 0.52) &&
		check_finger(information, 4, 0.03, 0.04, 0.05, -0.91, -0.89, -0.87, 0.40, 0.45, 0.48)) {

	std::cout << "The hand is open, palm facing forward." << std::endl;
				
	} else {

		//Open and palm facing camera
		if (check_finger(information, 0, 0.62, 0.17, -0.44, -0.24, -0.01, 0.27, 0.75, 0.98, 0.86) &&
			check_finger(information, 1, -0.01, -0.01, -0.01, -0.02, 0.02, 0.06, 0.99, 0.99, 0.99) &&
			check_finger(information, 2, 0.00, 0.01, 0.02, 0.02, 0.08, 0.12, 0.99, 0.99, 0.99) &&
			check_finger(information, 3, 0.11, 0.12, 0.13, 0.06, 0.14, 0.20, 0.99, 0.99, 0.98) &&
			check_finger(information, 4, 0.12, 0.15, 0.17, -0.02, 0.07, 0.13, 0.99, 0.99, 0.99)) {

			std::cout << "The hand is open, palm facing the camera." << std::endl;
				
		} else {

			std::cout << "The hand is not positioned in any particular fashion." << std::endl;

		}

	}

	std::cout << std::endl;

}




//Returns the char of a signed letter that is displayed. This is done stupidly.
//Also this is done with the LeapMotion mounted on the table, not as a webcam.
char whichLetter(float information[5][4][3]) {

	//A
	if (check_finger(information, 0, 0.49, 0.53, -0.04, -0.80, -0.78, -0.84, 0.36, 0.34, 0.53)) {
		if (check_finger(information, 1, 0.01, -0.08, -0.07, 0.00, 0.99, 0.84, 0.99, 0.06, -0.53)) {
			if (check_finger(information, 2, 0.18, 0.01, -0.09, 0.00, 0.99, 0.82, 0.98, 0.04, -0.55)) {
				if (check_finger(information, 3, 0.21, 0.19, 0.03, -0.10, 0.97, 0.88, 0.97, 0.13, -0.48)) {
					if(check_finger(information, 4, 0.30, 0.52, 0.25, -0.20, 0.85, 0.83, 0.93, 0.08, -0.50)) {

					return 'A';
				
					}
				}
			}
		}
	}


	//B
	if (check_finger(information, 0, 0.52, -0.17, -0.86, -0.33, -0.15, 0.18, 0.79, 0.98, 0.48)) {
		if (check_finger(information, 1, 0.01, 0.00, -0.01, -0.75, -0.71, -0.67, 0.66, 0.71, 0.75)) {
			if (check_finger(information, 2, -0.04, -0.04, -0.04, -0.72, -0.68, -0.64, 0.69, 0.73, 0.77)) {
				if (check_finger(information, 3, -0.08, -0.07, -0.06, -0.70, -0.65, -0.60, 0.71, 0.76, 0.80)) {
					if(check_finger(information, 4, -0.07, -0.05, -0.03, -0.73, -0.68, -0.62, 0.68, 0.74, 0.79)) {

					return 'B';
				
					}
				}
			}
		}
	}


	//C
	if (check_finger(information, 0, 0.76, 0.77, 0.77, -0.64, -0.63, -0.63, 0.09, 0.11, 0.10)) {
		if (check_finger(information, 1, 0.63, 0.66, 0.68, -0.67, -0.64, -0.62, 0.39, 0.38, 0.37)) {
			if (check_finger(information, 2, 0.69, 0.72, 0.74, -0.64, -0.62, -0.60, 0.34, 0.32, 0.30)) {
				if (check_finger(information, 3, 0.66, 0.70, 0.73, -0.67, -0.65, -0.62, 0.33, 0.30, 0.28)) {
					if(check_finger(information, 4, 0.65, 0.70, 0.73, -0.65, -0.62, -0.61, 0.39, 0.35, 0.32)) {

					return 'C';
				
					}
				}
			}
		}
	}


	//D
	if (check_finger(information, 0, 0.33, -0.06, -0.90, -0.72, -0.69, -0.12, 0.62, 0.72, 0.41)) {
		if (check_finger(information, 1, 0.02, 0.00, -0.01, -0.94, -0.88, -0.81, 0.33, 0.47, 0.58)) {
			if (check_finger(information, 2, 0.08, 0.08, 0.03, 0.35, 0.97, 0.69, 0.93, -0.21, -0.72)) {
				if (check_finger(information, 3, 0.13, 0.22, 0.12, 0.32, 0.96, 0.69, 0.94, -0.19, -0.71)) {
					if(check_finger(information, 4, 0.25, 0.41, 0.24, 0.21, 0.90, 0.71, 0.95, -0.14, -0.66)) {

					return 'D';
				
					}
				}
			}
		}
	}

	//E
	if (check_finger(information, 0, 0.28, -0.37, -0.71, -0.36, -0.21, -0.07, 0.89, 0.90, 0.70)) {
		if (check_finger(information, 1, 0.06, -0.04, -0.08, -0.69, 0.58, 0.98, 0.72, 0.82, -0.19)) {
			if (check_finger(information, 2, 0.04, 0.12, 0.02, -0.69, 0.60, 0.97, 0.72, 0.79, -0.25)) {
				if (check_finger(information, 3, 0.15, 0.22, 0.05, -0.57, 0.57, 0.99, 0.80, 0.78, -0.02)) {
					if(check_finger(information, 4, 0.19, 0.38, 0.18, -0.66, 0.44, 0.97, 0.72, 0.81, 0.11)) {

					return 'E';
				
					}
				}
			}
		}
	}


	//F
	if (check_finger(information, 0, 0.53, -0.13, -0.09, -0.28, -0.15, -0.16, 0.79, 0.98, 0.98)) {
		if (check_finger(information, 1, 0.15, -0.27, -0.37, 0.59, 0.94, 0.60, 0.80, -0.20, -0.70)) {
			if (check_finger(information, 2, -0.01, -0.01, 0.00, -0.71, -0.68, -0.66, 0.71, 0.73, 0.76)) {
				if (check_finger(information, 3, 0.10, 0.11, 0.12, -0.73, -0.71, -0.69, 0.67, 0.70, 0.71)) {
					if(check_finger(information, 4, 0.07, 0.09, 0.11, -0.84, -0.82, -0.79, 0.53, 0.57, 0.60)) {

					return 'F';
				
					}
				}
			}
		}
	}


	//G
	if (check_finger(information, 0, 0.85, 0.93, 0.88, -0.44, -0.14, -0.36, 0.29, 0.35, 0.31)) {
		if (check_finger(information, 1, 0.98, 0.99, 0.98, -0.11, -0.02, 0.03, 0.19, -0.05, -0.20)) {
			if (check_finger(information, 2, 0.84, -0.31, -0.77, -0.01, 0.21, 0.22, -0.54, -0.92, -0.60)) {
				if (check_finger(information, 3, 0.82, -0.34, -0.79, -0.07, 0.05, 0.08, -0.56, -0.94, -0.61)) {
					if(check_finger(information, 4, 0.86, -0.27, -0.73, -0.21, -0.14, -0.02, -0.47, -0.95, -0.68)) {

					return 'G';
				
					}
				}
			}
		}
	}


	//H
	if (check_finger(information, 0, 0.12, -0.17, -0.39, -0.08, -0.04, 0.00, 0.99, 0.98, 0.92)) {
		if (check_finger(information, 1, 0.07, 0.05, 0.03, -0.18, 0.00, 0.12, 0.98, 0.99, 0.99)) {
			if (check_finger(information, 2, 0.10, -0.03, -0.09, 0.70, 0.96, 0.68, 0.71, -0.28, -0.73)) {
				if (check_finger(information, 3, 0.11, 0.13, 0.08, 0.70, 0.94, 0.66, 0.70, -0.30, -0.75)) {
					if(check_finger(information, 4, 0.02, 0.08, 0.13, -0.21, 0.00, 0.15, 0.97, 0.99, 0.98)) {

					return 'H';
				
					}
				}
			}
		}
	}


	//I
	if (check_finger(information, 0, 0.31, -0.41, -0.85, -0.48, -0.39, -0.15, 0.82, 0.82, 0.50)) {
		if (check_finger(information, 1, 0.08, -0.29, -0.31, 0.40, 0.93, 0.62, 0.91, -0.21, -0.72)) {
			if (check_finger(information, 2, 0.21, -0.11, -0.22, 0.43, 0.96, 0.63, 0.88, -0.26, -0.74)) {
				if (check_finger(information, 3, 0.27, 0.02, -0.13, 0.41, 0.96, 0.64, 0.87, -0.28, -0.76)) {
					if(check_finger(information, 4, -0.18, -0.01, 0.14, -0.90, -0.70, -0.45, 0.41, 0.71, 0.88)) {

					return 'I';
				
					}
				}
			}
		}
	}

	//J
	if (check_finger(information, 0, -0.08, -0.37, -0.77, 0.32, 0.36, 0.35, 0.95, 0.86, 0.54)) {
		if (check_finger(information, 1, 0.00, -0.02, -0.02, 0.88, 0.55, -0.15, 0.47, -0.84, -0.99)) {
			if (check_finger(information, 2, 0.18, 0.02, -0.10, 0.86, 0.56, -0.12, 0.49, -0.82, -0.98)) {
				if (check_finger(information, 3, 0.17, 0.20, 0.04, 0.82, 0.59, -0.09, 0.55, -0.78, -0.99)) {
					if(check_finger(information, 4, -0.13, -0.05, 0.01, -0.03, 0.15, 0.28, 0.99, 0.99, 0.96)) {

					return 'J';
				
					}
				}
			}
		}
	}

	//K
	if (check_finger(information, 0, 0.43, -0.20, -0.37, -0.52, -0.29, -0.19, 0.74, 0.94, 0.91)) {
		if (check_finger(information, 1, 0.06, 0.08, 0.10, -0.74, -0.58, -0.41, 0.67, 0.81, 0.91)) {
			if (check_finger(information, 2, 0.22, 0.24, 0.24, 0.24, 0.33, 0.38, 0.95, 0.92, 0.90)) {
				if (check_finger(information, 3, 0.33, 0.22, 0.06, 0.68, 0.86, 0.56, 0.65, -0.46, -0.83)) {
					if(check_finger(information, 4, 0.44, 0.43, 0.23, 0.57, 0.80, 0.57, 0.69, -0.41, -0.79)) {

					return 'K';
				
					}
				}
			}
		}
	}


	//L
	if (check_finger(information, 0, 0.73, 0.75, 0.90, -0.44, -0.42, -0.30, 0.53, 0.50, 0.30)) {
		if (check_finger(information, 1, 0.11, 0.06, 0.02, -0.78, -0.64, -0.50, 0.61, 0.77, 0.87)) {
			if (check_finger(information, 2, -0.02, -0.06, -0.06, 0.52, 0.99, 0.86, 0.86, -0.06, -0.49)) {
				if (check_finger(information, 3, 0.03, 0.11, 0.11, 0.50, 0.99, 0.86, 0.86, -0.06, -0.49)) {
					if(check_finger(information, 4, 0.15, 0.39, 0.37, 0.40, 0.92, 0.84, 0.90, 0.04, -0.41)) {

					return 'L';
				
					}
				}
			}
		}
	}


	//M
	if (check_finger(information, 0, 0.29, -0.29, -0.84, 0.01, 0.14, 0.23, 0.96, 0.95, 0.49)) {
		if (check_finger(information, 1, 0.02, 0.00, -0.02, 0.41, 0.67, 0.80, 0.91, 0.75, 0.60)) {
			if (check_finger(information, 2, 0.04, 0.08, 0.10, 0.42, 0.69, 0.82, 0.91, 0.72, 0.57)) {
				if (check_finger(information, 3, 0.13, 0.17, 0.18, 0.39, 0.62, 0.74, 0.91, 0.77, 0.64)) {
					if(check_finger(information, 4, 0.27, 0.37, 0.38, 0.74, 0.93, 0.91, 0.61, 0.11, -0.16)) {

					return 'M';
				
					}
				}
			}
		}
	}

	//N
	if (check_finger(information, 0, 0.40, -0.12, -0.70, -0.20, -0.05, 0.15, 0.90, 0.99, 0.70)) {
		if (check_finger(information, 1, 0.07, 0.05, 0.03, 0.41, 0.57, 0.66, 0.91, 0.82, 0.75)) {
			if (check_finger(information, 2, 0.07, 0.09, 0.11, 0.44, 0.61, 0.70, 0.89, 0.78, 0.70)) {
				if (check_finger(information, 3, 0.19, 0.20, 0.14, 0.77, 0.94, 0.74, 0.61, -0.27, -0.65)) {
					if(check_finger(information, 4, 0.24, 0.51, 0.50, 0.72, 0.83, 0.64, 0.65, -0.20, -0.58)) {

					return 'N';
				
					}
				}
			}
		}
	}


	//O
	if (check_finger(information, 0, 0.55, 0.59, 0.32, -0.80, -0.66, -0.16, -0.24, 0.46, 0.93)) {
		if (check_finger(information, 1, 0.97, -0.07, -0.63, 0.18, 0.99, 0.77, 0.15, 0.03, -0.07)) {
			if (check_finger(information, 2, 0.98, -0.07, -0.64, 0.17, 0.98, 0.75, 0.01, -0.18, -0.17)) {
				if (check_finger(information, 3, 0.99, 0.01, -0.59, 0.09, 0.95, 0.77, -0.03, -0.31, -0.25)) {
					if(check_finger(information, 4, 0.99, 0.08, -0.53, -0.04, 0.79, 0.71, -0.09, -0.60, -0.47)) {

					return 'O';
				
					}
				}
			}
		}
	}


	//P
	if (check_finger(information, 0, 0.62, -0.02, -0.15, 0.14, 0.43, 0.46, 0.77, 0.90, 0.87)) {
		if (check_finger(information, 1, 0.12, 0.10, 0.09, 0.21, 0.31, 0.40, 0.97, 0.94, 0.91)) {
			if (check_finger(information, 2, 0.13, 0.09, 0.07, 0.97, 0.99, 0.95, 0.19, -0.13, -0.30)) {
				if (check_finger(information, 3, 0.14, 0.05, -0.03, 0.99, 0.22, -0.33, -0.09, -0.97, -0.95)) {
					if(check_finger(information, 4, 0.24, 0.31, 0.19, 0.97, 0.25, -0.29, -0.01, -0.92, -0.94)) {

					return 'P';
				
					}
				}
			}
		}
	}


	//Q
	if (check_finger(information, 0, -0.03, 0.02, 0.37, 0.91, 0.91, 0.79, 0.41, 0.42, 0.48)) {
		if (check_finger(information, 1, 0.04, 0.03, 0.02, 0.97, 0.99, 0.99, 0.22, -0.01, -0.16)) {
			if (check_finger(information, 2, 0.10, 0.18, 0.09, 0.93, -0.25, -0.82, -0.35, -0.95, -0.57)) {
				if (check_finger(information, 3, 0.12, 0.34, 0.21, 0.94, -0.24, -0.81, -0.33, -0.91, -0.54)) {
					if(check_finger(information, 4, 0.12, 0.68, 0.49, 0.96, -0.16, -0.76, -0.27, -0.72, -0.43)) {

					return 'Q';
				
					}
				}
			}
		}
	}


	//R
	if (check_finger(information, 0, 0.55, 0.59, 0.36, -0.83, -0.81, -0.91, 0.09, 0.06, 0.21)) {
		if (check_finger(information, 1, -0.07, -0.07, -0.06, -0.81, -0.79, -0.77, 0.58, 0.61, 0.64)) {
			if (check_finger(information, 2, 0.20, 0.20, 0.21, -0.86, -0.84, -0.82, 0.48, 0.51, 0.53)) {
				if (check_finger(information, 3, 0.26, 0.17, -0.01, 0.06, 0.98, 0.79, 0.96, -0.06, -0.62)) {
					if(check_finger(information, 4, 0.36, 0.49, 0.19, -0.05, 0.87, 0.75, 0.93, -0.09, -0.63)) {

					return 'R';
				
					}
				}
			}
		}
	}


	//S
	if (check_finger(information, 0, 0.30, -0.18, -0.42, -0.75, -0.76, -0.69, 0.59, 0.63, 0.59)) {
		if (check_finger(information, 1, 0.00, -0.35, -0.35, -0.07, 0.87, 0.91, 0.99, 0.35, -0.23)) {
			if (check_finger(information, 2, 0.11, -0.09, -0.15, -0.06, 0.92, 0.96, 0.99, 0.36, -0.22)) {
				if (check_finger(information, 3, 0.13, 0.07, -0.02, 0.10, 0.99, 0.76, 0.99, -0.08, -0.65)) {
					if(check_finger(information, 4, 0.26, 0.33, 0.12, 0.00, 0.94, 0.78, 0.97, -0.05, -0.61)) {

					return 'S';
				
					}
				}
			}
		}
	}


	//T
	if (check_finger(information, 0, 0.53, -0.13, -0.51, -0.49, -0.33, -0.15, 0.70, 0.93, 0.85)) {
		if (check_finger(information, 1, 0.09, 0.07, 0.05, -0.03, 0.16, 0.28, 0.99, 0.99, 0.96)) {
			if (check_finger(information, 2, 0.11, 0.05, 0.01, 0.80, 0.98, 0.86, 0.59, -0.18, -0.50)) {
				if (check_finger(information, 3, 0.18, 0.17, 0.13, 0.80, 0.97, 0.85, 0.58, -0.20, -0.51)) {
					if(check_finger(information, 4, 0.29, 0.43, 0.40, 0.70, 0.90, 0.80, 0.65, -0.12, -0.44)) {

					return 'T';
				
					}
				}
			}
		}
	}


	//U
	if (check_finger(information, 0, 0.28, -0.42, -0.88, -0.44, -0.31, -0.06, 0.86, 0.85, 0.49)) {
		if (check_finger(information, 1, -0.06, -0.07, -0.08, -0.72, -0.69, -0.67, 0.69, 0.72, 0.74)) {
			if (check_finger(information, 2, 0.05, 0.05, 0.05, -0.67, -0.63, -0.60, 0.74, 0.77, 0.80)) {
				if (check_finger(information, 3, 0.05, 0.19, 0.18, 0.64, 0.96, 0.74, 0.77, -0.23, -0.64)) {
					if(check_finger(information, 4, 0.17, 0.45, 0.41, 0.57, 0.88, 0.69, 0.80, -0.17, -0.59)) {

					return 'U';
				
					}
				}
			}
		}
	}


	//V
	if (check_finger(information, 0, 0.09, -0.30, -0.78, 0.37, 0.48, 0.49, 0.92, 0.82, 0.40)) {
		if (check_finger(information, 1, 0.25, 0.25, 0.25, -0.02, 0.03, 0.08, 0.97, 0.97, 0.96)) {
			if (check_finger(information, 2, -0.35, -0.35, -0.34, 0.04, 0.08, 0.12, 0.94, 0.93, 0.93)) {
				if (check_finger(information, 3, 0.11, 0.25, 0.30, 0.96, 0.96, 0.90, 0.25, -0.13, -0.32)) {
					if(check_finger(information, 4, 0.25, 0.38, 0.42, 0.90, 0.92, 0.88, 0.35, -0.04, -0.23)) {

					return 'V';
				
					}
				}
			}
		}
	}

	//W
	if (check_finger(information, 0, 0.34, -0.27, -0.82, -0.22, -0.08, 0.11, 0.91, 0.96, 0.57)) {
		if (check_finger(information, 1, 0.36, 0.36, 0.36, -0.65, -0.61, -0.58, 0.67, 0.70, 0.73)) {
			if (check_finger(information, 2, 0.07, 0.08, 0.08, -0.66, -0.62, -0.59, 0.75, 0.78, 0.80)) {
				if (check_finger(information, 3, -0.25, -0.24, -0.23, -0.54, -0.46, -0.39, 0.80, 0.85, 0.89)) {
					if(check_finger(information, 4, 0.21, 0.36, 0.31, 0.53, 0.93, 0.81, 0.82, -0.02, -0.50)) {

					return 'W';
				
					}
				}
			}
		}
	}


	//X
	if (check_finger(information, 0, 0.63, 0.55, 0.53, -0.61, -0.65, -0.65, 0.49, 0.53, 0.54)) {
		if (check_finger(information, 1, 0.20, -0.20, -0.33, -0.76, 0.34, 0.93, 0.61, 0.92, 0.20)) {
			if (check_finger(information, 2, -0.05, 0.02, 0.04, 0.30, 0.99, 0.73, 0.95, -0.16, -0.68)) {
				if (check_finger(information, 3, 0.01, 0.14, 0.12, 0.27, 0.98, 0.74, 0.96, -0.13, -0.67)) {
					if(check_finger(information, 4, 0.13, 0.44, 0.33, 0.18, 0.89, 0.71, 0.98, -0.08, -0.62)) {

					return 'X';
				
					}
				}
			}
		}
	}


	//Y
	if (check_finger(information, 0, 0.58, 0.52, 0.63, -0.68, -0.70, -0.66, 0.45, 0.49, 0.41)) {
		if (check_finger(information, 1, -0.08, -0.01, 0.04, 0.35, 0.98, 0.72, 0.93, -0.17, -0.69)) {
			if (check_finger(information, 2, 0.07, 0.21, 0.16, 0.35, 0.96, 0.70, 0.93, -0.18, -0.70)) {
				if (check_finger(information, 3, 0.09, 0.41, 0.34, 0.27, 0.91, 0.69, 0.96, -0.09, -0.64)) {
					if(check_finger(information, 4, -0.70, -0.62, -0.55, -0.45, -0.38, -0.33, 0.56, 0.69, 0.77)) {

					return 'Y';
				
					}
				}
			}
		}
	}

	//Z
	if (check_finger(information, 0, 0.69, 0.68, 0.90, -0.17, -0.17, -0.30, 0.71, 0.71, 0.32)) {
		if (check_finger(information, 1, 0.01, 0.00, -0.01, 0.00, 0.15, 0.27, 0.99, 0.99, 0.96)) {
			if (check_finger(information, 2, 0.10, -0.05, -0.10, 0.99, 0.20, -0.42, 0.11, -0.98, -0.90)) {
				if (check_finger(information, 3, 0.17, 0.03, -0.07, 0.98, 0.16, -0.43, 0.08, -0.99, -0.93)) {
					if(check_finger(information, 4, 0.30, 0.19, -0.01, 0.94, 0.22, -0.36, 0.18, -0.96, -0.93)) {

					return 'Z';
				
					}
				}
			}
		}
	}

	

    return ' ';

}

void SampleListener::onFrame(const Controller& controller) {

	//Recieve information about the current frame.
	const Frame frame = controller.frame();

/*
//Print out frame info, including time, number of hands, and the ID of the frame.
std::cout << "Frame id: " << frame.id()
	<< ", timestamp: " << frame.timestamp()
	<< ", hands: " << frame.hands().count()
	<< ", extended fingers: " << frame.fingers().extended().count()
	<< ", tools: " << frame.tools().count()
	<< ", gestures: " << frame.gestures().count() << std::endl;
*/

	HandList hands = frame.hands();
	
	for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
	
		// Get the first hand
		const Hand hand = *hl;
		std::string handType = hand.isLeft() ? "Left hand" : "Right hand";

/*
//Print out the infor for the current hand, including type and position.
std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
	<< ", palm position: " << hand.palmPosition() << std::endl;
*/

/*
		// Get the hand's normal vector and direction
		const Vector normal = hand.palmNormal();
		const Vector direction = hand.direction();


		//Create an array that houses the pitch, roll and yaw of the current hand.
		float angles[3];
	
		//Pitch
		angles[0] = direction.pitch() * RAD_TO_DEG;
		//Roll
		angles[1] = normal.roll() * RAD_TO_DEG;
		//Yaw
		angles[2] = direction.yaw() * RAD_TO_DEG;
*/


/*
// Print out the pitch, roll, and yaw.
std::cout << "Pitch: " << angles[0] << " | Roll: " << angles[1]
	<< " | Yaw: " << angles[2] << std::endl;
*/

		//Setup for isSideways test
		//int sideways_check = isSideways(angles);

		//Setup for openOrClosed test
		//Vector open_or_closed_vector = Vector::zero();

		//Keeps track of one bones of each finger. 
		//A triple array, with the first element being the finger (0 thumb, 4 pinky),
		//the second element being the bone of that finger, (0 metacarpel, 3 distal),
		//and the third element being the axis of that bones (x, y, z).
		float bone_information[5][4][3];
		
		//Get finger information
		const FingerList fingers = hand.fingers();

		int finger_number = 0;

		for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {

			const Finger finger = *fl;
	
			// Get finger bones
			for (int b = 0; b < 4; ++b) {

				Bone::Type boneType = static_cast<Bone::Type>(b);
				Bone bone = finger.bone(boneType);

/*
std::cout << std::string(6, ' ') <<  boneNames[boneType]
	<< " bone, start: " << bone.prevJoint()
	<< ", end: " << bone.nextJoint()
	<< ", direction: " << bone.direction() << std::endl;
*/

				Vector temp_vector = bone.direction();
				bone_information[finger_number][b][0] = temp_vector.x;
				bone_information[finger_number][b][1] = temp_vector.y;
				bone_information[finger_number][b][2] = temp_vector.z;

			}

			finger_number = finger_number + 1;

		}

printBoneInformation(bone_information);	
presentation(bone_information);	
std::cout << "\033[2J";
	}

	//Delay a quarter of a second so the output is readable.
	//This delay should be removed entirely once the output doesn't need to be read in real time.
	usleep(250000);

}



void SampleListener::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
  std::cout << "Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    std::cout << "id: " << devices[i].toString() << std::endl;
    std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
}

void SampleListener::onServiceConnect(const Controller& controller) {
  std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
  std::cout << "Service Disconnected" << std::endl;
}

int main(int argc, char** argv) {
  // Create a sample listener and controller
  SampleListener listener;
  Controller controller;
   
  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  if (argc > 1 && strcmp(argv[1], "--bg") == 0)
    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

  // Keep this process running until Enter is pressed
  std::cout << "Press Enter to quit..." << std::endl;
  std::cin.get();

  // Remove the sample listener when done
  controller.removeListener(listener);

  return 0;
}
