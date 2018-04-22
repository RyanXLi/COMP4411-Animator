#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "camera.h"
#include <FL/gl.h>
#include "bitmap.h"
#include <math.h>
#include <vector>
#include "modelerglobals.h"

class camera;
// To make a DoggModel, we inherit off of ModelerView
class DoggModel : public ModelerView 
{
public:
    DoggModel(int x, int y, int w, int h, char *label) 
        : ModelerView(x,y,w,h,label) { }

    virtual void draw();
    void init();
	std::vector<char> gsentence(const int stage);
	void drawLsystem(const std::vector<char> & sentence);
    void drawWing(bool isRightWing);
    void drawAxis();
    void resetLeg();
	void frameAll(float dx, float dy, float dz);
    void animate(int index);
    void moveHead();
    void moveTail();
    bool checkLegParam();
    void animateWing();


    unsigned char * texImg;
    int texImgW = 256;
    int texImgH = 256;
    GLuint tex = 0;
    bool firstTime = 1;
    bool shouldMoveHead = 0;
    bool shouldMoveTail = 0;
    int headBoundary = 45;
    int tailBoundary = 45;
    int headBoundaryCount = 0;
    int tailBoundaryCount = 0;
    int headIncr = 1;
    int tailIncr = 1;
    bool hasDiffLegParam = 0;
    double wingIncr = 1;

    double anim_incr [12] = {
        5, 5, 5,
        -5, -5, -5,
        5, 5, 5,
        -5, -5, -5
    };

    double anim_min [12] = {
        -45, -45, -45,
        -45, -45, -45,
        -45, -45, -45,
        -45, -45, -45
    };

    double anim_max [12] = {
        45, 45, 45,
        45, 45, 45,
        45, 45, 45,
        45, 45, 45
    };

