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

using namespace std;
typedef struct{
	int x, y, z;
	GLfloat radius;
}Point; // point = stea

Point * createRandomStarArray(int, float, float, float, float, float, float); 
typedef struct {
   GLdouble    x;
   GLdouble    z;
   GLdouble trx , trz;
   GLdouble raza; // folosita la sateliti pentru a vedea amploarea exploziei 
} navuta; 

GLfloat spin=0.0;
GLfloat aspect = 1;
GLfloat tr=0,step=0.01,sc=0.4,st=1.1, z_pr, x_pr; // z_pr si x_pr sunt directiile proiectilelor N , S, E, V
int x=0,y=0;
bool t=true;
int win_w = 1, win_h = 1;

int screen_size = 10; // jumatate din dimensiunea galaxiei; in mod normal infinit da dehh!
int nr_inamici = 5; // numarul initial de inamici din scena
int min_space = -7, max_space = 7; // maximele spatiului de joc intre care se genereaza nr random pt pozitiile navelor inamice
GLdouble x_hero, z_hero; // pozitia actuala a navei
int nr_sateliti = 2;  // nr de sateliti
//acesti vectori sunt folositi si pentru ca pozitiile random pe care se genereaza inamicii si satelitii sa nu se suprapuna
vector<navuta> proiectile; // vector de proiectile, in varianta finala unul singur
vector<navuta> navute; // vector ce contine inamicii, inamicii sunt representati de x si y, coordonatele pozitiei lor in spatiu
vector<navuta> sateliti; // vector ce contine satelitii
int nr_of_stars = 500; // nr de stele de pe ecran

int vieti = 2; //nr de vieti ale navei					
int proiectil_pe_ecran = 0; // proiectil_pe_ecran = 1 daca s-a dat deja drumul la un proiectil si acesta nu a diparut inca de pe ecran
Point * stars = new Point[nr_of_stars]; // vector de stele

//lumina directionala generala porneste de la -infinit pe axa oy
GLfloat light_position [] = {0.0, -1.0, 0.0, 0.0};
GLfloat light_ambient [] = { 0.0, 0.0, 0.0, 1};
GLfloat light_diffuse [] = {0.5, 0.5, 0.5, 1};
GLfloat light_specular [] = {0.5, 0.5, 0.5, 1};

