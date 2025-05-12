#include <iostream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <cmath>
#include <list>

using namespace std;

// Data Structures for Game
struct Vector3 {
	double x, y, z;
	Vector3() { x = y = z = 0.0; }
	Vector3(double x, double y, double z) : x(x), y(y), z(z) { }
	Vector3(Vector3 const& rhs) { *this = rhs; }
	Vector3& operator= (Vector3 const& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}
};

struct Disc {
	Disc() { normal = Vector3(0.0, 0.0, 1.0); }

	Vector3 position; //location
	Vector3 normal;   //orientation
};

struct ActiveDisc {    //Active Disc to be moved [later in motion]
	int disc_index;
	Vector3 start_pos, dest_pos;
	double u;		    // u E [0, 1]
	double step_u;
	bool is_in_motion;
	int direction;     // +1 for Left to Right & -1 for Right to left, 0 = stationary
};


// Rods and Discs Globals - Can be changed for different levels
const size_t NUM_DISCS = 8;
int num_discs = 8;
const double ROD_HEIGHT = 3.0;

struct Rod {
	Vector3 positions[NUM_DISCS];
	int occupancy_val[NUM_DISCS];
};

struct GameBoard {
	double x_min, y_min, x_max, y_max; //Base in XY-Plane
	double rod_base_rad;               //Rod's base radius
	Rod rods[3];
};

struct solution_pair {
	size_t f, t;         //f = from, t = to
};

//Game Globals
Disc discs[NUM_DISCS];
GameBoard t_board;
ActiveDisc active_disc;
list<solution_pair> sol;
bool to_solve = false;

//Globals for window, time, FPS, moves
const size_t FPS = 150;
size_t moves = 0;
size_t prev_time = 0;

const double PI = 3.141592653589793;

// mouse trigger
int lastMousePosX = 0, lastMousePosY = 0; // Previous mouse positions
bool leftButtonPressed = false;
bool leftRightButtonPressed = false;
GLfloat zoomFactor = 1.0f; // Zoom level

// Camera spherical coordinates
GLfloat radius = 5.0f; // Distance from the origin
GLfloat theta = PI / 4.0f;; // Angle from the Z-axis (in radians)
GLfloat phi = PI / 4.0f;; // Angle from the X-axis in the XY-plane (in radians)

// Initializes the game board, rods, discs, and active disc.
void initialize_game() {
	// 1. Sets up the game board dimensions and rod positions.
	// 2. Initializes disc positions on the first rod.
	// 3. Prepares the active disc state for animation.

	//1) Initializing GameBoard
	t_board.rod_base_rad = 1.0;
	t_board.x_min = 0.0;
	t_board.x_max = 6.6 * t_board.rod_base_rad;
	t_board.y_min = 0.0;
	t_board.y_max = 2.2 * t_board.rod_base_rad;

	double x_center = (t_board.x_max - t_board.x_min) / 2.0;
	double y_center = (t_board.y_max - t_board.y_min) / 2.0;

	double dx = (t_board.x_max - t_board.x_min) / 3.0; //Since 3 rods
	double r = t_board.rod_base_rad;

	//Initializing Rods Occupancy value		
	for (size_t i = 0; i < 3; i++) {
		for (size_t h = 0; h < num_discs; h++) {
			if (i == 0) {
				t_board.rods[i].occupancy_val[h] = num_discs - 1 - h;
			}
			else t_board.rods[i].occupancy_val[h] = -1;
		}
	}


	//Initializing Rod positions
	for (size_t i = 0; i < 3; i++) {
		for (size_t h = 0; h < num_discs; h++) {
			double x = x_center + ((int)i - 1) * dx;
			double y = y_center;
			double z = (h + 1) * 0.3;
			Vector3& pos_to_set = t_board.rods[i].positions[h];
			pos_to_set.x = x;
			pos_to_set.y = y;
			pos_to_set.z = z;
		}
	}


	//2) Initializing Discs
	for (size_t i = 0; i < num_discs; i++) {
		discs[i].position = t_board.rods[0].positions[num_discs - i - 1];
		//Normals are initialized whie creating a Disc object - ie in constructor of Disc
		discs[i].normal = Vector3(0.0, 0.0, 1.0);
	}


	//3) Initializing Active Disc
	active_disc.disc_index = -1;
	active_disc.is_in_motion = false;
	active_disc.step_u = 0.02;         //Verify
	active_disc.u = 0.0;
	active_disc.direction = 0;
}

