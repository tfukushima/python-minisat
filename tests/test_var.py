import minisat
from nose.tools import *
from nose.core import *

@raises(TypeError)
def test_var_new_var():
    """Test whether the instance object of Var can be created through Solver or
    not.
    """
    v = minisat.Var()

def test_var_positive():
    """Test for +a.
    """
    solver = minisat.Solver()
    v = solver.new_var()
    p = +v

def test_var_negative():
    """Test for -a.
    """
    solver = minisat.Solver()
    v = solver.new_var()
    n = -v

if __name__ == '__main__':
    tp = TestProgram()
    tp.runTests()
