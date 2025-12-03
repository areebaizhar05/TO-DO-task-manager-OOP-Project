FIRST TYPE THIS IN TERMINAL TO RUN OUR APP ON GUI: 
g++ -I <path-to-ur-include> -L <path-to-ur-lib> SFML_App.cpp AuthSystem.cpp User.cpp SubTask.cpp SimpleSubTask.cpp RecurringSubTask.cpp Character.cpp JSONHandler.cpp Task.cpp -o final_app.exe -lsfml-graphics -lsfml-window -lsfml-systemg++ -I "path to include folder" -L "path to library folder" SFML_App.cpp AuthSystem.cpp User.cpp SubTask.cpp SimpleSubTask.cpp RecurringSubTask.cpp Character.cpp JSONHandler.cpp -o final_app.exe -lsfml-graphics -lsfml-window -lsfml-system

THEN TYPE THIS:
./final_app.exe

TASK-TRACKER
Overview:
The Task-Tracker is a graphical desktop application built in C++ (OOP) using the SFML library. It helps you manage personal and professional tasks by providing a visual dashboard and progress feedback from a dynamic companion character.

Key Features:
->Dynamic Visual Feedback: A character on the dashboard reflects your overall progress via changing Emotions (from Sad to Excited).

->Smart Priority: Activities are automatically assigned Priority (HIGH, MEDIUM, LOW, OVERDUE) based on the Due Date you set.

->Color-Coded Status: Tasks are color-coded on the dashboard based on their priority (e.g., Red for HIGH/OVERDUE).

->Hierarchical Management: Organize large tasks (Activities) into smaller steps (Subtasks).

->Persistence: All user data, tasks, and status changes are automatically saved to and loaded from data.json.

->User Login: Supports secure user Sign Up and Login sessions.