    double last_angle[12] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };
    
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createDoggModel(int x, int y, int w, int h, char *label)
{ 
    return new DoggModel(x,y,w,h,label); 
}

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out DoggModel
void DoggModel::draw()
{

    if (firstTime) {
        init();
        firstTime = 0;
    }

    // This call takes care of a lot of the nasty projection 
    // matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...

	if (VAL(FRAME_ALL)) { 
		DoggModel::frameAll(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
	}
	
	ModelerView::draw();

    if (VAL(RESET_LEG)) { DoggModel::resetLeg(); }

    hasDiffLegParam = checkLegParam();

	// draw the dogg model

	setAmbientColor(.1f,.1f,.1f);
	setDiffuseColor(1.0f, 1.0f, 1.0f);

	if (VAL(IK_SWITCH)) {
		glPushMatrix();
		glTranslatef(VAL(IK_X), VAL(IK_Y), VAL(IK_Z));
		drawSphere(0.15);
		glPopMatrix();
	}

	Vec3f vIK(VAL(IK_X), VAL(IK_Y), VAL(IK_Z));

	glPushMatrix();
	glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));

        // part 1: torso
		glPushMatrix();
		    glTranslated(-2, 0, -1);
            glScaled(2, 3, 2);
		    drawTextureBox(1,1,1);
		glPopMatrix();


        // part 2: torso
        glPushMatrix();
            glTranslated(0, 1.5, -1);
            glRotated(-90, 0.0, 0.0, 1.0);
            glScaled(1.5, 4.5, 2);
            //drawTriangularPrism(1,1,1);

            // top bottom
            drawTriangle(
                0, 1, 0,
                1, 0, 0,
                0, 0, 0
            );
            drawTriangle(
                0, 0, 1,
                1, 0, 1,
                0, 1, 1
            );

            // x-z plane
            drawTriangle(
                0, 0, 0,
                0, 0, 1,
                1, 0, 1
            );
            drawTriangle(
                1, 0, 1,
                1, 0, 0,
                0, 0, 0
            );

            // z-y plane
            drawTriangle(
                0, 0, 0,
                0, 1, 0,
                0, 1, 1
            );
            drawTriangle(
                0, 1, 1,
                0, 0, 1,
                0, 0, 0
            );

            // final surface
            drawTriangle(
                0, 1, 0,
                0, 1, 1,
                1, 0, 1
            );
            drawTriangle(
                1, 0, 1,
                1, 0, 0,
                0, 1, 0
            );

        glPopMatrix();

        glPushMatrix();
            glTranslated(0, 1.5, -1);
            glScaled(4.5, 1.5, 2);
            drawTextureBox(1, 1, 1);
        glPopMatrix();

        // test metaball
        //glPushMatrix();
        //glTranslated(0, 5, 0);
        //glRotated(90, 0, 0, 1);
        //glScaled(1, 1, 1);
        //
        //auto m_func = [](double x, double y, double z) 
        //    -> double {return metaballFunc(-0.5, 0, 0, x, y, z) + metaballFunc(0.5, 0, 0, x, y, z); };
        //
        //drawMetaball(m_func(1, 0, 0), 2, m_func);
        //glPopMatrix();



        if (VAL(LV_DETAIL) > 1) {

            if (VAL(DRAW_PROP)) {
                // propeller
                // left

                glPushMatrix();
                glTranslated(1, 2.5, 0.9);
                glRotated(60, 1, 0, 0);

                glPushMatrix();
                glScaled(0.3, 1.2, 0.3);
                drawTextureBox(1, 1, 1);
                glPopMatrix();

                glTranslated(-1.5, 0.8 - 0.2 + 0.8, 0);
                glPushMatrix();
                glRotated(90, 0, 1, 0);
                drawCylinder(3, 0.8, 0.8);
                glPopMatrix();

                // wing
                glPushMatrix();
                glTranslated(0.3, 0, 0);
                glRotated(-90, 0, 1, 0);
                glRotated(-90, 0, 0, 1);
                glRotated(180, 1, 0, 0);

                //rotate
                glRotated(- VAL(WING_ANGLE), 0, 0, 1);
                animateWing();
                //drawAxis();
                drawWing(FALSE);
                glPopMatrix();

                glPopMatrix();

                // right
                glPushMatrix();
                glTranslated(1, 2.5 - 0.15, -0.9); // - (0.3 / 2) since rotation direction
                glRotated(-60, 1, 0, 0);

                glPushMatrix();
                glScaled(0.3, 1.2, 0.3);
                drawTextureBox(1, 1, 1);
                glPopMatrix();

                glTranslated(-1.5, 0.8 - 0.2 + 0.8, +0.15);
                glPushMatrix();
                glRotated(90, 0, 1, 0);
                drawCylinder(3, 0.8, 0.8);
                glPopMatrix();

                // wing
                glPushMatrix();
                glTranslated(0.3, 0, 0);
                glRotated(90, 0, 1, 0);
                glRotated(90, 0, 0, 1);

                //rotate
                glRotated(VAL(WING_ANGLE), 0, 0, 1);
                animateWing();
                //drawAxis();
                drawWing(TRUE);
                glPopMatrix();

                glPopMatrix();


            }


            // part 3: neck

            double angle0 = 45 + VAL(NECK_ANGLE);

            glPushMatrix();

            glTranslated(-1, 1.3, -1);
            glRotated(angle0, 0.0, 0.0, 1.0);

            glPushMatrix();
            glScaled(1.5, 2.2, 2);
            drawTextureBox(1, 1, 1);
            glPopMatrix();

            // head
            if (VAL(LV_DETAIL) > 2) {

                glTranslated(1.5, 2, 0);
                glRotated(90, 0, 0, 1);

                // rotate
                glTranslated(0, 0, 1);
                glRotated(VAL(HEAD_ANGLE_X), 1, 0, 0);
                glTranslated(0, 0, -1);
                glRotated(VAL(HEAD_ANGLE_Z), 0, 0, 1);

                if (VAL(CHEERFULNESS) < 0 && hasDiffLegParam) {
                    shouldMoveHead = 1;                    
                }
                moveHead();

                glPushMatrix();
                glScaled(0.75, 3, 2);
                drawTextureBox(1, 1, 1);
                glPopMatrix();

                glTranslated(0.75, 0, 0);

                glPushMatrix();
                glScaled(0.75, 2, 2);
                drawTextureBox(1, 1, 1);
                glPopMatrix();

                // torus
                //drawAxis();

                glPushMatrix();
                glTranslated(1.5, 1, 1);
                glRotated(90, 0, 1, 0);
                drawTorus(0.8, 0.2);
                glPopMatrix();
            }

            glPopMatrix();


            // part4, 8, 11 front right leg
            double frontLegSize = 0.6;
            double frontLegX = -2 + frontLegSize;
            double frontLegY = 0;
            double frontLegZ = -1;

            glPushMatrix();
                glTranslated(frontLegX, frontLegY + 0.5, frontLegZ);

                // rotate
                glRotated(VAL(RIGHT_FRONT_ANGLE1), 0, 0, 1);
                animate(RIGHT_FRONT_ANGLE1);

                if (VAL(METABALL)) {
                    glPushMatrix();
                    glTranslated(-0.3, -1.25, 0.3);
                    glRotated(-90, 0, 0, 1);
                    glScaled(1, 0.8, 0.8);

                    auto m_func = [](double x, double y, double z)
                        -> double {return metaballFunc(-0.55, 0, 0, x, y, z) + metaballFunc(0.55, 0, 0, x, y, z); };

                    drawMetaball(m_func(1.12, 0, 0), 1, m_func);
                    glPopMatrix();
                }
                else {
                    glPushMatrix();
                    glRotated(-180, 0, 0, 1);
                    glScaled(frontLegSize, 2 + 0.5, frontLegSize);
                    drawTextureBox(1, 1, 1);
                    glPopMatrix();
                }

                if (VAL(LV_DETAIL) > 2) {

                    glTranslated(0, -2 - 0.5 + 0.2, 0);
                    glRotated(VAL(RIGHT_FRONT_ANGLE2), 0, 0, 1);
                    animate(RIGHT_FRONT_ANGLE2);

                    if (VAL(METABALL)) {
                        glPushMatrix();
                        glTranslated(-0.3, -1.25 + 0.6, 0.3);
                        glRotated(-90, 0, 0, 1);
                        glScaled(0.8, 0.6, 0.6);

                        auto m_func = [](double x, double y, double z)
                            -> double {return metaballFunc(-0.55, 0, 0, x, y, z) + metaballFunc(0.55, 0, 0, x, y, z); };

                        drawMetaball(m_func(1.12, 0, 0), 1, m_func);
                        glPopMatrix();
                    }
                    else {
                        glPushMatrix();
                        glRotated(-180, 0, 0, 1);
                        glScaled(frontLegSize, 1.5, frontLegSize);
                        drawTextureBox(1, 1, 1);
                        glPopMatrix();
                    }

                    if (VAL(LV_DETAIL) > 3) {

                        glTranslated(0, -1.5 + 0.2, 0);
                        glRotated(VAL(RIGHT_FRONT_ANGLE3), 0, 0, 1);
                        animate(RIGHT_FRONT_ANGLE3);

                        glPushMatrix();
                        glRotated(-180, 0, 0, 1);
                        glScaled(1, 0.3, frontLegSize);
                        drawTextureBox(1, 1, 1);
                        glPopMatrix();
                    }
                }
            glPopMatrix();






            // front left leg
            frontLegZ = 1 - frontLegSize;

            // copied from above
            glPushMatrix();
                glTranslated(frontLegX, frontLegY + 0.5, frontLegZ);

                // rotate
                glRotated(VAL(LEFT_FRONT_ANGLE1), 0, 0, 1);
                animate(LEFT_FRONT_ANGLE1);

                if (VAL(METABALL)) {
                    glPushMatrix();
                    glTranslated(-0.3, -1.25, 0.3);
                    glRotated(-90, 0, 0, 1);
                    glScaled(1, 0.8, 0.8);

                    auto m_func = [](double x, double y, double z)
                        -> double {return metaballFunc(-0.55, 0, 0, x, y, z) + metaballFunc(0.55, 0, 0, x, y, z); };

                    drawMetaball(m_func(1.12, 0, 0), 1, m_func);
                    glPopMatrix();
                }
                else {
                    glPushMatrix();
                    glRotated(-180, 0, 0, 1);
                    glScaled(frontLegSize, 2 + 0.5, frontLegSize);
                    drawTextureBox(1, 1, 1);
                    glPopMatrix();
                }

                glTranslated(0, -2 - 0.5 + 0.2, 0);
                glRotated(VAL(LEFT_FRONT_ANGLE2), 0, 0, 1);
                animate(LEFT_FRONT_ANGLE2);

                if (VAL(LV_DETAIL) > 2) {

                    if (VAL(METABALL)) {
                        glPushMatrix();
                        glTranslated(-0.3, -1.25 + 0.6, 0.3);
                        glRotated(-90, 0, 0, 1);
                        glScaled(0.8, 0.6, 0.6);

                        auto m_func = [](double x, double y, double z)
                            -> double {return metaballFunc(-0.55, 0, 0, x, y, z) + metaballFunc(0.55, 0, 0, x, y, z); };

                        drawMetaball(m_func(1.12, 0, 0), 1.5, m_func);
                        glPopMatrix();
                    }
                    else {
                        glPushMatrix();
                        glRotated(-180, 0, 0, 1);
                        glScaled(frontLegSize, 1.5, frontLegSize);
                        drawTextureBox(1, 1, 1);
                        glPopMatrix();
                    }


                    glTranslated(0, -1.5 + 0.2, 0);
                    glRotated(VAL(LEFT_FRONT_ANGLE3), 0, 0, 1);
                    animate(LEFT_FRONT_ANGLE3);

                    if (VAL(LV_DETAIL) > 3) {
                        glPushMatrix();
                            glRotated(-180, 0, 0, 1);
                            glScaled(1, 0.3, frontLegSize);
                            drawTextureBox(1, 1, 1);
                        glPopMatrix();
                    }
                }
            glPopMatrix();






			// part5, 9, 12 back right leg

            double backLegSize = 0.6;
            double backLegX = 4.5;
            double backLegY = 1.7;
            double backLegZ = -1;

            double aux = 0.1;

            glPushMatrix();
            glTranslated(backLegX, backLegY + aux, backLegZ);
			//drawAxis();
            // rotate

			float alpha1, alpha2;
			const float l1 = 2.3, l2 = 3.0;
			if (VAL(IK_SWITCH)) {
				Mat4f mTrans, mRota;
				Vec3f vTrans(- backLegX, - (backLegY + aux), -backLegZ);
				MakeHTrans(mTrans, vTrans);
				vIK = (mTrans * vIK);
				std::cout << vIK << std::endl;
				float theta = atanf(vIK[2] / vIK[1]) / M_PI * 180.0f;
				if (VAL(IK_CONSTRAINT_SWITCH)) {
					theta = abs(theta) > VAL(IK_THETA_COS) ? theta/ abs(theta)*VAL(IK_THETA_COS) : theta;
				}
				float theta_radiant = atanf(vIK[2] / vIK[1]);
				printf("%f\n", theta);
				glRotatef(theta, 1, 0, 0);
				MakeHRotX(mRota, -theta_radiant);
				vIK = mRota * vIK;
				//glPushMatrix();
				//glTranslatef(vIK[0], vIK[1], vIK[2]);
				//drawAxis();
				//glPopMatrix;
				std::cout << vIK << std::endl;
				// deal with the situation that can't reach (too far
				float dis = sqrt(vIK[0] * vIK[0] + vIK[1] * vIK[1]);
				if (dis > (l1 + l2)) {
					printf("distancelong\n");
					vIK[0] *= (l1 + l2) / dis;
					vIK[1] *= (l1 + l2) / dis;
					while (sqrt(vIK[0] * vIK[0] + vIK[1] * vIK[1]) >= l1 + l2) {
						vIK[0] *= 0.98; vIK[1] *= 0.99;
					}
				}
				else if (dis < (l2 - l1)) {
					printf("distanceshort\n");
					vIK[0] *= (l2 - l1) / dis * 1.05;
					vIK[1] *= (l2 - l1) / dis * 1.05;
					while (sqrt(vIK[0] * vIK[0] + vIK[1] * vIK[1]) <= l2 - l1) {
						vIK[0] *= 1.02; vIK[1] *= 1.02;
					}
				}
				std::cout << vIK <<" distance:"<< sqrt(vIK[0] * vIK[0] + vIK[1] * vIK[1]) <<std::endl;
				// calculate alpha1 and alpha2
				float x, y; // target point
				if (vIK[1] < 0) {
					x = -vIK[1], y = vIK[0];
					alpha2 = acosf((x*x + y * y - l1 * l1 - l2 * l2) / (2 * l1*l2));
					//alpha1 = (-l1 * sinf(alpha2)*x + (l1 + l2 * cosf(alpha2))*y) / (l1 * sinf(alpha2)*y + (l1 + l2 * cosf(alpha2))*x);
					float temp = acosf((x*x + y * y + l1 * l1 - l2 * l2) / (2 * l1*sqrt(x*x + y * y)));
					alpha1 = atanf(y / x) - temp;
					alpha2 = alpha2 / M_PI * 180.0f + 30.0f;
					alpha1 = alpha1 / M_PI * 180.0f;
				}
				else if (vIK[0] >= 0) {
					x = vIK[0], y = vIK[1];
					alpha2 = acosf((x*x + y * y - l1 * l1 - l2 * l2) / (2 * l1*l2));
					//alpha1 = (-l1 * sinf(alpha2)*x + (l1 + l2 * cosf(alpha2))*y) / (l1 * sinf(alpha2)*y + (l1 + l2 * cosf(alpha2))*x);
					float temp = acosf((x*x + y * y + l1 * l1 - l2 * l2) / (2 * l1*sqrt(x*x + y * y)));
					alpha1 = atanf(y / x) - temp;
					alpha2 = alpha2 / M_PI * 180.0f + 30.0f;
					alpha1 = alpha1 / M_PI * 180.0f + 90.0f;
				}
				else {
					x = - vIK[0], y = vIK[1];
					alpha2 = acosf((x*x + y * y - l1 * l1 - l2 * l2) / (2 * l1*l2));
					//alpha1 = (-l1 * sinf(alpha2)*x + (l1 + l2 * cosf(alpha2))*y) / (l1 * sinf(alpha2)*y + (l1 + l2 * cosf(alpha2))*x);
					float temp = acosf((x*x + y * y + l1 * l1 - l2 * l2) / (2 * l1*sqrt(x*x + y * y)));
					alpha1 = atanf(y / x) - temp;
					alpha2 = - (alpha2 / M_PI * 180.0f - 30.0f);
					alpha1 = - (alpha1 / M_PI * 180.0f + 90.0f);
				}

				if (VAL(IK_CONSTRAINT_SWITCH)) {
					alpha1 = abs(alpha1) > VAL(IK_ALPHA_COS) ? alpha1 / abs(alpha1) * VAL(IK_ALPHA_COS) : alpha1;
					alpha2 = abs(alpha2) > VAL(IK_BETA_COS) ? alpha2 / abs(alpha2) * VAL(IK_BETA_COS) : alpha2;
				}

				printf("%f %f\n", alpha2, alpha1);
				glRotatef(alpha1, 0, 0, 1);
			}
			else { 
				glRotated(VAL(RIGHT_BACK_THETA), 1, 0, 0);
				glRotated(VAL(RIGHT_BACK_ANGLE1), 0, 0, 1);
			}
			
            animate(RIGHT_BACK_ANGLE1);

            glPushMatrix();
            glRotated(-180, 0, 0, 1);
            glScaled(backLegSize, 2.5 + aux, backLegSize);
            drawTextureBox(1, 1, 1);
            glPopMatrix();


            glTranslated(0, -2.5 + aux, 0);
            // rotate
			if (VAL(IK_SWITCH)) glRotatef(alpha2, 0, 0, 1);
            else glRotated(VAL(RIGHT_BACK_ANGLE2), 0, 0, 1);
            animate(RIGHT_BACK_ANGLE2);

            if (VAL(LV_DETAIL) > 2) {

                glRotated(-30, 0, 0, 1);
                glPushMatrix();
                glRotated(-180, 0, 0, 1);
                glScaled(backLegSize, 3, backLegSize);
                drawTextureBox(1, 1, 1);
                glPopMatrix();

                glTranslated(0, -3 + 3 * aux, 0);

                // rotate
                glRotated(VAL(RIGHT_BACK_ANGLE3), 0, 0, 1);
                animate(RIGHT_BACK_ANGLE3);
                glRotated(30, 0, 0, 1);

                if (VAL(LV_DETAIL) > 3) {

                    glPushMatrix();
                    glRotated(-180, 0, 0, 1);
                    glScaled(1, 0.3, backLegSize);
                    drawTextureBox(1, 1, 1);
                    glPopMatrix();
                }
            }
            glPopMatrix();





            // left
            backLegZ = 1 - backLegSize;

            glPushMatrix();
            glTranslated(backLegX, backLegY + aux, backLegZ);
            // rotate
            glRotated(VAL(LEFT_BACK_ANGLE1), 0, 0, 1);
            animate(LEFT_BACK_ANGLE1);

            glPushMatrix();
            glRotated(-180, 0, 0, 1);
            glScaled(backLegSize, 2.5 + aux, backLegSize);
            drawTextureBox(1, 1, 1);
            glPopMatrix();


            glTranslated(0, -2.5 + aux, 0);
            // rotate
            glRotated(VAL(LEFT_BACK_ANGLE2), 0, 0, 1);
            animate(LEFT_BACK_ANGLE2);

            if (VAL(LV_DETAIL) > 2) {
                glRotated(-30, 0, 0, 1);
                glPushMatrix();
                glRotated(-180, 0, 0, 1);
                glScaled(backLegSize, 3, backLegSize);
                drawTextureBox(1, 1, 1);
                glPopMatrix();

                glTranslated(0, -3 + 3 * aux, 0);

                // rotate
                glRotated(VAL(LEFT_BACK_ANGLE3), 0, 0, 1);
                animate(LEFT_BACK_ANGLE3);
                glRotated(30, 0, 0, 1);

                if (VAL(LV_DETAIL) > 3) {
                    glPushMatrix();
                    glRotated(-180, 0, 0, 1);
                    glScaled(1, 0.3, backLegSize);
                    drawTextureBox(1, 1, 1);
                    glPopMatrix();
                }
            }
            glPopMatrix();



            // tail 
            glPushMatrix();
                glTranslated(4.5, 2, -0.1);
                glRotated(VAL(TAIL_ANGLE_X), 1, 0, 0);
                glRotated(VAL(TAIL_ANGLE_Y), 0, 1, 0);

                if (VAL(CHEERFULNESS) > 0 && hasDiffLegParam) {
                    shouldMoveTail = 1;                    
                }
                moveTail();

                glRotated(-150 + VAL(TAIL_ANGLE_Z), 0, 0, 1);
                glScaled(0.2, 2.5, 0.2);
                drawTextureBox(1, 1, 1);
            glPopMatrix();

        }
		
		if (VAL(LSYSTEM_SWITCH)) {
			std::vector<char> sen = gsentence(VAL(LSYSTEM_STAGE));
			glPushMatrix();
				drawLsystem(sen);
			glPopMatrix();
		}

	glPopMatrix();

}


