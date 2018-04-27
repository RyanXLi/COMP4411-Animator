#include "modelerdraw.h"
#include <FL/gl.h>
#include <GL/glu.h>
#include <cstdio>
#include <math.h>
#include "vec.h"
#include "mat.h"

// ADDED
#include "modelerapp.h"
#include "modelerglobals.h"
#include "bitmap.h"
#include <map>


#define NUMSEG 20
// END

enum {
	front,
	left,
	back,
	right,
	up,
	bottom
};

// ********************************************************
// Support functions from previous version of modeler
// ********************************************************
void _dump_current_modelview( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    GLdouble mv[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, mv );
    fprintf( mds->m_rayFile, 
        "transform(\n    (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n     (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n",
        mv[0], mv[4], mv[8], mv[12],
        mv[1], mv[5], mv[9], mv[13],
        mv[2], mv[6], mv[10], mv[14],
        mv[3], mv[7], mv[11], mv[15] );
}

void _dump_current_material( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    fprintf( mds->m_rayFile, 
        "material={\n    diffuse=(%f,%f,%f);\n    ambient=(%f,%f,%f);\n}\n",
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2], 
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2]);
}

// ****************************************************************************

// Initially assign singleton instance to NULL
ModelerDrawState* ModelerDrawState::m_instance = NULL;

ModelerDrawState::ModelerDrawState() : m_drawMode(NORMAL), m_quality(MEDIUM)
{
    float grey[]  = {.5f, .5f, .5f, 1};
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    
    memcpy(m_ambientColor, black, 4 * sizeof(float));
    memcpy(m_diffuseColor, grey, 4 * sizeof(float));
    memcpy(m_specularColor, white, 4 * sizeof(float));
    
    m_shininess = 0.5;
    
    m_rayFile = NULL;
}

// CLASS ModelerDrawState METHODS
ModelerDrawState* ModelerDrawState::Instance()
{
    // Return the singleton if it exists, otherwise, create it
    return (m_instance) ? (m_instance) : m_instance = new ModelerDrawState();
}

// ****************************************************************************
// Modeler functions for your use
// ****************************************************************************
// Set the current material properties

void setAmbientColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_ambientColor[0] = (GLfloat)r;
    mds->m_ambientColor[1] = (GLfloat)g;
    mds->m_ambientColor[2] = (GLfloat)b;
    mds->m_ambientColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mds->m_ambientColor);
}

void setDiffuseColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_diffuseColor[0] = (GLfloat)r;
    mds->m_diffuseColor[1] = (GLfloat)g;
    mds->m_diffuseColor[2] = (GLfloat)b;
    mds->m_diffuseColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mds->m_diffuseColor);
    else
        glColor3f(r,g,b);
}

void setSpecularColor(float r, float g, float b)
{	
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_specularColor[0] = (GLfloat)r;
    mds->m_specularColor[1] = (GLfloat)g;
    mds->m_specularColor[2] = (GLfloat)b;
    mds->m_specularColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mds->m_specularColor);
}

void setShininess(float s)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_shininess = (GLfloat)s;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialf( GL_FRONT, GL_SHININESS, mds->m_shininess);
}

void setDrawMode(DrawModeSetting_t drawMode)
{
    ModelerDrawState::Instance()->m_drawMode = drawMode;
}

void setQuality(QualitySetting_t quality)
{
    ModelerDrawState::Instance()->m_quality = quality;
}

bool openRayFile(const char rayFileName[])
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	fprintf(stderr, "Ray file format output is buggy (ehsu)\n");
    
    if (!rayFileName)
        return false;
    
    if (mds->m_rayFile) 
        closeRayFile();
    
    mds->m_rayFile = fopen(rayFileName, "w");
    
    if (mds->m_rayFile != NULL) 
    {
        fprintf( mds->m_rayFile, "SBT-raytracer 1.0\n\n" );
        fprintf( mds->m_rayFile, "camera { fov=30; }\n\n" );
        fprintf( mds->m_rayFile, 
            "directional_light { direction=(-1,-1,-1); color=(0.7,0.7,0.7); }\n\n" );
        return true;
    }
    else
        return false;
}

void _setupOpenGl()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
	switch (mds->m_drawMode)
	{
	case NORMAL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);
		break;
	case FLATSHADE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_FLAT);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glShadeModel(GL_FLAT);
	default:
		break;
	}

}

void closeRayFile()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile) 
        fclose(mds->m_rayFile);
    
    mds->m_rayFile = NULL;
}

