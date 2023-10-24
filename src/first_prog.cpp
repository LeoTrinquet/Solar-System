// TP Corrige avec ajout de la gestion des textures
// For Code::Blocks 20 or higher (gcc/g++ x64)
// Date: 2022
// Using SDL, SDL OpenGL and standard IO
#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <GL/GLU.h>
#include <random>
#include <cstdlib>
#include <vector>

// Module for space geometry
#include "geometry.h"
// Module for generating and rendering forms
#include "forms.h"
//#include "param.h"

/***************************************************************************/
/* Constants and functions declarations                                    */
/***************************************************************************/
// Screen dimension constants
const int SCREEN_WIDTH = 1700;
const int SCREEN_HEIGHT = 1000;

// Max number of forms : static allocation
const int MAX_FORMS_NUMBER = 21;

// Animation actualization delay (in ms) => 100 updates per second
const Uint32 ANIM_DELAY = 10;

// Render actualization delay 40 (in ms) => 25 updates per second
const Uint32 FRAME_DELAY = 10;

// Create a coeff for Delta_t so we change the perception of Time

float Coeff_Temps = 1000000;
const double coeff = (149e9)/2;

// Starts up SDL, creates window, and initializes OpenGL
bool init(SDL_Window** window, SDL_GLContext* context);

// Initializes matrices and clear color
bool initGL();

// Updating forms for animation
void update(Form* formlist[MAX_FORMS_NUMBER], double delta_t);

// Renders scene to the screen
void render(Form* formlist[MAX_FORMS_NUMBER], const Point &cam_pos, const Point &origine, double angle, double phi, int focus, Point camPosFocus, Point viseur);

// Frees media and shuts down SDL
void close(SDL_Window** window);

// Creates a texture into graphic memory from an image file and assign it a
// unique ID, inside textureID
// returns 0 if all went fine, a negative value otherwise
int createTextureFromImage (const char* filename, GLuint* textureID);


/***************************************************************************/
/* Functions implementations                                               */

/***************************************************************************/

bool init(SDL_Window** window, SDL_GLContext* context)
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        success = false;
    }
    else
    {
        // Use OpenGL 2.1
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        // Create window
        *window = SDL_CreateWindow( "TP intro OpenGL / SDL 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
        if( *window == NULL )
        {
            std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
            success = false;
        }
        else
        {
            // Create context
            *context = SDL_GL_CreateContext(*window);
            if( *context == NULL )
            {
                std::cerr << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
                success = false;
            }
            else
            {
                // Use Vsync
                if(SDL_GL_SetSwapInterval(1) < 0)
                {
                    std::cerr << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
                }

                // Initialize OpenGL
                if( !initGL() )
                {
                    std::cerr << "Unable to initialize OpenGL!"  << std::endl;
                    success = false;
                }
            }
        }
    }

    return success;
}


bool initGL()
{
    bool success = true;
    GLenum error = GL_NO_ERROR;

    // Initialize Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the viewport : use all the window to display the rendered scene
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Fix aspect ratio and depth clipping planes
    gluPerspective(40.0, (GLdouble)SCREEN_WIDTH/SCREEN_HEIGHT, 1.0, 100.0);

    // Initialize Modelview Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Initialize clear color : black with no transparency
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

    // Activate Z-Buffer
    glEnable(GL_DEPTH_TEST);


    // Lighting basic configuration and activation
    const GLfloat light_ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
    const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

    const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
    const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
    const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat high_shininess[] = { 100.0f };

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);


    // Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        std::cerr << "Error initializing OpenGL!  " << gluErrorString( error ) << std::endl;
        success = false;
    }

    return success;
}

void update(Form* formlist[MAX_FORMS_NUMBER], double delta_t)
{
    // Update the list of forms
    unsigned short i = 0;
    while(formlist[i] != NULL)
    {
        formlist[i]->update(delta_t);
        i++;
    }
}

void render(Form* formlist[MAX_FORMS_NUMBER], const Point &cam_pos, const Point &origine, double rho, double phi, int focus, Point camPosFocus, Point viseur)
{
    // Clear color buffer and Z-Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if (focus != 0){
        gluLookAt(camPosFocus.x,camPosFocus.y,camPosFocus.z, origine.x,origine.y,origine.z, 0.0,1.0,0.0);
    }
    else {
        // Set the camera position and parameters
        gluLookAt(cam_pos.x,cam_pos.y,cam_pos.z, origine.x,origine.y,origine.z, 0.0,1.0,0.0);
    }

    // Isometric view
    glRotated(rho, 0, 1, 0);
    glRotated(phi, 1, 0, -1);

    // X, Y and Z axis
    glPushMatrix(); // Preserve the camera viewing point for further forms
    // Render the coordinates system
    glBegin(GL_LINES);
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3i(0, 0, 0);
        glVertex3i(1, 0, 0);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3i(0, 0, 0);
        glVertex3i(0, 1, 0);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3i(0, 0, 0);
        glVertex3i(0, 0, 1);
    }
    glEnd();
    glPopMatrix(); // Restore the camera viewing point for next object

    // Render the list of forms
    unsigned short i = 0;
    while(formlist[i] != NULL)
    {
        glPushMatrix(); // Preserve the camera viewing point for further forms
        formlist[i]->render();
        glPopMatrix(); // Restore the camera viewing point for next object
        i++;
    }
}

