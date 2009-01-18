#!/usr/bin/env python
# ToonLoop for Python
#
# Copyright 2008 Tristan Matthews & Alexandre Quessy
# <le.businessman@gmail.com> & <alexandre@quessy.net>
#
# Original idea by Alexandre Quessy
# http://alexandre.quessy.net
#
# ToonLoop is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ToonLoop is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with camLoop.py.  If not, see <http://www.gnu.org/licenses/>.
#
# Camera module for pygame available from pygame's svn revision 1744 or greater
# svn co svn://seul.org/svn/pygame/trunk

import pygame
import pygame.camera
from pygame.locals import *
from pygame import time

from twisted.internet import reactor

__version__ = 0.2

pygame.init()

class ToonLoop(object):
    def __init__(self, **argd):
        self.img_width = 640
        self.width = self.img_width * 2
        self.height = 480 
        self.size = (self.width, self.height)
        self.__dict__.update(**argd)
        super(ToonLoop, self).__init__(**argd)
        self.surface = pygame.display.set_mode(self.size)
    
        pygame.display.set_caption("ToonLoop")
        self.camera = pygame.camera.Camera("/dev/video0", (self.img_width, self.height))
        self.camera.start()
        self.clock = pygame.time.Clock()
        self.fps = 0
        self.image_list = []
        self.image_idx = 0
        self.running = True
        self.paused = False

    def get_and_flip(self):
        self.last_image = self.camera.get_image()
        self.surface.blit(self.last_image, (0, 0))
        if len(self.image_list) > self.image_idx:
            self.surface.blit(self.image_list[self.image_idx], (self.img_width, 0))
            self.image_idx += 1
        else:
            self.image_idx = 0
        pygame.display.update()

    def grab_image(self):
        self.image_list.append(self.last_image)

    def pause(self):
        self.paused = not self.paused

    def reset_loop(self):
        self.image_list = []
        self.rese_playback_window()

    def pop_one_frame(self):
        if self.image_list != []:
            self.image_list.pop()
            if self.image_list == []:
                self.reset_playback_window()

    def reset_playback_window(self):
        blank_surface = pygame.Surface((self.img_width, self.height))
        playback_pos = (self.img_width, 0)
        self.surface.blit(blank_surface, playback_pos)

    def print_help(self):
        print "Usage: "
        print "<Space bar> = add image to loop "
        print "<Backspace> = remove image from loop "
        print "r = reset loop"
        print "p = pause"
        print "i = print current loop frame number, number of frames in loop and global framerate"
        print "h = print this help message"
        print "<Esc> or q = quit program\n"

    def print_stats(self):
        print "Frame idx: " + str(self.image_idx)
        print "Num images: " + str(len(self.image_list))
        print str(self.fps) + " fps\n"

    def draw(self):
        """
        Renders one frame.
        Called from the event loop. (twisted)
        """
        events = pygame.event.get()
        for e in events:
            if e.type == QUIT:
                self.running = False
            elif e.type == KEYDOWN: 
                if (e.key == K_SPACE):
                    self.grab_image()
                elif (e.key == K_r):
                    self.reset_loop()
                elif (e.key == K_p):
                    self.pause()
                elif (e.key == K_i): 
                    self.print_stats()
                elif (e.key == K_h):
                    self.print_help()
                elif (e.key == K_BACKSPACE):
                    self.pop_one_frame()
                elif (e.key == K_ESCAPE or e.key == K_q):
                    self.running = False
        if not self.paused:
            self.get_and_flip()
            self.clock.tick()
            self.fps = self.clock.get_fps()
    
    def cleanup(self):
        pass


class PygameTimer:
    """
    Integrates a pygame game and twisted.
    See http://twistedmatrix.com/pipermail/twisted-python/2002-October/001884.html
    """
    def __init__(self, game):
        self.clock = time.Clock()
        self.game = game
        # start
        self.update()

    def update(self):
        self.clock.tick()
        self.ms = self.clock.get_rawtime()
        FPS = 60.0 # desired FPS
        framespeed = (1.0 / FPS) * 1000
        lastspeed = self.ms
        next = framespeed - lastspeed
        
        #if self.is_verbose:
        #    print "framespeed", framespeed, "ms", self.ms, "next", next, "fps", self.clock.get_fps()
        #    print "FPS: %5f" % (self.clock.get_fps())
        
        # calls its draw method, which draw and refresh the whole screen.
        self.game.draw()
        
        if not self.game.running:
            self.game.cleanup()
            reactor.stop()
        else:
            when = next / 1000.0 * 2.0
            if when < 0:
                when = 0
            reactor.callLater(when, self.update)

if __name__ == "__main__":
    print "ToonLoop - Version " + str(__version__)
    print "---------------------------------------"
    print "Press h for usage and instructions\n"
    
    pt = PygameTimer(ToonLoop())
    reactor.run()

