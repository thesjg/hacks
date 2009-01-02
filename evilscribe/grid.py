#!/usr/local/bin/python

import wx, wx.grid as grid

from romedit import *
from config import *


class GridSplitter(wx.SplitterWindow):
    def __init__(self, parent):
        wx.SplitterWindow.__init__(self, parent, -1, style = wx.SP_3D)
        self.SetMinimumPaneSize(8)


class ESGridTableBase(grid.PyGridTableBase):
    def __init__(self, parent, editor, type):
        grid.PyGridTableBase.__init__(self)
        self.Parent = parent
        self.Editor = editor
        self.Type = type

        if self.Type == 'fuel':
            self.NumRows = len(config.grid_fuel_rows)
            self.NumCols = len(config.grid_fuel_cols)
            self.Rows = config.grid_fuel_rows
            self.Cols = config.grid_fuel_cols
        elif self.Type == 'timing':
            self.NumRows = len(config.grid_timing_rows)
            self.NumCols = len(config.grid_timing_cols)
            self.Rows = config.grid_timing_rows
            self.Cols = config.grid_timing_cols

    def GetNumberRows(self):
        return self.NumRows

    def GetNumberCols(self):
        return self.NumCols

    def IsEmptyCell(self, row, col):
        return False

    def GetValue(self, row, col):
        return 0

    def GetColLabelValue(self, col):
        return self.Cols[col]

    def GetRowLabelValue(self, row):
        return self.Rows[row] + ' gm/rev'


class ESGrid(grid.Grid):
    def __init__(self, parent, editor, type):
        grid.Grid.__init__(self, parent, -1)
        self.Parent = parent
        self.Editor = editor
        self.Type = type

        if self.Type == 'fuel':
            self.NumRows = len(config.grid_fuel_rows)
            self.NumCols = len(config.grid_fuel_cols)
        elif self.Type == 'timing':
            self.NumRows = len(config.grid_timing_rows)
            self.NumCols = len(config.grid_timing_cols)

        self.SetDefaultRowSize(20)
        self.SetDefaultColSize(35)
        self.SetMargins(0, 0)
        self.EnableDragColSize(False)
        self.EnableDragRowSize(False)
        self.EnableDragGridSize(False)

        self.color_max = 0
        self.color_min = 0
        self.color_adder = 0

#        self.Bind(wx.EVT_MOTION, self.ShowTooltip, self.GetGridWindow())
        wx.EVT_MOTION(self.GetGridWindow(), self.ShowTooltip)

    def ShowTooltip(self, evt):
        x, y = evt.GetPosition()
        row = self.YToRow(y)
        col = self.XToCol(x)

        cell_value = 0
        gm_rev = 0
        rpm = 0

        if self.Type == 'fuel':
            cell_value = self.Editor.fuel_table_get(row, col)
            gm_rev = float(config.grid_fuel_rows[row])
            rpm = int(config.grid_fuel_cols[col])
        else:
            cell_value = self.Editor.timing_table_get(row, col)
            gm_rev = float(config.grid_timing_rows[row])
            rpm = int(config.grid_timing_cols[col])
        lb_min = "%2.2f" % ((gm_rev*.0022046)*rpm)

        self.GetGridWindow().SetToolTipString(lb_min + ' lb/min')
        evt.Skip()

    def UpdateColors(self):
        self.CalcColors()
        self.SetColors()
        self.ForceRefresh()

    def CalcColors(self):
        if self.Type == 'fuel':
            self.color_max = int(self.Editor.fuel_table_get(0, 0))
            self.color_min = int(self.Editor.fuel_table_get(0, 0))
        else:
            self.color_max = int(self.Editor.timing_table_get(0, 0))
            self.color_min = int(self.Editor.timing_table_get(0, 0))

        for row in range(self.NumRows):
            for col in range(self.NumCols):

                if self.Type == 'fuel':
                    val = int(self.Editor.fuel_table_get(row, col))
                else:
                    val = int(self.Editor.timing_table_get(row, col))

                if val > self.color_max:
                    self.color_max = val

                if val < self.color_min:
                    self.color_min = val

        if self.color_min < 0:
            if self.color_adder < abs(self.color_min):
                self.color_adder = abs(self.color_min)
                self.color_max = self.color_max + abs(self.color_min)
                self.color_min = 0

    def SetColors(self):
        multiplier = 255 / (self.color_max - self.color_min)

        for row in range(self.NumRows):
            for col in range(self.NumCols):

                if self.Type == 'fuel':
                    val = int(self.Editor.fuel_table_get(row, col)) + self.color_adder
                    color = abs((val - self.color_min)*multiplier - 255)
                else:
                    val = int(self.Editor.timing_table_get(row, col)) + self.color_adder
                    color = (val - self.color_min) * multiplier

                if color > 255:
                    color = 255
                if color < 0:
                    color = 0

                if self.Type == 'fuel':
                    self.SetCellBackgroundColour(row, col, wx.Colour(0, 0, color))
                else:
                    self.SetCellBackgroundColour(row, col, wx.Colour(color, 0, 0))
                self.SetCellTextColour(row, col, wx.Colour(255, 255, 255))


class FuelViewGridTableBase(ESGridTableBase):
    def __init__(self, parent, editor):
        ESGridTableBase.__init__(self, parent, editor, 'fuel')
        self.Parent = parent
        self.Editor = editor

    def GetValue(self, row, col):

        if self.Parent.TargetAFR == True:
            return "%2.2f" % (14.7/(float(self.Editor.fuel_table_get(row, col))/128))
        else:
            return self.Editor.fuel_table_get(row, col)

    def SetValue(self, row, col, value):
        self.Editor.fuel_table_set(row, col, value)
        self.Parent.UpdateColors()

