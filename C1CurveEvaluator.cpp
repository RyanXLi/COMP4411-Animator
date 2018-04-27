#include "C1CurveEvaluator.h"
#include <Eigen/Dense>
#include <iostream>
#include <vector>


#include "BezierCurveEvaluator.h"
#include "CatmullRomEvaluator.h"
#include "modelerapp.h"
#include "modelerglobals.h"


void C1CurveEvaluator::evaluateCurve(
    std::vector<Point>& ptvCtrlPts, 
    std::vector<Point>& ptvEvaluatedCurvePts, 
    const float & fAniLength, 
    const bool & bWrap) const {

    if (ptvCtrlPts.size() < 4) {
        BezierCurveEvaluator* evaluator = new BezierCurveEvaluator();
        evaluator->evaluateCurve(ptvCtrlPts, ptvEvaluatedCurvePts, fAniLength,
            bWrap);
        return;
    }

    ModelerApplication::Instance()->isDrawingCatmull = FALSE;
    int iCtrlPtCount = ptvCtrlPts.size();
    int lastIndex = ptvCtrlPts.size() - 1;
    ptvEvaluatedCurvePts.clear();
    float tension = ModelerApplication::Instance()->tension;
    Point b1, b2, b3, b4, p1, p2, p3, p4;
    //for (int i = 0; i < iCtrlPtCount; i++) {
    //    std::cout << i << "th: " << ptvCtrlPts[i].x << ", " << ptvCtrlPts[i].y << std::endl;
    //}


    // 1. determine action
    int mAction = NO_CHANGE;
    int indexPointChanged = -1;

    determineAction(ptvCtrlPts, mAction, indexPointChanged);


    //// 2. handle action
    //if (mAction == DRAG_CATMULL) {
    //    ModelerApplication::Instance()->catmullPoints.clear();
    //    for (int i = 0; i < ptvCtrlPts.size(); i += 2)
    //        ModelerApplication::Instance()->catmullPoints.push_back(ptvCtrlPts[i]);
    //
    //    ptvCtrlPts = ModelerApplication::Instance()->catmullPoints;
    //    CatmullRomEvaluator* evaluator = new CatmullRomEvaluator();
    //    evaluator->evaluateCurve(ptvCtrlPts, ptvEvaluatedCurvePts, fAniLength,
    //        bWrap);
    //}
    //else if (mAction == ADD) {
    //    
    //}
    //else {
    //    // other situations      
    //    // DRAG_BEZIER
    //
    //}

    for (int i = 0; i < ptvCtrlPts.size() - 3; i += 2) {
        p1 = ptvCtrlPts[i];
        p2 = ptvCtrlPts[i + 1];
        p3 = ptvCtrlPts[i + 2] - (ptvCtrlPts[i + 3] - ptvCtrlPts[i + 2]);
        p4 = ptvCtrlPts[i + 2];
        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);
    }


    // 3, wrap
    if (bWrap) {
        // wrapping: add phatom control point at the end with y of the first control point
        // segment connecting tail and head
        p1 = ptvCtrlPts[lastIndex-1];
        p2 = ptvCtrlPts[lastIndex];
        p3 = ptvCtrlPts[0] + ptvCtrlPts[0] - ptvCtrlPts[1];
        p3.x += fAniLength;
        p4 = ptvCtrlPts[0];
        p4.x += fAniLength;
        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);

        for (int k = 0; k < ptvEvaluatedCurvePts.size(); k++) {
            if (ptvEvaluatedCurvePts[k].x > fAniLength) {
                ptvEvaluatedCurvePts[k].x -= fAniLength;
            }
        }
    }
    else {
        ptvEvaluatedCurvePts.push_back({ fAniLength, ptvCtrlPts[ptvCtrlPts.size() - 2].y });
        ptvEvaluatedCurvePts.push_back({ 0, ptvCtrlPts[0].y });
    }

    // 4. store a copy of ctrlPts

    ModelerApplication::Instance()->prevC1Points = ptvCtrlPts;
}


void C1CurveEvaluator::determineAction(const std::vector<Point>& ptvCtrlPts, int& action, int& indexPointChanged) const {
    if (ptvCtrlPts.size() > ModelerApplication::Instance()->prevC1Points.size()) {
        action = ADD;
        for (int i = 0; i < ptvCtrlPts.size(); i++) {
            if (std::find(ModelerApplication::Instance()->prevC1Points.begin(),
                ModelerApplication::Instance()->prevC1Points.end(),
                ptvCtrlPts[i]) == ModelerApplication::Instance()->prevC1Points.end()) {
                indexPointChanged = i;
            }
        }
        return;
    }
    else if (ptvCtrlPts.size() < ModelerApplication::Instance()->prevC1Points.size()) {
        for (int i = 0; i < ModelerApplication::Instance()->prevC1Points.size(); i++) {
            if (std::find(ptvCtrlPts.begin(),
                ptvCtrlPts.end(),
                ModelerApplication::Instance()->prevC1Points[i]) == ModelerApplication::Instance()->prevC1Points.end()) {
                indexPointChanged = i;
            }
        }
        if (indexPointChanged % 2 == 0) {
            action = REMOVE_CATMULL;
        }
        else {
            action = REMOVE_BEZIER;
        }
    }
    else {
        // Now `ptvCtrlPts.size() == ModelerApplication::Instance()->prevC1Points.size()`
        for (int i = 0; i < ptvCtrlPts.size(); i++) {
            if (std::find(ModelerApplication::Instance()->prevC1Points.begin(),
                ModelerApplication::Instance()->prevC1Points.end(),
                ptvCtrlPts[i]) == ModelerApplication::Instance()->prevC1Points.end()) {
                indexPointChanged = i;
            }
        }
        if (indexPointChanged % 2 == 0) {
            action = DRAG_CATMULL;
        }
        else {
            action = DRAG_BEZIER;
        }
    }
}
