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

if __name__ == '__main__':
    tp = TestProgram()
    tp.runTests()
