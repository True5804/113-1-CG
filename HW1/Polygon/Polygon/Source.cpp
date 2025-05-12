#include <Gl/freeglut.h>

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POLYGON);
	// 長度單位為1
	// important for order of points
	glVertex2f(-0.5, -0.5);
	glVertex2f(-0.5, 0.5);
	glVertex2f(0.5, 0.5);
	glVertex2f(0, 0);
	glVertex2f(0.5, -0.5);
	glEnd();
	glFlush();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutCreateWindow("simple");  // set the name of the display window
	glutDisplayFunc(display);
	glutMainLoop();
}