

// Tnz6 includes
#include "tapp.h"
#include "mainwindow.h"
#include "formatsettingspopups.h"
#include "fileviewerpopup.h"
#include "flipbook.h"
#include "filebrowsermodel.h"
#include "previewfxmanager.h"

// TnzQt includes
#include "toonzqt/menubarcommand.h"
#include "toonzqt/dvdialog.h"
#include "toonzqt/gutil.h"

// TnzLib includes
#include "toonz/preferences.h"
#include "toonz/toonzscene.h"
#include "toonz/tscenehandle.h"
#include "toonz/txsheet.h"
#include "toonz/txsheethandle.h"
#include "toonz/fxdag.h"
#include "toonz/tcolumnhandle.h"
#include "toonz/tcamera.h"
#include "toonz/sceneproperties.h"
#include "toonz/onionskinmask.h"
#include "toonz/observer.h"
#include "toonz/scenefx.h"
#include "toonz/movierenderer.h"
#include "toonz/multimediarenderer.h"
#include "toutputproperties.h"

#ifdef _WIN32
#include "avicodecrestrictions.h"
#endif

// TnzBase includes
#include "tenv.h"
#include "trenderer.h"
#include "trasterfx.h"

// TnzCore includes
#include "tsystem.h"
#include "tiio.h"
#include "trop.h"
#include "tconvert.h"
#include "tfiletype.h"
#include "tflash.h"
#include "timagecache.h"
#include "tthreadmessage.h"

// Qt includes
#include <QObject>
#include <QDesktopServices>
#include <QUrl>

//---------------------------------------------------------

namespace
{

#include "bravomark.h"

TRaster32P loadLight()
{
	static TRaster32P ras(137, 48);
	static bool filled = false;

	if (filled)
		return ras;

	filled = true;

	ras->lock();

	UINT *pix = (UINT *)ras->getRawData();
	int count = 0;
	for (UINT i = 0; i < 5888; i += 2)
		for (UINT j = 0; j < lightmark137x48[i]; j++, pix++) {
			*pix = lightmark137x48[i + 1];
			if (*pix != 0) {
				TPixel32 *col = (TPixel32 *)pix;
				col->r = troundp(0.85 * col->r);
				col->g = troundp(0.85 * col->g);
				col->b = troundp(0.85 * col->b);
				col->m = troundp(0.85 * col->m);
			}
			count++;
		}

	ras->unlock();

	assert(count == 137 * 48);

	return ras;
}

#ifdef BRAVODEMO

//---------------------------------------------------------

TRaster32P loadBravo(const TDimension &res)
{
	static TRaster32P ras(160, 60);
	static TDimension Res;
	static TRaster32P rasOut;
	static bool filled = false;

	if (!filled) {
		filled = true;
		ras->lock();
		UINT *pix = (UINT *)ras->getRawData();

		for (UINT i = 0; i < 5216; i += 2)
			for (UINT j = 0; j < bravoMark160x60[i]; j++)
				*pix++ = bravoMark160x60[i + 1];

		ras->unlock();
	}

	if (Res != res) {
		rasOut = TRaster32P();
		int outLx, outLy;

		if (res.ly < res.lx) {
			outLy = troundp(res.ly * 60.0 / 576.0);
			outLx = troundp(outLy * 160. / 60.0);
		} else {
			outLx = troundp(res.lx * 160.0 / 768.0);
			outLy = troundp(outLx * 60.0 / 160.0);
		}

		if (outLx == 160 && outLy == 60)
			rasOut = ras;
		else {
			rasOut = TRaster32P(outLx, outLy);
			TRop::resample(rasOut, ras, TScale(outLx / 160.0, outLy / 60.0).place(ras->getCenterD(), rasOut->getCenterD()));
		}
		Res = res;
	}

	return rasOut;

#ifdef CREA_MARK
	TRasterP ras1;
	TImageReader::load(TFilePath("C:\\bravo52\\watermark.tif"), ras1);
	UINT *pix1 = (UINT *)ras1->getRawData();
	UINT *pixm = pix1;
	int counter = 0;
	int totcount = 0;
	int tmpcount = 0;
	int tt = 0;
	Tofstream os(TFilePath("c:\\test.txt"));
	for (UINT i = 0; i < ras1->getLx(); i++)
		for (UINT j = 0; j < ras1->getLy(); j++) {
			if (*pix1 == *pixm)
				counter++;
			else {

				os << counter << ", " << *pixm << ", ";
				*pixm = *pix1;
				tmpcount += counter;
				counter = 1;
				tt++;
				int azz = tt % 10;
				if (!(tt % 10))
					os << "\n";
			}
			*pix1++;
			totcount++;
		}
	tmpcount += counter;
	os << counter << ", " << *pixm << ", ";
#endif
	return ras;
}
#endif
} //namespace