//materiale nave si sateliti
GLfloat hero_ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat hero_diffuse[] = {0.6, 0.6, 0.6, 1.0};
GLfloat hero_specular[] = {0.30, 0.3, 0.3, 0.3};
GLfloat hero_shininess = 50;

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

	for(int i = 0 ; i < nr_inamici ; i ++)
	{
		int new_x = min_space + (rand() % (int)(max_space - min_space + 1));
		int new_z = min_space + (rand() % (int)(max_space - min_space + 1));
		for(int j = 0 ; j < navute.size(); j ++)
		{
			if( new_x == navute[j].x && new_z == navute[j].z)
			{
				new_x = min_space + (rand() % (int)(max_space - min_space + 1));
				new_z = min_space + (rand() % (int)(max_space - min_space + 1));
			}
		}
		for(int j = 0 ; j < sateliti.size(); j ++)
		{
			if( new_x == sateliti[j].x && new_z == sateliti[j].z)
			{
				new_x = min_space + (rand() % (int)(max_space - min_space + 1));
				new_z = min_space + (rand() % (int)(max_space - min_space + 1));
			}
		}

		navuta o_noua_nava_spatiala; // creez o noua intrare in vector care contine un obiect de tip navuta ce reprezinta un inamic
 		o_noua_nava_spatiala.x = new_x;
		o_noua_nava_spatiala.z = new_z;
		
		int aux_x =  (1 + (rand() % (int)(5 - 1 + 1)));
		int aux_z = (1 + (rand() % (int)(5 - 1 + 1)));
		if (aux_x % 2 == 0)
			o_noua_nava_spatiala.trx = aux_x / 80.0;
		else
			o_noua_nava_spatiala.trx = - aux_x / 80.0;
		if (aux_z % 2 == 0)
			o_noua_nava_spatiala.trz = aux_z / 80.0;
		else
			o_noua_nava_spatiala.trz = - aux_z / 80.0;
		navute.push_back(o_noua_nava_spatiala);
	}

	for(int i = 0 ; i < nr_sateliti ; i ++)
	{
		int new_x = min_space + (rand() % (int)(max_space - min_space + 1));
		int new_z = min_space + (rand() % (int)(max_space - min_space + 1));
		for(int j = 0 ; j < navute.size(); j ++)
		{
			if( new_x == navute[j].x && new_z == navute[j].z)
			{
				new_x = min_space + (rand() % (int)(max_space - min_space + 1));
				new_z = min_space + (rand() % (int)(max_space - min_space + 1));
			}
		}
		for(int j = 0 ; j < sateliti.size(); j ++)
		{
			if( new_x == sateliti[j].x && new_z == sateliti[j].z)
			{
				new_x = min_space + (rand() % (int)(max_space - min_space + 1));
				new_z = min_space + (rand() % (int)(max_space - min_space + 1));
			}
		}
		navuta satelit; // creez o noua intrare in vectorul de navute cu o navuta reprezentata de un satelit
		satelit.x = new_x;
		satelit.z = new_z;
		satelit.raza = 0;
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

	gluLookAt(0.0, 10.0, 0.0,   //observatorul este in origine la 10 pe axa 0y
		      0.0, -1.0, 0.0,   //si priveste in directia negativa a axei oy
			  0.0, 0.0, 1.0);  

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
    glEnable(GL_LIGHTING);
	
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);	

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	
	glEnable(GL_LIGHT0);

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

	//verifica daca eroul a fost lovit de o nava inamica=> si-a pierdut o viata sau jocul s-a terminat daca nu mai are vieti
	for(int i = 0 ; i < navute.size() ; i ++)
	{
		if(x_hero >= navute[i].x - 0.5 && x_hero <= navute[i].x + 0.5 && z_hero >= navute[i].z - 0.5 && z_hero <= navute[i].z + 0.5)
		{
			vieti --;
			x_hero = 0; z_hero = 0;
			if (vieti == 0)
			{
				glColor3f(1, 1, 1);
				printw(3, 3, 0, "GAME OVER");
				exit(0);
			}
			else break;
		}
	}

	//daca nu mai exista nicio nava inamica in joc apare gaura de vierme
	if(navute.size() == 0)
	{
		//desenare gaura de vierme
		glPushMatrix();
		glShadeModel(GL_FLAT);
		glColor3f(0.3, 0.3, 0.5);
		glTranslatef(0, 2, 6);
		glRotatef(90, 1, 0, 0);
		glutSolidSphere(1, 15, 15);
		
		glPopMatrix();
		
		//daca eroul se gaseste in gaura de vierme jocul s-a terminat cu succes si aceasta se lumineaza
		if(x_hero <= 1 && x_hero >= -1 && z_hero <= 7 && z_hero >= 5)
		{
			//desenare lumina gaura de vierme
			GLfloat satelite_ambient [] = { 0, 0.3, 1, 1.0};
			GLfloat satelite_diffuse [] = {1, 1, 1, 1.0};
			GLfloat satelite_specular [] = {1.0, 1.0, 1.0, 1.0};

			GLfloat satelite_position [] = { 0, 2, 6, 1};
		
			glLightfv(GL_LIGHT4, GL_AMBIENT, satelite_ambient);
			glLightfv(GL_LIGHT4, GL_DIFFUSE, satelite_diffuse);
			glLightfv(GL_LIGHT4, GL_SPECULAR, satelite_specular);
			glLightfv(GL_LIGHT4, GL_POSITION, satelite_position);
			
			glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, 1);
			glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, 0);
			glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, 0.1 / 2);
			glEnable(GL_LIGHT4);

			glColor3f(1, 1, 1);
			printw(3, 3, -6, "YOU WIN!");			
		}
	}

	//desen nava Praslea cel Voinic
	glPushMatrix();
	glColor3f(0.0, 0.35, 0.4);
	glTranslatef(x_hero, 2, z_hero);
	glRotatef(90, 1.0, 0.0, 0.0);
	glutSolidTorus(0.1, 0.5, 30, 30);
	glColor3f(0.0, 0.0, 0.55);
	glutSolidSphere(0.2, 30, 30);
	glPopMatrix();

	//lumina spot a navei eroului
	GLfloat spot_direction [] = {0, 0, 1};
	GLfloat spot_position [] = {x_hero, 2.0, z_hero, 1.0};
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
	//end desen nava Praslea
	
	//desenare inamici
	for(int i = 0 ; i < navute.size(); i ++)
	{
		glPushMatrix();
	
		glColor3f(0.45, 0.4, 0.0);
		navute[i].x += navute[i].trx;
		navute[i].z += navute[i].trz;
		glTranslatef(navute[i].x, 2, navute[i].z);
		glutSolidTeapot(0.5);
		
		glPopMatrix();
	}
	//end of desenare inamici

	//start of desenare sateliti
	for(int i = 0 ; i < sateliti.size(); i ++)
	{
		glPushMatrix();

		glColor3f(0.4, 0.4, 0.4);
		glTranslatef(sateliti[i].x, 0, sateliti[i].z);
		glutSolidDodecahedron();
		glPopMatrix();
	}

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
	if(explodeaza == 1)
	{
		if(implodeaza == 1)
		{
			sateliti[indice_satelit].raza -= 0.01;
			if(sateliti[indice_satelit].raza <= 0.001)
			{
				int new_x = min_space + (rand() % (int)(max_space - min_space + 1));
				int new_z = min_space + (rand() % (int)(max_space - min_space + 1));
				
				for(int k = 0 ; k < navute.size(); k ++)
				{
					if( new_x == navute[k].x && new_z == navute[k].z)
					{
						new_x = min_space + (rand() % (int)(max_space - min_space + 1));
						new_z = min_space + (rand() % (int)(max_space - min_space + 1));
					}
				}
				for(int k = 0 ; k < sateliti.size(); k ++)
				{
					if( new_x == sateliti[k].x && new_z == sateliti[k].z)
					{
						new_x = min_space + (rand() % (int)(max_space - min_space + 1));
						new_z = min_space + (rand() % (int)(max_space - min_space + 1));
					}
				}
			
				sateliti[indice_satelit].x = new_x;
				sateliti[indice_satelit].z = new_z;
				explodeaza = 0;
				implodeaza = 0;
				glDisable(GL_LIGHT3); 
			}
		}
		else
		{
			sateliti[indice_satelit].raza += 0.01;
			if (sateliti[indice_satelit].raza >= 0.5)
			{
				implodeaza = 1;
			}
		}
		
			
	}

	//directiile navelor inamice cand intalnesc peretele(limitele jocului)
	for(int i = 0 ; i < navute.size() ; i ++)
	{
		if(navute[i].x > 10)
			navute[i].trx = - navute[i].trx;
		if(navute[i].x < -10)
			navute[i].trx = - navute[i].trx;
		if(navute[i].z > 7)
			navute[i].trz = - navute[i].trz;
		if(navute[i].z < -7)
			navute[i].trz = - navute[i].trz;
	}

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
		for(j = 0 ; j < navute.size() ; j ++)
		{
				
			if(proiectile[i].x >= navute[j].x - 0.6 && proiectile[i].x <= navute[j].x + 0.6 && proiectile[i].z >= navute[j].z - 0.6 && proiectile[i].z <= navute[j].z + 0.6)
			{
				//Hit a  a SHIP
				proiectile.erase(proiectile.begin() + i);
				glDisable(GL_LIGHT2);
				proiectil_pe_ecran = 0;
				break;
			}
		}
		if (j != navute.size())
			break;
		for(j = 0 ; j < sateliti.size() ; j ++)
		{
			if(proiectile[i].x >= sateliti[j].x - 1.73 && proiectile[i].x <= sateliti[j].x + 1.73 && proiectile[i].z >= sateliti[j].z - 1.73 && proiectile[i].z <= sateliti[j].z + 1.73)
			{
				//HIT A Satelite; Satelitul dispare si apare in alta pozitie random pe harta de joc
				// si distruge toate navele din jur (raza pentru distrugerea inamicilor pe care am ales -o  este 2
				for(int k = 0; k < navute.size() ; k ++)
				{
					if(navute[k].x >= sateliti[j].x - 4 && navute[k].x <= sateliti[j].x + 4 && navute[k].z >= sateliti[j].z - 4 && navute[k].z <= sateliti[j].z + 4)
					{
						navute.erase(navute.begin() + k);
						k --;
					}
			
				}
				explodeaza = 1;
				indice_satelit = j;

				//lumina satelit
				GLfloat satelite_ambient [] = { 1, 1, 1, 1.0};
				GLfloat satelite_diffuse [] = {1, 1, 1, 1.0};
				GLfloat satelite_specular [] = {1.0, 1.0, 1.0, 1.0};
				GLfloat satelite_position [] = { sateliti[j].x, 2, sateliti[j].z, 1};
		
				glLightfv(GL_LIGHT3, GL_AMBIENT, satelite_ambient);
				glLightfv(GL_LIGHT3, GL_DIFFUSE, satelite_diffuse);
				glLightfv(GL_LIGHT3, GL_SPECULAR, satelite_specular);
				glLightfv(GL_LIGHT3, GL_POSITION, satelite_position);
				glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1);
				glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0);
				glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.1 / sateliti[j].raza);
		
				glEnable(GL_LIGHT3);
				
				
				proiectile.erase(proiectile.begin() + i);
				glDisable(GL_LIGHT2);
				proiectil_pe_ecran = 0;
				break;
			}
		}
	}
	
	glutPostRedisplay();
}

