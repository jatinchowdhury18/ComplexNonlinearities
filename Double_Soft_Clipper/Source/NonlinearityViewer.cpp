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

void NonlinearityViewer::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::forestgreen);
    Path drawPath;

    for (int n = 0; n <= getWidth(); ++n)
    {
        auto x = (float) n / (float) getWidth() * 4.5f - 2.25f;

        auto y = dsc.function (x) * 0.475f;
        auto yDraw = getHeight() * (0.5f - y);

        if (n == 0)
            drawPath.startNewSubPath ((float) n, yDraw);
        else
            drawPath.lineTo ((float) n, yDraw);
    }
    g.strokePath (drawPath, PathStrokeType (2.0f));
}

void NonlinearityViewer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
