#pragma once
#include "curveevaluator.h"
class C1CurveEvaluator :
    public CurveEvaluator {

    void evaluateCurve(std::vector<Point>& ptvCtrlPts,
        std::vector<Point>& ptvEvaluatedCurvePts,
        const float& fAniLength,
        const bool& bWrap) const;

    void determineAction(const std::vector<Point>& ptvCtrlPts, int & action, int & indexPointChanged) const;

    enum action {
        NO_CHANGE,
        DRAG_CATMULL,
        DRAG_BEZIER,
        ADD,
        REMOVE_CATMULL,
        REMOVE_BEZIER,
        ACTION_NUM
    };
};

