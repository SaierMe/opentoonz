

#include "menubar.h"

// Tnz6 includes
#include "menubarcommandids.h"
#include "tapp.h"
#include "cellselection.h"

// TnzQt includes
#include "toonzqt/tselectionhandle.h"
#include "toonzqt/dvdialog.h"
#include "toonzqt/menubarcommand.h"

// TnzLib includes
#include "toonz/toonzscene.h"
#include "toonz/childstack.h"
#include "toonz/tscenehandle.h"
#include "toonz/txsheethandle.h"
#include "toonz/tframehandle.h"
#include "toonz/tcolumnhandle.h"

// TnzTools includes
#include "tools/toolcommandids.h"

// TnzBase includes
#include "tenv.h"

// TnzCore includes
#include "tconvert.h"

// Qt includes
#include <QIcon>
#include <QPainter>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QShortcut>
#include <QDesktopServices>

void UrlOpener::open()
{
	QDesktopServices::openUrl(m_url);
}

UrlOpener dvHome(QUrl("http://www.toonz.com/"));
UrlOpener manual(QUrl("file:///C:/gmt/butta/M&C in EU.pdf"));

//=============================================================================
// StackedMenuBar
//-----------------------------------------------------------------------------

StackedMenuBar::StackedMenuBar(QWidget *parent)
	: QStackedWidget(parent)
{
	setObjectName("StackedMenuBar");
}

//---------------------------------------------------------------------------------

void StackedMenuBar::createMenuBarByName(const QString &roomName)
{
	std::cout << "create " << roomName.toStdString() << std::endl;
	if (roomName == "Cleanup")
		createCleanupMenuBar();
	else if (roomName == "PltEdit")
		createPltEditMenuBar();
	else if (roomName == "InknPaint")
		createInknPaintMenuBar();
	else if (roomName == "Xsheet" || roomName == "Schematic" || roomName == "QAR" || roomName == "Flip")
		createXsheetMenuBar();
	else if (roomName == "Batches")
		createBatchesMenuBar();
	else if (roomName == "Browser")
		createBrowserMenuBar();
	else /*-- どれにもあてはまらない場合は全てのコマンドの入ったメニューバーを作る --*/
		createFullMenuBar();
}

//---------------------------------------------------------------------------------

void StackedMenuBar::createCleanupMenuBar()
{
	QMenuBar *cleanupMenuBar;
	cleanupMenuBar = new QMenuBar(this);
	//----Files Menu
	QMenu *filesMenu = addMenu(tr("Files"), cleanupMenuBar);
	addMenuItem(filesMenu, MI_LoadLevel);
	addMenuItem(filesMenu, MI_LoadFolder);
	addMenuItem(filesMenu, MI_SaveLevel);
	addMenuItem(filesMenu, MI_SaveLevelAs);
	addMenuItem(filesMenu, MI_ExportLevel);
	addMenuItem(filesMenu, MI_OpenRecentLevel);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_LoadScene);
	addMenuItem(filesMenu, MI_SaveScene);
	addMenuItem(filesMenu, MI_SaveSceneAs);
	addMenuItem(filesMenu, MI_OpenRecentScene);
	addMenuItem(filesMenu, MI_RevertScene);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_ConvertFileWithInput);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_NewScene);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_Quit);

	//----Scan Menu
	QMenu *scanMenu = addMenu(tr("Scan"), cleanupMenuBar);
	addMenuItem(scanMenu, MI_DefineScanner);
	addMenuItem(scanMenu, MI_ScanSettings);
	addMenuItem(scanMenu, MI_Scan);
	addMenuItem(scanMenu, MI_SetScanCropbox);
	addMenuItem(scanMenu, MI_ResetScanCropbox);

	//----Settings Menu
	QMenu *settingsMenu = addMenu(tr("Settings"), cleanupMenuBar);
	addMenuItem(settingsMenu, MI_CleanupSettings);
	settingsMenu->addSeparator();
	addMenuItem(settingsMenu, MI_OutputSettings);
	addMenuItem(settingsMenu, MI_LevelSettings);

	//----Processing Menu
	QMenu *processingMenu = addMenu(tr("Processing"), cleanupMenuBar);
	addMenuItem(processingMenu, MI_CameraTest);
	addMenuItem(processingMenu, MI_OpacityCheck);
	addMenuItem(processingMenu, MI_CleanupPreview);
	addMenuItem(processingMenu, MI_Cleanup);

	//---Edit Menu
	QMenu *editMenu = addMenu(tr("Edit"), cleanupMenuBar);
	addMenuItem(editMenu, MI_Undo);
	addMenuItem(editMenu, MI_Redo);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_NextFrame);
	addMenuItem(editMenu, MI_PrevFrame);
	addMenuItem(editMenu, MI_FirstFrame);
	addMenuItem(editMenu, MI_LastFrame);
	addMenuItem(editMenu, MI_TestAnimation);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Copy);
	addMenuItem(editMenu, MI_Cut);
	addMenuItem(editMenu, MI_Paste);
	addMenuItem(editMenu, MI_PasteInto);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Clear);
	addMenuItem(editMenu, MI_Insert);
	addMenuItem(editMenu, MI_SelectAll);
	addMenuItem(editMenu, MI_InvertSelection);

	//----Windows Menu
	QMenu *windowsMenu = addMenu(tr("Windows"), cleanupMenuBar);
	addMenuItem(windowsMenu, MI_OpenFileBrowser);
	addMenuItem(windowsMenu, MI_OpenStyleControl);
	addMenuItem(windowsMenu, MI_OpenComboViewer);
	addMenuItem(windowsMenu, MI_OpenXshView);
	windowsMenu->addSeparator();
	QMenu *otherWindowsMenu = windowsMenu->addMenu(tr("Other Windows"));
	{
		addMenuItem(otherWindowsMenu, MI_OpenBatchServers);
		addMenuItem(otherWindowsMenu, MI_OpenTasks);
		addMenuItem(otherWindowsMenu, MI_OpenColorModel);
		addMenuItem(otherWindowsMenu, MI_OpenFileViewer);
		addMenuItem(otherWindowsMenu, MI_OpenFunctionEditor);
		addMenuItem(otherWindowsMenu, MI_OpenFilmStrip);
		addMenuItem(otherWindowsMenu, MI_OpenPalette);
		addMenuItem(otherWindowsMenu, MI_OpenFileBrowser2);
		addMenuItem(otherWindowsMenu, MI_OpenSchematic);
		addMenuItem(otherWindowsMenu, MI_OpenStudioPalette);
		addMenuItem(otherWindowsMenu, MI_OpenToolbar);
		addMenuItem(otherWindowsMenu, MI_OpenToolOptionBar);
		addMenuItem(otherWindowsMenu, MI_OpenHistoryPanel);
		addMenuItem(otherWindowsMenu, MI_OpenTMessage);
	}

	//----Customize Menu
	QMenu *customizeMenu = addMenu(tr("Customize"), cleanupMenuBar);
	addMenuItem(customizeMenu, MI_Preferences);
	addMenuItem(customizeMenu, MI_ShortcutPopup);
	addMenuItem(customizeMenu, MI_SceneSettings);
	customizeMenu->addSeparator();
	QMenu *viewPartsMenu = customizeMenu->addMenu(tr("View"));
	{
		addMenuItem(viewPartsMenu, MI_ViewCamera);
		addMenuItem(viewPartsMenu, MI_ViewTable);
		addMenuItem(viewPartsMenu, MI_FieldGuide);
		addMenuItem(viewPartsMenu, MI_SafeArea);
		addMenuItem(viewPartsMenu, MI_ViewBBox);
		addMenuItem(viewPartsMenu, MI_ViewColorcard);
	}
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, MI_DockingCheck);
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, "MI_RunScript");
	addMenuItem(customizeMenu, "MI_OpenScriptConsole");
