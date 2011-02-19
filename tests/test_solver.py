import minisat
from nose.tools import *
from nose.core import *

def test_solver_new():
    """Test whether the instance object of Solver can be created or not.
    """
    solver = minisat.Solver()

def test_solver_new_var():
    """Test whether the instance object of Var can be created through Solver or
    not.
    """
    solver = minisat.Solver()
    a = solver.new_var()

def test_solver_add_clause():
    """Test for adding clause to Solver.
    """
    solver = minisat.Solver()
    a = solver.new_var()
    b = solver.new_var()
    solver.add_clause([a, b])

def test_solver_var_size():
    """Test for var_size.
    """
    solver = minisat.Solver()
    assert_is(solver.var_size(), 0)
    a = solver.new_var()
    b = solver.new_var()
    assert_is(solver.var_size(), 2)

def test_solver_assigned_size():
    """Test for assigned size.
    """
    solver = minisat.Solver()
    assert_is(solver.assigned_size(), 0)

def test_solver_issolved():
    """Test for issolved.
    """
    solver = minisat.Solver()
    assert_is(solver.issolved(), False)

def test_solver_issatisfied():
    """Test for issatisfied.
    """
    solver = minisat.Solver()
    assert_is(solver.issatisfied(), False)

def test_solver_simplify():
    """Test for simplify.
    """
    solver = minisat.Solver()
    assert_is(solver.simplify(), True)

if __name__ == '__main__':
    tp = TestProgram()
    tp.runTests()
