import logging


def get_logger(name: str) -> logging.Logger:
    logger = logging.getLogger(name)
    if not logger.handlers:
        logger.propagate = False
        logger.addHandler(_handler)
    return logger


_formatter = logging.Formatter("%(levelname)s:%(name)s:%(message)s")  # :%(asctime)s this maybe?
_handler = logging.StreamHandler()
_handler.setFormatter(_formatter)

# Disable all logs if optimization is True
if not __debug__:
    logging.disable()
