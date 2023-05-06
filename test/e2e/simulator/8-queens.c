// RUN: %gcc %s -o %t
// RUN: %simulator --cosim %t | %fc %s

typedef int bool;

#define TRUE 1
#define FALSE 0
#define MAXCANDIDATES 100 /* max possible next extensions */
#define NMAX 100          /* maximum solution size */

typedef int data; /* type to pass data to backtrack */

int solution_count; /* how many solutions are there? */

bool finished = FALSE; /* found all solutions yet? */

bool is_a_solution(int arr[], int k, int n) { return (k == n); }

void process_solution(int arr[], int b) { solution_count++; }

void construct_candidates(int a[], int k, int n, int c[], int *ncandidates);

void backtrack(int a[], int k, data input) {
  int c[MAXCANDIDATES]; /* candidates for next position */
  int ncandidates;      /* next position candidate count */
  int i;                /* counter */

  if (is_a_solution(a, k, input))
    process_solution(a, k);
  else {
    k = k + 1;
    construct_candidates(a, k, input, c, &ncandidates);
    for (i = 0; i < ncandidates; i++) {
      a[k] = c[i];
      backtrack(a, k, input);
      if (finished)
        return; /* terminate early */
    }
  }
}

/*  What are possible elements of the next slot in the 8-queens
 *    problem?
 *    */

int abs(int v) { return v > 0 ? v : -v; }

void construct_candidates(int a[], int k, int n, int c[], int *ncandidates) {
  int i, j;        /* counters */
  bool legal_move; /* might the move be legal? */

  *ncandidates = 0;
  for (i = 1; i <= n; i++) {
    legal_move = TRUE;
    for (j = 1; j < k; j++) {
      if (abs((k)-j) == abs(i - a[j])) /* diagonal threat */
        legal_move = FALSE;
      if (i == a[j]) /* column threat */
        legal_move = FALSE;
    }
    if (legal_move == TRUE) {
      c[*ncandidates] = i;
      *ncandidates = *ncandidates + 1;
    }
  }
}

int main() {
  int a[NMAX];

  for (int i = 1; i <= 8; i++) {
    solution_count = 0;
    backtrack(a, 0, i);
  }

  // CHECK: NUM=3709836
  // CHECK: M[0x110003d4]=0x0000005d
  // CHECK: PC=0x000103b8
  solution_count += 1;

  // CHECK: NUM=3709837
  // CHECK: PC=0x000103bc
  asm("ecall");
}

/*
 *    8-queens.c
 *      Solve the eight queens problem using backtracking
 *
 *        begun: March 1, 2002
 *          by: Steven Skiena
 *          */

/*
 * Copyright 2003 by Steven S. Skiena; all rights reserved.
 *
 * Permission is granted for use in non-commerical applications
 * provided this copyright notice remains intact and unchanged.
 *
 * This program appears in my book:
 *
 * "Programming Challenges: The Programming Contest Training Manual"
 * by Steven Skiena and Miguel Revilla, Springer-Verlag, New York 2003.
 *
 * See our website www.programming-challenges.com for additional information.
 *
 * This book can be ordered from Amazon.com at
 *
 * http://www.amazon.com/exec/obidos/ASIN/0387001638/thealgorithmrepo/
 *
 * */
