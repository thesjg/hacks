import wx
import wx.html

class AboutDialog(wx.Dialog):
    def __init__(self, parent, config):
        wx.Dialog.__init__(self, parent, -1, 'About EvilScribe version ' +
                           config.app_version)

        html = wx.html.HtmlWindow(self, -1, size=(640, -1))
        html.LoadPage('http://www.evilscribe.net')
        ir = html.GetInternalRepresentation()
        html.SetSize((ir.GetWidth()+24, ir.GetHeight()+25))
        self.SetClientSize(html.GetSize())
        self.CentreOnParent(wx.BOTH)
