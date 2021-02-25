rule1: 
	g++-10 -pthread -o pi.out pi.cpp -O3 -march=corei7 -ftree-vectorize -ffast-math -mfma -mavx -msse
rule2:
	g++ -pthread -o pi_pthread.out pi_pthread.cpp -O3
rule3:
	g++ -pthread -o pi_newrand.out pi_newrand.cpp -O3 -march=corei7 -ftree-vectorize -ffast-math -mfma -mavx -msse
	# clang++-11 pi_newrand.cpp -I./common -O3 -march=corei7 -ftree-vectorize -std=c++17 -Wall -pthread -ffast-math -o pi_newrand.out
rule4:
	g++ -pthread -o pi_reantrant.out pi_reantrant.cpp -O3
	# clang++-11 pi_reantrant.cpp -I./common -O3 -std=c++17 -Wall -S -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -mavx2 -ffast-math -c $< -o assmebly.s
	# clang++-11 pi_reantrant.cpp -I./common -O3 -std=c++17 -Wall -pthread -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -mavx2 -ffast-math -o pi_reantrant.out
rule5:
	g++ -pthread -o pi_test.out pi_test.cpp -O3

clean:
	rm *.out