bool DoggModel::checkLegParam() {
    bool ret = FALSE;
    for (int i = 0; i < 12; i++) {
        if (VAL(LEFT_FRONT_ANGLE1 + i) != last_angle[i]) {
            ret = TRUE;
            last_angle[i] = VAL(LEFT_FRONT_ANGLE1 + i);
        }
    }
    return ret;
}

void DoggModel::moveHead() {
    if (!shouldMoveHead) {
        return;
    }
    if (headBoundaryCount >= 2) {
        shouldMoveHead = 0;
        headBoundaryCount = 0;
    }
    if (VAL(HEAD_ANGLE_X) + headIncr >= headBoundary 
        || VAL(HEAD_ANGLE_X) + headIncr <= -headBoundary) {
        headBoundaryCount++;
        headIncr = -headIncr;
    }
    SET(HEAD_ANGLE_X, VAL(HEAD_ANGLE_X) + headIncr);
}

void DoggModel::moveTail() {
    if (!shouldMoveTail) {
        return;
    }
    if (tailBoundaryCount >= 2) {
        shouldMoveTail = 0;
        tailBoundaryCount = 0;
    }
    if (VAL(TAIL_ANGLE_Y) + tailIncr >= tailBoundary
        || VAL(TAIL_ANGLE_Y) + tailIncr <= -tailBoundary) {
        tailBoundaryCount++;
        tailIncr = -tailIncr;
    }
    SET(TAIL_ANGLE_Y, VAL(TAIL_ANGLE_Y) + tailIncr);
}

