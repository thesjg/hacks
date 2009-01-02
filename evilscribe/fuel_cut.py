#!/usr/local/bin/python

from wxPython.wx import *
from wxPython.grid import *

from romedit import *
from config import *


class FuelCutFrame(wxMDIChildFrame):
    ID_FUEL = wxNewId()

    def __init__(self, parent, editor):
        wxMDIChildFrame.__init__(self, parent, -1, 'Fuel Cut')
        self.parent = parent
        self.editor = editor

        self.FuelCutChoices = ['Enable', 'Disable']
        self.FuelCutChoice = 0
        if self.editor.fuel_cut_status() == True:
            self.FuelCutChoice = 1

          # Create visible elements for this frame
        self.FuelCutValueLabel = wxStaticText(self, -1, 'Fuel cut value:')
        self.FuelCutTextCtrl = wxTextCtrl(self, 0, '', wxPoint(-1, -1),
                                          wxSize(100, -1), wxTE_CENTER)

        self.FuelCutLabel = wxStaticText(self, -1, 'Fuel cut:')
        self.FuelCutComboBox = wxComboBox(self, self.ID_FUEL, '',
                                          size=wxSize(100, -1),
                                          choices=self.FuelCutChoices,
                                          style=wxCB_READONLY)
        self.OkButton = wxButton(self, wxID_OK, 'OK')

          # Setup events
        EVT_COMBOBOX(self, self.ID_FUEL, self.OnComboBoxSelect)
        EVT_BUTTON(self, wxID_OK, self.OnOkButton)
        EVT_CLOSE(self, self.OnCloseWindow)

        self.SetProperties()
        self.DoLayout()

    def SetProperties(self):
        self.FuelCutComboBox.SetFocus()
        self.OkButton.SetDefault()
        self.FuelCutComboBox.SetSelection(self.FuelCutChoice)
        self.FuelCutTextCtrl.SetValue(str(long(self.editor.fuel_cut_get_value(), 16)))

    def DoLayout(self):
        MainSizer = wxBoxSizer(wxVERTICAL)

        FuelCutValueSizer = wxBoxSizer(wxHORIZONTAL)
        FuelCutValueSizer.Add(self.FuelCutValueLabel, 0, wxALIGN_CENTER|wxALL, 5)
        FuelCutValueSizer.Add(self.FuelCutTextCtrl, 0, wxALIGN_CENTER|wxALL, 5)

        FuelCutComboSizer = wxBoxSizer(wxHORIZONTAL)
        FuelCutComboSizer.Add(self.FuelCutLabel, 0, wxALIGN_CENTER|wxALL, 5)
        FuelCutComboSizer.Add(self.FuelCutComboBox, 0, wxALIGN_CENTER|wxALL, 5)

        MainSizer.Add(FuelCutValueSizer, 0, wxALIGN_CENTER|wxALL, 10)
        MainSizer.Add(FuelCutComboSizer, 0, wxALIGN_CENTER|wxALL, 10)

        OkButtonSizer = wxBoxSizer(wxVERTICAL)
        OkButtonSizer.Add(self.OkButton, 0, wxALIGN_CENTER)
        MainSizer.Add(OkButtonSizer, 1, wxEXPAND|wxALL, 10)

        self.SetAutoLayout(1)
        self.SetSizer(MainSizer)
        MainSizer.Fit(self)
        MainSizer.SetSizeHints(self)
        self.Layout()

    def OnOkButton(self, event):
        FuelCutValue = int(self.FuelCutTextCtrl.GetValue())
        self.editor.fuel_cut_set_value(FuelCutValue)
        self.parent.SetStatusText('Fuel cut value has been changed to: %i' %
                                  (FuelCutValue))

    def OnComboBoxSelect(self, event):
        self.FuelCutChoice = event.GetSelection()
        if self.FuelCutChoice == 0:
            self.FuelCutTextCtrl.SetValue(str(config.magic_fuel_cut_enable))
        else:
            self.FuelCutTextCtrl.SetValue(str(config.magic_fuel_cut_disable))
        EVT_CLOSE(self, self.OnCloseWindow)

    def OnCloseWindow(self, event):
        self.parent.fuel_cut = False
        self.Destroy()
