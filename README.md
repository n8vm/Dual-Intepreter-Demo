# Dual-Intepreter-Demo
A small example showing how two interpreters can be used together

This project is taking advantage of the fact that although subinterpreters don't share global variables, they do share the same process, and so they share memory from any linked libraries. If a C function is bound to python and imported by both subinterpreters, data used by that C function can be shared between the two interpreters.

This can be helpful if you wanted to run a GUI while simultaneously having access to a python interpreter.