//---------------------------------------------------------

//=========================================================

class OnRenderCompleted : public TThread::Message
{
	TFilePath m_fp;
	bool m_error;

public:
	OnRenderCompleted(const TFilePath &fp, bool error) : m_fp(fp), m_error(error) {}

	TThread::Message *clone() const { return new OnRenderCompleted(*this); }

	void onDeliver()
	{
		if (m_error) {
			m_error = false;
			MsgBox(DVGui::CRITICAL, QObject::tr("There was an error saving frames for the %1 level.").arg(QString::fromStdWString(m_fp.withoutParentDir().getWideString())));
		}

		bool isPreview = (m_fp.getType() == "noext");

		TImageCache::instance()->remove(toString(m_fp.getWideString() + L".0"));
		TNotifier::instance()->notify(TSceneNameChange());

		if (Preferences::instance()->isGeneratedMovieViewEnabled()) {
			if (!isPreview && (Preferences::instance()->isDefaultViewerEnabled()) &&
				(m_fp.getType() == "mov" || m_fp.getType() == "avi" || m_fp.getType() == "3gp")) {
				QString name = QString::fromStdString(m_fp.getName());
				int index;
				if ((index = name.indexOf("#RENDERID")) != -1) //!quite ugly I know....
					m_fp = m_fp.withName(name.left(index).toStdWString());

				if (!TSystem::showDocument(m_fp)) {
					QString msg(QObject::tr("It is not possible to display the file %1: no player associated with its format").arg(QString::fromStdWString(m_fp.withoutParentDir().getWideString())));
					MsgBox(WARNING, msg);
				}

			}

			else {
				int r0, r1, step;
				TApp *app = TApp::instance();
				ToonzScene *scene = app->getCurrentScene()->getScene();
				TOutputProperties &outputSettings = isPreview ? *scene->getProperties()->getPreviewProperties() : *scene->getProperties()->getOutputProperties();
				outputSettings.getRange(r0, r1, step);
				const TRenderSettings rs = outputSettings.getRenderSettings();
				if (r0 == 0 && r1 == -1)
					r0 = 0, r1 = scene->getFrameCount() - 1;

				double timeStretchFactor = isPreview ? 1.0 : (double)outputSettings.getRenderSettings().m_timeStretchTo /
																 outputSettings.getRenderSettings().m_timeStretchFrom;

				r0 = tfloor(r0 * timeStretchFactor);
				r1 = tceil((r1 + 1) * timeStretchFactor) - 1;

				TXsheet::SoundProperties *prop = new TXsheet::SoundProperties();
				prop->m_frameRate = outputSettings.getFrameRate();
				TSoundTrack *snd = app->getCurrentXsheet()->getXsheet()->makeSound(prop);
				if (outputSettings.getRenderSettings().m_stereoscopic) {
					assert(!isPreview);
					::viewFile(m_fp.withName(m_fp.getName() + "_l"), r0 + 1, r1 + 1, step, isPreview ? rs.m_shrinkX : 1, snd, 0, false, true);
					::viewFile(m_fp.withName(m_fp.getName() + "_r"), r0 + 1, r1 + 1, step, isPreview ? rs.m_shrinkX : 1, snd, 0, false, true);
				} else
					::viewFile(m_fp, r0 + 1, r1 + 1, step, isPreview ? rs.m_shrinkX : 1, snd, 0, false, true);
			}
		}
	}
};

