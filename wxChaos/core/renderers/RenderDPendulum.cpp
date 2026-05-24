#include "RenderDPendulum.h"

RenderDPendulum::RenderDPendulum()
{

}
void RenderDPendulum::Render()
{
    //Crea fractal
    double th1, th2, vth1, vth2;
    double ecMov1, ecMov2;
    bool insideSet;
    unsigned n;
    double den;
    double minTh1 = minX;
    double maxTh2 = maxY;
    double th1Factor = xFactor;
    double th2Factor = yFactor;
    double th1_init, th2_init;

    double part0, part1, part2, part3, part4, part5, part6;
    part0 = _m1+_m2;
    part1 = -_g*(2*_m1+_m2);
    part2 = 2*_m2;
    part3 = _m2*_g;
    part4 = _g*(_m1+_m2);
    part5 = _l*2*_m1+_m2;
    part6 = _l*_m2;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        if(_rungeKutta)
        {
            double k11, k12, k13, k14;
            double k21, k22, k23, k24;

            for(int y=ho; y<hf; y++)
            {
                for(int x=wo; x<wf; x++)
                {
                    th1 = minTh1 + x*th1Factor;
                    th2 = maxTh2 - y*th2Factor;
                    vth1 = 0;
                    vth2 = 0;
                    th1_init = th1;
                    th2_init = th2;

                    insideSet = true;
                    for(n=0; n<maxIter; n++)
                    {
                        den = part5 - part6*cos(2*(th1-th2));
                        k11 = part1*sin(th1)-part3*sin(th1-2*th2)-part2*sin(th1-th2)*(_l*(pow(vth2,2)+pow(vth1,2)*cos(th1-th2)));
                        k11 /= den;
                        k11 *= _dt;
                        k21 = 2*sin(th1-th2)*(part4*cos(th1)+_l*(pow(vth2,2)*_m2*cos(th1-th2)+pow(vth1,2)*part0));
                        k21 /= den;
                        k21 *= _dt;

                        den = part5 - part6*cos(2*(th1 + k11/2.0 - (th2 + k21/2.0)));
                        k12 = part1*sin(th1 + k11/2.0)-part3*sin(th1 + k11/2.0-2*(th2 + k21/2.0))-part2*sin(th1 + k11/2.0-(th2 + k21/2.0))*(_l*(pow(vth2,2)+pow(vth1,2)*cos(th1 + k11/2.0-(th2 + k21/2.0))));
                        k12 /= den;
                        k12 *= _dt;
                        k22 = 2*sin((th1 + k11/2.0)-(th2 + k21/2.0))*(part4*cos((th1 + k11/2.0))+_l*(pow(vth2,2)*_m2*cos((th1 + k11/2.0)-(th2 + k21/2.0))+pow(vth1,2)*part0));
                        k22 /= den;
                        k22 *= _dt;

                        den = part5 - part6*cos(2*(th1 + k12/2.0 - (th2 + k22/2.0)));
                        k13 = part1*sin(th1 + k12/2.0)-part3*sin(th1 + k12/2.0-2*(th2 + k22/2.0))-part2*sin(th1 + k12/2.0-(th2 + k22/2.0))*(_l*(pow(vth2,2)+pow(vth1,2)*cos(th1 + k12/2.0-(th2 + k22/2.0))));
                        k13 /= den;
                        k13 *= _dt;
                        k23 = 2*sin((th1 + k12/2.0)-(th2 + k22/2.0))*(part4*cos((th1 + k12/2.0))+_l*(pow(vth2,2)*_m2*cos((th1 + k12/2.0)-(th2 + k22/2.0))+pow(vth1,2)*part0));
                        k23 /= den;
                        k23 *= _dt;

                        den = part5 - part6*cos(2*(th1 + k13/2.0 - (th2 + k23/2.0)));
                        k14 = part1*sin(th1 + k13/2.0)-part3*sin(th1 + k13/2.0-2*(th2 + k23/2.0))-part2*sin(th1 + k13/2.0-(th2 + k23/2.0))*(_l*(pow(vth2,2)+pow(vth1,2)*cos(th1 + k13/2.0-(th2 + k23/2.0))));
                        k14 /= den;
                        k14 *= _dt;
                        k24 = 2*sin((th1 + k13/2.0)-(th2 + k23/2.0))*(part4*cos((th1 + k13/2.0))+_l*(pow(vth2,2)*_m2*cos((th1 + k13/2.0)-(th2 + k23/2.0))+pow(vth1,2)*part0));
                        k24 /= den;
                        k24 *= _dt;

                        vth1 += (k11/6) + (k12/3) + (k13/3) + (k14/6);
                        vth2 += (k21/6) + (k22/3) + (k23/3) + (k24/6);
                        th1 += _dt*vth1;
                        th2 += _dt*vth2;

                        if(_referenced)
                        {
                            if((_th1Bailout && (abs(th1-th1_init) > _th1NumBailout)) || (_th2Bailout && (abs(th2-th2_init) > _th2NumBailout)))
                            {
                                insideSet = false;
                                break;
                            }
                        }
                        else
                        {
                            if((_th1Bailout && (abs(th1) > _th1NumBailout)) || (_th2Bailout && (abs(th2) > _th2NumBailout)))
                            {
                                insideSet = false;
                                break;
                            }
                        }
                    }
                    if(insideSet)
                        setMap[x][y] = true;
                    else
                        colorMap[x][y] = n;
                }
            }
        }
        else
        {
            double cosTh1MinTh2, sinTh1MinTh2;
            for(y=ho; y<hf && threadRunning; y++)
            {
                for(x=wo; x<wf; x++)
                {
                    th1 = minTh1 + x*th1Factor;
                    th2 = maxTh2 - y*th2Factor;
                    vth1 = 0;
                    vth2 = 0;
                    th1_init = th1;
                    th2_init = th2;

                    insideSet = true;
                    for(n=0; n<maxIter; n++)
                    {
                        cosTh1MinTh2 = cos(th1-th2);
                        sinTh1MinTh2 = sin(th1-th2);

                        den = part5 - part6*cos(2*(th1-th2));
                        ecMov1 = part1*sin(th1)-part3*sin(th1-2*th2)-part2*sinTh1MinTh2*(_l*(vth2*vth2 + vth1*vth1*cosTh1MinTh2));
                        ecMov1 /= den;
                        ecMov2 = 2*sinTh1MinTh2*(part4*cos(th1)+_l*(vth2*vth2*_m2*cosTh1MinTh2 + vth1*vth1*(part0)));
                        ecMov2 /= den;
                        vth1 += _dt*ecMov1;
                        vth2 += _dt*ecMov2;
                        th1 += _dt*vth1;
                        th2 += _dt*vth2;

                        if(_referenced)
                        {
                            if((_th1Bailout && (abs(th1-th1_init) > _th1NumBailout)) || (_th2Bailout && (abs(th2-th2_init) > _th2NumBailout)))
                            {
                                insideSet = false;
                                break;
                            }
                        }
                        else
                        {
                            if((_th1Bailout && (abs(th1) > _th1NumBailout)) || (_th2Bailout && (abs(th2) > _th2NumBailout)))
                            {
                                insideSet = false;
                                break;
                            }
                        }
                    }

                    if(!threadRunning) break;

                    if(insideSet)
                        setMap[x][y] = true;

                    colorMap[x][y] = n;
                }
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            for(x=wo; x<wf; x++)
            {
                th1 = minTh1 + x*th1Factor;
                th2 = maxTh2 - y*th2Factor;
                vth1 = 0;
                vth2 = 0;
                th1_init = th1;
                th2_init = th2;

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    den = part5 - part6*cos(2*(th1-th2));
                    ecMov1 = part1*sin(th1)-part3*sin(th1-2*th2)-part2*sin(th1-th2)*(_l*(pow(vth2,2)+pow(vth1,2)*cos(th1-th2)));
                    ecMov1 /= den;
                    ecMov2 = 2*sin(th1-th2)*(part4*cos(th1)+_l*(pow(vth2,2)*_m2*cos(th1-th2)+pow(vth1,2)*(part0)));
                    ecMov2 /= den;
                    vth1 += _dt*ecMov1;
                    vth2 += _dt*ecMov2;
                    th1 += _dt*vth1;
                    th2 += _dt*vth2;

                    if(_referenced)
                    {
                        if((_th1Bailout && (abs(th1-th1_init) > _th1NumBailout)) || (_th2Bailout && (abs(th2-th2_init) > _th2NumBailout)))
                        {
                            insideSet = false;
                            break;
                        }
                    }
                    else
                    {
                        if((_th1Bailout && (abs(th1) > _th1NumBailout)) || (_th2Bailout && (abs(th2) > _th2NumBailout)))
                        {
                            insideSet = false;
                            break;
                        }
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(th1 > 0 && th2 > 0)
                    colorMap[x][y] = color1;
                else if(th1 <= 0 && th2 > 0)
                    colorMap[x][y] = color2;
                else if(th1 <= 0 && th2 < 0)
                    colorMap[x][y] = color3;
                else
                    colorMap[x][y] = color4;
            }
        }
    }
}
void RenderDPendulum::SetParams(bool th1B, bool th2B, double th1NB, double th2NB,
                    double dt, double m1, double m2, double l, double g, bool ref, bool rungeKutta)
{
    _th1Bailout = th1B;
    _th2Bailout = th2B;
    _th1NumBailout = th1NB;
    _th2NumBailout = th2NB;
    _dt = dt;
    _m1 = m1;
    _m2 = m2;
    _l = l;
    _g = g;
    _referenced = ref;
    _rungeKutta = rungeKutta;
}

