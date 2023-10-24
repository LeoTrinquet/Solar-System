#ifndef FORMS_H_INCLUDED
#define FORMS_H_INCLUDED
#include <cmath>

#include "geometry.h"
#include "animation.h"


class Color
{
public:
    float r, g, b;
    Color(float rr = 1.0f, float gg = 1.0f, float bb = 1.0f) {r=rr; g=gg; b=bb;}
};

// Constant Colors
const Color RED(1.0f, 0.0f, 0.0f);
const Color BLUE(0.0f, 0.0f, 1.0f);
const Color GREEN(0.0f, 1.0f, 0.0f);
const Color YELLOW(1.0f, 1.0f, 0.0f);
const Color WHITE(1.0f, 1.0f, 1.0f);
const Color ORANGE(1.0f, 0.65f, 0.0f);


// Generic class to render and animate an object
class Form
{
protected:
    Color col;
    Animation anim;
public:
    Animation& getAnim() {return anim;}
    void setAnim(Animation ani) {anim = ani;}
    // This method should update the anim object with the corresponding physical model
    // It has to be done in each inherited class, otherwise all forms will have the same movements !
    // Virtual method for dynamic function call
    // Pure virtual to ensure all objects have their physics implemented
    virtual void update(double delta_t) = 0;
    // Virtual method : Form is a generic type, only setting color and reference position
    virtual void render();
};


// A particular Form
class Sphere : public Form
{
private:
    // The sphere center is aligned with the coordinate system origin
    // => no center required here, information is stored in the anim object
    double radius;
    double masse;
    // Texture
    GLuint texture_id;
public:
    Sphere(double r = 1.0, Color cl = Color(), double m = 1);
    double getRadius() const {return radius;}
    void setRadius(double r) {radius = r;}
    void setTexture(GLuint textureid) {texture_id = textureid;}
    void update(double delta_t);
    void setMasse(double m) {masse =m;}
    double getMasse() const {return masse;}
    void render();
};

Vector Force_Gravitationelle(double m1,double m2,Point Pt1, Point Pt2);
#endif // FORMS_H_INCLUDED

extern Point ptMActuel_Mercure;
extern Point ptMActuel_Venus;
extern Point ptMActuel_Terre;
extern Point ptMActuel_Mars;
extern Point ptMActuel_Jupiter;
extern Point ptMActuel_Saturne;
extern Point ptMActuel_Uranus;
extern Point ptMActuel_Neptune;
extern Point ptMActuel_Objet;


//Rayon des planètes
extern float rayonMercure;
extern float rayonVenus;
extern float rayonTerre;
extern float rayonMars;
extern float rayonJupiter;
extern float rayonSaturne;
extern float rayonUranus;
extern float rayonNeptune;
extern float rayonSoleil;
extern float rayonObjet;

//Distance entre le soleil et les planètes
extern float distanceSoleilMercure;
extern float distanceSoleilVenus;
extern float distanceSoleilTerre;
extern float distanceSoleilMars;
extern float distanceSoleilJupiter;
extern float distanceSoleilSaturne;
extern float distanceSoleilUranus;
extern float distanceSoleilNeptune;

//Masse des planètes
extern float masseMercure;
extern float masseVenus;
extern float masseTerre;
extern float masseMars;
extern float masseJupiter;
extern float masseSaturne;
extern float masseUranus;
extern float masseNeptune;
extern float masseSoleil;
extern float masseObjet;

//Vitesse initiale des planètes
extern float vInitialeMercure;
extern float vInitialeVenus;
extern float vInitialeTerre;
extern float vInitialeMars;
extern float vInitialeJupiter;
extern float vInitialeSaturne;
extern float vInitialeUranus;
extern float vInitialeNeptune;

void reset_prog(void);
