#include "bSplineEvaluator.h"
#include <Eigen/Dense>
#include <iostream>
#include "BezierCurveEvaluator.h"

void BSplineEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
    std::vector<Point>& ptvEvaluatedCurvePts,
    const float& fAniLength,
    const bool& bWrap) const {
    int iCtrlPtCount = ptvCtrlPts.size();

    // evaluate evaluatedPoints
    //ptvEvaluatedCurvePts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
    ptvEvaluatedCurvePts.clear();

    //for (int i = 0; i < iCtrlPtCount; i++) {
    //    std::cout << i << "th: " << ptvCtrlPts[i].x << ", " << ptvCtrlPts[i].y << std::endl;
    //}

    int i;
    for (i = 0; i <= iCtrlPtCount - 4; i += 3) {
        Point p1 = ptvCtrlPts[i];
        Point p2 = ptvCtrlPts[i + 1];
        Point p3 = ptvCtrlPts[i + 2];
        Point p4 = ptvCtrlPts[i + 3];

        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);

    }

    if (bWrap && iCtrlPtCount - i == 3) {
        // if we only need 1 extra pt to get a bezier curve
        Point p1 = ptvCtrlPts[i];
        Point p2 = ptvCtrlPts[i + 1];
        Point p3 = ptvCtrlPts[i + 2];
        Point p4 = ptvCtrlPts[0];
        p4.x += fAniLength;

        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);

        for (int k = 0; k < ptvEvaluatedCurvePts.size(); k++) {
            if (ptvEvaluatedCurvePts[k].x > fAniLength) {
                ptvEvaluatedCurvePts[k].x -= fAniLength;
            }
        }
    }
    else {

        // use line curve
        for (int j = 0; j < iCtrlPtCount - i; j++) {
            ptvEvaluatedCurvePts.push_back(ptvCtrlPts[i + j]);
        }

        float x = 0.0;
        float y1;

        if (bWrap) {
            // if wrapping is on, interpolate the y value at xmin and
            // xmax so that the slopes of the lines adjacent to the
            // wraparound are equal.

            if ((ptvCtrlPts[0].x + fAniLength) - ptvCtrlPts[iCtrlPtCount - 1].x > 0.0f) {
                y1 = (ptvCtrlPts[0].y * (fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x) +
                    ptvCtrlPts[iCtrlPtCount - 1].y * ptvCtrlPts[0].x) /
                    (ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[iCtrlPtCount - 1].x);

            }
            else
                y1 = ptvCtrlPts[0].y;


        }
        else {
            // if wrapping is off, make the first and last segments of
            // the curve horizontal.

            y1 = ptvCtrlPts[0].y;
        }

        ptvEvaluatedCurvePts.push_back(Point(x, y1));

        // set the endpoint based on the wrap flag.
        float y2;
        x = fAniLength;
        if (bWrap)
            y2 = y1;
        else
            y2 = ptvCtrlPts[iCtrlPtCount - 1].y;

        ptvEvaluatedCurvePts.push_back(Point(x, y2));
    }
}
