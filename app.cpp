#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <iomanip>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <termios.h>

using namespace std;
using namespace std::chrono;

class ClockApplication
{
public:
    int validateTime(int hour, int minute)
    {
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59)
        {
            return 0;
        }
        return 1;
    }

    void setAlarm(int alarmHour, int alarmMinute)
    {
        cout << "Alarm Set at " << alarmHour << ":"<< alarmMinute << "\n" << endl;
        pid_t pid = fork();
        if (pid == 0)
        {
            while (true)
            {
                time_t currentTime = time(nullptr);

                struct tm *currentDate = localtime(&currentTime);
                int currentHour = currentDate->tm_hour;
                int currentMinute = currentDate->tm_min;

                if (currentHour > alarmHour || (currentHour == alarmHour && currentMinute >= alarmMinute))
                {
                    cout << "Alarm: Time's Up..!" << endl;
                    exit(0);
                }                
                sleep(1);
            }
        }
    }

    int kbhit(void)
    {
        static const int STDIN = 0;
        static bool initialized = false;

        if (!initialized)
        {
            termios term;
            tcgetattr(STDIN, &term);
            term.c_lflag &= ~ICANON;
            tcsetattr(STDIN, TCSANOW, &term);
            setbuf(stdin, NULL);
            initialized = true;
        }

        int bytesWaiting;
        ioctl(STDIN, FIONREAD, &bytesWaiting);
        return bytesWaiting;
    }

    void printTimer(int hours, int minutes, int seconds)
    {
        std::cout << std::setfill('0') << std::setw(2) << hours << ":"
                  << std::setfill('0') << std::setw(2) << minutes << ":"
                  << std::setfill('0') << std::setw(2) << seconds << "\r" << std::flush;
    }

    void stopwatch()
    {
        cout << "Press Enter to terminate." << endl;
        int hours = 0, minutes = 0, seconds = 0;

        while (true)
        {
            if (kbhit())
            {
                char key = getchar();
                if (key == 10)
                {
                    cout << "Stopwatch Terminated." << "\n" << endl;
                    break;
                }
            }
            printTimer(hours, minutes, seconds);
            seconds++;
            if (seconds == 60)
            {
                seconds = 0;
                minutes++;
                if (minutes == 60)
                {
                    minutes = 0;
                    hours++;
                    if (hours == 24)
                    {
                        hours = 0;
                    }
                }
            }
            sleep(1);
        }
    }

    void timer(int hours, int minutes, int seconds)
    {
        cout << "Press Enter to terminate the timer." << endl;

        while (true)
        {
            if (kbhit())
            {
                char key = getchar();

                if (key == 10)
                {
                    cout << "Process terminated..!" << endl;
                    return;
                }
            }

            printTimer(hours, minutes, seconds);

            if (hours == 0 && minutes == 0 && seconds == 0)
            {
                cout << "\nTimer reached zero. Exiting the application." << endl;
                return;
            }

            if (seconds == 0)
            {
                if (minutes == 0)
                {
                    if (hours == 0)
                    {
                        break; 
                    }
                    hours--;
                    minutes = 59;
                }
                else
                {
                    minutes--;
                }
                seconds = 59;
            }
            else
            {
                seconds--;
            }
            sleep(1);
        }
    }
};

int main()
{
    ClockApplication clockapp;
    while (1)
    {
        cout << "Available tasks : " << endl;
        cout << "1. Alarm \n"
             << "2. Stop Watch \n"
             << "3. Timer \n"
             << "4. Exit \n" << endl;
        cout << "Select task : " << endl;

        int choice;
        cin >> choice;
        if (choice == 1)
        {
            cout << "Enter the alarm time (HH MM): " << endl;
            int alarmHour, alarmMinute;
            cin >> alarmHour >> alarmMinute;

            if (!clockapp.validateTime(alarmHour, alarmMinute))
            {
                cout << "Invalid input..!" << endl;
            }

            clockapp.setAlarm(alarmHour, alarmMinute);
        }
        else if (choice == 2)
        {
            clockapp.stopwatch();
        }
        else if (choice == 3)
        {
            int inputHours, inputMinutes, inputSeconds;

            cout << "Enter timer duration (HH MM SS): " << endl;
            cin >> inputHours >> inputMinutes >> inputSeconds;
            clockapp.timer(inputHours, inputMinutes, inputSeconds);
        }
        else if (choice == 4)
        {
            cout << "Terminated..." << endl;
            break;
        }
        else
        {
            cout << "Invalid Choice..!" << endl;
        }
    }
}