void drawSphere(double r)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, "scale(%f,%f,%f,sphere {\n", r, r, r );
        _dump_current_material();
        fprintf(mds->m_rayFile, "}))\n" );
    }
    else
    {
        int divisions; 
        GLUquadricObj* gluq;
        
        switch(mds->m_quality)
        {
        case HIGH: 
            divisions = 32; break;
        case MEDIUM: 
            divisions = 20; break;
        case LOW:
            divisions = 12; break;
        case POOR:
            divisions = 8; break;
        }
        
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluSphere(gluq, r, divisions, divisions);
        gluDeleteQuadric( gluq );
    }
}


void drawBox( double x, double y, double z )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,  
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z );
        _dump_current_material();
        fprintf(mds->m_rayFile,  "})))\n" );
    }
    else
    {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv( GL_MATRIX_MODE, &savemode );
        
        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glScaled( x, y, z );
        
        glBegin( GL_QUADS );
        
        glNormal3d( 0.0, 0.0, -1.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        
        glNormal3d( 0.0, -1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 1.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        
        glNormal3d( -1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        
        glNormal3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 0.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        
        glNormal3d( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 1.0, 0.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        
        glNormal3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 0.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        
        glEnd();
        
        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode( savemode );
    }
}

void drawTextureBox(double x, double y, double z) {
    ModelerDrawState *mds = ModelerDrawState::Instance();

    _setupOpenGl();

    if (mds->m_rayFile) {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z);
        _dump_current_material();
        fprintf(mds->m_rayFile, "})))\n");
    }
    else {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv(GL_MATRIX_MODE, &savemode);

        if (VAL(APPLY_TEX)) {
            glEnable(GL_TEXTURE_2D);
        }

        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glScaled(x, y, z);

        glBegin(GL_QUADS);

        glNormal3d(0.0, 0.0, -1.0);
        glTexCoord2d(0.0, 1.0); glVertex3d(0.0, 0.0, 0.0); glTexCoord2d(1.0, 1.0); glVertex3d(0.0, 1.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex3d(1.0, 1.0, 0.0); glTexCoord2d(0.0, 0.0); glVertex3d(1.0, 0.0, 0.0);

        glNormal3d(0.0, -1.0, 0.0);
        glTexCoord2d(0.0, 1.0); glVertex3d(0.0, 0.0, 0.0); glTexCoord2d(1.0, 1.0); glVertex3d(1.0, 0.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex3d(1.0, 0.0, 1.0); glTexCoord2d(0.0, 0.0); glVertex3d(0.0, 0.0, 1.0);

        glNormal3d(-1.0, 0.0, 0.0);
        glTexCoord2d(0.0, 1.0); glVertex3d(0.0, 0.0, 0.0); glTexCoord2d(1.0, 1.0); glVertex3d(0.0, 0.0, 1.0);
        glTexCoord2d(1.0, 0.0); glVertex3d(0.0, 1.0, 1.0); glTexCoord2d(0.0, 0.0); glVertex3d(0.0, 1.0, 0.0);

        glNormal3d(0.0, 0.0, 1.0);
        glTexCoord2d(0.0, 1.0); glVertex3d(0.0, 0.0, 1.0); glTexCoord2d(1.0, 1.0); glVertex3d(1.0, 0.0, 1.0);
        glTexCoord2d(1.0, 0.0); glVertex3d(1.0, 1.0, 1.0); glTexCoord2d(0.0, 0.0); glVertex3d(0.0, 1.0, 1.0);

        glNormal3d(0.0, 1.0, 0.0);
        glTexCoord2d(0.0, 1.0); glVertex3d(0.0, 1.0, 0.0); glTexCoord2d(1.0, 1.0); glVertex3d(0.0, 1.0, 1.0);
        glTexCoord2d(1.0, 0.0); glVertex3d(1.0, 1.0, 1.0); glTexCoord2d(0.0, 0.0); glVertex3d(1.0, 1.0, 0.0);

        glNormal3d(1.0, 0.0, 0.0);
        glTexCoord2d(0.0, 1.0); glVertex3d(1.0, 0.0, 0.0); glTexCoord2d(1.0, 1.0); glVertex3d(1.0, 1.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex3d(1.0, 1.0, 1.0); glTexCoord2d(0.0, 0.0); glVertex3d(1.0, 0.0, 1.0);

        glEnd();

        if (VAL(APPLY_TEX)) {
            glDisable(GL_TEXTURE_2D);
        }

        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode(savemode);

    }
}

void drawCylinder( double h, double r1, double r2 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    int divisions;

	_setupOpenGl();
    
    switch(mds->m_quality)
    {
    case HIGH: 
        divisions = 32; break;
    case MEDIUM: 
        divisions = 20; break;
    case LOW:
        divisions = 12; break;
    case POOR:
        divisions = 8; break;
    }
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        GLUquadricObj* gluq;
        
        /* GLU will again do the work.  draw the sides of the cylinder. */
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluCylinder( gluq, r1, r2, h, divisions, divisions);
        gluDeleteQuadric( gluq );
        
        if ( r1 > 0.0 )
        {
        /* if the r1 end does not come to a point, draw a flat disk to
            cover it up. */
            
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_INSIDE );
            gluDisk( gluq, 0.0, r1, divisions, divisions);
            gluDeleteQuadric( gluq );
        }
        
        if ( r2 > 0.0 )
        {
        /* if the r2 end does not come to a point, draw a flat disk to
            cover it up. */
            
            /* save the current matrix mode. */	
            int savemode;
            glGetIntegerv( GL_MATRIX_MODE, &savemode );
            
            /* translate the origin to the other end of the cylinder. */
            glMatrixMode( GL_MODELVIEW );
            glPushMatrix();
            glTranslated( 0.0, 0.0, h );
            
            /* draw a disk centered at the new origin. */
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_OUTSIDE );
            gluDisk( gluq, 0.0, r2, divisions, divisions);
            gluDeleteQuadric( gluq );
            
            /* restore the matrix stack and mode. */
            glPopMatrix();
            glMatrixMode( savemode );
        }
    }
    
}
void drawTriangle( double x1, double y1, double z1,
                   double x2, double y2, double z2,
                   double x3, double y3, double z3 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "polymesh { points=((%f,%f,%f),(%f,%f,%f),(%f,%f,%f)); faces=((0,1,2));\n", x1, y1, z1, x2, y2, z2, x3, y3, z3 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        double a, b, c, d, e, f;
        
        /* the normal to the triangle is the cross product of two of its edges. */
        a = x2-x1;
        b = y2-y1;
        c = z2-z1;
        
        d = x3-x1;
        e = y3-y1;
        f = z3-z1;
        
        glBegin( GL_TRIANGLES );
        glNormal3d( b*f - c*e, c*d - a*f, a*e - b*d );
        glVertex3d( x1, y1, z1 );
        glVertex3d( x2, y2, z2 );
        glVertex3d( x3, y3, z3 );
        glEnd();
    }
}


void drawBSplineSurface(float dx, float dy, float dz) {


    _setupOpenGl();

    int numSeg = NUMSEG;
    //std::vector<std::vector<Point3>> mesh = {
    //    { { 0, 0, 0 },{ 0, 1, 1 },{ 0, 2, 0 },{ 0, 3, 1 } },
    //    { { 1, 0, -1 },{ 1, 1, 0 },{ 1, 2, -1 },{ 1, 3, 0 } },
    //    { { 2, 0, 0 },{ 2, 1, 1 },{ 2, 2, 0 },{ 2, 3, 1 } },
    //    { { 3, 0, -1 },{ 3, 1, 0 },{ 3, 2, -1 },{ 3, 3, 0 } }
    //};

    std::vector<std::vector<Point3>> mesh = {
        { { 0+dx, 0+dy, 2+dz  },{ 0, 1, -2 },{ 0, 2, 2  },{ 0, 3, -2 } },
        { { 1, 0, -2 },{ 1, 1, 2 },{ 1, 2, -2 },{ 1, 3, 2 } },
        { { 2, 0, 2  },{ 2, 1, -2 },{ 2, 2, 2  },{ 2, 3, -2 } },
        { { 3, 0, -2 },{ 3, 1, 2 },{ 3, 2, -2 },{ 3, 3, 2 } }
    };
    std::vector<std::vector<Point3>> firstDimensionSplines = {
        std::vector<Point3>(), std::vector<Point3>(), std::vector<Point3>(), std::vector<Point3>()
    };

    for (int j = 0; j < 4; j++) {
   
        //for (int i = 0; i <= ptvCtrlPts.size() - 4; i += 1) {
        Point3 b1 = mesh[j][0];
        Point3 b2 = mesh[j][1];
        Point3 b3 = mesh[j][2];
        Point3 b4 = mesh[j][3];

        Eigen::Matrix<float, 4, 3> b_matrix;
        b_matrix << b1.x, b1.y, b1.z,
            b2.x, b2.y, b2.z,
            b3.x, b3.y, b3.z,
            b4.x, b4.y, b4.z;

        Eigen::Matrix<float, 4, 4> transformMatrix;
        transformMatrix << 1, 4, 1, 0,
            0, 4, 2, 0,
            0, 2, 4, 0,
            0, 1, 4, 1;

        Eigen::Matrix<float, 4, 3> v_matrix = (1.0f / 6.0f) * transformMatrix * b_matrix;

        Point3 p1 = { v_matrix(0, 0), v_matrix(0, 1), v_matrix(0, 2) };
        Point3 p2 = { v_matrix(1, 0), v_matrix(1, 1), v_matrix(1, 2) };
        Point3 p3 = { v_matrix(2, 0), v_matrix(2, 1), v_matrix(2, 2) };
        Point3 p4 = { v_matrix(3, 0), v_matrix(3, 1), v_matrix(3, 2) };

        evalBezierSegment(p1, p2, p3, p4, firstDimensionSplines[j]);

        //std::cout << j << std::endl;
        //for (int i = 0; i < firstDimensionSplines[j].size(); i++) {
        //    std::cout << firstDimensionSplines[j][i].x << std::endl;
        //    std::cout << firstDimensionSplines[j][i].y << std::endl;
        //    std::cout << firstDimensionSplines[j][i].z << std::endl;
        //}
        //}
        
    }



    //for (int k = 0; k <= 4 - 4; k += 1) {
        for (int j = 0; j < numSeg-1; j += 1) {
            std::vector<Point3> secondDimensionSpline1 = {};
            std::vector<Point3> secondDimensionSpline2 = {};
            Point3 p1, p2, p3, p4, b1, b2, b3, b4;
            //for (int i = 0; i <= firstDimensionSplines.size() - 4; i += 1) {
            b1 = firstDimensionSplines[0][j];
            b2 = firstDimensionSplines[1][j];
            b3 = firstDimensionSplines[2][j];
            b4 = firstDimensionSplines[3][j];

            Eigen::Matrix<float, 4, 3> b_matrix;
            b_matrix << b1.x, b1.y, b1.z,
                b2.x, b2.y, b2.z,
                b3.x, b3.y, b3.z,
                b4.x, b4.y, b4.z;

            Eigen::Matrix<float, 4, 4> transformMatrix;
            transformMatrix << 1, 4, 1, 0,
                0, 4, 2, 0,
                0, 2, 4, 0,
                0, 1, 4, 1;

            Eigen::Matrix<float, 4, 3> v_matrix = (1.0f / 6.0f) * transformMatrix * b_matrix;

            p1 = { v_matrix(0, 0), v_matrix(0, 1), v_matrix(0, 2) };
            p2 = { v_matrix(1, 0), v_matrix(1, 1), v_matrix(1, 2) };
            p3 = { v_matrix(2, 0), v_matrix(2, 1), v_matrix(2, 2) };
            p4 = { v_matrix(3, 0), v_matrix(3, 1), v_matrix(3, 2) };

            evalBezierSegment(p1, p2, p3, p4, secondDimensionSpline1);

                //// second dimension
                //// hardcode
                //std::cout << "1:" << std::endl;
                //for (int i = 0; i < secondDimensionSpline1.size(); i++) {
                //    std::cout << secondDimensionSpline1[i].x << std::endl;
                //    std::cout << secondDimensionSpline1[i].y << std::endl;
                //    std::cout << secondDimensionSpline1[i].z << std::endl;
                //}
                //std::cout << std::endl;
                //std::cout << std::endl;
                //std::cout << std::endl;
                //std::cout << "2:" << std::endl;
                //for (int i = 0; i < secondDimensionSpline1.size(); i++) {
                //    std::cout << secondDimensionSpline2[i].x << std::endl;
                //    std::cout << secondDimensionSpline2[i].y << std::endl;
                //    std::cout << secondDimensionSpline2[i].z << std::endl;
                //}
            //}
            //for (int i = 0; i <= firstDimensionSplines.size() - 4; i += 1) {
            b1 = firstDimensionSplines[0][j+1];
            b2 = firstDimensionSplines[1][j+1];
            b3 = firstDimensionSplines[2][j+1];
            b4 = firstDimensionSplines[3][j+1];

            //Eigen::Matrix<float, 4, 3> b_matrix;
            b_matrix << b1.x, b1.y, b1.z,
                b2.x, b2.y, b2.z,
                b3.x, b3.y, b3.z,
                b4.x, b4.y, b4.z;

            //Eigen::Matrix<float, 4, 4> transformMatrix;
            transformMatrix << 1, 4, 1, 0,
                0, 4, 2, 0,
                0, 2, 4, 0,
                0, 1, 4, 1;

            v_matrix = (1.0f / 6.0f) * transformMatrix * b_matrix;

            p1 = { v_matrix(0, 0), v_matrix(0, 1), v_matrix(0, 2) };
            p2 = { v_matrix(1, 0), v_matrix(1, 1), v_matrix(1, 2) };
            p3 = { v_matrix(2, 0), v_matrix(2, 1), v_matrix(2, 2) };
            p4 = { v_matrix(3, 0), v_matrix(3, 1), v_matrix(3, 2) };

            evalBezierSegment(p1, p2, p3, p4, secondDimensionSpline2);

            // draw between 2 bsplines
            glBegin(GL_TRIANGLE_STRIP);
            for (int i = 0; i < secondDimensionSpline1.size(); i++) {
                glVertex3f(secondDimensionSpline1[i].x, secondDimensionSpline1[i].y, secondDimensionSpline1[i].z);
                glVertex3f(secondDimensionSpline2[i].x, secondDimensionSpline2[i].y, secondDimensionSpline2[i].z);
            }
            glEnd();
        }




        //}
    //}
}

void evalBezierSegment(Point3 p1, Point3 p2, Point3 p3, Point3 p4, std::vector<Point3>& ptvEvaluatedCurvePts) {

    float numSeg = NUMSEG;
    float step = 1 / numSeg;
    Eigen::Matrix<float, 4, 4> m_b;
    m_b << -1, 3, -3, 1,
        3, -6, 3, 0,
        -3, 3, 0, 0,
        1, 0, 0, 0;

    Eigen::Matrix<float, 4, 3> g_b;

    g_b << p1.x, p1.y, p1.z,
           p2.x, p2.y, p2.z,
           p3.x, p3.y, p3.z,
           p4.x, p4.y, p4.z;

    Eigen::Matrix<float, 1, 4> m_t;
    Eigen::Matrix<float, 1, 3> m_point;


    for (int j = 0; j < numSeg; ++j) {
        float t = j * step;
        m_t << t*t*t, t*t, t, 1;
        m_point = m_t * m_b * g_b;

        Point3 point = { m_point(0,0), m_point(0,1), m_point(0,2) };
        ptvEvaluatedCurvePts.push_back(point);
    }
}



// ADDED
void Skyboxloadbmp(unsigned char ** texture, const unsigned int orient, const GLuint texID) {
	int width, height;
	switch (orient) {
	case 0:
		texture[orient] = readBMP("front.bmp", width, height);
		break;
	case 1:
		texture[orient] = readBMP("left.bmp", width, height);
		break;
	case 2:
		texture[orient] = readBMP("back.bmp", width, height);
		break;
	case 3:
		texture[orient] = readBMP("right.bmp", width, height);
		break;
	case 4:
		texture[orient] = readBMP("up.bmp", width, height);
		break;
	case 5:
		texture[orient] = readBMP("bottom.bmp", width, height);
		break;
	}
	if (!texture[orient]) {
		printf("load texture failed!\n");
	}
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, texture[orient]);
}