void DoggModel::drawWing(bool isRightWing) {
    double sign = isRightWing ? 1 : -1;
    double incr = 0;
    double thickness = 0.4;
    double y = 0;

    
    glBegin(GL_TRIANGLE_STRIP);

    glVertex3d(0, y, 0);
    glVertex3d(0, y, 1);
    glVertex3d(1 * sign, y, 0.1);
    //y += incr;

    glVertex3d(1 * sign, y, 1.2);
    glVertex3d(2 * sign, y, 0.2);
    glVertex3d(2 * sign, y, 1.5);
    //y += incr;

    glVertex3d(3 * sign, y, 0.5);
    glVertex3d(3 * sign, y, 2);
    glVertex3d(4 * sign, y, 0.7);
    //y += incr;

    glVertex3d(4 * sign, y, 2.3);
    glVertex3d(5 * sign, y, 0.9);
    glVertex3d(5 * sign, y, 3);
    //y += incr;

    glVertex3d(6 * sign, y, 1);
    glVertex3d(6 * sign, y, 3.5);
    glVertex3d(7 * sign, y, 1.2);
    //y += incr;

    glVertex3d(8 * sign, y, 1.8);
    glVertex3d(8.7 * sign, y, 2.1);
    //y += incr;

    glEnd();

    double y1 = y - thickness;

    glBegin(GL_TRIANGLE_STRIP);

    glVertex3d(0, y1, 0);
    glVertex3d(0, y1, 1);
    glVertex3d(1 * sign, y1, 0.1);
    //y += incr;

    glVertex3d(1 * sign, y1, 1.2);
    glVertex3d(2 * sign, y1, 0.2);
    glVertex3d(2 * sign, y1, 1.5);
    //y += incr;

    glVertex3d(3 * sign, y1, 0.5);
    glVertex3d(3 * sign, y1, 2);
    glVertex3d(4 * sign, y1, 0.7);
    //y += incr;

    glVertex3d(4 * sign, y1, 2.3);
    glVertex3d(5 * sign, y1, 0.9);
    glVertex3d(5 * sign, y1, 3);
    //y += incr;

    glVertex3d(6 * sign, y1, 1);
    glVertex3d(6 * sign, y1, 3.5);
    glVertex3d(7 * sign, y1, 1.2);
    //y += incr;

    glVertex3d(7 * sign, y1, 2.7);
    glVertex3d(8 * sign, y1, 1.8);
    glVertex3d(8.7 * sign, y1, 2.1);
    //y += incr;

    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
    glVertex3d(0, y, 0);
    glVertex3d(0, y1, 0);

    glVertex3d(1 * sign, y, 0.1);
    glVertex3d(1 * sign, y1, 0.1);

    glVertex3d(2 * sign, y, 0.2);
    glVertex3d(2 * sign, y1, 0.2);

    glVertex3d(3 * sign, y, 0.5);
    glVertex3d(3 * sign, y1, 0.5);

    glVertex3d(4 * sign, y, 0.7);
    glVertex3d(4 * sign, y1, 0.7);

    glVertex3d(5 * sign, y, 0.9);
    glVertex3d(5 * sign, y1, 0.9);

    glVertex3d(6 * sign, y, 1);
    glVertex3d(6 * sign, y1, 1);

    glVertex3d(7 * sign, y, 1.2);
    glVertex3d(7 * sign, y1, 1.2);

    glVertex3d(8 * sign, y, 1.8);
    glVertex3d(8 * sign, y1, 1.8);

    glVertex3d(8.7 * sign, y, 2.1);
    glVertex3d(8.7 * sign, y1, 2.1);

    glVertex3d(7 * sign, y, 2.7);
    glVertex3d(7 * sign, y1, 2.7);

    glVertex3d(6 * sign, y, 3.5);    
    glVertex3d(6 * sign, y1, 3.5);

    glVertex3d(5 * sign, y, 3);
    glVertex3d(5 * sign, y1, 3);

    glVertex3d(4 * sign, y, 2.3);
    glVertex3d(4 * sign, y1, 2.3);

    glVertex3d(3 * sign, y, 2);
    glVertex3d(3 * sign, y1, 2);

    glVertex3d(2 * sign, y, 1.5);
    glVertex3d(2 * sign, y1, 1.5);

    glVertex3d(1 * sign, y, 1.2);
    glVertex3d(1 * sign, y1, 1.2);

    glVertex3d(0, y, 1);
    glVertex3d(0, y1, 1);

    glEnd();
}