void initialize() {
	// 1. Sets the background color and enables smooth shading.
	// 2. Configures lighting and depth tests.
	// 3. Calls `initialize_game()` to set up the game state.

	glClearColor(0.0, 0.0, 0.0, 0.0); //Setting the clear color
	glShadeModel(GL_SMOOTH);		  //SMOOTH Shading

	glEnable(GL_DEPTH_TEST);		  //Enabling Depth Test

	//Setting Light0 parameters
	GLfloat light0_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // A positional light
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

	glEnable(GL_LIGHTING);			  //Enabling Lighting
	glEnable(GL_LIGHT0);		      //Enabling Light0	


	//Globals initializations
	prev_time = glutGet(GLUT_ELAPSED_TIME);

	//Initializing Game State
	initialize_game();
}

// Updates the camera position and view angle based on game board size.
void updateCamera() {
	// Calculate center of the game board
	double x_center = (t_board.x_max + t_board.x_min) / 2.0;
	double y_center = (t_board.y_max + t_board.y_min) / 2.0;
	double z_center = ROD_HEIGHT / 2.0;

	// Adjust the camera radius to fit the entire Tower of Hanoi
	radius = max(t_board.x_max - t_board.x_min, ROD_HEIGHT * 2.0);

	// Set phi to 180 degrees for a side view
	phi = 3 * PI / 2.0;


	// Convert spherical coordinates to Cartesian for camera position
	GLfloat eyeX = radius * sin(theta) * cos(phi) + x_center;
	GLfloat eyeY = radius * sin(theta) * sin(phi) + y_center;
	GLfloat eyeZ = radius * cos(theta) + z_center;

	// Set the camera view
	gluLookAt(
		eyeX, eyeY, eyeZ,      // Camera position
		x_center, y_center, 0.0, // Look-at point (center of the board)
		0.0, 0.0, 1.0          // Up vector
	);
}

void mouseWheel(int wheel, int direction, int x, int y) {
	// Adjust zoom factor based on wheel scroll direction
	if (direction > 0) {
		zoomFactor += 0.05f; // Scroll up: Zoom in
	}
	else if (direction < 0) {
		zoomFactor -= 0.05f; // Scroll down: Zoom out
	}

	// Clamp zoom factor to avoid extreme zooming
	if (zoomFactor < 0.1f) zoomFactor = 0.1f; // Minimum zoom
	if (zoomFactor > 10.0f) zoomFactor = 10.0f; // Maximum zoom

	glutPostRedisplay(); // Redraw the scene
}

//Draw function for drawing a cylinder given position and radius and height
void draw_solid_cylinder(double x, double y, double r, double h) {
	GLUquadric* q = gluNewQuadric();
	GLint slices = 50;
	GLint stacks = 10;

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	gluCylinder(q, r, r, h, slices, stacks);
	glTranslatef(0, 0, h);
	gluDisk(q, 0, r, slices, stacks);
	glPopMatrix();
	gluDeleteQuadric(q);
}

//Draw function for drawing rods on a given game board 
void draw_board_and_rods(GameBoard const& board) {
	//Materials, 
	GLfloat mat_gray[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };

	glPushMatrix();
	//Drawing the Base Rectangle [where the rods are placed]
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_gray);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex2f(board.x_min, board.y_min);
	glVertex2f(board.x_min, board.y_max);
	glVertex2f(board.x_max, board.y_max);
	glVertex2f(board.x_max, board.y_min);
	glEnd();

	//Drawing Rods and Pedestals
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_yellow);

	double r = board.rod_base_rad;
	for (size_t i = 0; i < 3; i++) {
		Vector3 const& p = board.rods[i].positions[0];
		draw_solid_cylinder(p.x, p.y, r * 0.1, ROD_HEIGHT - 0.1);
		draw_solid_cylinder(p.x, p.y, r, 0.1);
	}

	glPopMatrix();
}


