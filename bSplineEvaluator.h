#pragma once
#pragma warning(disable : 4786)  

#include "curveevaluator.h"
class BSplineEvaluator :
    public CurveEvaluator {

public:
    void evaluateCurve(std::vector<Point>& ptvCtrlPts,
        std::vector<Point>& ptvEvaluatedCurvePts,
        const float& fAniLength,
        const bool& bWrap) const;

};