#ifndef NDEBUG
	addMenuItem(customizeMenu, "MI_ReloadStyle");
#endif

	//----Help Menu
	QMenu *helpMenu = addMenu(tr("Help"), cleanupMenuBar);
	addMenuItem(helpMenu, MI_About);

	addWidget(cleanupMenuBar);
}

//---------------------------------------------------------------------------------

void StackedMenuBar::createPltEditMenuBar()
{
	QMenuBar *pltEditMenuBar;
	pltEditMenuBar = new QMenuBar(this);

	//---Files Menu
	QMenu *filesMenu = addMenu(tr("Files"), pltEditMenuBar);
	addMenuItem(filesMenu, MI_LoadLevel);
	addMenuItem(filesMenu, MI_LoadFolder);
	addMenuItem(filesMenu, MI_SaveLevel);
	addMenuItem(filesMenu, MI_SaveLevelAs);
	addMenuItem(filesMenu, MI_ExportLevel);
	addMenuItem(filesMenu, MI_OpenRecentLevel);
	addMenuItem(filesMenu, MI_LevelSettings);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_NewLevel);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_OverwritePalette);
	addMenuItem(filesMenu, MI_SavePaletteAs);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_LoadScene);
	addMenuItem(filesMenu, MI_SaveScene);
	addMenuItem(filesMenu, MI_SaveSceneAs);
	addMenuItem(filesMenu, MI_OpenRecentScene);
	addMenuItem(filesMenu, MI_RevertScene);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_ConvertFileWithInput);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_NewScene);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_Quit);

	//---Tools Menu
	QMenu *toolsMenu = addMenu(tr("Tools"), pltEditMenuBar);
	addMenuItem(toolsMenu, T_StylePicker);
	addMenuItem(toolsMenu, T_RGBPicker);
	addMenuItem(toolsMenu, T_Tape);
	toolsMenu->addSeparator();
	addMenuItem(toolsMenu, T_Fill);
	addMenuItem(toolsMenu, T_Brush);
	addMenuItem(toolsMenu, T_PaintBrush);
	addMenuItem(toolsMenu, T_Geometric);
	addMenuItem(toolsMenu, T_Type);
	toolsMenu->addSeparator();
	addMenuItem(toolsMenu, T_Eraser);
	QMenu *moreToolsMenu = toolsMenu->addMenu(tr("More Tools"));
	{
		addMenuItem(moreToolsMenu, T_Edit);
		addMenuItem(moreToolsMenu, T_Selection);
		moreToolsMenu->addSeparator();
		addMenuItem(moreToolsMenu, T_ControlPointEditor);
		addMenuItem(moreToolsMenu, T_Pinch);
		addMenuItem(moreToolsMenu, T_Pump);
		addMenuItem(moreToolsMenu, T_Magnet);
		addMenuItem(moreToolsMenu, T_Bender);
		addMenuItem(moreToolsMenu, T_Iron);
		addMenuItem(moreToolsMenu, T_Cutter);
		moreToolsMenu->addSeparator();
		addMenuItem(moreToolsMenu, T_Skeleton);
		addMenuItem(moreToolsMenu, T_Tracker);
		addMenuItem(moreToolsMenu, T_Hook);
		addMenuItem(moreToolsMenu, T_Plastic);
		moreToolsMenu->addSeparator();
		addMenuItem(moreToolsMenu, T_Zoom);
		addMenuItem(moreToolsMenu, T_Rotate);
		addMenuItem(moreToolsMenu, T_Hand);
	}

	//---Edit Menu
	QMenu *editMenu = addMenu(tr("Edit"), pltEditMenuBar);
	addMenuItem(editMenu, MI_Undo);
	addMenuItem(editMenu, MI_Redo);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_NextFrame);
	addMenuItem(editMenu, MI_PrevFrame);
	addMenuItem(editMenu, MI_FirstFrame);
	addMenuItem(editMenu, MI_LastFrame);
	addMenuItem(editMenu, MI_TestAnimation);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Copy);
	addMenuItem(editMenu, MI_Cut);
	addMenuItem(editMenu, MI_Paste);
	addMenuItem(editMenu, MI_PasteInto);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Clear);
	addMenuItem(editMenu, MI_Insert);
	addMenuItem(editMenu, MI_SelectAll);
	addMenuItem(editMenu, MI_InvertSelection);

	//---Checks Menu
	QMenu *checksMenu = addMenu(tr("Checks"), pltEditMenuBar);
	addMenuItem(checksMenu, MI_TCheck);
	addMenuItem(checksMenu, MI_BCheck);
	addMenuItem(checksMenu, MI_ICheck);
	addMenuItem(checksMenu, MI_Ink1Check);
	addMenuItem(checksMenu, MI_PCheck);

	//---Render Menu
	QMenu *renderMenu = addMenu(tr("Render"), pltEditMenuBar);
	addMenuItem(renderMenu, MI_PreviewSettings);
	addMenuItem(renderMenu, MI_Preview);
	addMenuItem(renderMenu, MI_SavePreviewedFrames);
	renderMenu->addSeparator();
	addMenuItem(renderMenu, MI_OutputSettings);
	addMenuItem(renderMenu, MI_Render);

	//---Windows Menu
	QMenu *windowsMenu = addMenu(tr("Windows"), pltEditMenuBar);
	addMenuItem(windowsMenu, MI_OpenFileBrowser);
	addMenuItem(windowsMenu, MI_OpenFileViewer);
	addMenuItem(windowsMenu, MI_OpenFilmStrip);
	addMenuItem(windowsMenu, MI_OpenPalette);
	addMenuItem(windowsMenu, MI_OpenStudioPalette);
	addMenuItem(windowsMenu, MI_OpenStyleControl);
	addMenuItem(windowsMenu, MI_OpenLevelView);
	addMenuItem(windowsMenu, MI_OpenComboViewer);
	addMenuItem(windowsMenu, MI_OpenXshView);
	windowsMenu->addSeparator();
	QMenu *otherWindowsMenu = windowsMenu->addMenu(tr("Other Windows"));
	{
		addMenuItem(otherWindowsMenu, MI_OpenBatchServers);
		addMenuItem(otherWindowsMenu, MI_OpenColorModel);
		addMenuItem(otherWindowsMenu, MI_OpenFunctionEditor);
		addMenuItem(otherWindowsMenu, MI_OpenFileBrowser2);
		addMenuItem(otherWindowsMenu, MI_OpenSchematic);
		addMenuItem(otherWindowsMenu, MI_OpenTasks);
		addMenuItem(otherWindowsMenu, MI_OpenToolbar);
		addMenuItem(otherWindowsMenu, MI_OpenToolOptionBar);
		addMenuItem(otherWindowsMenu, MI_OpenHistoryPanel);
		addMenuItem(otherWindowsMenu, MI_OpenTMessage);
	}

	//---Customize Menu
	QMenu *customizeMenu = addMenu(tr("Customize"), pltEditMenuBar);
	addMenuItem(customizeMenu, MI_Preferences);
	addMenuItem(customizeMenu, MI_ShortcutPopup);
	addMenuItem(customizeMenu, MI_SceneSettings);
	customizeMenu->addSeparator();
	QMenu *viewPartsMenu = customizeMenu->addMenu(tr("View"));
	{
		addMenuItem(viewPartsMenu, MI_ViewCamera);
		addMenuItem(viewPartsMenu, MI_ViewTable);
		addMenuItem(viewPartsMenu, MI_FieldGuide);
		addMenuItem(viewPartsMenu, MI_SafeArea);
		addMenuItem(viewPartsMenu, MI_ViewColorcard);
	}
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, MI_DockingCheck);
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, "MI_RunScript");
	addMenuItem(customizeMenu, "MI_OpenScriptConsole");
