import json
import os

from jinja2 import Environment, FileSystemLoader, select_autoescape

import utils.concepts
import utils.config
import utils.state_space
import utils.delphi
import utils.general


def relative_file_path(*paths):
  dirname = os.path.dirname(__file__)
  return os.path.join(dirname, *paths)


def load_json(*paths):
  with open(relative_file_path(*paths)) as f:
    return json.load(f)


def load_children_model_schemas(paths):
  if isinstance(paths, str):
    return load_json("..", "modelSchemas", paths)
  else:
    return [load_json("..", "modelSchemas", p) for p in paths]


def generate(template_path, dest_path, *args, **kwargs):
  template = env.get_template(template_path)
  output = template.render(*args, **kwargs)
  with open(dest_path, "w") as f:
    f.write(output)


def apply_default_vars_to_overrides(dat, section):
  overrides = dat[section].get("overrides")
  if not overrides: return

  default_vars = dat[section]["default"]
  for override in overrides:
    override["vars"] = default_vars | override["vars"]


def generate_hpp(template_name, *args, **kwargs):
  template_path = f'cpp/{template_name}.j2'
  dest_path = relative_file_path("..", "..", "r-package", "inst", "include", "generated", f"{template_name}.hpp")
  generate(template_path, dest_path, *args, **kwargs)


def generate_delphi(template_name, *args, **kwargs):
  template_path = f'delphi/{template_name}.j2'
  dest_path = relative_file_path("..", "..", "delphi", f"{template_name}.pas")
  generate(template_path, dest_path, *args, **kwargs)


dat = load_json("..", "modelSchemas", "FullModel.json")
dat = { k: load_children_model_schemas(v) for k, v in dat.items() }
for cfg in dat["configs"]:
  apply_default_vars_to_overrides(cfg, "state_space")
  apply_default_vars_to_overrides(cfg, "pars")

file_loader = FileSystemLoader(relative_file_path("..", "templates"))
env = Environment(
  loader = file_loader,
  trim_blocks = True,
  lstrip_blocks = True,
  autoescape = select_autoescape(),
  keep_trailing_newline = True
)

generate_hpp("model_variants", dat)
generate_hpp("state_space", dat | vars(utils.state_space))
generate_hpp("concepts", dat | vars(utils.concepts))
generate_hpp("state_space_mixer", dat)
generate_hpp("config", dat | vars(utils.config) | vars(utils.general))
generate_hpp("config_mixer", dat)
generate_hpp("cpp_interface/cpp_adapters", dat | vars(utils.config) | vars(utils.general))
generate_hpp("r_interface/r_adapters", dat | vars(utils.config) | vars(utils.general))
generate_hpp("c_interface/c_adapters", dat | vars(utils.config) | vars(utils.general))
generate_hpp("c_interface/c_types", dat | vars(utils.config) | vars(utils.delphi) | vars(utils.general))
generate_delphi("LeapfrogInterface", dat | vars(utils.delphi) | vars(utils.general))
