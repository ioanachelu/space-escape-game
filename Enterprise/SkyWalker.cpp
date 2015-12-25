#include <stdlib.h>
#include <glut.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <cstdlib> 
#include <ctime> 
#include <iostream>
#include <vector>
//#include "shared/gltools.h"	// OpenGL toolkit
//#include "shared/MathsUtils.h" // Mathematical toolkit
#include <string.h>
#include <math.h>
//ground
#include "ground.h"

//camera
#include "camera.h"

using namespace std;
typedef struct{
	int x, y, z;
	GLfloat radius;
}Point; // point = stea

Point * createRandomStarArray(int, float, float, float, float, float, float); 
typedef struct {
   GLdouble    x;
   GLdouble    z;
   GLdouble y;
   GLdouble trx , trz;
   GLdouble raza; // folosita la sateliti pentru a vedea amploarea exploziei
   GLdouble culoare_x;
   GLdouble culoare_y;
   GLdouble culoare_z;
   GLdouble scale;
} navuta; 

//cam
Camera camera;
int care_camera;
double alfa = 0.5;


GLdouble x_sel, y_sel, z_sel;
GLfloat spin=0.0;
GLfloat aspect = 1;
GLfloat tr=0,step=0.01,sc=0.4,st=1.1, z_pr, x_pr; // z_pr si x_pr sunt directiile proiectilelor N , S, E, V
int x=0,y=0;
bool t=true;
int win_w = 1, win_h = 1;
int satelit_selectat = -1;


int screen_size = 10; // jumatate din dimensiunea galaxiei; in mod normal infinit da dehh!
int nr_inamici = 20; // numarul initial de inamici din scena
int min_space = -7, max_space = 7; // maximele spatiului de joc intre care se genereaza nr random pt pozitiile navelor inamice
GLdouble x_hero, z_hero, y_hero; // pozitia actuala a navei
int nr_sateliti = 5;  // nr de sateliti
//acesti vectori sunt folositi si pentru ca pozitiile random pe care se genereaza inamicii si satelitii sa nu se suprapuna
vector<navuta> proiectile; // vector de proiectile, in varianta finala unul singur
//vector<navuta> navute; // vector ce contine inamicii, inamicii sunt representati de x si y, coordonatele pozitiei lor in spatiu
vector<navuta> sateliti; // vector ce contine satelitii
int nr_of_stars = 500; // nr de stele de pe ecran

int vieti = 5; //nr de vieti ale navei					
int proiectil_pe_ecran = 0; // proiectil_pe_ecran = 1 daca s-a dat deja drumul la un proiectil si acesta nu a diparut inca de pe ecran
Point * stars = new Point[nr_of_stars]; // vector de stele

//lumina directionala generala porneste de la -infinit pe axa oy
GLdouble xx1 = 16, xx2 = -10, yy1 = -26, yy2 = -30, zz1 = 9, zz2 = 30;
GLfloat light1_position [] = {xx1, yy1, zz1, 1};
GLfloat light1_ambient [] = { 0, 0, 0.5, 1};
GLfloat light1_diffuse [] = {0.2, 0.2, 0.2, 1};
GLfloat light1_specular [] = {0.2, 0.2, 0.2, 1};


GLfloat light2_position [] = {xx2, yy2, zz2, 1};
GLfloat light2_ambient [] = { 0.05, 0, 0.015, 1};
GLfloat light2_diffuse [] = {0.2, 0.2, 0.2, 1};
GLfloat light2_specular [] = {0.2, 0.2, 0.2, 1};
//materiale nave si sateliti
GLfloat hero_ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat hero_diffuse[] = {0.6, 0.6, 0.6, 1.0};
GLfloat hero_specular[] = {0.30, 0.3, 0.3, 0.3};
GLfloat hero_shininess = 1;

//explodeaza = 1 daca a explodat un satelit, indice_satelit ne spune care din cele doua
//implodeaza  = 1 inseamna ca a explodat si incepe sa se micsoreze raza pana cand este egala cu 0 si acesta dispare
int explodeaza = 0, indice_satelit = 0, implodeaza = 0;


