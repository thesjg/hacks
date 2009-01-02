#!/usr/local/bin/python

from wxPython.wx import *
from wxPython.grid import *

from romedit import *
from config import *


class IdleSpeedFrame(wxMDIChildFrame):
    ID_IDLE = wxNewId()

    def __init__(self, parent, editor):
        wxMDIChildFrame.__init__(self, parent, -1, 'Idle Speed')
        self.parent = parent
        self.editor = editor

          # Retrieve current idle speed from the in-memory ROM image
        self.IdleSpeed = self.editor.get_idle_speed()

          # Create visible elements for this frame
        self.IdleSpeedLabel = wxStaticText(self, -1, 'Idle Speed: ')
        self.IdleSpeedTextCtrl = wxTextCtrl(self, self.ID_IDLE)
        self.IdleSpeedUnitLabel = wxStaticText(self, -1, 'RPM')
        self.OkButton = wxButton(self, wxID_OK, 'OK')

          # Setup events
        EVT_BUTTON(self, wxID_OK, self.OnOkButton)
        EVT_CLOSE(self, self.OnCloseWindow)

        self.SetProperties()
        self.DoLayout()

    def SetProperties(self):
        self.IdleSpeedTextCtrl.SetFocus()
        self.OkButton.SetDefault()
        self.IdleSpeedTextCtrl.SetValue(str(self.IdleSpeed))

    def DoLayout(self):
        MainSizer = wxBoxSizer(wxVERTICAL)

        IdleSpeedSizer = wxBoxSizer(wxHORIZONTAL)
        IdleSpeedSizer.Add(self.IdleSpeedLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5)
        IdleSpeedSizer.Add(self.IdleSpeedTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5)
        IdleSpeedSizer.Add(self.IdleSpeedUnitLabel, 0, wxALIGN_CENTER_VERTICAL)
        MainSizer.Add(IdleSpeedSizer, 0, wxALIGN_CENTER|wxALL, 10)

        OkButtonSizer = wxBoxSizer(wxVERTICAL)
        OkButtonSizer.Add(self.OkButton, 0, wxALIGN_CENTER)
        MainSizer.Add(OkButtonSizer, 0, wxEXPAND|wxALL, 10)

        self.SetAutoLayout(1)
        self.SetSizer(MainSizer)
        MainSizer.Fit(self)
        MainSizer.SetSizeHints(self)
        self.Layout()

    def OnOkButton(self, event):
        NewIdleSpeed = int(self.IdleSpeedTextCtrl.GetValue())

          # Check if new idle speed is valid
        if NewIdleSpeed > config.max_idle or NewIdleSpeed < config.min_idle:
            self.IdleSpeedError(NewIdleSpeed)
            return

        self.editor.set_idle_speed(NewIdleSpeed)
        self.IdleSpeed = self.editor.get_idle_speed()
        self.parent.SetStatusText('Idle speed changed to: %i RPM' %
                                 (self.IdleSpeed))
        self.IdleSpeedTextCtrl.SetValue(str(self.IdleSpeed))

    def IdleSpeedError(self, IdleSpeed):
        ErrorTitle = 'Idle Speed Error'
        ErrorText = ''

        if IdleSpeed > config.max_idle:
            ErrorText = 'Maximum allowable idle speed is: %i' % (config.max_idle)
        elif IdleSpeed < config.min_idle:
            ErrorText = 'Minimum allowable idle speed is: %i' % (config.min_idle)

        ErrorDialog = wxMessageDialog(self, ErrorText, ErrorTitle, wxOK)
        ErrorDialog.ShowModal()
        ErrorDialog.Destroy()
        EVT_CLOSE(self, self.OnCloseWindow)

    def OnCloseWindow(self, event):
        self.parent.idle_speed = False
        self.Destroy()
