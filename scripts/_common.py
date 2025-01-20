import os
import subprocess
import shutil
import dataclasses
import abc


class Command(abc.ABC):
    @abc.abstractmethod
    def execute(self):
        ...

    @abc.abstractmethod
    def __str__(self) -> str:
        ...


@dataclasses.dataclass(slots=True, frozen=True)
class SubprocessCommand(Command):
    command: list[str]
    cwd: str = "."

    def execute(self):
        subprocess.run(self.command, cwd=self.cwd).check_returncode()

    def __str__(self) -> str:
        return " ".join(self.command)


@dataclasses.dataclass(slots=True, frozen=True)
class CdCommand(Command):
    path: str

    def execute(self):
        os.chdir(self.path)

    def __str__(self) -> str:
        return f"cd {self.path}"


@dataclasses.dataclass(slots=True, frozen=True)
class CpCommand(Command):
    source_path: str
    destination_path: str

    def execute(self):
        shutil.copy(self.source_path, self.destination_path)

    def __str__(self) -> str:
        return f"cp {self.source_path} {self.destination_path}"
