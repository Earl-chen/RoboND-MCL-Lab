/*
 * @Author: chen_ming 1170517091@qq.com
 * @Date: 2024-02-29 15:11:09
 * @LastEditors: chen_ming 1170517091@qq.com
 * @LastEditTime: 2024-03-01 09:40:37
 * @FilePath: /robot_pose_provider/home/chenming/1_Robot/2_Slam_Nav/1_Ros2/0_project/Down/slam_v2/RoboND-MCL-Lab/test_numpy.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <stdint.h> // <cstdint> requires c++11 support

 

#include <Python.h>
#include <numpy/arrayobject.h>


#if PY_MAJOR_VERSION >= 3
#define PyString_FromString PyUnicode_FromString
#endif

namespace detail {
		static std::string s_backend;

		struct _interpreter {
			PyObject *s_python_function_show;
			PyObject *s_python_function_draw;
			PyObject *s_python_function_pause;
			PyObject *s_python_function_save;
			PyObject *s_python_function_figure;
			PyObject *s_python_function_plot;
			PyObject *s_python_function_semilogx;
			PyObject *s_python_function_semilogy;
			PyObject *s_python_function_loglog;
			PyObject *s_python_function_fill_between;
			PyObject *s_python_function_hist;
			PyObject *s_python_function_subplot;
			PyObject *s_python_function_legend;
			PyObject *s_python_function_xlim;
			PyObject *s_python_function_ylim;
			PyObject *s_python_function_title;
			PyObject *s_python_function_axis;
			PyObject *s_python_function_xlabel;
			PyObject *s_python_function_ylabel;
			PyObject *s_python_function_grid;
			PyObject *s_python_function_clf;
			PyObject *s_python_function_errorbar;
			PyObject *s_python_function_annotate;
			PyObject *s_python_function_tight_layout;
			PyObject *s_python_empty_tuple;

			/* For now, _interpreter is implemented as a singleton since its currently not possible to have
			   multiple independent embedded python interpreters without patching the python source code
			   or starting a separate process for each.
				http://bytes.com/topic/python/answers/793370-multiple-independent-python-interpreters-c-c-program
			   */

			static _interpreter& get() {
				static _interpreter ctx;
				return ctx;
			}

			private:
			_interpreter() {
                
                // optional but recommended
#if PY_MAJOR_VERSION >= 3
				wchar_t name[] = L"plotting";
#else
				char name[] = "plotting";
#endif
				Py_SetProgramName(name);
				Py_Initialize();

#ifndef WITHOUT_NUMPY
                                auto import_array_lambda = []() {
                                    import_array(); // initialize numpy C-API
                                };
                                // Call the lambda function
                                import_array_lambda();
#endif

				PyObject* matplotlibname = PyString_FromString("matplotlib");
				PyObject* pyplotname = PyString_FromString("matplotlib.pyplot");
				PyObject* pylabname  = PyString_FromString("pylab");
				if (!pyplotname || !pylabname || !matplotlibname) {
					throw std::runtime_error("couldnt create string");
				}

				PyObject* matplotlib = PyImport_Import(matplotlibname);
				Py_DECREF(matplotlibname);
				if(!matplotlib) { throw std::runtime_error("Error loading module matplotlib!"); }

				// matplotlib.use() must be called *before* pylab, matplotlib.pyplot,
				// or matplotlib.backends is imported for the first time
				if (!s_backend.empty()) {
					PyObject_CallMethod(matplotlib, const_cast<char*>("use"), const_cast<char*>("s"), s_backend.c_str());
				}

				PyObject* pymod = PyImport_Import(pyplotname);
				Py_DECREF(pyplotname);
				if(!pymod) { throw std::runtime_error("Error loading module matplotlib.pyplot!"); }


				PyObject* pylabmod = PyImport_Import(pylabname);
				Py_DECREF(pylabname);
				if(!pylabmod) { throw std::runtime_error("Error loading module pylab!"); }

				s_python_function_show = PyObject_GetAttrString(pymod, "show");
				s_python_function_draw = PyObject_GetAttrString(pymod, "draw");
				s_python_function_pause = PyObject_GetAttrString(pymod, "pause");
				s_python_function_figure = PyObject_GetAttrString(pymod, "figure");
				s_python_function_plot = PyObject_GetAttrString(pymod, "plot");
				s_python_function_semilogx = PyObject_GetAttrString(pymod, "semilogx");
				s_python_function_semilogy = PyObject_GetAttrString(pymod, "semilogy");
				s_python_function_loglog = PyObject_GetAttrString(pymod, "loglog");
				s_python_function_fill_between = PyObject_GetAttrString(pymod, "fill_between");
				s_python_function_hist = PyObject_GetAttrString(pymod,"hist");
				s_python_function_subplot = PyObject_GetAttrString(pymod, "subplot");
				s_python_function_legend = PyObject_GetAttrString(pymod, "legend");
				s_python_function_ylim = PyObject_GetAttrString(pymod, "ylim");
				s_python_function_title = PyObject_GetAttrString(pymod, "title");
				s_python_function_axis = PyObject_GetAttrString(pymod, "axis");
				s_python_function_xlabel = PyObject_GetAttrString(pymod, "xlabel");
				s_python_function_ylabel = PyObject_GetAttrString(pymod, "ylabel");
				s_python_function_grid = PyObject_GetAttrString(pymod, "grid");
				s_python_function_xlim = PyObject_GetAttrString(pymod, "xlim");
				s_python_function_save = PyObject_GetAttrString(pylabmod, "savefig");
				s_python_function_annotate = PyObject_GetAttrString(pymod,"annotate");
				s_python_function_clf = PyObject_GetAttrString(pymod, "clf");
				s_python_function_errorbar = PyObject_GetAttrString(pymod, "errorbar");
				s_python_function_tight_layout = PyObject_GetAttrString(pymod, "tight_layout");

				if(        !s_python_function_show
					|| !s_python_function_draw
					|| !s_python_function_pause
					|| !s_python_function_figure
					|| !s_python_function_plot
					|| !s_python_function_semilogx
					|| !s_python_function_semilogy
					|| !s_python_function_loglog
					|| !s_python_function_fill_between
					|| !s_python_function_subplot
				   	|| !s_python_function_legend
					|| !s_python_function_ylim
					|| !s_python_function_title
					|| !s_python_function_axis
					|| !s_python_function_xlabel
					|| !s_python_function_ylabel
					|| !s_python_function_grid
					|| !s_python_function_xlim
					|| !s_python_function_save
					|| !s_python_function_clf
					|| !s_python_function_annotate
					|| !s_python_function_errorbar
					|| !s_python_function_errorbar
					|| !s_python_function_tight_layout
				) { throw std::runtime_error("Couldn't find required function!"); }

				if (       !PyFunction_Check(s_python_function_show)
					|| !PyFunction_Check(s_python_function_draw)
					|| !PyFunction_Check(s_python_function_pause)
					|| !PyFunction_Check(s_python_function_figure)
					|| !PyFunction_Check(s_python_function_plot)
					|| !PyFunction_Check(s_python_function_semilogx)
					|| !PyFunction_Check(s_python_function_semilogy)
					|| !PyFunction_Check(s_python_function_loglog)
					|| !PyFunction_Check(s_python_function_fill_between)
					|| !PyFunction_Check(s_python_function_subplot)
					|| !PyFunction_Check(s_python_function_legend)
					|| !PyFunction_Check(s_python_function_annotate)
					|| !PyFunction_Check(s_python_function_ylim)
					|| !PyFunction_Check(s_python_function_title)
					|| !PyFunction_Check(s_python_function_axis)
					|| !PyFunction_Check(s_python_function_xlabel)
					|| !PyFunction_Check(s_python_function_ylabel)
					|| !PyFunction_Check(s_python_function_grid)
					|| !PyFunction_Check(s_python_function_xlim)
					|| !PyFunction_Check(s_python_function_save)
					|| !PyFunction_Check(s_python_function_clf)
					|| !PyFunction_Check(s_python_function_tight_layout)
					|| !PyFunction_Check(s_python_function_errorbar)
				) { throw std::runtime_error("Python object is unexpectedly not a PyFunction."); }

				s_python_empty_tuple = PyTuple_New(0);
			}

			~_interpreter() {
				Py_Finalize();
			}
		};
	}
	
inline void title(const std::string &titlestr)
{
	
	PyObject* pytitlestr = PyString_FromString(titlestr.c_str());
	PyObject* args = PyTuple_New(1);
	PyTuple_SetItem(args, 0, pytitlestr);

	PyObject* res = PyObject_CallObject(detail::_interpreter::get().s_python_function_title, args);
	if(!res) throw std::runtime_error("Call to title() failed.");


	// if PyDeCRFF, the function doesn't work on Mac OS
}
int main() {
    // 初始化 Python 解释器
    Py_Initialize();

    // // 调用 NumPy 初始化函数
    // import_array();

	title("MCL, step ");

    // 在这里可以进行其他与 NumPy 相关的操作

    // 关闭 Python 解释器
    Py_Finalize();

    return 0;
}