void DoggModel::drawAxis() {
    // draw axis
    glPushMatrix();
    glBegin(GL_LINES);

    // lines parallel to the axis
    //glColor3f(1, 0, 0);
    //glVertex3f(-50, -2, -20);
    //glVertex3f(50, -2, -20);
    //
    //glColor3f(0, 1, 0);
    //glVertex3f(0, -50, -20);
    //glVertex3f(0, 50, -20);
    //
    //glColor3f(0, 0, 1);
    //glVertex3f(0, -2, -100);
    //glVertex3f(0, -2, 10);

    // actual axis
    setDiffuseColor(1.0f, 0.0f, 0.0f); // red x
    glVertex3f(-5, 0, 0);
    glVertex3f(50, 0, 0);


    setDiffuseColor(0.0f, 1.0f, 0.0f); // green y
    glVertex3f(0, -5, 0);
    glVertex3f(0, 50, 0);

    setDiffuseColor(0.0f, 0.0f, 1.0f); // blue z
    glVertex3f(0, 0, -5);
    glVertex3f(0, 0, 50);

    glEnd();
    glPopMatrix();
    setDiffuseColor(1.0f, 1.0f, 1.0f);
}


void DoggModel::resetLeg() {
    SET(LEFT_FRONT_ANGLE1, 0);
    SET(LEFT_FRONT_ANGLE2, 0);
    SET(LEFT_FRONT_ANGLE3, 0);

    SET(RIGHT_FRONT_ANGLE1, 0);
    SET(RIGHT_FRONT_ANGLE2, 0);
    SET(RIGHT_FRONT_ANGLE3, 0);

    SET(LEFT_BACK_ANGLE1, 0);
    SET(LEFT_BACK_ANGLE2, 0);
    SET(LEFT_BACK_ANGLE3, 0);

    SET(RIGHT_BACK_ANGLE1, 0);
    SET(RIGHT_BACK_ANGLE2, 0);
    SET(RIGHT_BACK_ANGLE3, 0);

	SET(RIGHT_BACK_THETA, 0);

    SET(RESET_LEG, 0);
}

