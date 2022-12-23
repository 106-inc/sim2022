"""This module generates enum w/ opcodes & decoder function."""

import argparse
import sys
import textwrap
from collections import defaultdict
from pathlib import Path

import yaml

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
    #include <functional>
    #include <unordered_map>

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

BitDict = dict[str, bool | int]
InstDict = dict[str, list[str] | str]
RiscVDict = dict[str, InstDict]
ByMaskDict = defaultdict[int, dict[str, InstDict]]


def get_bit_map_dict(
    msb: int, lsb: int | None = None, lshift: int = 0, signext: bool = True
) -> BitDict:
    """Helper function to build decoding dictionary"""

    if lsb is None:
        lsb = msb
    assert msb >= lsb
    assert lshift < 32
    return {
        "msb": msb,
        "lsb": lsb,
        "lshift": lshift,
        "from": lshift + msb - lsb,
        "sign": signext,
    }


def gen_getbits(bit_dict: BitDict, arg: str = "binInst"):
    """Helper function to generate call of c++ getBits() function from bit dictionary"""

    to_ret = f"getBits<{bit_dict['msb']}, {bit_dict['lsb']}>({arg})"
    shift = bit_dict["lshift"]
    if shift != 0:
        to_ret += f" << Word({shift})"

    return to_ret


BRANCH_MNEMONICS = (
    "beq",
    "bne",
    "bge",
    "bgeu",
    "blt",
    "bltu",
    "jal",
    "jalr",
    "ecall",
)

REG_DICT = {
    "rm": get_bit_map_dict(14, 12),
    "rd": get_bit_map_dict(11, 7),
    "csr": get_bit_map_dict(31, 20),
    "rs2": get_bit_map_dict(24, 20),
    "rs1": get_bit_map_dict(19, 15),
    "rs3": get_bit_map_dict(31, 27),
}

IMM_DICT: dict[str, tuple[BitDict, ...]] = {
    "imm20": (get_bit_map_dict(31, 12, 12),),
    "jimm20": (
        get_bit_map_dict(31, lshift=20),
        get_bit_map_dict(30, 21, 1),
        get_bit_map_dict(20, lshift=11),
        get_bit_map_dict(19, 12, 12),
    ),
    "succ": (get_bit_map_dict(23, 20),),
    "pred": (get_bit_map_dict(27, 24, 4),),
    "fm": (get_bit_map_dict(31, 28, 8),),
    "imm12": (get_bit_map_dict(31, 20),),
    "zimm": (get_bit_map_dict(19, 15, signext=False),),
    "aq": (get_bit_map_dict(26, lshift=1),),
    "rl": (get_bit_map_dict(25),),
    "bimm12hi": (
        get_bit_map_dict(31, lshift=12),
        get_bit_map_dict(30, 25, 5),
    ),
    "bimm12lo": (
        get_bit_map_dict(11, 8, 1),
        get_bit_map_dict(7, lshift=11),
    ),
    "imm12hi": (get_bit_map_dict(31, 25, 5),),
    "imm12lo": (get_bit_map_dict(11, 7),),
    "shamtw": (get_bit_map_dict(24, 20),),
}


def gen_fill_inst(dec_data: InstDict, inst_name: str) -> str:
    """Generate instruction's fields filling function"""
    to_ret = ""

    to_ret += f"    decodedInst.type = OpType::{inst_name.upper()};\n"

    if inst_name in BRANCH_MNEMONICS:
        to_ret += "    decodedInst.isBranch = true;\n"

    max_from = 0
    has_imm = False
    sign_ext = True

    for field_name in dec_data["variable_fields"]:

        if field_name in REG_DICT:
            dst = f"decodedInst.{field_name}"
            to_ret += (
                f"    {dst} = static_cast<decltype({dst})>"
                f"({gen_getbits(REG_DICT[field_name])});\n"
            )

        elif field_name in IMM_DICT:
            has_imm = True
            for bits_dict in IMM_DICT[field_name]:
                if not bits_dict["sign"]:
                    sign_ext = False

                max_from = max(max_from, bits_dict["from"])
                to_ret += f"    decodedInst.imm |= {gen_getbits(bits_dict)};\n"

        else:
            raise ValueError(f"Unrecognized field name {field_name}")

    # sign extend an immediate
    if has_imm and sign_ext:
        assert max_from <= 32
        to_ret += (
            f"    decodedInst.imm = signExtend<{max_from + 1}>"
            "(decodedInst.imm);\n"
        )

    return to_ret


def gen_ifs(yaml_dict: RiscVDict) -> str:
    """Generate ifs decoding from yaml dictionary function"""

    to_ret = ""
    for inst_name, dec_data in yaml_dict.items():
        mask_str = dec_data["mask"]
        match_str = dec_data["match"]
        assert isinstance(mask_str, str)
        assert isinstance(match_str, str)

        mask = int(mask_str, 0)
        matched = int(match_str, 0)
        to_ret += f"if ((binInst & 0b{mask:032b}) == 0b{matched:032b}) {{\n"
        to_ret += gen_fill_inst(dec_data, inst_name)
        to_ret += "    return decodedInst;\n}\n"

    return to_ret


