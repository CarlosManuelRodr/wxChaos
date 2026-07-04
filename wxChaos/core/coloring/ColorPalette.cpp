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
        case CoolBlue:
            {
                grad = "rgb(2,29,61);rgb(2,53,112);rgb(8,72,149);rgb(16,93,184);rgb(36,123,225);rgb(79,163,249);rgb(176,198,217);rgb(231,197,164);rgb(233,173,116);rgb(205,135,65);rgb(133,90,41);rgb(28,33,30);rgb(2,29,61);";
                paletteSize = 720;
                colorCycleLength = 72;
            }
            break;
        case StarryNight:
            {
                grad = "rgb(1,0,1);rgb(1,1,12);rgb(0,0,40);rgb(16,16,88);rgb(40,48,144);rgb(1,0,1);rgb(41,21,13);rgb(95,60,35);rgb(148,108,65);rgb(192,156,99);rgb(236,214,147);rgb(249,243,186);rgb(220,189,125);rgb(116,86,63);rgb(10,5,15);rgb(1,0,1);";
                paletteSize = 720;
                colorCycleLength = 72;
            }
            break;
        case MoonlitGarden:
            {
                grad = "rgb(6,22,27);rgb(15,54,49);rgb(38,91,67);rgb(85,132,82);rgb(151,171,105);rgb(230,217,157);rgb(192,139,92);rgb(74,45,48);rgb(6,22,27);";
                paletteSize = 540;
                colorCycleLength = 84;
            }
            break;
        case CoralReef:
            {
                grad = "rgb(5,24,46);rgb(0,74,103);rgb(0,135,143);rgb(91,199,184);rgb(238,235,189);rgb(244,151,118);rgb(197,82,96);rgb(79,35,73);rgb(5,24,46);";
                paletteSize = 560;
                colorCycleLength = 84;
            }
            break;
        case DesertMirage:
            {
                grad = "rgb(18,16,35);rgb(46,43,82);rgb(102,76,101);rgb(158,111,88);rgb(213,164,94);rgb(245,219,154);rgb(146,163,126);rgb(58,71,71);rgb(18,16,35);";
                paletteSize = 600;
                colorCycleLength = 96;
            }
            break;
        case PolarEmber:
            {
                grad = "rgb(7,17,24);rgb(20,59,82);rgb(75,139,162);rgb(170,220,218);rgb(244,244,226);rgb(223,151,93);rgb(145,68,67);rgb(48,29,43);rgb(7,17,24);";
                paletteSize = 600;
                colorCycleLength = 84;
            }
            break;
        case JadeTemple:
            {
                grad = "rgb(8,18,18);rgb(18,65,53);rgb(31,123,84);rgb(96,164,111);rgb(210,201,131);rgb(238,226,178);rgb(167,124,67);rgb(64,48,40);rgb(8,18,18);";
                paletteSize = 540;
                colorCycleLength = 84;
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
