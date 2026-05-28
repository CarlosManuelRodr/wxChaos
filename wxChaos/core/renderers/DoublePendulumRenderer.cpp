#include "DoublePendulumRenderer.h"

DoublePendulumRenderer::DoublePendulumRenderer()
{
    _th1Bailout = false;
    _th2Bailout = false;
    _th1NumBailout = 0;
    _th2NumBailout = 0;
    _m1 = 0;
    _m2 = 0;
    _l = 0;
    _g = 0;
    _dt = 0;
    _referenced = false;
    _rungeKutta = false;
}

void DoublePendulumRenderer::EscapeTimeRender()
{
    //Crea fractal
    double th1, th2, vth1, vth2;
    bool insideSet;
    unsigned n;
    double den;
    double minTh1 = _minX;
    double maxTh2 = _maxY;
    double th1Factor = _xFactor;
    double th2Factor = _yFactor;
    double th1_init, th2_init;

    double part0, part1, part2, part3, part4, part5, part6;
    part0 = _m1+_m2;
    part1 = -_g*(2*_m1+_m2);
    part2 = 2*_m2;
    part3 = _m2*_g;
    part4 = _g*(_m1+_m2);
    part5 = _l*2*_m1+_m2;
    part6 = _l*_m2;

    if (_rungeKutta)
    {
        double k11, k12, k13, k14;
        double k21, k22, k23, k24;

        for (int y=_heightOrigin; y<_heightFinal; y++)
        {
            for (int x=_widthOrigin; x<_widthFinal; x++)
            {
                th1 = minTh1 + x*th1Factor;
                th2 = maxTh2 - y*th2Factor;
                vth1 = 0;
                vth2 = 0;
                th1_init = th1;
                th2_init = th2;

                insideSet = true;
                for (n=0; n<_maxIter; n++)
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

                    if (_referenced)
                    {
                        if ((_th1Bailout && (abs(th1-th1_init) > _th1NumBailout)) || (_th2Bailout && (abs(th2-th2_init) > _th2NumBailout)))
                        {
                            insideSet = false;
                            break;
                        }
                    }
                    else
                    {
                        if ((_th1Bailout && (abs(th1) > _th1NumBailout)) || (_th2Bailout && (abs(th2) > _th2NumBailout)))
                        {
                            insideSet = false;
                            break;
                        }
                    }
                }
                if (insideSet)
                    _setMap[x][y] = true;
                else
                    _colorMap[x][y] = n;
            }
        }
    }
    else
    {
        double movEq2;
        double movEq1;
        double cosTh1MinTh2, sinTh1MinTh2;
        for (_y=_heightOrigin; _y<_heightFinal && _threadRunning; _y++)
        {
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                th1 = minTh1 + _x*th1Factor;
                th2 = maxTh2 - _y*th2Factor;
                vth1 = 0;
                vth2 = 0;
                th1_init = th1;
                th2_init = th2;

                insideSet = true;
                for (n=0; n<_maxIter; n++)
                {
                    cosTh1MinTh2 = cos(th1-th2);
                    sinTh1MinTh2 = sin(th1-th2);

                    den = part5 - part6*cos(2*(th1-th2));
                    movEq1 = part1*sin(th1)-part3*sin(th1-2*th2)-part2*sinTh1MinTh2*(_l*(vth2*vth2 + vth1*vth1*cosTh1MinTh2));
                    movEq1 /= den;
                    movEq2 = 2*sinTh1MinTh2*(part4*cos(th1)+_l*(vth2*vth2*_m2*cosTh1MinTh2 + vth1*vth1*(part0)));
                    movEq2 /= den;
                    vth1 += _dt*movEq1;
                    vth2 += _dt*movEq2;
                    th1 += _dt*vth1;
                    th2 += _dt*vth2;

                    if (_referenced)
                    {
                        if ((_th1Bailout && (abs(th1-th1_init) > _th1NumBailout)) || (_th2Bailout && (abs(th2-th2_init) > _th2NumBailout)))
                        {
                            insideSet = false;
                            break;
                        }
                    }
                    else
                    {
                        if ((_th1Bailout && (abs(th1) > _th1NumBailout)) || (_th2Bailout && (abs(th2) > _th2NumBailout)))
                        {
                            insideSet = false;
                            break;
                        }
                    }
                }

                if (!_threadRunning)
                    break;

                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = n;
            }
        }
    }

}

