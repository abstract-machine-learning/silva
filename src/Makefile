#-----------------------------------------------------------------------
# Configuration

# Floating point precision
# [FLOAT | DOUBLE | LONG_DOUBLE]
PRECISION = DOUBLE

# Enforce soundness
# [ false | true ]
ifeq ($(SOUND), true)
    ENFORCE_SOUNDNESS = -DENFORCE_SOUNDNESS
else
    ENFORCE_SOUNDNESS = 
endif

CC = gcc
CCOPT = -Wall -Wextra -pedantic -O2 -std=c99 -g -DPRECISION_$(PRECISION) $(ENFORCE_SOUNDNESS)
LDOPT = -lm
NAME = silva
INSTALL_FOLDER = ../bin
DOXYFILE_PATH = ../doc/Doxyfile
DOC_PATH = ../doc/html/


#-----------------------------------------------------------------------
# Dependencies
all: $(NAME)

$(NAME): bitmask.o list.o stack.o set.o binary_heap.o priority_queue.o \
	binary_tree.o \
	search_algorithms/depth_first.o \
	search_algorithms/best_first.o \
	abstract_domains/abstract_domain.o \
	dataset.o \
	stopwatch.o \
	decision_tree.o \
	forest.o \
	classifier.o \
	data_mappers/decision_tree_silva.o \
	data_mappers/decision_tree_graphviz.o \
	data_mappers/forest_silva.o \
	data_mappers/classifier_silva.o \
	perturbation.o \
	abstract_interpreters/abstract_classifier.o \
	abstract_interpreters/classifier_hyperrectangle.o \
	abstract_interpreters/decision_tree_hyperrectangle.o \
	abstract_interpreters/forest_hyperrectangle.o \
	options.o \
	silva.o

install: $(NAME)

.PHONY: clean, doc


#-----------------------------------------------------------------------
# Recipes
%.o: %.c
	@echo "Compiling $@..."
	@$(CC) $(CCOPT) -c -o $@ $^

silva:
	@echo "Linking $^ into $@..."
	@$(CC) $(CCOPT) -o $@ $^ $(LDOPT)

install:
	@echo "Moving into installation folder $(INSTALL_FOLDER)/$(NAME)..."
	@mkdir -p $(INSTALL_FOLDER)
	@mv $(NAME) $(INSTALL_FOLDER)/$(NAME)

clean:
	@echo "Cleaning..."
	@rm -fR *.o */*.o

doc:
	@echo "Generating documentation..."
	@rm -fR $(DOC_PATH)
	@doxygen $(DOXYFILE_PATH)
