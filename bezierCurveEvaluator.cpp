#include "BezierCurveEvaluator.h"
#include <Eigen/Dense>
#include <iostream>
#include "modelerapp.h"
#include "modelerglobals.h"


void BezierCurveEvaluator::evaluateCurve(std::vector<Point>& ptvCtrlPts,
    std::vector<Point>& ptvEvaluatedCurvePts,
    const float& fAniLength,
    const bool& bWrap) const {
    int iCtrlPtCount = ptvCtrlPts.size();
    ModelerApplication::Instance()->bezierPoints.clear();
    ModelerApplication::Instance()->isDrawingCatmull = FALSE;

    // evaluate evaluatedPoints
    //ptvEvaluatedCurvePts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
    ptvEvaluatedCurvePts.clear();

    //for (int i = 0; i < iCtrlPtCount; i++) {
    //    std::cout << i << "th: " << ptvCtrlPts[i].x << ", " << ptvCtrlPts[i].y << std::endl;
    //}

    std::cout << "adaptive:"<<ModelerApplication::Instance()->adaptive << std::endl;
    
    int i;
    for (i = 0; i <= iCtrlPtCount - 4; i+=3) {
        Point p1 = ptvCtrlPts[i];
        Point p2 = ptvCtrlPts[i+1];
        Point p3 = ptvCtrlPts[i+2];
        Point p4 = ptvCtrlPts[i+3];

        if (ModelerApplication::Instance()->adaptive) {
            int ptCountBefore = ptvEvaluatedCurvePts.size();
            drawBezierSegmentAdaptively(p1, p2, p3, p4, ptvEvaluatedCurvePts);
            int ptCountAfter = ptvEvaluatedCurvePts.size();
            std::cout << "This segment generates " 
                << ptCountAfter - ptCountBefore << " points." << std::endl;
        }
        else {
            drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);
        }

    }

    if (bWrap && iCtrlPtCount - i == 3) {
        // if we only need 1 extra pt to get a bezier curve
        Point p1 = ptvCtrlPts[i];
        Point p2 = ptvCtrlPts[i + 1];
        Point p3 = ptvCtrlPts[i + 2];
        Point p4 = ptvCtrlPts[0];
        p4.x += fAniLength;

        if (ModelerApplication::Instance()->adaptive) {
            int ptCountBefore = ptvEvaluatedCurvePts.size();
            drawBezierSegmentAdaptively(p1, p2, p3, p4, ptvEvaluatedCurvePts);
            int ptCountAfter = ptvEvaluatedCurvePts.size();
            std::cout << "This segment generates "
                << ptCountAfter - ptCountBefore << " points." << std::endl;
        }
        else {
            drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);
        }

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



void BezierCurveEvaluator::drawBezierSegment(Point p1, Point p2, Point p3, Point p4,
    std::vector<Point>& ptvEvaluatedCurvePts) {

    float step = STEP;
    Eigen::Matrix<float, 4, 4> m_b;
    m_b << -1, 3, -3, 1,
        3, -6, 3, 0,
        -3, 3, 0, 0,
        1, 0, 0, 0;

    Eigen::Matrix<float, 4, 2> g_b;

    g_b << p1.x, p1.y,
        p2.x, p2.y,
        p3.x, p3.y,
        p4.x, p4.y;

    Eigen::Matrix<float, 1, 4> m_t;
    Eigen::Matrix<float, 1, 2> m_point;

    for (int j = 0; j < 1 / step; ++j) {
        float t = j * step;
        m_t << t*t*t, t*t, t, 1;
        m_point = m_t * m_b * g_b;

        Point point = { m_point(0,0), m_point(0,1) };
        ptvEvaluatedCurvePts.push_back(point);
    }
}


bool BezierCurveEvaluator::isFlatEnough(Point p1, Point p2, Point p3, Point p4)
{
    //std::cout << VAL(FLAT_THRES) << std::endl;
    return ((p1.distance(p2) + p2.distance(p3) + p3.distance(p4)) / p1.distance(p4) < 1 + ModelerApplication::Instance()->flatThreshold);
}
void BezierCurveEvaluator::drawBezierSegmentAdaptively(Point p1, Point p2, Point p3, Point p4, std::vector<Point>& ptvEvaluatedCurvePts)
 {
    if (isFlatEnough(p1, p2, p3, p4)) {
        ptvEvaluatedCurvePts.push_back(p1);
        ptvEvaluatedCurvePts.push_back(p4);
    }
    else{
        Point level1[3];
        Point level2[2];

        level1[0] = Point((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
        level1[1] = Point((p2.x + p3.x) / 2, (p2.y + p3.y) / 2);
        level1[2] = Point((p3.x + p4.x) / 2, (p3.y + p4.y) / 2);
        
        level2[0] = Point((level1[0].x + level1[1].x) / 2, (level1[0].y + level1[1].y) / 2);
        level2[1] = Point((level1[1].x + level1[2].x) / 2, (level1[1].y + level1[2].y) / 2);

        Point q((level2[0].x + level2[1].x) / 2, (level2[0].y + level2[1].y) / 2);

        drawBezierSegmentAdaptively(p1, level1[0], level2[0], q, ptvEvaluatedCurvePts);
        drawBezierSegmentAdaptively(q, level2[1], level1[2], p4, ptvEvaluatedCurvePts);
    }

}