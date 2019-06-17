# python script
import sys
from pkg_resources import load_entry_point

sys.exit(load_entry_point('Sphinx', 'console_scripts', 'sphinx-build')())
# EOF
