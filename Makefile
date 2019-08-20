A0 = alephzero

LIB_DIR = lib

PYTHON ?= python3
PY_SUFFIX ?= $(shell $(PYTHON)-config --extension-suffix)

CXXFLAGS += -Wall -Wextra -fPIC -std=c++17 -D_GLIBCXX_USE_CXX11_ABI=0
CXXFLAGS += -Ithird_party/pybind11/include
CXXFLAGS += $(shell $(PYTHON)-config --cflags)

.PHONY: all clean install

all:
	@echo "TODO"

$(LIB_DIR)/$(A0)$(PY_SUFFIX): $(A0).cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -shared $< -o $@


install: $(LIB_DIR)/$(A0)$(PY_SUFFIX)
	@echo "TODO"

uninstall:
	@echo "TODO"

clean:
	rm -rf $(LIB_DIR)/
