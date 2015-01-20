import wxversion
wxversion.select('3.0')
import serial
import wx
import re
import os

#
# KeyPunchInterface program to drive the KPgolem interface box in an IBM keypunch (e.g. 029)
#         see https://github.com/cclaunch/KPgolem for details and other materials
#

def kpinit(mylink, myport):
    mylink.baudrate = 9600
    mylink.parity = serial.PARITY_NONE
    mylink.stopbits = serial.STOPBITS_ONE
    mylink.bytesize = serial.EIGHTBITS
    mylink.xonxoff = True
    mylink.port = myport
    mylink.timeout = 0
    mylink.writeTimeout = 2
    mylink.open()
    if (mylink.isOpen() == False):
        KPapp.connected = True
        KPapp.errstatus = "our port did not open, aborting"
        KPapp.myconfig.DeleteEntry("Serial/Port")
        KPapp.myconfig.Flush()
        noKeypunch()
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
    return

def portlist():
    try:
        from serial.tools.list_ports import comports
    except ImportError:
        comports = None
    thelist = []
    for port, desc, hwid in sorted(comports()):
        thelist.append([port, desc, hwid])
    return thelist

# routine to start the keypunch punching or reading, either initially or after it is paused
def startpunch(self):
    if (KPapp.statfile == "CLOSED"):
        KPapp.errstatus = "No file open, can't start"
    else:
        KPapp.errstatus = "Starting to punch cards"
        KPapp.gopunch = True
        KPapp.pause = False
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    wx.LogGeneric(wx.LOG_User, "Starting or restarting punching of cards at card " + KPapp.myfilecurrentstr + "\n")
    return

# routine to pause the keypunch when it is punching or reading a deck
def pausepunch(self):
    if (KPapp.statfile == "CLOSED"):
        KPapp.errstatus = "No file open, can't pause"
    else:
        KPapp.errstatus = "Keypunch now paused"
        KPapp.pause = True
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    wx.LogGeneric(wx.LOG_User, "Pausing keypunch at card " + KPapp.myfilecurrentstr + "\n")
    return

# routine to start reading cards from the keypunch
def startread(self):
    if (KPapp.statfile == "CLOSED"):
        KPapp.errstatus = "No file open, can't start"
    elif (KPapp.readfile == False):
        KPapp.errstatus = "File opened for punching not ready"
    else:
        KPapp.errstatus = "Starting to read cards"
        KPapp.goread = True
        KPapp.pause = False
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    wx.LogGeneric(wx.LOG_User, "Starting or restarting read of cards from keypunch into file, at card " + KPapp.myfilecurrentstr + "\n")
    return

# routine to stop the keypunch when it has read the last card
def stoppunch(self):
    if (KPapp.statfile == "CLOSED"):
        KPapp.errstatus = "No file open, nothing to stop"
    else:
        KPapp.errstatus = "End of deck"
        KP.goread = False
        KP.pause = False
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    wx.LogGeneric(wx.LOG_User, "Ending the reading of cards from keypunch into file, length of deck is now " + KPapp.myfilelenstr + "\n")
    return

# routine to reposition a paused keypunch to a user selected card number
def goto(self):
    # must have an open file and be paused in order to reposition
    if (KPapp.statfile == "CLOSED"):
        KPapp.errstatus = "Must have file open to reposition"
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
        return
    elif (KPapp.pause == False):
        KPapp.errstatus = "Keypunch must be paused to reposition file"
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
        return

    # throw up the dialog box asking for card number to set as current position of file
    mydlg = wx.TextEntryDialog(KPapp.statpane,"Enter card number to which the file will be set")
    mystatus = mydlg.ShowModal()

    # user cancelled, go away
    if (mystatus == wx.ID_CANCEL):
        mydlg.Destroy()
        return
    myans = mydlg.GetValue()

    # validate this as a decimal number between 1 and file length
    if (re.search("^ *[0-9]+ *$",myans) == None):    # get None if not a dec number
        KPapp.errstatus = "Must be a number"
        wx.LogGeneric(wx.LOG_User, "Goto command but input " + myans + " not a number\n")
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
        mydlg.Destroy()
        return
    mycard = int(myans)
    if ((mycard < 1) or (mycard > KPapp.myfilelen)):
        KPapp.errstatus = "Requested card number not within file"
        wx.LogGeneric(wx.LOG_User, "Goto command but card " + myans + " not in deck\n")
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
        mydlg.Destroy()
        return

    # set the current card to this value and scroll the display
    mymove = mycard - KPapp.myfilecurrent
    KPapp.myfilecurrent = mycard
    KPapp.myfilecurrentstr = myans
    KPapp.viewpane.ScrollLines(mymove)
    KPapp.viewpane.Refresh()
    KPapp.viewpane.Update()
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    wx.LogGeneric(wx.LOG_User, "Goto command repositioned deck to card " + KPapp.myfilecurrentstr + "\n")

    # dump dialog and go away
    mydlg.Destroy()
    return

# forget the saved serial port
def dropport(self):
    KPapp.myconfig.DeleteEntry("Serial/Port")
    KPapp.myconfig.Flush()
    KPapp.errstatus = "Serial port forgotten, restart to select new one"
    wx.LogGeneric(wx.LOG_User, "User asked to remove serial port from init file, must restart now\n")
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    noKeypunch(self)
    return