void DoggModel::frameAll(float dx, float dy, float dz)
{
	m_camera->frameAll(dx,dy,dz);
	//gluLookAt(3.119846,3.837214,24.113871,2.040295,0.852658,-0.159513,0,1,0);
	SET(FRAME_ALL, 0);
}

void DoggModel::init() {
    //texImg = Fl_Shared_Image::get("texImg.png", 256, 256);
    texImg = readBMP("texImg.bmp", texImgW, texImgH);
    if (texImg == nullptr) {printf("Texture image not found.\n");}

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //float pixels[] = {
    //    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    //    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f
    //};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImgW, texImgH, 0, GL_RGB, GL_UNSIGNED_BYTE, texImg);

}

std::vector<char> DoggModel::gsentence(const int stage)
{
	std::vector<char> current, next;
	current.push_back('F');
	for (int i = 0; i < stage; ++i) {
		next.clear();
		for (int l = 0; l < current.size(); ++l) {
			// F -¡ú FF+[+F-F-F]-[-F+F+F]
			if (current[l] == 'F') {
				next.push_back('F');
				next.push_back('F');
				next.push_back('+');
				next.push_back('[');
				next.push_back('+');
				next.push_back('F');
				next.push_back('-');
				next.push_back('F');
				next.push_back('-');
				next.push_back('F');
				next.push_back(']');
				next.push_back('-');
				next.push_back('[');
				next.push_back('-');
				next.push_back('F');
				next.push_back('+');
				next.push_back('F');
				next.push_back('+');
				next.push_back('F');
				next.push_back(']');
			}
			else next.push_back(current[l]);
		}
		current = next;
	}
	//for (int i = 0; i < current.size(); ++i) printf("%c", current[i]);
	//printf("\n");
	return current;
}

