// Cut-Billiard map.
// Author: José Ramón Palacios Barreda.
// Email: palacios_barreda@hotmail.com

void Configure()
{
    SetFractalName("Cut-Billiard");
    SetCategory("Other");
    SetMinX(-1.57);
    SetMaxX(1.57);
    SetMinY(0.4);
    SetDefaultIter(50000);
}

// Otros
double pi = 3.14159265358979323846;
double radians(double x){return x*pi/180.0;}
double degrees(double x){return x*180.0/pi;}


// Parámetros
int prec = 13;               // Precisión decimal
int max_iter;                // Máximo número de iteraciones
double R = 1.0;              // Radio de la frontera
double W,w;                  // Corte de la frontera
double v0 = R;               // Módulo de la velocidad
double phi = 0.0;            // Ángulo de inyección
double vx, vy, vxp, vyp;     // Dirección de la velocidad
double omega = 3.0*pi/4.0;   // Posición de la entrada respecto al corte
double gammax = 3.0*pi/2.0;  // Posición de la salida
double delta = radians(10);  // Tamaño angular de las aperturas
double xPart,yPart;          // Coordenadas de la partícula
double b;                    // Ordenada al origen del corte
double alpha;                // Ángulo entre la normal de la frontera y el eje x
int a;                       // Apertura de salida

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
    double clx = -b/2.0 - sqrt_r(2.0*R*R - b*b)/2.0; // Limites del corte recto
    double cly = b/2.0 - sqrt_r(2.0*R*R - b*b)/2.0;
    double cux = -b/2.0 + sqrt_r(2.0*R*R - b*b)/2.0;
    double cuy = b/2.0 + sqrt_r(2.0*R*R - b*b)/2.0;
    double eps = 1.0/pow_r(10.0, prec-2); // Precisión

    // Coordenadas de Birkhoff
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
        // Adelantar hasta la colisión más próxima
        double m;
        if(vx == 0.0) m = _large;
        else m = vy/vx;
        double prx = 0.0, pry = 0.0, dr = 0.0;
        double pc1x, pc1y,pc2x,pc2y,dc1,dc2;
        bool col_corte = false;
        // Colisión con corte recto
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
            // Trayectoria con pendiente infinita
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

        // Casos de colisión
        // Puerta de salida
        if(res.colisiones==0) dc1 = _large;
        // Puntos detrás del corte (en la circ)
        if((pc1x <= (pc1y - b))&&(pc1y >= (pc1x + b))){ dc1 = _large; }
        if((pc2x <= (pc2y - b))&&(pc2y >= (pc2x + b))){ dc2 = _large; }
        // Paralelo a corte
        if(m==1.0) dr = _large;
        // Intersección entre circunferencia y corte recto
        if(dr==dc1) dr = _large;
        if(dr==dc2) dr = _large;
        // Punto de colisión
        if(dr < eps) dr = _large;
        if(dc1 < eps) dc1 = _large;
        if(dc2 < eps) dc2 = _large;
        // Puntos en el corte, fuera de la circ.
        if((prx > cux)||(prx < clx)) { dr = _large; }
        // Escoger colisión "ganadora"
        if((dr < dc1)&&(dr < dc2)){xPart = prx; yPart = pry; col_corte=true;}
        else if((dc1 < dc2)&&(dc1 < dr)){xPart = pc1x; yPart = pc1y;}
        else if((dc2 < dc1)&&(dc2 < dr)){xPart = pc2x; yPart = pc2y;}
        else
        {
            if(!silent)
                PrintString("Error en detección de colisión. simular()");
        }

        // Actualizar velocidades
        if(col_corte) // Colisión con corte recto
        {
            // Coordenadas de Birkhoff
            s = l1 + sqrt_r((xPart-cux)*(xPart-cux)+(yPart-cuy)*(yPart-cuy));
            s /= l; // Normalizar
            // Pasar al marco de referencia rotado
            vxp = vx*cos_r(omega) + vy*sin_r(omega);
            vyp = -vx*sin_r(omega) + vy*cos_r(omega);
            // Ángulo de colisión
            if(vyp > 0) theta = acos_r(vyp/sqrt_r(vxp*vxp+vyp*vyp));
            else theta = acos_r(-vyp/sqrt_r(vxp*vxp+vyp*vyp));
            p = cos_r(theta);
            // Invertir velocidad normal a la frontera
            vxp *= -1.0;
            // Regresar al marco original
            vx = vxp*cos_r(omega) - vyp*sin_r(omega);
            vy = vxp*sin_r(omega) + vyp*cos_r(omega);

            res.colisiones += 1;
        }
        else // Colisión con arco circular
        {
            if ((xPart >= 0)&&(yPart >= 0)){// Cuad I
                alpha = acos_r(xPart/R);
            }
            else if ((xPart <= 0)&&(yPart >= 0)){// Cuad II
                alpha = pi/2.0 + acos_r(yPart/R);
            }
            else if ((xPart <= 0)&&(yPart <= 0)){// Cuad III
                alpha = pi + acos_r(abs_r(xPart)/R);
            }
            else if ((xPart >= 0)&&(yPart <= 0)){// Cuad IV
                alpha = 2.0*pi - acos_r(xPart/R);
            }

            //Coordenadas de Birkhoff
            if((0 <= alpha )&&(alpha <= psi1)) s = R*alpha;
            else if ((psi2 <= alpha)&&(alpha <= 2*pi)) s=l1+l2+R*(alpha-psi2);
            else{
                if(!silent){
                    PrintString("Error en detección de colisión. simular()");
                }
            }
            s /= l; // Normalizar

            // Pasar al marco de referencia rotado
            vxp = vx*cos_r(alpha) + vy*sin_r(alpha);
            vyp = -vx*sin_r(alpha) + vy*cos_r(alpha);
            // Invertir velocidad normal a la frontera
            vxp *= -1;
            // Ángulo de colisión
            if(vyp > 0) theta = acos_r(vyp/sqrt_r(vxp*vxp+vyp*vyp));
            else theta = acos_r(-vyp/sqrt_r(vxp*vxp+vyp*vyp));
            p = cos_r(theta);
            // Regresar al marco original
            vx = vxp*cos_r(alpha) - vyp*sin_r(alpha);
            vy = vxp*sin_r(alpha) + vyp*cos_r(alpha);

            // Colisión con aperturas
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
    max_iter = maxIter;
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
