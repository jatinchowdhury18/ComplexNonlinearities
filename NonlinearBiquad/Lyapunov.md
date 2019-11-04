$$
x_1[n] = f_{NL}(b_1 u[n-1] - a_2y[n-1] + x_2[n-1])\\
x_2[n] = f_{NL}(b_2 u[n-2] - a_2y[n-2])
$$

Linear biquad
$$
\begin{bmatrix} x_1[n+1] \\ x_2[n+1] \\ y[n+1] \end{bmatrix} =
\begin{bmatrix} 0& 1& -a_1\\ 0& 0& -a_2\\ 1& 0& 0 \end{bmatrix}
\begin{bmatrix} x_1[n] \\ x_2[n] \\ y[n] \end{bmatrix}
+ \begin{bmatrix} b_1\\ b_2\\ b_0 \end{bmatrix} u[n]
$$

Nonlinear biquad
$$
\begin{bmatrix} x_1[n+1] \\ x_2[n+1] \\ y[n+1] \end{bmatrix} =
\begin{bmatrix} h_1(x_1[n], x_2[n], y[n])\\ h_2(x_1[n], x_2[n], y[n])\\
h_3(x_1[n], x_2[n], y[n]) \end{bmatrix}
\begin{bmatrix} x_1[n] \\ x_2[n] \\ y[n] \end{bmatrix}
+ \begin{bmatrix} b_1\\ b_2\\ b_0 \end{bmatrix} u[n]
$$

where
$$
h_1(x_1[n], x_2[n], y[n]) = f_{NL}(x_2[n]) - a_1y[n] \\
h_2(x_1[n], x_2[n], y[n]) = -a_2y[n] \\
h_3(x_1[n], x_2[n], y[n]) = f_{NL}(x_1[n])
$$

The Jacobian matrix is then:
$$
J = \begin{bmatrix}
    0& f'_{NL}(x_2[n])& -a_1 \\
    0& 0& -a_2 \\
    f'{NL}(x_1[n])& 0& 0
\end{bmatrix}
$$

Constraint on the nonlinear function that $f'_{NL}$ must be
less than 1 everywhere. Condition on $a_1, a_2$ means that
the original linear filter must also be stable.

-----------------------------------------------------------------

Nonlinear feedback:

$$
h_1(x_1[n], x_2[n], y[n]) = x_2[n] - a_1f_{NL}(y[n]) \\
h_2(x_1[n], x_2[n], y[n]) = -a_2f_{NL}(y[n]) \\
h_3(x_1[n], x_2[n], y[n]) = y[n]
$$

The Jacobian matrix is then:
$$
J = \begin{bmatrix}
    0& 0& -a_1f'_{NL}(y[n]) \\
    0& 0& -a_2f'_{NL}(y[n]) \\
    0& 0& 0
\end{bmatrix}
$$

---------------------------------

$$
|p|^2 = \frac{-a_1+\sqrt{a_1^2-4a_2}}{2} \frac{-a_1+\sqrt{a_1^2-4a_2}}{2} \\
 = \frac{a_1^2 - |a_1^2 - 4a_2|}{4}
$$

case 1: $a_1^2 > 4a_2$

$$
= \frac{a_1^2 - a_1^2 + 4a_2}{4} = a_2
$$

case 2: $4a_2 >  a_1^2$:

$$
 = \frac{a_1^2 + a_1^2 - 4a_2}{4} = \frac{1}{2} a_1^2 - a_2
$$

-------------------

$$
\frac{-a_1 \pm j\sqrt{4a_2 - a_1^2}}{2}
$$

$$
\angle = \arctan \left( \pm \frac{\sqrt{4a_2 - a_1^2}}{a_1} \right)
$$

