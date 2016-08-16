#pragma once
#ifndef CATHETER_GUI_APP_H
#define CATHETER_GUI_APP_H


#include "ser/serial_thread.h"

#include <wx/wx.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "gui/catheter_grid.h"
#include "gui/status_text.h"

#include "gui/status_frame.h"

//This file defines the gui layout

class CatheterGuiFrame : public wxFrame {
public:
    
	CatheterGuiFrame(const wxString& title, SerialThreadObject *);
    ~CatheterGuiFrame();

    // control buttons
    void OnRefreshSerialButtonClicked(wxCommandEvent& e);
    void OnSelectPlayfileButtonClicked(wxCommandEvent& e);
    void OnNewPlayfileButtonClicked(wxCommandEvent& e);
    void OnSavePlayfileButtonClicked(wxCommandEvent& e);
    void OnSendCommandsButtonClicked(wxCommandEvent& e);
    void OnSendResetButtonClicked(wxCommandEvent& e);
	void OnSendPollButtonClicked(wxCommandEvent& e);
	void onIdle(wxIdleEvent& e);

    enum {
        ID_SELECT_PLAYFILE_BUTTON = 1024,
        ID_NEW_PLAYFILE_BUTTON,
        ID_SAVE_PLAYFILE_BUTTON,
        ID_SEND_COMMANDS_BUTTON,
        ID_SEND_RESET_BUTTON, 
        ID_REFRESH_SERIAL_BUTTON,
		ID_SEND_POLL_BUTTON
    };

    wxDECLARE_EVENT_TABLE();

	

private:
    // status panel
    void setStatusText(const wxString& msg);
    // control buttons
    void warnSavePlayfile();
    void savePlayfile();
    wxString openPlayfile();
    void loadPlayfile(const wxString& path);
    void unloadPlayfile(const wxString& path);
    bool sendCommands(const std::vector<CatheterChannelCmdSet> &cmdVect);
    bool sendGridCommands();
    bool sendResetCommand();
	bool sendPollCommand();
    bool refreshSerialConnection();
    bool closeSerialConnection();
    wxString wxToString(const CatheterChannelCmd &cmd);
    void wxSummarizeCmds(const std::vector<CatheterChannelCmd> &cmds);

    wxPanel* parentPanel;
    CatheterGrid* grid;

    // status panel
    //wxStaticText* statusText;
	CatheterStatusText *statusText;
	incomingText* statusTextData;

	//status Grid
	statusData * statusGridCmdPtr;
	StatusGrid * statusGridPtr;

    // control buttons
    wxButton* selectPlayfileButton;
    wxButton* newPlayfileButton;
    wxButton* savePlayfileButton;
    wxButton* sendCommandsButton;
    wxButton* sendResetButton;
	wxButton* pollButton;
    wxButton* refreshSerialButton;
    bool playfileSaved;
    wxString playfilePath;

	SerialThreadObject *serialObject;
	
};

class CatheterGuiApp : public wxApp {
public:
    bool OnInit();
	int OnExit();

private:
	CatheterGuiFrame* gui;
	// thead object pointer
	SerialThreadObject *serialObject;
};


#endif