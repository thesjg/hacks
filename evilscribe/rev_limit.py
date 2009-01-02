#!/usr/local/bin/python

from wxPython.wx import *
from wxPython.grid import *

from romedit import *
from config import *


class RevLimitFrame(wxMDIChildFrame):
    ID_REVLIMIT = wxNewId()

    def __init__(self, parent, editor):
        wxMDIChildFrame.__init__(self, parent, -1, 'Rev Limit')
        self.parent = parent
        self.editor = editor

          # Retrieve current rev limit from the in-memory ROM image
        self.RevLimit = editor.get_rev_limit()

          # Create visible elements for this frame
        self.RevLimitLabel = wxStaticText(self, -1, 'Rev Limit: ')
        self.RevLimitTextCtrl = wxTextCtrl(self, self.ID_REVLIMIT, '')
        self.RevLimitUnitLabel = wxStaticText(self, -1, 'RPM')
        self.OkButton = wxButton(self, wxID_OK, 'OK')

          # Setup events
        EVT_BUTTON(self, wxID_OK, self.OnOkButton)
        EVT_CLOSE(self, self.OnCloseWindow)

        self.SetProperties()
        self.DoLayout()

    def SetProperties(self):
        self.RevLimitTextCtrl.SetFocus()
        self.OkButton.SetDefault()
        self.RevLimitTextCtrl.SetValue(str(self.RevLimit))

    def DoLayout(self):
        MainSizer = wxBoxSizer(wxVERTICAL)

        RevLimitSizer = wxBoxSizer(wxHORIZONTAL)
        RevLimitSizer.Add(self.RevLimitLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5)
        RevLimitSizer.Add(self.RevLimitTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5)
        RevLimitSizer.Add(self.RevLimitUnitLabel, 0, wxALIGN_CENTER_VERTICAL)
        MainSizer.Add(RevLimitSizer, 0, wxALIGN_CENTER|wxALL, 10)

        OkButtonSizer = wxBoxSizer(wxVERTICAL)
        OkButtonSizer.Add(self.OkButton, 0, wxALIGN_CENTER)
        MainSizer.Add(OkButtonSizer, 1, wxEXPAND|wxALL, 10)

        self.SetAutoLayout(1)
        self.SetSizer(MainSizer)
        MainSizer.Fit(self)
        MainSizer.SetSizeHints(self)
        self.Layout()

    def OnOkButton(self, event):
        NewRevLimit = int(self.RevLimitTextCtrl.GetValue())

          # Check if new rev limit is valid
        if NewRevLimit > config.max_rev or NewRevLimit < config.min_rev:
            self.RevLimitError(NewRevLimit)
            return

        self.editor.set_rev_limit(NewRevLimit)
        self.RevLimit = self.editor.get_rev_limit()
        self.parent.SetStatusText('Rev Limit changed to: %i RPM' %
                                 (self.RevLimit))
        self.RevLimitTextCtrl.SetValue(str(self.RevLimit))

    def RevLimitError(self, RevLimit):
        ErrorTitle = 'Rev Limit Error'
        ErrorText = ''

        if RevLimit > config.max_rev:
            ErrorText = 'Maximum allowable rev limit is: %i' % (config.max_rev)
        elif RevLimit < config.min_rev:
            ErrorText = 'Minimum allowable rev limit is: %i' % (config.min_rev)

        ErrorDialog = wxMessageDialog(self, ErrorText, ErrorTitle, wxOK)
        ErrorDialog.ShowModal()
        ErrorDialog.Destroy()
        EVT_CLOSE(self, self.OnCloseWindow)

    def OnCloseWindow(self, event):
        self.parent.rev_limit = False
        self.Destroy()