void DoublePendulumRenderer::EscapeAngleRender()
{
    //Crea fractal
    const double minTh1 = _minX;
    const double maxTh2 = _maxY;
    const double th1Factor = _xFactor;
    const double th2Factor = _yFactor;

    const double part0 = _m1 + _m2;
    const double part1 = -_g * (2 * _m1 + _m2);
    const double part2 = 2 * _m2;
    const double part3 = _m2 * _g;
    const double part4 = _g * (_m1 + _m2);
    const double part5 = _l * 2 * _m1 + _m2;
    const double part6 = _l * _m2;

    const int color1 = 1;
    const int color2 = 0.25 * _myOpt.paletteSize;
    const int color3 = 0.50 * _myOpt.paletteSize;
    const int color4 = 0.75 * _myOpt.paletteSize;

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double th1 = minTh1 + _x * th1Factor;
            double th2 = maxTh2 - _y * th2Factor;
            double vth1 = 0;
            double vth2 = 0;
            const double th1_init = th1;
            const double th2_init = th2;

            bool insideSet = true;
            for (unsigned n = 0; n<_maxIter; n++)
            {
                const double den = part5 - part6 * cos(2 * (th1 - th2));
                double movEq1 = part1 * sin(th1) - part3 * sin(th1 - 2 * th2) - part2 * sin(th1 - th2) * (_l * (pow(vth2, 2) +
                    pow(vth1, 2) * cos(th1 - th2)));
                movEq1 /= den;
                double movEq2 = 2 * sin(th1 - th2) * (part4 * cos(th1) + _l * (pow(vth2, 2) * _m2 * cos(th1 - th2) + pow(vth1, 2) *
                    (part0)));
                movEq2 /= den;
                vth1 += _dt*movEq1;
                vth2 += _dt*movEq2;
                th1 += _dt*vth1;
                th2 += _dt*vth2;

                if (_referenced)
                {
                    if ((_th1Bailout && (abs(th1-th1_init) > _th1NumBailout)) || (_th2Bailout && (abs(th2-th2_init) > _th2NumBailout)))
                    {
                        insideSet = false;
                        break;
                    }
                }
                else
                {
                    if ((_th1Bailout && (abs(th1) > _th1NumBailout)) || (_th2Bailout && (abs(th2) > _th2NumBailout)))
                    {
                        insideSet = false;
                        break;
                    }
                }
            }
            if (insideSet)
                _setMap[_x][_y] = true;

            if (th1 > 0 && th2 > 0)
                _colorMap[_x][_y] = color1;
            else if (th1 <= 0 && th2 > 0)
                _colorMap[_x][_y] = color2;
            else if (th1 <= 0 && th2 < 0)
                _colorMap[_x][_y] = color3;
            else
                _colorMap[_x][_y] = color4;
        }
    }

}

void DoublePendulumRenderer::Render()
{
    switch (_myOpt.alg)
    {
        case RenderingAlgorithmType::EscapeTime:
            EscapeTimeRender();
            break;
        case RenderingAlgorithmType::EscapeAngle:
            EscapeAngleRender();
            break;
        default:
            break;
    }
}
void DoublePendulumRenderer::SetParams(const bool th1B, const bool th2B, const double th1NB, const double th2NB,
                                       const double dt, const double m1, const double m2, const double l, const double g,
                                       const bool ref, const bool rungeKutta)
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

