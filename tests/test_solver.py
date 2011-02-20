import minisat
from nose.tools import *
from nose.core import *

def test_solver_new():
    """Solver should be able to be create.
    """
    solver = minisat.Solver()

def test_solver_new_var():
    """Solver should be able to create a new Var object
    """
    solver = minisat.Solver()
    a = solver.new_var()

def test_solver_add_clause():
    """Solver should be able to be added a clause
    """
    solver = minisat.Solver()
    a = solver.new_var()
    b = solver.new_var()
    solver.add_clause([a, b])

def test_solver_var_size():
    """Solver should be aware of the number of its Var objects.
    """
    solver = minisat.Solver()
    assert_is(solver.var_size(), 0)
    a = solver.new_var()
    b = solver.new_var()
    assert_is(solver.var_size(), 2)

def test_solver_assigned_size():
    """Solver should be aware of the number of assigned Var objects.
    """
    solver = minisat.Solver()
    assert_is(solver.assigned_size(), 0)

def test_solver_issolved():
    """Solver should be aware whether given model has been solved or not.
    At first, given model is not solved.
    """
    solver = minisat.Solver()
    assert_is(solver.issolved(), False)

def test_solver_issatisfied():
    """Solver should be aware whether given model has been satisfied or not.
    At first, given model is not satisfied.
    """
    solver = minisat.Solver()
    assert_is(solver.issatisfied(), False)

def test_solver_simplify():
    """Solver should be able to simplify the model by given clauses.
    """
    solver = minisat.Solver()
    assert_is(solver.simplify(), True)

def test_solver_solve():
    """Solver should be able to solve the modle by given clauses.
    """
    solver = minisat.Solver()
    a = solver.new_var()
    b = solver.new_var()
    solver.add_clause([ a,  b])
    # solver.add_clause([a, a])
    # solver.add_clause([-a,  b])
    # solver.add_clause([ a, -b])
    assert_is(solver.solve(), True)

if __name__ == '__main__':
    tp = TestProgram()
    tp.runTests()
