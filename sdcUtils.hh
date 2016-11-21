#include <type_traits>
#include <math.h>

#include <opencv2/opencv.hpp>

// computes the pythagorean theoram (for distance calculation), and only
// accepts numeric types.
template<typename T>
T pythag_thm(T x, T y) {
  static_assert(std::is_arithmetic<T>::value,
                "pythag_thm only accepts numeric values");
  return sqrt(pow(x, 2) + pow(y, 2));
}

/////////////////////////////////////////
// Linear Algebra functions for OpenCV //
/////////////////////////////////////////

/*
 * Returns a point rotated about axis by angle.
 */
template<typename T>
cv::Point_<T> rotatePoint(cv::Point_<T> point, const cv::Point_<T>& axis,
                          T angle) {
  static_assert(std::is_arithmetic<T>::value,
                "rotatePoint only accepts Points with numeric values");
  float s = sin(angle);
  float c = cos(angle);

  // translate point to rotate about the origin
  point = point - axis;

  // rotate point by angle about the origin, and translate back
  point.x = (point.x * c - point.y * s) + axis.x;
  point.y = (point.x * s + point.y * c) + axis.y;

  return point;
}

/*
 * Returns a point rotated about the origin by angle.
 */
template<typename T>
cv::Point_<T> rotatePoint(cv::Point_<T> point, T angle) {
  static_assert(std::is_arithmetic<T>::value,
                "rotatePoint only accepts Points with numeric values");
  float s = sin(angle);
  float c = cos(angle);

  // rotate point by angle about the origin, and translate back
  point.x = point.x * c - point.y * s;
  point.y = point.x * s + point.y * c;

  return point;
}

/*
 * Returns a vector rotated about an axis by an angle.
 */
template<typename T>
cv::Vec<T, 2> rotateVector(cv::Vec<T, 2> vec, T angle) {
  static_assert(std::is_arithmetic<T>::value,
                "rotatePoint only accepts Points with numeric values");
  float s = sin(angle);
  float c = cos(angle);

  // rotate vec by angle about the origin
  vec.x = vec.x * c - vec.y * s;
  vec.y = vec.x * s + vec.y * c;

  return vec;
}

/*
 * Returns the length of a point (that is, its distance from the origin).
 */
template<typename T>
T norm2(cv::Point_<T> point) {
  static_assert(std::is_arithmetic<T>::value,
                "norm only accepts Vecs with numeric values");
  return pythag_thm(point.x, point.y);
}

/*
 * Returns the length of a vector.
 */
template<typename T>
cv::Vec<T, 2> norm2(cv::Vec<T, 2> vec) {
  static_assert(std::is_arithmetic<T>::value,
                "norm only accepts Vecs with numeric values");
  return pythag_thm(vec.x, vec.y);
}

/*
 * Returns a normalized vector parallel to vec.
 */
template<typename T>
cv::Vec<T, 2> normalized(cv::Vec<T, 2> vec) {
  static_assert(std::is_arithmetic<T>::value,
                "normalized only accepts Vecs with numeric values");
  T length = pythag_thm(vec.x, vec.y);
  vec.x = vec.x / length;
  vec.y = vec.y / length;
  return vec;
}
