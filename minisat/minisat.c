/*
 * minisat.c - Python binding for MiniSat
 *
 * This source code is licensed under the MIT License.
 * See the file COPYING for more details.
 *
 * @author: Taku Fukushima <tfukushima@dcl.info.waseda.ac.jp>
 */

#include <Python.h>
// #include <structmember.h>
#include <stdbool.h>

#include "minisat.h"

typedef enum status {
    NOT_SOLVED_YET = 0,
    SATISFIED,
    UNSATISFIABLE,
    UNSATISFIABLE_UNDER_ASSUMPTIONS,
} status_t;

// MiniSat Solver object
typedef struct {
    PyObject_HEAD
    minisat_solver _solver;
    status_t result;
} SolverObject;

// Var object
typedef struct {
    PyObject_HEAD
    int value;
    SolverObject *solver;
} VarObject;

static PyMethodDef MiniSat_methods[] = {
    {NULL, NULL, 0, NULL}  // sentinel
};

static PyObject *SolverError;
static PyObject *VarError;

// Var
static PyTypeObject VarType = {
    PyObject_HEAD_INIT(NULL)
    tp_name: "minisat.Var",
    tp_basicsize: sizeof(VarObject),
    tp_flags: Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    tp_doc: "MiniSat variable objects"
};

static PyObject *Var_positive(VarObject *var)
{
    VarObject *rvar = PyObject_New(VarObject, &VarType);

    rvar->value = minisat_lit_pos_var(var->value);
    rvar->solver = var->solver;
    return (PyObject *)rvar;
}

static PyObject *Var_negative(VarObject *var)
{
    VarObject *rvar = PyObject_New(VarObject, &VarType);

    rvar->value = minisat_lit_neg_var(var->value);
    rvar->solver = var->solver;
    return (PyObject *)rvar;
}

static int check_model_available(status_t result, int deep_check)
{
    switch (result) {
    case NOT_SOLVED_YET:
        if (deep_check) {
            PyErr_SetString(SolverError, "Model haven't been solved yet.");
            return false;
        }
        break;
   case UNSATISFIABLE:
       PyErr_SetString(SolverError, "Model is not satifiable." );
       return false;
       break;
    case UNSATISFIABLE_UNDER_ASSUMPTIONS:
        if (deep_check) {
            PyErr_SetString(SolverError,
                            "Model is not satifiable under given assumptions.");
            return false;
        }
    default:
        return true;
        break;
    }
}

static PyObject *Var_value(VarObject *var)
{
    int value;
    const PyObject *exception;

    if (!check_model_available(var->solver->result, true))
        return NULL;

    value = minisat_model_value(var->solver, var->value);
    switch (value) {
    case 0:  // case of l_True
        Py_RETURN_TRUE;
        break;
    case 1:  // case of l_False
        Py_RETURN_FALSE;
        break;
    default:
        break;
    }
    Py_RETURN_NONE;        
}

static PyMethodDef Var_methods[] = {
    {"__invert__", (PyCFunction) Var_negative, METH_O | METH_COEXIST,
     "Return value of var."},
    {"__neg__", (PyCFunction) Var_negative, METH_O | METH_COEXIST,
     "Return value of var."},
    {"__pos__", (PyCFunction) Var_positive, METH_O | METH_COEXIST,
     "Return value of var."},
    {"value", (PyCFunction) Var_value, METH_NOARGS,
     "Return value of var."},
    { NULL, NULL, 0, NULL}  // Sentinel
};

static PyNumberMethods Var_as_number = {
    0,  // nb_add
    0,  // nb_subtract
    0,  // nb_multiply
    0,  // nb_divide
    0,  // nb_remainder
    0,  // nb_divmod
    0,  // nb_power
    (unaryfunc) Var_negative, // nb_negative
    (unaryfunc) Var_positive, // nb_positive
    0,  // nb_absolute
    0,  // nb_nonzero
    (unaryfunc) Var_negative,  // nb_invert
};

// Solver
static PyObject* Solver_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SolverObject *self;
    
    self = (SolverObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->_solver = minisat_new();
        self->result = NOT_SOLVED_YET;
        if (self->_solver == NULL) {
            Py_DECREF(self);
            return NULL;
        }
    }
    return (PyObject *)self;
}

static PyObject *Solver_dealloc(SolverObject *self)
{
    minisat_free(self->_solver);
    /* Py_XDECREF(self->_solver); */
    /* Py_XDECREF(self->result); */
    /* Py_XDECREF(self->clause_size); */
    /* Py_XDECREF(self->var_size); */
    self->ob_type->tp_free((PyObject *)self);
}

static PyTypeObject SolverType = {
    PyObject_HEAD_INIT(NULL)
    tp_name: "minisat.Solver",
    tp_basicsize: sizeof(SolverObject),
    tp_flags: Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    tp_new :  Solver_new,
    tp_dealloc: (destructor) Solver_dealloc,
    tp_doc: "MiniSat solver objects",
};

static void _vars_to_lits(PyObject *seq, int *lits, int len)
{
    int i;
    PyObject *element;

    for (i = 0; i < len; i++) {
        element = PySequence_Fast_GET_ITEM(seq, i);
        lits[i] = minisat_lit_pos_var(((VarObject *)element)->value);
    }
}

static PyObject *Solver_assigned_size(SolverObject *self)
{
    return Py_BuildValue("i", minisat_assigned_size(self->_solver));
}

