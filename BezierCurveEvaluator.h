#pragma once
#pragma warning(disable : 4786)  

#include "curveevaluator.h"
class BezierCurveEvaluator :
    public CurveEvaluator {
public:
    void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
        std::vector<Point>& ptvEvaluatedCurvePts,
        const float& fAniLength,
        const bool& bWrap) const;
    static void drawBezierSegment(Point p1, Point p2, Point p3, Point p4, std::vector<Point>& ptvEvaluatedCurvePts);
};

