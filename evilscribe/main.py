#!/usr/local/bin/python

from wxPython.wx import *
from wxPython.grid import *

from romedit import *
from about import *
from config import *
from grid import *
from idle_speed import *
from rev_limit import *
from injectors import *
from fuel_cut import *
from maf import *


class EvilScribe(wxApp):
    def OnInit(self):
        frame = Main(NULL)
        frame.Show(true)
        self.SetTopWindow(frame)
        return true


class Main(wxMDIParentFrame):
    ID_OPEN = wxNewId()
    ID_CLOSE = wxNewId()
    ID_SAVE = wxNewId()
    ID_SAVEAS = wxNewId()
    ID_EXIT = wxNewId()

    ID_FUEL = wxNewId()
    ID_TIMING = wxNewId()
    ID_INJECTORS = wxNewId()
    ID_IDLE_SPEED = wxNewId()
    ID_REV_LIMIT = wxNewId()
    ID_FUEL_CUT = wxNewId()
    ID_MAF = wxNewId()

    ID_ABOUT = wxNewId()

      # Set true if loaded ROM image has been modified
    image_modified = false

    image_path = ''


    def __init__(self, parent):
        wxMDIParentFrame.__init__(self, parent, -1, config.app_name,
                                  wxDefaultPosition,
                                  wxSize(config.app_height, config.app_width))

        #self.Icon = wxIcon('mitsubishi_logo.xpm', wxBITMAP_TYPE_XPM)
        #self.SetIcon(self.Icon)

        self.StatusBar = self.CreateStatusBar()

        menubar = self.CreateMenu()
        self.SetMenuBar(menubar)
        menubar.EnableTop(1, false)

          # Is there a file currently open?
        self.file_open = True

          # We check here to see if our various windows are already open or
          # not, False is not open.
        self.fuel = False
        self.timing = False
        self.injectors = False
        self.idle_speed = False
        self.rev_limit = False
        self.fuel_cut = False
        self.maf = False


        EVT_MENU(self, self.ID_OPEN, self.OnMenuOpen)
        EVT_MENU(self, self.ID_CLOSE, self.OnMenuClose)
        EVT_MENU(self, self.ID_SAVE, self.OnMenuSave)
        EVT_MENU(self, self.ID_SAVEAS, self.OnMenuSaveAs)
        EVT_MENU(self, self.ID_EXIT, self.OnMenuExit)

        EVT_MENU(self, self.ID_FUEL, self.OnMenuFuel)
        EVT_MENU(self, self.ID_TIMING, self.OnMenuTiming)
        EVT_MENU(self, self.ID_INJECTORS, self.OnMenuInjectors)
        EVT_MENU(self, self.ID_IDLE_SPEED, self.OnMenuIdleSpeed)
        EVT_MENU(self, self.ID_REV_LIMIT, self.OnMenuRevLimit)
        EVT_MENU(self, self.ID_FUEL_CUT, self.OnMenuFuelCut)
        EVT_MENU(self, self.ID_MAF, self.OnMenuMAF)

        EVT_MENU(self, self.ID_ABOUT, self.OnMenuAbout)

        EVT_CLOSE(self, self.OnCloseWindow)

    def CreateMenu(self):
        file_menu = wxMenu()
        file_menu.Append(self.ID_OPEN, '&Open', 'Open a ROM image')
        file_menu.Append(self.ID_CLOSE, '&Close', 'Close current ROM image')
        file_menu.Append(self.ID_SAVE, '&Save', 'Save ROM image')
        file_menu.Append(self.ID_SAVEAS, 'Save &As', 'Save ROM image as ...')
        file_menu.AppendSeparator()
        file_menu.Append(self.ID_EXIT, 'E&xit', 'Exit application')

        view_menu = wxMenu()
        view_menu.Append(self.ID_FUEL, '&Fuel', 'Edit fuel map')
        view_menu.Append(self.ID_TIMING, '&Timing', 'Edit timing map')
        view_menu.Append(self.ID_INJECTORS, '&Injectors', 'Edit fuel injector compensation')
        view_menu.Append(self.ID_IDLE_SPEED, 'Idle &Speed', 'Edit idle speed')
        view_menu.Append(self.ID_REV_LIMIT, '&Rev Limit', 'Edit rev limiter')
        view_menu.Append(self.ID_FUEL_CUT, '&Fuel Cut', 'Enable or disable fuel cut')
        view_menu.Append(self.ID_MAF, '&MAF', 'Edit mass air compensation')

        help_menu = wxMenu()
        help_menu.Append(self.ID_ABOUT, '&About', 'About this application')

        main = wxMenuBar()
        main.Append(file_menu, '&File')
        main.Append(view_menu, '&View')
        main.Append(help_menu, '&Help')

        return main


    def OnMenuOpen(self, event):
        # Check if another open dialog already exists

        if self.file_open == True:
            self.OnMenuClose(NULL)

        dlg = wxFileDialog(self)
        dlg.SetStyle(wxOPEN)
        #dlg.SetWildcard('*.bin')
        if dlg.ShowModal() == wxID_OK:
            path = dlg.GetPath()
            self.image_path = path

            self.SetTitle(config.app_name + ' - ' + path)

              # XXX: Error checking
            editor.import_image(path)

              # Enable 'View' menu
            self.GetMenuBar().EnableTop(1, true)

            self.OnMenuFuel(None)
            self.OnMenuTiming(None)
            self.file_open = True

        dlg.Destroy()

    def OnMenuClose(self, event):
        self.fuel = self.DoClose(self.fuel)
        self.timing = self.DoClose(self.timing)
        self.injectors = self.DoClose(self.injectors)
        self.idle_speed = self.DoClose(self.idle_speed)
        self.rev_limit = self.DoClose(self.rev_limit)
        self.fuel_cut = self.DoClose(self.fuel_cut)
        self.maf = self.DoClose(self.maf)

        editor.close()
        self.file_open = False
        self.GetMenuBar().EnableTop(1, false)

    def OnMenuSave(self, event):
        editor.export_image(self.image_path)

    def OnMenuSaveAs(self, event):

        dlg = wxFileDialog(self)
        dlg.SetStyle(wxSAVE)

        if dlg.ShowModal() == wxID_OK:
            path = dlg.GetPath()
            self.image_path = path

            self.SetTitle(config.app_name + ' - ' + path)

            editor.export_image(path)

        dlg.Destroy()

    def OnMenuExit(self, event):
        self.Close()

    def OnMenuFuel(self, event):
        self.fuel = self.DoViewSelection(self.fuel, FuelView)

    def OnMenuTiming(self, event):
        self.timing = self.DoViewSelection(self.timing, TimingView)

    def OnMenuInjectors(self, event):
        self.injectors = self.DoViewSelection(self.injectors, InjectorFrame)

    def OnMenuIdleSpeed(self, event):
        self.idle_speed = self.DoViewSelection(self.idle_speed, IdleSpeedFrame)

    def OnMenuRevLimit(self, event):
        self.rev_limit = self.DoViewSelection(self.rev_limit, RevLimitFrame)

    def OnMenuFuelCut(self, event):
        self.fuel_cut = self.DoViewSelection(self.fuel_cut, FuelCutFrame)

    def OnMenuMAF(self, event):
        self.maf = self.DoViewSelection(self.maf, MAFCompensationFrame)

    def OnMenuAbout(self, event):
        dlg = AboutDialog(self, config)
        dlg.ShowModal()
        dlg.Destroy()

    def OnCloseWindow(self, event):
       self.Destroy()

    def DoViewSelection(self, var, classname):
        if var == False:
            var = classname(self, editor)
        else:
            var.Close()
            var = False

        return var

    def DoClose(self, var):
        if var != False:
            var.Close()
        return False

editor = ROMEdit()
app = EvilScribe(0)
app.MainLoop()