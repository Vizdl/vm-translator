include Makefile-extras

# C++ 11 just in case
CXXFLAGS=--std=c++11 -I. -Iincludes

# must specify make target
all: test

# compile only
notest: translator

# testing student code
test: translator
	@bash bin/run-tests translator

# testing "working" code
test-working:
	@bash bin/run-tests working-translator

# add missing expected test outputs
test-add: test-add-Pxml

test-add-Pxml:
	@bash bin/run-tests add-tests quiet

# regenerate all expected test outputs
test-new: test-new-Pxml

test-new-Pxml:
	@bash bin/run-tests renew-tests quiet


clean:
	rm -f lib/*/translator

translator: lib/$(CS_ARCH)/translator
	@true

lib/$(CS_ARCH)/translator: translator.cpp lib/$(CS_ARCH)/lib.a
	${CXX} ${CXXFLAGS} -o $@ $^
