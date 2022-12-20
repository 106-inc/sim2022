import argparse
import re
from pathlib import Path

kPreambuleRegx = r"[-]+"
kHexRegx = r"[0-9A-Fa-f]+"
kInstrNumRegx = r"NUM=({})".format(kHexRegx)
kPCRegx = r"PC=0x({})".format(kHexRegx)
kRegAccessRegx = r"x({})=0x({})".format(kHexRegx, kHexRegx)
kMemAccessRegx = r"M\[0x({})\]=0x({})".format(kHexRegx, kHexRegx)


class State:
    def __init__(self, streamFilename):
        self.stream = streamFilename.open()

        line = self.stream.readline()  # skip first preambula in trace
        if not re.match(kPreambuleRegx, line):
            raise RuntimeError("{}: Invalid file format.".format(streamFilename))

        self.streamFilename = streamFilename
        self.currentInstrCount = 0
        self.PC = 0
        self.lastChangedRegNum = None
        self.lastChangedRegVal = None
        self.lastChangedMemAddr = None
        self.lastChangedMemVal = None
        self.EOF = False

    def update(self):
        finishState = self.EOF
        while not finishState:
            line = self.stream.readline()
            if not line:
                finishState = True
                self.EOF = True
                break

            match = re.match(kPreambuleRegx, line)
            if match != None:
                finishState = True
                break

            match = re.match(kInstrNumRegx, line)
            if match != None:
                self.currentInstrCount = int(match.group(1))
                break

            match = re.match(kPCRegx, line)
            if match != None:
                self.PC = int(match.group(1), base=16)
                break

            match = re.match(kRegAccessRegx, line)
            if match != None:
                self.lastChangedRegNum = int(match.group(1))
                self.lastChangedRegVal = int(match.group(2), base=16)
                break

            match = re.match(kMemAccessRegx, line)
            if match != None:
                self.lastChangedMemAddr = int(match.group(1), base=16)
                self.lastChangedMemVal = int(match.group(2), base=16)
                break

            raise RuntimeError("{}: Invalid file format.".format(self.streamFilename))


class Cosim:
    def __init__(self, masterFilename, slaveFilename):
        self.masterState = State(masterFilename)
        self.slaveState = State(slaveFilename)
        self.errorMsg = ""
        self.isSame = True

    def run(self):
        masterState = self.masterState
        slaveState = self.slaveState

        while self.isSame and not (masterState.EOF and slaveState.EOF):
            masterState.update()
            slaveState.update()

            self.isSame = self.__compare()

    def __compare(self):
        masterState = self.masterState
        slaveState = self.slaveState

        if masterState.currentInstrCount != slaveState.currentInstrCount:
            self.errorMsg += "master " if masterState.EOF else "slave "
            self.errorMsg += "trace unexpectedly finished"
            return False

        if (
            (masterState.PC != slaveState.PC)
            or (masterState.lastChangedRegNum != slaveState.lastChangedRegNum)
            or (masterState.lastChangedRegVal != slaveState.lastChangedRegVal)
            or (masterState.lastChangedMemAddr != slaveState.lastChangedMemAddr)
            or (masterState.lastChangedMemVal != slaveState.lastChangedMemVal)
        ):
            self.errorMsg += "Mismatch at NUM = {}\n".format(
                masterState.currentInstrCount
            )
            return False

        return True

    def dumpResults(self):
        if self.errorMsg:
            print("Comparison failed:")
            print(self.errorMsg)
        else:
            print("Successfuly compared!")


def main():
    parser = argparse.ArgumentParser(description="Cosimulation")
    parser.add_argument(
        "--master", type=Path, required=True, help="Path to master trace"
    )
    parser.add_argument("--slave", type=Path, required=True, help="Path to slave trace")
    args = parser.parse_args()

    cosim = Cosim(args.master, args.slave)
    cosim.run()
    cosim.dumpResults()


if __name__ == "__main__":
    main()
