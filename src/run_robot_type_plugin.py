#!/usr/bin/env python2

import sys

from rqt_gui.main import Main

main = Main()
sys.exit(main.main(sys.argv, standalone='robot_type_plugin'))
