#define _CRT_SECURE_NO_DEPRECATE
#include <GL\glew.h>
#include <GL\freeglut.h>   // handle the window-managing operations
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>
using namespace std;
#define PI 3.1415926535898

int scale_x=2, scale_y=1.5;
int angle_rotate = 1;
int inc_rotate = 1;

GLfloat BXMAX, BXMIN, BYMAX, BYMIN;
GLfloat TBXMAX, TBXMIN;
GLint points = 100;
float size_of_ball = 0.2;  //change the size of ball
GLfloat x = 0.0, y = 0.0, xt = 0.0, yt = -0.8;
GLdouble l, r, b, t;
GLfloat speed_horizontal = 0.001f; //speedX
GLfloat speed_vertical = 0.001f; //speedY
int p, q;
int lock = 0, level = 1;
int touch_ball_board = 0;
const char* score_level;
const char* level_now;

GLuint texture[3];

//lighting


GLfloat light_ambient[] = { 1.0, 1.0, 0.0, 1.0 };
GLfloat light_diffuse[] = { 0.2, 0.0, 0.4, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position[] = { 3.0, 5.0, 10.0, 1.0 };// light position
GLfloat shininess[] = { 128 };
//
GLuint LoadTexture(const char* filename)
{
    GLuint texture;

    FILE* my_file = fopen(filename, "rb");

    unsigned char offSet[14];
    fread(offSet, sizeof(unsigned char), 14, my_file);

    unsigned char* info = new unsigned char[*(int*)&offSet[10] - 14];
    fread(info, sizeof(unsigned char), *(int*)&offSet[10] - 14, my_file);


    int width = *(int*)&info[4];
    int height = *(int*)&info[8];


    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size];


    fread(data, sizeof(unsigned char), size, my_file);
    fclose(my_file);


    int i;
    for (i = 0; i < size; i += 3)
    {
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }

    /////////////////////////////////////////
    //////
    glGenTextures(1, &texture);            //generate the texture with the loaded data
    glBindTexture(GL_TEXTURE_2D, texture); //bind the texture to it's array

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); //set texture environment parameters

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);// Image data to texture

    glBindTexture(GL_TEXTURE_2D, 0);// Unbind texture



 /////////////////////////////////////////

    free(data); //free the texture array

    if (glGetError() != GL_NO_ERROR)
        printf("GLError in genTexture()\n");

    return texture; //return whether it was successfull
}


//fuction to print string of characters on the window screen
void print_text(int x, int y, int z, const char* string) {
    glRasterPos2f(x, y);
    int i;
    int len = (int)strlen(string);

    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
        //in built func to print the text bit vz
    }
}


void draw_background()
{
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0); //the base color of background
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D); // Enable texture for drawing
    glBindTexture(GL_TEXTURE_2D, texture[1]);// Binding texture
    // Drawing a quad with texture mapping
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); // Top Left
    glVertex2f(l, t);
    glTexCoord2f(1.0, 1.0);// Top Right
    glVertex2f(r, t);
    glTexCoord2f(1.0, 0.0);// Bottom Right
    glVertex2f(r, b);
    glTexCoord2f(0.0, 0.0);// Bottom Left
    glVertex2f(l, b);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0); // Unbinding Texture
    glPopMatrix();
}

void draw_Board()
{
    // Draw Board
    glPushMatrix();
    glTranslatef(xt, yt, 0.0);
    glScalef(scale_x, scale_y, 1);
    glColor3f(0.0, 0.0, 0.0); //the color of board
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(-0.3, 0.1);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(0.3, 0.1);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(0.3, -0.1);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(-0.3, -0.1);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    // we pop that pic to screen
    glPopMatrix();
}


void draw_ball()
{
    glPushMatrix();
    glTranslatef(x, y, 0.00);
    glRotatef(angle_rotate, 0, 0, 1);
    // Draw Ball
    int i;
    float angle;
    glColor3f(0.5, 0.3, 0.2); //color of ball
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_POLYGON);
    // this is to draw a circle on the screen
    for (i = 0; i < points; i++)
    {
        angle = 2 * PI * i / points;
        glTexCoord2f(((cos(angle)) * 4.9 * size_of_ball + 1.0) / 2.0, ((sin(angle)) * 4.9 * size_of_ball + 1.0) / 2.0); // Mapping texture to circle
        glVertex2f((cos(angle)) * size_of_ball, (sin(angle)) * size_of_ball);
    }
    glEnd();
    //pop that circle on the screen
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();


}