class FuelViewGrid(ESGrid):
    def __init__(self, parent, editor):
        ESGrid.__init__(self, parent, editor, 'fuel')

        self.TargetAFR = False

        self.grid = FuelViewGridTableBase(self, editor)
        self.SetTable(self.grid, True)

        self.UpdateColors()

class FuelControl(wx.Window):

    SpinValue = 1

    def __init__(self, parent, editor, grid):
        wx.Window.__init__(self, parent)
        self.Editor = editor
        self.Grid = grid

        self.Text = wx.TextCtrl(self, -1, "1", (4, 5), (35, -1))
        h = self.Text.GetSize().height
        w = self.Text.GetSize().width + self.Text.GetPosition().x
        self.Text.SetValue(str(self.SpinValue) + ' %')

        self.Spin = wx.SpinButton(self, -1, (w + 4, 5), (h/2, h), wx.SP_VERTICAL)
        self.Plus = wx.Button(self, wx.NewId(), '+', size=(20, 20))
        self.Minus = wx.Button(self, wx.NewId(), '-', size=(20, 20))
        self.AFToggle = wx.Button(self, wx.NewId(), 'Toggle Target AFR')

        self.Bind(wx.EVT_SPIN, self.OnSpin, self.Spin)
        self.Bind(wx.EVT_BUTTON, self.OnClickPlus, self.Plus)
        self.Bind(wx.EVT_BUTTON, self.OnClickMinus, self.Minus)
        self.Bind(wx.EVT_BUTTON, self.OnClickAF, self.AFToggle)

        self.SetProperties()
        self.DoLayout()

    def SetProperties(self):
        self.Spin.SetRange(1, 9)
        self.Spin.SetValue(self.SpinValue)
        self.Plus.SetToolTipString('Add set percentage to selected cells')
        self.Plus.SetSize((5, 5))
        self.Minus.SetToolTipString('Subtract set percentage to selected cells')
        self.Minus.SetSize((5, 5))

    def DoLayout(self):
        MainSizer = wx.BoxSizer(wx.VERTICAL)

        PMSizer = wx.BoxSizer(wx.HORIZONTAL)
        PMSizer.Add(self.Text, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5)
        PMSizer.Add(self.Spin, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5)
        PMSizer.Add(self.Plus, 0, wx.ALIGN_CENTER_VERTICAL)
        PMSizer.Add(self.Minus, 0, wx.ALIGN_CENTER_VERTICAL)
        PMSizer.Add(self.AFToggle, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT)
        MainSizer.Add(PMSizer, 0, wx.ALIGN_CENTER|wx.ALL, 10)

        self.SetAutoLayout(1)
        self.SetSizer(MainSizer)
        MainSizer.Fit(self)
        MainSizer.SetSizeHints(self)
        self.Layout()

    def OnSpin(self, event):
        self.SpinValue = event.GetPosition()
        self.text.SetValue(str(event.GetPosition()) + ' %')

    def OnClickPlus(self, event):
        for row in range(len(config.grid_fuel_rows)):
            for col in range(len(config.grid_fuel_cols)):
                if self.Grid.IsInSelection(row, col):
                    val = int(self.Grid.GetCellValue(row, col))
                    mod = float("1.0%d" % (self.SpinValue))
                    newvalue = int(val*mod)
                    self.Grid.SetCellValue(row, col, str(newvalue))

    def OnClickMinus(self, event):
        for row in range(len(config.grid_fuel_rows)):
            for col in range(len(config.grid_fuel_cols)):
                if self.Grid.IsInSelection(row, col):
                    val = int(self.Grid.GetCellValue(row, col))
                    mod = float("1.0%d" % (self.SpinValue))
                    newvalue = val-(int(val*mod)-val)
                    self.Grid.SetCellValue(row, col, str(newvalue))

    def OnClickAF(self, event):
        if self.Grid.TargetAFR == True:
            self.Grid.TargetAFR = False
        else:
            self.Grid.TargetAFR = True

        self.Grid.ForceRefresh()


class FuelView(wx.MDIChildFrame):
    def __init__(self, parent, editor):
        wx.MDIChildFrame.__init__(self, parent, -1, 'Fuel')
        self.parent = parent
        self.editor = editor

        splitter = GridSplitter(self)

        grid = FuelViewGrid(splitter, editor)
        control = FuelControl(splitter, self.editor, grid)

        splitter.SplitHorizontally(grid, control, -30)

        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)
#        wx.EVT_CLOSE(self, self.OnCloseWindow)

    def OnCloseWindow(self, event):
        self.parent.fuel = False
        self.Destroy()



class TimingViewGridTableBase(ESGridTableBase):
    def __init__(self, parent, editor):
        ESGridTableBase.__init__(self, parent, editor, 'timing')

    def GetValue(self, row, col):
        return self.Editor.timing_table_get(row, col)

    def SetValue(self, row, col, value):
        self.Editor.timing_table_set(row, col, value)
        self.Parent.UpdateColors()

class TimingViewGrid(ESGrid):
    def __init__(self, parent, editor):
        ESGrid.__init__(self, parent, editor, 'timing')

        self.grid = TimingViewGridTableBase(self, editor)
        self.SetTable(self.grid, True)

        self.UpdateColors()

class TimingView(wx.MDIChildFrame):
    def __init__(self, parent, editor):
        wx.MDIChildFrame.__init__(self, parent, -1, 'Timing')
        self.parent = parent
        self.editor = editor

        self.grid = TimingViewGrid(self, editor)

        wx.EVT_CLOSE(self, self.OnCloseWindow)

    def OnCloseWindow(self, event):
        self.parent.timing = False
        self.Destroy()