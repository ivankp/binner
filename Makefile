TESTS := $(patsubst %.cc,%,$(wildcard test/*.cc))

DEP := $(TESTS:%=.dep/%.d)

CXXFLAGS := -std=c++14 -Wall -Og -Isrc

all: $(EXE)

#Don't create dependencies when cleaning
ifeq (0, $(words $(findstring $(MAKECMDGOALS), clean)))
-include $(DEP)
endif

$(TESTS): test/%:
	g++ $(CXXFLAGS) $< -o $@ $(LIBS_$*)

$(DEP): .dep/%.d: test/%.cc | .dep
	@g++ $(CXXFLAGS) -MM -MT '$(<:%.cc=%)' $< -MF $@

.dep:
	@mkdir $@

clean:
	@rm -rfv .dep $(TESTS)