//=========================================================

class RenderCommand
{
	int m_r0, m_r1, m_step;
	TFilePath m_fp;
	int m_numFrames;
	TPixel32 m_oldBgColor;
	TDimension m_oldCameraRes;
	double m_r, m_stepd;
	double m_timeStretchFactor;

	int m_multimediaRender;

public:
	RenderCommand()
		: m_r0(0), m_r1(-1), m_step(1), m_numFrames(0), m_r(0), m_stepd(1), m_oldCameraRes(0, 0), m_timeStretchFactor(1), m_multimediaRender(0)
	{
		setCommandHandler("MI_Render", this, &RenderCommand::onRender);
		setCommandHandler("MI_Preview", this, &RenderCommand::onPreview);
	}

	bool init(bool isPreview); // true if r0<=r1
	void flashRender();
	void rasterRender(bool isPreview);
	void multimediaRender();
	void onRender();
	void onPreview();
	void doRender(bool isPreview);
};

RenderCommand renderCommand;

//---------------------------------------------------------

bool RenderCommand::init(bool isPreview)
{
	ToonzScene *scene = TApp::instance()->getCurrentScene()->getScene();
	TSceneProperties *sprop = scene->getProperties();
	/*-- Preview/Renderに応じてそれぞれのSettingを取得 --*/
	TOutputProperties &outputSettings = isPreview ? *sprop->getPreviewProperties() : *sprop->getOutputProperties();
	outputSettings.getRange(m_r0, m_r1, m_step);
	/*-- シーン全体のレンダリングの場合、m_r1をScene長に設定 --*/
	if (m_r0 == 0 && m_r1 == -1) {
		m_r0 = 0;
		m_r1 = scene->getFrameCount() - 1;
	}
	if (m_r0 < 0)
		m_r0 = 0;
	if (m_r1 >= scene->getFrameCount())
		m_r1 = scene->getFrameCount() - 1;
	if (m_r1 < m_r0) {
		MsgBox(WARNING, QObject::tr("The command cannot be executed because the scene is empty."));
		return false;
		// throw TException("empty scene");
		// non so perche', ma termina il programma
		// nonostante il try all'inizio
	}

	// Initialize the preview case

	/*TRenderSettings rs = sprop->getPreviewProperties()->getRenderSettings();
  TRenderSettings rso = sprop->getOutputProperties()->getRenderSettings();
  rs.m_stereoscopic=true;
  rs.m_stereoscopicShift=0.05;
  rso.m_stereoscopic=true;
  rso.m_stereoscopicShift=0.05;
  sprop->getPreviewProperties()->setRenderSettings(rs);
  sprop->getOutputProperties()->setRenderSettings(rso);*/

	if (isPreview) {
		/*-- PreviewではTimeStretchを考慮しないので、そのままフレーム値を格納してゆく --*/
		m_numFrames = (int)(m_r1 - m_r0 + 1);
		m_r = m_r0;
		m_stepd = m_step;
		m_multimediaRender = 0;
		return true;
	}

	// Full render case

	// Read the output filepath
	TFilePath fp = outputSettings.getPath();
	/*-- ファイル名が指定されていない場合は、シーン名を出力ファイル名にする --*/
	if (fp.getWideName() == L"")
		fp = fp.withName(scene->getScenePath().getName());
	/*-- ラスタ画像の場合、ファイル名にフレーム番号を追加 --*/
	if (TFileType::getInfo(fp) == TFileType::RASTER_IMAGE || fp.getType() == "pct" || fp.getType() == "pic" || fp.getType() == "pict") //pct e' un formato"livello" (ha i settings di quicktime) ma fatto di diversi frames
		fp = fp.withFrame(TFrameId::EMPTY_FRAME);
	fp = scene->decodeFilePath(fp);
	if (!TFileStatus(fp.getParentDir()).doesExist()) {
		try {
			TFilePath parent = fp.getParentDir();
			TSystem::mkDir(parent);
			DvDirModel::instance()->refreshFolder(parent.getParentDir());
		} catch (TException &e) {
			MsgBox(WARNING, QObject::tr("It is not possible to create folder : %1").arg(QString::fromStdString(toString(e.getMessage()))));
			return false;
		} catch (...) {
			MsgBox(WARNING, QObject::tr("It is not possible to create a folder."));
			return false;
		}
	}
	m_fp = fp;

	// Retrieve camera infos
	const TCamera *camera = isPreview ? scene->getCurrentPreviewCamera() : scene->getCurrentCamera();
	TDimension cameraSize = camera->getRes();
	TPointD cameraDpi = camera->getDpi();

	// Retrieve render interval/step/times
	double stretchTo = (double)outputSettings.getRenderSettings().m_timeStretchTo;
	double stretchFrom = (double)outputSettings.getRenderSettings().m_timeStretchFrom;

	m_timeStretchFactor = stretchTo / stretchFrom;
	m_stepd = m_step / m_timeStretchFactor;

	int stretchedR0 = tfloor(m_r0 * m_timeStretchFactor);
	int stretchedR1 = tceil((m_r1 + 1) * m_timeStretchFactor) - 1;

	m_r = stretchedR0 / m_timeStretchFactor;
	m_numFrames = (stretchedR1 - stretchedR0) / m_step + 1;

	// Update the multimedia render switch
	m_multimediaRender = outputSettings.getMultimediaRendering();

	return true;
}

