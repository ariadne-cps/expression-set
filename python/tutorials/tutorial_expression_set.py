from pyariadne import *

x, y = RealVariable("x"), RealVariable("y")
box = RealVariablesBox([RealVariableInterval(x, RealInterval(Real(0), Real(1))), RealVariableInterval(y, RealInterval(Real(-1), Real(1)))])
constraints = RealExpressionConstraintSet([x+y <= Real(1)])
region = intersection(box, constraints)
print(region)
print(region.euclidean_set(RealSpace([x,y])))
