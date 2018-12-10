#include <iostream>

using namespace std;

int main(int argc, const char *argv[])
{
  int *thing = new int[24];

  delete [] thing;

  cout << "Hi there!" << endl;
  cout << "argc = ," << argc << endl;
  for (int ii = 0; ii < argc; ++ii)
  {
    cout << "argv[" << ii << "] = " << argv[ii] << endl;
  }

  cout << "Press ENTER to continue...";
  cin.ignore();
  return 0;
}
