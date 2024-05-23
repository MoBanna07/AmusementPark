#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

float PlayerX = 0;
float PlayerZ = -0.2;
float PlayerAngle = 0;

float RedFence = 0;
float GreenFence = 0;
float BlueFence = 0;
int redD = 1;
int greenD = 1;
int blueD = 1;

bool AnimKey = false;
float rotationAngle = 0.0;
float translatevalue = 0.0;
bool up = true;
float rotationAngleSwing = 0.0;
bool Swingleft = false;
float scaleCastle = 1;
bool largen = false;
float translatevalueTrain = 0;
bool forwarCarousel = true;
int timer = 3000;

float translatevalueGoal = 0.0;
bool upGoal = true;
bool goal = false;

float angle = 0.0f;

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

int currentView = 0;  // 0: Default, 1: Top View, 2: Front View, 3: Side View

void setTopView() {
	camera = Camera(0.5f, 12.5f, -6.25f, 0.5f, 0.0f, -6.25f, 0.0f, 0.0f, -1.0f);
}

void setFrontView() {
	camera = Camera(0.5f, 0.5f, 3.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void setSideView() {
	camera = Camera(12.5f, 0.5f, -6.25f, 0.0f, 0.5f, -6.25f, 0.0f, 1.0f, 0.0f);
}

void setDefaultView() {
	camera = Camera(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void drawSphere(float x, float y, float z, float radius) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glutSolidSphere(radius, 10, 10);
	glPopMatrix();
}

void drawGround(double thickness) {
	glPushMatrix();
	glTranslated(0.5, 0.5 , -6.25);
	glScaled(5.0, thickness, 12.5);
	glColor3f(0.1, 1, 0.4);
	glutSolidCube(1);
	glPopMatrix();
}

void drawFenceLeg(double thick, double len) {
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawFence(double x, double z) {
	glPushMatrix();
	glTranslated(x, 0.5, z);
	drawFenceLeg(0.02, 0.5);
	glTranslated(0, 0, -12.5);
	for (int i = 0;i < 25;i++) {
		glTranslated(0, 0, 0.5);
		drawFenceLeg(0.02, 0.5);
	}
	glPopMatrix();
	glPushMatrix();
	glTranslated(x, 0.75, -12.5);
	GLUquadric* quad = gluNewQuadric();
	gluCylinder(quad, 0.01, 0.01, 12.5, 20, 20);
	glTranslated(0, 0.25, 0);
	gluCylinder(quad, 0.01, 0.01, 12.5, 20, 20);
	glPopMatrix();
}

void drawPlayer(float x, float y, float z) {
	// Body
	glColor3f(1.0, 1.0, 1.0); // White color for the player's body
	glPushMatrix();
	glTranslatef(x, y, z);
	glScaled(0.2, 0.2, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	// Head
	glColor3f(0.7, 0.6, 0.3); // Human color for the player's head
	glPushMatrix();
	glTranslatef(x, y+0.1, z);
	glutSolidSphere(0.1, 10, 10);
	glColor3f(0.7, 0.6, 0.3); // Human color for the player's nose
	glTranslatef(0, 0, 0.1);
	glutSolidCone(0.01, 0.05, 10, 10);
	glPopMatrix();

	// Arms
	glColor3f(1.0, 1.0, 1.0); // White color for the player's arms
	// Right arm
	glPushMatrix();
	glTranslated(x-0.15, y+0, z+0);
	glScaled(0.09, 0.15, 0.09);
	glutSolidCube(1.0);
	glPopMatrix();
	// Left arm
	glPushMatrix();
	glTranslated(x+0.15, y+0, z+0);
	glScaled(0.09, 0.15, 0.09);
	glutSolidCube(1.0);
	glPopMatrix();

	// Legs
	glColor3f(0.0, 0.0, 0.0); // Black color for the player's legs
	// Right leg
	glPushMatrix();
	glTranslated(x-0.1, y-0.1, z+0);
	glScaled(0.1, 0.2, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	// Left leg
	glPushMatrix();
	glTranslated(x+0.1, y-0.1, z+0);
	glScaled(0.1, 0.2, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
}

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void drawCylinder(float radius, float height) {
	GLUquadric* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluCylinder(quad, radius, radius, height, 20, 20);
	gluDeleteQuadric(quad);
}

void drawFerrisWheel(float baseHeight, float pillarRadius, float pillarHeight, float armWidth, float armHeight, float armSpacing, float carSize, float xPos, float yPos, float zPos) {
	glPushMatrix();
	glTranslatef(xPos, yPos, zPos);

	glColor3f(1, 1, 1);  // Black color for the structure

	// Central Pillar
	glTranslatef(0.0, 0.0, (baseHeight / 2) - 0.1 );
	drawCylinder(pillarRadius, 1.2);

	// Ferris Wheel Arms (using cubes)
	glColor3f(0.5, 0.5, 0.5);  // Grey color for the arms
	for (int i = 0; i < 8; ++i) {
		glPushMatrix();
		glRotatef(rotationAngle + i * 45.0, 0.0, 0.0, 1.0);
		glTranslatef(0.0, (pillarRadius + armSpacing), pillarHeight / 2);
		glRotatef(90.0, 1.0, 0.0, 0.0);  // Rotate to align with Z-axis
		glScaled(1, 1, 10);
		glutSolidCube(armWidth); // Using glutSolidCube to draw cubes
		glPopMatrix();
	}

	// Ferris Wheel Cars (simple cubes for representation)
	glColor3f(1.0, 0.0, 0.0);  // Red color for the cars
	for (int i = 0; i < 8; ++i) {
		glPushMatrix();
		glRotatef(rotationAngle + i * 45.0, 0.0, 0.0, 1.0);
		glTranslatef(0.0, (pillarRadius + armSpacing + armHeight / 2 + carSize / 2), pillarHeight / 2);
		glutSolidCube(carSize);
		glPopMatrix();
	}

	glPopMatrix();
}

void drawFerrisWheelWithStand(float baseHeight, float pillarRadius, float pillarHeight, float armWidth, float armHeight, float armSpacing, float carSize, float xPos, float yPos, float zPos) {
	// Stand Base (Triangle)
	glPushMatrix();	
	glTranslatef(xPos, yPos, zPos);
	glColor3f(0, 0, 0);  // Black color for the stand base
	glScaled(1, 25, 1);
	glutSolidCube(0.1); // Using glutSolidCube to draw cubes
	glPopMatrix();

	// Ferris Wheel
	glTranslatef(0.0, 0.45, 0.0);
	drawFerrisWheel(baseHeight, pillarRadius, pillarHeight, armWidth, armHeight, armSpacing, carSize, xPos, yPos, zPos);

}

void drawCarouselCarnivalGame(float baseRadius, float poleHeight, float topRadius, float platformHeight, int numSeats, float xPos, float yPos, float zPos) {
	glPushMatrix();
	glTranslatef(xPos, yPos, zPos);

	// Pole
	glColor3f(1.0, 1.0, 1.0);  // Grey color for the pole
	drawCylinder(0.1, poleHeight+1);

	//Cone
	glPushMatrix();
	glColor3f(0.8, 0.7, 0.5);
	glTranslatef(0, 0, poleHeight-0.6);
	glRotatef(180, 1, 0, 0);
	glutSolidCone(baseRadius+0.5, poleHeight-0.4,100,100);
	glPopMatrix();

	// Fence
	glPushMatrix();
    glTranslatef(0.0, 0.0, poleHeight + 0.5);
    glColor3f(0.5, 0.5, 0.5);  // Grey color for the Fence
    drawCylinder(baseRadius+0.2, 0.7);
	glPopMatrix();

	// Inner Seats 
	glTranslatef(0.0, 0.0, poleHeight);
	glPushMatrix();
	glRotatef(rotationAngle, 0, 0, 1);
	for (int i = 0; i < numSeats; ++i) {
		float angle = i * (360.0 / numSeats);
		float seatX = topRadius * cos(angle * M_PI / 180.0);
		float seatY = topRadius * sin(angle * M_PI / 180.0);
		glPushMatrix();
		glTranslatef(0, 0, translatevalue);
		glColor3f(1.0, 0.0, 0.0);  // Red color for the seats
		glTranslatef(seatX, seatY, platformHeight / 2);
		glutSolidCube(0.2);
		glPopMatrix();
		//Holder
		glPushMatrix();
		glTranslatef(seatX, seatY, platformHeight / 2);
		glColor3f(0.8, 0.7, 0.5);
		glScalef(0.5, 0.5, 15.9 + 3.9);
		glTranslatef(0, 0, -0.055 + 0.039);
		glutSolidCube(0.1);
		glPopMatrix();
	}
	glPopMatrix();

	glTranslatef(0.0, 0.0, -0.05);

	// Outer Seats
	glRotatef(rotationAngle, 0, 0, 1);
	for (int i = 0; i < numSeats; ++i) {
		float angle = i * (360.0 / numSeats);
		float seatX = baseRadius * cos(angle * M_PI / 180.0);
		float seatY = baseRadius * sin(angle * M_PI / 180.0);
		//Seat
		glPushMatrix();
		glTranslatef(0, 0, translatevalue);
		glColor3f(1.0, 0.0, 0.0);  // Red color for the seats
		glTranslatef(seatX, seatY, platformHeight / 2);
		glutSolidCube(0.2);
		glPopMatrix();
		//Holder
		glPushMatrix();
		glTranslatef(seatX, seatY, platformHeight / 2);
		glColor3f(0.8, 0.7, 0.5);
		glScalef(0.5, 0.5, 10 + 5);
		glTranslatef(0, 0, -0.055 + 0.05);
		glutSolidCube(0.1);
		glPopMatrix();
	}

	glPopMatrix();
}

void drawCylinder(float radius, float height, int segments) {
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= segments; ++i) {
		float angle = i * 2.0f * M_PI / segments;
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		glVertex3f(x, y, 0.0);
		glVertex3f(x, y, height);
	}
	glEnd();

	// Draw the top and bottom circles
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0, 0.0, 0.0);  // Center of the bottom circle
	for (int i = 0; i <= segments; ++i) {
		float angle = i * 2.0f * M_PI / segments;
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		glVertex3f(x, y, 0.0);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0, 0.0, height);  // Center of the top circle
	for (int i = 0; i <= segments; ++i) {
		float angle = i * 2.0f * M_PI / segments;
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		glVertex3f(x, y, height);
	}
	glEnd();
}

void drawTree(float trunkHeight, float trunkRadius, float crownRadius, float xPos, float yPos, float zPos) {
	glPushMatrix();
	glTranslatef(xPos, yPos, zPos);

	// Trunk
	glColor3f(0.5, 0.35, 0.15);  // Brown color for the trunk
	drawCylinder(trunkRadius, trunkHeight, 20);

	// Crown
	glTranslatef(0.0, 0.0, trunkHeight);
	glColor3f(0.0, 0.5, 0.0);  // Green color for the leaves
	glutSolidSphere(crownRadius, 20, 20);
	glPushMatrix();
	
	glTranslatef(0, (- crownRadius / 2) - 0.25, 0);
	glColor3f(1, 0, 0);
	glutSolidSphere(0.05, 10, 10);
	glColor3f(0, 1, 0);
	glTranslatef(0, 0, 0.05);
	glutSolidCube(0.01);
	glPopMatrix();


	//apples
	glTranslatef(0, -crownRadius/2, - trunkHeight+0.05);
	glColor3f(1, 0, 0);
	glutSolidSphere(0.05, 10, 10);
	glColor3f(0, 1, 0);
	glTranslatef(0, 0, 0.05);
	glutSolidCube(0.01);

	glTranslatef(-0.2, 0, -0.05);
	glColor3f(1, 0, 0);
	glutSolidSphere(0.05, 10, 10);
	glColor3f(0, 1, 0);
	glTranslatef(0, 0, 0.05);
	glutSolidCube(0.01);
	
	glTranslatef(0, 0.2, -0.05);
	glColor3f(1, 0, 0);
	glutSolidSphere(0.05, 10, 10);
	glColor3f(0, 1, 0);
	glTranslatef(0, 0, 0.05);
	glutSolidCube(0.01);

	glTranslatef(0.4 , 0, -0.05);
	glColor3f(1, 0, 0);
	glutSolidSphere(0.05, 10, 10);
	glColor3f(0, 1, 0);
	glTranslatef(0, 0, 0.05);
	glutSolidCube(0.01);

	glPopMatrix();
}

void drawBoatSwing(float baseWidth, float baseLength, float poleHeight, float seatWidth, float seatLength, float ropeLength, float xPos, float yPos, float zPos) {
	glPushMatrix();
	glTranslatef(xPos, yPos, zPos);

	// Base
	glColor3f(0.5, 0.35, 0.15);  // Brown color for the base
	glPushMatrix();
	glScalef(baseWidth, baseLength, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	//Stand
	glColor3f(0, 0, 0); // Black color for the seat
	glPushMatrix();
	glTranslatef(0, -baseWidth / 2, (poleHeight+ropeLength)/2);
	glRotatef(90, 1, 0, 0);
	glScalef(0.2, poleHeight+ropeLength, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	glRotatef(rotationAngleSwing, 0, 1, 0);

	// Seat
	glColor3f(0.5, 0.35, 0.15);  // Brown color for the seat
	glPushMatrix();
	glTranslatef(0.0, 0.0, poleHeight);
	glScalef(seatWidth, seatLength, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	// Ropes
	glColor3f(0.5, 0.35, 0.15);  // Brown color for the ropes
	glPushMatrix();
	glTranslatef((seatWidth / 4), 0.0, (std::sqrt(std::pow(poleHeight, 2) + std::pow(seatWidth / 2, 2)))/2);
	glRotatef(20.0, 0.0, 1.0, 0.0);
	glScalef(0.05, 0.05, std::sqrt(std::pow(poleHeight, 2) + std::pow(seatWidth / 2, 2)));
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-(seatWidth / 4), 0.0, (std::sqrt(std::pow(poleHeight, 2) + std::pow(seatWidth / 2, 2))) / 2);
	glRotatef(-20.0, 0.0, 1.0, 0.0);
	glScalef(0.05, 0.05, std::sqrt(std::pow(poleHeight, 2) + std::pow(seatWidth / 2, 2)));
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

void drawJumpingCastle(float castleWidth, float castleLength, float castleHeight, float towerWidth, float towerHeight, float towerSpacing, float xPos, float yPos, float zPos) {
	glPushMatrix();
	glTranslatef(xPos, yPos, zPos);

	// Castle Base
	glColor3f(1.0, 0.8, 0.0);  // Yellow color for the base
	glPushMatrix();
	glScalef(castleWidth, castleLength, castleHeight);
	glutSolidCube(1.0);
	glPopMatrix();

	//Slide
	glColor3f(1.0, 0.8, 0.0);  // Yellow color for the slide
	glPushMatrix();
	glTranslatef(0, -castleWidth, -castleHeight/4);
	glScalef(castleWidth,castleLength/2,castleHeight/2);
	glRotatef(90, 1, 0, 0);
	glutSolidCube(1.0);
	glPopMatrix();


	// Towers
	glColor3f(0, 1, 0);  // Green color for the towers
	glPushMatrix();
	glTranslatef(-(castleWidth / 2) + (towerWidth / 2), -(castleLength / 2) + (towerWidth / 2), castleHeight / 2);
	glScalef(towerWidth, towerWidth, towerHeight);
	glutSolidCube(1.0);
	glColor3f(1.0, 0.8, 0.0);
	glScalef(1/towerWidth, 1/towerWidth, 1/towerHeight);
	glTranslatef(0, 0, towerHeight-0.1);
	glutSolidCone(towerWidth-0.03, 0.1, 10, 10);
	glPopMatrix();

	glColor3f(0, 1, 0);  // Green color for the towers
	glPushMatrix();
	glTranslatef((castleWidth / 2) - (towerWidth / 2), -(castleLength / 2) + (towerWidth / 2), castleHeight / 2);
	glScalef(towerWidth, towerWidth, towerHeight);
	glutSolidCube(1.0);
	glColor3f(1.0, 0.8, 0.0);
	glScalef(1 / towerWidth, 1 / towerWidth, 1 / towerHeight);
	glTranslatef(0, 0, towerHeight - 0.1);
	glutSolidCone(towerWidth - 0.03, 0.1, 10, 10);
	glPopMatrix();

	glColor3f(0, 1, 0);  // Green color for the towers
	glPushMatrix();
	glTranslatef(-(castleWidth / 2) + (towerWidth / 2), (castleLength / 2) - (towerWidth / 2), castleHeight / 2);
	glScalef(towerWidth, towerWidth, towerHeight);
	glutSolidCube(1.0);
	glColor3f(1.0, 0.8, 0.0);
	glScalef(1 / towerWidth, 1 / towerWidth, 1 / towerHeight);
	glTranslatef(0, 0, towerHeight - 0.1);
	glutSolidCone(towerWidth - 0.03, 0.1, 10, 10);
	glPopMatrix();

	glColor3f(0, 1, 0);  // Green color for the towers
	glPushMatrix();
	glTranslatef((castleWidth / 2) - (towerWidth / 2), (castleLength / 2) - (towerWidth / 2), castleHeight / 2);
	glScalef(towerWidth, towerWidth, towerHeight);
	glutSolidCube(1.0);
	glColor3f(1.0, 0.8, 0.0);
	glScalef(1 / towerWidth, 1 / towerWidth, 1 / towerHeight);
	glTranslatef(0, 0, towerHeight - 0.1);
	glutSolidCone(towerWidth - 0.03, 0.1, 10, 10);
	glPopMatrix();

	// Tower Connectors
	glColor3f(0.8, 0.3, 0);  // Orange color for the connectors

	glPushMatrix();
	glTranslatef(0.0, (castleLength / 2) - (towerWidth / 2), castleHeight / 2 + towerHeight / 2);
	glScalef(castleWidth, towerSpacing, towerHeight);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-(castleWidth / 2) + (towerWidth / 2), 0.0, castleHeight / 2 + towerHeight / 2);
	glScalef(towerSpacing, castleLength - towerWidth * 2, towerHeight);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef((castleWidth / 2) - (towerWidth / 2), 0.0, castleHeight / 2 + towerHeight / 2);
	glScalef(towerSpacing, castleLength - towerWidth * 2, towerHeight);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

void drawFinishLine(float lineLength, float lineWidth, float lineHeight, float xPos, float yPos, float zPos) {
	glPushMatrix();
	glTranslatef(xPos, yPos, zPos);

	if (!goal) {
		// Finish Line Base
		glColor3f(0.8, 0.8, 0.8);  // Light grey color for the base
		glPushMatrix();
		glTranslatef(0, translatevalueGoal, 0);
		glScalef(lineLength, lineWidth, lineHeight);
		glutSolidCube(1.0);
		glPopMatrix();

		// Finish Line Top
		glColor3f(1.0, 0.0, 0.0);  // Red color for the top
		glPushMatrix();
		glTranslatef(0, translatevalueGoal, 0);
		glTranslatef(0.0, 0.0, lineHeight / 2 + 0.1);  // Slightly above the base
		glScalef(lineLength, lineWidth, 0.2);
		glutSolidCube(1.0);
		glPopMatrix();
	}
	//Poles
	//Right Pole
	glColor3f(1.0, 1.0, 1.0);  // White color for the top
	glPushMatrix();
	glTranslatef(lineLength/2, 0.25, 0.0);  // Slightly above the base
	glScalef(0.1, 1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	//Right Pole
	glColor3f(1.0, 1.0, 1.0);  // White color for the top
	glPushMatrix();
	glTranslatef(-lineLength / 2, 0.25, 0.0);  // Slightly above the base
	glScalef(0.1, 1, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();


	glPopMatrix();
}

void drawNumber3D(float x, float y, float z, float width, float height, int number) {
	glPushMatrix();
	glTranslatef(x, y, z);  // Set the position for the text
	glColor3f(1, 1, 1);

	// Scale the text to match the specified dimensions
	glScalef(width / 100.0f, height / 100.0f, 10.0f);

	// Convert the number to a string
	std::string numberString = std::to_string(number);

	// Loop through each character in the string and display it
	for (char& c : numberString) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}

	glPopMatrix();
}

void drawString3D(float x, float y, float z, float width, float height, const std::string& text) {
	glPushMatrix();
	glTranslatef(x, y, z);  // Set the position for the text
	glColor3f(1, 1, 1);

	// Scale the text to match the specified dimensions
	glScalef(width / 100.0f, height / 100.0f, 1.0f);

	// Loop through each character in the string and display it
	for (char c : text) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}

	glPopMatrix();
}

void drawCandyCart(float x, float y, float z, float width, float height, float depth) {
	// Draw the base of the cart
	glColor3f(0.8, 0.5, 0.2);  // Brown color for the base
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(width, height, depth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw the roof of the cart
	glColor3f(0.9, 0.7, 0.3);  // Light brown color for the roof
	glPushMatrix();
	glTranslatef(x, y + height, z);
	glScalef(width, height * 0.2, depth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw some decorations
	glColor3f(1.0, 0.0, 0.0);  // Red color for decorations
	glPushMatrix();
	glTranslatef(x - width * 0.4, y + height * 0.5, z + depth * 0.5);
	glRotatef(45.0, 0.0, 1.0, 0.0);
	glutSolidCube(height * 0.1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(x + width * 0.4, y + height * 0.5, z + depth * 0.5);
	glRotatef(-45.0, 0.0, 1.0, 0.0);
	glutSolidCube(height * 0.1);
	glPopMatrix();
}

void drawTrain(float x, float y, float z, float length, float height, float width) {
	// Draw the train body
	glColor3f(0.8, 0.2, 0.2);  // Red color for the train body
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(length, height, width);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw the train cabin
	glColor3f(0.2, 0.2, 0.8);  // Blue color for the train cabin
	glPushMatrix();
	glTranslatef(x + length * 0.4, y + height * 0.2, z);
	glScalef(length * 0.4, height * 0.6, width * 0.8);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw the train chimney
	glColor3f(0.2, 0.8, 0.2);  // Green color for the chimney
	glPushMatrix();
	glTranslatef(x + length * 0.7, y + height * 0.6, z);
	glScalef(width * 0.2, height * 0.4, width * 0.2);
	glutSolidCube(1.0);
	glPopMatrix();
}

void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (timer <= 0) {

		glPushMatrix();
		if (goal) {
			drawString3D(-5.7, 4, -8, 2, 2, "You Win!");
			bool played = PlaySound(TEXT("Win.wav"), nullptr, SND_SYNC);
		}
		else {
			drawString3D(-5.7, 4, -8, 2, 2, "You Lose!");
			bool played = PlaySound(TEXT("Lose.wav"), nullptr, SND_SYNC);
		}

		glPopMatrix();
	}
	else {
		//Right Fence
		glPushMatrix();
		glColor3f(RedFence, GreenFence, BlueFence);
		drawFence(3, -0);
		glPopMatrix();

		//Left Fence
		glPushMatrix();
		glColor3f(RedFence, GreenFence, BlueFence);
		drawFence(-2, -0);
		glPopMatrix();

		//Front Fence
		glPushMatrix();
		glTranslated(2, 0.5, -12.5);
		drawFenceLeg(0.02, 0.5);
		glTranslated(-4.5, 0, 0);
		for (int i = 0; i < 10; i++) {
			glTranslated(0.5, 0, 0);
			drawFenceLeg(0.02, 0.5);
		}
		glPopMatrix();
		glPushMatrix();
		glTranslated(-2, 0.75, -12.5);
		glRotatef(90, 0, 1, 0);
		GLUquadric* quad = gluNewQuadric();
		gluCylinder(quad, 0.01, 0.01, 5, 20, 20);
		glTranslated(0, 0.25, 0);
		gluCylinder(quad, 0.01, 0.01, 5, 20, 20);
		glPopMatrix();

		//Ground
		glPushMatrix();
		drawGround(0.01);
		glPopMatrix();

		//Player
		glPushMatrix();
		glTranslated(PlayerX, 0.7, PlayerZ);
		glRotatef(PlayerAngle, 0, 1, 0);
		glTranslatef(-PlayerX, -0.7, -PlayerZ);
		drawPlayer(PlayerX, 0.7, PlayerZ);
		glPopMatrix();

		//Ferris Wheel
		glPushMatrix();
		glRotatef(90, 0, 1, 0);
		drawFerrisWheelWithStand(0.2, 0.1, 2, 0.2, 1.6, 0.2, 0.4, 3.0, 1.75, -1.8);
		glPopMatrix();

		//Carousel
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		drawCarouselCarnivalGame(1.5, 2.5, 1.0, 0.5, 8, 1.0, -8.0, -4.2);
		glPopMatrix();

		//Tree
		glPushMatrix();
		glRotatef(-90, 1, 0, 0);
		glTranslatef(-1, 1, 0.5);
		glRotatef(rotationAngle, 0, 0, 1);
		glTranslatef(1, -1, -0.5);
		drawTree(0.8, 0.04, 0.5, -1.0, 1.0, 0.5);
		glPopMatrix();

		//Boat Swing
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 0, 1);
		drawBoatSwing(0.3, 0.4, 0.5, 0.4, 0.1, 0.2, -3.0, -2.0, -1.2);
		glPopMatrix();

		//Jumping Castle
		glPushMatrix();
		glRotatef(-90, 1, 0, 0);
		glRotatef(-90, 0, 0, 1);
		glTranslatef(-1, 2, 0.7);
		glScalef(scaleCastle, scaleCastle, scaleCastle);
		glTranslatef(1, -2, -0.7);
		drawJumpingCastle(0.6, 0.8, 0.4, 0.1, 0.2, 0.05, -1.0, 2.0, 0.7);
		glPopMatrix();

		//Goal
		glPushMatrix();
		drawFinishLine(5.0, 0.05, 0.05, 0.5, 0.75, -11.0);
		glPopMatrix();

		//Candy Car
		glPushMatrix();
		glTranslatef(translatevalue, 0, 0);
		glRotatef(-90, 0, 1, 0);
		drawCandyCart(-5, 0.7, -2, 0.8, 0.4, 0.4);
		glPopMatrix();

		//Train
		glPushMatrix();
		glTranslatef(0, 0, translatevalueTrain);
		glRotatef(90, 0, 1, 0);
		if (!forwarCarousel) {
			glTranslatef(1, 0.75, 0.5);
			glRotatef(180, 0, 1, 0);
			glTranslatef(-1, -0.75, -0.5);
		}
		drawTrain(1, 0.75, 0.5, 1, 0.5, 0.3);
		glPopMatrix();


		//Timer
		glPushMatrix();
		int time = timer / 100;
		drawNumber3D(-7, 1, -12.5, 10, 10, time);
		glPopMatrix();

	}
	glFlush();
}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.5;

	switch (key) {
	//Camera Movement
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	//Camera Views
	case 'z':
		setDefaultView();
		break;
	case 't':
		setTopView();
		break;
	case 'f':
		setFrontView();
		break;
	case 'c':
		setSideView();
		break;
	//Player Movements
	case 'i':
		if (PlayerZ - 0.1 - 0.15 > -12.5) {
			if (PlayerZ - 0.1 - 0.2 <= -11 && timer > 0 && !goal) {
				bool played = PlaySound(TEXT("Applause.wav"), nullptr, SND_SYNC);
				goal = true;
			}
			PlayerZ -= 0.1;
			PlayerAngle = 180;
		}
		else {
			bool played = PlaySound(TEXT("Thump.wav"), nullptr, SND_SYNC);
		}
		break;
	case 'k':
		if (PlayerZ + 0.1 + 0.15 < 0) {
			PlayerZ += 0.1;
			PlayerAngle = 0;
		}
		else {
			bool played = PlaySound(TEXT("Thump.wav"), nullptr, SND_SYNC);
		}
		break;
	case 'j':
		if (PlayerX - 0.1 - 0.15 > -2) {
			PlayerX -= 0.1;
			PlayerAngle = -90;
		}
		else {
			bool played = PlaySound(TEXT("Thump.wav"), nullptr, SND_SYNC);
		}
		break;
	case 'l':
		if (PlayerX + 0.1 + 0.15 < 3) {
			PlayerX += 0.1;
			PlayerAngle = 90;
		}
		else {
			bool played = PlaySound(TEXT("Thump.wav"), nullptr, SND_SYNC);
		}
		break;
	//Animations Control
	case 'r':
		if (AnimKey) {
			AnimKey = false;
		}
		else {
			AnimKey = true;
		}
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}

void Special(int key, int x, int y) {
	float a = 2.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

void time(int val)//timer animation function, allows the user to pass an integer valu to the timer function.
{
	if (timer > 0) {
		timer--;
	}
	
	if (AnimKey) {
		//Ferris
		rotationAngle += 0.5;
		if (rotationAngle >= 360.0f) {
			rotationAngle -= 360.0f;
		}
		//swing
		if (!Swingleft) {
			rotationAngleSwing += 0.5;
			if (rotationAngleSwing >= 45.0f) {
				rotationAngleSwing -= 0.5f;
				Swingleft = true;
			}
		}
		else {
			rotationAngleSwing -= 0.5;
			if (rotationAngleSwing <= -45.0f) {
				rotationAngleSwing += 0.5f;
				Swingleft = false;
			}
		}
		//Carousel
		if (up) {
			translatevalue -= 0.002;
			if (translatevalue <= -0.2) {
				up = false;
				translatevalue += 0.002;
			}
		}
		else {
			translatevalue += 0.002;
			if (translatevalue >= 0.2) {
				up = true;
				translatevalue -= 0.002;
			}
		}
		//Train
		if (forwarCarousel) {
			translatevalueTrain -= 0.01;
			if (translatevalueTrain <= -4) {
				forwarCarousel = false;
				translatevalueTrain += 0.01;
			}
		}
		else {
			translatevalueTrain += 0.01;
			if (translatevalueTrain >= 0) {
				forwarCarousel = true;
				translatevalueTrain -= 0.01;
			}
		}
		//Castle
		if (!largen) {
			scaleCastle -= 0.01;
			if (scaleCastle <= 0) {
				largen = true;
				scaleCastle += 0.01;
			}
		}
		else {
			scaleCastle += 0.01;
			if (scaleCastle >= 1) {
				largen = false;
				scaleCastle -= 0.01;
			}
		}
	}
	//Goal
	if (upGoal && !goal) {
		translatevalueGoal -= 0.002;
		if (translatevalueGoal <= -0.2) {
			upGoal = false;
			translatevalueGoal += 0.002;
		}
	}
	else {
		translatevalueGoal += 0.002;
		if (translatevalueGoal >= 0.2) {
			upGoal = true;
			translatevalueGoal -= 0.002;
		}
	}
	//Fence Color
	RedFence += 0.1 * redD;							//--
	if (RedFence < 0 || RedFence>1)					//	|
		redD *= -1;								//	|
	//	|
	GreenFence += 0.1 * greenD;						//	|
	if (GreenFence < 0 || GreenFence>0.5)				//	|		
		greenD *= -1;								//	|this code is called every 1000 ms "ever 1 sec"
	//	|		
	BlueFence += 0.1 * blueD;						//	|
	if (BlueFence < 0.5 || BlueFence>0.75)				//	|
		blueD *= -1;

	glutPostRedisplay();						// redraw 		
	glutTimerFunc(1, time, 0);					//recall the time function after 1000 ms and pass a zero value as an input to the time func.
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Lab 5");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(0, time, 0);
	glutSpecialFunc(Special);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}
