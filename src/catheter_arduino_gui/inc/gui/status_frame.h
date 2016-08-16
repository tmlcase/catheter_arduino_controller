#ifndef CATHETER_STATUS_H
#define CATHETER_STATUS_H


#include <wx/wx.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "com/catheter_commands.h"


// This files defines the display class which shows the current status of individual
// catheter channels. (ADC and DAC)
// This is the status frame.

// a shared object is included to avoid race conditions.
struct statusData
{
	boost::mutex statusMutex;
	std::vector<CatheterChannelCmd> inputCommands;
	bool updated;

	// update function.
	void updateCmdList(std::vector<CatheterChannelCmd> & inputCommands);

	statusData() : updated(false)
	{};
};



class StatusGrid: public wxFlexGridSizer
{

public:
	 StatusGrid(wxPanel* parent);
    ~StatusGrid();

    void updateStatus(statusData*);

    // void SetCommands(const std::vector<CatheterChannelCmdSet>& cmds);
	// void GetCommands(std::vector<CatheterChannelCmdSet>& cmds);
    // void ResetDefault();

private:
	

	std::vector < wxTextCtrl* > textCtrl;



};

#endif