#ifndef NDEBUG
	addMenuItem(customizeMenu, "MI_ReloadStyle");
#endif

	//---Help Menu
	QMenu *helpMenu = addMenu(tr("Help"), pltEditMenuBar);
	addMenuItem(helpMenu, MI_About);

	addWidget(pltEditMenuBar);
}

//---------------------------------------------------------------------------------

void StackedMenuBar::createInknPaintMenuBar()
{
	QMenuBar *inknPaintMenuBar;
	inknPaintMenuBar = new QMenuBar(this);

	//---Files Menu
	QMenu *filesMenu = addMenu(tr("Files"), inknPaintMenuBar);
	addMenuItem(filesMenu, MI_LoadLevel);
	addMenuItem(filesMenu, MI_LoadFolder);
	addMenuItem(filesMenu, MI_SaveLevel);
	addMenuItem(filesMenu, MI_SaveLevelAs);
	addMenuItem(filesMenu, MI_OpenRecentLevel);
	addMenuItem(filesMenu, MI_ExportLevel);
	addMenuItem(filesMenu, MI_LevelSettings);
	addMenuItem(filesMenu, MI_CanvasSize);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_NewLevel);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_OverwritePalette);
	addMenuItem(filesMenu, MI_SavePaletteAs);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_LoadColorModel);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_ApplyMatchLines);
	addMenuItem(filesMenu, MI_DeleteMatchLines);
	addMenuItem(filesMenu, MI_DeleteInk);
	addMenuItem(filesMenu, MI_MergeCmapped);
	addMenuItem(filesMenu, MI_MergeColumns);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_LoadScene);
	addMenuItem(filesMenu, MI_SaveScene);
	addMenuItem(filesMenu, MI_SaveSceneAs);
	addMenuItem(filesMenu, MI_OpenRecentScene);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_NewScene);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_Quit);

	//---Tools Menu
	QMenu *toolsMenu = addMenu(tr("Tools"), inknPaintMenuBar);
	addMenuItem(toolsMenu, T_StylePicker);
	addMenuItem(toolsMenu, T_RGBPicker);
	addMenuItem(toolsMenu, T_Tape);
	toolsMenu->addSeparator();
	addMenuItem(toolsMenu, T_Fill);
	addMenuItem(toolsMenu, T_Brush);
	addMenuItem(toolsMenu, T_PaintBrush);
	addMenuItem(toolsMenu, T_Geometric);
	addMenuItem(toolsMenu, T_Type);
	toolsMenu->addSeparator();
	addMenuItem(toolsMenu, T_Eraser);
	toolsMenu->addSeparator();
	QMenu *moreToolsMenu = toolsMenu->addMenu(tr("More Tools"));
	{
		addMenuItem(moreToolsMenu, T_Edit);
		addMenuItem(moreToolsMenu, T_Selection);
		moreToolsMenu->addSeparator();
		addMenuItem(moreToolsMenu, T_ControlPointEditor);
		addMenuItem(moreToolsMenu, T_Pinch);
		addMenuItem(moreToolsMenu, T_Pump);
		addMenuItem(moreToolsMenu, T_Magnet);
		addMenuItem(moreToolsMenu, T_Bender);
		addMenuItem(moreToolsMenu, T_Iron);
		addMenuItem(moreToolsMenu, T_Cutter);
		moreToolsMenu->addSeparator();
		addMenuItem(moreToolsMenu, T_Skeleton);
		addMenuItem(moreToolsMenu, T_Tracker);
		addMenuItem(moreToolsMenu, T_Hook);
		addMenuItem(moreToolsMenu, T_Plastic);
		moreToolsMenu->addSeparator();
		addMenuItem(moreToolsMenu, T_Zoom);
		addMenuItem(moreToolsMenu, T_Rotate);
		addMenuItem(moreToolsMenu, T_Hand);
	}

	//---Draw Menu
	QMenu *drawMenu = addMenu(tr("Draw"), inknPaintMenuBar);
	addMenuItem(drawMenu, MI_ShiftTrace);
	addMenuItem(drawMenu, MI_EditShift);
	addMenuItem(drawMenu, MI_NoShift);
	addMenuItem(drawMenu, MI_ResetShift);
	drawMenu->addSeparator();
	addMenuItem(drawMenu, MI_RasterizePli);

	//---Edit Menu
	QMenu *editMenu = addMenu(tr("Edit"), inknPaintMenuBar);
	addMenuItem(editMenu, MI_Undo);
	addMenuItem(editMenu, MI_Redo);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_NextFrame);
	addMenuItem(editMenu, MI_PrevFrame);
	addMenuItem(editMenu, MI_FirstFrame);
	addMenuItem(editMenu, MI_LastFrame);
	addMenuItem(editMenu, MI_TestAnimation);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Copy);
	addMenuItem(editMenu, MI_Cut);
	addMenuItem(editMenu, MI_Paste);
	addMenuItem(editMenu, MI_PasteInto);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Clear);
	addMenuItem(editMenu, MI_Insert);
	addMenuItem(editMenu, MI_SelectAll);
	addMenuItem(editMenu, MI_InvertSelection);

	//---Checks Menu
	QMenu *checksMenu = addMenu(tr("Checks"), inknPaintMenuBar);
	addMenuItem(checksMenu, MI_TCheck);
	addMenuItem(checksMenu, MI_BCheck);
	addMenuItem(checksMenu, MI_ICheck);
	addMenuItem(checksMenu, MI_Ink1Check);
	addMenuItem(checksMenu, MI_PCheck);
	addMenuItem(checksMenu, MI_GCheck);
	addMenuItem(checksMenu, MI_ACheck);
	addMenuItem(checksMenu, MI_IOnly);

	//---Windows Menu
	QMenu *windowsMenu = addMenu(tr("Windows"), inknPaintMenuBar);
	addMenuItem(windowsMenu, MI_OpenStyleControl);
	addMenuItem(windowsMenu, MI_OpenPltGizmo);
	addMenuItem(windowsMenu, MI_OpenPalette);
	addMenuItem(windowsMenu, MI_OpenStudioPalette);
	addMenuItem(windowsMenu, MI_OpenComboViewer);
	addMenuItem(windowsMenu, MI_OpenXshView);
	addMenuItem(windowsMenu, MI_OpenColorModel);
	addMenuItem(windowsMenu, MI_OpenFileBrowser);
	addMenuItem(windowsMenu, MI_OpenFilmStrip);
	addMenuItem(windowsMenu, MI_OpenToolbar);
	addMenuItem(windowsMenu, MI_OpenToolOptionBar);
	windowsMenu->addSeparator();
	QMenu *otherWindowsMenu = windowsMenu->addMenu(tr("Other Windows"));
	{
		addMenuItem(otherWindowsMenu, MI_OpenBatchServers);
		addMenuItem(otherWindowsMenu, MI_OpenFileViewer);
		addMenuItem(otherWindowsMenu, MI_OpenFunctionEditor);
		addMenuItem(otherWindowsMenu, MI_OpenFileBrowser2);
		addMenuItem(otherWindowsMenu, MI_OpenSchematic);
		addMenuItem(otherWindowsMenu, MI_OpenTasks);
		addMenuItem(otherWindowsMenu, MI_OpenHistoryPanel);
		addMenuItem(otherWindowsMenu, MI_OpenTMessage);
	}

	//---Customize Menu
	QMenu *customizeMenu = addMenu(tr("Customize"), inknPaintMenuBar);
	addMenuItem(customizeMenu, MI_Preferences);
	addMenuItem(customizeMenu, MI_ShortcutPopup);
	addMenuItem(customizeMenu, MI_SceneSettings);
	customizeMenu->addSeparator();
	QMenu *viewPartsMenu = customizeMenu->addMenu(tr("View"));
	{
		addMenuItem(viewPartsMenu, MI_ViewCamera);
		addMenuItem(viewPartsMenu, MI_ViewTable);
		addMenuItem(viewPartsMenu, MI_FieldGuide);
		addMenuItem(viewPartsMenu, MI_SafeArea);
		addMenuItem(viewPartsMenu, MI_ViewColorcard);
	}
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, MI_DockingCheck);
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, "MI_RunScript");
	addMenuItem(customizeMenu, "MI_OpenScriptConsole");
