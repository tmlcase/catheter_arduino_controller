#include "gui/status_text.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG

// explicit constructor
CatheterStatusText::CatheterStatusText(wxWindow* parent, wxWindowID id) : wxScrolledWindow(parent, id, wxDefaultPosition, wxSize(50,150)) {
		
        this -> SetBackgroundColour(*wxBLACK);
        this -> SetForegroundColour(*wxYELLOW);

        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

		statusText = new wxStaticText(this, wxID_ANY, wxEmptyString);
		sizer->Add(statusText, 0, wxALL, 5);

		this->SetSizer(sizer);
		this->FitInside();
		this->SetScrollRate(5, 5);
	}

    // public method to send text into the status box
    bool CatheterStatusText::addText(const std::string &incomingInfo)
    {
        boost::mutex::scoped_lock lock(textMutex);
        SetCatheterStatusText(wxString(incomingInfo));
        return true;
    }
	bool CatheterStatusText::addWxText(const wxString& msg)
    {
        boost::mutex::scoped_lock lock(textMutex);
        SetCatheterStatusText(msg);
        return true;
    }


	    // appends text and scrolls down.
void CatheterStatusText::SetCatheterStatusText(const wxString& msg) {
        wxString current = statusText->GetLabelText();
        current = current + wxT("\n") + msg;
        statusText->SetLabelText(current);
        
        this->FitInside();
        int r(this->GetScrollRange(wxVERTICAL));
        this->Scroll(0, r);
}