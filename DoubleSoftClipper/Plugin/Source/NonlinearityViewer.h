/*
  ==============================================================================

    NonlinearityViewer.h
    Created: 8 Sep 2019 4:01:55pm
    Author:  jatin

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DoubleSoftClipper.h"

//==============================================================================
/*
*/
class NonlinearityViewer    : public Component
{
public:
    NonlinearityViewer (DoubleSoftClipper& dsc);
    ~NonlinearityViewer();

    void paint (Graphics&) override;
    void resized() override;

    void updateCurve();

private:
    DoubleSoftClipper& dsc;
    Path curvePath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NonlinearityViewer)
};
