#include "basic.h"
#include <cmath>
#include <cstdio>

Vector3 Vector3::operator+ (const Vector3& rv) const {
    return Vector3(x + rv.x, y + rv.y, z + rv.z);
}

Vector3 Vector3::operator- (const Vector3& rv) const {
    return Vector3(x - rv.x, y - rv.y, z - rv.z);
}

double Vector3::operator* (const Vector3& rv) const {
    return x * rv.x + y * rv.y + z * rv.z;
}

Vector3 Vector3::operator* (double mul) const {
    return Vector3(mul * x, mul * y, mul * z);
}

Vector3 operator* (double mul, Vector3 rv) {
    return rv * mul;
}

Vector3 Vector3::getNormal() const {
    double l = getLength();
    if (l != 0) return Vector3(x / l, y / l, z / l);
    else return Vector3();
}

double Vector3::getLength() const {
    return std::sqrt(x * x + y * y + z * z);
}

void Vector3::dump() const {
    printf("Vec( x=%lf, y=%lf, z=%lf )\n", x, y, z);
}

Vector3::Vector3() {
    x = 0; y = 0; z = 0;
}

Vector3::Vector3(double nx, double ny, double nz) {
    x = nx; y = ny; z = nz;
}

Vector3 Vector3::getVertical() const {
    if (z == 0)
        return Vector3(0, 0, 1);
    else
        return Vector3(1, 0, - x / z);
}

Vector3 Vector3::cross(const Vector3 &rv) const {
    return Vector3(y * rv.z - z * rv.y,
                   z * rv.x - x * rv.z,
                   x * rv.y - y * rv.x);
}

Vector3 Vector3::rotate(const Vector3 &axis, double angle) const {
    Vector3 ret;
    double cost = cos( angle );
    double sint = sin( angle );
    ret.x += x * ( axis.x * axis.x + ( 1 - axis.x * axis.x ) * cost );
    ret.x += y * ( axis.x * axis.y * ( 1 - cost ) - axis.z * sint );
    ret.x += z * ( axis.x * axis.z * ( 1 - cost ) + axis.y * sint );
    ret.y += x * ( axis.y * axis.x * ( 1 - cost ) + axis.z * sint );
    ret.y += y * ( axis.y * axis.y + ( 1 - axis.y * axis.y ) * cost );
    ret.y += z * ( axis.y * axis.z * ( 1 - cost ) - axis.x * sint );
    ret.z += x * ( axis.z * axis.x * ( 1 - cost ) - axis.y * sint );
    ret.z += y * ( axis.z * axis.y * ( 1 - cost ) + axis.x * sint );
    ret.z += z * ( axis.z * axis.z + ( 1 - axis.z * axis.z ) * cost );
    return ret;
}


void Color::dump() const {
    printf("Color( R=%lf, G=%lf, B=%lf )\n", r, g, b);
}

Color::Color() {
    r = 0; g = 0; b = 0;
}

Color::Color(double nr, double ng, double nb) {
    r = nr; g = ng; b = nb;
}

Ray::Ray() : source(Vector3(0, 0, 0)), direction(Vector3(1, 0, 0)) {

}

Ray::Ray(const Vector3 &src, const Vector3 &dir)
  : source(src), direction(dir) { }

void Ray::dump() {
    printf("Ray( sx=%lf, sy=%lf, sz=%lf )\n",
           source.x, source.y, source.z);
    printf(" & direct = ( %lf, %lf, %lf)\n",
           direction.x, direction.y, direction.z);
}




