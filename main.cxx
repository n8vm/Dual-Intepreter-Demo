#include <thread>
#include <Python.h>

int main()
{
    Py_InitializeEx(1);
    PyEval_InitThreads();

    auto main_state = PyThreadState_Get();

    auto s1_state = Py_NewInterpreter();
    PyThreadState_Swap(main_state);

    auto A = [] (PyInterpreterState *interp)
    {
        PyEval_AcquireLock();
        auto thread_state = PyThreadState_New(interp);
        auto thread_state_swap = PyThreadState_Swap(thread_state);
        
        // PyRun_SimpleString("for i in range(0, 1000): print('I am subinterpreter A')");
        PyRun_InteractiveLoop(stdin, "<stdin>");

        PyThreadState_Swap(thread_state_swap);
        PyThreadState_Clear(thread_state);
        PyThreadState_Delete(thread_state);
        PyEval_ReleaseLock();
    };

    std::thread t1(A, s1_state->interp);

    PyRun_SimpleString("import QtGUI");

    auto _state = PyEval_SaveThread();

    t1.join();

    std::getchar();

    auto s1_state_swap = PyThreadState_Swap(s1_state);
    Py_EndInterpreter(s1_state);
    PyThreadState_Swap(s1_state_swap);

    PyEval_RestoreThread(_state);
    Py_Finalize();
    return 0;
}