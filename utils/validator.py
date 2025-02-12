import operator

def is_in_bound(value, bound):
    ops = None
    if type(bound) is tuple:
        ops = operator.lt
    elif type(bound) is list:
        ops = operator.le
    if bound[0] == float("-inf") and bound[1] == float("inf"):
        return True
    elif bound[0] == float("-inf") and ops(value, bound[1]):
        return True
    elif ops(bound[0], value) and bound[1] == float("inf"):
        return True
    elif ops(bound[0], value) and ops(value, bound[1]):
        return True
    return False


def is_str_in_list(value: str, my_list: list):
    if type(value) == str and my_list is not None:
        return True if value in my_list else False
    return False


class Validator:
    def __init__(self, **kwargs):
        self.log_to, self.log_file = None, None
        self.__set_keyword_arguments(kwargs)

    def __set_keyword_arguments(self, kwargs):
        for key, value in kwargs.items():
            setattr(self, key, value)

    # recupere is in bound qui est au dessus et  check_int
    # et met les dans une autre class pour pouvoir supprimé cette page
    def check_int(self, name:str, value: int, bound=None):
        if type(value) in [int, float]:
            if bound is None:
                return int(value)
            elif is_in_bound(value, bound):
                return int(value)
        bound = "" if bound is None else f"and value should be in range: {bound}"
        raise ValueError(f"'{name}' is an integer {bound}.")
