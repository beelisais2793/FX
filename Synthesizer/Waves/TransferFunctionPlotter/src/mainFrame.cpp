/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainFrame.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains the class functionality (event handlers, etc.) for the
//				 MainFrame class.  Uses wxWidgets for the GUI components.

// Local headers
#include "mainFrame.h"
#include "plotterApp.h"
#include "tfDialog.h"

// LibPlot2D headers
#include <lp2d/renderer/plotRenderer.h>
#include <lp2d/renderer/color.h>

// wxWidgets headers
#include <wx/grid.h>
#include <wx/colordlg.h>
#include <wx/splitter.h>

// Standard C++ headers
#include <algorithm>

// *nix Icons
#ifdef __WXGTK__
#include "res/tfPlotter16.xpm"
#include "res/tfPlotter24.xpm"
#include "res/tfPlotter32.xpm"
#include "res/tfPlotter48.xpm"
#include "res/tfPlotter64.xpm"
#include "res/tfPlotter128.xpm"
#include "res/tfPlotter256.xpm"
#endif

//==========================================================================
// Class:			MainFrame
// Function:		MainFrame
//
// Description:		Constructor for MainFrame class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, wxEmptyString, wxDefaultPosition,
	wxDefaultSize, wxDEFAULT_FRAME_STYLE), individualAmplitudeInterface(this),
	individualPhaseInterface(this), totalAmplitudeInterface(this), totalPhaseInterface(this)
{
	CreateControls();
	SetProperties();
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateControls
//
// Description:		Creates sizers and controls and lays them out in the window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::CreateControls()
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxSplitterWindow *mainSplitter = new wxSplitterWindow(this);
	topSizer->Add(mainSplitter, 1, wxGROW);

	wxPanel *lowerPanel = new wxPanel(mainSplitter);
	wxBoxSizer *lowerSizer = new wxBoxSizer(wxHORIZONTAL);
	lowerSizer->Add(CreateButtons(lowerPanel), 0, wxALL, 5);
	lowerSizer->Add(CreateOptionsGrid(lowerPanel), 1, wxGROW | wxALL, 5);
	lowerPanel->SetSizer(lowerSizer);

	wxWindow *upperPanel = new wxPanel(mainSplitter);
	individualAmplitudePlot = CreatePlotArea(upperPanel, individualAmplitudeInterface, _T("Amplitude"), _T("Amplitude [dB]"));
	individualPhasePlot = CreatePlotArea(upperPanel, individualPhaseInterface, _T("Phase"), _T("Phase [deg]"));
	totalAmplitudePlot = CreatePlotArea(upperPanel, totalAmplitudeInterface, _T("Amplitude (Total)"), _T("Amplitude [dB]"));
	totalPhasePlot = CreatePlotArea(upperPanel, totalPhaseInterface, _T("Phase (Total)"), _T("Phase [deg]"));

	wxGridSizer *upperSizer = new wxGridSizer(2,0,0);
	upperSizer->Add(individualAmplitudePlot, 1, wxGROW);
	upperSizer->Add(totalAmplitudePlot, 1, wxGROW);
	upperSizer->Add(individualPhasePlot, 1, wxGROW);
	upperSizer->Add(totalPhasePlot, 1, wxGROW);
	upperPanel->SetSizer(upperSizer);

	SetXLabels();

	mainSplitter->SplitHorizontally(upperPanel, lowerPanel, 500);
	mainSplitter->SetSize(GetClientSize());
	mainSplitter->SetSashGravity(1.0);
	mainSplitter->SetMinimumPaneSize(255);

	SetSizerAndFit(topSizer);
	mainSplitter->SetSashPosition(mainSplitter->GetSashPosition(), false);
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotArea
//
// Description:		Creates the main plot control.
//
// Input Arguments:
//		parent			= wxWindow*
//		guiInterface	= LibPlot2D::GuiInterface
//		title			= const wxString&
//		yLabel			= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		LibPlot2D::PlotRenderer* pointing to plotArea
//
//==========================================================================
LibPlot2D::PlotRenderer* MainFrame::CreatePlotArea(wxWindow *parent, LibPlot2D::GuiInterface& guiInterface, const wxString& title, const wxString& yLabel)
{
	wxGLAttributes displayAttributes;
	displayAttributes.PlatformDefaults().RGBA().DoubleBuffer().SampleBuffers(1).Samplers(4).Stencil(1).EndList();
	assert(wxGLCanvas::IsDisplaySupported(displayAttributes));
	LibPlot2D::PlotRenderer *plotArea = new LibPlot2D::PlotRenderer(guiInterface, *parent, wxID_ANY, displayAttributes);

	plotArea->SetMinSize(wxSize(500, 200));
	plotArea->SetMajorGridOn();
	plotArea->SetXLogarithmic(true);
	plotArea->SetCurveQuality(LibPlot2D::PlotRenderer::CurveQuality::HighWrite);

	plotArea->SetTitle(title);
	plotArea->SetLeftYLabel(yLabel);

	return plotArea;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateButtons
//
// Description:		Creates buttons to add/remove transfer functions.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//==========================================================================
wxSizer* MainFrame::CreateButtons(wxWindow *parent)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	wxButton *add = new wxButton(parent, idAddButton, _T("Add"));
	wxButton *remove = new wxButton(parent, idRemoveButton, _T("Remove"));
	wxButton *removeAll = new wxButton(parent, idRemoveAllButton, _T("Remove All"));

	sizer->Add(add, 1, wxGROW | wxALL, 2);
	sizer->Add(remove, 1, wxGROW | wxALL, 2);
	sizer->Add(removeAll, 1, wxGROW | wxALL, 2);

	sizer->AddSpacer(10);
	sizer->Add(CreateInputControls(parent));

	wxStaticText *version = new wxStaticText(parent, wxID_ANY, TransferFunctionPlotterApp::versionString);
	sizer->Add(version, 0, wxALL, 2);

	return sizer;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateInputControls
//
// Description:		Creates additional input controls.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//==========================================================================
wxSizer* MainFrame::CreateInputControls(wxWindow *parent)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *freqUnitsLabel = new wxStaticText(parent, wxID_ANY, _T("Frequency Units:"));
	frequencyUnitsHertzRadioButton = new wxRadioButton(parent, wxID_ANY, _T("Hz"));
	wxRadioButton *frequencyUnitsRadRadioButton = new wxRadioButton(parent, wxID_ANY, _T("rad/sec"));
	frequencyUnitsHertzRadioButton->SetValue(true);

	sizer->Add(freqUnitsLabel, 0, wxALL, 2);
	sizer->Add(frequencyUnitsHertzRadioButton, 0, wxALL, 2);
	sizer->Add(frequencyUnitsRadRadioButton, 0, wxALL, 2);
	sizer->AddSpacer(10);

	double minFreq(0.01), maxFreq(100.0);
	wxStaticText *minFreqLabel = new wxStaticText(parent, wxID_ANY, _T("Min. Frequency"));
	minFrequencyTextBox = new wxTextCtrl(parent, wxID_ANY, wxString::Format("%0.2f", minFreq));
	wxStaticText *maxFreqLabel = new wxStaticText(parent, wxID_ANY, _T("Max. Frequency"));
	maxFrequencyTextBox = new wxTextCtrl(parent, wxID_ANY, wxString::Format("%0.1f", maxFreq));
	dataManager.SetFrequencyRange(minFreq, maxFreq);

	minFrequencyTextBox->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(MainFrame::TextBoxChangeEvent), nullptr, this);
	maxFrequencyTextBox->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(MainFrame::TextBoxChangeEvent), nullptr, this);

	wxFlexGridSizer *gridSizer = new wxFlexGridSizer(2, 5, 5);
	gridSizer->Add(minFreqLabel);
	gridSizer->Add(minFrequencyTextBox);
	gridSizer->Add(maxFreqLabel);
	gridSizer->Add(maxFrequencyTextBox);

	sizer->Add(gridSizer, 0, wxALL, 2);

	return sizer;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateOptionsGrid
