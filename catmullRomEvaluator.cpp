#include "CatmullRomEvaluator.h"
#include <Eigen/Dense>
#include <iostream>
#include <vector>


#include "BezierCurveEvaluator.h"
#include "modelerapp.h"
#include "modelerglobals.h"


void CatmullRomEvaluator::evaluateCurve(std::vector<Point>& ptvCtrlPts,
    std::vector<Point>& ptvEvaluatedCurvePts,
    const float& fAniLength,
    const bool& bWrap) const {

    if (ptvCtrlPts.size() < 4) {
        BezierCurveEvaluator* evaluator = new BezierCurveEvaluator();
        evaluator->evaluateCurve(ptvCtrlPts, ptvEvaluatedCurvePts, fAniLength,
            bWrap);
        return;
    }

    ModelerApplication::Instance()->isDrawingCatmull = TRUE;
    int iCtrlPtCount = ptvCtrlPts.size();
    int lastIndex = ptvCtrlPts.size() - 1;
    float tension = ModelerApplication::Instance()->tension;
    //std::cout << tension << std::endl;

    ptvEvaluatedCurvePts.clear();

    //for (int i = 0; i < iCtrlPtCount; i++) {
    //    std::cout << i << "th: " << ptvCtrlPts[i].x << ", " << ptvCtrlPts[i].y << std::endl;
    //}

    Point b1, b2, b3, b4, p1, p2, p3, p4;

    //if (!ModelerApplication::Instance()->catmullDragged) {

        ModelerApplication::Instance()->bezierPoints.clear();

        // first segment(special)
        p1 = ptvCtrlPts[0];
        p2 = ptvCtrlPts[0] + ptvCtrlPts[1] - ptvCtrlPts[0];
        p3 = ptvCtrlPts[1] - ((ptvCtrlPts[2] - ptvCtrlPts[0]) * (tension / 3.0f));
        p4 = ptvCtrlPts[1];
        //addToBezierPoints(p1, p2, p3, p4);
        addToBezierPoints(p2);
        addToBezierPoints(p4+p4-p3);
        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);


        // normal segments
        int i;
        for (i = 0; i <= iCtrlPtCount - 4; i += 1) {
            b1 = ptvCtrlPts[i];
            b2 = ptvCtrlPts[i + 1];
            b3 = ptvCtrlPts[i + 2];
            b4 = ptvCtrlPts[i + 3];

            p1 = b2;
            p2 = b2 + ((b3 - b1) * (tension / 3.0f));
            p3 = b3 - ((b4 - b2) * (tension / 3.0f));
            p4 = b3;

            //std::cout << "p1: " << p1 << std::endl;
            //std::cout << "p2: " << p2 << std::endl;
            //std::cout << "p3: " << p3 << std::endl;
            //std::cout << "p4: " << p4 << std::endl;
            //std::cout <<  std::endl;

            //addToBezierPoints(p1, p2, p3, p4);
            addToBezierPoints(p4 + p4 - p3);
            BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);

        }



        // last segment(special)
        p1 = ptvCtrlPts[lastIndex - 1];
        p2 = ptvCtrlPts[lastIndex - 1] + ((ptvCtrlPts[lastIndex] - ptvCtrlPts[lastIndex - 2]) * (tension / 3.0f));
        p3 = ptvCtrlPts[lastIndex] - (ptvCtrlPts[lastIndex] - ptvCtrlPts[lastIndex - 1]);
        p4 = ptvCtrlPts[lastIndex];
        //addToBezierPoints(p1, p2, p3, p4);
        addToBezierPoints(p4 + p4 - p3);
        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);

        //std::cout << "i: " << i << std::endl;
        //std::cout << "iCtrlPtCount: " << iCtrlPtCount << std::endl;
    //}

    if (bWrap) {
        // TODO
        // wrapping: add phatom control point at the end with y of the first control point
        // segment connecting tail and head
    
        b1 = ptvCtrlPts[lastIndex - 1];
        b2 = ptvCtrlPts[lastIndex];
        b3 = ptvCtrlPts[0];
        b3.x += fAniLength;
        b4 = ptvCtrlPts[1];
        b4.x += fAniLength;
    
        p1 = b2;
        p2 = b2 + ((b3 - b1) * (tension / 3.0f));
        p3 = b3 - ((b4 - b2) * (tension / 3.0f));
        p4 = b3;
        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);
    
        for (int k = 0; k < ptvEvaluatedCurvePts.size(); k++) {
            if (ptvEvaluatedCurvePts[k].x > fAniLength) {
                ptvEvaluatedCurvePts[k].x -= fAniLength;
            }
        }
    
    }
    else {
    
        ptvEvaluatedCurvePts.push_back({ fAniLength, ptvCtrlPts[ptvCtrlPts.size() - 1].y });
        ptvEvaluatedCurvePts.push_back({ 0, ptvCtrlPts[0].y });
    }

    //// evaluate points in ModelerApplication::Instance()->bezierPoints
    //for (int i = 0; i < ModelerApplication::Instance()->bezierPoints.size() - 3; i += 4) {
    //    p1 = ModelerApplication::Instance()->bezierPoints[i];
    //    p2 = ModelerApplication::Instance()->bezierPoints[i + 1];
    //    p3 = ModelerApplication::Instance()->bezierPoints[i + 2];
    //    p4 = ModelerApplication::Instance()->bezierPoints[i + 3];
    //    BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);
    //}

}


void CatmullRomEvaluator::addToBezierPoints(Point p1, Point p2, Point p3, Point p4) const {
    ModelerApplication::Instance()->bezierPoints.push_back(p1);
    ModelerApplication::Instance()->bezierPoints.push_back(p2);
    ModelerApplication::Instance()->bezierPoints.push_back(p3);
    ModelerApplication::Instance()->bezierPoints.push_back(p4);
}


void CatmullRomEvaluator::addToBezierPoints(Point p3) const {

    ModelerApplication::Instance()->bezierPoints.push_back(p3);

}