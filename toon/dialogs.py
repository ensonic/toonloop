#!/usr/bin/env python
import pprint
from twisted.internet import gtk2reactor
gtk2reactor.install()
import gtk
from twisted.internet import reactor
from twisted.internet import defer

#from kiwi.ui import dialogs
#def blocking():
#    dialogs.error("Error 404", "Not found.")
#    print("error")
#    dialogs.info("Some info")
#    print("info")
#    f = dialogs.save("Save to movie file...")
#    print("Saving to %s" % (f))
#    f = dialogs.open("Open a movie file...")
#    print("Opening %s" % (f))
#    answer = dialogs.yesno("You sure?")
#    print("Yes/no: %s" % (answer == gtk.RESPONSE_YES))
#    while gtk.events_pending():
#        gtk.main_iteration()
    
class Save(object):
    def __init__(self, deferred, title="Save...", folder=None, default_file_name=None):
        self.deferredResult = deferred
        parent = None
        filechooser = gtk.FileChooserDialog(
            title,
            parent,
            gtk.FILE_CHOOSER_ACTION_SAVE,
            (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
            gtk.STOCK_SAVE, gtk.RESPONSE_OK))
        if default_file_name is not None:
            filechooser.set_current_name(default_file_name)
        filechooser.set_default_response(gtk.RESPONSE_OK)
        if folder is not None:
            filechooser.set_current_folder(folder)
        # connecting signals
        filechooser.connect("close", self.on_close)
        filechooser.connect("response", self.on_response)
        filechooser.show()

    def on_close(self, dialog, *params):
        print("on_close %s %s" % (dialog, params))

    def on_response(self, dialog, response_id, *params):
        #print("on_response %s %s %s" % (dialog, response_id, params))
        if response_id == gtk.RESPONSE_DELETE_EVENT:
            print("Deleted")
        elif response_id == gtk.RESPONSE_CANCEL:
            print("Cancelled")
            self.terminate(dialog, None)
        elif response_id == gtk.RESPONSE_OK:
            print("Accepted")
            file_name = dialog.get_filename()
            self.terminate(dialog, file_name)

    def terminate(self, dialog, file_name):
        dialog.destroy()
        self.deferredResult.callback(file_name)

def non_blocking_save_dialog():
    def _on_result(result):
        print(result)
    deferred = defer.Deferred()
    deferred.addCallback(_on_result)
    Save(deferred)
    return deferred

def non_blocking():
    def _later():
        def _cb(result):
            reactor.stop()
        deferred = non_blocking_save_dialog()
        deferred.addCallback(_cb)
    reactor.callLater(0.1, _later)
    reactor.run()

if __name__ == "__main__":
    non_blocking()