# routine to set encoding to BCD as ASCII characters
def setbcd(self):
    KPapp.goodchars = " !" + '"' + "#$%&'()*+,-./0123456789:<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n"
    KPapp.isbinary = 0
    KPapp.myconfig.Write('Encoding/Code', 'BCD')
    KPapp.myconfig.Flush()
    KPapp.statcode = 'BCD'
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    if (KPapp.goodlink == False):                   # link not established
        return
    KPsend(self, "_MODE ASCII")
    while (True):
        getKPresponse(self)
        temppos = KPapp.KPmessage.find('is ASCII')
        if (temppos != -1):
            break
    KPsend(self, "_CODE BCD")
    while (True):
        getKPresponse(self)
        temppos = KPapp.KPmessage.find('is BCD')
        if (temppos != -1):
            break
    wx.LogGeneric(wx.LOG_User, "Set keypunch to BCD mode\n")
    return

# routine to set encoding to EBCDIC as ASCII characters
def setebcdic(self):
    KPapp.goodchars = " !" + '"' + "#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n"
    KPapp.isbinary = 0
    KPapp.myconfig.Write('Encoding/Code', 'EBCDIC')
    KPapp.myconfig.Flush()
    KPapp.statcode = 'EBCDIC'
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    if (KPapp.goodlink == False):                   # link not established
        return
    KPsend(self, "_MODE ASCII")
    while (True):
        getKPresponse(self)
        temppos = KPapp.KPmessage.find('is ASCII')
        if (temppos != -1):
            break
    KPsend(self, "_CODE EBCDIC")
    while (True):
        getKPresponse(self)
        temppos = KPapp.KPmessage.find('is EBCDIC')
        if (temppos != -1):
            break
    wx.LogGeneric(wx.LOG_User, "Set keypunch to EBCDIC mode\n")
    return

# routine to set encoding to keypunch binary mode as sets of four ASCII hex digits
def setbinary(self):
    KPapp.goodchars = " 0123456789abcdefABCDEF\n"
    KPapp.isbinary = 1
    KPapp.myconfig.Write('Encoding/Code', 'BINARY')
    KPapp.myconfig.Flush()
    KPapp.statcode = 'binary'
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    if (KPapp.goodlink == False):                   # link not established
        return
    KPsend(self, "_MODE BINARY")
    while (True):
        getKPresponse(self)
        temppos = KPapp.KPmessage.find('is BINARY')
        if (temppos != -1):
            break
    wx.LogGeneric(wx.LOG_User, "Set keypunch to binary mode\n")
    return

# routine to set encoding to 1130 simulator card files
def set1130card(self):
    KPapp.goodchars = ""
    KPapp.isbinary = 0
    KPapp.myconfig.Write('Encoding/Code', '1130CARD')
    KPapp.myconfig.Flush()
    KPapp.statcode = '1130 Card'
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    wx.LogGeneric(wx.LOG_User, "Set keypunch to 1130 card image mode\n")
    return

# routine to set encoding to 1130 simulator binary files
def set1130binary(self):
    KPapp.goodchars = ""
    KPapp.isbinary = 0
    KPapp.myconfig.Write('Encoding/Code', '1130BINARY')
    KPapp.myconfig.Flush()
    KPapp.statcode = '1130 binary'
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    wx.LogGeneric(wx.LOG_User, "Set keypunch to 1130 binary image mode\n")
    return

def validateline(theline):

    # if not binary, can't be more than 80 characters plus NL
    if ((KPapp.isbinary == 0) and (len(theline) > 81)):
        return False
    
    # look at every character in the line in sequence and test to see if it is a 'good character'
    for c in theline:
        if ((c in KPapp.goodchars) == False):
            return False
   
    # if binary mode, must verify the pattern is legitimate
    if (KPapp.isbinary == 0):          # not binary
        return True
    
    # check for format of 0 to 80 groups of four hex digits with spaces in between, optional NL
    if (re.search("^(([0123456789abcdefABCDEF]{4}) ?){0,80}\\n?$",theline) == None):    # get None if no match
        return False
   
    return True               # passed the tests

