/*
  ==============================================================================

    NonlinearityViewer.cpp
    Created: 8 Sep 2019 4:01:55pm
    Author:  jatin

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "NonlinearityViewer.h"

//==============================================================================
NonlinearityViewer::NonlinearityViewer (DoubleSoftClipper& dsc) :
    dsc (dsc)
{
}

NonlinearityViewer::~NonlinearityViewer()
{
}

void NonlinearityViewer::updateCurve()
{
    curvePath.clear();
    for (int n = 0; n <= getWidth(); ++n)
    {
        auto x = (float) n / (float) getWidth() * 4.5f - 2.25f;

        auto y = dsc.function (x) * 0.475f;
        auto yDraw = getHeight() * (0.5f - y);

        if (n == 0)
            curvePath.startNewSubPath ((float) n, yDraw);
        else
            curvePath.lineTo ((float) n, yDraw);
    }

    repaint();
}

void NonlinearityViewer::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::forestgreen);
    g.strokePath (curvePath, PathStrokeType (2.0f));
}

void NonlinearityViewer::resized()
{
}
