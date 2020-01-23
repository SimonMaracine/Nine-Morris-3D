from abc import abstractmethod, ABC


class Disposable(ABC):

    def __init__(self):
        self.cleaned_up = False

    def __del__(self):
        if not self.cleaned_up:
            raise RuntimeError("The object hasn't been disposed")

    @abstractmethod
    def dispose(self):
        self.cleaned_up = True
