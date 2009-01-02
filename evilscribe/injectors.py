#!/usr/local/bin/python

from wxPython.wx import *
from wxPython.grid import *

from romedit import *
from config import *



class InjectorFrame(wxMDIChildFrame):
    ID_DEADTIME_OK = wxNewId()
    ID_SELECT = wxNewId()

    def __init__(self, parent, editor):
        wxMDIChildFrame.__init__(self, parent, -1, 'Injectors')
        self.parent = parent
        self.editor = editor

        self.OldInjectorSize = 0
        self.OldDeadtime = []
        self.GetOldValues()

          # Make room for deadtime wxTextCtrl's
        self.DeadtimeTextCtrls = []

          # Make room for our injector list
        self.InjectorChoices = []


          # Create visible elements for this frame

          # Injector Size
        self.SizeLabel = wxStaticText(self, -1, 'Injector size:')
        self.SizeTextCtrl = wxTextCtrl(self, -1, style=wxTE_RIGHT)
        self.SizeUnitLabel = wxStaticText(self, -1, 'cc')

          # Create deadtime wxTextCtrl's
        for i in config.grid_injector_deadtime:
            self.DeadtimeTextCtrls.append(wxTextCtrl(self, 0, '',
                                          wxPoint(-1, -1), wxSize(50, -1),
                                          wxTE_CENTER))
        self.DeadtimeAddLabel = wxStaticText(self, -1, 'Add deadtime:')
        self.DeadtimeAddTextCtrl = wxTextCtrl(self, 0, '', style=wxTE_RIGHT)
        self.DeadtimeAddUnitLabel = wxStaticText(self, -1, 'usec')
        self.DeadtimeAddOkButton = wxButton(self, self.ID_DEADTIME_OK, 'OK')

          # Pre-defined injector selection combo box
        self.SelectLabel = wxStaticText(self, -1, 'Select Injectors:')

        for i in config.injectors:
            self.InjectorChoices.append(i[9] + ' ' + i[0] + 'cc')

        self.SelectComboBox = wxComboBox(self, self.ID_SELECT, '',
                                         size=wxSize(200, -1),
                                         choices=self.InjectorChoices,
                                         style=wxCB_READONLY)

          # Set up main buttons
        self.OkButton = wxButton(self, wxID_OK, 'OK')
        self.ResetButton = wxButton(self, wxID_CANCEL, 'Reset')

          # Setup events
        EVT_COMBOBOX(self, self.ID_SELECT, self.OnComboBoxSelect)
        EVT_BUTTON(self, self.ID_DEADTIME_OK, self.OnAddDeadtime)
        EVT_BUTTON(self, wxID_OK, self.OnOkButton)
        EVT_BUTTON(self, wxID_CANCEL, self.OnResetButton)
        EVT_CLOSE(self, self.OnCloseWindow)

        self.SetProperties()
        self.DoLayout()

        self.RestoreOldValues()

    def SetProperties(self):
        self.SetFocus()

    def DoLayout(self):
        MainSizer = wxBoxSizer(wxVERTICAL)

          # Size
        SizeBox = wxStaticBox(self, -1, 'Set Injector Size')
        SizeMainSizer = wxStaticBoxSizer(SizeBox, wxVERTICAL)
        SizeSizer = wxBoxSizer(wxHORIZONTAL)
        SizeSizer.Add(self.SizeLabel, 0, wxALIGN_CENTER|wxALL, 5)
        SizeSizer.Add(self.SizeTextCtrl, 0, wxALIGN_CENTER|wxALL, 5)
        SizeSizer.Add(self.SizeUnitLabel, 0, wxALIGN_CENTER|wxALL)
        SizeMainSizer.Add(SizeSizer, 1, wxALIGN_CENTER)
        MainSizer.Add(SizeMainSizer, 0, wxEXPAND|wxALL, 10)

          # Deadtime
        DeadtimeBox = wxStaticBox(self, -1, 'Manage Injector Deadtime')
        DeadtimeMainSizer = wxStaticBoxSizer(DeadtimeBox, wxVERTICAL)
        DeadtimeAddSizer = wxBoxSizer(wxHORIZONTAL)
        DeadtimeSizer = wxBoxSizer(wxHORIZONTAL)

        DeadtimeGridSizer = wxGridSizer(2, 7, 0, 10)

        for i in config.grid_injector_deadtime:
            DeadtimeGridSizer.Add(wxStaticText(self, -1, i + 'v'), 0, wxALIGN_CENTER)

        for i in self.DeadtimeTextCtrls:
            DeadtimeGridSizer.Add(i, 0, wxALIGN_CENTER)

        DeadtimeSizer.Add(DeadtimeGridSizer, 0, wxALIGN_CENTER)

        DeadtimeAddSizer.Add(self.DeadtimeAddLabel, 0, wxALIGN_CENTER|wxALL, 5)
        DeadtimeAddSizer.Add(self.DeadtimeAddTextCtrl, 0, wxALIGN_CENTER|wxALL, 5)
        DeadtimeAddSizer.Add(self.DeadtimeAddUnitLabel, 0, wxALIGN_CENTER)
        DeadtimeAddSizer.Add(self.DeadtimeAddOkButton, 0, wxALIGN_CENTER|wxALL, 5)

        DeadtimeMainSizer.Add(DeadtimeSizer, 1, wxALIGN_CENTER)
        DeadtimeMainSizer.Add(DeadtimeAddSizer, 1, wxALIGN_CENTER|wxALL, 10)
        MainSizer.Add(DeadtimeMainSizer, 0, wxEXPAND|wxALL, 10)

          # Select
        SelectMainSizer = wxBoxSizer(wxVERTICAL)
        SelectSizer = wxBoxSizer(wxHORIZONTAL)
        SelectSizer.Add(self.SelectLabel, 0, wxALIGN_CENTER|wxALL, 5)
        SelectSizer.Add(self.SelectComboBox, 0, wxALIGN_CENTER)
        SelectMainSizer.Add(SelectSizer, 1, wxALIGN_CENTER)
        MainSizer.Add(SelectMainSizer, 0, wxEXPAND|wxALL, 10)

          # Main buttons
        ButtonMainSizer = wxBoxSizer(wxVERTICAL)
        ButtonSizer = wxBoxSizer(wxHORIZONTAL)
        ButtonSizer.Add(self.OkButton, 0, wxALL, 10)
        ButtonSizer.Add(self.ResetButton, 0, wxALL, 10)
        ButtonMainSizer.Add(ButtonSizer, 1, wxALIGN_CENTER)
        MainSizer.Add(ButtonMainSizer, 1, wxEXPAND)

        self.SetAutoLayout(1)
        self.SetSizer(MainSizer)
        MainSizer.Fit(self)
        MainSizer.SetSizeHints(self)
        self.Layout()

    def RestoreOldValues(self):
        self.SizeTextCtrl.SetValue(str(self.OldInjectorSize))

        j = 0
        for i in self.DeadtimeTextCtrls:
            i.SetValue(str(self.OldDeadtime[j]))
            j = j + 1

    def GetOldValues(self):
          # Retrieve current injector size from the in-memory ROM image
        self.OldInjectorSize = self.editor.get_injector_size()

          # Retrieve deadtime map
        self.OldDeadtime = []
        for i in range(len(config.offset_injector_deadtime)):
            self.OldDeadtime.append(self.editor.get_injector_deadtime(i))

    def OnComboBoxSelect(self, event):
        InjID = event.GetSelection()

        InjSize = config.injectors[InjID][1]
        self.SizeTextCtrl.SetValue(str(InjSize))

        j = 2
        for i in self.DeadtimeTextCtrls:
            i.SetValue(str(config.injectors[InjID][j]))
            j = j + 1

    def OnAddDeadtime(self, event):
        AddVal = int(self.DeadtimeAddTextCtrl.GetValue())

        for i in self.DeadtimeTextCtrls:
            NewVal = int(i.GetValue()) + AddVal
            i.SetValue(str(NewVal))

        self.DeadtimeAddTextCtrl.SetValue('')

    def OnOkButton(self, event):
        InjSize = int(self.SizeTextCtrl.GetValue())

        Deadtimes = []
        for i in self.DeadtimeTextCtrls:
            Deadtimes.append(int(i.GetValue()))

        self.editor.set_injector_size(InjSize)

        j = 0
        for i in Deadtimes:
            self.editor.set_injector_deadtime(j, i)
            j = j + 1

        self.parent.SetStatusText('Fuel injectors have been updated')

        self.GetOldValues()
        self.RestoreOldValues()

    def OnResetButton(self, event):
        self.RestoreOldValues()

    def OnCloseWindow(self, event):
        self.parent.injectors = False
        self.Destroy()
