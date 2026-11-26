# [M1 DiPaQ] High-Performance Computing on Multicore Architectures

**Instructor:** Oguz Kaya (oguz.kaya[at]universite-paris-saclay.fr), Université Paris-Saclay and LISN

This course is the follow-up of **[M1 DiPaQ] Introduction to Parallel Algorithms and
Programming**, but the dependency is light: only one and a half of its seven sessions are
needed here (half a session on SIMD, one session on OpenMP). (i) Reminders will be given at
the beginning of the sessions that need them, and (ii) the lecture notes of these sessions
are given to the students who did not follow that course, so that they can catch up: they
must read them before coming to sessions 5 and 6 in particular.

A straightforward program uses a tiny part of a modern multicore processor: a matrix
product written with three nested loops reaches about one thousandth of what the
processor can do, and a well-optimized one close to 90%. Getting the rest back is the
focus of a whole discipline, **high-performance computing (HPC)**, and the subject of
this course.

It starts with understanding how the processor works: we open **one CPU core** and go
over its units one by one. We then learn to write programs that use all of it, one layer
at a time: **sequential code optimization**, the **memory hierarchy** and its caches,
**SIMD programming** with AVX2, and **multicore programming** with OpenMP. The matrix
product follows us through the sessions, from its three nested loops to a version that
gets close to the peak performance of the machine.

## Course organization

We will have seven sessions of 3 hours each, on Thursday, from 9:00 to 12:00. Sessions 1
and 2 are lectures only. From session 3 on, each session starts with a lecture (CM) and
continues with a programming lab (TP), about half of the session each. It is normal not to
finish the lab in class: you finish it at home and send it to me (cf. Evaluation). A lab may
also contain written exercises, such as the performance analysis of a loop; they are sent
with the coding exercises of the same lab. There are no separate tutorial (TD) sessions.
Mini-exams take place from 12:00 to 12:30 after sessions 4 to 7 (cf. below). Please bring
your laptop to every session: everything runs in the browser, there is nothing to install
(cf. Technical tips).

The plan of the sessions is tentative and may shift as we go:

- **Session 1:** The anatomy of a CPU core architecture
- **Session 2:** CPU core architecture (cont.); sequential code optimization, part 1
- **Session 3:** Sequential code optimization, part 2 (TP1)
- **Session 4:** The memory architecture and memory optimizations (TP2, + mini-exam 1)
- **Session 5:** Advanced SIMD programming with AVX2 (TP3, + mini-exam 2)
- **Session 6:** Multicore architecture optimizations; multithreading with OpenMP (TP4, +
  mini-exam 3)
- **Session 7:** Advanced OpenMP: task parallelism (TP5, + mini-exam 4)

## Course material

- **Lecture notes:** each session has a chapter in `course/lecture-notes.pdf`, which
  explains everything in detail, with figures, examples, and questions with their
  answers.
- **Slides:** in `course/cmN-<topic>/`, with the example programs of the lecture in
  `src/`. The slides are derived from the lecture notes and do not hold everything.
- **Labs:** in `lab/tpN/`: the handout, the skeleton code in `skeleton/` (one file per
  exercise) and, once posted, the solutions in `solution/` (code and slides).

## Evaluation

### Graded lab assignments

Each lab must be submitted by e-mail to `oguz.kaya[at]universite-paris-saclay.fr` by
**23:59:59 on the Tuesday following the lab session**, with the e-mail subject

```
M1DIPAQHPC LABX SURNAME(s) Name(s)
```

(replace X by the lab number). Please follow this format for the e-mail subject
**exactly to the letter**; it is what lets me sort the submissions automatically.

- Attach only the source code files (`.cpp`) and, for the written exercises, a plain text
  file (`.txt`, not a Word document), **one file per exercise**, named as in the skeleton
  (e.g. `peak.txt`, `dot.cpp`, `sums.cpp`, `split.cpp` for lab 1), directly attached to
  the e-mail: no zip archive, no folder.
- Answers to the questions of the assignment (timings, speedups, explanations, and the
  written exercises) go as comments at the top of the corresponding source file, in the
  block provided for this purpose in the skeleton, or in the text file of a written
  exercise.
