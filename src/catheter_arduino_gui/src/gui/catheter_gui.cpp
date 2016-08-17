#include "gui/catheter_gui.h"
#include "com/pc_utils.h"
#include "ser/serial_thread.h"
#include <wx/wfstream.h>
#include <wx/numdlg.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG

// file definitions
#define playfile_wildcard wxT("*.play")

#define CATHETER_GUI_DEBUG 1
#define DBG(do_something) if (CATHETER_GUI_DEBUG) { do_something; }


IMPLEMENT_APP(CatheterGuiApp)

bool CatheterGuiApp::OnInit() {
	// debug memory:
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );

	serialObject = new SerialThreadObject;
    gui = new CatheterGuiFrame(wxT("Catheter Gui"),serialObject);
    gui->Show(true);
    return (gui != NULL);
}

int CatheterGuiApp::OnExit()
{
	delete serialObject;
	return 0;
}

wxBEGIN_EVENT_TABLE(CatheterGuiFrame, wxFrame)
    EVT_BUTTON(CatheterGuiFrame::ID_REFRESH_SERIAL_BUTTON, CatheterGuiFrame::OnRefreshSerialButtonClicked)
    EVT_BUTTON(CatheterGuiFrame::ID_SELECT_PLAYFILE_BUTTON, CatheterGuiFrame::OnSelectPlayfileButtonClicked)
    EVT_BUTTON(CatheterGuiFrame::ID_NEW_PLAYFILE_BUTTON, CatheterGuiFrame::OnNewPlayfileButtonClicked)
    EVT_BUTTON(CatheterGuiFrame::ID_SAVE_PLAYFILE_BUTTON, CatheterGuiFrame::OnSavePlayfileButtonClicked)
    EVT_BUTTON(CatheterGuiFrame::ID_SEND_COMMANDS_BUTTON, CatheterGuiFrame::OnSendCommandsButtonClicked)
    EVT_BUTTON(CatheterGuiFrame::ID_SEND_RESET_BUTTON, CatheterGuiFrame::OnSendResetButtonClicked)
	EVT_BUTTON(CatheterGuiFrame::ID_SEND_POLL_BUTTON, CatheterGuiFrame::OnSendPollButtonClicked)
	EVT_IDLE(CatheterGuiFrame::onIdle)
wxEND_EVENT_TABLE()


void CatheterGuiFrame::onIdle(wxIdleEvent & e)
{
	this->statusGridPtr->updateStatus(statusGridCmdPtr);
	this->statusText->addText(statusTextData);
}

CatheterGuiFrame::CatheterGuiFrame(const wxString& title, SerialThreadObject* thrdPtr) : 
wxFrame(NULL, wxID_ANY, title)
{

	this->serialObject = thrdPtr;
    parentPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);

    grid = new CatheterGrid(parentPanel);

	// add the status grid:
	statusGridPtr = new StatusGrid(parentPanel);
    
	statusGridCmdPtr = new statusData;

	// add the status text.
	statusText = new CatheterStatusText(parentPanel, wxID_ANY);

	statusTextData = new incomingText;
	serialObject->setStatusTextPtr(statusTextData);
	serialObject->setStatusGrid(statusGridCmdPtr);
    // control buttons (break this up into 2 rows)

    // row 1:
    selectPlayfileButton = new wxButton(parentPanel, ID_SELECT_PLAYFILE_BUTTON, wxT("Select Playfile"));
    newPlayfileButton = new wxButton(parentPanel, ID_NEW_PLAYFILE_BUTTON, wxT("New Playfile"));
    savePlayfileButton = new wxButton(parentPanel, ID_SAVE_PLAYFILE_BUTTON, wxT("Save Playfile"));
	pollButton = new wxButton(parentPanel, ID_SEND_POLL_BUTTON, wxT("Poll Arduino"));

    // row 2   
    sendCommandsButton = new wxButton(parentPanel, ID_SEND_COMMANDS_BUTTON, wxT("Send Commands"));
    sendResetButton = new wxButton(parentPanel, ID_SEND_RESET_BUTTON, wxT("Send Reset"));
    refreshSerialButton = new wxButton(parentPanel, ID_REFRESH_SERIAL_BUTTON, wxT("Refresh Serial"));

    playfileSaved = false;
    playfilePath = wxEmptyString;

    // add buttons to the frame
    wxFlexGridSizer* buttonBox = new wxFlexGridSizer(2, 4, wxSize(2, 2));
    buttonBox->Add(selectPlayfileButton);
    buttonBox->Add(newPlayfileButton);
    buttonBox->Add(savePlayfileButton);
	buttonBox->Add(pollButton);
    buttonBox->Add(sendCommandsButton);
    buttonBox->Add(sendResetButton);
    buttonBox->Add(refreshSerialButton);

    // Add the different boxes to the grid.
	// This box is the top level one.
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(buttonBox, 0, wxALL, 5);
	
	// add the status Grid 
	wxStaticBoxSizer *statusBox = new wxStaticBoxSizer(wxHORIZONTAL, parentPanel, wxT("status information"));
	
	statusBox->Add(statusGridPtr);
	vbox->Add(statusBox, 1, wxALL, 5);

    wxStaticBoxSizer *consoleBox = new wxStaticBoxSizer(wxVERTICAL, parentPanel, wxT("console information"));
    consoleBox->Add(statusText, 1, wxEXPAND | wxALL, 5);
    vbox->Add(consoleBox, 1, wxEXPAND | wxALL, 5);

    // wxPanel *fillPanel = new wxPanel(parentPanel);

    //removed the wxExpand flag
    vbox->Add(grid, 1, wxALL, 5);
    
    parentPanel->SetSizer(vbox);
    vbox->SetSizeHints(parentPanel);
    vbox->Fit(parentPanel);

    this->Fit();
    this->Center();

    setStatusText(wxT("Welcome to Catheter Gui"));    
}

