#include "DoubleSoftClipper.h"

void DoubleSoftClipper::setUpperLim (float newLim)
{
    upperLim = newLim;
    threeQuartersUpperLim = upperLim * 0.75f;
    halfUpperLim = upperLim / 2.0f;
}

void DoubleSoftClipper::setLowerLim (float newLim)
{
    lowerLim = -newLim;
    threeQuartersNegLowerLim = -0.75f * lowerLim;
    halfLowerLim = lowerLim / 2.0f;
}

void DoubleSoftClipper::setSlope (float slopeParam)
{
    slope = getSlopeFromParam (slopeParam);
    oneOverSlope = 1.0f / slope;
    setWidth (xOffFactor);
}

void DoubleSoftClipper::setWidth (float newWidth)
{
    xOffFactor = newWidth;
    xOff = oneOverSlope * powf (slope, xOffFactor);
}

void DoubleSoftClipper::setUpperSkew (float skewParam)
{
    upperSkew = getSkewFromParam (skewParam);
}

void DoubleSoftClipper::setLowerSkew (float skewParam)
{
    lowerSkew = getSkewFromParam (skewParam);
}

float DoubleSoftClipper::getSlopeFromParam (float param)
{
    return powf (5.0f, param);
}

float DoubleSoftClipper::getSkewFromParam (float param)
{
    return powf (4.0f, param);
}
