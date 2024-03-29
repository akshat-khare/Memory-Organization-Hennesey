#include <stdio.h>
#include <time.h>
#define ARRAY_MIN (1024) /* 1/4 smallest cache */
#define ARRAY_MAX (4096*4096) /* 1/4 largest cache */
int x[ARRAY_MAX]; /* array going to stride through */
int label(int i) {/* generate text labels */
  if (i<1e3) printf("%1dB,",i);
  else if (i<1e6) printf("%1dK,",i/1024);
  else if (i<1e9) printf("%1dM,",i/1048576);
  else printf("%1dG,",i/1073741824);
  return 0;
}
int main(int argc, char* argv[]) {
  int register nextstep, i, index, stride;
  int csize;
  double steps, tsteps,loadtime;
  struct timespec lastsec, sec0, sec1, sec; /* timing variables */
  /* Initialize output */
  // printf("%ld\n", sizeof(int));
  printf(" ,");
  for (stride=1; stride <= ARRAY_MAX/2; stride=stride*2)
    label(stride*sizeof(int));
  printf("\n");
  /* Main loop for each configuration */
  for (csize=ARRAY_MIN; csize <= ARRAY_MAX; csize=csize*2) {
    label(csize*sizeof(int)); /* print cache size this loop */
    for (stride=1; stride <= csize/2; stride=stride*2) {
  //    /* Lay out path of memory references in array */
      // label(stride*sizeof(int)); /* print cache size this loop */
      for (index=0; index < csize; index=index+stride)
        x[index] = index + stride; /* pointer to next */
      x[index-stride] = 0; /* loop back to beginning */
  //    /* Wait for timer to roll over */
     // clock_gettime(CLOCK_MONOTONIC_RAW, &lastsec);
     // do (clock_gettime(CLOCK_MONOTONIC_RAW, &sec0)); while (sec0.tv_sec == lastsec.tv_sec);
  //    /* Walk through path in array for twenty seconds */
  //    /* This gives 5% accuracy with second resolution */
      steps = 0.0; /* number of steps taken */
      nextstep = 0; /* start at beginning of path */
      clock_gettime(CLOCK_MONOTONIC_RAW, &sec0); /* start timer */
      do { /* repeat until collect 20 seconds */
      	for (i=stride;i!=0;i=i-1) { /* keep samples same */
      		nextstep = 0;
		do nextstep = x[nextstep]; /* dependency */
      		while (nextstep != 0);
      	}
  	steps = steps + 1.0; /* count loop iterations */
  	clock_gettime(CLOCK_MONOTONIC_RAW, &sec1); /* end timer */
      } while ((sec1.tv_sec - sec0.tv_sec) < 20.0); /* collect 20 seconds */
      //printf("%f\n", steps);
      sec.tv_sec= sec1.tv_sec - sec0.tv_sec;
      //printf("%ld\n", sec.tv_sec);
      /* Repeat empty loop to loop subtract overhead */
      tsteps = 0.0; /* used to match no. while iterations */
      clock_gettime(CLOCK_MONOTONIC_RAW, &sec0); /* start timer */
      do { /* repeat until same no. iterations as above */
  	    for (i=stride;i!=0;i=i-1) { /* keep samples same */
  	    	index = 0;
  	    	do index = index + stride;
  	    	while (index < csize);
  	    }
  	    tsteps = tsteps + 1.0;
  	    clock_gettime(CLOCK_MONOTONIC_RAW, &sec1); /* - overhead */
     } while (tsteps<steps); /* until = no. iterations */
     sec.tv_sec = sec.tv_sec - (sec1.tv_sec - sec0.tv_sec);
    //  printf("%ld\t", sec.tv_sec);
     loadtime = (sec.tv_sec*1e9)/(steps*csize);
     /* write out results in .csv format for Excel */
     printf("%4.1f,", (loadtime<0.1) ? 0.1 : loadtime);
     } /* end of inner for loop */
     printf("\n");
  } /* end of outer for loop */
    return 0;
}