void draw_light()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    /////////////////////////////////////////

    glColor3f(0.5, 0.5, 0.8);
    glEnable(GL_DEPTH_TEST);
    glutSolidTorus(0.137, 0.42, 10, 10); //purple tara size 
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    /////////////////////////////////////////

    glFlush();

}


void score()
{
    //  the score is the number of times the board blocks the ball
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(0.98, 0.92, 0.00);
    switch (touch_ball_board) {  //score in touch 
    case 0: score_level = "00"; break;
    case 1: score_level = "01"; break;
    case 2: score_level = "02"; break;
    case 3: score_level = "03"; break;
        /* case 4: score_level = "04"; break;
         case 5: score_level = "05"; break;*/
    default: score_level = "00";
    }
    print_text(0, 0, 0, score_level);
    glPopMatrix();
}


void levels_game()
{
    // value of level
    glPushMatrix();
    glColor3f(0.3, 0.5, 0.3);
    glTranslatef(-0.3, 0.92, 0.00);
    switch (level) {

    case 1: level_now = "01"; break;
    case 2: level_now = "02"; break;
    case 3: level_now = "03"; break;
    case 4: level_now = "04"; break;
    case 5: level_now = "05"; break;
    case 6: level_now = "06"; break;
    case 7: level_now = "07"; break;
    case 8: level_now = "08"; break;
    case 9: level_now = "09"; break;
    case 10: level_now = "10"; break;
    case 11: level_now = "11"; break;
    case 12: level_now = "12"; break;
    default: level_now = "00";

    }
    print_text(0, 0, 0, level_now);
    glPopMatrix();

}

// fuction to intialz basic window
void init_GLM() {
    glClearColor(0.5, 1.0, 0.8, 1.0); //color of background
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glEnable(GL_LINE_SMOOTH);
    // Load images to texture
    texture[0] = LoadTexture("Data\\Image\\Ball.bmp");  // Loading image of ball to  texture
    texture[1] = LoadTexture("Data\\Image\\Bg.bmp");    // Loading image of background to  texture
    texture[2] = LoadTexture("Data\\Image\\Board.bmp"); // Loading image of board to  texture

//lighting

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    /////////////////////////////////////////
}



/*idle function to show the current picture on the screen while next
one being drawn in processor */
void idle() {
    glutPostRedisplay();
}