void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	const GLfloat globalAmbientColor4f [] = {0.2, 0.2, 0.2, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientColor4f);

	glShadeModel(GL_SMOOTH);

	x_hero = 0; // nava eroului fara nume se afla in centru initial
	z_hero = 0;
	y_hero = 0;
	
	for(int i = 0 ; i < nr_sateliti ; i ++)
	{
		int new_x = min_space + (rand() % (int)(max_space - min_space + 1));
		int new_z = min_space + (rand() % (int)(max_space - min_space + 1));
		int new_y = 0 + (rand() % 6);
		

		for(int j = 0 ; j < sateliti.size(); j ++)
		{
			if( new_x == sateliti[j].x && new_z == sateliti[j].z && new_y == sateliti[j].y )
			{
				new_x = min_space + (rand() % (int)(max_space - min_space + 1));
				new_z = min_space + (rand() % (int)(max_space - min_space + 1));
				int new_y = 1 + (rand() % (int) (4 - 1 + 1));
			}
		}
		navuta satelit; // creez o noua intrare in vectorul de navute cu o navuta reprezentata de un satelit
		satelit.x = new_x;
		satelit.z = new_z;
		satelit.y = new_y;
		satelit.trz = -0.01;
		satelit.scale = 0.5;//(2 + (rand() % (8 - 2 + 1))) / 10 ;
		satelit.culoare_x = (2 + (rand() % 9)) / 10 ;
		satelit.culoare_y = (2 + (rand() % 9)) / 10 ;
		satelit.culoare_z = (2 + (rand() % 9)) / 10 ;
		sateliti.push_back(satelit);
	}
	stars = createRandomStarArray(nr_of_stars,-10,10,-10,10,10,10); // creez stelele pe pozitii random si le stochez in stars
}


