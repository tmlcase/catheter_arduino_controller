#ifndef CATHETER_GRID_H
#define CATHETER_GRID_H

#include "wx/wx.h"
#include "wx/panel.h"
#include "wx/grid.h"
#include "wx/headerctrl.h"
#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"
#include <vector>

#include "com/common_utils.h"

class CatheterGrid : public wxGrid {
    public:
    CatheterGrid(wxPanel* parent);
    ~CatheterGrid();

    void OnGridCellChanging(wxGridEvent& e);

    void SetCommands(const std::vector<CatheterChannelCmdSet>& cmds);
	void GetCommands(std::vector<CatheterChannelCmdSet>& cmds);
    void ResetDefault();

    wxDECLARE_EVENT_TABLE();

    private:
	void RecalculateGridSize(int rows);
    void setRowReadOnly(int row, bool readOnly);
    void formatDefaultRow(int row);
    void formatDefaultGrid(int nrows);
    void resetDefaultGrid(int nrows);
    bool isGridRowNumValid(int row);
    bool isGridCellEmpty(int row, int col);
    bool isGridRowComplete(int row);

	long parseGridRow(int row, CatheterChannelCmd& c); 

    void addGridRow(bool readOnly);
    void setGridRowChannel(int row, int channel);
    void setGridRowChannel(int row, const wxString& channel);
    void setGridRowCurrentMA(int row, double currentMA);
    void setGridRowDirection(int row, dir_t direction);
    void setGridRowDelayMS(int row, int delayMS);
    int getGridRowChannel(int row);
    double getGridRowCurrentMA(int row);
    dir_t getGridRowDirection(int row);
    int getGridRowDelayMS(int row);

    unsigned int cmdCount;
};

#endif