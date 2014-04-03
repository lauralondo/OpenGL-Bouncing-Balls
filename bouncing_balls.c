/* londo_hw3.c
 * Laura Londo
 * 24 February 2014
 * HW #3
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include <string.h>

float WINDOW_SIZE = 700;
char* debug = "debug";
int totalNum = 10;			//total number of circles (10 by default)
float radius = 0.1;			//circle radius
int waitTime = 16;			//milliseconds between steps
//coordinates
float x[10];
float y[10];
//velocity
float vx[10];
float vy[10];
//new velocity
float newvx[10];
float newvy[10];
//color
float colorR[10];
float colorG[10];
float colorB[10];

float glowAlph[10];

int lights = 1;
float lightGlow = 0;


//Function to write a string to the screen at a specified location
void bitmapText(float x, float y, char* words) {
	int len = 0, i = 0;
	//Set the location where the string should appear
	glRasterPos2f(x,y);
	len = (int) strlen(words);
	//Set the character in the string to helvetica size 18
	for(int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,words[i]);
	}
}

//creates a gl triangle fan circle of indicated radius and segments
void flatCircle(float cx, float cy, float radius, int segments) {
	float phi, x1, y1;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);					//center vertex
	for (int j = 0; j<=segments; j++) {	//for every segment,
		phi = 2 * M_PI * j / segments;	//calculate the new vertex
		x1 = radius * cos(phi) + cx;
		y1 = radius * sin(phi) + cy;
		glVertex2f(x1, y1);
	}
	glEnd();
} //end circle

//creates a gl triangle fan circle of indicated radius and segments
void circle(float cx, float cy,
			float outerR, float outerG, float outerB,
			float radius, int segments)
{
	float phi, x1, y1;
	glBegin(GL_TRIANGLE_FAN);
	//glColor3f(outerR+0.5, outerG+0.5, outerB+0.5);	//inner color
	glColor3f(outerR+0.3, outerG+0.3, outerB+0.3);	//inner color
	glVertex2f(cx, cy);								//center vertex
	glColor3f(outerR,outerG,outerB);				//outer color
	for (int j = 0; j<=segments; j++) {				//for every segment,
		phi = 2 * M_PI * j / segments;				//calculate the new vertex
		x1 = radius * cos(phi) + cx;
		y1 = radius * sin(phi) + cy;
		glVertex2f(x1, y1);
	}
	glEnd();
} //end circle

void circleOutline(float cx, float cy,
			float outerR, float outerG, float outerB,
			float radius, int segments)
{
	float phi, x1, y1;
	glBegin(GL_LINES);
	//glColor3f(outerR+0.5, outerG+0.5, outerB+0.5);	//inner color
	glColor3f(outerR+0.3, outerG+0.3, outerB+0.3);	//inner color
	//glVertex2f(cx, cy);								//center vertex
	//glColor3f(outerR,outerG,outerB);				//outer color
	for (int j = 0; j<=segments; j++) {				//for every segment,
		phi = 2 * M_PI * j / segments;				//calculate the new vertex
		x1 = radius * cos(phi) + cx;
		y1 = radius * sin(phi) + cy;
		glVertex2f(x1, y1);
		phi = 2 * M_PI * (j+1) / segments;				//calculate the new vertex
		x1 = radius * cos(phi) + cx;
		y1 = radius * sin(phi) + cy;
		glVertex2f(x1, y1);
	}
	glEnd();
} //end circle

void glow(float cx, float cy,
			float outerR, float outerG, float outerB, float outerA,
			float radius, int segments)
{
	float phi, x1, y1;
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(outerR+0.5, outerG+0.5, outerB+0.5, outerA); //inner color
	glVertex2f(cx, cy);					//center vertex
	glColor4f(outerR,outerG,outerB,0.0);	//outer color
	for (int j = 0; j<=segments; j++) {	//for every segment,
		phi = 2 * M_PI * j / segments;	//calculate the new vertex
		x1 = radius * cos(phi) + cx;
		y1 = radius * sin(phi) + cy;
		glVertex2f(x1, y1);
	}
	glEnd();
} //end circle

void shadow(float cx, float cy,
			float outerR, float outerG, float outerB, float outerA,
			float radius, int segments)
{
	float phi, x1, y1;
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.0, 0.0, 0.0, 1.0); //inner color
	glVertex2f(cx, cy);					//center vertex
	glColor4f(outerR,outerG,outerB,0.0);	//outer color
	for (int j = 0; j<=segments; j++) {	//for every segment,
		phi = 2 * M_PI * j / segments;	//calculate the new vertex
		x1 = radius * cos(phi) + cx;
		y1 = radius * sin(phi) + cy;
		glVertex2f(x1, y1);
	}
	glEnd();
} //end circle

//initializes all circle posiitons, colors, and velocities
void initCircles(void) {
	srand(time(NULL));							// seed the random number generator
	for(int i=0; i<totalNum; i++){				// for each circle,
		//current position
		x[i] = ((rand()%(int)(200-(radius*200)))/100.0)-(1.0-radius); //  random number between
		y[i] = ((rand()%(int)(200-(radius*200)))/100.0)-(1.0-radius); //    -0.9 and 0.9 (to account for radius size)
		//color values
		colorR[i] = (rand()%10)/10.0;			//	random color values
		colorG[i] = (rand()%10)/10.0;			//	  between 0 and 1
		colorB[i] = (rand()%10)/10.0;
		//velocity
		vx[i] = ((rand()%200)/10000.0)-0.01; 	//	random velocities between
		vy[i] = ((rand()%200)/10000.0)-0.01; 	//	  -0.02 and 0.02

		glowAlph[i] = 0.0;
	}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	if(lights) {
		//3 background colors
		glBegin(GL_POLYGON);
		glColor3f(0.42, 1.0, 0.87);		//light blue
		glVertex2f(-1.0, -1.0);
		glVertex2f(-1.0, 1.0);
		glColor3f(0.14, 0.62, 0.76); 	//darker blue
		glVertex2f((-1.0/3.0), 1.0);
		glVertex2f((-1.0/3.0), -1.0);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.69, 0.29, 0.74);	//purple
		glVertex2f((-1.0/3.0), -1.0);
		glVertex2f((-1.0/3.0), 1.0);
		glColor3f(0.92, 0.33, 0.38);	//reddish
		glVertex2f((1.0/3.0), 1.0);
		glVertex2f((1.0/3.0), -1.0);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.99, 0.54, 0.41);	//orange
		glVertex2f((1.0/3.0), -1.0);
		glVertex2f((1.0/3.0), 1.0);
		glColor3f(1.0, 0.82, 0.53);		//yellow
		glVertex2f(1.0, 1.0);
		glVertex2f(1.0, -1.0);
		glEnd();

		//help text
		glColor3f(0.92/3.0, 0.33/3.0, 0.38/3.0);	//reddish
		bitmapText(-0.45, 0.85, "press <spacebar> to turn out the light!");

		//shadows
		for (int i=0; i<totalNum; i++) {
			glColor4f(0.0,0.0,0.0,0.5);
			flatCircle(x[i]+x[i]/15,y[i]+(y[i]-1.0)/20, radius, 30);
		}

		//bsll body
		for (int i=0; i<totalNum; i++) {
			float gAlph = glowAlph[i]-0.3;
			circle(x[i], y[i], colorR[i]+gAlph, colorG[i]+gAlph, colorB[i]+gAlph, radius,30);
			if(glowAlph[i] > 0.0) {
				glowAlph[i] -= 0.02;
				glow(x[i], y[i], colorR[i]+glowAlph[i],colorG[i]+glowAlph[i],colorB[i]+glowAlph[i],glowAlph[i], radius+.1,30);
			}
		}

		//desk light. blinks when balls hit top of the screen
		glow(0, 1.0,  1,1,1, 0.8+lightGlow, 1, 30);
		if(lightGlow > 0.0) {
			lightGlow-=0.04;
		}

	}

	//lights out mode
	else {
		//corner lights
		glow(-1.0, 1.0,   -0.08,0.5,0.37, 0.3+lightGlow,	0.2,	30);
		glow( 1.0, 1.0,   0.5, 0.32, 0.03, 0.3+lightGlow,	0.2,	30);
		glow( 1.0,-1.0,   0.5, 0.32, 0.03, 0.3+lightGlow,	0.2,	30);
		glow(-1.0,-1.0,   -0.08,0.5,0.37, 0.3+lightGlow,	0.2,	30);

		//help text
		glColor4f(0.38, 0.92, 0.38, 0.1+lightGlow);
		bitmapText(-0.45, 0.85, "press <spacebar> to turn on the light!");
		if(lightGlow > 0.0) {
			lightGlow-=0.04;
		}


		//circle outlines
		for (int i=0; i<totalNum; i++) {
			glow(x[i],y[i],  colorR[i],colorG[i],colorB[i], 1.0, radius+.04, 30);
			glColor3f(0,0,0);
			flatCircle(x[i],y[i], radius,30);
			circleOutline(x[i], y[i], colorR[i]+glowAlph[i], colorG[i]+glowAlph[i], colorB[i]+glowAlph[i], radius,30);
		}

		//collision glow
		for (int i=0; i<totalNum; i++) {
			if(glowAlph[i] > 0.0) {
				glowAlph[i] -= 0.02;
				glow(x[i], y[i], colorR[i]+glowAlph[i],colorG[i]+glowAlph[i],colorB[i]+glowAlph[i],glowAlph[i], radius+.1,30);
			}

		}

	}

	//glColor3f(0.0,0.0,0.0);
	//bitmapText(-0.98,0.94,debug);

	glFlush();							//flush image to the screen
}



void timer(int value) {

	//Actually move the circles
	for(int i=0; i<totalNum; i++) {
		x[i] += vx[i];
		y[i] += vy[i];
	}


	//resolve collisions
	for(int i=0; i<totalNum; i++) {	//for each ball,
		// Reverse direction when you reach edges
	    if(x[i] > 1.0 - radius ) {		//right edge
	    	x[i] = 1.0-radius;				//to prevent balls from sticking
	       	vx[i] = -vx[i];					//change velocity
	       	lightGlow = 1;					//create a glow
	    }
	    else if(x[i] < -1.0 + radius) {	//left edge
	    	x[i] = -1.0+radius;				///to prevent balls from sticking
	    	vx[i] = -vx[i];					//change velocity
	    	lightGlow = 1;					//create a glow
	    }

	    if(y[i] > 1.0 - radius){		//top edge
	    	y[i] = 1.0 - radius;			//to prevent balls from sticking
	    	vy[i] = -vy[i];					//change velocity
	    	lightGlow = 1;					//create a glow
	    }
	    else if(y[i] < -1.0 + radius) {	//bottom edge
	    	y[i] = -1.0 + radius;			//to prevent balls from sticking
	    	vy[i] = -vy[i];					//change velocity
	    	lightGlow = 1;					//create a glow
	    }


	    for(int j=i+1; j<totalNum; j++) {	//second ball

	    	//vector between the centers of two circles
	    	float dx = x[i] - x[j];
	    	float dy = y[i] - y[j];

	    	//square of its magnitude
	    	float dSq = dx*dx + dy*dy;

	    	if (dSq <= 4.0*radius*radius) {
	    		//vector of difference of velocities
	    		float dvx = vx[j] - vx[i];
	    		float dvy = vy[j] - vy[i];

	    		//dot product of d and dv
	    		float dP = dx*dvx + dy*dvy;

	    		if (dP > 0.0) { //if they are moving towards eachother
	    			debug = "moving towards eachother";
	    			float factor = dP / dSq;

	    			float xChange = dx * factor;
	    			float yChange = dy * factor;

	    			//add change to the velocity of the first circle
	    			vx[i] += xChange;
	    			vy[i] += yChange;

	    			//subtract change from the velocity of tht second circle
	    			vx[j] -= xChange;
	    			vy[j] -= yChange;

	    			glowAlph[i] = 0.8;	//create a glow in first ball
	    			glowAlph[j] = 0.8;	//create a glow in second ball
	    		}
	    		//*/
	    	}
	    } //end for j
	} //end for i

	glutPostRedisplay();
	glutTimerFunc(waitTime, timer, 1);
}