void close(SDL_Window** window)
{
    //Destroy window
    SDL_DestroyWindow(*window);
    *window = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}


int createTextureFromImage (const char* filename, GLuint* textureID)
{
    SDL_Surface *imgSurface = IMG_Load(filename);
    if (imgSurface == NULL)
    {
        std::cerr << "Failed to load texture image: " << filename << std::endl;
        return -1;
    }
    else
    {
        // Work out what format to tell glTexImage2D to use...
        int mode;
        if (imgSurface->format->BytesPerPixel == 3)   // RGB 24bit
        {
            mode = GL_RGB;
        }
        else if (imgSurface->format->BytesPerPixel == 4)     // RGBA 32bit
        {
            mode = GL_RGBA;
        }
        else
        {
            SDL_FreeSurface(imgSurface);
            std::cerr << "Unable to detect the image color format of: " << filename << std::endl;
            return -2;
        }
        // create one texture name
        glGenTextures(1, textureID);

        // tell opengl to use the generated texture name
        glBindTexture(GL_TEXTURE_2D, *textureID);

        // this reads from the sdl imgSurface and puts it into an openGL texture
        glTexImage2D(GL_TEXTURE_2D, 0, mode, imgSurface->w, imgSurface->h, 0, mode, GL_UNSIGNED_BYTE, imgSurface->pixels);

        // these affect how this texture is drawn later on...
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        // clean up
        SDL_FreeSurface(imgSurface);
        return 0;
    }
}

