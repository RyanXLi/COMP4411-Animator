#pragma once
#pragma warning(disable : 4786)  

#include "curveevaluator.h"
class CatmullRomEvaluator :
    public CurveEvaluator {
public:


    void evaluateCurve(std::vector<Point>& ptvCtrlPts,
        std::vector<Point>& ptvEvaluatedCurvePts,
        const float& fAniLength,
        const bool& bWrap) const;
    void addToBezierPoints(Point p1, Point p2, Point p3, Point p4) const;
    void addToBezierPoints(Point p3) const;
};