#ifndef NDEBUG
	addMenuItem(customizeMenu, "MI_ReloadStyle");
#endif

	//---Help Menu
	QMenu *helpMenu = addMenu(tr("Help"), inknPaintMenuBar);
	addMenuItem(helpMenu, MI_About);

	addWidget(inknPaintMenuBar);
}

//---------------------------------------------------------------------------------

void StackedMenuBar::createXsheetMenuBar()
{
	QMenuBar *xsheetMenuBar;
	xsheetMenuBar = new QMenuBar(this);
	//----Xsheet Menu
	QMenu *xsheetMenu = addMenu(tr("Xsheet"), xsheetMenuBar);
	addMenuItem(xsheetMenu, MI_LoadScene);
	addMenuItem(xsheetMenu, MI_SaveScene);
	addMenuItem(xsheetMenu, MI_SaveSceneAs);
	addMenuItem(xsheetMenu, MI_OpenRecentScene);
	addMenuItem(xsheetMenu, MI_RevertScene);
	xsheetMenu->addSeparator();
	addMenuItem(xsheetMenu, MI_NewScene);
	xsheetMenu->addSeparator();
	addMenuItem(xsheetMenu, MI_PrintXsheet);
	addMenuItem(xsheetMenu, MI_Export);
	xsheetMenu->addSeparator();
	addMenuItem(xsheetMenu, MI_Quit);

	//----Subxsheet Menu
	QMenu *subxsheetMenu = addMenu(tr("Subxsheet"), xsheetMenuBar);
	addMenuItem(subxsheetMenu, MI_OpenChild);
	addMenuItem(subxsheetMenu, MI_CloseChild);
	addMenuItem(subxsheetMenu, MI_Collapse);
	addMenuItem(subxsheetMenu, MI_Resequence);
	addMenuItem(subxsheetMenu, MI_SaveSubxsheetAs);
	addMenuItem(subxsheetMenu, MI_LoadSubSceneFile);
	addMenuItem(subxsheetMenu, MI_CloneChild);
	addMenuItem(subxsheetMenu, MI_ExplodeChild);

	//----Levels Menu
	QMenu *levelsMenu = addMenu(tr("Levels"), xsheetMenuBar);
	addMenuItem(levelsMenu, MI_LoadLevel);
	addMenuItem(levelsMenu, MI_LoadFolder);
	addMenuItem(levelsMenu, MI_SaveLevel);
	addMenuItem(levelsMenu, MI_SaveLevelAs);
	addMenuItem(levelsMenu, MI_ExportLevel);
	addMenuItem(levelsMenu, MI_OpenRecentLevel);
	addMenuItem(levelsMenu, MI_LevelSettings);
	addMenuItem(levelsMenu, MI_CanvasSize);
	levelsMenu->addSeparator();
	addMenuItem(levelsMenu, MI_NewLevel);
	levelsMenu->addSeparator();
	addMenuItem(levelsMenu, MI_ViewFile);
	addMenuItem(levelsMenu, MI_FileInfo);
	levelsMenu->addSeparator();
	addMenuItem(levelsMenu, MI_CloneLevel);
	addMenuItem(levelsMenu, MI_ReplaceLevel);
	levelsMenu->addSeparator();
	addMenuItem(levelsMenu, MI_AddFrames);
	addMenuItem(levelsMenu, MI_Renumber);
	addMenuItem(levelsMenu, MI_RevertToCleanedUp);
	addMenuItem(levelsMenu, MI_ConvertToVectors);
	addMenuItem(levelsMenu, MI_ConvertFileWithInput);
	addMenuItem(levelsMenu, MI_Tracking);
	addMenuItem(levelsMenu, MI_ExposeResource);
	addMenuItem(levelsMenu, MI_EditLevel);
	addMenuItem(levelsMenu, MI_RemoveUnused);

	//----Cells Menu
	QMenu *cellsMenu = addMenu(tr("Cells"), xsheetMenuBar);
	addMenuItem(cellsMenu, MI_Dup);
	addMenuItem(cellsMenu, MI_Reverse);
	addMenuItem(cellsMenu, MI_Rollup);
	addMenuItem(cellsMenu, MI_Rolldown);
	addMenuItem(cellsMenu, MI_Swing);
	addMenuItem(cellsMenu, MI_Random);
	addMenuItem(cellsMenu, MI_TimeStretch);
	cellsMenu->addSeparator();
	QMenu *reframeSubMenu = cellsMenu->addMenu(tr("Reframe"));
	{
		addMenuItem(reframeSubMenu, MI_Reframe1);
		addMenuItem(reframeSubMenu, MI_Reframe2);
		addMenuItem(reframeSubMenu, MI_Reframe3);
		addMenuItem(reframeSubMenu, MI_Reframe4);
	}
	QMenu *stepMenu = cellsMenu->addMenu(tr("Step"));
	{
		addMenuItem(stepMenu, MI_Step2);
		addMenuItem(stepMenu, MI_Step3);
		addMenuItem(stepMenu, MI_Step4);
	}
	QMenu *eachMenu = cellsMenu->addMenu(tr("Each"));
	{
		addMenuItem(eachMenu, MI_Each2);
		addMenuItem(eachMenu, MI_Each3);
		addMenuItem(eachMenu, MI_Each4);
	}

	//----Edit Menu
	QMenu *editMenu = addMenu(tr("Edit"), xsheetMenuBar);
	addMenuItem(editMenu, MI_Undo);
	addMenuItem(editMenu, MI_Redo);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Copy);
	addMenuItem(editMenu, MI_Cut);
	addMenuItem(editMenu, MI_Paste);
	addMenuItem(editMenu, MI_PasteInto);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Clear);
	addMenuItem(editMenu, MI_Insert);
	addMenuItem(editMenu, MI_SelectAll);
	addMenuItem(editMenu, MI_InvertSelection);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_InsertSceneFrame);
	addMenuItem(editMenu, MI_RemoveSceneFrame);
	addMenuItem(editMenu, MI_InsertGlobalKeyframe);
	addMenuItem(editMenu, MI_RemoveGlobalKeyframe);

	//---Render Menu
	QMenu *renderMenu = addMenu(tr("Render"), xsheetMenuBar);
	addMenuItem(renderMenu, MI_PreviewSettings);
	addMenuItem(renderMenu, MI_Preview);
	addMenuItem(renderMenu, MI_SavePreviewedFrames);
	renderMenu->addSeparator();
	addMenuItem(renderMenu, MI_OutputSettings);
	addMenuItem(renderMenu, MI_Render);
	renderMenu->addSeparator();
	addMenuItem(renderMenu, MI_Link);

	//----Windows Menu
	QMenu *windowsMenu = addMenu(tr("Windows"), xsheetMenuBar);
	addMenuItem(windowsMenu, MI_OpenSchematic);
	addMenuItem(windowsMenu, MI_OpenComboViewer);
	addMenuItem(windowsMenu, MI_OpenFileBrowser);
	addMenuItem(windowsMenu, MI_OpenFunctionEditor);
	addMenuItem(windowsMenu, MI_OpenFileViewer);
	addMenuItem(windowsMenu, MI_OpenFilmStrip);
	addMenuItem(windowsMenu, MI_OpenLevelView);
	addMenuItem(windowsMenu, MI_OpenXshView);
	windowsMenu->addSeparator();
	QMenu *otherWindowsMenu = windowsMenu->addMenu(tr("Other Windows"));
	{
		addMenuItem(otherWindowsMenu, MI_OpenBatchServers);
		addMenuItem(otherWindowsMenu, MI_OpenColorModel);
		addMenuItem(otherWindowsMenu, MI_OpenPalette);
		addMenuItem(otherWindowsMenu, MI_OpenFileBrowser2);
		addMenuItem(otherWindowsMenu, MI_OpenStudioPalette);
		addMenuItem(otherWindowsMenu, MI_OpenStyleControl);
		addMenuItem(otherWindowsMenu, MI_OpenTasks);
		addMenuItem(otherWindowsMenu, MI_OpenToolbar);
		addMenuItem(otherWindowsMenu, MI_OpenToolOptionBar);
		addMenuItem(otherWindowsMenu, MI_OpenHistoryPanel);
		addMenuItem(otherWindowsMenu, MI_OpenTMessage);
	}

	//---Customize Menu
	QMenu *customizeMenu = addMenu(tr("Customize"), xsheetMenuBar);
	addMenuItem(customizeMenu, MI_Preferences);
	addMenuItem(customizeMenu, MI_ShortcutPopup);
	addMenuItem(customizeMenu, MI_SceneSettings);
	customizeMenu->addSeparator();
	QMenu *viewPartsMenu = customizeMenu->addMenu(tr("View"));
	{
		addMenuItem(viewPartsMenu, MI_ViewCamera);
		addMenuItem(viewPartsMenu, MI_ViewTable);
		addMenuItem(viewPartsMenu, MI_FieldGuide);
		addMenuItem(viewPartsMenu, MI_SafeArea);
		addMenuItem(viewPartsMenu, MI_ViewBBox);
		addMenuItem(viewPartsMenu, MI_ViewColorcard);
	}
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, MI_DockingCheck);
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, "MI_RunScript");
	addMenuItem(customizeMenu, "MI_OpenScriptConsole");
