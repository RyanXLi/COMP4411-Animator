#pragma once
#include "curveevaluator.h"
class C2CurveEvaluator :
    public CurveEvaluator {
public:
    void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
        std::vector<Point>& ptvEvaluatedCurvePts,
        const float& fAniLength,
        const bool& bWrap) const;
};