void reshape(int w, int h)
{
	glViewport(0,0, (GLsizei) w, (GLsizei) h);

	aspect = (GLfloat) w / (GLfloat) h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, aspect, 0.1f, 60.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	win_w = w;
	win_h = h;
}
// Aceasta functie e folosita pentru a afisa un text pe ecran cu un anumit font la anumite coordonate
void printw (float x, float y, float z, char* format, ...)
{
    va_list args;   //  Variable argument list
    int len;        // String length
    int i;          //  Iterator
    char * text;    // Text
	GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;
    //  Initialize a variable argument list
    va_start(args, format);

    //  Return the number of characters in the string referenced the list of arguments.
    // _vscprintf doesn't count terminating '\0' (that's why +1)
    len = _vscprintf(format, args) + 1;

    //  Allocate memory for a string of the specified size
    text = (char *) malloc(len * sizeof(char));

    //  Write formatted output using a pointer to the list of arguments
    vsprintf_s(text, len, format, args);

    //  End using variable argument list
    va_end(args);

    //  Specify the raster position for pixel operations.
    glRasterPos3f (x, y, z);

    //  Draw the characters one by one
    for (i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(font_style, text[i]);

    //  Free the allocated memory for the string
    free(text);
}

void display(void)
{
	
	//Render objects
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.render();

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
	
    glLightfv(GL_LIGHT0, GL_POSITION, light1_position);	

	glLightfv(GL_LIGHT0, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light1_specular);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT4, GL_POSITION, light2_position);	
	glLightfv(GL_LIGHT4, GL_AMBIENT, light2_ambient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT4, GL_SPECULAR, light2_specular);
	glEnable(GL_LIGHT4);

	glEnable(GL_COLOR_MATERIAL);

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, hero_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, hero_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hero_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, hero_shininess);

	
	//Suprafata planului de joc (spatiul)
	glPushMatrix();
	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glVertex3f(-screen_size, 0, -screen_size);
    glNormal3f(0,1,0);
    glVertex3f(screen_size, 0, -screen_size);
    glNormal3f(0,1,0);
    glVertex3f(screen_size, 0, screen_size);
    glNormal3f(0,1,0);
    glVertex3f(-screen_size, 0, screen_size);
    glEnd();
	glPopMatrix();


	glPushMatrix();
	glColor3f(	0, 1, 0);
	int zz = 9;
	glBegin(GL_LINES);
    glVertex3f(-10, 0, zz);
	glVertex3f(-10, 0, -zz);
	glVertex3f(-10, 0, -zz);
	glVertex3f(10, 0, -zz);
	glVertex3f(10, 0, -zz);
	glVertex3f(10, 0, zz);
	glVertex3f(10, 0, zz);
	glVertex3f(-10, 0, zz);

	glVertex3f(-10, 5, zz);
	glVertex3f(-10, 5, -zz);
	glVertex3f(-10, 5, -zz);
	glVertex3f(10, 5, -zz);
	glVertex3f(10, 5, -zz);
	glVertex3f(10, 5, zz);
	glVertex3f(10, 5, zz);
	glVertex3f(-10, 5, zz);

	glVertex3f(-10, 0, zz);
	glVertex3f(-10, 5, zz);
	glVertex3f(-10, 0, -zz);
	glVertex3f(-10, 5, -zz);
	glVertex3f(10, 0, -zz);
	glVertex3f(10, 5, -zz);
	glVertex3f(10, 0, zz);
	glVertex3f(10, 5, zz);
	glEnd();
	glPopMatrix();
	// end of spatiu
	
	//desenare stele
	for(int i = 0 ; i < nr_of_stars; i++)
	{
		glPushMatrix();
		glPointSize(stars[i].radius);
		glEnable(GL_POINT_SMOOTH);
		glBegin(GL_POINTS);

		glColor3ub(200, 200, 200); //white colour for the stars

		glVertex3f(stars[i].x, stars[i].y, stars[i].z); 
		glEnd();

		glPopMatrix();
	}

	
	glPushMatrix();
	glColor3f(0, 0, 0.6);
	glTranslatef(xx1, yy1, zz1);
	glutSolidSphere(3, 50, 50);
	glPopMatrix();

		
	glPushMatrix();
	glColor3f(1, 0, 0.4);
	glTranslatef(xx2, yy2, zz2);
	glutSolidSphere(2, 50, 50);
	glPopMatrix();
	//start of desenare sateliti
	for(int i = 0 ; i < sateliti.size(); i ++)
	{
		if(satelit_selectat == i)
		{

			glPushMatrix();
			glColor3f(0.2,1,0);
			glBegin(GL_LINES);
			glVertex3f(x_hero,y_hero,z_hero);
			glVertex3f(sateliti[i].x - 0.5, sateliti[i].y - 0.5 , sateliti[i].z - 0.5);
				
			glEnd();
			glPopMatrix();
		
			glPushMatrix();
			glColor3f(1,0,0);
			sateliti[i].z += sateliti[i].trz;
			glTranslatef(sateliti[i].x, 0, sateliti[i].z);
			glScalef(sateliti[i].scale, sateliti[i].scale, sateliti[i].scale);
			glutSolidDodecahedron();
			glPopMatrix();
			


		}
		else
		{
			glPushMatrix();
			glColor3f(sateliti[i].culoare_x, sateliti[i].culoare_y, sateliti[i].culoare_z);
			sateliti[i].z += sateliti[i].trz;
			glTranslatef(sateliti[i].x, 0, sateliti[i].z);
			glScalef(sateliti[i].scale, sateliti[i].scale, sateliti[i].scale);
			glutSolidDodecahedron();
			glPopMatrix();
		}
	}

	for(int i = 0 ; i < sateliti.size() ; i ++)
	{
		if(x_hero >= sateliti[i].x - 1 && x_hero <= sateliti[i].x + 1 && z_hero >= sateliti[i].z - 1 && z_hero <= sateliti[i].z + 1 && y_hero >= sateliti[i].y - 1 && y_hero <= sateliti[i].y + 1)
		{
			
			vieti --;
			alfa -= 0.1;
			x_hero = 0; z_hero = 0; y_hero = 2;
			if (vieti == 0)
			{
				glColor3f(1, 1, 1);
				printw(3, 3, 0, "GAME OVER");
				exit(0);
			}
			else break;
		}
		printf("%f %f %f \n", x_hero, y_hero, z_hero);
			printf("%f %f %f \n", sateliti[i].x, sateliti[i].y, sateliti[i].z);
		
	}
	
		//desen nava Praslea cel Voinic
	glPushMatrix();
	glTranslatef(x_hero, y_hero, z_hero);
	glRotatef(90, 1.0, 0.0, 0.0);
	glColor4f(0.5, 0.4, 0, alfa);
	glutSolidSphere(1, 30, 30);
	glColor3f(0.0, 0.35, 0.4);
	glutSolidTorus(0.1, 0.5, 30, 30);
	glColor3f(0.0, 0.0, 0.55);
	glutSolidSphere(0.2, 30, 30);
	glPopMatrix();

	//lumina spot a navei eroului
	GLfloat spot_direction [] = {0, 0, 1};
	GLfloat spot_position [] = {x_hero, y_hero, z_hero, 1.0};
	GLfloat spot_angle = 15.0;
	GLfloat spot_ambient [] = { 0.5, 0.5, 0.5, 1.0};
	GLfloat spot_diffuse [] = {1.0, 1.0, 1.0, 1.0};
	GLfloat spot_specular [] = {1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
    glLightfv(GL_LIGHT1, GL_POSITION, spot_position);	
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot_angle);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, spot_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, spot_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spot_specular);

	
	glEnable(GL_LIGHT1);

	//spot 2
	//lumina spot a navei eroului

	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, spot_direction);
    glLightfv(GL_LIGHT5, GL_POSITION, spot_position);	
	glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, spot_angle);
	
	glLightfv(GL_LIGHT5, GL_AMBIENT, spot_ambient);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, spot_diffuse);
	glLightfv(GL_LIGHT5, GL_SPECULAR, spot_specular);

	
	glEnable(GL_LIGHT5);
	//end desen nava Praslea

	if(explodeaza == 1)
	{
		//lumina satelit 
		GLfloat satelite_ambient [] = { 1, 0.3, 0, 1.0};
		GLfloat satelite_diffuse [] = {1, 1, 1, 1.0};
		GLfloat satelite_specular [] = {1.0, 1.0, 1.0, 1.0};
		GLfloat satelite_position [] = { sateliti[indice_satelit].x, 2, sateliti[indice_satelit].z, 1};
		
		glLightfv(GL_LIGHT3, GL_AMBIENT, satelite_ambient);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, satelite_diffuse);
		glLightfv(GL_LIGHT3, GL_SPECULAR, satelite_specular);
		glLightfv(GL_LIGHT3, GL_POSITION, satelite_position);
		glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1);
		glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0);
		glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.1 / sateliti[indice_satelit].raza);
		
		glEnable(GL_LIGHT3);
	}
	
	//desenare proiectile
	for(int i = 0 ; i < proiectile.size(); i ++)
	{
		glPushMatrix();

		glColor3f(0.0, 0.0, 1.0);
		proiectile[i].x += proiectile[i].trx;
		proiectile[i].z += proiectile[i].trz;
		glTranslatef(proiectile[i].x, 2, proiectile[i].z);
		glutSolidSphere(0.2,30,30);
		
		glPopMatrix();

		GLfloat omni_ambient [] = { 1, 1, 1, 1.0};
		GLfloat omni_diffuse [] = {1, 1, 1, 1.0};
		GLfloat omni_specular [] = {1.0, 1.0, 1.0, 1.0};
		GLfloat omni_position [] = { proiectile[i].x, 2, proiectile[i].z, 1};
		
		glLightfv(GL_LIGHT2, GL_AMBIENT, omni_ambient);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, omni_diffuse);
		glLightfv(GL_LIGHT2, GL_SPECULAR, omni_specular);
		glLightfv(GL_LIGHT2, GL_POSITION, omni_position);
		glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1);
		glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.2);
		glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.08);
		
		glEnable(GL_LIGHT2);
	}
	
	glDisable(GL_LIGHTING);
	
	glutSwapBuffers();

}

