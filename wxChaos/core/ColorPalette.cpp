#include "ColorPalette.h"

ColorPalette::ColorPalette()
{
    paletteSize = -1;
    colorCycleLength = 72;
}

void ColorPalette::SetStyle(const ColorPaletteTypes palette)
{
    switch (palette)
    {
        case Retro:
            {
                grad = "rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);";
                paletteSize = 300;
                colorCycleLength = 72;
            }
            break;
        case Hakim:
            {
                grad = "rgb(255,255,255);rgb(91,91,91);rgb(0,0,0);rgb(125,199,44);rgb(228,213,12);rgb(192,5,5);rgb(61,43,94);rgb(255,255,255);";
                paletteSize = 300;
                colorCycleLength = 72;
            }
            break;
        case Aquamarine:
            {
                grad = "rgb(30,102,162);rgb(255,255,255);rgb(91,18,122);rgb(0,0,0);rgb(16,33,112);rgb(30,145,87);rgb(30,102,162);";
                paletteSize = 300;
                colorCycleLength = 72;
            }
            break;
        case PastelDream:
            {
                grad = "rgb(255,209,220);rgb(255,245,186);rgb(207,244,252);rgb(210,230,255);rgb(230,214,255);rgb(255,209,220);";
                paletteSize = 320;
                colorCycleLength = 72;
            }
            break;
        case RoseGold:
            {
                grad = "rgb(47,36,36);rgb(122,82,72);rgb(224,170,146);rgb(255,230,214);rgb(201,138,111);rgb(77,52,50);rgb(47,36,36);";
                paletteSize = 300;
                colorCycleLength = 72;
            }
            break;
        case Gunmetal:
            {
                grad = "rgb(8,12,18);rgb(32,45,58);rgb(82,95,110);rgb(201,208,214);rgb(94,106,119);rgb(24,31,40);rgb(8,12,18);";
                paletteSize = 280;
                colorCycleLength = 72;
            }
            break;
        case SunsetDrive:
            {
                grad = "rgb(34,10,66);rgb(92,29,115);rgb(180,52,108);rgb(255,126,95);rgb(255,210,120);rgb(89,166,255);rgb(34,10,66);";
                paletteSize = 360;
                colorCycleLength = 72;
            }
            break;
        case AuroraBorealis:
            {
                grad = "rgb(2,24,43);rgb(0,78,92);rgb(0,168,150);rgb(126,255,214);rgb(96,139,255);rgb(48,64,173);rgb(2,24,43);";
                paletteSize = 340;
                colorCycleLength = 72;
            }
            break;
        case Vaporwave:
            {
                grad = "rgb(22,15,70);rgb(59,32,145);rgb(255,71,181);rgb(255,183,77);rgb(91,240,255);rgb(255,255,255);rgb(22,15,70);";
                paletteSize = 360;
                colorCycleLength = 72;
            }
            break;
        case DeepOcean:
            {
                grad = "rgb(0,7,20);rgb(0,32,63);rgb(0,91,150);rgb(72,202,228);rgb(173,232,244);rgb(0,91,150);rgb(0,7,20);";
                paletteSize = 320;
                colorCycleLength = 72;
            }
            break;
        case Ember:
            {
                grad = "rgb(18,6,6);rgb(82,17,0);rgb(173,44,0);rgb(255,120,24);rgb(255,210,94);rgb(255,248,212);rgb(18,6,6);";
                paletteSize = 320;
                colorCycleLength = 72;
            }
            break;
        case RainbowFire:
            {
                grad = "rgb(255,0,102);rgb(255,94,0);rgb(255,217,0);rgb(0,214,143);rgb(0,153,255);rgb(123,63,228);rgb(255,0,102);";
                paletteSize = 420;
                colorCycleLength = 72;
            }
            break;
        case ClassicMandelbrot:
            {
                grad = "rgb(8,12,28);rgb(18,38,114);rgb(27,99,183);rgb(137,218,236);rgb(255,255,236);rgb(255,194,67);rgb(184,68,20);rgb(72,18,44);rgb(8,12,28);";
                paletteSize = 720;
                colorCycleLength = 72;
            }
            break;
        case CustomGradient:
            break;
    };
}
