import subprocess
import os
import uuid

def run_c(code, input_data, timeout=5):
    file_id = str(uuid.uuid4())
    temp_dir = os.path.join(os.path.dirname(__file__), "..", "temp")
    os.makedirs(temp_dir, exist_ok=True)

    source_path = os.path.join(temp_dir, f"{file_id}.c")
    exe_path = os.path.join(temp_dir, f"{file_id}.out")

    with open(source_path, "w") as f:
        f.write(code)

    compile_result = subprocess.run(
        ["gcc", source_path, "-o", exe_path],
        capture_output=True, text=True, timeout=10
    )

    if compile_result.returncode != 0:
        _cleanup(source_path, exe_path)
        return {"status": "Compilation Error", "output": "", "error": compile_result.stderr}

    try:
        run_result = subprocess.run(
            [exe_path],
            input=input_data,
            capture_output=True, text=True, timeout=timeout
        )
        _cleanup(source_path, exe_path)
        if run_result.returncode != 0:
            return {"status": "Runtime Error", "output": run_result.stdout, "error": run_result.stderr}
        return {"status": "Success", "output": run_result.stdout, "error": ""}
    except subprocess.TimeoutExpired:
        _cleanup(source_path, exe_path)
        return {"status": "Time Limit Exceeded", "output": "", "error": ""}

def _cleanup(*paths):
    for p in paths:
        if os.path.exists(p):
            os.remove(p)