//functie pentru creare stele pe pozitii random
Point* createRandomStarArray(int numOfStars,float xMin,float xMax, float yMin,float yMax, float zMin, float zMax)
{	
	Point * points; 
	srand((unsigned)time(0));
	
	points = new Point[numOfStars];
	
	int xScope = (int)(abs(xMin) + (abs(xMax)));
	int yScope = (int)(abs(yMin) + (abs(yMax)));
	int zScope = (int)(abs(zMin) + (abs(zMax)));
	int radiusScope = (int) (1 + 100);
	for(int index=0; index<numOfStars; index++) 
	{
		points[index].x = rand()%(xScope)+1.0 - abs(xMin);
		points[index].y = rand()%(yScope)+1.0 - abs(yMin);
		points[index].z = rand()%(zScope)+1.0 - abs(zMin);
		points[index].radius = (rand()%(radiusScope) + 1.0 - 1) / 100;
		
	}
	return points;
}

void animatieDisplay()
{
	
	//directiile navelor inamice cand intalnesc peretele(limitele jocului)
	for(int i = 0 ; i < sateliti.size() ; i ++)
	{
		if(sateliti[i].z < -7)
			sateliti[i].z = 7;
	}
	if(care_camera == 2)
		camera.position = Vector3D(sateliti[1].x, sateliti[1].y + 1, sateliti[1].z);

	//disparitia proiectilelor in caz in care ies din joc, intalnesc o nava sau un satelit
	for(int i = 0 ; i < proiectile.size() ; i ++)
	{
		if(proiectile[i].x > 10 || proiectile[i].x < -10 || proiectile[i].z > 7 || proiectile[i].z < - 7)
		{
			proiectile.erase(proiectile.begin() + i);
			glDisable(GL_LIGHT2);
			proiectil_pe_ecran = 0;
			break;
		}
		int j;
		
	}
	
	glutPostRedisplay();
}