CatheterGuiFrame::~CatheterGuiFrame()
{
	//do nothing here 
	//All windows should be auto cleaned.
	delete statusGridCmdPtr;
	delete statusTextData;
}


//////////////////////////////////
// public event handler methods //
//////////////////////////////////

void CatheterGuiFrame::OnSelectPlayfileButtonClicked(wxCommandEvent& e) {
    warnSavePlayfile();

    wxString path = openPlayfile();

    if (!path.IsEmpty()) {
        playfileSaved = false;
        playfilePath = path;

        grid->ResetDefault();
        loadPlayfile(playfilePath);

        setStatusText(wxString::Format(wxT("Editing Existing Playfile %s\n"), playfilePath));
    }
}

void CatheterGuiFrame::OnNewPlayfileButtonClicked(wxCommandEvent& e) {
    warnSavePlayfile();

    grid->ResetDefault();

    playfileSaved = false;
    playfilePath = wxEmptyString;

    setStatusText(wxT("Editing New Playfile\n"));
}

void CatheterGuiFrame::OnSavePlayfileButtonClicked(wxCommandEvent& e) {
    savePlayfile();
    if (playfileSaved) {
        // save contents of edit panel to playfilePath
        unloadPlayfile(playfilePath);
        setStatusText(wxString::Format(wxT("Saved Playfile as %s"), playfilePath));
    }
}

void CatheterGuiFrame::OnSendCommandsButtonClicked(wxCommandEvent& e) {
    //setStatusText(wxT("Sending Commands...\n"));
    if (sendGridCommands()) {
        setStatusText(wxT("Commands Successfully Sent"));
    } else {
        setStatusText(wxT("Error Sending Commands"));
    }
}




void CatheterGuiFrame::OnSendPollButtonClicked(wxCommandEvent& e) {
	sendPollCommand();
	setStatusText(wxT("Poll Command Successfully Sent"));
}



void CatheterGuiFrame::OnSendResetButtonClicked(wxCommandEvent& e) {
    //setStatusText(wxT("Sending Reset Command...\n"));
    if (sendResetCommand()) {
        setStatusText(wxT("Reset Command Successfully Sent"));
    } else {
        setStatusText(wxT("Error Sending Reset Command"));
    }
}

void CatheterGuiFrame::OnRefreshSerialButtonClicked(wxCommandEvent& e) {
    if (!refreshSerialConnection()) {
        setStatusText(wxString::Format("Serial Disconnected"));
    }
}

//////////////////////////////////
// status panel private methods //
//////////////////////////////////

void CatheterGuiFrame::setStatusText(const wxString& msg) {
	// append the new message to the current status message
    //statusText->SetLabel(wxString::Format("%s\n%s", statusText->GetLabel(), msg));
	//statusText->SetLabel(msg);
	statusText->addWxText(msg);
}

///////////////////////////////////
// control panel private methods //
///////////////////////////////////