//===================================================================

void RenderCommand::flashRender()
{
	ToonzScene *scene = TApp::instance()->getCurrentScene()->getScene();
	TSceneProperties *sprop = scene->getProperties();
	FILE *fileP = fopen(m_fp, "wb");
	if (!fileP)
		return;
	ProgressDialog pb("rendering " + toQString(m_fp), "Cancel", 0, m_numFrames);
	pb.show();

	TDimension cameraSize = scene->getCurrentCamera()->getRes();
	double frameRate = sprop->getOutputProperties()->getFrameRate();
	TFlash flash(
		cameraSize.lx,
		cameraSize.ly,
		m_numFrames,
		frameRate,
		sprop->getOutputProperties()->getFileFormatProperties("swf"));
	flash.setBackgroundColor(sprop->getBgColor());

	std::vector<TXshSoundColumn *> columns;
	scene->getSoundColumns(columns);
	if (!columns.empty()) {
		TXsheet::SoundProperties *prop = new TXsheet::SoundProperties();
		prop->m_frameRate = frameRate;
		TSoundTrack *st = scene->getXsheet()->makeSound(prop);
		if (st)
			flash.putSound(st, 0);
	}

	int i = 0;
	for (i = 0; i < m_numFrames; ++i, m_r += m_stepd) {
		flash.beginFrame(m_step * i + 1);
		TRasterFxP rfx = buildSceneFx(scene, m_r, 0, false);
		assert(rfx);
		rfx->compute(flash, tround(m_r)); // WARNING: This should accept a DOUBLE...
#ifdef BRAVODEMO
		TRasterImageP ri(loadBravo(scene->getCurrentCamera()->getRes()));
		int lx = ri->getRaster()->getLx();
		int ly = ri->getRaster()->getLx();
		flash.pushMatrix();
		int dx = tround(0.1 * (cameraSize.lx - lx));
		int dy = tround(0.1 * (cameraSize.ly - ly));
		flash.multMatrix(TTranslation((cameraSize.lx - lx) / 2 - (dx > 0 ? dx : 0), -(cameraSize.ly - ly) / 2 + (dy > 0 ? dy : 0)));
		flash.draw(ri, 0);
		flash.popMatrix();
#endif
		flash.endFrame(i == m_numFrames - 1, 0, true);
		if (pb.wasCanceled())
			break;
		pb.setValue(i + 1);
	}

	flash.writeMovie(fileP);
	fclose(fileP);

	TSystem::showDocument(m_fp);
	//QDesktopServices::openUrl(QUrl(toQString(m_fp)));

	TImageCache::instance()->remove(toString(m_fp.getWideString() + L".0"));
	TNotifier::instance()->notify(TSceneNameChange());
}

