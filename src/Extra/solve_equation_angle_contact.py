from sympy import *
x, y, c = symbols('x y c')
# phi_c = x 
# theta = y
init_printing(use_unicode=True)
print(
simplify(
    c - 0.5*cos(y)+ (1/6)*cos(y)**3 + 
    (-c*tan(y)+ 0.5*sin(y)-(1/6)*cos(y)**2 * sin(y))*tan(x) - tan(x) - tan(y)
)
)