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
    for (i = 0; i <= iCtrlPtCount - 4; i += 1) {
        Point b1 = ptvCtrlPts[i];
        Point b2 = ptvCtrlPts[i + 1];
        Point b3 = ptvCtrlPts[i + 2];
        Point b4 = ptvCtrlPts[i + 3];

        Eigen::Matrix<float, 4, 2> b_matrix;
        b_matrix << b1.x, b1.y,
            b2.x, b2.y,
            b3.x, b3.y,
            b4.x, b4.y;

        Eigen::Matrix<float, 4, 4> transformMatrix;
        transformMatrix << 1, 4, 1, 0,
            0, 4, 2, 0,
            0, 2, 4, 0,
            0, 1, 4, 1;

        Eigen::Matrix<float, 4, 2> v_matrix = (1.0f / 6.0f) * transformMatrix * b_matrix;

        Point p1 = { v_matrix(0, 0), v_matrix(0, 1) };
        Point p2 = { v_matrix(1, 0), v_matrix(1, 1) };
        Point p3 = { v_matrix(2, 0), v_matrix(2, 1) };
        Point p4 = { v_matrix(3, 0), v_matrix(3, 1) };

        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);

    }

    //std::cout << "i: " << i << std::endl;
    //std::cout << "iCtrlPtCount: " << iCtrlPtCount << std::endl;

    if (bWrap) {
        // wrapping: add phatom control point at the end with y of the first control point
        Eigen::Matrix<float, 4, 2> b_matrix;
        Eigen::Matrix<float, 4, 4> transformMatrix;
        Eigen::Matrix<float, 4, 2> v_matrix;
        Point b1, b2, b3, b4, p1, p2, p3, p4;

        // segment 1
        b1 = ptvCtrlPts[i];
        b2 = ptvCtrlPts[i + 1];
        b3 = ptvCtrlPts[i + 2];
        b4 = ptvCtrlPts[0];
        b4.x += fAniLength;


        
        b_matrix << b1.x, b1.y,
            b2.x, b2.y,
            b3.x, b3.y,
            b4.x, b4.y;

        
        transformMatrix << 1, 4, 1, 0,
            0, 4, 2, 0,
            0, 2, 4, 0,
            0, 1, 4, 1;

        v_matrix = (1.0f / 6.0f) * transformMatrix * b_matrix;

        p1 = { v_matrix(0, 0), v_matrix(0, 1) };
        p2 = { v_matrix(1, 0), v_matrix(1, 1) };
        p3 = { v_matrix(2, 0), v_matrix(2, 1) };
        p4 = { v_matrix(3, 0), v_matrix(3, 1) };

        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);

        for (int k = 0; k < ptvEvaluatedCurvePts.size(); k++) {
            if (ptvEvaluatedCurvePts[k].x > fAniLength) {
                ptvEvaluatedCurvePts[k].x -= fAniLength;
            }
        }

        // segment 2
        b1 = ptvCtrlPts[i + 1];
        b2 = ptvCtrlPts[i + 2];
        b3 = ptvCtrlPts[0];
        b3.x += fAniLength;
        b4 = ptvCtrlPts[1];
        b4.x += fAniLength;

        b_matrix << b1.x, b1.y,
            b2.x, b2.y,
            b3.x, b3.y,
            b4.x, b4.y;

        transformMatrix << 1, 4, 1, 0,
            0, 4, 2, 0,
            0, 2, 4, 0,
            0, 1, 4, 1;

        v_matrix = (1.0f / 6.0f) * transformMatrix * b_matrix;

        p1 = { v_matrix(0, 0), v_matrix(0, 1) };
        p2 = { v_matrix(1, 0), v_matrix(1, 1) };
        p3 = { v_matrix(2, 0), v_matrix(2, 1) };
        p4 = { v_matrix(3, 0), v_matrix(3, 1) };

        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);

        for (int k = 0; k < ptvEvaluatedCurvePts.size(); k++) {
            if (ptvEvaluatedCurvePts[k].x > fAniLength) {
                ptvEvaluatedCurvePts[k].x -= fAniLength;
            }
        }

        // segment 3
        b1 = ptvCtrlPts[i + 2];
        b2 = ptvCtrlPts[0];
        b2.x += fAniLength;
        b3 = ptvCtrlPts[1];
        b3.x += fAniLength;
        b4 = ptvCtrlPts[2];
        b4.x += fAniLength;

        b_matrix << b1.x, b1.y,
            b2.x, b2.y,
            b3.x, b3.y,
            b4.x, b4.y;

        transformMatrix << 1, 4, 1, 0,
            0, 4, 2, 0,
            0, 2, 4, 0,
            0, 1, 4, 1;

        v_matrix = (1.0f / 6.0f) * transformMatrix * b_matrix;

        p1 = { v_matrix(0, 0), v_matrix(0, 1) };
        p2 = { v_matrix(1, 0), v_matrix(1, 1) };
        p3 = { v_matrix(2, 0), v_matrix(2, 1) };
        p4 = { v_matrix(3, 0), v_matrix(3, 1) };

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

}
