// Nate VM. Run two subinterpreters which share the same underlying "BridgeLib" memory space 
// This is helpful when trying to connect a PyQt GUI with an interactive interpreter, like Jupyter
#include <thread>
#include <Python.h>

int main()
{
    Py_InitializeEx(1);
    PyEval_InitThreads();

    auto main_state = PyThreadState_Get();
    auto subinterpreter_state = Py_NewInterpreter();
    PyThreadState_Swap(main_state);

    std::thread t([] (PyInterpreterState *interp)
    {
        PyEval_AcquireLock();
        auto thread_state = PyThreadState_New(interp);
        auto thread_state_swap = PyThreadState_Swap(thread_state);
        
        PyRun_SimpleString("import Bridge");
        PyRun_SimpleString("Bridge.set_x(42)");
        PyRun_SimpleString("print('Try calling Bridge.set_x(<value>) or Bridge.get_x()')");
        PyRun_InteractiveLoop(stdin, "<stdin>");

        PyThreadState_Swap(thread_state_swap);
        PyThreadState_Clear(thread_state);
        PyThreadState_Delete(thread_state);
        PyEval_ReleaseLock();
    }, subinterpreter_state->interp);

    #ifdef NDEBUG
        PyRun_SimpleString("import QtGUI");
    #else
        std::cout<<"Error: PyQt5 doesnt seem to work in a debug build of python 3. "<<std::endl;
    #endif

    t.join();

    auto subinterpreter_state_swap = PyThreadState_Swap(subinterpreter_state);
    Py_EndInterpreter(subinterpreter_state);
    PyThreadState_Swap(subinterpreter_state_swap);

    Py_Finalize();
    return 0;
}