//
// Description:		Creates and formats the options grid.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxGrid* pointing to optionsGrid
//
//==========================================================================
wxGrid* MainFrame::CreateOptionsGrid(wxWindow *parent)
{
	optionsGrid = new wxGrid(parent, wxID_ANY);

	optionsGrid->BeginBatch();

	optionsGrid->CreateGrid(0, colCount, wxGrid::wxGridSelectRows);
	optionsGrid->SetRowLabelSize(0);
	optionsGrid->SetColFormatNumber(colLineSize);
	optionsGrid->SetColFormatFloat(colLeftCursor);
	optionsGrid->SetColFormatFloat(colRightCursor);
	optionsGrid->SetColFormatFloat(colDifference);
	optionsGrid->SetColFormatBool(colVisible);
	optionsGrid->SetColFormatBool(colRightAxis);

	optionsGrid->SetColLabelValue(colName, _T("Curve"));
	optionsGrid->SetColLabelValue(colColor, _T("Color"));
	optionsGrid->SetColLabelValue(colLineSize, _T("Line"));
	optionsGrid->SetColLabelValue(colLeftCursor, _T("Left Cursor"));
	optionsGrid->SetColLabelValue(colRightCursor, _T("Right Cursor"));
	optionsGrid->SetColLabelValue(colDifference, _T("Difference"));
	optionsGrid->SetColLabelValue(colVisible, _T("Visible"));
	optionsGrid->SetColLabelValue(colRightAxis, _T("Right Axis"));

	optionsGrid->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	optionsGrid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

	unsigned int i;
	for (i = 1; i < colCount; ++i)// Skip the name column
		optionsGrid->AutoSizeColLabelSize(i);

	optionsGrid->EndBatch();

	return optionsGrid;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetProperties
//
// Description:		Sets the window properties for this window.  Includes
//					title, frame size, and default fonts.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetProperties()
{
	SetTitle(TransferFunctionPlotterApp::transferFunctionPlotterTitle);
	SetName(TransferFunctionPlotterApp::transferFunctionPlotterName);
	Center();

#ifdef __WXMSW__
	SetIcon(wxIcon(_T("ICON_ID_MAIN"), wxBITMAP_TYPE_ICO_RESOURCE));
#elif __WXGTK__
	SetIcon(wxIcon(tfPlotter16_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter24_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter32_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter48_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter64_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter128_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter256_xpm, wxBITMAP_TYPE_XPM));
#endif
}

//==========================================================================
// Class:			MainFrame
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	// Buttons
	EVT_BUTTON(idAddButton,							MainFrame::AddButtonClicked)
	EVT_BUTTON(idRemoveButton,						MainFrame::RemoveButtonClicked)
	EVT_BUTTON(idRemoveAllButton,					MainFrame::RemoveAllButtonClicked)

	EVT_RADIOBUTTON(wxID_ANY,						MainFrame::RadioButtonChangeEvent)

	// Grid control
	EVT_GRID_CELL_RIGHT_CLICK(MainFrame::GridRightClickEvent)
	EVT_GRID_CELL_LEFT_DCLICK(MainFrame::GridDoubleClickEvent)
	EVT_GRID_CELL_LEFT_CLICK(MainFrame::GridLeftClickEvent)
	EVT_GRID_CELL_CHANGED(MainFrame::GridCellChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainFrame
// Function:		GetFileNameFromUser
//
// Description:		Displays a dialog asking the user to specify a file name.
//					Arguments allow this to be for opening or saving files,
//					with different options for the wildcards.
//
// Input Arguments:
//		dialogTitle			= wxString containing the title for the dialog
//		defaultDirectory	= wxString specifying the initial directory
//		defaultFileName		= wxString specifying the default file name
//		wildcard			= wxString specifying the list of file types to
//							  allow the user to select
//		style				= long integer specifying the type of dialog
//							  (this is usually wxFD_OPEN or wxFD_SAVE)
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing the paths and file names of the specified files,
//		or and empty array if the user cancels
//
//==========================================================================
wxArrayString MainFrame::GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
										 wxString defaultFileName, wxString wildcard, long style)
{
	wxArrayString pathsAndFileNames;

	wxFileDialog dialog(this, dialogTitle, defaultDirectory, defaultFileName,
		wildcard, style);

	dialog.CenterOnParent();
	if (dialog.ShowModal() == wxID_OK)
	{
		// If this was an open dialog, we want to get all of the selected paths,
		// otherwise, just get the one selected path
		if (style & wxFD_OPEN)
			dialog.GetPaths(pathsAndFileNames);
		else
			pathsAndFileNames.Add(dialog.GetPath());
	}

	return pathsAndFileNames;
}

//==========================================================================
// Class:			MainFrame
// Function:		AddButtonClicked
//
// Description:		Displays the add transfer function dialog.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	TFDialog dialog(this);
	dialog.CenterOnParent();
	if (dialog.ShowModal() != wxID_OK)
		return;

	AddCurve(dialog.GetNumerator(), dialog.GetDenominator());
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveButtonClicked
//
// Description:		Removes selected plot.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	wxArrayInt selection = optionsGrid->GetSelectedRows();
	selection.Sort([](int* a, int* b)
	{
		return *b - *a;
	});

	for (unsigned i = 0; i < selection.Count(); ++i)
	{
		if (selection[i] == 0)
			continue;
		RemoveCurve(selection[i] - 1);
	}

	UpdatePlotDisplays();
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveAllButtonClicked
//
// Description:		Removes all plots.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveAllButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	ClearAllCurves();
	UpdatePlotDisplays();
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateGridContextMenu
//
// Description:		Displays a context menu for the grid control.
//
// Input Arguments:
//		position	= const wxPoint& specifying the position to display the menu
//		row			= const unsigned int& specifying the row that was clicked
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::CreateGridContextMenu(const wxPoint &position, const unsigned int &row)
{
	// FIXME:  Remove?
}

//==========================================================================
// Class:			MainFrame
// Function:		ClearAllCurves
//
// Description:		Removes all curves from the plot.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ClearAllCurves()
{
	individualAmplitudeInterface.ClearAllCurves();
	individualPhaseInterface.ClearAllCurves();
	totalAmplitudeInterface.ClearAllCurves();
	totalPhaseInterface.ClearAllCurves();

	UpdatePlotDisplays();
}

//==========================================================================
// Class:			MainFrame
// Function:		AddCurve
//
// Description:		Adds an new dataset to the plot.
//
// Input Arguments:
//		numerator	= wxString
//		denominator	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddCurve(wxString numerator, wxString denominator)
{
	if (!dataManager.AddTransferFunction(numerator, denominator))
		return;

	optionsGrid->BeginBatch();
	if (optionsGrid->GetNumberRows() == 0)
		AddXRowToGrid();
	unsigned int index = AddDataRowToGrid("(" + numerator + ")/(" + denominator + ")");
	optionsGrid->EndBatch();

	totalAmplitudeInterface.ClearAllCurves();
	totalPhaseInterface.ClearAllCurves();

	totalAmplitudeInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetTotalAmplitudeData()), _T("Total Amplitude"));
	totalPhaseInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetTotalPhaseData()), _T("Total Phase"));

	individualAmplitudeInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetAmplitudeData(index - 1)), wxEmptyString);
	individualPhaseInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetPhaseData(index - 1)), wxEmptyString);
	UpdateCurveProperties(index - 1, GetNextColor(index), true, false);
	UpdatePlotDisplays();
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCurve
//
// Description:		Updates an existing dataset.
//
// Input Arguments:
//		i			= unsigned int
//		numerator	= wxString
//		denominator	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCurve(unsigned int i, wxString numerator, wxString denominator)
{
	if (!dataManager.UpdateTransferFunction(i, numerator, denominator))
		return;

	optionsGrid->SetCellValue(i + 1, 0, "(" + numerator + ")/(" + denominator + ")");
	UpdatePlotData();
}