# this routine is called at startup to verify the connection by interrogating the keypunch
# along with basic readiness to punch, it will determine if the reader cable is configured 
# permitting reading of cards as well as punching of the files
def KPconnect(self):
    # yield first time we are entered to free up the idle loop
    wx.Yield()
    wx.LogGeneric(wx.LOG_User, "Initiating link to keypunch interface\n")


    # set up a timer we can use to handle timeout if the box isn't there or the cable or link is down
    # if the timer pops, we set errstatus message and disable all menu entries except quit in a 
    # seperate handler routine, while this one stalls. Gives us 10 seconds for it to initialize
    self.conntimer = mytimer(id = 99)
    self.conntimer.Start(milliseconds = 10000, oneShot = True)

    # first look for healthy keypunch interface box
    KPsend(self, "_IDLE", sendidle=False)                   # first idle just to terminate any junk of partial command in interface buffer
    KPsend(self, "_DIAG 0")                                 # diag 0 will test interface and send back confirmation of read cable if installed

    # now loop through responses until we verify we are live and discover the state of the read cable
    while (True):
        wx.Yield()
        getKPresponse(self)                                 # read a line from keypunch interface

        if (self.conntimer.IsRunning() == False):             # did we see timer pop?
            return                                          # get out of this routine entirely
        if (KPapp.KPmessage.find("Read cable verified") != -1):
            KPapp.errstatus = "Read cable configured, permitting read functions"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
        elif (KPapp.KPmessage.find("Read cable not") != -1):
            noReadCable(self)
        elif (KPapp.KPmessage.find("Diag 0 completed") != -1):
            break

    # stop the timer since we completed the Diag 0 routine successfully
    self.conntimer.Stop()

    # now setup the box for the encoding desired by the user
    # which sends the MODE command and usually the CODE command
    encodemsg = "_CODE "
    modemsg = "_MODE "
    if (KPapp.statcode == 'BCD'):
        modemsg += "ASCII"
        encodemsg += "BCD"
    elif (KPapp.statcode == "EBCDIC"):
        modemsg += "ASCII"
        encodemsg += "EBCDIC"
    elif (KPapp.statcode == "binary"):
        modemsg += "BINARY"
        encodemsg = ""
    else:
        KPapp.errstatus = "1130 modes not implemented yet"
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
        return

    # send the mode message first and get response
    KPsend(self, modemsg)                                   # ask for desired mode (ASCII or BINARY)
    while (True):
        getKPresponse(self)                                 # wait for response then check it
        if (KPapp.KPmessage.find("OK Mode is ") != -1):
            break

    # verify we had a successful switch to our targeted mode
    try:
        temppos = re.search('(?<=^OK Mode is ).+$', KPapp.KPmessage).group(0)
    except:
        temppos = ""
    if (temppos.find("ASCII") != -1):
        if (KPapp.statcode == 'binary'):
            KPapp.errstatus = "Switched to wrong mode"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return
    elif (temppos.find("BINARY") != -1):
        if (KPapp.statcode != 'binary'):
            KPapp.errstatus = "Switched to wrong mode"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return

    # send the code message if needed now
    if (len(encodemsg) > 0):
        # send the encoding message mow and get response
        KPsend(self, encodemsg)                             # ask for desired encoding

        # now scan for response
        while (True):
            getKPresponse(self)                             # block until we get response

            temppos = KPapp.KPmessage.find("OK ASCII input to encode ")
            if (temppos != -1):
                break
    
        # verify we got a positive confirmation from the keypunch that it is using the right encoding
        try:
            temppos = re.search('(?<=^OK ASCII input to encode ).+$', KPapp.KPmessage).group(0)
        except:
            temppos = ""
        if ((temppos.find("BCD ") != -1) and (KPapp.statcode != "BCD")):
            KPapp.errstatus = "Did not switch to BCD mode"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return
        elif ((temppos.find("EBCDIC") != -1) and (KPapp.statcode != "EBCDIC")):
            KPapp.errstatus = "Did not switch to EBCDIC mode"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return
    
        # bye bye, we have connected
        KPapp.goodlink = True
        wx.LogGeneric(wx.LOG_User, "Initiated communication with keypunch interface\n")
        return

# this function will punch a card from the file
def KPpunch(self):
    tempcmd = "_P"
    tempcmd += KPapp.flines[KPapp.myfilecurrent-1]

    KPapp.myfilecurrent += 1
    KPapp.myfilecurrentstr = str(KPapp.myfilecurrent)

    KPapp.errstatus = " "
    KPapp.statpane.Refresh()
    KPapp.statpane.Update()
    
    KPsend(self, tempcmd)
    while (True):
        getKPresponse(self)
        temppos = KPapp.KPmessage.find("OK Punch operation complete")
        if (temppos != -1):
            KPapp.errstatus = " "
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            break
        temppos = KPapp.KPmessage.find("Register a card")
        if (temppos != -1):
            KPapp.errstatus = "Card not registered at punch station!"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
    return

# this function will read a card from the keypunch and add it to the previously read ones
# updates flines for viewing and expects the caller to write the line to the file
def KPreadCard(self):
    KPsend(self,"_R")
    while (True):
        getKPresponse(self)
        temppos = KPapp.KPmessage.find("OK Read card:")
        if (temppos != -1):
            KPapp.myfilecurrent += 1                                # update the card pointer
            KPapp.myfilecurrentstr = str(KPapp.myfilecurrent)       # update the text card pointer
            KPapp.flines.append(KPapp.KPmessage[temppos+13:])       # add the remainder of the line (the card image) to the file
            KPapp.myfilelen += 1                                    # update the card count (file length)
            KPapp.myfilelenstr = str(KPapp.myfilelen)               # update the text card count
            KPapp.errstatus = " "
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            break
        temppos = KPapp.KPmessage.find("Register card")             # interim issue - let user know cards are not ready
        if (temppos != -1):
            KPapp.errstatus = "Keypunch not ready - register cards"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
    return

# this function will keep returning to read the serial port until we have
# at least one complete message. It is a blocking read of a message at a time
# pulls out the extracted message and removes it from the buffer
def getKPresponse(self):
    while (KPapp.fullmessage == False):
        wx.Yield()
        try:
            readKP()
        except serial.SerialException:
            return
        endpos = KPapp.KPbuffer.find('\n')
        if (endpos != -1):
            KPapp.fullmessage = True
    KPapp.KPmessage = KPapp.KPbuffer[:endpos-1]         # copy message dropping the final NL and preceeding character
    if (KPapp.KPbuffer[endpos-1] != '\r'):
        KPapp.KPmessage += KPapp.KPbuffer[endpos-1]
    if (len(KPapp.KPbuffer) > (endpos + 1)):            # more data after the NL?
        KPapp.KPbuffer = KPapp.KPbuffer[endpos+1:]      # get the remainder
        KPapp.KPlen -= (endpos+1)                       # drop the length down as well
    else:
        KPapp.KPbuffer = ""                             # set up as empty buffer
        KPapp.KPlen = 0                                 # and mark the length as zero
    KPapp.fullmessage = False
    wx.LogGeneric(wx.LOG_User, "KP-> " + KPapp.KPmessage + "\n")
    return

# this function will extract all the inbound characters that are 
# available in the serial port, pushing them onto our buffer
def readKP():
   avail = 0                                            # necessary in case inWaiting takes an exception
   avail = KPapp.mylink.inWaiting()
   if (avail > 0):
       KPapp.KPbuffer += KPapp.mylink.read(avail)
       KPapp.KPlen += avail
   return avail

