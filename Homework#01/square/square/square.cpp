#include <GL/freeglut.h>  /*根據所使用的 window widget進行修改*/

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
	glutCreateWindow("S11159020 卓楚庭"); 
	glutDisplayFunc(mydisplay);
	glutMainLoop();
}