void drawSkybox()
{
	ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();

	if (mds->m_rayFile)
	{
		// Skip
	}
	else {
		unsigned char *texture[6]; // front - 0 left - 1 back - 2 right - 3 up - 4 down - 5
		//int tex_w[6], tex_h[6];
		GLuint textureID[6];
		glGenTextures(6, textureID);
		//texture[0] = readBMP("front", tex_w[0], tex_h[0]);
		for (unsigned int i = 0; i < 6; ++i) Skyboxloadbmp(texture, i, textureID[i]);
		
		int savemode;
		glGetIntegerv(GL_MATRIX_MODE, &savemode);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		glScaled(50, 50, 50);
		
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		// Just in case we set all vertices to white.
		// glColor4f(1, 1, 1, 1);

		// Render the front quad
		glBindTexture(GL_TEXTURE_2D, textureID[0]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(0.5f, -0.5f, -0.5f);
		glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
		glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
		glTexCoord2f(0, 1); glVertex3f(0.5f, 0.5f, -0.5f);
		glEnd();

		// Render the left quad
		glBindTexture(GL_TEXTURE_2D, textureID[1]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(0.5f, -0.5f, 0.5f);
		glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, -0.5f);
		glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, -0.5f);
		glTexCoord2f(0, 1); glVertex3f(0.5f, 0.5f, 0.5f);
		glEnd();

		// Render the back quad
		glBindTexture(GL_TEXTURE_2D, textureID[2]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
		glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, 0.5f);
		glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, 0.5f);
		glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, 0.5f);

		glEnd();

		// Render the right quad
		glBindTexture(GL_TEXTURE_2D, textureID[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
		glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
		glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
		glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
		glEnd();

		// Render the top quad
		glBindTexture(GL_TEXTURE_2D, textureID[4]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
		glTexCoord2f(0, 0); glVertex3f(-0.5f, 0.5f, 0.5f);
		glTexCoord2f(1, 0); glVertex3f(0.5f, 0.5f, 0.5f);
		glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, -0.5f);
		glEnd();

		// Render the bottom quad
		glBindTexture(GL_TEXTURE_2D, textureID[5]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
		glTexCoord2f(0, 1); glVertex3f(-0.5f, -0.5f, 0.5f);
		glTexCoord2f(1, 1); glVertex3f(0.5f, -0.5f, 0.5f);
		glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, -0.5f);
		glEnd();

		// Restore enable bits and matrix
		glDisable(GL_TEXTURE_2D);
		glPopAttrib();
		glPopMatrix();
		glMatrixMode(savemode);
	}
}