// display function
void display() {
    // Draw Background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_background();
 
    if (lock == 0)
    {
        draw_light();
      
        draw_Board();

        draw_ball();

        score();

        // level display
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);
        glTranslatef(-0.6, 0.92, 0.00);
        glPopMatrix();

        levels_game();
        glutSwapBuffers();
        //If the player reaches the forth level, set lock = 2 to end the game
        if (level == 4) {
            lock = 2;
        }
        angle_rotate += inc_rotate;
        if (x > BXMAX) { x = BXMAX; speed_horizontal = -speed_horizontal; }
        else if (x < BXMIN) { x = BXMIN; speed_horizontal = -speed_horizontal; }
        if (y > BYMAX) { y = BYMAX;  speed_vertical = -speed_vertical; }
        else if (y < BYMIN) { y = BYMIN; speed_vertical = -speed_vertical; }

        float f;
        float a1, b1{}, a2, b2;
        a1 = ((xt)-(x));
        b1 == ((yt)-(y));
        a2 = pow(a1, 2);
        b2 = pow(b1, 2);
        f = sqrt(a2 + b2);

        if (f <= 0.424264068 && y <= -0.5 && y >= -0.7) {
            speed_vertical = -1.002 * speed_vertical; //bounce back the ball if it touches the bar
            touch_ball_board++;    // increase the score by 1

            scale_x = 1;

            if (touch_ball_board == 4) { //after 4 touch 
                touch_ball_board = 0;
                level++; // next level

                inc_rotate += 1;//increase rotate angle

                speed_horizontal = 1.2 * speed_horizontal; //increase the speed of ball in x-axis
                speed_vertical = 1.2* speed_vertical; //increase the speed of ball in y-axis
            }
        }
        if ((y < -0.55 && f < 0.5831 && f > 0.5)) {
            x = x; y = y; speed_horizontal = 0; speed_vertical = 0; 
            lock = 1; // make lock =1 to end and print game over

        }
        else {
            if (y <= -0.6)
            {
                x = x; y = y; speed_horizontal = 0; speed_vertical = 0; 
                lock = 1; // make lock =1 to end and print game over
            }
            else {
                //move the base rect to left
                p = xt - 0.3;
                //move the base rect to right
                q = xt + 0.3;
                // increase the speed of ball in x axis
                x = x + speed_horizontal;
                // increase the speed of ball in y axis
                y = y + speed_vertical;
            }
        }
    }
    else if (lock == 1)
    {
    
        // Display Game Over
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.3);
        glTranslatef(-0.20, 0.20, 0.00);
        print_text(0, 0, 0, "Game Over");
        glPopMatrix();
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.3);
        glTranslatef(-0.35, -0.10, 0.00);
        print_text(0, 0, 0, " Done by : fantastic group ");
        glPopMatrix();
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.4);
        glTranslatef(-0.85, -0.25, 0.00);
        print_text(0, 0, 0, " Arwa & lubaba & Sarah & bothainah & Ghaida & Fatma ");
        glPopMatrix();
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.3);
        glTranslatef(-0.35, -0.40, 0.00);
        print_text(0, 0, 0, "Best Supervisor : Dr.Hoda");
        glPopMatrix();
        glutSwapBuffers();
    }
    else {// If lock =2  ,,  player wins
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(-0.26, 0.20, 0.00);
    print_text(0,0,0, "* * * * * * * * * * * * *");
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(-0.26, 0.15, 0.00);
    print_text(0, 0, 0, "*    Congratulations    *");
    glPopMatrix();
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(-0.26, 0.10, 0.00);
    print_text(0, 0, 0, "*                                  *");
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(-0.26, 0.05, 0.00);
    print_text(0, 0, 0, "* * * * * * * * * * * * *");
    glPopMatrix();

    glutSwapBuffers();
    }
}


void reshape(int width, int height) {
    if (height == 0) height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (width >= height) {
        l = -1.0 * aspect;
        r = 1.0 * aspect;
        b = -1.0;
        t = 1.0;
    }
    else {
        l = -1.0;
        r = 1.0;
        b = -1.0 / aspect;
        t = 1.0 / aspect;
    }
    gluOrtho2D(l, r, b, t);
    BXMAX = r - (size_of_ball);
    BXMIN = l + (size_of_ball);
    BYMAX = t - (size_of_ball)-0.1;
    BYMIN = b + (size_of_ball);
    TBXMAX = r - (0.3);
    TBXMIN = l + (0.3);
}

void specialKeys(int key, int x, int y) {

    switch (key) {
    case GLUT_KEY_LEFT: if (lock == 0) {
        if (xt <= TBXMIN)
        {
            xt = TBXMIN;
        }
        else
        {
            xt = xt - 0.1;
            light_position[0] -= 0.5;
            light_position[1] -= 0.5;
            light_position[2] -= 0.5;
            shininess[0] -= 0.5;
        }
    }
                      break;
    case GLUT_KEY_RIGHT: if (lock == 0) {
        if (xt >= TBXMAX)
        {
            xt = TBXMAX;

        }
        else
        {
            xt = xt + 0.1;
            light_position[1] += 0.5;
            light_position[0] += 0.5;
            light_position[2] += 0.5;
            shininess[0] += 0.5;

        }
    }
                       break;
    }
}

int main(int argc, char** argv) {
    PlaySoundA("Data\\Music\\Fun_game.wav", NULL, SND_ASYNC | SND_LOOP);   // Play Audio
    glutInit(&argc, argv);   // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE);  // Setting display mode
    glutInitWindowSize(840, 680);   //  width & height window's
    glutInitWindowPosition(50, 50);  // Position the window's initial top-left corner
    glutCreateWindow("jumpping ball");       // Create a window with the given title
    glutReshapeFunc(reshape);       // Register callback for window resize
    glutSpecialFunc(specialKeys);// Register keyboard callback
    glutDisplayFunc(display);// Register display callback handler for window re-paint
    glutIdleFunc(idle);   // Rigister timer callback
    init_GLM();   // Initialize GLM
    glutMainLoop();   // Enter the event-processing loop
    return 0;
}