# this function will build and transmit a command line to the interface box
# with an optional no-operation (IDLE) command appended when this is the 
# final command of a group being sent (or only command if a singleton)
def KPsend(self, thecommand, sendidle=True):
    # if asked to send IDLE command after the submitted command, append it to string
    if (sendidle == True):
        thecommand += "_IDLE"

    try:
        KPapp.mylink.write(thecommand)
    except serial.serialutil.SerialTimeoutException:
        KPapp.errstatus = "Timeout sending to keypunch - aborting"
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
        noKeypunch(self)
    return

     

# routine to disable the read menu item due to lack of a reader cable on the keypunch
def noReadCable(self):
     KPapp.menufile.Enable(2, False)
     KPapp.menuactions.Enable(23, False)
     KPapp.menuactions.Enable(24, False)
     wx.LogGeneric(wx.LOG_User, "No reader cable configured thus disabling reading actions\n")
     return

# routine to disable all menus except encoding and quit
def noKeypunch(self):
     KPapp.menufile.Enable(2, False)
     KPapp.menuactions.Enable(20, False)
     KPapp.menuactions.Enable(21, False)
     KPapp.menuactions.Enable(22, False)
     KPapp.menuactions.Enable(24, False)
     wx.LogGeneric(wx.LOG_User, "Unable to reach keypunch thus disabling all machine actions\n")
     return

# routine to toggle the log on and off
def toggleLog(self):
    KPapp.mylog.Show(True)
    return

class myviewclass (wx.HVScrolledWindow ):
#    def __init__(self, myparent, myid, mypos=wx.Point(0,0), mysize=wx.Size(0,0), mystyle=0):
#        myparent.__init__(myparent, myid, mypos, mysize, mystyle)
#        return

    def OnGetRowHeight(self,rowno):
        return 18

    def OnGetColumnWidth(self,colno):
        return 14

    def OnPaint(self, myevent):
        startpt = self.GetVisibleBegin()
        endpt = self.GetVisibleEnd()
        mydc = wx.PaintDC(self)

        # set up font
        myfont = wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, underline=False)
        myfontbold = wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, underline=True)

        # now extract row and col start/end separately
        startrow = startpt.GetRow()
        endrow = endpt.GetRow()
        startcol = startpt.GetCol()
        endcol = endpt.GetCol()

        # run through the new rows, writing each card to the window
        for i in range (startrow, endrow):
 
            # highlight only the current line whereever it is in the display
            if ((i) == (KPapp.myfilecurrent - 1)):      # are we on the line with the current card?
                mydc.SetFont(myfontbold)                # make this line bold
            else:
                mydc.SetFont(myfont)                    # only bold for first line, the current card

            # now run through the columns of this card
            for j in range (startcol, endcol):
                mydc.DrawText(' ', (j-startcol)*14+3, (i-startrow)*18+1)                    # blank it first
                if (j < len(KPapp.flines[i])):
                    mydc.DrawText(KPapp.flines[i][j],(j-startcol)*14+3, (i-startrow)*18+1)  # real character if not past end of line

        return

class mytimer(wx.Timer):
    def Notify(self):
        noKeypunch(self)                                # shut off all menu actions since we are dead in the water
        KPapp.errstatus = "Timeout connecting to keypunch, fix and restart this program"
        wx.LogGeneric(wx.LOG_User,"Timer aborted attempt to reach keypunch interface, check cables\n")
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
        return