//===================================================================

class RenderListener
	: public ProgressDialog,
	  public MovieRenderer::Listener
{
	QString m_progressBarString;
	int m_frameCounter;
	TRenderer *m_renderer;
	bool m_error;

	class Message : public TThread::Message
	{
		RenderListener *m_pb;
		int m_frame;
		QString m_labelText;

	public:
		Message(RenderListener *pb, int frame, const QString &labelText) : TThread::Message(), m_pb(pb), m_frame(frame), m_labelText(labelText) {}
		TThread::Message *clone() const { return new Message(*this); }
		void onDeliver()
		{
			if (m_frame == -1)
				m_pb->hide();
			else {
				m_pb->setLabelText("Rendering frame " + QString::number(m_frame) + "/" + m_labelText);
				m_pb->setValue(m_frame);
			}
		}
	};

public:
	RenderListener(TRenderer *renderer, const TFilePath &path, int steps, bool isPreview)
		: ProgressDialog("Precomputing " + QString::number(steps) + " Frames" + ((isPreview) ? "" : " of " + toQString(path)), "Cancel", 0, steps, TApp::instance()->getMainWindow()), m_renderer(renderer), m_frameCounter(0), m_error(false)
	{
#ifdef MACOSX
		//Modal dialogs seem to be preventing the execution of Qt::BlockingQueuedConnections on MAC...!
		setModal(false);
#endif
		setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
		m_progressBarString = QString::number(steps) + ((isPreview) ? "" : " of " + toQString(path));
		//setMinimumDuration (0);
		show();
	}

	/*-- 以下３つの関数はMovieRenderer::Listenerの純粋仮想関数の実装 --*/
	bool onFrameCompleted(int frame)
	{
		bool ret = wasCanceled();
		Message(this, ret ? -1 : ++m_frameCounter, m_progressBarString).send();
		return !ret;
	}
	bool onFrameFailed(int frame, TException &)
	{
		m_error = true;
		return onFrameCompleted(frame);
	}
	void onSequenceCompleted(const TFilePath &fp)
	{
		Message(this, -1, "").send();
		OnRenderCompleted(fp, m_error).send();
		m_error = false;
	}

	void onCancel()
	{
		m_isCanceled = true;
		setLabelText("Aborting render...");
		reset();
		hide();
	}
};

//---------------------------------------------------------

