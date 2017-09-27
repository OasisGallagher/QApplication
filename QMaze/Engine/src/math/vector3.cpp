#include "math/vector3.h"

Vector3 Vector3::one = Vector3(1);

Vector3::Vector3() : Vector3(0) {
}

Vector3::Vector3(float value) : Vector3(value, value, value) {
}

Vector3::Vector3(float x, float y, float z) : x_(x), y_(y), z_(z) {
}
