#include <iostream>
#include <pthread.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <random>
using namespace std;

long long int cpu_counts;
long long int toss;
long long int circle_count = 0;
// __m128 v_circle_count = _mm_set_ps(0.f,0.f,0.f,0.f);
pthread_mutex_t mutex_mon;

void *MonteCarlo(void *param);
 
int main (int argc, const char * argv[]) {
    long long int iter_per_thread;
    double pi;
    cpu_counts = atoi(argv[1]);
    toss = atoll(argv[2]);
    iter_per_thread = toss/ cpu_counts;
    pthread_t MonteCarlos[cpu_counts];
     pthread_mutex_init(&mutex_mon,NULL);
 
    for (int i = 0; i < cpu_counts; i++)
        pthread_create(&MonteCarlos[i], NULL, MonteCarlo, &iter_per_thread);     
    for (int i = 0; i < cpu_counts; i++)
        pthread_join(MonteCarlos[i],NULL);
    
    /*for(int j=0;j<4;j++){
        circle_count+=v_circle_count[j];
    }*/
    pi = 4.0 * circle_count / toss;
    // printf("Pi = %f\n",); 
		cout << pi << endl;
    return 0;
}
 
void *MonteCarlo(void *param)
{
    long long int iter;
    iter = *((long long int *)param);
    long long int incircle_count = 0;
    const __m128 one =  _mm_set_ps(1.f,1.f,1.f,1.f);
    const __m128 two =  _mm_set_ps(2.f,2.f,2.f,2.f);
    const __m128 randmax = _mm_set_ps((double)RAND_MAX, (double)RAND_MAX, (double)RAND_MAX, (double)RAND_MAX);
    // double *x = new double[4];
    // double *y = new double[4];
    unsigned int seed = 10;
    __m128 simd_x;
    __m128 simd_y;
    __m128 incircle_mask = _mm_set_ps(0.f,0.f,0.f,0.f);
    for (register long long int i = 0; i < iter; i+=4) {
        /*x = Rand(0.0, 1.0);
        y = Rand(0.0, 1.0);*/
        /*for(int j=0;j<4;j++)
        {
    	      x[j] = rand_r(&seed); //thread-safe random number generator
            y[j] = rand_r(&seed); //thread-safe random number generator
        }*/
        // x = Rand(0.0, 1.0) * 2.0 - 1.0;
        // y = Rand(0.0, 1.0) * 2.0 - 1.0;
        // __m256 simd_x = _mm256_loadu_ps(x);
        // __m256 simd_y = _mm256_loadu_ps(y);
        // __m128 sum = _mm_add_ps( *simd_x, *simd_y );
        simd_x = _mm_set_ps(rand_r(&seed), rand_r(&seed), rand_r(&seed), rand_r(&seed));
        simd_y = _mm_set_ps(rand_r(&seed), rand_r(&seed), rand_r(&seed), rand_r(&seed));
        simd_x = _mm_div_ps(simd_x, randmax);
        simd_y = _mm_div_ps(simd_y, randmax);
        // simd_x = _mm_sub_ps(_mm_mul_ps(simd_x, two), one);
        // simd_y = _mm_sub_ps(_mm_mul_ps(simd_y, two), one);
        // simd_x = _mm_fmsub_ps(simd_x, two, one);
        // simd_y = _mm_fmsub_ps(simd_y, two, one);
        __m128 ans = _mm_add_ps(_mm_mul_ps( simd_x, simd_x ),_mm_mul_ps( simd_y, simd_y));
        
        ans = _mm_cmplt_ps( ans, one);
        ans = _mm_and_ps(_mm_set1_ps(1.0f), ans);
        incircle_mask = _mm_add_ps(incircle_mask, ans);
        
    }
    for(int j=0;j<4;j++){
		// printf("for = %d\n", (int)incircle_mask[j]);       
    incircle_count+=(long long int)incircle_mask[j];
    }
    // printf("incircle_count = %d\n", incircle_count);
    // int index[4] = {0,1,2,3};
    // int tmp = _mm_cvt_roundss_si32(incircle_mask, index);
    // tmp_count+=tmp;
    // printf("mm_mask = %d\n", tmp_count);
 
    pthread_mutex_lock(&mutex_mon);
    circle_count += incircle_count;
    // v_circle_count = _mm_add_ps(v_circle_count, incircle_mask);
    pthread_mutex_unlock(&mutex_mon);
     
    pthread_exit(0);
}