// Draw function for drawing discs
void draw_discs() {
	int slices = 100;
	int stacks = 10;

	double rad;

	GLfloat r, g, b;
	GLfloat emission[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat material[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	for (size_t i = 0; i < num_discs; i++) {
		if (i >= num_discs) break;

		switch (i) {
		case 0: r = 1.0f; g = 0.0f; b = 0.0f; //red
			break;
		case 1: r = 0.75f; g = 0.5f; b = 0.0f; //orange
			break;
		case 2: r = 0.0f; g = 1.0f; b = 0.0f; //green
			break;
		case 3: r = 0.0f; g = 1.0f; b = 1.0f; // cyan
			break;
		case 4: r = 0.0f; g = 0.0f; b = 1.0f; //blue
			break;
		case 5: r = 0.5f; g = 0.0f; b = 1.0f; //purple
			break;
		case 6: r = 1.0f; g = 0.0f; b = 1.0f; //pink
			break;
		default: r = g = b = 1.0f; //white
			break;
		};

		material[0] = r;
		material[1] = g;
		material[2] = b;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, material);

		GLfloat u = 0.0f;

		if (i == active_disc.disc_index) {
			glMaterialfv(GL_FRONT, GL_EMISSION, emission);
			u = active_disc.u;
		}

		// size of each disk
		GLfloat factor = 0.8f;
		switch (i) {
		case 0: factor = 0.1f;
			break;
		case 1: factor = 0.2f;
			break;
		case 2: factor = 0.3f;
			break;
		case 3: factor = 0.4f;
			break;
		case 4: factor = 0.5f;
			break;
		case 5: factor = 0.6f;
			break;
		case 6: factor = 0.7f;
			break;
		case 7: factor = 0.8f;
			break;
		default: factor = 0.8f;
			break;
		};
		rad = factor * t_board.rod_base_rad;
		int d = active_disc.direction;


		glPushMatrix();
		glTranslatef(discs[i].position.x, discs[i].position.y, discs[i].position.z);
		double theta = acos(discs[i].normal.z);
		theta *= 180.0f / PI;
		glRotatef(d * theta, 0.0f, 1.0f, 0.0f);
		glutSolidTorus(0.2 * t_board.rod_base_rad, rad, stacks, slices);
		glPopMatrix();

		glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
	}
}

void display_handler() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	double x_center = (t_board.x_max - t_board.x_min) / 2.0;
	double y_center = (t_board.y_max - t_board.y_min) / 2.0;
	double r = t_board.rod_base_rad;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	updateCamera();

	glScalef(zoomFactor, zoomFactor, zoomFactor);

	glPushMatrix();
	draw_board_and_rods(t_board);
	draw_discs();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		leftButtonPressed = true;
	}

	lastMousePosX = x;
	lastMousePosY = y;
}

// Function to handle mouse movement while dragging
void motion(int x, int y) {
	int deltaX = x - lastMousePosX;
	int deltaY = y - lastMousePosY;

	if (leftButtonPressed) {
		// Adjust theta and phi based on mouse movement
		theta -= deltaY * 0.01f; // Vertical movement adjusts theta
		phi -= deltaX * 0.01f;   // Horizontal movement adjusts phi

		// Clamp theta to avoid flipping
		if (theta < 0.1f) theta = 0.1f;
		if (theta > PI - 0.1f) theta = PI - 0.1f;

	}

	lastMousePosX = x;
	lastMousePosY = y;

	glutPostRedisplay(); // Redraw the scene
}

void reshape_handler(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h) {
		glOrtho(-2, 2, -2 * (GLfloat)h / (GLfloat)w, 2 * (GLfloat)h / (GLfloat)w, -10, 10);
	}
	else {
		glOrtho(-2 * (GLfloat)w / (GLfloat)h, 2 * (GLfloat)w / (GLfloat)h, -2, 2, -10, 10);
	}

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void move_stack(int n, int f, int t) {
	if (n == 1) {
		solution_pair s;
		s.f = f;
		s.t = t;
		sol.push_back(s);         //pushing the (from, to) pair of solution to a list [so that it can be animated later]
		moves++;
		cout << "From rod " << f << " to Rod " << t << endl;
		return;
	}
	move_stack(n - 1, f, 3 - t - f);
	move_stack(1, f, t);
	move_stack(n - 1, 3 - t - f, t);
}

