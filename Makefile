# wrapper makefile for control cmake and build
BUILD_DIR=build
CMAKE_CONFIG_OPTS=-H. -B${BUILD_DIR} -DCMAKE_INSTALL_PREFIX=`pwd`/${BUILD_DIR}/install/usr
BUILD_CONFIG_OPTS=-s -C ${BUILD_DIR}

help:
	@echo "make targets:"
	@echo ""
	@echo "	build targets:"
	@echo "	cfgclean	remove complete build area (including cmake configuration!)"
	@echo "	clean		remove build artifacts in build area"
	@echo "	build		build/rebuild program"
	@echo "	doc		create doc files"
	@echo "	doxygen		create doxygen documentation"
	@echo "	check		run regression test"
	@echo "	debian		create debian packages"
	@echo ""
	@echo "	configure targets:"
	@echo "	debug"
	@echo "	no-debug	set/reset DEBUG option (default OFF)"
	@echo "	verilog"
	@echo "	no-verilog	set/reset VERILOG option (default OFF)"
	@echo "	python"
	@echo "	no-python	set/reset PYTHON option (default OFF)"
	@echo "	tcl"
	@echo "	no-tcl		set/reset TCL option (default OFF)"
	@echo "	valgrind"
	@echo "	no-valgrind	set/reset gtest regression check with valgrind leak check (default OFF)"
	@echo "	all    		set all configure variants (python, tcl, verilog)"
	@echo "	simple		reset all configure variants (python, tcl, verilog)"
	@echo ""
	@echo "	maintainance targets:"
	@echo "	keytrans	create keytrans.h"

cfgclean:
	rm -rf build

# for config cmake options see:
# https://stackoverflow.com/questions/11143062/getting-cmake-to-build-out-of-source-without-wrapping-scripts
check-config:
	@test -f build/Makefile || cmake ${CMAKE_CONFIG_OPTS}

# build targets

clean: check-config
	@make ${BUILD_CONFIG_OPTS} clean

build: check-config
	@make ${BUILD_CONFIG_OPTS}

check: check-config
	@make ${BUILD_CONFIG_OPTS} check

doc: check-config
	@make ${BUILD_CONFIG_OPTS} doc

doxygen: check-config
	@make ${BUILD_CONFIG_OPTS} doxygen

debian: check-config
	@make ${BUILD_CONFIG_OPTS} debian

# configure targets

debug:
	@cmake ${CMAKE_CONFIG_OPTS} -DCMAKE_BUILD_TYPE=Debug
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

no-debug:
	@cmake ${CMAKE_CONFIG_OPTS} -DCMAKE_BUILD_TYPE=
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

verilog:
	@cmake ${CMAKE_CONFIG_OPTS} -DBUILD_VERILOG=ON
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

no-verilog:
	@cmake ${CMAKE_CONFIG_OPTS} -DBUILD_VERILOG=OFF
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

python:
	@cmake ${CMAKE_CONFIG_OPTS} -DBUILD_PYTHON=ON
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

no-python:
	@cmake ${CMAKE_CONFIG_OPTS} -DBUILD_PYTHON=OFF
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

tcl:
	@cmake ${CMAKE_CONFIG_OPTS} -DBUILD_TCL=ON
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

no-tcl:
	@cmake ${CMAKE_CONFIG_OPTS} -DBUILD_TCL=OFF
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

valgrind:
	@cmake ${CMAKE_CONFIG_OPTS} -DCHECK_VALGRIND=ON
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

no-valgrind:
	@cmake ${CMAKE_CONFIG_OPTS} -DCHECK_VALGRIND=OFF
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

all:
	@cmake ${CMAKE_CONFIG_OPTS} -DBUILD_TCL=ON -DBUILD_PYTHON=ON -DBUILD_VERILOG=ON -DCHECK_VALGRIND=ON
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

simple:
	@cmake ${CMAKE_CONFIG_OPTS} -DBUILD_TCL=OFF -DBUILD_PYTHON=OFF -DBUILD_VERILOG=OFF -DCHECK_VALGRIND=OFF
	@cmake ${CMAKE_CONFIG_OPTS} -L | grep -v "^--"

# maintainance targets

keytrans: check-config
	@make ${BUILD_CONFIG_OPTS} keytrans

# EOF