#ifndef NDEBUG
	addMenuItem(customizeMenu, "MI_ReloadStyle");
#endif

	//---Help Menu
	QMenu *helpMenu = addMenu(tr("Help"), xsheetMenuBar);
	addMenuItem(helpMenu, MI_About);

	addWidget(xsheetMenuBar);
}

//---------------------------------------------------------------------------------

void StackedMenuBar::createBatchesMenuBar()
{
	QMenuBar *batchesMenuBar;
	batchesMenuBar = new QMenuBar(this);

	//---Files Menu
	QMenu *filesMenu = addMenu(tr("Files"), batchesMenuBar);
	addMenuItem(filesMenu, MI_Quit);

	//----Windows Menu
	QMenu *windowsMenu = addMenu(tr("Windows"), batchesMenuBar);
	addMenuItem(windowsMenu, MI_OpenFileBrowser);
	addMenuItem(windowsMenu, MI_OpenBatchServers);
	addMenuItem(windowsMenu, MI_OpenTasks);

	//---Customize Menu
	QMenu *customizeMenu = addMenu(tr("Customize"), batchesMenuBar);
	addMenuItem(customizeMenu, MI_Preferences);
	addMenuItem(customizeMenu, MI_ShortcutPopup);
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, MI_DockingCheck);
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, "MI_RunScript");
	addMenuItem(customizeMenu, "MI_OpenScriptConsole");
