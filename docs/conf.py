# -- Path setup --------------------------------------------------------------

import os
import sys
sys.path.insert(0, os.path.abspath(".."))


# -- Project information -----------------------------------------------------

project = "Beetle"
author = "Igor Machado"
release = "0.1"

# The master toctree document
master_doc = "index"


# -- General configuration ---------------------------------------------------

extensions = [
    "sphinx.ext.autodoc",      # auto docs from code
    "sphinx.ext.napoleon",     # Google/Numpy docstrings
    "sphinx.ext.viewcode",     # add links to source code
    "sphinx.ext.todo",         # TODO notes
]

templates_path = ["_templates"]

exclude_patterns = [
    "_build",
    "Thumbs.db",
    ".DS_Store",
]


# -- Options for TODO extension ---------------------------------------------

todo_include_todos = True


# -- HTML output -------------------------------------------------------------

html_theme = "sphinx_rtd_theme"

html_static_path = ["_static"]

html_title = "Beetle Documentation"


# -- Syntax highlighting -----------------------------------------------------

highlight_language = "python"  # fallback (you can change later)


# -- Autodoc options ---------------------------------------------------------

autodoc_member_order = "bysource"
autoclass_content = "both"


# -- Napoleon settings -------------------------------------------------------

napoleon_google_docstring = True
napoleon_numpy_docstring = True


# -- Custom setup (optional) -------------------------------------------------

def setup(app):
    # You can register custom CSS here later
    pass
