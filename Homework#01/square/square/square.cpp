#include <GL/freeglut.h>  /*�ھکҨϥΪ� window widget�i��ק�*/

void mydisplay(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POLYGON);
	glVertex2f(-0.5, -0.5);
	glVertex2f(-0.5, 0.5);
	glVertex2f(0.5, 0.5);
	glVertex2f(0.0, 0.0);
	glVertex2f(0.5, -0.5);
	glEnd();
	glFlush();
}

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutCreateWindow("S11159020 �����x"); 
	glutDisplayFunc(mydisplay);
	glutMainLoop();
}