//Solve from 1st rod to 2nd
void solve() {
	move_stack(num_discs, 0, 2);
}

void keyboard_handler(unsigned char key, int x, int y) {
	//Console Outputs
	switch (key) {
	case 'Q':
	case 'q':
		exit(0);
		break;
	default:
		break;
	};
}


void move_disc(int from_rod, int to_rod) {

	int d = to_rod - from_rod;
	if (d > 0) active_disc.direction = 1;
	else if (d < 0) active_disc.direction = -1;

	if ((from_rod == to_rod) || (from_rod < 0) || (to_rod < 0) || (from_rod > 2) || (to_rod > 2))
		return;

	size_t i;
	for (i = num_discs - 1; i >= 0 && t_board.rods[from_rod].occupancy_val[i] < 0; i--);
	if ((i < 0) || (i == 0 && t_board.rods[from_rod].occupancy_val[i] < 0))
		return; //Either the index < 0 or index at 0 and occupancy < 0 => it's an empty rod

	active_disc.start_pos = t_board.rods[from_rod].positions[i];

	active_disc.disc_index = t_board.rods[from_rod].occupancy_val[i];
	active_disc.is_in_motion = true;
	active_disc.u = 0.0;


	size_t j;
	for (j = 0; j < num_discs - 1 && t_board.rods[to_rod].occupancy_val[j] >= 0; j++);
	active_disc.dest_pos = t_board.rods[to_rod].positions[j];

	t_board.rods[from_rod].occupancy_val[i] = -1;
	t_board.rods[to_rod].occupancy_val[j] = active_disc.disc_index;
}


Vector3 get_inerpolated_coordinate(Vector3 sp, Vector3 tp, double u) {
	//4 Control points
	Vector3 p;
	double x_center = (t_board.x_max - t_board.x_min) / 2.0;
	double y_center = (t_board.y_max - t_board.y_min) / 2.0;

	double u3 = u * u * u;
	double u2 = u * u;

	Vector3 cps[4]; //P1, P2, dP1, dP2


	//Hermite Interpolation [Check Reference for equation of spline]
	{
		//P1
		cps[0].x = sp.x;
		cps[0].y = y_center;
		cps[0].z = ROD_HEIGHT + 0.2 * (t_board.rod_base_rad);

		//P2
		cps[1].x = tp.x;
		cps[1].y = y_center;
		cps[1].z = ROD_HEIGHT + 0.2 * (t_board.rod_base_rad);

		//dP1
		cps[2].x = (sp.x + tp.x) / 2.0 - sp.x;
		cps[2].y = y_center;
		cps[2].z = 2 * cps[1].z; //change 2 * ..

		//dP2
		cps[3].x = tp.x - (tp.x + sp.x) / 2.0;
		cps[3].y = y_center;
		cps[3].z = -cps[2].z; //- cps[2].z;


		double h0 = 2 * u3 - 3 * u2 + 1;
		double h1 = -2 * u3 + 3 * u2;
		double h2 = u3 - 2 * u2 + u;
		double h3 = u3 - u2;

		p.x = h0 * cps[0].x + h1 * cps[1].x + h2 * cps[2].x + h3 * cps[3].x;
		p.y = h0 * cps[0].y + h1 * cps[1].y + h2 * cps[2].y + h3 * cps[3].y;
		p.z = h0 * cps[0].z + h1 * cps[1].z + h2 * cps[2].z + h3 * cps[3].z;

	}

	return p;
}

//Normalize function for a vector
void normalize(Vector3& v) {
	double length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length == 0.0) return;
	v.x /= length;
	v.y /= length;
	v.z /= length;
}


