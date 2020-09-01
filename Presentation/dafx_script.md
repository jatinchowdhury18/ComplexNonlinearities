# DAFx Script

## Intro Slide
Hello, my name is Jatin Chowdhury and I am a recent Master's
graduate from CCRMA at Stanford University. Today, I'll be
presenting my research on Nonlinear Biquad Filters, for the
2020 DAFx conference.

## Outline
I'll start this presentation by giving a brief review of linear
biquad filters, and then outlining two nonlinear structures: the
nonlinear biquad filter, and the nonlinear feedback filter. Next,
I'll analyze the conditions under which these filter structures are
guaranteed to be stable, and finally I'll discuss the possibility
of using these filters for analog modelling. Along the way, there
will be audio examples demonstrating these ideas.

## Motivation
My research on Nonlinear Biquad Filters began as part of a more
general exploration of designing digital audio effects using
nonlinear signal processing. I've found that most of the interesting
uses of nonlinear functions in audio effects come from the world
of physical and virtual analog modelling, and while there is a
wealth of great research in this area, I was interested in trying
to develop nonlinear effects purely in the digital domain.

From a mathematical standpoint, it is pretty straightforward to
apply nonlinear functions to a signal, however in the audio world,
we want effects that will give us a certain desired sound, meaning
that we also need to be able to control these nonlinear effects in
an intelligent and useful way.

Finally, even though these effects might be developed in the digital
domain, there is no reason we can't also use them for analog
modelling purposes. I'll give a couple examples of this idea later
on.

## Biquad Filter
Before we can develop nonlinear biquad filters, we must first
review the linear biquad filter. In general, the term
"biquad filter" is used to describe any 2nd-order recursive filter.
Biquad filters are often used in digital signal processing, since
any higher-order filter can be implemented using a series of biquad
filters.

Biquad filters can be implemented in several well-known "Direct Forms".
Here we show the popular "Transposed Direct Form II", which is known
for having favorable numerical propoerties.

## Biquad Filter Equations
Transposed Direct Form II is also useful because the placement of
the delay elements in the structure allow it to be described in a
state-space form. Here, we define the states $x_1$ and $x_2$ as
the state values stored in the two delay elements.

## Nonlinear Biquad Filter Structure
Now we can examine the first nonlinear biquad structure,
referred to here as the "nonlinear biquad filter". In this
structure we simply apply nonlinear functions to the state
values of the filter.

## Nonlinear Biquad LPF
While we can't directly determine the frequency response of
a nonlinear filter, we can obtain an estimate of the frequency
response at different operating points of the filter, by
passing sine sweeps of different amplitudes through the filter,
and measuring the frequency-domain output.

Here we see the output of a resonant lowpass filter constructed
using the nonlinear biquad structure, using saturating
nonlinearities for the nonlinear elements. Note that as the
amplitude of the input signal increases, the amount of resonance
exhibited by the filter becomes increasingly damped.

## Nonlinear Biquad Bell -> Shelf
We can se a similar type of damped resonance for this peaking
filter... and again for this shelving filter.

## Nonlinear Biquad Pole/Zero
We can explain this behavior using pole-zero analysis. By
linearizing the nonlinear functions about a given operating
point, we can analyze the poles and zeros of the filter about
that operating point. Note that as the input gain to the filter
increases, the poles and zeros move closer to zero.

## DEMO BREAK
Now let's hear what this effect sounds like...

## Nonlinear Feedback Structure
Next, we can examine a second nonlinear filter structure,
referred to as the "nonlinear feedback filter", in which we
apply a nonlinear function to the output of the filter before
using it to calculate the future states.

## Nonlinear Feedback LPF
Here we see the output of a resonant lowpass filter constructed
using the nonlinear feedback structure. Note that as the input
gain to the filter increases, the resonant frequency seems to
sweep up to higher frequencies.

## Nonlinear Feedback Pole/Zero
Again, this behavior can be explained using pole-zero analysis.
Note that for this structure, the zero locations do not change,
regardless of the input, however, the pole locations sweep
outward and then back towards zero again.

## DEMO BREAK
Now let's hear what the nonlinear feedback filter sounds like...

