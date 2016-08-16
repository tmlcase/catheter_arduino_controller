#include "gui/status_frame.h"

StatusGrid:: ~StatusGrid()
{
}

StatusGrid::StatusGrid(wxPanel* parentPanel):
	wxFlexGridSizer(7, 4, 2, 10)
{
	wxStaticText *channelIndex = new wxStaticText(parentPanel, -1, wxT("Channel #"));
	wxStaticText *setMa = new wxStaticText(parentPanel, -1, wxT("Set Current"));
	wxStaticText *senseMa = new wxStaticText(parentPanel, -1, wxT("Sensed Current"));
	wxStaticText *enable = new wxStaticText(parentPanel, -1, wxT("enabled"));


	this->Add(channelIndex);
	this->Add(setMa);
	this->Add(senseMa);
	this->Add(enable);

	textCtrl.clear();
	for ( int index(0); index < 24; index++)
	{
		textCtrl.push_back(new wxTextCtrl(parentPanel, -1));
		textCtrl[index]->SetEditable(false);

		switch (index % 4)
		{
		case 0: // channel number
			textCtrl[index]->SetValue(wxString::Format(wxT("%d"), (index  >> 2)+1));
			break;
		case 1:
		case 2:
			textCtrl[index]->SetValue(wxT("0.00"));
			break;
		case 3:
			textCtrl[index]->SetValue(wxT("false"));
			break;
		}
		this->Add(textCtrl[index]);
	}

   
}

void StatusGrid::updateStatus(statusData* inputData)
{
	
	boost::mutex::scoped_lock lock(inputData->statusMutex);
	if (inputData->updated)
	{
		int cmdCount(inputData->inputCommands.size());
		for (int index(0); index < cmdCount; index++)
		{
			int channelNum(inputData->inputCommands[index].channel);
			int baseIndex(((channelNum - 1) << 2));
			textCtrl[baseIndex + 1]->SetValue(wxString::Format(wxT("%f"), inputData->inputCommands[index].currentMilliAmp));
			textCtrl[baseIndex + 2]->SetValue(wxString::Format(wxT("%f"), inputData->inputCommands[index].currentMilliAmp_ADC));
			if (inputData->inputCommands[index].enable)
			{
				textCtrl[baseIndex + 3]->SetValue(wxT("true"));
			}
			else
			{
				textCtrl[baseIndex + 3]->SetValue(wxT("false"));
			}
		}
		inputData->updated = false;
	}
	return;
}


void statusData::updateCmdList(std::vector<CatheterChannelCmd> & inputCommands_)
{
	boost::mutex::scoped_lock lock(this->statusMutex);
	this->inputCommands = inputCommands_;
	this->updated = true;

}

 /*  wxPanel *panel = new wxPanel(this, -1);

  wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

  wxFlexGridSizer *fgs = new wxFlexGridSizer(3, 2, 9, 25);



  wxTextCtrl *tc1 = new wxTextCtrl(panel, -1);
  wxTextCtrl *tc2 = new wxTextCtrl(panel, -1);
  wxTextCtrl *tc3 = new wxTextCtrl(panel, -1, wxT(""),
      wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE);

  fgs->Add(thetitle);
  fgs->Add(tc1, 1, wxEXPAND);
  fgs->Add(author);
  fgs->Add(tc2, 1, wxEXPAND);
  fgs->Add(review, 1, wxEXPAND);
  fgs->Add(tc3, 1, wxEXPAND);

  fgs->AddGrowableRow(2, 1);
  fgs->AddGrowableCol(1, 1);

  hbox->Add(fgs, 1, wxALL | wxEXPAND, 15);
  panel->SetSizer(hbox);
  Centre();
} */