void RenderCommand::rasterRender(bool isPreview)
{
	ToonzScene *scene = TApp::instance()->getCurrentScene()->getScene();

	if (isPreview) {
		//Let the PreviewFxManager own the rest. Just pass him the current output node.
		PreviewFxManager::instance()->showNewPreview((TFx *)scene->getXsheet()->getFxDag()->getCurrentOutputFx());
		return;
	}

	string ext = m_fp.getType();

#ifdef _WIN32
	if (ext == "avi" && !isPreview) {
		TPropertyGroup *props = scene->getProperties()->getOutputProperties()->getFileFormatProperties(ext);
		string codecName = props->getProperty(0)->getValueAsString();
		TDimension res = scene->getCurrentCamera()->getRes();
		if (!AviCodecRestrictions::canWriteMovie(toWideString(codecName), res)) {
			QString msg(QObject::tr("The resolution of the output camera does not fit with the options chosen for the output file format."));
			MsgBox(WARNING, msg);
			return;
		}
	}
#endif;

	//Extract output properties
	TOutputProperties *prop = isPreview ? scene->getProperties()->getPreviewProperties() : scene->getProperties()->getOutputProperties();

	//Build thread count
	/*-- Dedicated CPUs のコンボボックス (Single, Half, All) --*/
	int index = prop->getThreadIndex();

	const int procCount = TSystem::getProcessorCount();
	const int threadCounts[3] = {1, procCount / 2, procCount};

	int threadCount = threadCounts[index];

	/*-- MovieRendererを作る。Previewの場合はファイルパスは空 --*/
	MovieRenderer movieRenderer(scene, isPreview ? TFilePath() : m_fp, threadCount, isPreview);

	TRenderSettings rs = prop->getRenderSettings();

	//Build raster granularity size
	index = prop->getMaxTileSizeIndex();

	const int maxTileSizes[4] = {
		(std::numeric_limits<int>::max)(),
		TOutputProperties::LargeVal,
		TOutputProperties::MediumVal,
		TOutputProperties::SmallVal};
	rs.m_maxTileSize = maxTileSizes[index];

//Build

#ifdef BRAVODEMO
	rs.m_mark = loadBravo(scene->getCurrentCamera()->getRes());
#endif
	/*-- RenderSettingsをセット --*/
	movieRenderer.setRenderSettings(rs);
	/*-- カメラDPIの取得、セット --*/
	TPointD cameraDpi = isPreview ? scene->getCurrentPreviewCamera()->getDpi() : scene->getCurrentCamera()->getDpi();
	movieRenderer.setDpi(cameraDpi.x, cameraDpi.y);
	movieRenderer.enablePrecomputing(true);

	/*-- プログレス ダイアログの作成 --*/
	RenderListener *listener =
		new RenderListener(movieRenderer.getTRenderer(), m_fp, ((m_numFrames - 1) / m_step) + 1, isPreview);
	QObject::connect(listener, SIGNAL(canceled()), &movieRenderer, SLOT(onCanceled()));
	movieRenderer.addListener(listener);

	bool fieldRendering = rs.m_fieldPrevalence != TRenderSettings::NoField;

	/*-- buildSceneFxの進行状況を表示するプログレスバー --*/
	QProgressBar *buildSceneProgressBar = new QProgressBar(TApp::instance()->getMainWindow());
	buildSceneProgressBar->setAttribute(Qt::WA_DeleteOnClose);
	buildSceneProgressBar->setWindowFlags(Qt::SubWindow | Qt::Dialog | Qt::WindowStaysOnTopHint);
	buildSceneProgressBar->setMinimum(0);
	buildSceneProgressBar->setMaximum(m_numFrames - 1);
	buildSceneProgressBar->setValue(0);
	buildSceneProgressBar->move(600, 500);
	buildSceneProgressBar->setWindowTitle("Building Schematic...");
	buildSceneProgressBar->show();

	for (int i = 0; i < m_numFrames; ++i, m_r += m_stepd) {
		buildSceneProgressBar->setValue(i);

		if (rs.m_stereoscopic)
			scene->shiftCameraX(-rs.m_stereoscopicShift / 2);
		TFxPair fx;
		fx.m_frameA = buildSceneFx(scene, m_r, rs.m_shrinkX, isPreview);

		if (fieldRendering && !isPreview)
			fx.m_frameB = buildSceneFx(scene, m_r + 0.5 / m_timeStretchFactor, rs.m_shrinkX, isPreview);
		else if (rs.m_stereoscopic) {
			scene->shiftCameraX(rs.m_stereoscopicShift);
			fx.m_frameB = buildSceneFx(scene, m_r + 0.5 / m_timeStretchFactor, rs.m_shrinkX, isPreview);
			scene->shiftCameraX(-rs.m_stereoscopicShift / 2);
		} else
			fx.m_frameB = TRasterFxP();
		/*-- movieRendererにフレーム毎のFxを登録 --*/
		movieRenderer.addFrame(m_r, fx);
	}
	/*-- プログレスバーを閉じる --*/
	buildSceneProgressBar->close();

	//resetViewer(); //TODO cancella le immagini dell'eventuale render precedente
	//FileViewerPopupPool::instance()->getCurrent()->onClose();

	movieRenderer.start();
}

//===================================================================

