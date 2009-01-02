#!/usr/local/bin/python

from wxPython.wx import *
from wxPython.grid import *

from romedit import *
from config import *


class MAFCompensationFrame(wxMDIChildFrame):
    ID_SELECT = wxNewId()

    def __init__(self, parent, editor):
        wxMDIChildFrame.__init__(self, parent, -1, 'MAF')
        self.parent = parent
        self.editor = editor


          # Make room for MAF compensation and scalar wxTextCtrl's
        self.CompTextCtrls = []
        self.ScalarTextCtrls = []

          # List of choices for wxComboBox
        self.MAFChoices = []

          # Old value storage
        self.OldComp = []
        self.OldScalar = []
        self.OldMultiplier = []
        self.GetOldValues()


          # Create visible elements for this frame

          # Compensation
        #self.CompLabel = wxStaticText(self, -1, 'Compensation:')
        for i in range(len(config.mafs[0][0])):
            self.CompTextCtrls.append(wxTextCtrl(self, 0, '', wxPoint(-1, -1),
                                                 wxSize(50, -1), wxTE_CENTER))

          # Scalar
        #self.ScalarLabel = wxStaticText(self, -1, 'Scalar:')
        for i in range(len(config.mafs[0][1])):
            self.ScalarTextCtrls.append(wxTextCtrl(self, 0, '', wxPoint(-1, -1),
                                                   wxSize(50, -1), wxTE_CENTER))

          # Multiplier
        #self.MultiLabel = wxStaticText(self, -1, 'Multiplier:')
        self.MultiTextCtrl = wxTextCtrl(self, 0, '', wxPoint(-1, -1),
                                        wxSize(50, -1), wxTE_CENTER)

          # MAF selection combo box
        self.SelectLabel = wxStaticText(self, -1, 'Select MAF:')

        for i in config.mafs:
            self.MAFChoices.append(i[3])

        self.SelectComboBox = wxComboBox(self, self.ID_SELECT, '',
                                         size=wxSize(200, -1),
                                         choices=self.MAFChoices,
                                         style=wxCB_READONLY)

          # Main buttons
        self.OkButton = wxButton(self, wxID_OK, 'OK')
        self.ResetButton = wxButton(self, wxID_CANCEL, 'Reset')

          # Setup events
        EVT_COMBOBOX(self, self.ID_SELECT, self.OnComboBoxSelect)
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

          # Compensation
        CompBox = wxStaticBox(self, -1, 'Set Mass Air Compensation')
        CompMainSizer = wxStaticBoxSizer(CompBox, wxVERTICAL)
        CompSizer = wxBoxSizer(wxHORIZONTAL)

        CompRow1GridSizer = wxGridSizer(1, 8, 0, 5)
        CompRow2GridSizer = wxGridSizer(1, 8, 0, 5)
        CompRow3GridSizer = wxGridSizer(1, 5, 0, 5)

        for i in range(8):
            CompRow1GridSizer.Add(self.CompTextCtrls[i])

        for i in range(8, 16):
            CompRow2GridSizer.Add(self.CompTextCtrls[i])

        for i in range(16, 21):
            CompRow3GridSizer.Add(self.CompTextCtrls[i])

        CompMainSizer.Add(CompRow1GridSizer, 0, wxALIGN_CENTER|wxALL, 2)
        CompMainSizer.Add(CompRow2GridSizer, 0, wxALIGN_CENTER|wxALL, 2)
        CompMainSizer.Add(CompRow3GridSizer, 0, wxALIGN_CENTER|wxALL, 2)
        CompMainSizer.Add(CompSizer, 1, wxALIGN_CENTER)
        MainSizer.Add(CompMainSizer, 0, wxEXPAND|wxALL, 10)

          # Scalar
        ScalarBox = wxStaticBox(self, -1, 'Set Mass Air Scalar')
        ScalarMainSizer = wxStaticBoxSizer(ScalarBox, wxVERTICAL)
        ScalarSizer = wxBoxSizer(wxHORIZONTAL)

        ScalarGridSizer = wxGridSizer(1, 2, 0, 5)
        ScalarGridSizer.Add(self.ScalarTextCtrls[0])
        ScalarGridSizer.Add(self.ScalarTextCtrls[1])

        ScalarSizer.Add(ScalarGridSizer, 0, wxALIGN_CENTER|wxALL, 5)
        ScalarMainSizer.Add(ScalarSizer, 1, wxALIGN_CENTER)
        MainSizer.Add(ScalarMainSizer, 0, wxEXPAND|wxALL, 10)


          # Multiplier
        MultiBox = wxStaticBox(self, -1, 'Set Mass Air Multiplier')
        MultiMainSizer = wxStaticBoxSizer(MultiBox, wxVERTICAL)
        MultiSizer = wxBoxSizer(wxHORIZONTAL)

        MultiSizer.Add(self.MultiTextCtrl, 0, wxALIGN_CENTER|wxALL, 5)

        MultiMainSizer.Add(MultiSizer, 1, wxALIGN_CENTER)
        MainSizer.Add(MultiMainSizer, 0, wxEXPAND|wxALL, 10)

          # Select
        SelectMainSizer = wxBoxSizer(wxVERTICAL)
        SelectSizer = wxBoxSizer(wxHORIZONTAL)
        SelectSizer.Add(self.SelectLabel, 0, wxALIGN_CENTER|wxALL, 5)
        SelectSizer.Add(self.SelectComboBox, 0, wxALIGN_CENTER)
        SelectMainSizer.Add(SelectSizer, 1, wxALIGN_CENTER)
        MainSizer.Add(SelectMainSizer, 0, wxEXPAND|wxALL, 10)

          # Main Buttons
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

        j = 0
        for i in self.CompTextCtrls:
            i.SetValue(str(self.OldComp[j]))
            j = j + 1

        j = 0
        for i in self.ScalarTextCtrls:
            i.SetValue(str(self.OldScalar[j]))
            j = j + 1

        self.MultiTextCtrl.SetValue(str(self.OldMultiplier))


    def GetOldValues(self):
        for i in range(len(config.offset_maf_compensation)):
            self.OldComp.append(self.editor.get_maf_comp(i))

        for i in range(len(config.offset_maf_scalar)):
            self.OldScalar.append(self.editor.get_maf_scalar(i))

        self.OldMultiplier = self.editor.get_maf_multiplier()

    def OnComboBoxSelect(self, event):
        MAFID = event.GetSelection()

        MAF = config.mafs[MAFID]
        MAFComp = MAF[0]
        MAFScalar = MAF[1]
        MAFMulti = MAF[2]

        j = 0
        for i in self.CompTextCtrls:
            i.SetValue(str(long(MAFComp[j], 16)))
            j = j + 1

        j = 0
        for i in self.ScalarTextCtrls:
            i.SetValue(str(long(MAFScalar[j], 16)))

        self.MultiTextCtrl.SetValue(str(long(MAFMulti, 16)))

    def OnOkButton(self, event):
        MAFComp = []
        MAFScalar = []
        MAFMulti = 0

        j = 0
        for i in self.CompTextCtrls:
            self.editor.set_maf_comp(j, int(i.GetValue()))
            j = j + 1

        j = 0
        for i in self.ScalarTextCtrls:
            self.editor.set_maf_scalar(j, int(i.GetValue()))

        MAFMulti = int(self.MultiTextCtrl.GetValue())
        self.editor.set_maf_multiplier(MAFMulti)

        self.parent.SetStatusText('MAF compensation values have been updated')

        self.GetOldValues()
        self.RestoreOldValues()

    def OnResetButton(self, event):
        self.RestoreOldValues()

    def OnCloseWindow(self, event):
        self.parent.maf = False
        self.Destroy()
