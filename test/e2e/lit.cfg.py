from os import path
import lit.formats

config.name = "sim2022 testing"
config.test_format = lit.formats.ShTest(True)

config.suffixes = [".c", ".s"]

config.test_source_root = path.dirname(__file__)
config.test_exec_root = path.join(config.my_obj_root, "bin/test")

config.substitutions.append(
    ("%simulator", path.join(config.my_obj_root, "bin/simulator"))
)
config.substitutions.append(
    ("%fc", "FileCheck-10 --allow-empty --match-full-lines")
)
config.substitutions.append(
    (
        "%gcc",
        "riscv32-unknown-elf-gcc -O0 -e main -nostdlib -march=rv32g",
    )
)
