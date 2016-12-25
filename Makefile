TESTS := $(patsubst %.cc,%,$(wildcard test/*.cc))

DEP := $(TESTS:test/%=.dep/%.d)

CXXFLAGS := -std=c++14 -Wall -O3 -Isrc -fmax-errors=3

all: $(TESTS)

#Don't create dependencies when cleaning
ifeq (0, $(words $(findstring $(MAKECMDGOALS), clean)))
-include $(DEP)
endif

$(TESTS): test/%: | .dep/%.d
	g++ $(CXXFLAGS) $< -o $@ $(LIBS_$*)

$(DEP): .dep/%.d: test/%.cc | .dep
	g++ $(CXXFLAGS) -MM -MT '$(<:%.cc=%)' $< -MF $@

.dep:
	mkdir $@

clean:
	@rm -rfv .dep $(TESTS)
