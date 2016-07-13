#ifndef CATHETER_STATUS_H
#define CATHETER_STATUS_H


#include <wx/wx.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "com/catheter_commands.h"


// This files defines the display class which shows the current status of individual
// catheter channels. (ADC and DAC)
// This is the status frame.

class StatusGrid: public wxFlexGridSizer
{

public:
	 StatusGrid(wxPanel* parent);
    ~StatusGrid();

    void updateStatus(const std::vector<CatheterChannelCmd> & inputCommands);

    void SetCommands(const std::vector<CatheterChannelCmdSet>& cmds);
	void GetCommands(std::vector<CatheterChannelCmdSet>& cmds);
    void ResetDefault();

private:
	boost::mutex statusMutex;

	std::vector < wxTextCtrl* > textCtrl;



};

#endif