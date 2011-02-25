import minisat
from nose.tools import *
from nose.core import *

@raises(TypeError)
def test_var_new_var():
    """Var object should not be created except for Solver's new_var() method.
    """
    v = minisat.Var()

def test_var_positive():
    """Var object should be able to return its positive Var object.
    """
    solver = minisat.Solver()
    v = solver.new_var()
    p = +v

def test_var_negative():
    """Var object should be able to return its negative Var object.
    """
    solver = minisat.Solver()
    v = solver.new_var()
    n = -v

@raises(minisat.SolverError)
def test_var_value_first():
    """Var object should not return its value at first.
    """
    solver = minisat.Solver()
    v = solver.new_var()
    assert_is(solver.issatisfied(), False)
    v.value()

def test_var_value():
    """Var object should be able to return its value after solving its model.
    """
    solver = minisat.Solver()
    v = solver.new_var()
    assert_is(solver.issatisfied(), False)
    solver.add_clause([v])
    assert_is(solver.solve(), True)
    assert_is(solver.issatisfied(), True)
    v.value()

def test_var_multi_value():
    """Var objects should be able to return its value after solving its model.
    """
    solver = minisat.Solver()
    a = solver.new_var()
    b = solver.new_var()
    assert_is(solver.issatisfied(), False)
    solver.add_clause([ a,  b])
    # solver.add_clause([ a,  a])
    # solver.add_clause([ b,  b])
    solver.add_clause([-a,  b])
    solver.add_clause([ a, -b])
    assert_is(solver.solve(), True)
    assert_is(solver.issatisfied(), True)
    assert_is(a.value(), True)
    assert_is(b.value(), True)

if __name__ == '__main__':
    tp = TestProgram()
    tp.runTests()
