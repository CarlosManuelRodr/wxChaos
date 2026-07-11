// Cut-Billiard map.
// Author: José Ramón Palacios Barreda.
// Email: palacios_barreda@hotmail.com
// For more information and context check the paper:
// Suhan Ree, L. E. Reichl, Fractal analysis of chaotic classical scattering in a cut-circle billiard with two openings Phys. Rev. E 65, 055205(R), (2002).
// DOI: 10.1103/PhysRevE.65.055205

void Configure()
{
    SetFractalName("Cut-Billiard");
    SetCategory("Other");
    SetMinX(-1.57);
    SetMaxX(1.57);
    SetMinY(0.4);
    SetDefaultIter(50000);
}

// Miscellaneous
double pi = 3.14159265358979323846;
double radians(double x){return x*pi/180.0;}
double degrees(double x){return x*180.0/pi;}


// Parameters
int prec = 13;               // Decimal precision
int max_iter;                // Maximum number of iterations
double R = 1.0;              // Boundary radius
double W,w;                  // Boundary cut
double v0 = R;               // Velocity magnitude
double phi = 0.0;            // Injection angle
double vx, vy, vxp, vyp;     // Velocity direction
double omega = 3.0*pi/4.0;   // Entrance position relative to the cut
double gammax = 3.0*pi/2.0;  // Exit position
double delta = radians(10);  // Angular size of the openings
double xPart,yPart;          // Particle coordinates
double b;                    // Y-intercept of the cut
double alpha;                // Angle between the boundary normal and the x-axis
int a;                       // Exit opening

class simres
{
    int colisiones;
    int salida;
};


