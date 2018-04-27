#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#pragma warning(disable : 4786)

#include <functional>
#include <iostream>
#include <cmath>

#define ERR 0.00001

//using namespace std;

class Point
{
public:

	Point(void);
	Point(const float& x, const float& y);

	void toStream(std::ostream& output_stream) const;
	void fromStream(std::istream& input_stream);

	float distance(const Point& p) const {
		float xd = x - p.x;
		float yd = y - p.y;
		return sqrtf(xd * xd + yd * yd);
	}

	float x;
	float y;

    Point operator+(Point p2) {
        return{ this->x + p2.x, this->y + p2.y };
    }

    Point operator-(Point p2) {
        return{ this->x - p2.x, this->y - p2.y };
    }

    Point operator+(const Point p2) const {
        return{ this->x + p2.x, this->y + p2.y };
    }

    Point operator-(const Point p2) const {
        return{ this->x - p2.x, this->y - p2.y };
    }

    Point operator-() {
        return{ -(this->x), - (this->y) };
    }

    Point operator*(float f) {
        return{ this->x * f, this->y * f };
    }

    bool operator==(const Point p2) const {
        return{
            fabs(this->x - p2.x) <= ERR
            && fabs(this->y - p2.y) <= ERR
        };
    }
};

std::ostream& operator<<(std::ostream& output_stream, const Point& point);
std::istream& operator>>(std::istream& input_stream, Point& point);





class PointSmallerXCompare : public std::binary_function<const Point&, const Point&, bool>
{
public:
	bool operator()(const Point& first, const Point& second) const;
};

class PointLargerXCompare : public std::binary_function<const Point&, const Point&, bool>
{
public:
	bool operator()(const Point& first, const Point& second) const;
};



class Point3 {
public:
    Point3(void) {}
    Point3(const float& new_x, const float& new_y, const float& new_z) :x(new_x),
        y(new_y), z(new_z) {}
    //void toStream(std::ostream& output_stream) const {
    //    
    //        output_stream << x << ", ";
    //        output_stream << y << ", ";
    //        output_stream << z << std::endl;
    //}
    float x = 0;
    float y = 0;
    float z = 0;
};
//std::ostream& operator<<(std::ostream& output_stream, const Point3& point) {
//    point.toStream(output_stream);
//
//    return output_stream;
//}

#endif // POINT_H_INCLUDED