//directiile navei eroului pe sageti
void  sageti (int key, int a, int b){

	switch(key){
		case GLUT_KEY_UP: z_hero += 0.1;  break;
		case  GLUT_KEY_DOWN: z_hero -= 0.1; break;
		case  GLUT_KEY_LEFT: x_hero += 0.1; break;
		case GLUT_KEY_RIGHT: x_hero -= 0.1; break;
		case GLUT_KEY_PAGE_UP: y_hero += 0.1; break;
		case GLUT_KEY_PAGE_DOWN: y_hero -= 0.1; break;
		case GLUT_KEY_F1:
			{	
				if(satelit_selectat != -1)
				{
					sateliti.erase(sateliti.begin() + satelit_selectat);
					satelit_selectat = -1;
				}
				break;
			}
	}
	if(care_camera == 1)
		camera.position = Vector3D(x_hero, y_hero - 4, z_hero);
	glutPostRedisplay();
}

// right-click button poate opri animatiile ...si miscarile navelor inamice(just for back-up, nu strica)
void mouse(int buton, int stare, int x, int y)
{
	if (buton==GLUT_RIGHT_BUTTON && stare==GLUT_DOWN) 
	{ 
		satelit_selectat = -1;
	}
	if (buton==GLUT_LEFT_BUTTON && stare==GLUT_DOWN) 
	{ 
		//determin coordonatele in 3d 
		GLint viewport[4]; 
		GLdouble modelview[16]; 
		GLdouble projection[16]; 
		GLfloat winX, winY, winZ; 

		glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); 
		glGetDoublev( GL_PROJECTION_MATRIX, projection ); 
		glGetIntegerv( GL_VIEWPORT, viewport ); 
		winX = (float)x; 
		winY = (float)viewport[3] - (float)y; 
		glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ ); 
		gluUnProject( winX, winY, winZ, modelview, projection, viewport, &x_sel, &y_sel, &z_sel); 

		int i;
		double min = 30000000;
		for(i = 0 ; i < sateliti.size(); i ++)
		{
			int aux = sqrt(abs (sateliti[i].x - x_sel) * abs (sateliti[i].x - x_sel) + abs(sateliti[i].z - z_sel)* abs(sateliti[i].z - z_sel)) ;
			if(min > aux)
			{
				min = aux;
				satelit_selectat = i;
			}
		}
	}
}

