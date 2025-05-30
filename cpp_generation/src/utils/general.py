def dict_len(dict):
  return len(dict.keys())

def to_camel_case(snake_str):
  return "".join(x.capitalize() for x in snake_str.lower().split("_"))


def to_lower_camel_case(snake_str):
  camel_string = to_camel_case(snake_str)
  return snake_str[0].lower() + camel_string[1:]
