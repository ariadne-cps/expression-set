from pyariadne import *

def test_expression_set():
    x, y = RealVariable("x"), RealVariable("y")
    bounds = RealVariablesBox([RealVariableInterval(x, RealInterval(Real(0), Real(1))), RealVariableInterval(y, RealInterval(Real(-1), Real(1)))])
    assert len(bounds) == 2
    cs = RealExpressionConstraintSet([x + y <= Real(1)])
    bounded = intersection(bounds, cs)
    assert bounded.number_of_constraints() == 1
    assert bounded.euclidean_set(RealSpace([x,y])).dimension() == 2