#ifndef NDEBUG
	addMenuItem(customizeMenu, "MI_ReloadStyle");
#endif

	//---Help Menu
	QMenu *helpMenu = addMenu(tr("Help"), batchesMenuBar);
	addMenuItem(helpMenu, MI_About);

	addWidget(batchesMenuBar);
}

//---------------------------------------------------------------------------------

void StackedMenuBar::createBrowserMenuBar()
{
	QMenuBar *browserMenuBar;
	browserMenuBar = new QMenuBar(this);

	//---Files Menu
	QMenu *filesMenu = addMenu(tr("Files"), browserMenuBar);
	addMenuItem(filesMenu, MI_NewProject);
	addMenuItem(filesMenu, MI_ProjectSettings);
	addMenuItem(filesMenu, MI_SaveDefaultSettings);
	filesMenu->addSeparator();
	addMenuItem(filesMenu, MI_Quit);

	//---Customize Menu
	QMenu *customizeMenu = addMenu(tr("Customize"), browserMenuBar);
	addMenuItem(customizeMenu, MI_Preferences);
	addMenuItem(customizeMenu, MI_ShortcutPopup);
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, MI_DockingCheck);
	customizeMenu->addSeparator();
	addMenuItem(customizeMenu, "MI_RunScript");
	addMenuItem(customizeMenu, "MI_OpenScriptConsole");
#ifndef NDEBUG
	addMenuItem(customizeMenu, "MI_ReloadStyle");
#endif

	//iwsw commented out temporarily
	//customizeMenu->addSeparator();
	//addMenuItem(customizeMenu, "MI_ToonShadedImageToTLVByFolder");

	//---Help Menu
	QMenu *helpMenu = addMenu(tr("Help"), browserMenuBar);
	addMenuItem(helpMenu, MI_About);

	addWidget(browserMenuBar);
}

//---------------------------------------------------------------------------------

void StackedMenuBar::createFullMenuBar()
{
	QMenuBar *fullMenuBar = new QMenuBar(this);
	//Menu' FILE
	QMenu *fileMenu = addMenu(tr("File"), fullMenuBar);
	addMenuItem(fileMenu, MI_NewScene);
	addMenuItem(fileMenu, MI_LoadScene);
	addMenuItem(fileMenu, MI_SaveScene);
	addMenuItem(fileMenu, MI_SaveSceneAs);
	addMenuItem(fileMenu, MI_OpenRecentScene);
	addMenuItem(fileMenu, MI_RevertScene);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_LoadFolder);
	addMenuItem(fileMenu, MI_LoadSubSceneFile);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_NewLevel);
	addMenuItem(fileMenu, MI_LoadLevel);
	addMenuItem(fileMenu, MI_SaveLevel);
	addMenuItem(fileMenu, MI_SaveLevelAs);
	addMenuItem(fileMenu, MI_ExportLevel);
	addMenuItem(fileMenu, MI_OpenRecentLevel);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_ConvertFileWithInput);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_LoadColorModel);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_ImportMagpieFile);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_NewProject);
	addMenuItem(fileMenu, MI_ProjectSettings);
	addMenuItem(fileMenu, MI_SaveDefaultSettings);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_PreviewSettings);
	addMenuItem(fileMenu, MI_Preview);
	//addMenuItem(fileMenu, MI_SavePreview);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_OutputSettings);
	addMenuItem(fileMenu, MI_Render);
	//  addMenuItem(fileMenu, MI_SavePreviewedFrames);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_PrintXsheet);
	addMenuItem(fileMenu, MI_Print);
	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_Preferences);
	addMenuItem(fileMenu, MI_ShortcutPopup);
	fileMenu->addSeparator();
	//addMenuItem(fileMenu, MI_Export);
	//addMenuItem(fileMenu, MI_TestAnimation);
	//fileMenu->addSeparator();

	addMenuItem(fileMenu, "MI_RunScript");
	addMenuItem(fileMenu, "MI_OpenScriptConsole");

	fileMenu->addSeparator();
	addMenuItem(fileMenu, MI_Quit);

#ifndef NDEBUG
	addMenuItem(fileMenu, "MI_ReloadStyle");
#endif

	//Menu' EDIT
	QMenu *editMenu = addMenu(tr("Edit"), fullMenuBar);
	addMenuItem(editMenu, MI_Undo);
	addMenuItem(editMenu, MI_Redo);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Cut);
	addMenuItem(editMenu, MI_Copy);
	addMenuItem(editMenu, MI_Paste);
	//addMenuItem(editMenu, MI_PasteNew);
	addMenuItem(editMenu, MI_PasteInto);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Clear);
	addMenuItem(editMenu, MI_Insert);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_SelectAll);
	addMenuItem(editMenu, MI_InvertSelection);
	editMenu->addSeparator();