static PyObject *Solver_add_clause(SolverObject *self, PyObject *args)
{
    int i, len;
    int *lits;
    PyObject *var_list, *seq, *element, *ret;
    
    if (!PyArg_ParseTuple(args, "O", &var_list)) {
        PyErr_SetString(PyExc_RuntimeError,
                        "The clause should be given as a list.");
        return NULL;
    }
    seq = PySequence_Fast(var_list, "the clause should be a list.");
    len = PySequence_Length(seq);
    lits = (int *) PyMem_Malloc(sizeof(int)*len);
    // lits = (int *) malloc(sizeof(int)*len);
    if (lits == NULL)
        return PyErr_NoMemory();
    _vars_to_lits(seq, lits, len);
    Py_DECREF(seq);

    if (!minisat_add_clause(self->_solver, lits, len)) {
        PyMem_Free(lits);
        // free(lits);
        Py_RETURN_FALSE;
    }

    PyMem_Free(lits);
    // free(lits);
    Py_RETURN_TRUE;
}

static PyObject *Solver_clause_size(SolverObject *self)
{
    return Py_BuildValue("i", minisat_clause_size(self->_solver));
}

static PyObject *Solver_new_var(SolverObject *self)
{
    VarObject *var;

    var = PyObject_New(VarObject, &VarType);
    var->value = minisat_new_var(self->_solver);
    var->solver = self;
    self->result = NOT_SOLVED_YET;
    return (PyObject *)var;
}


static PyObject *Solver_issolved(SolverObject *self){
    if (self->result != NOT_SOLVED_YET)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *Solver_issatisfied(SolverObject *self)
{
    if (self->result == SATISFIED)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *Solver_simplify(SolverObject *self)
{
    if (!minisat_simplify(self->_solver)) {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject *Solver_solve(SolverObject *self, PyObject *args)
{
    int i, len;
    int *assumps;
    PyObject *var_list, *seq, *ret;

    if (args == NULL) {
        if (!minisat_solve(self->_solver)) {
            self->result = UNSATISFIABLE_UNDER_ASSUMPTIONS;
            Py_RETURN_FALSE;
        }
        self->result = SATISFIED;
        Py_RETURN_TRUE;
    }

    if (!PyArg_ParseTuple(args, "O", &var_list)) {
        PyErr_SetString(SolverError, "Model haven't been solved yet.");
        return NULL;
    }
    seq = PySequence_Fast(var_list, "the assumption should be a list.");
    len = PySequence_Length(seq);
    assumps = (int *) PyMem_Malloc(sizeof(int)*len);
    // assumps = (int *) malloc(sizeof(int)*len);
    if (assumps == NULL)
        return PyErr_NoMemory();
    _vars_to_lits(seq, assumps, len);
    Py_DECREF(seq);

    if (!minisat_solve_with_assumps(self->_solver, assumps, len)) {
        PyMem_Free(assumps);
        // free(assumps);
        self->result = UNSATISFIABLE;
        Py_RETURN_FALSE;
    }
    PyMem_Free(assumps);
    // free(assumps);
    self->result = SATISFIED;
    Py_RETURN_TRUE;
}

static PyObject *Solver_var_size(SolverObject *self)
{
    return Py_BuildValue("i", minisat_var_size(self->_solver));
}

static PyMethodDef Solver_methods[] = {
    {"add_clause", (PyCFunction) Solver_add_clause, METH_VARARGS,
     "Add a clause to solver.\nThe clause should be a list contains vars."},
    {"assigned_size", (PyCFunction) Solver_assigned_size, METH_NOARGS,
     "Return the size of assined variables."},
    {"clause_size", (PyCFunction) Solver_clause_size, METH_NOARGS,
     "Return the size of clauses."},
    {"issolved", (PyCFunction) Solver_issolved, METH_NOARGS,
     "Return if model is solved or not."},
    {"issatisfied", (PyCFunction) Solver_issatisfied, METH_NOARGS,
     "Return if model is satified or not"},
    {"new_var", (PyCFunction) Solver_new_var, METH_NOARGS,
     "Add new variables to solver."},
    {"simplify", (PyCFunction) Solver_simplify, METH_NOARGS,
     "Simplyfy the model with given variables."},
    {"solve", (PyCFunction) Solver_solve, METH_NOARGS,
     "Solve the model with given assumptions"},
    {"var_size", (PyCFunction) Solver_var_size, METH_NOARGS,
     "Return the size of variables"},
    { NULL, NULL, 0, NULL}  // Sentinel
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initminisat(void)
{
    PyObject *minisat;
    
    minisat = Py_InitModule3("minisat", MiniSat_methods,
                             "Python binding for MiniSat.");
    if (minisat == NULL)
        return;
    // Solver
    // SolverType.tp_new = PyType_GenericNew;
    SolverType.tp_methods = Solver_methods;
    if (PyType_Ready(&SolverType) < 0)
        return;
    Py_INCREF(&SolverType);
    PyModule_AddObject(minisat, "Solver", (PyObject *)&SolverType);
    
    SolverError = PyErr_NewException("minisat.SolverError", NULL, NULL);
    Py_INCREF(SolverError);
    PyModule_AddObject(minisat, "SolverError", SolverError);

    // Var
    VarType.tp_methods = Var_methods;
    VarType.tp_as_number = &Var_as_number;
    if (PyType_Ready(&VarType) < 0)
        return;
    Py_INCREF(&VarType);
    PyModule_AddObject(minisat, "Var", (PyObject *)&VarType);
    
    VarError = PyErr_NewException("minisat.VarError", NULL, NULL);
    Py_INCREF(SolverError);
    PyModule_AddObject(minisat, "VarError", VarError);
}