//mouse callback.
void mouse(int butt, int state, int x, int y) {
	debug = "mouse";
	if (state == GLUT_DOWN) {
		debug = "down";
		if(butt == GLUT_LEFT_BUTTON) {			//left click
			for(int i=0; i<totalNum; i++) {		//for every ball
				vx[i] /= 2;						//slow down x velocity
				vy[i] /= 2;						//slow down y velocity
			}
			glutPostRedisplay();
		}
		else if (butt == GLUT_RIGHT_BUTTON) {	//right click
			for(int i=0; i<totalNum; i++) {		//for every ball
				vx[i] *= 2;						//speed up x velocity
				vy[i] *= 2;						//speed up y velocity
			}
			glutPostRedisplay();
		}
	}
}

//The keyboard callback
void keyboard(char key, int x, int y) {
	int k = (int)key;
	if (k == 27)					//if the user types ESC
		exit(0);					//exit the program
	else if (k == 32){				//spacebar	toggles lights
		if(lights)
			lights = 0;
		else
			lights = 1;
	}
}

//reshape callback. adjusts both the clipping box and viewport. keeps proportions unchanged
void reshape(int w, int h) {
	float aspectRatio = 1.0;

	//Compute the aspect ratio of the resized window
	aspectRatio = (float)h / (float)w;

	// Adjust the clipping box
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h >= w)
		gluOrtho2D(-1.0, 1.0, -aspectRatio, aspectRatio);
	else
		gluOrtho2D(-1.0/aspectRatio, 1.0/aspectRatio, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

	//adjust the viewport
	glViewport(0, 0, w, h);
}


int main(int argc, char **argv) {
	initCircles();		//initialize circle values

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA | GLUT_ALPHA);

	glutInitWindowPosition(0,0);					//window position
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);	//window size
	glutCreateWindow ("circles!!!");				//window name
	glClearColor(0.0, 0.0, 0.0, 0.0);				//background color
	glClear(GL_COLOR_BUFFER_BIT);

	//The four following statements set up the viewing rectangle
	glMatrixMode(GL_PROJECTION);					// use proj. matrix
	glLoadIdentity();								// load identity matrix
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);				// set orthogr. proj.
	glMatrixMode(GL_MODELVIEW);						// back to modelview m.

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glutDisplayFunc(display);
	glutTimerFunc(waitTime, timer, 1);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	glutMainLoop();
  	return 0;
}