#ifndef XPRESS
	addMenuItem(editMenu, MI_RemoveEndpoints);
	editMenu->addSeparator();
	addMenuItem(editMenu, MI_Group);
	addMenuItem(editMenu, MI_Ungroup);
	addMenuItem(editMenu, MI_EnterGroup);
	addMenuItem(editMenu, MI_ExitGroup);
	editMenu->addSeparator();

	addMenuItem(editMenu, MI_BringToFront);
	addMenuItem(editMenu, MI_BringForward);
	addMenuItem(editMenu, MI_SendBack);
	addMenuItem(editMenu, MI_SendBackward);
#endif
//Menu' SCAN CLEANUP
#ifndef BRAVO
#ifdef LINETEST
	QMenu *scanCleanupMenu = addMenu(tr("Scan"));
#else
	QMenu *scanCleanupMenu = addMenu(tr("Scan && Cleanup"), fullMenuBar);
#endif

	addMenuItem(scanCleanupMenu, MI_DefineScanner);
	addMenuItem(scanCleanupMenu, MI_ScanSettings);
	addMenuItem(scanCleanupMenu, MI_Scan);
	addMenuItem(scanCleanupMenu, MI_SetScanCropbox);
	addMenuItem(scanCleanupMenu, MI_ResetScanCropbox);
#ifdef LINETEST
	scanCleanupMenu->addSeparator();
	addMenuItem(scanCleanupMenu, MI_Autocenter);
#endif
#ifndef LINETEST
	scanCleanupMenu->addSeparator();
	addMenuItem(scanCleanupMenu, MI_CleanupSettings);
	addMenuItem(scanCleanupMenu, MI_CleanupPreview);
	addMenuItem(scanCleanupMenu, MI_CameraTest);
	addMenuItem(scanCleanupMenu, MI_Cleanup);
	scanCleanupMenu->addSeparator();
#endif
#endif
	//Menu' LEVEL
	QMenu *levelMenu = addMenu(tr("Level"), fullMenuBar);
	addMenuItem(levelMenu, MI_AddFrames);
	addMenuItem(levelMenu, MI_Renumber);
	addMenuItem(levelMenu, MI_ReplaceLevel);
#ifndef BRAVO
	addMenuItem(levelMenu, MI_RevertToCleanedUp);
#endif
	addMenuItem(levelMenu, MI_RevertToLastSaved);
#ifndef XPRESS
	addMenuItem(levelMenu, MI_ConvertToVectors);
#endif
	addMenuItem(levelMenu, MI_Tracking);
	levelMenu->addSeparator();
	addMenuItem(levelMenu, MI_ExposeResource);
	addMenuItem(levelMenu, MI_EditLevel);
	levelMenu->addSeparator();
	addMenuItem(levelMenu, MI_LevelSettings);
	addMenuItem(levelMenu, MI_FileInfo);
	levelMenu->addSeparator();
	addMenuItem(levelMenu, MI_AdjustLevels);
	addMenuItem(levelMenu, MI_AdjustThickness);
	addMenuItem(levelMenu, MI_Antialias);
	addMenuItem(levelMenu, MI_Binarize);
	addMenuItem(levelMenu, MI_BrightnessAndContrast);
	addMenuItem(levelMenu, MI_CanvasSize);
	addMenuItem(levelMenu, MI_LinesFade);
	levelMenu->addSeparator();
	addMenuItem(levelMenu, MI_RemoveUnused);
#ifdef LINETEST
	levelMenu->addSeparator();
	addMenuItem(levelMenu, MI_Capture);
#endif

	//Menu' XSHEET
	QMenu *xsheetMenu = addMenu(tr("Xsheet"), fullMenuBar);
	addMenuItem(xsheetMenu, MI_SceneSettings);
	addMenuItem(xsheetMenu, MI_CameraSettings);
	xsheetMenu->addSeparator();
	addMenuItem(xsheetMenu, MI_OpenChild);
	addMenuItem(xsheetMenu, MI_CloseChild);
	addMenuItem(xsheetMenu, MI_SaveSubxsheetAs);
	addMenuItem(xsheetMenu, MI_Collapse);
	addMenuItem(xsheetMenu, MI_Resequence);
	addMenuItem(xsheetMenu, MI_CloneChild);
	addMenuItem(xsheetMenu, MI_ExplodeChild);
	xsheetMenu->addSeparator();
	addMenuItem(xsheetMenu, MI_ApplyMatchLines);
	addMenuItem(xsheetMenu, MI_MergeCmapped);
	xsheetMenu->addSeparator();
	addMenuItem(xsheetMenu, MI_MergeColumns);

	addMenuItem(xsheetMenu, MI_DeleteMatchLines);
	addMenuItem(xsheetMenu, MI_DeleteInk);
	xsheetMenu->addSeparator();
	addMenuItem(xsheetMenu, MI_InsertFx);
	addMenuItem(xsheetMenu, MI_NewOutputFx);
	xsheetMenu->addSeparator();
	addMenuItem(xsheetMenu, MI_InsertSceneFrame);
	addMenuItem(xsheetMenu, MI_RemoveSceneFrame);
	addMenuItem(xsheetMenu, MI_InsertGlobalKeyframe);
	addMenuItem(xsheetMenu, MI_RemoveGlobalKeyframe);

	//Menu' CELLS
	QMenu *cellsMenu = addMenu(tr("Cells"), fullMenuBar);
	addMenuItem(cellsMenu, MI_Reverse);
	addMenuItem(cellsMenu, MI_Swing);
	addMenuItem(cellsMenu, MI_Random);
	addMenuItem(cellsMenu, MI_Increment);
	addMenuItem(cellsMenu, MI_Dup);
	cellsMenu->addSeparator();
	addMenuItem(cellsMenu, MI_ResetStep);
	addMenuItem(cellsMenu, MI_IncreaseStep);
	addMenuItem(cellsMenu, MI_DecreaseStep);
	addMenuItem(cellsMenu, MI_Step2);
	addMenuItem(cellsMenu, MI_Step3);
	addMenuItem(cellsMenu, MI_Step4);
	addMenuItem(cellsMenu, MI_Each2);
	addMenuItem(cellsMenu, MI_Each3);
	addMenuItem(cellsMenu, MI_Each4);
	addMenuItem(cellsMenu, MI_Rollup);
	addMenuItem(cellsMenu, MI_Rolldown);
	addMenuItem(cellsMenu, MI_TimeStretch);
	cellsMenu->addSeparator();
	addMenuItem(cellsMenu, MI_Autorenumber);
	addMenuItem(cellsMenu, MI_Duplicate);
	addMenuItem(cellsMenu, MI_MergeFrames);
	addMenuItem(cellsMenu, MI_CloneLevel);

	//Menu' VIEW
	QMenu *viewMenu = addMenu(tr("View"), fullMenuBar);

	addMenuItem(viewMenu, MI_ViewCamera);
	addMenuItem(viewMenu, MI_ViewTable);
	addMenuItem(viewMenu, MI_FieldGuide);
	addMenuItem(viewMenu, MI_SafeArea);
	addMenuItem(viewMenu, MI_ViewBBox);
	addMenuItem(viewMenu, MI_ViewColorcard);
	addMenuItem(viewMenu, MI_ViewGuide);
	addMenuItem(viewMenu, MI_ViewRuler);
	viewMenu->addSeparator();
	addMenuItem(viewMenu, MI_TCheck);
	addMenuItem(viewMenu, MI_ICheck);
	addMenuItem(viewMenu, MI_Ink1Check);
	addMenuItem(viewMenu, MI_PCheck);
	addMenuItem(viewMenu, MI_IOnly);
	addMenuItem(viewMenu, MI_BCheck);
	addMenuItem(viewMenu, MI_GCheck);
	addMenuItem(viewMenu, MI_ACheck);
	viewMenu->addSeparator();
	addMenuItem(viewMenu, MI_ShiftTrace);
	addMenuItem(viewMenu, MI_EditShift);
	addMenuItem(viewMenu, MI_NoShift);
	addMenuItem(viewMenu, MI_ResetShift);
	viewMenu->addSeparator();
	addMenuItem(viewMenu, MI_RasterizePli);
	viewMenu->addSeparator();
	addMenuItem(viewMenu, MI_Link);