//==========================================================================
// Class:			MainFrame
// Function:		AddTimeRowToGrid
//
// Description:		Adds the entry for the time data to the options grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddXRowToGrid()
{
	optionsGrid->AppendRows();
	for (unsigned int i = 0; i < colCount; ++i)
		optionsGrid->SetReadOnly(0, i, true);

	optionsGrid->SetCellValue(0, 0, xLabel);
}

//==========================================================================
// Class:			MainFrame
// Function:		AddDataRowToGrid
//
// Description:		Adds the entry for the data to the options grid.
//
// Input Arguments:
//		name	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the index of the new data
//
//==========================================================================
unsigned int MainFrame::AddDataRowToGrid(const wxString &name)
{
	unsigned int index = optionsGrid->GetNumberRows();
	optionsGrid->AppendRows();

	unsigned int maxLineSize(5);

	optionsGrid->SetCellRenderer(index, colVisible, new wxGridCellBoolRenderer);
	optionsGrid->SetCellRenderer(index, colRightAxis, new wxGridCellBoolRenderer);
	optionsGrid->SetCellEditor(index, colLineSize, new wxGridCellNumberEditor(1, maxLineSize));

	for (unsigned int i = 0; i < colCount; ++i)
			optionsGrid->SetReadOnly(index, i, true);
	optionsGrid->SetReadOnly(index, colLineSize, false);
	optionsGrid->SetCellValue(index, colName, name);

	LibPlot2D::Color color = GetNextColor(index);

	optionsGrid->SetCellBackgroundColour(index, colColor, color.ToWxColor());
	optionsGrid->SetCellValue(index, colLineSize, _T("1"));
	optionsGrid->SetCellValue(index, colVisible, _T("1"));

/*	int width = optionsGrid->GetColumnWidth(colName);
	optionsGrid->AutoSizeColumn(colName);
	if (optionsGrid->GetColumnWidth(colName) < width)
		optionsGrid->SetColumnWidth(colName, width);*/

	return index;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetNextColor
//
// Description:		Determines the next color to use (cycles through all the
//					pre-defined colors).
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		Color to sue
//
//==========================================================================
LibPlot2D::Color MainFrame::GetNextColor(const unsigned int &index) const
{
	unsigned int colorIndex = (index - 1) % 10;
	if (colorIndex == 0)
		return LibPlot2D::Color::ColorBlue;
	else if (colorIndex == 1)
		return LibPlot2D::Color::ColorRed;
	else if (colorIndex == 2)
		return LibPlot2D::Color::ColorGreen;
	else if (colorIndex == 3)
		return LibPlot2D::Color::ColorMagenta;
	else if (colorIndex == 4)
		return LibPlot2D::Color::ColorCyan;
	else if (colorIndex == 5)
		return LibPlot2D::Color::ColorOrange;
	else if (colorIndex == 6)
		return LibPlot2D::Color::ColorGray;
	else if (colorIndex == 7)
		return LibPlot2D::Color::ColorPurple;
	else if (colorIndex == 8)
		return LibPlot2D::Color::ColorLightBlue;
	else if (colorIndex == 9)
		return LibPlot2D::Color::ColorBlack;
	else
		assert(false);

	// The following colors we opt'ed not to use - either too hard to see or too similar to other colors
	// Color::ColorYellow
	// Color::ColorDrabGreen
	// Color::ColorPaleGreen
	// Color::ColorPink

	return LibPlot2D::Color::ColorBlack;
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveCurve
//
// Description:		Removes a curve from the plot.
//
// Input Arguments:
//		i	= const unsigned int& specifying curve to remove
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveCurve(const unsigned int &i)
{
	optionsGrid->DeleteRows(i + 1);

	if (optionsGrid->GetNumberRows() == 1)
		optionsGrid->DeleteRows();

	optionsGrid->AutoSizeColumns();

	dataManager.RemoveTransferFunctions(i);
	if (dataManager.GetCount() == 0)
	{
		ClearAllCurves();
	}
	else
	{
		individualAmplitudeInterface.RemoveCurve(i);
		individualPhaseInterface.RemoveCurve(i);

		totalAmplitudeInterface.ClearAllCurves();
		totalPhaseInterface.ClearAllCurves();

		totalAmplitudeInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetTotalAmplitudeData()), _T("Total Amplitude"));
		totalPhaseInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetTotalPhaseData()), _T("Total Phase"));
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		GridRightClickEvent
//
// Description:		Handles right-click events on the grid control.  Displays
//					context menu.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridRightClickEvent(wxGridEvent &event)
{
	optionsGrid->SelectRow(event.GetRow());
	CreateGridContextMenu(event.GetPosition() + optionsGrid->GetPosition()
		+ optionsGrid->GetParent()->GetPosition(), event.GetRow());
}