class MultimediaProgressBar
	: public ProgressDialog,
	  public MultimediaRenderer::Listener
{
	QString m_progressBarString;
	int m_frameCounter;
	int m_columnCounter;
	int m_pbCounter;
	MultimediaRenderer *m_renderer;

	//----------------------------------------------------------

	class Message : public TThread::Message
	{
		MultimediaProgressBar *m_pb;
		int m_frame;
		int m_column;
		int m_pbValue;
		QString m_labelText;

	public:
		Message(MultimediaProgressBar *pb, int frame, int column, int pbValue, const QString &labelText)
			: TThread::Message(), m_pb(pb), m_frame(frame), m_column(column), m_pbValue(pbValue), m_labelText(labelText) {}

		TThread::Message *clone() const { return new Message(*this); }

		void onDeliver()
		{
			if (m_pbValue == -1)
				m_pb->hide();
			else {
				QString modeStr(m_pb->m_renderer->getMultimediaMode() == MultimediaRenderer::COLUMNS ? "column " : "layer ");
				m_pb->setLabelText(
					"Rendering " + modeStr + QString::number(m_column) +
					", frame " + QString::number(m_frame) +
					"/" + m_labelText);
				m_pb->setValue(m_pbValue);
			}
		}
	};

	//----------------------------------------------------------

public:
	MultimediaProgressBar(MultimediaRenderer *renderer)
		: ProgressDialog(
			  "Rendering " + QString::number(renderer->getFrameCount()) +
				  " frames " + " of " + toQString(renderer->getFilePath()),
			  "Cancel", 0, renderer->getFrameCount() * renderer->getColumnsCount()),
		  m_renderer(renderer), m_frameCounter(0), m_columnCounter(0), m_pbCounter(0)
	{
#ifdef MACOSX
		//Modal dialogs seem to be preventing the execution of Qt::BlockingQueuedConnections on MAC...!
		setModal(false);
#endif
		setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
		m_progressBarString =
			QString::number(m_renderer->getFrameCount()) + " of " + toQString(m_renderer->getFilePath());
		show();
	}

	MultimediaRenderer *getRenderer() { return m_renderer; }

	bool onFrameCompleted(int frame, int column)
	{
		bool ret = wasCanceled();
		Message(this, ++m_frameCounter, m_columnCounter, ret ? -1 : ++m_pbCounter, m_progressBarString).send();
		return !ret;
	}

	bool onFrameFailed(int frame, int column, TException &)
	{
		return onFrameCompleted(frame, column);
	}

	void onSequenceCompleted(int column)
	{
		m_frameCounter = 0;
		Message(this, m_frameCounter, ++m_columnCounter, m_pbCounter, m_progressBarString).send();
		//OnRenderCompleted(fp).send();   //No output should have been put in cache - and no viewfile to be called...
	}

	virtual void onRenderCompleted()
	{
		Message(this, -1, -1, -1, "").send();
		//OnRenderCompleted(fp).send();
	}

	void onCancel()
	{
		m_isCanceled = true;
		setLabelText("Aborting render...");
		TRenderer *trenderer(m_renderer->getTRenderer());
		if (m_renderer)
			trenderer->stopRendering(true);
		reset();
		hide();
	}
};

//---------------------------------------------------------

