from runners.c_runner import run_c
from runners.cpp_runner import run_cpp
from runners.java_runner import run_java
from runners.python_runner import run_python

def execute_code(language, code, input_data="", timeout=5):
    runners = {
        "c": run_c,
        "cpp": run_cpp,
        "java": run_java,
        "python": run_python
    }

    if language not in runners:
        return {"status": "Error", "output": "", "error": "Unsupported language"}

    return runners[language](code, input_data, timeout)