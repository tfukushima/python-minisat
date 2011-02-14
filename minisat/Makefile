MINISAT_DIR = $(HOME)/Downloads/minisat
MINISAT_CORE = $(MINISAT_DIR)/core
MINISAT_SIMP = $(MINISAT_DIR)/simp
MINISAT_MTL = $(MINISAT_DIR)/mtl
MINISAT_UTILS = $(MINISAT_DIR)/utils
PYTHON_INCLUDE = /usr/include/python2.7

PHONEY: check-syntax
check-syntax:
		$(CXX) -Wall -Wextra -fsyntax-only -I$(MINISAT_DIR) -I$(MINISAT_CORE) -I$(MINISAT_SIMP) -I$(MINISAT_MTL) -I$(MINISAT_UTILS) -I$(PYTHON_INCLUDE)  $(CHK_SOURCES)