import argparse
import yaml
import sys
from pathlib import Path
import textwrap

COMMENT = textwrap.dedent(
    """\
    /*
     * This file is autogenerated by 'decoder.py' script from .yaml file in riscv-opcodes repo
     * Do not try to change anything in this file
     *****************************************************************************************
     * Written by Derzhavin Andrey (derzhavin3016)
     */

    """
)
INCLUDES = textwrap.dedent(
    """
    #include "decoder/decoder.hh"

    """
)
START_NAMESPACE = textwrap.dedent(
    """
    namespace sim {
    """
)

END_NAMESPACE = textwrap.dedent(
    """

    }
    """
)

FUNC_HEADER = textwrap.dedent(
    """
    Instruction Decoder::decode(Word binInst) {
        Instruction decodedInst{};

    """
)

FUNC_FOOTER = END_NAMESPACE


def get_bit_map_dict(msb, lsb=None, lshift=0):
    if lsb is None:
        lsb = msb
    assert msb >= lsb
    assert lshift < 32
    return {"msb": msb, "lsb": lsb, "lshift": lshift, "from": lshift + msb - lsb}


def gen_getbits(bit_dict, arg="binInst"):
    to_ret = f"getBits<{bit_dict['msb']}, {bit_dict['lsb']}>({arg})"
    shift = bit_dict["lshift"]
    if shift != 0:
        to_ret += f" << Word({shift})"

    return to_ret


REG_DICT = {
    "rm": get_bit_map_dict(14, 12),
    "rd": get_bit_map_dict(11, 7),
    "csr": get_bit_map_dict(31, 20),
    "rs2": get_bit_map_dict(24, 20),
    "rs1": get_bit_map_dict(19, 15),
    "rs3": get_bit_map_dict(31, 27),
}

IMM_DICT = {
    "imm20": [get_bit_map_dict(31, 12, 12)],
    "jimm20": [
        get_bit_map_dict(31, lshift=20),
        get_bit_map_dict(30, 21, 1),
        get_bit_map_dict(20, lshift=11),
        get_bit_map_dict(19, 12, 12),
    ],
    "succ": [get_bit_map_dict(23, 20)],
    "pred": [get_bit_map_dict(27, 24, 4)],
    "fm": [get_bit_map_dict(31, 28, 8)],
    "imm12": [get_bit_map_dict(31, 20)],
    "zimm": [get_bit_map_dict(19, 15)],
    "aq": [get_bit_map_dict(26, lshift=1)],
    "rl": [get_bit_map_dict(25)],
    "bimm12hi": [get_bit_map_dict(31, lshift=12), get_bit_map_dict(30, 25, 5)],
    "bimm12lo": [get_bit_map_dict(11, 8, 1), get_bit_map_dict(7, lshift=11)],
    "imm12hi": [get_bit_map_dict(31, 25, 5)],
    "imm12lo": [get_bit_map_dict(11, 7)],
}


def gen_ifs(yaml_dict):
    to_ret = ""
    for inst_name, dec_data in yaml_dict.items():
        mask = int(dec_data["mask"], 0)
        match = int(dec_data["match"], 0)
        to_ret += f"if ((binInst & 0b{mask:032b}) == 0b{match:032b}) {{\n"
        to_ret += f"    decodedInst.type = OpType::{inst_name.upper()};\n"

        max_from = 0
        has_imm = False
        for field_name in dec_data["variable_fields"]:
            if field_name in REG_DICT:
                to_ret += (
                    f"    decodedInst.{field_name} = {gen_getbits(REG_DICT[field_name])};\n"
                )
            elif field_name in IMM_DICT:
                has_imm = True
                for bits_dict in IMM_DICT[field_name]:
                    max_from = max(max_from, bits_dict["from"])
                    to_ret += f"    decodedInst.imm |= {gen_getbits(bits_dict)};\n"

            else:
                raise ValueError(f"Unrecognized field name {field_name}")

        # sign extend an immediate
        if has_imm:
            assert max_from <= 32
            to_ret += f"    decodedInst.imm = signExtend<{max_from}>(decodedInst.imm);\n"

        to_ret += "    return decodedInst;\n}\n"

    return to_ret


def gen_cc(filename, yaml_dict):
    to_write = COMMENT
    to_write += INCLUDES
    to_write += START_NAMESPACE
    to_write += FUNC_HEADER
    to_write += gen_ifs(yaml_dict)
    to_write += "return decodedInst;\n"
    to_write += FUNC_FOOTER
    to_write += END_NAMESPACE

    with open(filename, "w") as fout:
        fout.write(to_write)


def gen_hh(filename, yaml_dict):
    to_write = COMMENT
    to_write += "enum class OpType {\n    UNKNOWN,\n"

    for inst_name in yaml_dict:
        to_write += f"    {inst_name.upper()},\n"

    to_write += "};\n"

    with open(filename, "w") as fout:
        fout.write(to_write)


def main():
    parser = argparse.ArgumentParser(
        description="Tool to generate decoder function according to riscv encoding"
    )

    parser.add_argument(
        "-y", "--yaml", required=True, type=Path, help="RiscV .yaml file"
    )
    parser.add_argument(
        "-d",
        "--decoder-file",
        required=True,
        type=Path,
        help="Output .cc file for decoder function",
    )
    parser.add_argument(
        "-e",
        "--enum-file",
        required=True,
        type=Path,
        help="Output .hh file for OpType enum definition",
    )

    args = parser.parse_args()

    with open(args.yaml) as yml:
        yaml_data = yaml.safe_load(yml)
    args.decoder_file.parent.mkdir(parents=True, exist_ok=True)
    args.enum_file.parent.mkdir(parents=True, exist_ok=True)
    gen_cc(args.decoder_file, yaml_data)
    gen_hh(args.enum_file, yaml_data)


if "__main__" == __name__:
    try:
        main()
    except Exception as ex:
        print(f"Caught an exception. Error: {str(ex)}")
        sys.exit(1)
