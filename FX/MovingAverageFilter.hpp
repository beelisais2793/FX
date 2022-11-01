#define MAX_DATA_POINTS 20

class MovingAverageFilter
{
public:
  //construct without coefs
  MovingAverageFilter(unsigned int newDataPointsCount);

  DspFloatType process(DspFloatType in);

private:
  DspFloatType values[MAX_DATA_POINTS];
  int k; // k stores the index of the current array read to create a circular memory through the array
  int dataPointsCount;
  DspFloatType out;
  int i; // just a loop counter
};


MovingAverageFilter::MovingAverageFilter(unsigned int newDataPointsCount)
{
  k = 0; //initialize so that we start to write at index 0
  if (newDataPointsCount < MAX_DATA_POINTS)
    dataPointsCount = newDataPointsCount;
  else
    dataPointsCount = MAX_DATA_POINTS;

  for (i = 0; i < dataPointsCount; i++)
  {
    values[i] = 0; // fill the array with 0's
  }
}

DspFloatType MovingAverageFilter::process(DspFloatType in)
{
  out = 0;

  values[k] = in;
  k = (k + 1) % dataPointsCount;

  for (i = 0; i < dataPointsCount; i++)
  {
    out += values[i];
  }

  return out / dataPointsCount;
}