//tastele w,a,s,d arunca proiectile dupa directiile N, W, S, E
void taste(unsigned char key, int x, int y){
	int aruncat_proiectil = 0;
	if (proiectil_pe_ecran == 0)
		switch(key)
		{
			case 'w':
				camera.translate_Forward(0.5);
				aruncat_proiectil = 1;
				break;
			case 'x':
				camera.translate_Right(0.5);
				break;
			case 'z':
				camera.translate_Right(-0.5);
				break;
			case 'c':
				camera.translate_Up(0.5);
				break;
			case 'v':
				camera.translate_Up(-0.5);
				break;
			case 'a':
				camera.rotateFPS_OY(-0.1);
				break;
			case 's':
				camera.translate_Forward(-0.5);
				break;
			case 'd':
				camera.rotateFPS_OY(0.1);
				break;
			case 't':
				camera.rotateFPS_OX(0.1);
				break;
			case 'y':
				camera.rotateFPS_OX(-0.1);
				break;
			case 'u':
				camera.rotateFPS_OZ(-0.1);
				break;
			case 'i':
				camera.rotateFPS_OZ(0.1);
				break;

			case 'q':
				camera.rotateTPS_OY(-0.1,10);
				break;
			case 'e':
				camera.rotateTPS_OY(0.1,10);
				break;

			case 'g':
				camera.rotateTPS_OX(0.1,10);
				break;
			case 'h':
				camera.rotateTPS_OX(-0.1,10);
				break;
			case 'j':
				camera.rotateTPS_OZ(-0.1,10);
				break;
			case 'k':
				camera.rotateTPS_OZ(0.1,10);
				break;
			case '`':
			{
				care_camera = 1;
				camera.position = Vector3D(x_hero, y_hero - 4, z_hero);
				camera.forward = Vector3D(0,0,1);
				camera.up = Vector3D(0,1,0);
				camera.right = Vector3D(-1,0,0);
				break;
			}
			case '1':
			{
				care_camera = 2;
				camera.position = Vector3D(sateliti[1].x, sateliti[1].y + 1, sateliti[1].z);
				camera.forward = Vector3D(0,0,-1);
				camera.up = Vector3D(0,1,0);
				camera.right = Vector3D(1,0,0);
				break;
			}
			case '2':
			{
				care_camera = 0;
				camera.position = Vector3D(0,15,0);
				camera.forward = Vector3D(0,-1,0);
				camera.up = Vector3D(0,0,1);
				camera.right = Vector3D(-1,0,0);
				break;
			}
		}

	if (aruncat_proiectil == 1)
	{
		navuta proiectil;
		proiectil.x = x_hero;
		proiectil.z = z_hero;
		proiectil.trx = x_pr;
		proiectil.trz = z_pr;
		proiectile.push_back(proiectil);
		proiectil_pe_ecran = 1;
	}
	
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640,480);
	glEnable(GL_DEPTH_TEST);
	glutInitWindowPosition(100,100);
	glutCreateWindow("No Name Game");
	init();
	camera.init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutIdleFunc(animatieDisplay);
	glutSpecialFunc(sageti);
	glutKeyboardFunc(taste);
	glutMainLoop();
}
  