Vec3d getColorfromBMP(unsigned char * bitmap, int bwidth, int bheight, int x, int y) {
	if (x >= 0 && y >= 0 && x < bwidth && y < bheight) {
		unsigned char * pixel = bitmap + (x + y * bwidth) * 3;
		unsigned char r = *pixel;
		unsigned char g = *(pixel + 1);
		unsigned char b = *(pixel + 2);
		return Vec3d(r, g, b);
	}
	else {
		printf("bmp access out of bound!\n");
		return Vec3d(-1, -1, -1);
	}
}

double getIntensityfromBMP(unsigned char * bitmap, int bwidth, int bheight, int x, int y) {
	Vec3d color = getColorfromBMP(bitmap, bwidth, bheight, x, y);
	return 0.299*color[0] + 0.587*color[1] + 0.114*color[2];
}

void drawHeightfield()
{
	// load file and store data
	std::vector<Vec3d> heightmap;
	std::vector<Vec3d> colormap;
	int height = 0, width = 0;
	unsigned char * hfIntensity = readBMP("hf_map1.bmp", width, height);
	unsigned char * hfColor = readBMP("hf_map2.bmp", width, height);
	if (!hfIntensity || !hfColor) { printf("fail to load height field BMP!\n"); return; }
	for (unsigned int h = 0; h < height; ++h) {
		for (unsigned int w = 0; w < width; ++w) {
			Vec3d temp(2 * (double)w / (double)width, 2 * double(h) / double(height), getIntensityfromBMP(hfIntensity, width, height, w, h) / 255);
			heightmap.push_back(temp);
			colormap.push_back(getColorfromBMP(hfColor, width, height, w, h));
		}
	}

	// generate triangle mesh
	for (unsigned int h = 0; h < height - 1; ++h) {
		for (unsigned int w = 0; w < width - 1; ++w) {
			// this point set as the top left of the square
			int TL = w + h * width;
			int TR = TL + 1;
			int BL = TL + width;
			int BR = BL + 1;

			// the top left triangle
			Vec3d color = (colormap[TL] + colormap[TR] + colormap[BL]) / 3.0 /255.0;
			//color = { 1.0f,0,0 };
			//cout << color << "\n";
			setDiffuseColor(color[0], color[1], color[2]);
			drawTriangle(heightmap[TL][0], heightmap[TL][1], heightmap[TL][2],
				heightmap[TR][0], heightmap[TR][1], heightmap[TR][2],
				heightmap[BL][0], heightmap[BL][1], heightmap[BL][2]);

			// the bottom right triangle
			color = (colormap[BR] + colormap[TR] + colormap[BL]) / 3.0 / 255.0;
			//color = { 0,1.0f,0 };
			//cout << color << "\n";
			setDiffuseColor(color[0], color[1], color[2]);
			drawTriangle(heightmap[BR][0], heightmap[BR][1], heightmap[BR][2],
				heightmap[TR][0], heightmap[TR][1], heightmap[TR][2],
				heightmap[BL][0], heightmap[BL][1], heightmap[BL][2]);

		}
	}

}