void DoggModel::drawLsystem(const std::vector<char>& sentence)
{
	float len = 0.2;
	glTranslated(1, 2.5, 0);
	for (int i = 0; i < sentence.size(); ++i) {
		char c = sentence[i];
		switch (c) {
		case 'F':
			glLineWidth(3.0f);
			glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, len, 0.0f);
			glEnd();
			glTranslatef(0.0f, len, 0.0f);
			break;
		case '+':
			glRotatef(-25, 0, 0, 1);
			break;
		case '-':
			glRotatef(25, 0, 0, 1);
			break;
		case '[':
			glPushMatrix();
			break;
		case ']':
			glPopMatrix();
			break;
		default: break;
		}
	}
}

void DoggModel::animate(int index) {

    if (ModelerApplication::Instance()->m_animating) {
        int anim_part_count = index - LEFT_FRONT_ANGLE1;

        if (VAL(index) >= anim_max[anim_part_count] || VAL(index) <= anim_min[anim_part_count]) {
            anim_incr[anim_part_count] = - anim_incr[anim_part_count];
        }
        SET(index, VAL(index) + anim_incr[anim_part_count]);
    }
}


void DoggModel::animateWing() {

    if (ModelerApplication::Instance()->m_animating) {

        if (VAL(WING_ANGLE) >= 45 || VAL(WING_ANGLE) <= -45) {
            wingIncr = -wingIncr;
        }
        SET(WING_ANGLE, VAL(WING_ANGLE) + wingIncr);
    }
}


