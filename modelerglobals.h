#ifndef _MODELER_GLOBALS_H
#define _MODELER_GLOBALS_H

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

// This is a list of the controls for the SampleModel
// We'll use these constants to access the values 
// of the controls from the user interface.
enum SampleModelControls
{ 
	XPOS, YPOS, ZPOS, 
    NECK_ANGLE, HEAD_ANGLE_X, HEAD_ANGLE_Z,

    // Note: The following 12 items should remain in their place 
    // do not insert anything in between!
    LEFT_FRONT_ANGLE1, LEFT_FRONT_ANGLE2, LEFT_FRONT_ANGLE3,
    RIGHT_FRONT_ANGLE1, RIGHT_FRONT_ANGLE2, RIGHT_FRONT_ANGLE3,
    LEFT_BACK_ANGLE1, LEFT_BACK_ANGLE2, LEFT_BACK_ANGLE3,
    RIGHT_BACK_ANGLE1, RIGHT_BACK_ANGLE2, RIGHT_BACK_ANGLE3,
    // Note: The above 12 items should remain in their place 
    // do not insert anything in between!

    TAIL_ANGLE_X, TAIL_ANGLE_Y, TAIL_ANGLE_Z,
    RESET_LEG,
	FRAME_ALL,
    LIGHT0_X, LIGHT0_Y, LIGHT0_Z,
    LIGHT0_R, LIGHT0_G, LIGHT0_B,
    DRAW_PROP,
    LV_DETAIL,
    APPLY_TEX,
	LSYSTEM_SWITCH, LSYSTEM_STAGE,
    CHEERFULNESS,
    METABALL,
    METABALL_RESOLUTION,
	RIGHT_BACK_THETA,
	IK_SWITCH, IK_X, IK_Y, IK_Z, IK_CONSTRAINT_SWITCH, IK_THETA_COS, IK_ALPHA_COS, IK_BETA_COS,

    WING_ANGLE,
    NUMCONTROLS
};

// Colors
#define COLOR_RED		1.0f, 0.0f, 0.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))
#define SET(x, y) (ModelerApplication::Instance()->SetControlValue(x, y))

#endif