class MyStatClass(wx.Panel):

    def StatPaint(self, myevent):
        mydc = wx.PaintDC(self)

        # set up font
        myfont = wx.Font(10, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        mydc.SetFont(myfont)

        # now update the status line info
        self.GetGrandParent().statfileF.SetLabel(KPapp.statfile)
        self.GetGrandParent().myfilelenstrF.SetLabel(KPapp.myfilelenstr)
        self.GetGrandParent().myfilecurrentstrF.SetLabel(KPapp.myfilecurrentstr)
        self.GetGrandParent().errstatusF.SetLabel(KPapp.errstatus)
        self.GetGrandParent().statcodeF.SetLabel(KPapp.statcode)
        return

class MyFrame(wx.Frame):

    def __init__(self, myparent, myid, mytitle):
        wx.Frame.__init__(self, myparent, myid, mytitle,
             wx.Point(200,200), wx.Size(600,400))
        toppane = wx.Panel(self,-1)

        # create a scrolling pane inside top pane taking all the room except for the status pane
        self.myview = myviewclass(toppane, -1, size=(self.GetClientSize().GetWidth(),self.GetClientSize().GetHeight()-50), style=wx.FULL_REPAINT_ON_RESIZE)

        # create our status pane inside top pane
        self.statpane = MyStatClass(toppane, -1, size=(-1,50))

        #get a sizer for our top panel then add the two panels inside
        mybox = wx.BoxSizer(wx.VERTICAL)
        mybox.Add(self.myview, proportion = 1, flag = wx.ALIGN_TOP | wx.ALIGN_LEFT | wx.EXPAND)
        mybox.Add(self.statpane, proportion = 0, flag = wx.ALIGN_BOTTOM | wx.ALIGN_LEFT | wx.FIXED_MINSIZE)
        self.statpane.SetMinSize((-1,50))
        self.statpane.SetMaxSize((-1,50))

        # set this sizer into the top pane
        toppane.SetSizer(mybox)

       # activate the onPaint event handler for our main view
        self.myview.Bind(wx.EVT_PAINT, self.myview.OnPaint)
        KPapp.viewpane = self.myview

        # activate the onPaint event handler for our statpane view
        self.statpane.Bind(wx.EVT_PAINT, self.statpane.StatPaint)
        KPapp.statpane = self.statpane

        # create the two lines of our pseudo status bar
        stvbox = wx.BoxSizer(wx.VERTICAL)               # divide pane into two lines
        sthbox1 = wx.BoxSizer(wx.HORIZONTAL)            # divide top line of status bar into horizontal fields
        sthbox2 = wx.BoxSizer(wx.HORIZONTAL)            # divide the bottom line of status bar into horizontal fields

        # create the widgets for our pseudo status bar
        sh11a = wx.StaticText(self.statpane,-1,label='Code: ')
        sh11b = wx.StaticText(self.statpane, -1, label=KPapp.statcode)
        self.statcodeF = sh11b
        sh12a = wx.StaticText(self.statpane,-1,label ='Fstat: ')
        sh12b = wx.StaticText(self.statpane,-1,label = KPapp.statfile)
        self.statfileF = sh12b
        sh13a = wx.StaticText(self.statpane,-1,label ='Length: ')
        sh13b = wx.StaticText(self.statpane,-1,label = KPapp.myfilelenstr)
        self.myfilelenstrF = sh13b
        sh14a = wx.StaticText(self.statpane,-1,label ='Current Card: ')
        sh14b = wx.StaticText(self.statpane,-1,label = KPapp.myfilecurrentstr)
        self.myfilecurrentstrF = sh14b
        sh21a = wx.StaticText(self.statpane, -1, label = 'Status: ')
        sh21b = wx.StaticText(self.statpane, -1, label = KPapp.errstatus)
        sh22 = wx.StaticText(self.statpane, -1, label = " x ")
        self.padding = sh22
        self.errstatusF = sh21b
        sthbox1.Add((50,-1))                                                    # space over 50 pixels first
        sthbox1.Add(sh12a, wx.ALIGN_LEFT)                                       # add the file status label on the left of line 1
        sthbox1.Add(sh12b, wx.ALIGN_LEFT)                                       # add the file status text on the left of line 1
        sthbox1.Add((40,-1))                                                    # space over 40 pixels between fields
        sthbox1.Add(sh11a, wx.ALIGN_LEFT)                                       # add the code label next in line 1
        sthbox1.Add(sh11b, wx.ALIGN_LEFT)                                       # add the code text next in line 1
        sthbox1.Add((60,-1))                                                    # space a bit
        sthbox1.Add(sh22, proportion = 1, flag = wx.ALIGN_LEFT | wx.EXPAND)     # separate the two sizes with variable spacing
        sthbox1.Add(sh13a, wx.ALIGN_LEFT)                                       # add the num of cards label next in line 1
        sthbox1.Add(sh13b, wx.ALIGN_LEFT | wx.EXPAND)                           # add the num of cards text next in line 1
        sthbox1.Add((40,-1))                                                    # space over 40 pixels between fields
        sthbox1.Add(sh14a, wx.ALIGN_LEFT)                                       # add the current card label on the right of line 1
        sthbox1.Add(sh14b, wx.ALIGN_LEFT | wx.EXPAND)                           # add the current card text on the right of line 1
        sthbox2.Add((50,-1))                                                    # space over 50 pixels first
        sthbox2.Add(sh21a, wx.ALIGN_LEFT)                                       # add the error msg label on the left of line 2
        sthbox2.Add(sh21b, wx.ALIGN_LEFT | wx.EXPAND)                           # add the error msg text on the left of line 2

        # attach a resize handler to redraw the variable padding between the sides of the status pane
        toppane.Bind(wx.EVT_SIZE, self.onSize)

        # Install the pseudo status bar now
        stvbox.Add(sthbox1)
        stvbox.Add(sthbox2)

        # activate the sizer for the pseudo status bar panel
        self.statpane.SetSizer(stvbox)

        # set up menus
        mybar = wx.MenuBar()
        myfilemenu = wx.Menu()
        myread = myfilemenu.Append(2, 'N&ew\tCtrl+N', 'Read cards to new file')
        mypunch = myfilemenu.Append(1, 'O&pen\tCtrl+O', 'Open file to punch')
        myclose = myfilemenu.Append(4, 'C&lose\tCtrl+C', 'Close active file')
        myquit = myfilemenu.Append(wx.ID_EXIT, 'Q&uit\tCtrl+Q', 'Quit application')
        myactionmenu = wx.Menu()
        mystartpunch = myactionmenu.Append(20, 'S&tart punching\tCtrl+S', 'Start or resume punching')
        myline1 = myactionmenu.AppendSeparator()
        mystartread = myactionmenu.Append(24, 'S&tart reading\tCtrl+R', 'Start or resume reading cards')
        mystoppunch = myactionmenu.Append(22, 'E&nd reading\tCtrl+E', 'End of deck of cards to read')
        myline2 = myactionmenu.AppendSeparator()
        mypausepunch = myactionmenu.Append(21, 'P&ause keypunch\tCtrl+P', 'Hold punching or reading at current point')
        myline3 = myactionmenu.AppendSeparator()
        mygoto = myactionmenu.Append(23, 'G&o to card . . .\tCtrl+G', 'Pick card in deck to begin/resume punching')
        myoptionsmenu = wx.Menu()
        menbcd = myoptionsmenu.Append(30, 'BCD\tCtrl + 1', 'BCD encoded ASCII', kind=wx.ITEM_RADIO)
        menebcdic = myoptionsmenu.Append(31, 'EBCDIC\tCtrl + 2', 'EBCDIC encoded ASCII', kind=wx.ITEM_RADIO)
        menbinary = myoptionsmenu.Append(32, 'binary\tCtrl + 3', 'binary encoded in ASCII', kind=wx.ITEM_RADIO)
        men1130card = myoptionsmenu.Append(33, '1130card\tCtrl + 4', '1130 hollerith encoded in ASCII', kind=wx.ITEM_RADIO)
        men1130binary = myoptionsmenu.Append(34, '1130binary\tCtrl + 5', '1130 binary cards encoded ASCII', kind=wx.ITEM_RADIO)
        myline4 = myoptionsmenu.AppendSeparator()
        myreset = myoptionsmenu.Append(50, 'F&orget serial port\tCtrl + F', 'Forget the saved serial port')
        mylog = myoptionsmenu.Append(45, 'Show Keypunch L&og\tCtrl + L', 'Displays a log of KP interface outputs')
        myline5 = myoptionsmenu.AppendSeparator()
        myabout = myoptionsmenu.Append(40, 'About . . .', 'About this program')
        mybar.Append(myfilemenu, "File")
        mybar.Append(myactionmenu, "Actions")
        mybar.Append(myoptionsmenu, "Options")
        self.SetMenuBar(mybar)

        # set up accelerators for menus
        self.myacceltab = wx.AcceleratorTable(     [(wx.ACCEL_CTRL, ord('N'), 2),
                                                    (wx.ACCEL_CTRL, ord('O'), 1),
                                                    (wx.ACCEL_CTRL, ord('C'), 4),
                                                    (wx.ACCEL_CTRL, ord('Q'), wx.ID_EXIT),
                                                    (wx.ACCEL_CTRL, ord('S'), 20),
                                                    (wx.ACCEL_CTRL, ord('P'), 21),
                                                    (wx.ACCEL_CTRL, ord('E'), 22),
                                                    (wx.ACCEL_CTRL, ord('G'), 23),
                                                    (wx.ACCEL_CTRL, ord('R'), 24),
                                                    (wx.ACCEL_CTRL, ord('1'), 30),
                                                    (wx.ACCEL_CTRL, ord('2'), 31),
                                                    (wx.ACCEL_CTRL, ord('3'), 32),
                                                    (wx.ACCEL_CTRL, ord('4'), 33),
                                                    (wx.ACCEL_CTRL, ord('5'), 34),
                                                    (wx.ACCEL_CTRL, ord('L'), 45),
                                                    (wx.ACCEL_CTRL, ord('F'), 50),
                                                    ])
        self.SetAcceleratorTable(self.myacceltab)
        KPapp.menoptions = myoptionsmenu
        KPapp.menufile = myfilemenu
        KPapp.menuactions = myactionmenu

        # bind the accelerators to the menu procedures
        self.Bind(wx.EVT_MENU, self.OnPunch, mypunch)
        self.Bind(wx.EVT_MENU, self.OnRead, myread)
        self.Bind(wx.EVT_MENU, self.OnCloseFile, myclose)
        self.Bind(wx.EVT_MENU, self.OnQuit, myquit)
        self.Bind(wx.EVT_MENU, startpunch, mystartpunch)
        self.Bind(wx.EVT_MENU, pausepunch, mypausepunch)
        self.Bind(wx.EVT_MENU, stoppunch, mystoppunch)
        self.Bind(wx.EVT_MENU, startread, mystartread)
        self.Bind(wx.EVT_MENU, goto, mygoto)
        self.Bind(wx.EVT_MENU, setbcd, menbcd)
        self.Bind(wx.EVT_MENU, setebcdic, menebcdic)
        self.Bind(wx.EVT_MENU, setbinary, menbinary)
        self.Bind(wx.EVT_MENU, set1130card, men1130card)
        self.Bind(wx.EVT_MENU, set1130binary, men1130binary)
        self.Bind(wx.EVT_MENU, self.showabout, myabout)
        self.Bind(wx.EVT_MENU, dropport, myreset)
        self.Bind(wx.EVT_MENU, toggleLog, mylog)

        # now create the log window but leave it initially hidden
        KPapp.mylog = wx.LogWindow(self, "Log of messages from keypunch interface")
        KPapp.mylog.Show(False)
        wx.LogGeneric(wx.LOG_User, "Starting KP interface\n")

        # finish our startup
        self.punchfilename = ""
        self.readfilename = ""
        self.fileobject = None
        self.filelen = 0
        self.filecurrent = 0
        KPapp.errstatus = ""
        self.mystartup(666)
        self.Bind(wx.EVT_IDLE, self.OnIdle)

    # routine to do the actual punch or read operations in idle cycles
    # does one card each time a file is active and keypunch is not paused
    # also connects at startup time
    def OnIdle(self,event):

        # if not yet connected to keypunch, try now
        if (KPapp.connected == False):
            KPapp.connected = True
            KPconnect(self)

        # if not doing read or punch, or if paused, go away immediately
        if ((KPapp.pause == True) or ((KPapp.gopunch == False) and (KPapp.goread == False))):
            return

        if (KPapp.gopunch == True):

            wx.LogGeneric (wx.LOG_User, "punching card" + str(KPapp.myfilecurrent-1) + "\n")
            # drive punching a card, using current card position
            KPpunch(self)               # ask our routine to do the interaction

            # now if we have punched the last card, lets pause and put up a "Done" message
            if (KPapp.myfilecurrent > KPapp.myfilelen):
                KPapp.pause = True
                KPapp.errstatus = "Done punching file"
                wx.LogGeneric(wx.LOG_User,"Done punching file\n")
                KPapp.myfilecurrentstr = "1"
                KPapp.myfilecurrent = 1
                KPapp.statpane.Refresh()
                KPapp.statpane.Update()

        elif (KPapp.goread == True):

            # drive reading a card into the fline array and on screen
            wx.LogGeneric(wx.LOG_User, "Reading a card\n")
            KPreadCard(self)                                                    # get from KP and put into last flines
            self.fileobject.write(KPapp.flines[KPapp.myfilecurrent-1] + '\n')     # write it out to the file

        return

    def OnQuit(self, event):
        wx.LogGeneric(wx.LOG_User, "User issued quit command\n")
        if ((KPapp.gopunch == True) or (KPapp.goread == True)):
            KPapp.pause = True                      # blocks from trying to punch or read another card
            try:                                    # protect against exceptions
                self.fileobject.close()             # close it up before ending
            except:
                pass                                # do nothing if close failed
        # we are done with the port
        try:
            KPapp.mylink.close()
        except:
            pass
        # now go away
        self.Close()

    def OnClose(self, event):
        self.Destroy()

    def OnPunch(self, event):
        # make sure no other file is open now
        if (self.fileobject != None):
            KPapp.errstatus = "A file is currently open"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return

        # go ask user to pick the file they want opened
        punchfilename = wx.LoadFileSelector('file with card images to punch', 'txt')
        #open the file, check it over and get the count of cards
        try:
            self.fileobject = open(punchfilename)
        except:
            KPapp.errstatus = "Error opening file"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return
        self.filecurrent = 0
        KPapp.flines = []

        # put filename in window
        tempjunk, tempfilename = os.path.split(str(punchfilename))
        KPapp.myframe.SetTitle("Keypunch Interface - file: " + tempfilename)

        # log the opening
        wx.LogGeneric(wx.LOG_User, "Opened file " + punchfilename + "\n")

        while (True):
            templine = self.fileobject.readline()
            if (templine == ""):
                KPapp.errstatus = ""
                KPapp.statpane.Refresh()
                KPapp.statpane.Update()
                break
            if (validateline(templine) == False):
                KPapp.errstatus = "File validity check at card " + str(self.filecurrent+1)
                wx.LogGeneric(wx.LOG_User, "File validity check at card " + str(self.filecurrent+1) + "\n")
                KPapp.statpane.Refresh()
                KPapp.statpane.Update()
                self.fileobject.close()
                self.fileobject = None
                KPapp.flines = []
                break
            KPapp.flines.append(templine)
            self.filecurrent += 1

        if (self.fileobject == None):
            KPapp.myfilelen = 0
            KPapp.myfilecurrent = 0
            KPapp.myfilelenstr = "0"
            KPapp.myfilecurrentstr = "0"
            KPapp.statfile = "CLOSED"
            self.myview.SetRowColumnCount(0,0)
        else:
            # save the count of cards then set to the first one
            KPapp.myfilelen = self.filecurrent
            KPapp.myfilecurrent = 1
            KPapp.myfilelenstr = str(self.filecurrent)
            KPapp.myfilecurrentstr = "1"
            KPapp.statfile = " OPEN "
            wx.LogGeneric(wx.LOG_User, "File has " + str(KPapp.myfilelen) +" cards, ready at card 1\n")
            if (KPapp.statcode == "binary"):
                self.myview.SetRowColumnCount(KPapp.myfilelen, 399)
            else:
                self.myview.SetRowColumnCount(KPapp.myfilelen,80)
        self.statpane.Refresh()
        self.statpane.Update()
        self.myview.Refresh()
        self.myview.Update()
        KPapp.readfile = False
        KPapp.pause = True
        KPapp.goread = False
        KPapp.gopunch = True

    def OnRead(self, event):
        # make sure no file is currently open
        if (self.fileobject != None):
            KPapp.errstatus = "A File is currently open"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return

        # go ask user to pick the file they want to create
        readfilename = wx.SaveFileSelector('cards being read as file', 'txt')
        if (os.path.exists(readfilename) == True):
            KPapp.errstatus = "The file already exists"
            wx.LogGeneric(wx.LOG_User, "New output file " + readfilename + " already exists\n")
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return
        try:
            self.fileobject = open(readfilename, 'w')
        except:
            KPapp.errstatus = "Error creating file"
            wx.LogGeneric(wx.LOG_User, "Error creating output file " + readfilename + "\n")
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return
        # put filename in window
        tempjunk, tempfilename = os.path.split(str(readfilename))
        KPapp.myframe.SetTitle("Keypunch Interface - file: " + tempfilename)
        wx.LogGeneric(wx.LOG_User, "Created new file " + readfilename + " for reading, currently empty\n")

        KPapp.myfilelen = 0
        KPapp.myfilecurrent = 0
        KPapp.myfilelenstr = "0"
        KPapp.myfilecurrentstr = '0'
        KPapp.flines = []
        KPapp.statfile = " OPEN "

        #lets put the empty file into the UI window
        if (KPapp.statcode == "binary"):
            self.myview.SetRowColumnCount(0, 399)
        else:
            self.myview.SetRowColumnCount(0,80)
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
        KPapp.pause = True
        KPapp.gopunch = False
        KPapp.goread = True
        KPapp.readfile = True

    def OnCloseFile(self, event):
        # keypunch must be paused to close the file
        if (KPapp.pause == False):
            KPapp.errstatus = "Keypunch must be paused to close files"
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return

        # turn off all processing
        KPapp.gopunch = False
        KPapp.goread = False

        # try to close the file now
        try:
            self.fileobject.close()
            KPapp.errstatus = ""
        except:
            KPapp.errstatus = "File was not open"

        # strip filename from window title
        KPapp.myframe.SetTitle("Keypunch Interface - file: *none*")
        wx.LogGeneric(wx.LOG_User, "Closed file\n")

        self.fileobject = None
        KPapp.myfilelen = 0
        KPapp.myfilecurrent = 0
        self.filecurrent = 0
        KPapp.myfilelenstr = "0"
        KPapp.myfilecurrentstr = "0"
        KPapp.flines = []
        self.readfilename = ""
        self.punchfilename = ""
        KPapp.statfile = "CLOSED"
        self.myview.SetRowColumnCount(0,0)
        KPapp.statpane.Refresh()
        KPapp.statpane.Update()
        self.myview.Refresh()
        self.myview.Update()
        return

    def onSize(self, event):
        winclient = self.GetClientSize().GetWidth()         # find width of resized top level window
        winclient -= 550
        winclient /= 3
        thepad = ''
        if (winclient > 0):
            for i in range (winclient):
                thepad += ' '
            self.padding.SetLabel(thepad)
        self.statpane.Refresh()
        self.statpane.Update()
        event.Skip()                # pass this along for more handling

    # user asked for the about message
    def showabout(self, event):
        aboutcontents = "Keypunch Interface program for KPgolem project\n\n"
        aboutcontents += "Communicates with KPgolem box in a keypunch machine\n"
        aboutcontents += "to punch contents of PC file onto cards or read cards\n"
        aboutcontents += "into a new file on the PC\n\n"
        aboutcontents += "    see KPgolem project\n         https://github.com/search?utf8=%E2%9C%93&q=KPgolem \n"
        mydialog = wx.MessageDialog(self, aboutcontents, style = wx.OK)
        mydialog.ShowModal()
        mydialog.Destroy()
        return

    # does startup of main program processing
    def mystartup(self, event):
        #start of main program
        KPapp.mylink = serial.Serial()

        # see if our config file exists, if not create one
        KPapp.myconfig = wx.FileConfig("KPinterface", style=wx.CONFIG_USE_LOCAL_FILE)
        KPapp.myconfig.Set(None)

        # prepare to have user select the port to open
        quiklist = portlist()               # serial function to build a list of all serial ports on the system
        quiklen = len(quiklist)             # get number of ports, each is a tuple of short name and extended description/OS path
        fulllist = []
        for i in range(quiklen):
            fulllist.append(str(quiklist[i][0]) + " " + str(quiklist[i][1]))

        # if config file, use that value with checking
        if (KPapp.myconfig.HasGroup('Serial')):         #  group entry for Serial ports is there
            thisport = str(KPapp.myconfig.Read('Serial/Port','FUBAR'))
        else:
            # throw up a dialog box asking the user to select the serial port we will use
            outcome = wx.GetSingleChoiceIndex("Select the serial port to open", "Startup", fulllist)
            if (outcome == -1):
                self.OnQuit(999)                   # we stop application
                return
            thisport = quiklist[int(outcome)][0]
        for x in range (len(quiklist)):
            aport = quiklist[x]
            if (aport[0] == thisport):
                break
        if (aport[0] == thisport):
            pass
        else:
            thisport = 'FUBAR'
        if (thisport == 'FUBAR'):
            KPapp.connected = True
            KPapp.errstatus = "Invalid port in config file, restart"
            KPapp.myconfig.DeleteEntry("Serial/Port")
            KPapp.myconfig.Flush()
            KPapp.statpane.Refresh()
            KPapp.statpane.Update()
            return

        # open the selected port
        kpinit(KPapp.mylink, thisport)

        # set up the current goodchars to the initial value
        code = str(KPapp.myconfig.Read('Encoding/Code','FUBAR'))
        if (code == 'BCD'):
            KPapp.menoptions.Check(30,True)
            setbcd(self)
        elif (code == 'EBCDIC'):
            KPapp.menoptions.Check(31,True)
            setebcdic(self)
        elif (code == 'BINARY'):
            KPapp.menoptions.Check(32,True)
            setbinary(self)
        elif (code == '1130CARD'):
            KPapp.menoptions.Check(33,True)
            set1130card(self)
        elif (code == '1130BINARY'):
            KPapp.menoptions.Check(34,True)
            set1130binary(self)
        else:
            KPapp.menoptions.Check(30,True)
            setbcd(self)
 
        # set up or update the config file entry
        KPapp.myconfig.Write('Serial/Port', thisport)
        KPapp.myconfig.Flush()


class KPapp (wx.App):

    mystatusname = "starting"
    goodchars = ""
    isbinary = 0
    mylink = None
    myconfig = None
    menoptions = None
    statcode = 'BCD'
    statfile = 'CLOSED'
    staterr = '*none*'
    statpane = None
    viewpane = None
    myfilelen = 0
    myfilelenstr = "0"
    myfilecurrent = 0
    myfilecurrentstr = '0'
    errstatus = ' '
    page = None
    # buffers
    KPlen = 0
    KPbuffer = ""
    KPmessage = ""
    flines = []
    # control flags
    KPflow = False
    gopunch = False
    goread = False
    pause = False
    fullmessage = False
    connected = False
    goodlink = False


    # this is called by the GUI library when we start it up and lets us put up our main window
    def OnInit(self):
        KPapp.myframe = MyFrame(None, -1, "Keypunch Interface - file: *none*")   # builds a window with a title
        KPapp.myframe.Show(True)                                  # makes the frame visible on the PC
        return True


#
# we are called here when the code first executes
#

# start app and initialize GUI
myapp = KPapp(0)

# run the main GUI loop now just waiting for events
myapp.MainLoop()
