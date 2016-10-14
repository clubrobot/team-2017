#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import curses
import robotcom

# Connect the Arduino

engine = robotcom.RobotCom('/dev/arduino/engine')
engine.connect()

# Define the events

def key_up_event(stdscr):
	stdscr.addstr(2, 60, 'UP   ')
	engine.send(2, 0, 128, 0)
	engine.send(2, 1, 128, 0)

def key_down_event(stdscr):
	stdscr.addstr(2, 60, 'DOWN ')
	engine.send(2, 0, 128, 1)
	engine.send(2, 1, 128, 1)

def key_left_event(stdscr):
	stdscr.addstr(2, 60, 'LEFT ')
	engine.send(2, 0, 128, 1)
	engine.send(2, 1, 128, 0)

def key_right_event(stdscr):
	stdscr.addstr(2, 60, 'RIGHT')
	engine.send(2, 0, 128, 0)
	engine.send(2, 1, 128, 1)

def key_space_event(stdscr):
	stdscr.addstr(2, 60, 'SPACE')
	engine.send(2, 0, 0, 0)
	engine.send(2, 1, 0, 0)

# Setup and launch the user interface

def draw(stdscr):
	stdscr.clear()
	stdscr.addstr('Welcome to the Club Robot interactive application\n\n')
	stdscr.addstr('Please press one of these commands to manipulate the robot:\n')
	stdscr.addstr('  UP    : Go forward\n')
	stdscr.addstr('  DOWN  : Go backward\n')
	stdscr.addstr('  LEFT  : Turn left\n')
	stdscr.addstr('  RIGHT : Turn right\n')
	stdscr.addstr('  SPACE : Stop\n')
	stdscr.addstr(curses.LINES - 1, 0, '{:<{}}'.format('Press q to quit', curses.COLS - 1), curses.A_REVERSE)

def user_interface(stdscr):

	# Init screen
	curses.curs_set(False)

	# Draw basic informations
	draw(stdscr)

	# Loop until key 'q' is pressed
	while True:

		# This will wait until a key is pressed
		key = stdscr.getch()

		if key == ord('q'):
			break

		if key == curses.KEY_UP:
			key_up_event(stdscr)
		elif key == curses.KEY_DOWN:
			key_down_event(stdscr)
		elif key == curses.KEY_LEFT:
			key_left_event(stdscr)
		elif key == curses.KEY_RIGHT:
			key_right_event(stdscr)
		elif key == ord(' '):
			key_space_event(stdscr)

		if key == curses.KEY_RESIZE:
			stdscr.refresh()
			try:
				draw(stdscr)
			except:
				pass

curses.wrapper(user_interface)