//directiile navei eroului pe sageti
void  sageti (int key, int a, int b){

	switch(key){
		case GLUT_KEY_UP:  z_hero += 0.1; break;
		case  GLUT_KEY_DOWN: z_hero -= 0.1; break;
		case  GLUT_KEY_LEFT: x_hero += 0.1; break;
		case GLUT_KEY_RIGHT: x_hero -= 0.1; break;
	}
	glutPostRedisplay();
}

// right-click button poate opri animatiile ...si miscarile navelor inamice(just for back-up, nu strica)
void mouse(int buton, int stare, int x, int y)
{
	switch(buton)
	{
	case GLUT_LEFT_BUTTON:
		
			glutIdleFunc(animatieDisplay);
		break;
	case GLUT_RIGHT_BUTTON:
		
			glutIdleFunc(NULL);
		break;
	}
}

//tastele w,a,s,d arunca proiectile dupa directiile N, W, S, E
void taste(unsigned char key, int x, int y){
	int aruncat_proiectil = 0;
	if (proiectil_pe_ecran == 0)
		switch(key)
		{
			case 'w': 
				z_pr = 0.01; 
				x_pr = 0;
				aruncat_proiectil = 1;
				break;
			case 's':
				z_pr = -0.01;
				x_pr = 0;
				aruncat_proiectil = 1;
				break;
			case 'a' : 
				z_pr = 0;
				x_pr = 0.01;
				aruncat_proiectil = 1;
				break;
			case 'd' :
				z_pr = 0;
				x_pr = -0.01;
				aruncat_proiectil = 1;
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
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(640,480);
	glutInitWindowPosition(100,100);
	glutCreateWindow("No Name Game");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutIdleFunc(animatieDisplay);
	glutSpecialFunc(sageti);
	glutKeyboardFunc(taste);
	glutMainLoop();
}
  