//!Specialized render invocation for multimedia rendering. Flash rendering
//!is currently not supported in this mode.
void RenderCommand::multimediaRender()
{
	ToonzScene *scene = TApp::instance()->getCurrentScene()->getScene();
	string ext = m_fp.getType();

#ifdef _WIN32
	if (ext == "avi") {
		TPropertyGroup *props = scene->getProperties()->getOutputProperties()->getFileFormatProperties(ext);
		string codecName = props->getProperty(0)->getValueAsString();
		TDimension res = scene->getCurrentCamera()->getRes();
		if (!AviCodecRestrictions::canWriteMovie(toWideString(codecName), res)) {
			QString msg(QObject::tr("The resolution of the output camera does not fit with the options chosen for the output file format."));
			MsgBox(WARNING, msg);
			return;
		}
	}
#endif;

	TOutputProperties *prop = scene->getProperties()->getOutputProperties();

	//Build thread count
	int index = prop->getThreadIndex();

	const int procCount = TSystem::getProcessorCount();
	const int threadCounts[3] = {1, procCount / 2, procCount};

	int threadCount = threadCounts[index];

	//Build raster granularity size
	index = prop->getMaxTileSizeIndex();

	const int maxTileSizes[4] = {
		(std::numeric_limits<int>::max)(),
		TOutputProperties::LargeVal,
		TOutputProperties::MediumVal,
		TOutputProperties::SmallVal};

	TRenderSettings rs = prop->getRenderSettings();
	rs.m_maxTileSize = maxTileSizes[index];

	MultimediaRenderer multimediaRenderer(scene, m_fp, prop->getMultimediaRendering(), threadCount);
	multimediaRenderer.setRenderSettings(rs);

#ifdef BRAVODEMO
	rs.m_mark = loadBravo(scene->getCurrentCamera()->getRes());
#endif

	TPointD cameraDpi = scene->getCurrentCamera()->getDpi();
	multimediaRenderer.setDpi(cameraDpi.x, cameraDpi.y);
	multimediaRenderer.enablePrecomputing(true);

	for (int i = 0; i < m_numFrames; ++i, m_r += m_stepd)
		multimediaRenderer.addFrame(m_r);

	MultimediaProgressBar *listener =
		new MultimediaProgressBar(&multimediaRenderer);
	QObject::connect(listener, SIGNAL(canceled()), &multimediaRenderer, SLOT(onCanceled()));
	multimediaRenderer.addListener(listener);

	multimediaRenderer.start();
}

//===================================================================

void RenderCommand::onRender()
{
	doRender(false);
}

void RenderCommand::onPreview()
{
	doRender(true);
}

//---------------------------------------------------------

void RenderCommand::doRender(bool isPreview)
{
	bool isWritable = true;
	bool isMultiFrame;
	/*-- 初期化処理。フレーム範囲の計算や、Renderの場合はOutputSettingsから保存先パスも作る --*/
	if (!init(isPreview))
		return;
	if (m_fp.getDots() == ".") {
		isMultiFrame = false;
		TFileStatus fs(m_fp);
		if (fs.doesExist())
			isWritable = fs.isWritable();
	} else {
		isMultiFrame = true;
		TFilePath dir = m_fp.getParentDir();
		QDir qDir(QString::fromStdWString(dir.getWideString()));
		QString levelName = QRegExp::escape(QString::fromStdWString(m_fp.getWideName()));
		QString levelType = QString::fromStdString(m_fp.getType());
		QString exp(levelName + ".[0-9]{1,4}." + levelType);
		QRegExp regExp(exp);
		QStringList list = qDir.entryList(QDir::Files);
		QStringList livelFrames = list.filter(regExp);

		int i;
		for (i = 0; i < livelFrames.size() && isWritable; i++) {
			TFilePath frame = dir + TFilePath(livelFrames[i].toStdWString());
			if (frame.isEmpty() || !frame.isAbsolute())
				continue;
			TFileStatus fs(frame);
			isWritable = fs.isWritable();
		}
	}
	if (!isWritable) {
		string str = "It is not possible to write the output:  the file";
		str += isMultiFrame ? "s are read only." : " is read only.";
		MsgBox(WARNING, QString::fromStdString(str));
		return;
	}

	ToonzScene *scene = 0;
	TCamera *camera = 0;

	try {
		/*-- Xsheetノードに繋がっている各ラインごとに計算するモード。
			MultipleRender で Schematic Flows または Fx Schematic Terminal Nodes が選択されている場合
		--*/
		if (m_multimediaRender && m_fp.getType() != "swf") //swf is not currently supported on multimedia...
			multimediaRender();
		else if (!isPreview && m_fp.getType() == "swf")
			flashRender();
		else
			/*-- 通常のRendering --*/
			rasterRender(isPreview);
	} catch (TException &e) {
		MsgBox(WARNING, QString::fromStdString(toString(e.getMessage())));
	} catch (...) {
		MsgBox(WARNING, QObject::tr("It is not possible to complete the rendering."));
	}
}

//===================================================================