Vector3 operator-(Vector3 const& v1, Vector3 const& v2) {
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

void anim_handler() {
	int curr_time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = curr_time - prev_time; // in ms
	if (elapsed < 1000 / FPS) return;

	prev_time = curr_time;

	if (to_solve && active_disc.is_in_motion == false) {
		solution_pair s = sol.front();

		cout << s.f << ", " << s.t << endl;

		sol.pop_front();
		int i;
		for (i = num_discs; i >= 0 && t_board.rods[s.f].occupancy_val[i] < 0; i--);
		int ind = t_board.rods[s.f].occupancy_val[i];

		if (ind >= 0) {
			active_disc.disc_index = ind;
		}

		move_disc(s.f, s.t);
		if (sol.size() == 0) {
			to_solve = false;
		}
	}

	if (active_disc.is_in_motion) {
		int ind = active_disc.disc_index;
		ActiveDisc& ad = active_disc;

		if (ad.u == 0.0 && (discs[ind].position.z < ROD_HEIGHT + 0.2 * (t_board.rod_base_rad))) {
			discs[ind].position.z += 0.05;
			glutPostRedisplay();
			return;
		}

		static bool done = false;
		if (ad.u == 1.0 && discs[ind].position.z > ad.dest_pos.z) {
			done = true;
			discs[ind].normal = Vector3(0, 0, 1);
			discs[ind].position.z -= 0.05;
			glutPostRedisplay();
			return;
		}

		ad.u += ad.step_u;
		if (ad.u > 1.0) {
			ad.u = 1.0;
		}

		if (!done) {
			Vector3 prev_p = discs[ind].position;
			Vector3 p = get_inerpolated_coordinate(ad.start_pos, ad.dest_pos, ad.u);
			discs[ind].position = p;
			discs[ind].normal.x = (p - prev_p).x;
			discs[ind].normal.y = (p - prev_p).y;
			discs[ind].normal.z = (p - prev_p).z;
			normalize(discs[ind].normal);
		}

		if (ad.u >= 1.0 && discs[ind].position.z <= ad.dest_pos.z) {
			discs[ind].position.z = ad.dest_pos.z;
			ad.is_in_motion = false;
			done = false;
			ad.u = 0.0;
			discs[ad.disc_index].normal = Vector3(0, 0, 1);
			ad.disc_index = -1;

		}
		glutPostRedisplay();
	}
}

void main_menu(int id) {
	if (id == 1) {
		exit(0);
	}
}

void lay(int id) {
	switch (id) {
	case 1: num_discs = 3; break;
	case 2: num_discs = 4; break;
	case 3: num_discs = 5; break;
	case 4: num_discs = 6; break;
	case 5: num_discs = 7; break;
	case 6: num_discs = 8; break;
	default: num_discs = 3; break;
	}

	// Reinitialize the game with the new number of discs
	initialize_game();

	// Generate a new solution for the updated number of discs
	sol.clear();
	moves = 0;
	solve();
	to_solve = true;

	// Redraw the scene
	glutPostRedisplay();
}

void createMenu() {

	int layer = glutCreateMenu(lay);
	glutAddMenuEntry("3", 1);
	glutAddMenuEntry("4", 2);
	glutAddMenuEntry("5", 3);
	glutAddMenuEntry("6", 4);
	glutAddMenuEntry("7", 5);
	glutAddMenuEntry("8", 6);


	int menu = glutCreateMenu(main_menu);
	glutAddSubMenu("Disk Layer", layer);
	glutAddMenuEntry("Exit", 1);

	glutDetachMenu(GLUT_LEFT_BUTTON);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {

	// Initialize GLUT Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1200, 1200);
	glutInitWindowPosition(75, 60);
	glutCreateWindow("Towers of Hanoi");

	createMenu();
	initialize();

	// Callbacks
	glutDisplayFunc(display_handler);
	glutReshapeFunc(reshape_handler);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMouseWheelFunc(mouseWheel);
	glutIdleFunc(anim_handler);

	// Automatically solve the puzzle
	solve(); // Generate the solution sequence
	to_solve = true;


	// glutMouseFunc(mouse_handler);

	glutMainLoop();
	return 0;
}