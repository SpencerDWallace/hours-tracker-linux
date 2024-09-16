// to print entire month use -a or 3 as an arg
// alternatively, can pass 1 or 2 as an arg to select the pay period
// month and year (MM/YY) can also be passed as an arg
// ex: ./foo MM/YY -a | ./foo MM/YY 1 | ./foo MM/YY 2
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

using namespace std;

//day of the month to use for middle of the pay period
#ifndef MID
#define MID 15
#endif

//path to directory containing all hours data, relative to directory of this program
#ifndef HT_OUT_DIR
#define HT_OUT_DIR "./hours"
#endif

bool PRINTALL{false};

void parseFile(const string& y, const string& m, const bool& pp);
bool checkValidMonthAndYear(const string& input, string& month, string& year);
void readMonthAndYear(string& m, string& y);
void readPayPeriod(int& p);

int main(int argc, char** argv)
{
  string year;
  string month;
  int    payPeriod{-1};
  bool   monthAndYearFromArgs{false};
  //read any command args
  if (argc > 1)
  {
    //check if -a is passed as an arg
    if ((string(argv[1]) == "-a" || string(argv[1]) == "3") || (argc > 2 && (string(argv[2]) == "-a" || string(argv[2]) == "3")))
    {
      PRINTALL = true;
    }
    //check if pay cycle is passed as an arg
    else if ((string(argv[1]) == "1" || string(argv[1]) == "2") || (argc > 2 && (string(argv[2]) == "1" || string(argv[2]) == "2")))
      payPeriod = (string(argv[1]) == "1" || string(argv[2]) == "2") ? stoi(argv[1]) - 1 : stoi(argv[2]) - 1;
    //check if month and year are passed as an arg
    if (string(argv[1]).length() == 5 || (argc > 2 && string(argv[2]).length() == 5))
    {
      monthAndYearFromArgs = (string(argv[1]).length() == 5) ?
                               checkValidMonthAndYear(argv[1], month, year) :
                               checkValidMonthAndYear(argv[2], month, year);
    }
  }

  if (!monthAndYearFromArgs)
    readMonthAndYear(month, year);
  if (!PRINTALL && payPeriod == -1) // only get input from user if not printing all
    readPayPeriod(payPeriod);
  parseFile(year, month, payPeriod);

  return 0;
}

/**
Calculates the minutes between two times given as strings
@param earlier: preferably the earlier time of the two input strings
@param later: preferably the later time of the two input strings
**/
int calcMinutesFromTimes(const string& earlier, const string& later)
{
  int h1  = stoi(earlier.substr(0, 2));
  int m1  = stoi(earlier.substr(3, 2));
  int h2  = stoi(later.substr(0, 2));
  int m2  = stoi(later.substr(3, 2));
  int dif = (h2 - h1) * 60 + (m2 - m1);
  return (dif < 0) ? dif + (24 * 60) : dif;
}

/**
Takes a map of the following pattern: hour as key and minutes as value
and iterates over it, printing the associations in order.
@param hours: container holding hour-to-minutes pairs
**/
void printHours(const map<int, int>& hours)
{
  cout << "\n"
       << setw(10) << "day"
       << "|hours\n"
       << "--------------------------\n";
  for (const auto& day : hours)
  {
    cout << setw(11) << day.first << day.second / 60 << ":" << setfill('0')
         << setw(2) << right << day.second % 60 << "\n"
         << left << setfill(' ');
  }
}

/**
Finds the file that should contain the data for this pay cycle by
building a path from the user's input. Parses through this file to
generate an output for the clock-in and clock-out times of each day
as well as the total hours worked each day.
@param y: The year from user input
@param m: The month from user input
@param pp: The pay cycle that the user is interested in (false = 1-15, true = 16-end)
**/
void parseFile(const string& y, const string& m, const bool& pp)
{
  map<int, int> hours; // key is day, value is minutes

  // check env override for hours dir
  std::string hours_dir = HT_OUT_DIR;
  char*       temp_hours_dir;
  if ((temp_hours_dir = getenv("HT_OUT_DIR")))
    hours_dir = temp_hours_dir;

  string   file{hours_dir + "/" + y + "/" + m + ".txt"};
  ifstream data(file);
  if (data.is_open())
  {
    cout << "\n";
    char   current{'i'}; // clock in time (i) or clock out time (o)
    char   last{'*'}; // track whether last line was clock in or clock out
    string clockin;
    string clockout;
    cout << setfill(' ') << setw(10) << left << "day" << setw(15) << "|clock-in"
         << "|clock-out\n"
         << "--------------------------------------------\n";
    string line;
    while (getline(data, line))
    {
      int ptr{0};
      if (line[ptr++] == current)
      {
        int day{0};
        while (line[ptr++] != '#')
          day++; // length of day at this point

        // use length from above to set actual day value
        day = stoi(line.substr(ptr - day - 1, day));
        // day is not in pay cycle of interest, ignore if PRINTALL is true
        if (!PRINTALL && ((!pp && day > MID) || (pp && day <= MID)))
          continue;
        string hourAndMin = line.substr(ptr, 5);
        if (current == 'i')
        {
          current = 'o';
          clockin = hourAndMin;
        }
        else
        {
          current  = 'i';
          clockout = hourAndMin;
          cout << setw(11) << left << day << setw(15) << clockin << clockout
               << "\n";
          hours[day] += calcMinutesFromTimes(clockin, clockout);
        }
      }
    }
    data.close();
    printHours(hours);
  }
  else
    cout << "Unable to open file";
}

/**
Checks if the input follows the format of MM/YY
@param input: input to get month and year from, if valid
@param month: string to hold value for month
@param year: string to hold value for year
**/
bool checkValidMonthAndYear(const string& input, string& month, string& year)
{
  int ind{0};
  if (input.length() != 5 || input[2] != '/')
  {
    cout << "Bad month and year entered, please reenter.\n";
    return false;
  }
  while (input[ind] != '/' && ind < input.length())
    ind++;

  month = input.substr(ind - 2, 2);
  year  = input.substr(ind + 1, 2);
  if (stoi(year) >= 0 && stoi(year) < 100 && stoi(month) > 0 &&
      stoi(month) < 13)
  {
    cout << "Year is: " << year << " | "
         << "Month is: " << month << "\n";
    return true;
  }
  month.clear();
  year.clear();
  return false;
}

/**
Requests input from the user to generate the month and year
to build the path for the text file containing the data of interest
@param month: string to hold value for month
@param year: string to hold value for year
**/
void readMonthAndYear(string& month, string& year)
{
  while (true)
  {
    string input{};
    cout << "Please enter the month and year (as MM/YY): ";
    cin >> input;
    if (checkValidMonthAndYear(input, month, year))
      return;
  }
}

/**
Requests input from the user to flag which pay cycle they are interested in
@param p: flag to mark which pay cycle to generate output for
**/
void readPayPeriod(int& p)
{
  cout << "Please enter the pay cycle (1 for 1-15, 2 16-end, or 3 for both): ";
  while (true)
  {
    cin >> p;
    p--; // to use as a bool later (1-->0, 2-->1)
    if (p == 0 || p == 1)
    {
      return;
    }
    else if (p == 2)
    {
      PRINTALL = true;
      return;
    }
    cout << "Invalid input, please enter 1 or 2 (1 for 1-15, 2 for 16-end of "
            "month): ";
  }
}
