from pathlib import Path
import subprocess
from typing import Dict

counts: Dict[str, int] = {}

def py_shakespeare(word: str) -> int:
    if word in counts:
        return counts[word]
    return 0

def py_collect(name: str = "tinyshakespeare.txt"):
    with Path(name).open() as f:
        for line in f.readlines():
            line = line.strip()
            if line == "":
                continue

            for word in line.split():
                if word in counts:
                    counts[word] = counts[word] + 1
                else:
                    counts[word] = 1

def c_shakespeare(word: str) -> int:
    count = subprocess.check_output(['./shakespeare', word])
    return int(count)


if __name__ == "__main__":
    py_collect()
    for (word, count) in counts.items():
        c_count = c_shakespeare(word)
        if c_count != count:
            print(word)
            print(f"python version: {count}.")
            print(f"c version: {c_count}.")
