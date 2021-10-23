#include <fstream>
#include <iostream>
#include <math.h>
#include <omp.h>
#include <string>
#include <vector>

using namespace std;

long int threadCount;
long int binCount;
char *fileName;
ifstream readInput;
vector<float> floatingPoints;
int binPerThread;
int floatCount;
vector<int> histogram;


void parallelize() {

  vector<vector<int>> local(threadCount, vector<int>(binCount, 0));
  float interval = 20.0 / binCount;

#pragma omp parallel for num_threads(threadCount)
  for (int i = 0; i < floatCount; i++) {
    local[omp_get_thread_num()][floor(floatingPoints[i] / interval)]++;
  }

  for (int b = 0; b < binCount; b++) {
    histogram.push_back(0);
  }

#pragma omp parallel for num_threads(threadCount)
  for (int b = 0; b < binCount; b++) {
    for (int t = 0; t < threadCount; t++) {
      histogram[b] += local[t][b];
    }
  }

}

void printResult() {
  for (int b = 0; b < binCount; b++) {
    cout << "bin[" << b << "] = " << histogram[b] << endl;
  }
}

void readFloatingPoints() {
  readInput.open(fileName);

  float a;
  readInput >> floatCount;
  for (int i = 0; readInput >> a; i++) {
    floatingPoints.push_back(a);
  }
}

void processArgs(int argc, char *argv[]) {
  char *bEnd;
  char *tEnd;

  binCount = strtol(argv[1], &bEnd, 10);
  threadCount = strtol(argv[2], &tEnd, 10);
  fileName = argv[3];
}

int main(int argc, char *argv[]) {

  processArgs(argc, argv);
  readFloatingPoints();
  parallelize();
  printResult();

  return 0;
}
