O=arts/testpew.zsg

default: run

all:
	cmake -B build
	make -C build -j `python -c 'from multiprocessing import cpu_count; print(cpu_count() * 2)'`

release_all:
	cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/tmp-install
	make -C build -j `python -c 'from multiprocessing import cpu_count; print(cpu_count() * 2)'`

debug_all:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/tmp/tmp-install
	make -C build -j `python -c 'from multiprocessing import cpu_count; print(cpu_count() * 2)'`

configure:
	cmake -B build
	ccmake -B build

test: all
	build/tests/zentest

run: all
	ZEN_OPEN=$O ./run.sh

debug: debug_all
	USE_GDB=1 ZEN_SPROC=1 ZEN_OPEN=$O ./run.sh

.PHONY: all debug_all debug run test configure default