//==========================================================================
// Class:			MainFrame
// Function:		GridDoubleClickEvent
//
// Description:		Handles double click event for the grid control.  If the
//					click occurs on the color box, display the color dialog.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridDoubleClickEvent(wxGridEvent &event)
{
	// No color for Time
	int row = event.GetRow();
	if (row == 0)
		return;

	if (event.GetCol() == colName)
		UpdateCurve(row - 1);
	
	if (event.GetCol() != colColor)
		return;

	wxColourData colorData;
	colorData.SetColour(optionsGrid->GetCellBackgroundColour(row, colColor));

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Line Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
        colorData = dialog.GetColourData();
		optionsGrid->SetCellBackgroundColour(row, colColor, colorData.GetColour());
		UpdateCurveProperties(row - 1);
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCurve
//
// Description:		Updates the TF associated with the specified curve.
//
// Input Arguments:
//		i	= unsigned int
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCurve(unsigned int i)
{
	TFDialog dialog(this, dataManager.GetNumerator(i), dataManager.GetDenominator(i));
	dialog.CenterOnParent();
	if (dialog.ShowModal() != wxID_OK)
		return;

	UpdateCurve(i, dialog.GetNumerator(), dialog.GetDenominator());
}

//==========================================================================
// Class:			MainFrame
// Function:		GridLeftClickEvent
//
// Description:		Handles grid cell change events (for boolean controls).
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridLeftClickEvent(wxGridEvent &event)
{
	unsigned int row = event.GetRow();

	// This stuff may be necessary after bug is fixed with wxGrid::GetSelectedRows()?
	// See ButtonRemoveCurveClickedEvent() for details
	//optionsGrid->SetSelectionMode(wxGrid::wxGridSelectRows);
	//optionsGrid->SelectRow(row, event.ControlDown());

	// Skip to handle row selection (with SHIFT and CTRL) and also boolean column click handlers
	event.Skip();

	// Was this click in one of the boolean columns and not in the time row?
	if (row == 0 || (event.GetCol() != colVisible && event.GetCol() != colRightAxis))
		return;

	if (optionsGrid->GetCellValue(row, event.GetCol()).Cmp(_T("1")) == 0)
		optionsGrid->SetCellValue(row, event.GetCol(), wxEmptyString);
	else
		optionsGrid->SetCellValue(row, event.GetCol(), _T("1"));

	UpdateCurveProperties(row - 1);
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties.
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCurveProperties(const unsigned int &index)
{
	LibPlot2D::Color color;
	color.Set(optionsGrid->GetCellBackgroundColour(index + 1, colColor));
	UpdateCurveProperties(index, color,
		!optionsGrid->GetCellValue(index + 1, colVisible).IsEmpty(),
		!optionsGrid->GetCellValue(index + 1, colRightAxis).IsEmpty());
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties to match the arguments.
//
// Input Arguments:
//		index		= const unsigned int&
//		color		= const LibPlot2D::Color&
//		visible		= const bool&
//		rightAxis	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCurveProperties(const unsigned int &index, const LibPlot2D::Color &color,
	const bool &visible, const bool &rightAxis)
{
	unsigned long lineSize;
	optionsGrid->GetCellValue(index + 1, colLineSize).ToULong(&lineSize);
	individualAmplitudePlot->SetCurveProperties(index, color, visible, rightAxis, lineSize, -1);
	individualPhasePlot->SetCurveProperties(index, color, visible, rightAxis, lineSize, -1);

	if (totalAmplitudeInterface.GetCurveCount() > 0)
		totalAmplitudePlot->SetCurveProperties(0, LibPlot2D::Color::ColorBlue, true, false, 1, 0);
	if (totalPhaseInterface.GetCurveCount() > 0)
		totalPhasePlot->SetCurveProperties(0, LibPlot2D::Color::ColorBlue, true, false, 1, 0);
}

//==========================================================================
// Class:			MainFrame
// Function:		GridCellChangeEvent
//
// Description:		Handles grid cell change events (for text controls).
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridCellChangeEvent(wxGridEvent &event)
{
	unsigned int row(event.GetRow());
	if (row == 0 || event.GetCol() != colLineSize)
	{
		event.Skip();
		return;
	}

	UpdateCurveProperties(row - 1);
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateSingleCursorValue
//
// Description:		Updates a single cursor value.
//
// Input Arguments:
//		row		= const unsigned int& specifying the grid row
//		value	= const double& specifying the value to populate
//		column	= const unsigned int& specifying which grid column to populate
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateSingleCursorValue(const unsigned int &row,
	double value, const unsigned int &column, const bool &isVisible)
{
	/*if (isVisible)
	{
		optionsGrid->SetCellValue(0, column, wxString::Format("%f", value));

		if (plotList[row - 1]->GetYAt(value))
			optionsGrid->SetCellValue(row, column, _T("*") + wxString::Format("%f", value));
		else
			optionsGrid->SetCellValue(row, column, wxString::Format("%f", value));
	}
	else
	{
		optionsGrid->SetCellValue(0, column, wxEmptyString);
		optionsGrid->SetCellValue(row, column, wxEmptyString);

		// The difference column only exists if both cursors are visible
		optionsGrid->SetCellValue(0, colDifference, wxEmptyString);
		optionsGrid->SetCellValue(row, colDifference, wxEmptyString);
	}*/
}

//==========================================================================
// Class:			MainFrame
// Function:		SetXLabels
//
// Description:		Sets the x-label for all four plots depending on selected
//					frequency units.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetXLabels()
{
	if (frequencyUnitsHertzRadioButton->GetValue())
		xLabel = _T("Frequency [Hz]");
	else
		xLabel = _T("Frequency [rad/sec]");

	individualAmplitudeInterface.SetXDataLabel(xLabel);
	individualPhaseInterface.SetXDataLabel(xLabel);
	totalAmplitudeInterface.SetXDataLabel(xLabel);
	totalPhaseInterface.SetXDataLabel(xLabel);
}

//==========================================================================
// Class:			MainFrame
// Function:		TextBoxChangeEvent
//
// Description:		Event handler for text box change (really loss of focus)
//					events.
//
// Input Arguments:
//		event	= wxFocusEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::TextBoxChangeEvent(wxFocusEvent& event)
{
	double min, max;
	if (minFrequencyTextBox->GetValue().ToDouble(&min) &&
		maxFrequencyTextBox->GetValue().ToDouble(&max))
		dataManager.SetFrequencyRange(min, max);
	UpdatePlotData();
	event.Skip();// Without skipping the event, the cursot gets stuck in the box and we never loose focus
}

//==========================================================================
// Class:			MainFrame
// Function:		RadioButtonChangeEvent
//
// Description:		Event handler for radio button change events.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RadioButtonChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	SetXLabels();
	if (frequencyUnitsHertzRadioButton->GetValue())
		dataManager.SetFrequencyUnitsHertz();
	else
		dataManager.SetFrequencyUnitsRadPerSec();
	UpdatePlotData();
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdatePlotData
//
// Description:		Updates data for all rendered plots.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdatePlotData()
{
	dataManager.UpdateTotalTransferFunctionData();

	individualAmplitudeInterface.ClearAllCurves();
	individualPhaseInterface.ClearAllCurves();
	totalAmplitudeInterface.ClearAllCurves();
	totalPhaseInterface.ClearAllCurves();

	for (unsigned int i = 0; i < dataManager.GetCount(); ++i)
	{
		individualAmplitudeInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetAmplitudeData(i)), wxEmptyString);
		individualPhaseInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetPhaseData(i)), wxEmptyString);
		UpdateCurveProperties(i);
	}

	totalAmplitudeInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetTotalAmplitudeData()), _T("Total Amplitude"));
	totalPhaseInterface.AddCurve(std::make_unique<LibPlot2D::Dataset2D>(dataManager.GetTotalPhaseData()), _T("Total Phase"));

	UpdatePlotDisplays();
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdatePlotDisplays
//
// Description:		Updates rendering of all rendered plots.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdatePlotDisplays()
{
	individualAmplitudePlot->UpdateDisplay();
	individualPhasePlot->UpdateDisplay();
	totalAmplitudePlot->UpdateDisplay();
	totalPhasePlot->UpdateDisplay();
}
