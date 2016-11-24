#ifndef __SDCHLC_HH__
#define __SDCHLC_HH__

#include <math.h>
#include <vector>

#include "gazebo/physics/physics.hh"
#include "gazebo/transport/transport.hh"

#include "globals.hh"
#include "sdcIntersection.hh"
#include "sdcLLC.hh"
#include "sdcVisibleObject.hh"
#include "Waypoints.hh"

namespace gazebo {
  typedef struct {
    double time;
    math::Vector2d worldLocation;
    math::Vector2d carLocation;
  } CollisionEvent;

  class sdcCar;

  class GAZEBO_VISIBLE sdcHLC {
  public:
    sdcHLC(sdcCar* car);
    ~sdcHLC();

    void update();

    // The 'Brain' Methods
    void Drive();
    void MatchTargetDirection();
    void MatchTargetSpeed();
    void DetectIntersection();

    //Dijkstra Methods
    void GenerateWaypoints();
    void initializeGraph();
    int getFirstIntersection();
    void removeStartingEdge(int start);
    std::vector<int> dijkstras(int start, int dest);
    void insertWaypointTypes(std::vector<int> path, Direction startDir);

    // Driving algorithms
    void LanedDriving();
    void GridTurning(int turn);
    void WaypointDriving(std::vector<sdcWaypoint> waypoints);
    void Follow();
    void Avoidance();
    void PerpendicularPark();
    void ParallelPark();

    // Collision detection/avoidance functions
    sdcVisibleObject* CheckNearbyObjectsForCollision() const;
    bool IsObjectOnCollisionCourse(const sdcVisibleObject* obj) const;
    bool DoMaximumBoundingBoxesCollide(const sdcVisibleObject* obj) const;
    double DoMaximumRadiiCollide(const sdcVisibleObject* obj) const;
    bool DoMaximumRadiiCollideAtTime(const sdcVisibleObject* obj,
                                     double time) const;
    bool DoAccurateVehicleShapesCollide(const sdcVisibleObject* obj,
                                        double time) const;
    bool DoAccurateVehicleShapesCollideAtTime(const sdcVisibleObject* obj,
                                              double time) const;
    std::vector<sdcWaypoint*>* ComputeAvoidancePath(
      sdcVisibleObject* obj, math::Vector2d collision);
    math::Vector2d GetPositionAtTime(double time) const;
    sdcAngle GetAngleAtTime(double time) const;
    sdcAngle GetCollisionAngleAtTime(const sdcVisibleObject* obj,
                                     double time) const;


  private:
    sdcCar* car_;
    sdcLLC* llc_;
    Waypoints* waypoints_;

    double splineTime_ = 0;
    sdcSpline* spline_;

    // ================================================
    // 2016 states
    // ================================================
    enum MetaStates {
      START_16, FINISH_16, ROAD_16, INTERSECTION_16, PARKING_16
    };

    // The different sub-states within the ROAD metastate
    enum RoadStates {
      ENTER_16, EXIT_16, FOLLOW_16, APPROACH_16, STOP_16, WAIT_16,
      PASS_16, AVOID_16, RETURN_16
    };

    // ================================================
    // 2015 states
    // ================================================

    // The different states the car can be in. The logic and behavior of
    // the car will change depending on which state it's in, with various
    // sensor readings affecting the decision to transition states
    enum CarState {
      STOP, WAYPOINT, INTERSECTION, FOLLOW, AVOIDANCE, PARKING
    };

    // The different states the car can be in while performing a back
    // parallel park
    enum ParallelParkingState {
      rightBack, leftBack, rightForward, straightForward, doneParallel
    };

    // The different states the car can be in while performing a back
    // perpendicular park
    enum PerpendicularParkingState {
      stopPark, frontPark, straightPark, backPark, donePark
    };

    // The different states available when attempting to avoid objects
    enum AvoidanceState {
      emergencyStop, emergencySwerve, navigation, notAvoiding
    };

    PerpendicularParkingState currentPerpendicularState_;
    ParallelParkingState currentParallelState_;
    AvoidanceState currentAvoidanceState_;

    // The current state of the car
    CarState DEFAULT_STATE;
    CarState currentState_;

    //dijkstra's stuff
    std::vector<int> unvisited_;
    std::vector<sdcIntersection> intersections_;
    const int size = 5;
    const std::pair<double,double> destination_ = {0,0};
  };
}
#endif