simres simular()
{
    bool log=false;
    bool isClosed=false;
    bool silent=true;
    simres res;

    res.colisiones = 0;
    double _large = pow_r(10,99);
    double clx = -b/2.0 - sqrt_r(2.0*R*R - b*b)/2.0; // Straight cut limits
    double cly = b/2.0 - sqrt_r(2.0*R*R - b*b)/2.0;
    double cux = -b/2.0 + sqrt_r(2.0*R*R - b*b)/2.0;
    double cuy = b/2.0 + sqrt_r(2.0*R*R - b*b)/2.0;
    double eps = 1.0/pow_r(10.0, prec-2); // Precision

    // Birkhoff coordinates
    double psi1,psi2;
    double s = 0, p, theta = 0;
    if(cux > 0){
        psi1 = acos_r(cux/R);
        psi2 = acos_r(abs_r(clx)/R) + pi;
    }
    else{
        psi1 = pi/2.0 + acos_r(cuy/R);
        psi2 = pi - acos_r(clx/R);
    }

    double l1 = R*psi1;
    double l2 = sqrt_r((clx-cux)*(clx-cux)+(cly-cuy)*(cly-cuy));
    double l = l1+l2+R*(2.0*pi-psi2);
    a = 0;

    while(true)
    {
        if(abs_r(vx) < eps) vx = 0.0;
        if(abs_r(vy) < eps) vy = 0.0;
        // Advance to the nearest collision
        double m;
        if(vx == 0.0) m = _large;
        else m = vy/vx;
        double prx = 0.0, pry = 0.0, dr = 0.0;
        double pc1x, pc1y,pc2x,pc2y,dc1,dc2;
        bool col_corte = false;
        // Collision with the straight cut
        if (m != _large){
            prx = (yPart - m*xPart - b)/(1.0-m);
            pry = prx + b;
            dr = sqrt_r((prx-xPart)*(prx-xPart)+(pry-yPart)*(pry-yPart));
            pc1x = (1.0/(2.0*(1.0+m*m)))*(-2.0*m*(yPart-m*xPart) + sqrt_r(4.0*m*m*(yPart - m*xPart)*(yPart - m*xPart) - 4*(1+m*m)*(m*m*xPart*xPart + yPart*yPart - 2*m*xPart*yPart - R*R)));
            pc1y = m*(pc1x-xPart) + yPart;
            dc1 = sqrt_r((pc1x-xPart)*(pc1x-xPart)+(pc1y-yPart)*(pc1y-yPart));
            pc2x = (1.0/(2.0*(1.0+m*m)))*(-2.0*m*(yPart-m*xPart) - sqrt_r(4.0*m*m*(yPart - m*xPart)*(yPart - m*xPart) - 4*(1+m*m)*(m*m*xPart*xPart + yPart*yPart - 2*m*xPart*yPart - R*R)));
            pc2y = m*(pc2x-xPart) + yPart;
            dc2 = sqrt_r((pc2x-xPart)*(pc2x-xPart)+(pc2y-yPart)*(pc2y-yPart));
        }
        else
        {
            // Vertical trajectory
            int z = 0;
            if(vy < 0) z++;
            prx = xPart;
            pry = xPart + b;
            dr = sqrt_r((prx-xPart)*(prx-xPart)+(pry-yPart)*(pry-yPart));
            pc1x = xPart;
            pc1y = pow_r(-1.0,z+1.0)*sqrt_r(R*R - xPart*xPart);
            dc1 = sqrt_r((pc1x-xPart)*(pc1x-xPart)+(pc1y-yPart)*(pc1y-yPart));
            pc2x = xPart;
            pc2y = pow_r(-1.0,z)*sqrt_r(R*R - xPart*xPart);
            dc2 = sqrt_r((pc2x-xPart)*(pc2x-xPart)+(pc2y-yPart)*(pc2y-yPart));
        }

        // Collision cases
        // Exit opening
        if(res.colisiones==0) dc1 = _large;
        // Points behind the cut (on the circle)
        if((pc1x <= (pc1y - b))&&(pc1y >= (pc1x + b))){ dc1 = _large; }
        if((pc2x <= (pc2y - b))&&(pc2y >= (pc2x + b))){ dc2 = _large; }
        // Parallel to the cut
        if(m==1.0) dr = _large;
        // Intersection between the circle and the straight cut
        if(dr==dc1) dr = _large;
        if(dr==dc2) dr = _large;
        // Collision point
        if(dr < eps) dr = _large;
        if(dc1 < eps) dc1 = _large;
        if(dc2 < eps) dc2 = _large;
        // Points on the cut but outside the circle
        if((prx > cux)||(prx < clx)) { dr = _large; }
        // Select the winning collision
        if((dr < dc1)&&(dr < dc2)){xPart = prx; yPart = pry; col_corte=true;}
        else if((dc1 < dc2)&&(dc1 < dr)){xPart = pc1x; yPart = pc1y;}
        else if((dc2 < dc1)&&(dc2 < dr)){xPart = pc2x; yPart = pc2y;}
        else
        {
            if(!silent)
                PrintString("Collision detection error. simular()");
        }

        // Update velocities
        if(col_corte) // Collision with the straight cut
        {
            // Birkhoff coordinates
            s = l1 + sqrt_r((xPart-cux)*(xPart-cux)+(yPart-cuy)*(yPart-cuy));
            s /= l; // Normalize
            // Transform to the rotated reference frame
            vxp = vx*cos_r(omega) + vy*sin_r(omega);
            vyp = -vx*sin_r(omega) + vy*cos_r(omega);
            // Collision angle
            if(vyp > 0) theta = acos_r(vyp/sqrt_r(vxp*vxp+vyp*vyp));
            else theta = acos_r(-vyp/sqrt_r(vxp*vxp+vyp*vyp));
            p = cos_r(theta);
            // Reverse the velocity component normal to the boundary
            vxp *= -1.0;
            // Transform back to the original reference frame
            vx = vxp*cos_r(omega) - vyp*sin_r(omega);
            vy = vxp*sin_r(omega) + vyp*cos_r(omega);

            res.colisiones += 1;
        }
        else // Collision with the circular arc
        {
            if ((xPart >= 0)&&(yPart >= 0)){// Quadrant I
                alpha = acos_r(xPart/R);
            }
            else if ((xPart <= 0)&&(yPart >= 0)){// Quadrant II
                alpha = pi/2.0 + acos_r(yPart/R);
            }
            else if ((xPart <= 0)&&(yPart <= 0)){// Quadrant III
                alpha = pi + acos_r(abs_r(xPart)/R);
            }
            else if ((xPart >= 0)&&(yPart <= 0)){// Quadrant IV
                alpha = 2.0*pi - acos_r(xPart/R);
            }

            // Birkhoff coordinates
            if((0 <= alpha )&&(alpha <= psi1)) s = R*alpha;
            else if ((psi2 <= alpha)&&(alpha <= 2*pi)) s=l1+l2+R*(alpha-psi2);
            else{
                if(!silent){
                    PrintString("Collision detection error. simular()");
                }
            }
            s /= l; // Normalize

            // Transform to the rotated reference frame
            vxp = vx*cos_r(alpha) + vy*sin_r(alpha);
            vyp = -vx*sin_r(alpha) + vy*cos_r(alpha);
            // Reverse the velocity component normal to the boundary
            vxp *= -1;
            // Collision angle
            if(vyp > 0) theta = acos_r(vyp/sqrt_r(vxp*vxp+vyp*vyp));
            else theta = acos_r(-vyp/sqrt_r(vxp*vxp+vyp*vyp));
            p = cos_r(theta);
            // Transform back to the original reference frame
            vx = vxp*cos_r(alpha) - vyp*sin_r(alpha);
            vy = vxp*sin_r(alpha) + vyp*cos_r(alpha);

            // Collision with the openings
            if(isClosed==false){
                if ((alpha < (gammax + delta/2.0))&&(alpha > (gammax - delta/2.0))) a=2.0;
                else if ((alpha <= delta/2.0)||((2.0*pi - delta/2.0) <= alpha)) a=1;
            }

            res.colisiones += 1;
        }

        if (a != 0) break;
        if(res.colisiones >= max_iter) break;
    }

    res.salida = a;
    return res;
}

void Render()
{
    max_iter = maxIterations;
    simres result;

    // W
    for(int y=ho; y<hf; y++)
    {
        W = maxY - y*yFactor;
        if(W > 2) continue;

        // Phi
        for(int x=wo; x<wf; x++)
        {
            phi = minX + x*xFactor;
            xPart = R*cos_r(delta);
            yPart = 0;
            w = W/R;
            vx = -v0*cos_r(phi);
            vy = v0*sin_r(phi);
            b = (W-R)/cos_r(omega - pi/2);

            result = simular();
            if(a == 1)
                SetPoint(x, y, false, result.colisiones);
            else
                SetPoint(x, y, true, result.colisiones);
        }
    }
}