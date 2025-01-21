import os
import subprocess
import shutil
import dataclasses
import abc


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
