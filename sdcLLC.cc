//sdcLLC.hh containsCGAL typedefs and includes - may restructure location
#include "sdcLLC.hh"

#include "gazebo/physics/physics.hh"
#include "gazebo/transport/transport.hh"
#include <opencv2/opencv.hpp>

#include "arr_print.h"
#include "dataProcessing.hh"
#include "globals.hh"
#include "sdcCar.hh"
#include "sdcHLC.hh"
#include "Waypoints.hh"
#include "dubins.hh"

using namespace gazebo;

void sdcLLC::update() {
}

// LLC Constructor
sdcLLC::sdcLLC(sdcCar* car): car_(car) {
  std::vector<Waypoint> testPoints;
  Waypoint testPoint;
  testPoint.x = 70;
  testPoint.y = 5;
  //testPoint.direction = car_->GetDirection().angle;
  testPoint.direction=0;

  math::Vector2d carPos = sdcSensorData::GetPosition();
  Waypoint carPoint;
  //  carPoint.x = carPos.x;
  //carPoint.y = carPos.y;
  //carPoint.direction = car_->GetDirection().angle;
  carPoint.x=0;
  carPoint.y=0;
  carPoint.direction=0;

  dubins_ = new dubins();

  testPoints.push_back(testPoint);

  path_ = dubins_->calculateDubins(testPoints, carPoint);
}



/*
 * Speeds up the car by the given amount (in m/s) at the given rate
 *
 * Default amt: 1.0
 * Default rate: 1.0
 */
void sdcLLC::Accelerate(double amt, double rate) {
  car_->SetTargetSpeed(car_->GetSpeed() + amt);
  car_->SetAccelRate(rate);
}

/*
 * Slows down the car by the given amount (in m/s) at the given rate
 *
 * Default amt: 1.0
 * Default rate: 1.0
 */
void sdcLLC::Brake(double amt, double rate) {
  car_->SetTargetSpeed(car_->GetSpeed() - amt);
  car_->SetBrakeRate(rate);
}

/*
 * Sets the target speed to 0 m/s
 */
void sdcLLC::Stop() {
  car_->SetTargetSpeed(0);
}

/*
 * Move the car in reverse. Target speed will now be matched with the car going
 * backwards and target direction should be the direction of velocity desired,
 * NOT the direction the front of the car is facing
 */
void sdcLLC::Reverse() {
  car_->reversing_ = true;
}

/*
 * Stop reversing the car.
 */
void sdcLLC::StopReverse() {
  car_->reversing_ = false;
}

//Helper function for calculating our dubins point
//Given a distance to travel and input controls corresponding to a dubins path, output controls coresponding to travel a given distance along the path
std::vector<Control> dubinsPointHelper(std::vector<Control> controls, double distance) {

  std::vector<Control>::iterator it;
  std::vector<Control> newControls;

  for(it = controls.begin(); it < controls.end(); it++) {
    Control temp;
    temp.direction = it->direction;
    if (it->distance <= distance) {
      temp.distance = it->distance;
      distance = distance - it->distance;
    }
    else{
      temp.distance = distance;
      distance = 0;
    }
    newControls.push_back(temp);
  }
  return newControls;
}

//Function that finds a point along our dubins path at a specified distance
cv::Point2d sdcLLC::GetDubinsPoint(double distance) const {
  distance = fmin(distance, path_.length);
  math::Vector2d carPos = sdcSensorData::GetPosition();

  std::vector<Control> cont = dubins_->pathToControls(path_);
  cv::Point3d origin = cv::Point3d(path_.origin);

  //generates temporary set of controls used to help find a point on the dubins path
  cont = dubinsPointHelper(cont, distance);

  //this loop controls the logic to find a point along our dubins path
  for(std::vector<Control>::iterator it = cont.begin(); it < cont.end(); it++) {
    switch(it->direction) {
    case -1:
      origin = dubins_->leftTurn(origin.x, origin.y, origin.z, it->distance);
      break;
    case 0:
      origin = dubins_->straightTurn(origin.x, origin.y, origin.z, it->distance);
      break;
    case 1:
      origin = dubins_->rightTurn(origin.x, origin.y, origin.z, it->distance);
      break;
    }
  }


  cv::Point2d finalPoint;
  cv::Point2d tempPoint;

  //move target point to the origin of our original dubins path
  tempPoint.x = origin.x - path_.origin.x;
  tempPoint.y = origin.y - path_.origin.y;

  //rotate target point around dubins path origin
  finalPoint.x = tempPoint.x * cos(path_.rotationAngle)
    - tempPoint.y * sin(path_.rotationAngle);
finalPoint.y = tempPoint.x * sin(path_.rotationAngle)
  + tempPoint.y * cos(path_.rotationAngle);

    //scale rotated point back to a place corressponding to its original coords
  finalPoint.x += path_.origin.x;
  finalPoint.y += path_.origin.y;


  printf("(x,y,theta): (%f, %f, %f)\n", finalPoint.x, finalPoint.y, origin.z);
  return finalPoint;
}