void drawTorus(double R, double r) {
    double s, t, x, y, z;
    double twoPi = 2 * M_PI;

    int numt = 32;

    ModelerDrawState *mds = ModelerDrawState::Instance();
    switch (mds->m_quality) {
    case HIGH:
        numt = 16; break;
    case MEDIUM:
        numt = 14; break;
    case LOW:
        numt = 10; break;
    case POOR:
        numt = 8; break;
    }

    int numc = 2 * numt;

    for (int i = 0; i < numc; i++) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= numt; j++) {
            for (int k = 1; k >= 0; k--) {
                s = (i + k) % numc + 0.5;
                t = j % numt;

                x = (R + r * cos(s / numc * twoPi))*cos(t / numt * twoPi);
                y = (R + r * cos(s / numc * twoPi))*sin(t / numt * twoPi);
                z = r * sin(s * twoPi / numc);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
}




double metaballFunc(double x0, double y0, double z0, double x, double y, double z) {
    if (x - x0 == 0 && y - y0 == 0 && z - z0 == 0) { return INFINITY; }
    return 1 / (pow(x - x0, 2) + pow(y - y0, 2) + pow(z - z0, 2));
}

XYZ VertexInterp(double threshold, XYZ v1, XYZ v2, double val1, double val2) {
    double diff1 = fabs(threshold - val1);
    double diff2 = fabs(threshold - val2);

    return{
        (v1.x * diff2 + v2.x * diff1) / (diff1 + diff2),
        (v1.y * diff2 + v2.y * diff1) / (diff1 + diff2),
        (v1.z * diff2 + v2.z * diff1) / (diff1 + diff2)
    };
}

void triangulize(double threshold, Cell grid) {
    int cubeindex;
    XYZ vertlist[12];

    int edgeTable[256] = {
        0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
        0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
        0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
        0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
        0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
        0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
        0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
        0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
        0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
        0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
        0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
        0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
        0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
        0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
        0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
        0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
        0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
        0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
        0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
        0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
        0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
        0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
        0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
        0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
        0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
        0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
        0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
        0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
        0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
        0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
        0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
        0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0 };
    int triTable[256][16] =
    { { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
    { 8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1 },
    { 3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1 },
    { 4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
    { 4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1 },
    { 9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1 },
    { 10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1 },
    { 5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1 },
    { 5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1 },
    { 8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1 },
    { 2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
    { 2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1 },
    { 11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1 },
    { 5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1 },
    { 11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1 },
    { 11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1 },
    { 2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1 },
    { 6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
    { 3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1 },
    { 6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1 },
    { 6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1 },
    { 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1 },
    { 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1 },
    { 3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1 },
    { 0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1 },
    { 9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1 },
    { 8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1 },
    { 5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1 },
    { 0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1 },
    { 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1 },
    { 10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
    { 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1 },
    { 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1 },
    { 3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1 },
    { 6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1 },
    { 9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1 },
    { 8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1 },
    { 3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1 },
    { 10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1 },
    { 10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
    { 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1 },
    { 7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1 },
    { 2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1 },
    { 1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1 },
    { 11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1 },
    { 8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1 },
    { 0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1 },
    { 7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1 },
    { 7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1 },
    { 10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1 },
    { 0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1 },
    { 7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1 },
    { 6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1 },
    { 4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1 },
    { 10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1 },
    { 8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1 },
    { 1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1 },
    { 10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1 },
    { 10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1 },
    { 9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1 },
    { 7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1 },
    { 3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1 },
    { 7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1 },
    { 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1 },
    { 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1 },
    { 9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1 },
    { 1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1 },
    { 4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1 },
    { 7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1 },
    { 6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1 },
    { 0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1 },
    { 6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1 },
    { 0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1 },
    { 11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1 },
    { 6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1 },
    { 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1 },
    { 9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1 },
    { 1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1 },
    { 10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1 },
    { 0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1 },
    { 11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1 },
    { 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1 },
    { 7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1 },
    { 2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1 },
    { 9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1 },
    { 9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1 },
    { 1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1 },
    { 0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1 },
    { 10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1 },
    { 2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1 },
    { 0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1 },
    { 0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1 },
    { 9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1 },
    { 5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1 },
    { 5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1 },
    { 8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1 },
    { 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1 },
    { 1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1 },
    { 3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1 },
    { 4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1 },
    { 9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1 },
    { 11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1 },
    { 2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1 },
    { 9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1 },
    { 3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1 },
    { 1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1 },
    { 4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1 },
    { 0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1 },
    { 9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1 },
    { 1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } };

    /*
    Determine the index into the edge table which
    tells us which vertices are inside of the surface
    */
    cubeindex = 0;
    if (grid.val[0] < threshold) cubeindex |= 1;
    if (grid.val[1] < threshold) cubeindex |= 2;
    if (grid.val[2] < threshold) cubeindex |= 4;
    if (grid.val[3] < threshold) cubeindex |= 8;
    if (grid.val[4] < threshold) cubeindex |= 16;
    if (grid.val[5] < threshold) cubeindex |= 32;
    if (grid.val[6] < threshold) cubeindex |= 64;
    if (grid.val[7] < threshold) cubeindex |= 128;

    /* Cube is entirely in/out of the surface */
    if (edgeTable[cubeindex] == 0)
        return;

    /* Find the vertices where the surface intersects the cube */
    if (edgeTable[cubeindex] & 1)
        vertlist[0] =
        VertexInterp(threshold, grid.p[0], grid.p[1], grid.val[0], grid.val[1]);
    if (edgeTable[cubeindex] & 2)
        vertlist[1] =
        VertexInterp(threshold, grid.p[1], grid.p[2], grid.val[1], grid.val[2]);
    if (edgeTable[cubeindex] & 4)
        vertlist[2] =
        VertexInterp(threshold, grid.p[2], grid.p[3], grid.val[2], grid.val[3]);
    if (edgeTable[cubeindex] & 8)
        vertlist[3] =
        VertexInterp(threshold, grid.p[3], grid.p[0], grid.val[3], grid.val[0]);
    if (edgeTable[cubeindex] & 16)
        vertlist[4] =
        VertexInterp(threshold, grid.p[4], grid.p[5], grid.val[4], grid.val[5]);
    if (edgeTable[cubeindex] & 32)
        vertlist[5] =
        VertexInterp(threshold, grid.p[5], grid.p[6], grid.val[5], grid.val[6]);
    if (edgeTable[cubeindex] & 64)
        vertlist[6] =
        VertexInterp(threshold, grid.p[6], grid.p[7], grid.val[6], grid.val[7]);
    if (edgeTable[cubeindex] & 128)
        vertlist[7] =
        VertexInterp(threshold, grid.p[7], grid.p[4], grid.val[7], grid.val[4]);
    if (edgeTable[cubeindex] & 256)
        vertlist[8] =
        VertexInterp(threshold, grid.p[0], grid.p[4], grid.val[0], grid.val[4]);
    if (edgeTable[cubeindex] & 512)
        vertlist[9] =
        VertexInterp(threshold, grid.p[1], grid.p[5], grid.val[1], grid.val[5]);
    if (edgeTable[cubeindex] & 1024)
        vertlist[10] =
        VertexInterp(threshold, grid.p[2], grid.p[6], grid.val[2], grid.val[6]);
    if (edgeTable[cubeindex] & 2048)
        vertlist[11] =
        VertexInterp(threshold, grid.p[3], grid.p[7], grid.val[3], grid.val[7]);

    /* Draw the triangle */
    for (int i = 0; triTable[cubeindex][i] != -1; i += 3) {

        drawTriangle(
            vertlist[triTable[cubeindex][i]].x, vertlist[triTable[cubeindex][i]].y, vertlist[triTable[cubeindex][i]].z,
            vertlist[triTable[cubeindex][i + 1]].x, vertlist[triTable[cubeindex][i + 1]].y, vertlist[triTable[cubeindex][i + 1]].z,
            vertlist[triTable[cubeindex][i + 2]].x, vertlist[triTable[cubeindex][i + 2]].y, vertlist[triTable[cubeindex][i + 2]].z
        );
    }
}

void drawMetaball(double threshold, const double r, double(*metaballFunc)(double x, double y, double z)) {
    // r is the radius of the metaball, 
    // r should be positive
    double border = 0.2;
    double cellSize = VAL(METABALL_RESOLUTION);

    for (double x = -r - border; x <= r + border; x += cellSize) {
        for (double y = -r - border; y <= r + border; y += cellSize) {
            for (double z = -r - border; z <= r + border; z += cellSize) {
                XYZ vertices[8] = {
                    { x, y, z },
                    { x + cellSize, y, z },
                    { x + cellSize, y, z + cellSize },
                    { x, y, z + cellSize },
                    { x, y + cellSize, z },
                    { x + cellSize, y + cellSize, z },
                    { x + cellSize, y + cellSize, z + cellSize },
                    { x, y + cellSize, z + cellSize }
                };

                double values[8];
                for (int i = 0; i < 8; i++) {
                    values[i] = (*metaballFunc)(
                        vertices[i].x, vertices[i].y, vertices[i].z);
                }

                Cell grid(vertices, values);
                triangulize(threshold, grid);
            }
        }
    }
}


Mat4f getModelViewMatrix()
{
	/**************************
	**
	**	GET THE OPENGL MODELVIEW MATRIX
	**
	**	Since OpenGL stores it's matricies in
	**	column major order and our library
	**	use row major order, we will need to
	**	transpose what OpenGL gives us before returning.
	**
	**	Hint:  Use look up glGetFloatv or glGetDoublev
	**	for how to get these values from OpenGL.
	**
	*******************************/

	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	Mat4f matMV(m[0], m[1], m[2], m[3],
		m[4], m[5], m[6], m[7],
		m[8], m[9], m[10], m[11],
		m[12], m[13], m[14], m[15]);

	return matMV.transpose(); // convert to row major
}