- **One** of the five lab assignments will be selected at random and graded, giving up to
  **1/20** points. Submit all of them: you do not know which one will be graded.
- If you do not follow the e-mail subject format in at least one of your submissions,
  you will lose half a point (0.5/20).

### Solutions

Solutions to the labs are posted on Wednesday, the day after the deadline and before the
class on Thursday. You are expected to compare them with your own work beforehand and to
come to class with your questions, if any, for when we discuss them.

### End-of-session mini-exams

Four written mini-exams of 30 minutes are scheduled, from 12:00 to 12:30 after sessions 4,
5, 6 and 7. Together they give the remaining **19/20** points, with equal weights:
**4.75/20** each (19/4). If a session has to take place remotely (because of national
strikes, for instance), its mini-exam is postponed to the following week and held together
with the mini-exam of that week. If we have fewer than four mini-exams in the end (session 7
might not take place, for instance), the grade is neutralized: with X mini-exams, each one
gives **19/X** points. Each mini-exam covers the material of all sessions up to that
week's session, excluding that week's session: for instance, mini-exam 1 (session 4)
covers the material of sessions 1 to 3, and mini-exam 2 (session 5) covers sessions 1 to
4. Questions are of the following types:

- *Debugging:* given a complete code with multiple bugs, identify each bug.
- *Output:* given a complete code, find out what the program could potentially display.
- *Coding:* a skeleton code or a function signature is given for a specific task; write
  the code.
- *Course concepts:* small questions requiring interpretation of the concepts learned.
- *Code deciphering:* given a complete code (with anonymized variable names), understand
  and explain what it computes.
- *Code performance analysis:* given a sequence of instructions (in a loop, for example),
  evaluate its performance and identify its bottlenecks (similar to what we do in the
  lectures and the labs).

You can bring **four** A4 sheets (8 pages, two-sided) to the mini-exam, with whatever you
want on them; no other material is allowed.

## Technical tips

### The lab machines: MyDocker

The labs run on the university's servers, through **MyDocker** (Université Paris-Saclay),
in the environment *VSCode et C++*: Visual Studio Code in your browser, with a terminal
and a recent `g++`. If the environment changes during the trimester, you will be informed.
Use Chrome, Brave or Firefox: the interface might not work in Safari.
To launch your machine:

1. Go to <https://mydocker.gitlab.dsi.universite-paris-saclay.fr> and launch the
   environment *VSCode et C++* from there.
2. Log in with your Université Paris-Saclay credentials.
3. On the next page, click on "Request an environment" for the image *VSCode et C++*. A
   button "Connect to the interface" appears, with a password below it.
4. Click on "Connect to the interface": Visual Studio Code opens in a new window of your
   browser. It asks for the password: copy the one shown below the button.
5. Your machine stops after 3 h 30, and the connection drops: before then, click on
   "Reset the timer" to give it 3 h 30 again.

The first time, get the course material onto your machine:

1. Open a terminal in Visual Studio Code: click on the menu icon (three horizontal bars,
   at the top left), then *Terminal*, then *New Terminal*. The terminal appears at the
   bottom of the window.
2. Go to the folder `workspace`, the one that the file explorer shows on the left:

   ```
   cd ~/workspace
   ```

3. Clone this repository there:

   ```
   git clone https://github.com/kayaogz/2627-m1dipaq-highperformancecomputingonmulticorearchitectures.git
   ```

   The course material is now in the folder
   `2627-m1dipaq-highperformancecomputingonmulticorearchitectures` of `workspace`, with
   the skeleton of each lab in `lab/tpN/skeleton`.

The next times, update it from inside this folder with `git pull`: the material and the
labs of the next sessions arrive this way. The folder of the machine has no backup: keep a
copy of your files on your own machine, and save your files for your lab work before the
session expires.

### Compiler Explorer

[Compiler Explorer](https://godbolt.org) (compiler *x86-64 gcc 16.2*) is useful to read the
instructions that the compiler produces for a piece of code, and to try a short code
quickly. The measurements of the labs are done on MyDocker.