def gen_by_mask_dict(yaml_dict: RiscVDict) -> ByMaskDict:
    """Generate by-mask dictionary function"""
    to_ret: ByMaskDict = defaultdict(dict)
    for inst_name, dec_data in yaml_dict.items():
        mask = dec_data["mask"]
        assert isinstance(mask, str)
        to_ret[int(mask, 0)][inst_name] = dec_data

    return to_ret


def gen_switches(yaml_dict: RiscVDict) -> str:
    """Generate decoding by switches function"""
    to_ret = ""

    mask_dict = gen_by_mask_dict(yaml_dict)

    for mask, insts_dict in mask_dict.items():
        to_ret += f"  switch (binInst & 0b{mask:032b}) {{\n"

        for inst_name, dec_dict in insts_dict.items():
            matched = dec_dict["match"]
            assert isinstance(matched, str)

            to_ret += f"  case 0b{int(matched, 0):032b}:\n"
            to_ret += gen_fill_inst(dec_dict, inst_name)
            to_ret += "    return decodedInst;\n"

        to_ret += "  default:\n    break;\n  }\n"

    return to_ret


def gen_maps(yaml_dict: RiscVDict) -> str:
    """Generate decoding by maps function"""

    def make_map_def(mask: int) -> str:
        return (
            "  static const std::unordered_map<decltype(binInst), "
            + "std::function<void(Instruction &, decltype(binInst))>> "
            + f"decMap_{mask:04X} = {{\n"
        )

    maps_defs: defaultdict[int, str] = defaultdict(str)

    map_finds = ""
    masks_dict = gen_by_mask_dict(yaml_dict)

    for mask, insts_dict in masks_dict.items():
        dec_map_name = f"decMap_{mask:04X}"

        map_finds += (
            f"  if (auto it = {dec_map_name}.find(binInst & 0b{mask:032b}); "
            + f"it != {dec_map_name}.end()) {{\n"
        )
        map_finds += "    it->second(decodedInst, binInst);\n"
        map_finds += "    return decodedInst;\n"
        map_finds += "  }\n"

        maps_defs[mask] = make_map_def(mask)

        for inst_name, dec_dict in insts_dict.items():
            matched = dec_dict["match"]
            assert isinstance(matched, str)

            maps_defs[mask] += (
                f"    {{0b{int(matched, 0):032b}, "
                "[](Instruction &decodedInst, decltype(binInst) binInst) {\n"
                + "      (void)binInst;\n"
            )
            maps_defs[mask] += gen_fill_inst(dec_dict, inst_name)
            maps_defs[mask] += "    }},\n"

        maps_defs[mask] += "};\n"

    return "\n".join(maps_defs.values()) + map_finds


def gen_cc(filename: Path, yaml_dict: RiscVDict) -> None:
    """Function tp generate decoder function c++ file"""

    to_write = COMMENT
    to_write += INCLUDES
    to_write += START_NAMESPACE
    to_write += FUNC_HEADER
    to_write += gen_maps(yaml_dict)
    to_write += "return decodedInst;\n"
    to_write += FUNC_FOOTER
    to_write += END_NAMESPACE

    with open(filename, "w", encoding="utf-8") as fout:
        fout.write(to_write)


def gen_hh(filename: Path, yaml_dict: RiscVDict) -> None:
    """Function to generate c++ header with enum with instructions"""

    to_write = COMMENT
    to_write += "#include <string_view>\n"
    to_write += "#include <unordered_map>\n\n"

    to_write += "namespace sim {\n"
    to_write += "enum class OpType {\n"
    to_write += "    UNKNOWN = 0,\n"
    for inst_name in yaml_dict:
        to_write += f"    {inst_name.upper()},\n"
    to_write += "};\n\n"

    to_write += (
        "inline std::unordered_map<OpType, std::string_view> opTypeToString {\n"
    )
    for inst_name in yaml_dict:
        iname = inst_name.upper()
        to_write += f'    {{OpType::{iname}, "{iname}"}},\n'
    to_write += "};\n\n"

    to_write += "}\n"

    with open(filename, "w", encoding="utf-8") as fout:
        fout.write(to_write)


def main() -> None:
    """Main function"""

    parser = argparse.ArgumentParser(
        description="Tool to generate decoder function according to "
        "riscv encoding"
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

    with open(args.yaml, encoding="utf-8") as yml:
        yaml_data = yaml.safe_load(yml)

    # make dirs for generated files
    args.decoder_file.parent.mkdir(parents=True, exist_ok=True)
    args.enum_file.parent.mkdir(parents=True, exist_ok=True)

    # generate enum & decoder files
    gen_cc(args.decoder_file, yaml_data)
    gen_hh(args.enum_file, yaml_data)


if "__main__" == __name__:
    try:
        main()
    except ValueError as ex:
        print(f"Caught an exception. Error: {str(ex)}")
        sys.exit(1)
