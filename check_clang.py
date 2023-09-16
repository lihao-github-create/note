import subprocess

def check_clang():
    try:
        subprocess.run(["clang", "--version"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print("clang is installed")
    except subprocess.CalledProcessError as e:
        print("clang is not installed")

def check_clangplusplus():
    try:
        subprocess.run(["clang++", "--version"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print("clang++ is installed")
    except subprocess.CalledProcessError as e:
        print("clang++ is not installed")

check_clang()
check_clangplusplus()
