# -*- coding: utf-8 -*-
#
# This file is execfile()d with the current directory set to its containing dir.
# All configuration values have a default; values that are commented out
# serve to show the default.
import sys
sys.path.insert(0, '..')

from config import *

# conditional texts
tags.add('website')

# Add any Sphinx extension module names here, as strings. They can be extensions
# coming with Sphinx (named 'sphinx.addons.*') or your custom ones.
extensions = []

# Add any paths that contain templates here, relative to this directory.
templates_path = ['../_templates']

# The suffix of source filenames.
source_suffix = '.rst'

# The master toctree document.
master_doc = 'contents'

# List of documents that shouldn't be included in the build.
exclude_patterns = ['index.rst', 'tclgui.rst', 'helpwanted.rst',
                    'platform.rst', 'examples.rst', 'limitations.rst',
                    'verilog.rst', 'copyright.rst', 'build.rst']

# General information about the project. -> see config.py
# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.

# Show author directives in the output.
show_authors = True

# The HTML template theme.
html_theme = 'sphinxdoc'

# The name for this set of Sphinx documents.  If None, it defaults to
# "<project> v<release> documentation".
html_title = 'Simulavr homepage'

# A list of ignored prefixes names for module index sorting.
modindex_common_prefix = ['sphinx.']

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['../_static']

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
html_last_updated_fmt = '%b %d, %Y'

# Additional templates that should be rendered to pages, maps page names to
# templates.
html_additional_pages = {'index': 'index.html'}

# Custom sidebar templates, maps page names to templates.
html_sidebars = {
   '**': ['globaltoc.html', 'searchbox.html'],
   'index': ['globaltoc.html', 'indexsidebar.html', 'searchbox.html'],
}

# If false, no index is generated.
html_use_index = False

# If true, links to the reST sources are added to the pages.
html_show_sourcelink = False

# Put TODOs into the output.
#todo_include_todos = True
