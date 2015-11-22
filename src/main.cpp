// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <memory>
#include <unistd.h>
#include <csignal>

// Qt headers
#include <QApplication>

// OpenGL related headers
#include <GL/glew.h>

// Internal headers
#include "math.hpp"
#include "shader.hpp"
#include "mainwindow.h"


using namespace std;

extern "C" {
    void initialize_window();
    void terminate();
    bool is_running();
    void plot_binary( PyObject* pybuffer, PyObject* var_name, int buffer_width_i,
            int buffer_height_i, int channels, int type, int step);
    void update_plot( PyObject* pybuffer, PyObject* var_name, int buffer_width_i,
            int buffer_height_i, int channels, int type, int step);
}

MainWindow* wnd;
bool is_running_ = false;

bool is_running() {
    return is_running_;
}

void update_plot( PyObject* pybuffer, PyObject* var_name, int buffer_width_i,
        int buffer_height_i, int channels, int type, int step )
{
    PyObject *var_name_bytes = PyUnicode_AsEncodedString(var_name, "ASCII", "strict");
    string var_name_str = PyBytes_AS_STRING(var_name_bytes);

    BufferRequestMessage request;
    request.var_name_str = var_name_str;
    request.py_buffer = pybuffer;
    request.width_i = buffer_width_i;
    request.height_i = buffer_height_i;
    request.channels = channels;
    request.type = type;
    request.step = step;

    while(wnd == nullptr)
        usleep(1/30*1e6);

    wnd->plot_buffer(request);
}

void plot_binary( PyObject* pybuffer, PyObject* var_name, int buffer_width_i,
        int buffer_height_i, int channels, int type, int step)
{
    PyObject *var_name_bytes = PyUnicode_AsEncodedString(var_name, "ASCII", "strict");
    string var_name_str = PyBytes_AS_STRING(var_name_bytes);

    BufferRequestMessage request;
    request.var_name_str = var_name_str;
    request.py_buffer = pybuffer;
    request.width_i = buffer_width_i;
    request.height_i = buffer_height_i;
    request.channels = channels;
    request.type = type;
    request.step = step;

    while(wnd == nullptr)
        usleep(1/30*1e6);

    wnd->plot_buffer(request);
}

void signalHandler( int signum )
{
    cout << "SIGNAL (" << signum << ") received.\n";
}

void initialize_window() {
    char* argv[] = { "GDB ImageWatch", NULL };
    int argc = 1;
    sighandler_t gdb_sigchld_handler = std::signal(SIGCHLD, signalHandler);

    QApplication app(argc, &argv[0]);
    MainWindow window;
    window.show();
    wnd = &window;
    is_running_ = true;

    // Restore GDB SIGCHLD handler
    std::signal(SIGCHLD, gdb_sigchld_handler);

    app.exec();

    is_running_ = false;
}

void terminate() {
    QApplication::exit();
}
