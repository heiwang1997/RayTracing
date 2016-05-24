#include "basic.h"
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <cstring>


const double DOZ = 1e-6;
const double PI  = 3.1415926f;
const double INF = 1e30;
int dsx, dsy;

template <class T>
T max(const T& a, const T& b) {
    return (a > b) ? a : b;
}

template <class T>
T min(const T& a, const T& b) {
    return (a < b) ? a : b;
}

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
    return (x == 0 && y == 0) ? Vector3(1, 0, 0) : Vector3(y, -x, 0);
}

Vector3 Vector3::cross(const Vector3 &rv) const {
    return Vector3(y * rv.z - z * rv.y,
                   z * rv.x - x * rv.z,
                   x * rv.y - y * rv.x);
}

Vector3 Vector3::rotate(const Vector3 &axis, double angle) const {
    if (fabs(angle) < DOZ) return Vector3(x, y, z);
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

Vector3 Vector3::operator-() const {
    return Vector3(- x, - y, - z);
}

void Vector3::loadAttr(FILE *fp) {
    x = getAttrDouble(fp);
    y = getAttrDouble(fp);
    z = getAttrDouble(fp);
}

double &Vector3::operator[](int idx) {
    if (idx == 0) return x;
    if (idx == 1) return y;
    return z;
}

double Vector3::getAttr(int idx) const {
    if (idx == 0) return x;
    if (idx == 1) return y;
    return z;
}

Vector3::Vector3(const Color& col) {
    x = col.r; y = col.g; z = col.b;
}

Vector3 Vector3::operator+=(const Vector3 &rv) {
    x += rv.x;
    y += rv.y;
    z += rv.z;
    return *this;
}


void Color::dump() const {
    printf("Color( R=%lf, G=%lf, B=%lf )\n", r, g, b);
}

Color::Color() {
    r = 0; g = 0; b = 0;
}

Color::Color(double nr, double ng, double nb) {
    r = nr; g = ng; b = nb;
    confine();
}

Color Color::confine() {
    r = max(min(r, 255 - DOZ), DOZ);
    g = max(min(g, 255 - DOZ), DOZ);
    b = max(min(b, 255 - DOZ), DOZ);
    return *this;
}

Color Color::operator+(const Color &rv) const {
    return Color(r + rv.r, g + rv.g, b + rv.b).confine();
}

Color Color::operator+=(const Color &rv) {
    r = r + rv.r;
    g = g + rv.g;
    b = b + rv.b;
    confine();
    return *this;
}

Color Color::operator*(double ratio) const {
    return Color(ratio * r, ratio * g, ratio * b).confine();
}

Color Color::operator*(const Color &rv) const {
    return Color(r * rv.r / 255.0,
                 g * rv.g / 255.0,
                 b * rv.b / 255.0).confine();
}

Color operator*(double ratio, Color col) {
    return (col * ratio).confine();
}

void Color::loadAttr(FILE *fp) {
    r = getAttrDouble(fp);
    g = getAttrDouble(fp);
    b = getAttrDouble(fp);
}


Ray::Ray() : source(Vector3(0, 0, 0)), direction(Vector3(1, 0, 0)) {

}

Ray::Ray(const Vector3 &src, const Vector3 &dir)
  : source(src), direction(dir) { }

void Ray::dump() const {
    printf("Ray( sx=%lf, sy=%lf, sz=%lf )\n",
           source.x, source.y, source.z);
    printf(" & direct = ( %lf, %lf, %lf)\n",
           direction.x, direction.y, direction.z);
}

Ray Ray::getNormal() const {
    return Ray(source, direction.getNormal());
}


std::string getAttrName(FILE *fp) {
    char attr_res[20];
    char probe;
    fscanf_s(fp, "%*[^a-zA-Z}]");
    if (feof(fp)) return "EOF";
    fscanf_s(fp, "%c", &probe);
    if (probe == '}') return "END";
    fseek(fp, -1, SEEK_CUR);
    fscanf_s(fp, "%[^:]", attr_res);
    fscanf_s(fp, "%*[:{ ]");
    for (int i = 0; i < 20 && attr_res[i] != 0; ++ i)
        attr_res[i] = (char)toupper(attr_res[i]);
    return std::string(attr_res);
}

std::string getAttrString(FILE *fp) {
    char result[100];
    fscanf_s(fp, "\"%[^\"]\"", result);
    return std::string(result);
}

double getAttrDouble(FILE *fp) {
    double result;
    fscanf_s(fp, "%lf", &result);
    return result;
}

int getAttrInt(FILE* fp) {
    int result;
    fscanf_s(fp, "%d", &result);
    return result;
}

void generateCommentlessFile(std::string file_name) {
    FILE* fp, *fw;
    fopen_s(&fp, file_name.data(), "r");
    fopen_s(&fw, "temp_RayTracer.txt", "w");
    char line_buf[10240];
    while (fscanf_s(fp, "%[^/]", line_buf) != -1) {
        fwrite(line_buf, sizeof(char) * strlen(line_buf), 1, fw);
        fscanf_s(fp, "%*[^\n]");
    }
    fclose(fw);
    fclose(fp);
}

double random01() {
    return rand() / (double) RAND_MAX;
}