#ifdef LINETEST
	viewMenu->addSeparator();
	addMenuItem(viewMenu, MI_CapturePanelFieldGuide);
#endif

	//Menu' WINDOWS
	QMenu *windowsMenu = addMenu(tr("Windows"), fullMenuBar);
	addMenuItem(windowsMenu, MI_DockingCheck);
	windowsMenu->addSeparator();
#ifndef BRAVO
	addMenuItem(windowsMenu, MI_OpenBatchServers);
#endif
	addMenuItem(windowsMenu, MI_OpenCleanupSettings);
	addMenuItem(windowsMenu, MI_OpenColorModel);
#ifdef LINETEST
	addMenuItem(windowsMenu, MI_OpenExport);
#endif
	addMenuItem(windowsMenu, MI_OpenFileBrowser);
	addMenuItem(windowsMenu, MI_OpenFileViewer);
	addMenuItem(windowsMenu, MI_OpenFunctionEditor);
	addMenuItem(windowsMenu, MI_OpenFilmStrip);
#ifdef LINETEST
	addMenuItem(windowsMenu, MI_OpenLineTestCapture);
	addMenuItem(windowsMenu, MI_OpenLineTestView);
#endif
	addMenuItem(windowsMenu, MI_OpenPalette);
	addMenuItem(windowsMenu, MI_OpenFileBrowser2);
	addMenuItem(windowsMenu, MI_OpenSchematic);
	addMenuItem(windowsMenu, MI_OpenStudioPalette);
	addMenuItem(windowsMenu, MI_OpenStyleControl);
	addMenuItem(windowsMenu, MI_OpenTasks);
	addMenuItem(windowsMenu, MI_OpenTMessage);
	addMenuItem(windowsMenu, MI_OpenToolbar);
	addMenuItem(windowsMenu, MI_OpenToolOptionBar);
	addMenuItem(windowsMenu, MI_OpenLevelView);
	addMenuItem(windowsMenu, MI_OpenComboViewer);
	addMenuItem(windowsMenu, MI_OpenXshView);
	addMenuItem(windowsMenu, MI_OpenHistoryPanel);
	windowsMenu->addSeparator();
	addMenuItem(windowsMenu, MI_ResetRoomLayout);

	//---Help Menu
	QMenu *helpMenu = addMenu(tr("Help"), fullMenuBar);
	addMenuItem(helpMenu, MI_About);

	addWidget(fullMenuBar);
}

//-----------------------------------------------------------------------------

QMenu *StackedMenuBar::addMenu(const QString &menuName, QMenuBar *menuBar)
{
	QMenu *menu = new QMenu(menuName, menuBar);
	menuBar->addMenu(menu);
	return menu;
}

//-----------------------------------------------------------------------------

void StackedMenuBar::addMenuItem(QMenu *menu, const char *cmdId)
{
	QAction *action = CommandManager::instance()->getAction(cmdId);
	if (!action)
		return;
	assert(action); // check MainWindow::defineActions() if assert fails
	menu->addAction(action);
}

//=============================================================================
// DvTopBar
//-----------------------------------------------------------------------------

TopBar::TopBar(QWidget *parent)
	: QToolBar(parent)
{
	setAllowedAreas(Qt::TopToolBarArea);
	setMovable(false);
	setFloatable(false);
	setObjectName("TopBar");

	m_containerFrame = new QFrame(this);
	m_roomTabBar = new QTabBar(this);
	m_stackedMenuBar = new StackedMenuBar(this);

	m_containerFrame->setObjectName("TopBarTabContainer");
	m_roomTabBar->setObjectName("TopBarTab");
	m_roomTabBar->setDrawBase(false);

	QHBoxLayout *mainLayout = new QHBoxLayout();
	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);
	{
		QVBoxLayout *menuLayout = new QVBoxLayout();
		menuLayout->setSpacing(0);
		menuLayout->setMargin(0);
		{
			menuLayout->addStretch(1);
			menuLayout->addWidget(m_stackedMenuBar, 0);
			menuLayout->addStretch(1);
		}
		mainLayout->addLayout(menuLayout);
		mainLayout->addStretch(1);
		mainLayout->addWidget(m_roomTabBar, 0);
	}
	m_containerFrame->setLayout(mainLayout);
	addWidget(m_containerFrame);

	connect(m_roomTabBar, SIGNAL(currentChanged(int)),
			m_stackedMenuBar, SLOT(setCurrentIndex(int)));
}