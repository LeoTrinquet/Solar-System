#include <cmath>
#include <SDL2/SDL_opengl.h>
#include <GL/GLU.h>
#include "forms.h"
//#include "param.h"


double coeff = (149e9)/2;

void Form::update(double delta_t)
{
    // Nothing to do here, animation update is done in child class method
}


void Form::render()
{
    // Point of view for rendering
    // Common for all Forms
    Point org = anim.getPos();
    glTranslated(org.x/coeff, org.y/coeff, org.z/coeff);

    glRotated(anim.getTheta(), 1,0,0);
    glRotated(anim.getPhi(), 0,1,0);

    glColor3f(col.r, col.g, col.b);
}


Sphere::Sphere(double r, Color cl, double m)
{
    radius = r;
    col = cl;
    masse = m;
}

void Sphere::update(double delta_t)
{
    // Complete this part

    double G = 6.67428e-11; //(m3 kg-1 s-2)
    double massSol = masseSoleil ; //(kg)
    double massPlanete = this->getMasse(); // (kg)
    double rayon = this->getRadius();

    Point ptM=this->anim.getPos();


    if (rayon == rayonObjet)
    {
        // Complétez ici avec la force gravitationnelle due à chaque planète.
        Vector accel_total(0, 0, 0);
        Point positions[] = { ptMActuel_Mercure, ptMActuel_Venus, ptMActuel_Terre, ptMActuel_Mars, ptMActuel_Jupiter, ptMActuel_Saturne, ptMActuel_Uranus, ptMActuel_Neptune, Point(0, 0, 0) };
        double masses[] = { masseMercure, masseVenus, masseTerre, masseMars, masseJupiter, masseSaturne, masseUranus, masseNeptune, masseSoleil };
        Vector OM;
        for(int i = 0; i < 9; i++)
        {

            OM = Vector(positions[i], this->anim.getPos());
            double dist = sqrt(OM.x*OM.x + OM.y*OM.y + OM.z*OM.z);

            Vector direction;
            direction.x = -1*OM.x;
            direction.y = -1*OM.y;
            direction.z = -1*OM.z;

            direction.x /= dist;
            direction.y /= dist;
            direction.z /= dist;

            double force = (G * this->getMasse() * masses[i]) / (dist * dist);
            Vector accel;
            accel.x = direction.x * force/ this->getMasse();
            accel.y = direction.y * force/ this->getMasse();
            accel.z = direction.z * force/ this->getMasse();



            accel_total += accel;

        }
        Vector vit = this->anim.getSpeed();
        vit +=  delta_t * accel_total;
        this->anim.setSpeed(vit);
        OM.x += vit.x *delta_t;
        OM.y += vit.y *delta_t;
        OM.z += vit.z *delta_t;
        ptM = Point(OM.x, OM.y, OM.z);
        this->anim.setPos(ptM);
        ptMActuel_Objet = ptM;
        //this->anim.setPos(this->anim.getPos() + delta_t * vit);

    }
    else
    {
        Vector OM(Point(0,0,0),ptM);
        double dist = sqrt(OM.x*OM.x + OM.y*OM.y + OM.z*OM.z);
        if( dist != 0)
        {
            Vector vit;
            vit=this->anim.getSpeed();
            OM.x += vit.x *delta_t;
            OM.y += vit.y *delta_t;
            OM.z += vit.z *delta_t;

            Vector accel;
            ptM = Point(OM.x, OM.y, OM.z);
            this->anim.setPos(ptM);
            if (rayon == rayonMercure){
                ptMActuel_Mercure = ptM;
            }
            else if (rayon == rayonVenus){
                ptMActuel_Venus = ptM;
            }
            else if (rayon == rayonTerre){
                ptMActuel_Terre = ptM;
            }
            else if (rayon == rayonMars){
                ptMActuel_Mars = ptM;
            }
            else if (rayon == rayonJupiter){
                ptMActuel_Jupiter = ptM;
            }
            else if (rayon == rayonSaturne){
                ptMActuel_Saturne = ptM;
            }
            else if (rayon == rayonUranus){
                ptMActuel_Uranus = ptM;
            }
            else if (rayon == rayonNeptune){
                ptMActuel_Neptune = ptM;
            }


            // Calculez le vecteur direction
            Vector direction;
            direction.x = -1*OM.x;
            direction.y = -1*OM.y;
            direction.z = -1*OM.z;

            direction.x /= dist;
            direction.y /= dist;
            direction.z /= dist;

            float force = (G * massSol * massPlanete) / (dist * dist );
            accel.x = direction.x * force/ massPlanete ;
            accel.y = direction.y * force/ massPlanete ;
            accel.z = direction.z * force/ massPlanete ;

            vit +=  delta_t * accel;
            this->anim.setSpeed(vit);
        }

        double angle=this->anim.getPhi();
        if(angle>0){
            if(this->getMasse() > 1e25)
            {
                angle=angle+delta_t/100000;

            }
            else
            {
                angle=angle+delta_t/5000;
            }
        }
        while(angle>360){
                angle=angle-360;
        }
        this->anim.setPhi(angle);

        angle=this->anim.getTheta();
        if(angle>0){
            if(this->getMasse() > 1e25)
            {
                angle=angle+delta_t/100000;

            }
            else
            {
                angle=angle+delta_t/5000;
            }
        }
        while(angle>360){
                angle=angle-360;
        }
        this->anim.setTheta(angle);
    }



}

