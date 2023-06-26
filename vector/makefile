 
all:
	make simd
	make avx
 
simd:
	g++ -ggdb3 -o simd -mavx512f -O2 simd.cpp
 
avx:
	g++ -ggdb3 -o avx -mavx -mavx2 -O2 avx.cpp
 
clean:
	rm -rf simd avx
