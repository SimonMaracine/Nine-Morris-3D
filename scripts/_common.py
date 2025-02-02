import os
import subprocess
import shutil
import dataclasses
import abc
import sys
from typing import Collection


class Command(abc.ABC):
    @abc.abstractmethod
    def execute(self):
        ...


@dataclasses.dataclass(slots=True, frozen=True)
class SubprocessCommand(Command):
    _command: list[str]
    _cwd: str = "."

    def execute(self):
        subprocess.run(self._command, cwd=self._cwd).check_returncode()

    def __str__(self) -> str:
        return " ".join(self._command)

    def append(self, string: str):
        self._command.append(string)


@dataclasses.dataclass(slots=True, frozen=True)
class CdCommand(Command):
    _path: str

    def execute(self):
        os.chdir(self._path)

    def __str__(self) -> str:
        return f"cd {self._path}"


@dataclasses.dataclass(slots=True, frozen=True)
class CpCommand(Command):
    _source_path: str
    _destination_path: str

    def execute(self):
        shutil.copy(self._source_path, self._destination_path)

    def __str__(self) -> str:
        return f"cp {self._source_path} {self._destination_path}"


@dataclasses.dataclass(slots=True, frozen=True)
class MakeDirsCommand(Command):
    _directory: str
    _exist_ok: bool = False

    def execute(self):
        os.makedirs(self._directory, exist_ok=self._exist_ok)

    def __str__(self) -> str:
        return f"mkdirs {self._directory} {"[exist okay]" if self._exist_ok else ""}"


def execute(commands: Collection[Command]) -> int:
    try:
        for command in commands:
            print(command)
            command.execute()
    except subprocess.CalledProcessError as err:
        print(f"An error occurred: {err}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print()
        return 1
    except Exception as err:
        print(f"An unexpected error occurred: {err}", file=sys.stderr)
        return 1

    return 0