void Sphere::render()
{
    GLUquadric *quad;
    quad = gluNewQuadric();

    // Complete this part
    Form::render(); // Position the form and assign its color

    // Mise en route de la texture associee
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    gluQuadricTexture(quad,texture_id);
    gluQuadricNormals(quad,GLU_SMOOTH);

   // if (this->getRadius() == 0.08){
   //     gluSphere(quad, radius, 2, 500);
   // }
   // else{

    gluSphere(quad, radius, 20, 20);

    gluDeleteQuadric(quad);
    //this->anim.setPhi(this->anim.getPhi() + 10);
    // Ne plus appliquer la texture pour la suite
    glDisable(GL_TEXTURE_2D);
}

Point ptMActuel_Mercure = Point(0,0,0);
Point ptMActuel_Venus = Point(0,0,0);
Point ptMActuel_Terre = Point(0,0,0);
Point ptMActuel_Mars = Point(0,0,0);
Point ptMActuel_Jupiter = Point(0,0,0);
Point ptMActuel_Saturne = Point(0,0,0);
Point ptMActuel_Uranus = Point(0,0,0);
Point ptMActuel_Neptune = Point(0,0,0);
Point ptMActuel_Objet = Point(0,0,0);

Point accelActuel_Objet = Point(0,0,0);

//Rayon des planètes
float rayonMercure = 0.02439;
float rayonVenus = 0.06051;
float rayonTerre = 0.06371;
float rayonMars = 0.03389;
float rayonJupiter = 0.349555;
float rayonSaturne = 0.29116;
float rayonUranus = 0.12681;
float rayonNeptune = 0.12311;
float rayonSoleil = 0.4;
float rayonObjet = 0.018 ;


//Distance entre le soleil et les planètes
float distanceSoleilMercure  = 57910000e3;
float distanceSoleilVenus = 108208475e3;
float distanceSoleilTerre = 149598023e3;
float distanceSoleilMars = 227939200e3;
float distanceSoleilJupiter = 778340821e3;
float distanceSoleilSaturne = 1429400000e3;
float distanceSoleilUranus = 2870658186e3;
float distanceSoleilNeptune = 4498396441e3;

//Masse des planètes
float masseMercure = 3.3011e23;
float masseVenus = 4.8675e24;
float masseTerre = 5.9724e24;
float masseMars = 6.4171e23;
float masseJupiter = 1.8982e27;
float masseSaturne = 5.6834e26;
float masseUranus = 8.681e25;
float masseNeptune =1.02413e26;
float masseSoleil = 1.989e30;
float masseObjet =9.5e20;


//Vitesse initiale des planètes
float vInitialeMercure = 47870;
float vInitialeVenus = 35020;
float vInitialeTerre = 29780;
float vInitialeMars = 24070;
float vInitialeJupiter = 13070;
float vInitialeSaturne = 9690;
float vInitialeUranus = 6800;
float vInitialeNeptune = 5430;

void reset_prog(void)
{
    //Rayon des planètes
rayonMercure = 0.02439;
rayonVenus = 0.06051;
rayonTerre = 0.06371;
rayonMars = 0.03389;
rayonJupiter = 0.349555;
rayonSaturne = 0.29116;
rayonUranus = 0.12681;
rayonNeptune = 0.12311;
rayonSoleil = 0.4;
rayonObjet = 0.018 ;

//Distance entre le soleil et les planètes
distanceSoleilMercure  = 57910000e3;
distanceSoleilVenus = 108208475e3;
distanceSoleilTerre = 149598023e3;
distanceSoleilMars = 227939200e3;
distanceSoleilJupiter = 778340821e3;
distanceSoleilSaturne = 1429400000e3;
distanceSoleilUranus = 2870658186e3;
distanceSoleilNeptune = 4498396441e3;

//Masse des planètes
masseMercure = 3.3011e23;
masseVenus = 4.8675e24;
masseTerre = 5.9724e24;
masseMars = 6.4171e23;
masseJupiter = 1.8982e27;
masseSaturne = 5.6834e26;
masseUranus = 8.681e25;
masseNeptune =1.02413e26;
masseSoleil = 1.989e30;
masseObjet = 9.5e20;

//Vitesse initiale des planètes
vInitialeMercure = 47870;
vInitialeVenus = 35020;
vInitialeTerre = 29780;
vInitialeMars = 24070;
vInitialeJupiter = 13070;
vInitialeSaturne = 9690;
vInitialeUranus = 6800;
vInitialeNeptune = 5430;


}
