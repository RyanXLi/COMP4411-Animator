#include "C2CurveEvaluator.h"
#include <Eigen/Dense>
#include <iostream>
#include <vector>


#include "BezierCurveEvaluator.h"
#include "modelerapp.h"
#include "modelerglobals.h"



void C2CurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
    std::vector<Point>& ptvEvaluatedCurvePts,
    const float& fAniLength,
    const bool& bWrap) const {
    ptvEvaluatedCurvePts.clear();
    int iCtrlPtCount = ptvCtrlPts.size();

    if (ptvCtrlPts.size() < 4) {
        BezierCurveEvaluator* evaluator = new BezierCurveEvaluator();
        evaluator->evaluateCurve(ptvCtrlPts, ptvEvaluatedCurvePts, fAniLength,
            bWrap);
        return;
    }

    // m * dVec = cVec
    // 1. prepare m
    Eigen::MatrixXf m(iCtrlPtCount, iCtrlPtCount);
    m.setZero();

    m(0, 0) = 2;
    m(0, 1) = 1;
    for (int i = 1; i < iCtrlPtCount-1; i++) {
        m(i, i - 1) = 1;
        m(i, i)     = 4;
        m(i, i + 1) = 1;
    }
    m(iCtrlPtCount - 1, iCtrlPtCount - 2) = 1;
    m(iCtrlPtCount - 1, iCtrlPtCount - 1) = 2;

    Eigen::MatrixXf m_inverse = m.inverse();

    // 2. prepare cVec
    Eigen::MatrixXf cVec(iCtrlPtCount, 2);
    cVec(0, 0) = ((ptvCtrlPts[1] - ptvCtrlPts[0]) * 3.0f).x;
    cVec(0, 1) = ((ptvCtrlPts[1] - ptvCtrlPts[0]) * 3.0f).y;
    for (int i = 1; i < iCtrlPtCount-1; i++) {
        cVec(i, 0) = ((ptvCtrlPts[i + 1] - ptvCtrlPts[i - 1]) * 3.0f).x;
        cVec(i, 1) = ((ptvCtrlPts[i + 1] - ptvCtrlPts[i - 1]) * 3.0f).y;
    }
    cVec(iCtrlPtCount - 1, 0) = ((ptvCtrlPts[iCtrlPtCount - 1] - ptvCtrlPts[iCtrlPtCount - 2]) * 3.0f).x;
    cVec(iCtrlPtCount - 1, 1) = ((ptvCtrlPts[iCtrlPtCount - 1] - ptvCtrlPts[iCtrlPtCount - 2]) * 3.0f).y;

    // 3. prepare dVec
    //std::cout << "cVec: " << cVec << std::endl;
    //std::cout << "m_inverse: " << m_inverse << std::endl;

    Eigen::MatrixXf dVec = m_inverse * cVec;

    //std::cout << "dVec: " << dVec << std::endl;

    // 4. draw
    Point p1, p2, p3, p4;
    for (int i = 0; i < iCtrlPtCount - 1; i+=1) {
        p1 = ptvCtrlPts[i];
        p2 = ptvCtrlPts[i] + Point(dVec(i, 0), dVec(i, 1)) * (1 / 3.0f);
        p3 = ptvCtrlPts[i + 1] - Point(dVec(i + 1, 0), dVec(i + 1, 1) )* (1 / 3.0f);
        p4 = ptvCtrlPts[i + 1];

        //std::cout << "p1: " << p1 << std::endl;
        //std::cout << "p2: " << p2 << std::endl;
        //std::cout << "p3: " << p3 << std::endl;
        //std::cout << "p4: " << p4 << std::endl;
        //std::cout <<  std::endl;

        BezierCurveEvaluator::drawBezierSegment(p1, p2, p3, p4, ptvEvaluatedCurvePts);
    }

    // 5. wrap
    if (bWrap) {
        // TODO
        // wrapping: add phatom control point at the end with y of the first control point
        // segment connecting tail and head

        int lastIndex = iCtrlPtCount - 1;

        p1 = ptvCtrlPts[lastIndex];
        p2 = ptvCtrlPts[lastIndex] + Point(dVec(lastIndex, 0), dVec(lastIndex, 1)) * (1 / 3.0f);

        p4 = ptvCtrlPts[0];
        p4.x += fAniLength;

        p3 = p4 - Point(dVec(0, 0), dVec(0, 1))* (1 / 3.0f);

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