## Stability Question
Next, we want to ask about the stability of these filter
structures. In particular, we want to know if we can guarantee
that a nonlinear biquad filter will be stable, given that
the corresponding linear filter is stable, and if we need
any constraints on the types of nonlinear functions we can use.

Let's start by looking at a few examples: Again, we construct
a resonant lowpass filter, and pass a sine sweep through it,
using several different nonlinear functions.

## Stability Tanh
If we using a saturating nonlinearity, such as $\tanh$
the filter appears to be stable.

## Stability Abs
If we use a full wave rectifying nonlinearity, the filter
goes unstable. At first, I thought this instability
might be linked to the fact that the absolute value function
is not monotonic.

## Stability Sine
For my next experiment, I tried another non-monotonic function,
the $\sin$ nonlinearity, which appears to be stable.

## Lyapunov Stability
In order to analyze the stability of these nonlinear filter
structures more rigorously, let us look at these filters
through the lens of Lyapunov stability: a sufficient, but
not strictly neccessary metric for determining the Bounded-Input
Bounded-Output stability of nonlinear systems.

In order to evaluate the Lyapunov stability of a system, we
need to first construct a state-space function that describes
the system, and then determine the Jacobian of that function.
If every element of the Jacobian matrix is less than 1 at some
operating point, we can claim the system to be stable about that
point.

## Nonlinear Biquad Stability -> Jacobian
For the nonlinear biquad structure, we can formulate the
state-space function as shown here. We can then compute the
Jacobian as follows... An important note is that if the
derivative of the nonlinear function does not exist at some
operating point, we cannot guarantee that the filter will
be stable about that point.

## Nonlinear Feedback Stability -> Jacobian
We can now form a similar state-space function for the
nonlinear feedback filter... and compute the corresponding
Jacobian matrix...

## Stability Constraint
In order to satisy Lyapunov stability for the Jacobian matrices
computed above, the following stability constraint is proposed:
If the absolute value of the derivative of the nonlinear function
is less than 1, then we can guarantee that the nonlinear filter will
be stable, provided, of course, that the corresponding linear filter
is stable as well.

Note that for the absolute value function that we saw earlier,
the derivative does not exist at $x=0$, meaning that we can't
guarantee the nonlinear filter will be stable. Unfortunately,
many nonlinear functions that are often used in audio signal 
processing are piecewise functions, meaning they have this
issue as well.

## Stability BLAMP
For piecewise nonlinear functions, we can use a technique
called "BLAMP" for smoothing these functions, to ensure that
the function has a derivative everywhere.

## Nonlinear Biquad Modelling Question
Finally, I wanted to look at the possibility of using these
nonlinear filter structures for analog modelling. From looking
at the response of this resonant lowpass filter, we can see that
nonlinear resonance is somewhat similar to the response of an
analog filter. When an analog filter exhibits resonant
behaviour, the filter can only supply so much gain for the
resonance before distorting.

## Nonlinear Biquad Sallen-Key
As an example, we can use the nonlinear biquad structure to
approximate the nonlinear behavior of an overdriven Sallen-Key
lowpass filter. While the model shown here, is not particularly
accurate, it demonatrates a remarkable improvement over the
strictly linear biquad filter, and could be improved with a more
intelligent choice of nonlinear functions.

## 808 circuit
Another possibility would be to use the nonlinear feedback filter
to model the Roland TR-808 kick drum circuit. As shown here, the
envelope generator, which is triggered by an input pulse, causes
the cutoff frequency of the Bridged-T Network to shift, in a way
not unlike the nonlinear feedback filter developed here.

### Future Work
Future research on this topic will focus on the the following
questions: How do we choose nonlinear functions, that will give
some desired pole/zero movement? How does the distortion caused
by the nonlinear functions affect the overall sound? And how can
we integrate these filter structures into audio effects in a way
that makes sense to non-technical users?

## Conclusion
In this presentation, we have analyzed two structures for
implementing nonlinear biquad filters, and shown how they can be
guaranteed stable. We have also shown some possibilities for using
these filter structures in the virtual analog domain. Finally, the
audio examples presented here were made using open-source audio
plugins that immplement these filter structures. These plugins can
be downloaded at the link provided.

## Thanks
Thank you to the DAFx organizing committee for hosting these
virtual presentations, and thank you all for taking the time to
listen.