int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
    ModelerControl controls[NUMCONTROLS];
    controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
    controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
    controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);

    controls[NECK_ANGLE] = ModelerControl("Neck Angle", -45, 45, 0.1f, 1);
	controls[HEAD_ANGLE_X] = ModelerControl("Head Angle in X direction", -45, 45, 0.1f, 0);
    controls[HEAD_ANGLE_Z] = ModelerControl("Head Angle in Z direction", 0, 45, 0.1f, 0);
    controls[LEFT_FRONT_ANGLE1] = ModelerControl("Left front upper  arm angle", -45, 45, 0.1f, 0);
    controls[LEFT_FRONT_ANGLE2] = ModelerControl("Left front middle arm angle", -45, 45, 0.1f, 0);
    controls[LEFT_FRONT_ANGLE3] = ModelerControl("Left front lower  arm angle", -45, 45, 0.1f, 0);
    controls[RIGHT_FRONT_ANGLE1] = ModelerControl("Right front upper  arm angle", -45, 45, 0.1f, 0);
    controls[RIGHT_FRONT_ANGLE2] = ModelerControl("Right front middle arm angle", -45, 45, 0.1f, 0);
    controls[RIGHT_FRONT_ANGLE3] = ModelerControl("Right front lower  arm angle", -45, 45, 0.1f, 0);
    controls[LEFT_BACK_ANGLE1] = ModelerControl("Left back upper  leg angle", -45, 45, 0.1f, 0);
    controls[LEFT_BACK_ANGLE2] = ModelerControl("Left back middle leg angle", -45, 45, 0.1f, 0);
    controls[LEFT_BACK_ANGLE3] = ModelerControl("Left back lower  leg angle", -45, 45, 0.1f, 0);
    controls[RIGHT_BACK_ANGLE1] = ModelerControl("Right back upper  leg angle", -45, 45, 0.1f, 0);
    controls[RIGHT_BACK_ANGLE2] = ModelerControl("Right back middle leg angle", -45, 45, 0.1f, 0);
    controls[RIGHT_BACK_ANGLE3] = ModelerControl("Right back lower  leg angle", -45, 45, 0.1f, 0);
    controls[TAIL_ANGLE_X] = ModelerControl("Tail Angle X", -45, 45, 0.1f, 0);
    controls[TAIL_ANGLE_Y] = ModelerControl("Tail Angle Y", -45, 45, 0.1f, 0);
    controls[TAIL_ANGLE_Z] = ModelerControl("Tail Angle Z", -15, 135, 0.1f, 0);

    controls[RESET_LEG] = ModelerControl("Reset Leg Pose", 0, 1, 1, 0);
	controls[FRAME_ALL] = ModelerControl("Frame All", 0, 1, 1, 0);

    controls[LIGHT0_X] = ModelerControl("Light 0 X position", -5, 5, 0.1f, 4);
    controls[LIGHT0_Y] = ModelerControl("Light 0 Y position", -5, 5, 0.1f, 2);
    controls[LIGHT0_Z] = ModelerControl("Light 0 Z position", -5, 5, 0.1f, -4);
                                                              
    controls[LIGHT0_R] = ModelerControl("Light 0 red color", 0, 1, 0.01f, 1);
    controls[LIGHT0_G] = ModelerControl("Light 0 green color", 0, 1, 0.01f, 1);
    controls[LIGHT0_B] = ModelerControl("Light 0 blue color", 0, 1, 0.01f, 1);

    controls[DRAW_PROP] = ModelerControl("Draw propeller?", 0, 1, 1, 1);

    controls[LV_DETAIL] = ModelerControl("Level of Detail", 1, 4, 1, 4);

    controls[APPLY_TEX] = ModelerControl("Apply Texture", 0, 1, 1, 0);

	controls[LSYSTEM_SWITCH] = ModelerControl("Display Tree", 0, 1, 1, 0);
	controls[LSYSTEM_STAGE] = ModelerControl("LSystem Stage", 1, 6, 1, 3);

    controls[CHEERFULNESS] = ModelerControl("cheerfulness level", -1, 1, 1, 0);

    controls[METABALL] = ModelerControl("Enable metaball", 0, 1, 1, 0);
    controls[METABALL_RESOLUTION] = ModelerControl("Metaball Resolution", 0.05, 0.3, 0.01, 0.15);

	controls[RIGHT_BACK_THETA] = ModelerControl("Right back upper leg theta", -15, 15, 1, 0);

	controls[IK_SWITCH] = ModelerControl("IK Switch", 0, 1, 1, 0);
	controls[IK_X] = ModelerControl("IK X", -10, 10, 0.1f, 4.5);
	controls[IK_Y] = ModelerControl("IK Y", -10, 10, 0.1f, 4.4);
	controls[IK_Z] = ModelerControl("IK Z", -10, 10, 0.1f, -1);
	controls[IK_CONSTRAINT_SWITCH] = ModelerControl("IK Constraint Switch", 0, 1, 1, 0);
	controls[IK_ALPHA_COS] = ModelerControl("IK Alpha Constraint(deg)", 0, 90, 1, 45);
	controls[IK_BETA_COS] = ModelerControl("IK Alpha2 Constraint(deg)", 0, 90, 1, 45);
	controls[IK_THETA_COS] = ModelerControl("IK Theta Constraint(deg)", 0, 45, 1, 15);

    controls[WING_ANGLE] = ModelerControl("Wing Angle", -30, 30, 0.1f, 0);

    ModelerApplication::Instance()->Init(&createDoggModel, controls, NUMCONTROLS);

    return ModelerApplication::Instance()->Run();
}