wxString CatheterGuiFrame::openPlayfile() {
    wxString path = wxEmptyString;
    wxFileDialog openDialog(this, "Open Playfile", wxGetCwd(), "", playfile_wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openDialog.ShowModal() != wxID_CANCEL) {
        wxFileInputStream playfile_stream(openDialog.GetPath());
        if (playfile_stream.IsOk()) {
            path = openDialog.GetPath();
        } else {
            wxLogError("Selected file could not be opened.");
        }
    }
    return path;
}

void CatheterGuiFrame::savePlayfile() {
    wxString path = wxEmptyString;
    wxFileDialog saveDialog(this, wxT("Save Playfile"), wxGetCwd(), "", playfile_wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveDialog.ShowModal() != wxID_CANCEL) {
        wxFileOutputStream save_stream(saveDialog.GetPath());
        if (!save_stream.IsOk()) {
            wxLogError("Could not save to selected file");
        } else {
            path = saveDialog.GetPath();
        }
    }
    if (!path.IsEmpty()) {
        playfilePath = path;
        playfileSaved = true;
    }
}

void CatheterGuiFrame::loadPlayfile(const wxString& path) {
    std::vector<CatheterChannelCmdSet> gridCmds;
    loadPlayFile(path.mb_str(), gridCmds);
    grid->SetCommands(gridCmds);
}

void CatheterGuiFrame::unloadPlayfile(const wxString& path) {
    std::vector<CatheterChannelCmdSet> gridCmds;
    grid->GetCommands(gridCmds);
    writePlayFile(path.mb_str(), gridCmds);
}

void CatheterGuiFrame::warnSavePlayfile() {
    if (!playfileSaved) {
        if (wxMessageBox(wxT("Current content has not been saved! Proceed?"), wxT("Warning!"),
            wxICON_QUESTION | wxYES_NO, this) == wxNO) {
            savePlayfile();
            return;
        }
    }
}

// @TODO: The functionality of this block is very different now.
bool CatheterGuiFrame::sendCommands(const std::vector<CatheterChannelCmdSet> &cmdVect) {
	if (cmdVect.size())
	{
		serialObject->queueCommands(cmdVect);
		return true;
	}	
	return false;
}

bool CatheterGuiFrame::sendGridCommands() {
    std::vector<CatheterChannelCmdSet> cmds;
    grid->GetCommands(cmds);
	setStatusText(wxString::Format("Parsed %d Channel Commands\n", (int)(cmds.size())));
	setStatusText(wxString::Format("Parsed %d Channel Commands\n", (int)(cmds.size())));
    return sendCommands(cmds);
}

// resets the catheter.
bool CatheterGuiFrame::sendResetCommand() {
	setStatusText(wxT("Sending Global Reset Command...\n"));
	bool reset(true); //  
	//send a reset to the arduino
	serialObject->serialCommand(SerialThreadObject::ThreadCmd::resetArduino);
	return reset;
}

bool CatheterGuiFrame::sendPollCommand() {
	setStatusText(wxT("Sending Global Poll Command...\n"));
	serialObject->serialCommand(SerialThreadObject::ThreadCmd::poll);
	return true;
}

// @TODO: The functionality of this code will be dramatically different.
bool CatheterGuiFrame::refreshSerialConnection() {
	serialObject->serialCommand(SerialThreadObject::ThreadCmd::resetSerial);
	return true;
}

bool CatheterGuiFrame::closeSerialConnection() {
    // return ss->resetStop();
	return false;
}

void CatheterGuiFrame::wxSummarizeCmds(const std::vector<CatheterChannelCmd> &cmds) {
    if (!(cmds.size() % NCHANNELS)) {
        for (int i = 0; i < (cmds.size() / NCHANNELS); i++) {
            wxString allchannels = wxEmptyString;
            for (int j = 0; j < NCHANNELS; j++) {
                allchannels = allchannels + wxToString(cmds[(i * NCHANNELS) + j]) + wxT("\n");
            }
            wxMessageBox(allchannels);
        }
    } else {
        for (int i = 0; i < cmds.size(); i++)
            wxMessageBox(wxToString(cmds[i]));
    }    
}

wxString CatheterGuiFrame::wxToString(const CatheterChannelCmd &cmd)
{
    return wxString::Format("channel: %d\ncurrent: %3.3f\n", cmd.channel, cmd.currentMilliAmp);
}