/***************************************************************************/
/* MAIN Function                                                           */
/***************************************************************************/
int main(int argc, char* args[])
{

    // The window we'll be rendering to
    SDL_Window* gWindow = NULL;

    // OpenGL context
    SDL_GLContext gContext;


    // Start up SDL and create window
    if( !init(&gWindow, &gContext))
    {
        std::cerr << "Failed to initialize!" << std::endl;
    }
    else
    {
        // Main loop flag
        bool quit = false;
        Uint32 current_time, previous_time_anim, previous_time_render, elapsed_time_anim, elapsed_time_render;

        // Event handler
        SDL_Event event;

        // Camera
        double hCam = 50;
        double rho = 0;
        double phi = 0;
        double camDist = 5;
        Point camera_position(0,0,0);
        camera_position.x = camDist;
        camera_position.y = 0;
        camera_position.z = camDist;
        Point origine(0, 0, 0);
        bool vueHaut = false;
        double savedRho = rho;
        int focus = 0;
        Point camPosFocus(0,0,0);
        Point camViseur(0,0,0);

        bool isCtrlPressed = false;
        bool isAMercurePressed = false;
        bool isZVenusPressed = false;
        bool isETerrePressed = false;
        bool isRMarsPressed = false;
        bool isTJupiterPressed = false;
        bool isYSaturnePressed = false;
        bool isUUranusPressed = false;
        bool isINeptunePressed = false;
        bool isOSoleilPressed = false;
        bool isPMeteoritePressed = false;

        bool focus1 = false;
        bool focus2 = false;
        bool focus3 = false;
        bool focus4 = false;
        bool focus5 = false;
        bool focus6 = false;
        bool focus7 = false;
        bool focus8 = false;
        bool focus9 = false;

        bool isMercureInv = false;
        bool isVenusInv = false;
        bool isTerreInv = false;
        bool isMarsInv = false;
        bool isJupiterInv = false;
        bool isSaturneInv = false;
        bool isUranusInv = false;
        bool isNeptuneInv = false;
        bool isMeteoriteInv = false;

        bool isbPressed = false;

        // Textures creation //////////////////////////////////////////////////////////
        GLuint textureid_Mercure, textureid_Venus,textureid_Terre, textureid_Mars,textureid_Jupiter, textureid_Saturne, textureid_Uranus, textureid_Neptune, textureid_Soleil, textureid_Meteorite;
        createTextureFromImage("resources/images/mercure_texture.jpg", &textureid_Mercure);
        createTextureFromImage("resources/images/venus_texture.jpg", &textureid_Venus);
        createTextureFromImage("resources/images/earth_texture.jpg", &textureid_Terre);
        createTextureFromImage("resources/images/mars_texture.jpg", &textureid_Mars);
        createTextureFromImage("resources/images/jupiter_texture.jpg", &textureid_Jupiter);
        createTextureFromImage("resources/images/saturne_texture.jpg", &textureid_Saturne);
        createTextureFromImage("resources/images/uranus_texture.jpg", &textureid_Uranus);
        createTextureFromImage("resources/images/neptune_texture.jpg", &textureid_Neptune);
        createTextureFromImage("resources/images/sun_texture.jpg", &textureid_Soleil);
        createTextureFromImage("resources/images/sun_texture.jpg", &textureid_Soleil);
        createTextureFromImage("resources/images/meteorite_texture.jpg", &textureid_Meteorite);

        // Textures ready to be enabled (with private member " texture_id" of each form)


        // The forms to render
        Form* forms_list[MAX_FORMS_NUMBER];
        unsigned short number_of_forms = 0, i;
        for (i=0; i<MAX_FORMS_NUMBER; i++)
        {
            forms_list[i] = NULL;
        }

        // Spheres
        Sphere* Mercure = new Sphere(rayonMercure,WHITE ,masseMercure);
        Animation sphAnimMercure;
        sphAnimMercure.setPos(Point(distanceSoleilMercure,0,0));
        sphAnimMercure.setPhi(10); // angle en degre
        sphAnimMercure.setTheta(0); // angle en degre
        sphAnimMercure.setSpeed(Vector(0,0,vInitialeMercure)); // v initiale colineaire a Ox
        Mercure->setAnim(sphAnimMercure);
        Mercure->setTexture(textureid_Mercure);
        Mercure->getAnim().setPhi(sphAnimMercure.getPhi());
        Mercure->getAnim().setTheta(sphAnimMercure.getTheta());
        forms_list[number_of_forms] = Mercure;
        number_of_forms++;

        Sphere* Venus = new Sphere(rayonVenus,WHITE ,masseVenus);
        Animation sphAnimVenus;
        sphAnimVenus.setPos(Point(distanceSoleilVenus,0,0));
        sphAnimVenus.setPhi(10); // angle en degre
        sphAnimVenus.setTheta(0); // angle en degre
        sphAnimVenus.setSpeed(Vector(0,0,vInitialeVenus)); // v initiale colineaire a Ox
        Venus->setAnim(sphAnimVenus);
        Venus->setTexture(textureid_Venus);
        Venus->getAnim().setPhi(sphAnimVenus.getPhi());
        Venus->getAnim().setTheta(sphAnimVenus.getTheta());
        forms_list[number_of_forms] = Venus;
        number_of_forms++;

        Sphere* Terre = new Sphere(rayonTerre,WHITE ,masseTerre);
        Animation sphAnimTerre;
        sphAnimTerre.setPos(Point(distanceSoleilTerre,0,0));
        sphAnimTerre.setPhi(10); // angle en degre
        sphAnimTerre.setTheta(0); // angle en degre
        sphAnimTerre.setSpeed(Vector(0,0,vInitialeTerre)); // v initiale colineaire a Ox
        Terre->setAnim(sphAnimTerre);
        Terre->setTexture(textureid_Terre);
        Terre->getAnim().setPhi(sphAnimTerre.getPhi());
        Terre->getAnim().setTheta(sphAnimTerre.getTheta());
        forms_list[number_of_forms] = Terre;
        number_of_forms++;

        Sphere* Mars = new Sphere(rayonMars,WHITE ,masseMars);
        Animation sphAnimMars;
        sphAnimMars.setPos(Point(distanceSoleilMars,0,0));
        sphAnimMars .setPhi(10); // angle en degre
        sphAnimMars.setTheta(0); // angle en degre
        sphAnimMars.setSpeed(Vector(0,0,vInitialeMars)); // v initiale colineaire a Ox
        Mars->setAnim(sphAnimMars);
        Mars->setTexture(textureid_Mars);
        Mars->getAnim().setPhi(sphAnimMars.getPhi());
        Mars->getAnim().setTheta(sphAnimMars.getTheta());
        forms_list[number_of_forms] = Mars;
        number_of_forms++;

        Sphere* Jupiter = new Sphere(rayonJupiter,WHITE ,masseJupiter);
        Animation sphAnimJupiter;
        sphAnimJupiter.setPos(Point(distanceSoleilJupiter,0,0));
        sphAnimJupiter.setPhi(10); // angle en degre
        sphAnimJupiter.setTheta(0); // angle en degre
        sphAnimJupiter.setSpeed(Vector(0,0,vInitialeJupiter)); // v initiale colineaire a Ox
        Jupiter->setAnim(sphAnimJupiter);
        Jupiter->setTexture(textureid_Jupiter);
        Jupiter->getAnim().setPhi(sphAnimJupiter.getPhi());
        Jupiter->getAnim().setTheta(sphAnimJupiter.getTheta());
        forms_list[number_of_forms] = Jupiter;
        number_of_forms++;

        Sphere* Saturne = new Sphere(rayonSaturne,WHITE ,masseSaturne);
        Animation sphAnimSaturne;
        sphAnimSaturne.setPos(Point(distanceSoleilSaturne,0,0));
        sphAnimSaturne.setPhi(10); // angle en degre
        sphAnimSaturne.setTheta(0); // angle en degre
        sphAnimSaturne.setSpeed(Vector(0,0,vInitialeSaturne)); // v initiale colineaire a Ox
        Saturne->setAnim(sphAnimSaturne);
        Saturne->setTexture(textureid_Saturne);
        Saturne->getAnim().setPhi(sphAnimSaturne.getPhi());
        Saturne->getAnim().setTheta(sphAnimSaturne.getTheta());
        forms_list[number_of_forms] = Saturne;
        number_of_forms++;

        Sphere* Uranus = new Sphere(rayonUranus,WHITE ,masseUranus);
        Animation sphAnimUranus;
        sphAnimUranus.setPos(Point(2870658186e3,0,0));
        sphAnimUranus.setPhi(10); // angle en degre
        sphAnimUranus.setTheta(0); // angle en degre
        sphAnimUranus.setSpeed(Vector(0,0,vInitialeUranus)); // v initiale colineaire a Ox
        Uranus->setAnim(sphAnimUranus);
        Uranus->setTexture(textureid_Uranus);
        Uranus->getAnim().setPhi(sphAnimUranus.getPhi());
        Uranus->getAnim().setTheta(sphAnimUranus.getTheta());
        forms_list[number_of_forms] = Uranus;
        number_of_forms++;

        Sphere* Neptune = new Sphere(rayonNeptune,WHITE ,masseNeptune);
        Animation sphAnimNeptune;
        sphAnimNeptune.setPos(Point(distanceSoleilNeptune,0,0));
        sphAnimNeptune.setPhi(10); // angle en degre
        sphAnimNeptune.setTheta(0); // angle en degre
        sphAnimNeptune.setSpeed(Vector(0,0,vInitialeNeptune)); // v initiale colineaire a Ox
        Neptune->setAnim(sphAnimNeptune);
        Neptune->setTexture(textureid_Neptune);
        Neptune->getAnim().setPhi(sphAnimNeptune.getPhi());
        Neptune->getAnim().setTheta(sphAnimNeptune.getTheta());
        forms_list[number_of_forms] = Neptune;
        number_of_forms++;

        Sphere* Soleil = new Sphere(rayonSoleil, YELLOW ,masseSoleil);
        Animation sphAnimSoleil;
        sphAnimSoleil.setPos(Point(0,0,0));
        sphAnimSoleil.setPhi(1); // angle en degre
        sphAnimSoleil.setTheta(0); // angle en degre
        sphAnimSoleil.setSpeed(Vector(0,0,0)); // v initiale dans plan x0y
        Soleil->setAnim(sphAnimSoleil);
        Soleil->setTexture(textureid_Soleil);
        Soleil->getAnim().setPhi(sphAnimSoleil.getPhi());
        Soleil->getAnim().setTheta(sphAnimSoleil.getTheta());
        forms_list[number_of_forms] = Soleil;
        number_of_forms++;


        Sphere* Objet = new Sphere(rayonObjet, WHITE ,masseObjet);
        Animation sphObjet;
        sphObjet.setPos(Point(distanceSoleilTerre+(rayonTerre+rayonObjet)*coeff,0,0));
        sphObjet.setPhi(0); // angle en degre
        sphObjet.setTheta(0); // angle en degre
        sphObjet.setSpeed(Vector(10,0,0)); // v initiale dans plan x0y
        Objet->setAnim(sphObjet);
        Objet->setTexture(textureid_Meteorite);
        Objet->getAnim().setPhi(sphObjet.getPhi());
        Objet->getAnim().setTheta(sphObjet.getTheta());
        forms_list[number_of_forms] = Objet;
        number_of_forms++;
        int randPlanete  =rand()%8;
        // Get first "current time"
        previous_time_anim = previous_time_render = SDL_GetTicks();
        // While application is running
        while(!quit)
        {

            // Handle events on queue
            while(SDL_PollEvent(&event) != 0)
            {
                int x = 0, y = 0;
                SDL_Keycode key_pressed = event.key.keysym.sym;

                switch(event.type)
                {
                // User requests quit
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    // Handle key pressed with current mouse position
                    SDL_GetMouseState( &x, &y );

                    switch(key_pressed)
                    {
                    // Quit the program when 'q' or Escape keys are pressed
                    case SDLK_q:
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_v:

                        reset_prog();

                        sphAnimMercure.setPos(Point(distanceSoleilMercure,0,0));
                        sphAnimVenus.setPos(Point(distanceSoleilVenus,0,0));
                        sphAnimTerre.setPos(Point(distanceSoleilTerre,0,0));
                        sphAnimMars.setPos(Point(distanceSoleilMars,0,0));
                        sphAnimJupiter.setPos(Point(distanceSoleilJupiter,0,0));
                        sphAnimSaturne.setPos(Point(distanceSoleilSaturne,0,0));
                        sphAnimUranus.setPos(Point(distanceSoleilUranus,0,0));
                        sphAnimNeptune.setPos(Point(distanceSoleilNeptune,0,0));

                        randPlanete = rand()%8;
                        if(randPlanete == 0)
                            sphObjet.setPos(Point(distanceSoleilMercure+(rayonMercure+rayonObjet)*coeff,0,0));
                         if(randPlanete == 1)
                            sphObjet.setPos(Point(distanceSoleilVenus+(rayonVenus+rayonObjet)*coeff,0,0));
                         if(randPlanete == 2)
                            sphObjet.setPos(Point(distanceSoleilTerre+(rayonTerre+rayonObjet)*coeff,0,0));
                         if(randPlanete == 3)
                            sphObjet.setPos(Point(distanceSoleilMars+(rayonMars+rayonObjet)*coeff,0,0));
                         if(randPlanete == 4)
                            sphObjet.setPos(Point(distanceSoleilJupiter+(rayonJupiter+rayonObjet)*coeff,0,0));
                         if(randPlanete == 5)
                            sphObjet.setPos(Point(distanceSoleilSaturne+(rayonSaturne+rayonObjet)*coeff,0,0));
                         if(randPlanete == 6)
                            sphObjet.setPos(Point(distanceSoleilUranus+(rayonUranus+rayonObjet)*coeff,0,0));
                         if(randPlanete>=7)
                            sphObjet.setPos(Point(distanceSoleilNeptune+(rayonNeptune+rayonObjet)*coeff,0,0));

                        Mercure->setAnim(sphAnimMercure);
                        Venus->setAnim(sphAnimVenus);
                        Terre->setAnim(sphAnimTerre);
                        Mars->setAnim(sphAnimMars);
                        Jupiter->setAnim(sphAnimJupiter);
                        Saturne->setAnim(sphAnimSaturne);
                        Uranus->setAnim(sphAnimUranus);
                        Neptune->setAnim(sphAnimNeptune);
                        Objet -> setAnim(sphObjet);


                        Mercure->setRadius(rayonMercure);
                        Venus->setRadius(rayonVenus);
                        Terre->setRadius(rayonTerre);
                        Mars->setRadius(rayonMars);
                        Jupiter->setRadius(rayonJupiter);
                        Saturne->setRadius(rayonSaturne);
                        Uranus->setRadius(rayonUranus);
                        Neptune->setRadius(rayonNeptune);

                        Objet ->setRadius(rayonObjet);


                        Mercure->setMasse(masseMercure);
                        Venus->setMasse(masseVenus);
                        Terre->setMasse(masseTerre);
                        Mars->setMasse(masseMars);
                        Jupiter->setMasse(masseJupiter);
                        Saturne->setMasse(masseSaturne);
                        Uranus->setMasse(masseUranus);
                        Neptune->setMasse(masseNeptune);
                        Objet -> setMasse(masseObjet);

                        isMercureInv = false;
                        isVenusInv = false;
                        isTerreInv = false;
                        isMarsInv = false;
                        isJupiterInv = false;
                        isSaturneInv = false;
                        isUranusInv = false;
                        isNeptuneInv = false;

                        Coeff_Temps = 1000000;

                        camera_position.x = camDist;

                        camera_position.y = 0;
                        camera_position.z = camDist;
                        origine.x = 0;
                        origine.y = 0;
                        origine.z = 0;
                        rho = savedRho;
                        focus = 0;
                        sphObjet.setSpeed(Vector(rand()%10000,rand()%10000,rand()%10000)); // v initiale dans plan x0y


                        break;

                    case SDLK_s: // Haut
                        origine.y += 0.1;
                        break;
                    case SDLK_x: // Bas
                        origine.y -= 0.1;
                        break;
                    case SDLK_LEFT: // Rotation
                        rho += 1;
                        if (!vueHaut){savedRho = rho;}
                        break;
                    case SDLK_RIGHT: // Rotation
                        rho -= 1;
                        if (!vueHaut){savedRho = rho;}
                        break;
                    case SDLK_m: // Vue Haut
                        if (vueHaut) {
                            camera_position.x = camDist;
                            camera_position.y = 0;
                            camera_position.z = camDist;
                            origine.x = 0;
                            origine.y = 0;
                            origine.z = 0;
                            rho = savedRho;
                            focus = 0;
                        }
                        else {
                            camera_position.x = 0;
                            camera_position.y = 10;
                            camera_position.z = 0.001;
                            origine.x = 0;
                            origine.y = 0;
                            origine.z = 0;
                            rho = 0;
                            focus = 0;
                        }
                        vueHaut = !vueHaut;
                        break;

                    case SDLK_b:
                        isbPressed = true;
                        break;

                    case SDLK_a: // Mercure
                        if (isCtrlPressed){
                            if (focus1){
                                camera_position.x = camDist;
                                camera_position.y = 0;
                                camera_position.z = camDist;
                                origine.x = 0;
                                origine.y = 0;
                                origine.z = 0;
                                rho = savedRho;
                                focus = 0;
                                focus1 = !focus1;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                            else if(!isMercureInv){
                                focus = 1;
                                focus1 = !focus1;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                        }
                        else{
                            isAMercurePressed = true;
                        }

                        break;

                    case SDLK_z: // Venus
                        if (isCtrlPressed){
                            if (focus2){
                                camera_position.x = camDist;
                                camera_position.y = 0;
                                camera_position.z = camDist;
                                origine.x = 0;
                                origine.y = 0;
                                origine.z = 0;
                                rho = savedRho;
                                focus = 0;
                                focus2 = !focus2;
                                focus1 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                            else if(!isVenusInv){
                                focus = 2;
                                focus2 = !focus2;
                                focus1 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                        }
                        else{
                            isZVenusPressed = true;
                        }
                        break;

                    case SDLK_e: // Terre
                        if (isCtrlPressed){
                            if (focus3){
                                camera_position.x = camDist;
                                camera_position.y = 0;
                                camera_position.z = camDist;
                                origine.x = 0;
                                origine.y = 0;
                                origine.z = 0;
                                rho = savedRho;
                                focus = 0;
                                focus3 = !focus3;
                                focus1 = false;
                                focus2 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                            else if(!isTerreInv){
                                focus = 3;
                                focus3 = !focus3;
                                focus1 = false;
                                focus2 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                        }
                        else{
                            isETerrePressed = true;
                        }
                        break;

                    case SDLK_r: // Mars
                        if (isCtrlPressed){
                            if (focus4){
                                camera_position.x = camDist;
                                camera_position.y = 0;
                                camera_position.z = camDist;
                                origine.x = 0;
                                origine.y = 0;
                                origine.z = 0;
                                rho = savedRho;
                                focus = 0;
                                focus4 = !focus4;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                            else if(!isMarsInv){
                                focus = 4;
                                focus4 = !focus4;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                        }
                        else{
                            isRMarsPressed = true;
                        }
                        break;

                    case SDLK_t: // Jupiter
                        if (isCtrlPressed){
                            if (focus5){
                                camera_position.x = camDist;
                                camera_position.y = 0;
                                camera_position.z = camDist;
                                origine.x = 0;
                                origine.y = 0;
                                origine.z = 0;
                                rho = savedRho;
                                focus = 0;
                                focus5 = !focus5;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                            else if(!isJupiterInv){
                                focus = 5;
                                focus5 = !focus5;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                        }
                        else{
                            isTJupiterPressed = true;
                        }
                        break;

                    case SDLK_y: // Saturne
                        if (isCtrlPressed){
                            if (focus6){
                                camera_position.x = camDist;
                                camera_position.y = 0;
                                camera_position.z = camDist;
                                origine.x = 0;
                                origine.y = 0;
                                origine.z = 0;
                                rho = savedRho;
                                focus = 0;
                                focus6 = !focus6;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                            else if(!isSaturneInv){
                                focus = 6;
                                focus6 = !focus6;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus7 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                        }
                        else{
                            isYSaturnePressed = true;
                        }
                        break;

                    case SDLK_u: // Uranus
                        if (isCtrlPressed){
                            if (focus7){
                                camera_position.x = camDist;
                                camera_position.y = 0;
                                camera_position.z = camDist;
                                origine.x = 0;
                                origine.y = 0;
                                origine.z = 0;
                                rho = savedRho;
                                focus = 0;
                                focus7 = !focus7;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                            else if(!isUranusInv){
                                focus = 7;
                                focus7 = !focus7;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus8 = false;
                                focus9 = false;
                            }
                        }
                        else{
                            isUUranusPressed = true;
                        }
                        break;

                    case SDLK_i: // Neptune
                        if (isCtrlPressed){
                            if (focus8){
                                camera_position.x = camDist;
                                camera_position.y = 0;
                                camera_position.z = camDist;
                                origine.x = 0;
                                origine.y = 0;
                                origine.z = 0;
                                rho = savedRho;
                                focus = 0;
                                focus8 = !focus8;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus9 = false;
                            }
                            else if(!isNeptuneInv){
                                focus = 8;
                                focus8 = !focus8;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus9 = false;
                            }
                        }
                        else{
                            isINeptunePressed = true;
                        }
                        break;

                        case SDLK_p: // Meteorite
                        if (isCtrlPressed){
                            if (focus9){
                                camera_position.x = camDist;
                                camera_position.y = 0;
                                camera_position.z = camDist;
                                origine.x = 0;
                                origine.y = 0;
                                origine.z = 0;
                                rho = savedRho;
                                focus = 0;
                                focus9 = !focus9;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                            }
                            else if(!isMeteoriteInv){
                                focus = 9;
                                focus9 = !focus9;
                                focus1 = false;
                                focus2 = false;
                                focus3 = false;
                                focus4 = false;
                                focus5 = false;
                                focus6 = false;
                                focus7 = false;
                                focus8 = false;
                            }
                        }
                        else{
                            isPMeteoritePressed = true;
                        }
                        break;

                    case SDLK_LCTRL:
                        isCtrlPressed = true;
                        break;

                    case SDLK_o: // Soleil
                        isOSoleilPressed = true;
                        break;


                    case SDLK_UP:
                        if (isCtrlPressed){
                                if (camera_position.x - 0.1 < 0){
                                    //camera_position.x -= 0.1;
                                    camera_position.y -= 0.1;
                                    //camera_position.z -= 0.1;
                                }
                                else {
                                    camera_position.x -= 0.1;
                                    //camera_position.y -= 0.1;
                                    camera_position.z -= 0.1;
                                }
                        }
                        if (isAMercurePressed) {
                            masseMercure = masseMercure*10.8;
                            Mercure->setMasse(masseMercure);
                        }
                        if (isZVenusPressed) {
                            masseVenus = masseVenus*10.8;
                            Venus->setMasse(masseVenus);
                        }
                        if (isETerrePressed) {
                            masseTerre = masseTerre*10.8;
                            Terre->setMasse(masseTerre);
                        }
                        if (isRMarsPressed) {
                            masseMars = masseMars*10.8;
                            Mars->setMasse(masseMars);
                        }
                        if (isTJupiterPressed) {
                            masseJupiter = masseJupiter*10.8;
                            Jupiter->setMasse(masseJupiter);
                        }
                        if (isYSaturnePressed) {
                            masseSaturne = masseSaturne*10.8;
                            Saturne->setMasse(masseSaturne);
                        }
                        if (isUranusInv) {
                            masseUranus = masseUranus*10.8;
                            Uranus->setMasse(masseUranus);
                        }
                        if (isINeptunePressed) {
                            masseNeptune = masseNeptune*10.8;
                            Neptune->setMasse(masseNeptune);
                        }
                        if (isOSoleilPressed) {
                            masseSoleil = masseSoleil*1.8;
                        }
                        if(isbPressed) {
                            Coeff_Temps = Coeff_Temps*10;
                        }
                        break;

                    case SDLK_DOWN:
                        if (isCtrlPressed){
                                if (camera_position.x - 0.1 < 0){
                                    //camera_position.x -= 0.1;
                                    camera_position.y += 0.1;
                                    //camera_position.z -= 0.1;
                                }
                                else {
                                    camera_position.x += 0.1;
                                    //camera_position.y -= 0.1;
                                    camera_position.z += 0.1;
                                }
                        }
                        if (isAMercurePressed) {
                            masseMercure = masseMercure/10.8;
                            Mercure->setMasse(masseMercure);
                        }
                        if (isZVenusPressed) {
                            masseVenus = masseVenus/10.8;
                            Venus->setMasse(masseVenus);
                        }
                        if (isETerrePressed) {
                            masseTerre = masseTerre/10.8;
                            Terre->setMasse(masseTerre);
                        }
                        if (isRMarsPressed) {
                            masseMars = masseMars/10.8;
                            Mars->setMasse(masseMars);
                        }
                        if (isTerreInv) {
                            masseJupiter = masseJupiter/10.8;
                            Jupiter->setMasse(masseJupiter);
                        }
                        if (isYSaturnePressed) {
                            masseSaturne = masseSaturne/10.8;
                            Saturne->setMasse(masseSaturne);
                        }
                        if (isUranusInv) {
                            masseUranus = masseUranus/10.8;
                            Uranus->setMasse(masseUranus);
                        }
                        if (isINeptunePressed) {
                            masseNeptune = masseNeptune/10.8;
                            Neptune->setMasse(masseNeptune);
                        }
                        if (isOSoleilPressed) {
                            masseSoleil = masseSoleil/1.8;
                        }
                        if(isbPressed) {
                            Coeff_Temps = Coeff_Temps/10;
                        }
                        break;

                    default:
                        break;
                    }
                    break;

                case SDL_KEYUP:
                    switch(key_pressed)
                    {
                    case SDLK_LCTRL:
                        isCtrlPressed = false;
                        break;
                    case SDLK_a:  // Mercure
                        isAMercurePressed = false;
                        break;

                    case SDLK_z: // Venus
                        isZVenusPressed = false;
                        break;

                    case SDLK_e: // Terre
                        isETerrePressed = false;
                        break;

                    case SDLK_b: // Terre
                        isbPressed = false;
                        break;

                    case SDLK_r: // Mars
                        isRMarsPressed = false;
                        break;

                    case SDLK_t: // Jupiter
                        isTJupiterPressed = false;
                        break;

                    case SDLK_y: //  Saturne
                        isYSaturnePressed = false;
                        break;

                    case SDLK_u: // Uranus
                        isUUranusPressed = false;
                        break;

                    case SDLK_i: // Neptune
                        isINeptunePressed = false;
                        break;

                    case SDLK_o: // Soleil
                        isOSoleilPressed = false;
                        break;

                    case SDLK_p: // Soleil
                        isPMeteoritePressed = false;
                        break;

                    default:
                        break;
                    }
                default:
                    break;
                }
            }

            // Update the scene
            current_time = SDL_GetTicks(); // get the elapsed time from SDL initialization (ms)
            elapsed_time_anim = (current_time - previous_time_anim)*Coeff_Temps;
            elapsed_time_render = current_time - previous_time_render;

            if (elapsed_time_anim > ANIM_DELAY)
            {
                previous_time_anim = current_time;
                update(forms_list, 1e-3 * elapsed_time_anim); // International system units : seconds
            }

            if (elapsed_time_render > FRAME_DELAY)
            {
                previous_time_render = current_time;

                switch(focus){
                case 1:
                    camPosFocus.x = 3 * ptMActuel_Mercure.x / coeff;
                    camPosFocus.y = 0;
                    camPosFocus.z = 3 * ptMActuel_Mercure.z / coeff;
                    break;
                case 2:
                    camPosFocus.x = 2.07 * ptMActuel_Venus.x / coeff;
                    camPosFocus.y = 0;
                    camPosFocus.z = 2.07 * ptMActuel_Venus.z / coeff;
                    break;
                case 3:
                    camPosFocus.x = 1.77 * ptMActuel_Terre.x / coeff;
                    camPosFocus.y = 0;
                    camPosFocus.z = 1.77 * ptMActuel_Terre.z / coeff;
                    break;
                case 4:
                    camPosFocus.x = 1.51 * ptMActuel_Mars.x / coeff;
                    camPosFocus.y = 0;
                    camPosFocus.z = 1.51 * ptMActuel_Mars.z / coeff;
                    break;
                case 5:
                    camPosFocus.x = 1.5 * ptMActuel_Jupiter.x / coeff;
                    camPosFocus.y = 0;
                    camPosFocus.z = 1.5 * ptMActuel_Jupiter.z / coeff;
                    break;
                case 6:
                    camPosFocus.x = 1.25 * ptMActuel_Saturne.x / coeff;
                    camPosFocus.y = 0;
                    camPosFocus.z = 1.25 * ptMActuel_Saturne.z / coeff;
                    break;
                case 7:
                    camPosFocus.x = 1.06 * ptMActuel_Uranus.x / coeff;
                    camPosFocus.y = 0;
                    camPosFocus.z = 1.06 * ptMActuel_Uranus.z / coeff;
                    break;
                case 8:
                    camPosFocus.x = 1.04 * ptMActuel_Neptune.x / coeff;
                    camPosFocus.y = 0;
                    camPosFocus.z = 1.04 * ptMActuel_Neptune.z / coeff;
                    break;
                case 9:
                    camPosFocus.x = 2 * ptMActuel_Objet.x / coeff;
                    camPosFocus.y = 2 * ptMActuel_Objet.y / coeff;
                    camPosFocus.z = 2 * ptMActuel_Objet.z / coeff;
                    break;
                default:
                    break;
                }

                if((distance(Point(0,0,0),ptMActuel_Mercure)-rayonMercure)/(coeff)<=rayonSoleil )
                {
                        Mercure ->setRadius(0);
                        isMercureInv = true;
                }
                if((distance(Point(0,0,0),ptMActuel_Venus)-rayonVenus)/(coeff)<=rayonSoleil)
                {
                        Venus ->setRadius(0);
                        isVenusInv = true;
                }

                if((distance(Point(0,0,0),ptMActuel_Terre)-rayonTerre)/(coeff)<=rayonSoleil)
                {
                        Terre ->setRadius(0);
                        isTerreInv = true;
                }
                 if((distance(Point(0,0,0),ptMActuel_Mars)-rayonMars)/(coeff)<=rayonSoleil)
                {
                        Mars ->setRadius(0);
                        isMarsInv = true;
                }
                if((distance(Point(0,0,0),ptMActuel_Jupiter)-rayonJupiter)/(coeff)<=rayonSoleil)
                {
                        Jupiter ->setRadius(0);
                        isJupiterInv = true;
                }
                if((distance(Point(0,0,0),ptMActuel_Saturne)-rayonSaturne)/(coeff)<=rayonSoleil)
                {
                        Saturne ->setRadius(0);
                        isSaturneInv = true;
                }
                if((distance(Point(0,0,0),ptMActuel_Uranus)-rayonUranus)/(coeff)<=rayonSoleil)
                {
                        Uranus ->setRadius(0);
                        isUranusInv = true;
                }

                if((distance(Point(0,0,0),ptMActuel_Neptune)-rayonNeptune)/(coeff)<=rayonSoleil)
                {
                        Neptune ->setRadius(0);
                        isNeptuneInv = true;
                }



                Sphere* corps[] = { Mercure, Venus, Terre, Mars, Jupiter, Saturne, Uranus, Neptune,Soleil };
                for(int i = 0 ; i < 9; i++)
                {
                    if((distance(corps[i]->getAnim().getPos(),ptMActuel_Objet)-rayonObjet)/coeff  <= corps[i]->getRadius())
                    {
                        Objet->setRadius(0);
                    }
                }
                double dist1 =(distance(Point(0,0,0),ptMActuel_Objet)-rayonObjet)/(coeff);
                double dist2 = rayonSoleil;
                if(((distance(Point(0,0,0),ptMActuel_Objet)-rayonObjet)/(coeff))<=rayonSoleil  && ((distance(Point(0,0,0),ptMActuel_Objet)-rayonObjet)/(coeff)) > 0 )
                {
                        Objet->setRadius(0);
                        //isNeptuneInv = true;
                }

                render(forms_list, camera_position, origine, rho, phi, focus, camPosFocus, camViseur);

                // Update window screen
                SDL_GL_SwapWindow(gWindow);


            }
        }
    }


    // Free resources and close SDL
    close(&gWindow);

    return 0;
}

